#include "stackreduce.h"
#include "visitor.h"
#include "context.h"
#include "vector.h"

typedef struct schemerlicht_stack_reduce_visitor
  {
  schemerlicht_visitor* visitor;
  schemerlicht_vector estimated_vars_on_the_stack;
  int max_vars_on_stack_allowed;
  } schemerlicht_stack_reduce_visitor;

static void postvisit_let_binding(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_let_binding* e)
  {
  schemerlicht_stack_reduce_visitor* vis = (schemerlicht_stack_reduce_visitor*)(v->impl);
  int* vars = schemerlicht_vector_back(&vis->estimated_vars_on_the_stack, int);
  *vars += 1;
  UNUSED(ctxt);    
  UNUSED(e);
  }

static void postvisit_let(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_stack_reduce_visitor* vis = (schemerlicht_stack_reduce_visitor*)(v->impl);
  int* vars = schemerlicht_vector_back(&vis->estimated_vars_on_the_stack, int);
  *vars -= e->expr.let.bindings.vector_size;
  UNUSED(ctxt);
  }

static int previsit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_stack_reduce_visitor* vis = (schemerlicht_stack_reduce_visitor*)(v->impl);
  schemerlicht_vector_push_back(ctxt, &vis->estimated_vars_on_the_stack, e->expr.lambda.variables.vector_size+2, int); // +2 for self and continuation
  return 1;
  }

static void postvisit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_stack_reduce_visitor* vis = (schemerlicht_stack_reduce_visitor*)(v->impl);
  schemerlicht_vector_pop_back(&vis->estimated_vars_on_the_stack);
  UNUSED(ctxt);
  UNUSED(e);
  }

// this is actuall previsit_primcall, but we cannot change the type of the expression in previsit_primcall,
// therefore we use previsit_expression to check for a prim call and then we can possibly change the prim call
// to another expression type.
static int previsit_expression(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  if (e->type == schemerlicht_type_primitive_call)
    {
    schemerlicht_stack_reduce_visitor* vis = (schemerlicht_stack_reduce_visitor*)(v->impl);
    int* vars = schemerlicht_vector_back(&vis->estimated_vars_on_the_stack, int);
    *vars += e->expr.prim.arguments.vector_size;
    if (e->expr.prim.as_object == 0 && (*vars > vis->max_vars_on_stack_allowed)) // if the stack is too full
      { // then change the prim call to a fun call with the prim as object. cps will make sure the stack is cleared.
      schemerlicht_expression funcall = schemerlicht_init_funcall(ctxt);
      e->expr.prim.as_object = 1;
      schemerlicht_vector tmp = funcall.expr.funcall.arguments;
      funcall.expr.funcall.arguments = e->expr.prim.arguments;
      e->expr.prim.arguments = tmp;
      schemerlicht_vector_push_back(ctxt, &funcall.expr.funcall.fun, *e, schemerlicht_expression);
      *e = funcall;
      }    
    }  
  return 1;
  }

static void postvisit_primcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_stack_reduce_visitor* vis = (schemerlicht_stack_reduce_visitor*)(v->impl);
  int* vars = schemerlicht_vector_back(&vis->estimated_vars_on_the_stack, int);
  *vars -= e->expr.prim.arguments.vector_size;
  UNUSED(ctxt);
  }

static int previsit_foreigncall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_stack_reduce_visitor* vis = (schemerlicht_stack_reduce_visitor*)(v->impl);
  int* vars = schemerlicht_vector_back(&vis->estimated_vars_on_the_stack, int);
  *vars += e->expr.foreign.arguments.vector_size;
  UNUSED(ctxt);
  UNUSED(e);
  return 1;
  }

static void postvisit_foreigncall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_stack_reduce_visitor* vis = (schemerlicht_stack_reduce_visitor*)(v->impl);
  int* vars = schemerlicht_vector_back(&vis->estimated_vars_on_the_stack, int);
  *vars -= e->expr.foreign.arguments.vector_size;
  UNUSED(ctxt);
  }

static int previsit_funcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_stack_reduce_visitor* vis = (schemerlicht_stack_reduce_visitor*)(v->impl);
  int* vars = schemerlicht_vector_back(&vis->estimated_vars_on_the_stack, int);
  *vars = e->expr.funcall.arguments.vector_size + 1; // cps will clear the stack
  UNUSED(ctxt);
  UNUSED(e);
  return 1;
  }

static void postvisit_funcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_stack_reduce_visitor* vis = (schemerlicht_stack_reduce_visitor*)(v->impl);
  int* vars = schemerlicht_vector_back(&vis->estimated_vars_on_the_stack, int);
  *vars = 0;
  UNUSED(ctxt);
  UNUSED(e);
  }

static schemerlicht_stack_reduce_visitor* schemerlicht_stack_reduce_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_stack_reduce_visitor* v = schemerlicht_new(ctxt, schemerlicht_stack_reduce_visitor);
  v->max_vars_on_stack_allowed = schemerlicht_maxstack*8/10;
  schemerlicht_vector_init(ctxt, &v->estimated_vars_on_the_stack, int);
  schemerlicht_vector_push_back(ctxt, &v->estimated_vars_on_the_stack, 0, int);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->postvisit_let_binding = postvisit_let_binding;
  v->visitor->postvisit_let = postvisit_let;
  v->visitor->previsit_lambda = previsit_lambda;
  v->visitor->postvisit_lambda = postvisit_lambda;
  v->visitor->previsit_expression = previsit_expression;
  v->visitor->postvisit_primcall = postvisit_primcall;
  v->visitor->previsit_funcall = previsit_funcall;
  v->visitor->postvisit_funcall = postvisit_funcall;
  v->visitor->previsit_foreigncall = previsit_foreigncall;
  v->visitor->postvisit_foreigncall = postvisit_foreigncall;
  return v;
  }

static void schemerlicht_stack_reduce_visitor_free(schemerlicht_context* ctxt, schemerlicht_stack_reduce_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_vector_destroy(ctxt, &v->estimated_vars_on_the_stack);
    schemerlicht_delete(ctxt, v);
    }
  }

void schemerlicht_stack_reduce_conversion(schemerlicht_context* ctxt, schemerlicht_program* program, int max_vars_on_stack_allowed)
  {
  schemerlicht_stack_reduce_visitor* v = schemerlicht_stack_reduce_visitor_new(ctxt);
  v->max_vars_on_stack_allowed = max_vars_on_stack_allowed;
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_stack_reduce_visitor_free(ctxt, v);
  }