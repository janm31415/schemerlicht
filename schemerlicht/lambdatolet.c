#include "lambdatolet.h"
#include "visitor.h"
#include "context.h"
#include "vector.h"

typedef struct schemerlicht_lambda_to_let_visitor
  {
  schemerlicht_visitor* visitor;

  } schemerlicht_lambda_to_let_visitor;

static void _convert(schemerlicht_context* ctxt, schemerlicht_expression* e)
  {
  schemerlicht_expression* lambda = schemerlicht_vector_at(&e->expr.funcall.fun, 0, schemerlicht_expression);
  schemerlicht_assert(lambda->type == schemerlicht_type_lambda);
  schemerlicht_assert(lambda->expr.lambda.variable_arity == 0);
  schemerlicht_assert(e->expr.funcall.arguments.vector_size == lambda->expr.lambda.variables.vector_size);
  schemerlicht_expression let = schemerlicht_init_let(ctxt);
  schemerlicht_vector_push_back(ctxt, &let.expr.let.body, *schemerlicht_vector_at(&lambda->expr.lambda.body, 0, schemerlicht_expression), schemerlicht_expression);
  for (schemerlicht_memsize i = 0; i < lambda->expr.lambda.variables.vector_size; ++i)
    {
    schemerlicht_let_binding bind;
    bind.binding_name = *schemerlicht_vector_at(&lambda->expr.lambda.variables, i, schemerlicht_string);
    bind.binding_expr = *schemerlicht_vector_at(&e->expr.funcall.arguments, i, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &let.expr.let.bindings, bind, schemerlicht_let_binding);
    }
  schemerlicht_string_destroy(ctxt, &e->expr.funcall.filename);
  schemerlicht_vector_destroy(ctxt, &e->expr.funcall.arguments);
  schemerlicht_string_destroy(ctxt, &lambda->expr.lambda.filename);
  schemerlicht_vector_destroy(ctxt, &lambda->expr.lambda.assignable_variables);
  schemerlicht_vector_destroy(ctxt, &lambda->expr.lambda.free_variables);
  schemerlicht_vector_destroy(ctxt, &lambda->expr.lambda.variables);
  schemerlicht_vector_destroy(ctxt, &lambda->expr.lambda.body);
  schemerlicht_vector_destroy(ctxt, &e->expr.funcall.fun);
  *e = let;
  }

static void postvisit_funcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_expression* fun = schemerlicht_vector_at(&e->expr.funcall.fun, 0, schemerlicht_expression);
  if (fun->type == schemerlicht_type_lambda)
    {
    if (fun->expr.lambda.variable_arity == 0)
      _convert(ctxt, e);
    }
  }

static schemerlicht_lambda_to_let_visitor* schemerlicht_lambda_to_let_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_lambda_to_let_visitor* v = schemerlicht_new(ctxt, schemerlicht_lambda_to_let_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->postvisit_funcall = postvisit_funcall;
  return v;
  }

static void schemerlicht_lambda_to_let_visitor_free(schemerlicht_context* ctxt, schemerlicht_lambda_to_let_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }

void schemerlicht_lambda_to_let_conversion(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_lambda_to_let_visitor* v = schemerlicht_lambda_to_let_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_lambda_to_let_visitor_free(ctxt, v);
  }