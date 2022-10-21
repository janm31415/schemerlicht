#include "closure.h"
#include "visitor.h"
#include "context.h"
#include "string.h"
#include "stringvec.h"

#include <stdio.h>
#include <string.h>

typedef struct schemerlicht_resolve_free_variables_visitor
  {
  schemerlicht_visitor* visitor;  
  schemerlicht_vector active_lambda;
  } schemerlicht_resolve_free_variables_visitor;

static int previsit_lambda_resolve_free_variables(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_resolve_free_variables_visitor* vis = (schemerlicht_resolve_free_variables_visitor*)(v->impl);
  schemerlicht_vector_push_back(ctxt, &vis->active_lambda, *e, schemerlicht_expression);
  return 1;
  }

static void postvisit_lambda_resolve_free_variables(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_resolve_free_variables_visitor* vis = (schemerlicht_resolve_free_variables_visitor*)(v->impl);
  schemerlicht_vector_pop_back(&vis->active_lambda);
  UNUSED(ctxt);
  UNUSED(e);
  }

static void visit_variable_resolve_free_variables(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_resolve_free_variables_visitor* vis = (schemerlicht_resolve_free_variables_visitor*)(v->impl);
  if (vis->active_lambda.vector_size == 0)
    return;
  schemerlicht_expression* last_lambda = schemerlicht_vector_back(&vis->active_lambda, schemerlicht_expression);
  schemerlicht_string* sit = schemerlicht_vector_begin(&last_lambda->expr.lambda.free_variables, schemerlicht_string);
  schemerlicht_string* sit_end = schemerlicht_vector_end(&last_lambda->expr.lambda.free_variables, schemerlicht_string);
  schemerlicht_string* find_free_var_it = schemerlicht_lower_bound(sit, sit_end, &e->expr.var.name);
  int contains_var = (!(find_free_var_it == sit_end) && !(schemerlicht_string_compare_less(&e->expr.var.name, find_free_var_it))) ? 1 : 0;
  if (contains_var == 1)
    {
    schemerlicht_expression prim = schemerlicht_init_primcall(ctxt);
    schemerlicht_string_init(ctxt, &prim.expr.prim.name, "closure-ref");
    schemerlicht_expression var = schemerlicht_init_variable(ctxt);
    schemerlicht_string* first_var = schemerlicht_vector_at(&last_lambda->expr.lambda.variables, 0, schemerlicht_string);
    schemerlicht_string_copy(ctxt, &var.expr.var.name, first_var);
    schemerlicht_vector_push_back(ctxt, &prim.expr.prim.arguments, var, schemerlicht_expression);
    schemerlicht_expression fx = schemerlicht_init_fixnum(ctxt);
    fx.expr.lit.lit.fx.value = cast(schemerlicht_fixnum, find_free_var_it - sit) + 1;
    schemerlicht_vector_push_back(ctxt, &prim.expr.prim.arguments, fx, schemerlicht_expression);
    schemerlicht_string_destroy(ctxt, &e->expr.var.filename);
    schemerlicht_string_destroy(ctxt, &e->expr.var.name);
    *e = prim;
    }
  }

static schemerlicht_resolve_free_variables_visitor* schemerlicht_resolve_free_variables_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_resolve_free_variables_visitor* v = schemerlicht_new(ctxt, schemerlicht_resolve_free_variables_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->previsit_lambda = previsit_lambda_resolve_free_variables;
  v->visitor->postvisit_lambda = postvisit_lambda_resolve_free_variables;
  v->visitor->visit_variable = visit_variable_resolve_free_variables;
  schemerlicht_vector_init(ctxt, &v->active_lambda, schemerlicht_expression);
  return v;
  }

static void schemerlicht_resolve_free_variables_visitor_free(schemerlicht_context* ctxt, schemerlicht_resolve_free_variables_visitor* v)
  {
  if (v)
    {
    schemerlicht_vector_destroy(ctxt, &v->active_lambda);
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }

typedef struct schemerlicht_closure_conversion_visitor
  {
  schemerlicht_visitor* visitor;
  schemerlicht_memsize self_index;
  } schemerlicht_closure_conversion_visitor;

static void postvisit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_closure_conversion_visitor* vis = (schemerlicht_closure_conversion_visitor*)(v->impl);
  schemerlicht_string name;
  schemerlicht_string_init(ctxt, &name, "#%self");
  char number[256];
  sprintf(number, "%d", vis->self_index);
  ++vis->self_index;
  schemerlicht_string_append_cstr(ctxt, &name, number);
  schemerlicht_string* insert_it = schemerlicht_vector_at(&e->expr.lambda.variables, 0, schemerlicht_string);
  schemerlicht_vector_insert_element(ctxt, &e->expr.lambda.variables, &insert_it, name, schemerlicht_string);

  schemerlicht_expression prim = schemerlicht_init_primcall(ctxt);
  schemerlicht_string_init(ctxt, &prim.expr.prim.name, "closure");
  schemerlicht_vector_push_back(ctxt, &prim.expr.prim.arguments, *e, schemerlicht_expression);

  schemerlicht_string* it = schemerlicht_vector_begin(&e->expr.lambda.free_variables, schemerlicht_string);
  schemerlicht_string* it_end = schemerlicht_vector_end(&e->expr.lambda.free_variables, schemerlicht_string);
  for (; it != it_end; ++it)
    {
    schemerlicht_expression var = schemerlicht_init_variable(ctxt);
    schemerlicht_string_copy(ctxt, &var.expr.var.name, it);
    schemerlicht_vector_push_back(ctxt, &prim.expr.prim.arguments, var, schemerlicht_expression);
    }
  *e = prim;
  }

static schemerlicht_closure_conversion_visitor* schemerlicht_closure_conversion_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_closure_conversion_visitor* v = schemerlicht_new(ctxt, schemerlicht_closure_conversion_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->self_index = 0;
  v->visitor->postvisit_lambda = postvisit_lambda;
  return v;
  }

static void schemerlicht_closure_conversion_visitor_free(schemerlicht_context* ctxt, schemerlicht_closure_conversion_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }

void schemerlicht_closure_conversion(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_closure_conversion_visitor* v = schemerlicht_closure_conversion_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_closure_conversion_visitor_free(ctxt, v);

  schemerlicht_resolve_free_variables_visitor* v2 = schemerlicht_resolve_free_variables_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v2->visitor, program);
  schemerlicht_resolve_free_variables_visitor_free(ctxt, v2);
  }