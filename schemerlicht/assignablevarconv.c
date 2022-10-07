#include "assignablevarconv.h"
#include "visitor.h"
#include "context.h"
#include "vector.h"
#include "map.h"
#include "stringvec.h"

typedef struct schemerlicht_find_assignable_variables_visitor
  {
  schemerlicht_visitor* visitor;
  schemerlicht_vector assignable_variables;
  } schemerlicht_find_assignable_variables_visitor;

static int previsit_let(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_vector assignable_vars;
  schemerlicht_vector_init(ctxt, &assignable_vars, schemerlicht_string);
  schemerlicht_find_assignable_variables_visitor* vis = (schemerlicht_find_assignable_variables_visitor*)(v->impl);
  schemerlicht_vector_push_back(ctxt, &vis->assignable_variables, assignable_vars, schemerlicht_vector);
  UNUSED(e);
  return 1;
  }

static void postvisit_let(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_find_assignable_variables_visitor* vis = (schemerlicht_find_assignable_variables_visitor*)(v->impl);
  schemerlicht_vector* vec = schemerlicht_vector_back(&vis->assignable_variables, schemerlicht_vector);
  swap(e->expr.let.assignable_variables, *vec, schemerlicht_vector);
  schemerlicht_vector_destroy(ctxt, vec);
  schemerlicht_vector_pop_back(&vis->assignable_variables);
  vec = schemerlicht_vector_back(&vis->assignable_variables, schemerlicht_vector);
  schemerlicht_string* it = schemerlicht_vector_begin(&e->expr.let.assignable_variables, schemerlicht_string);
  schemerlicht_string* it_end = schemerlicht_vector_end(&e->expr.let.assignable_variables, schemerlicht_string);
  for (; it != it_end; ++it)
    {
    schemerlicht_string s;
    schemerlicht_string_copy(ctxt, &s, it);
    schemerlicht_string_vector_insert_sorted(ctxt, vec, &s);
    }
  }

static int previsit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_vector assignable_vars;
  schemerlicht_vector_init(ctxt, &assignable_vars, schemerlicht_string);
  schemerlicht_find_assignable_variables_visitor* vis = (schemerlicht_find_assignable_variables_visitor*)(v->impl);
  schemerlicht_vector_push_back(ctxt, &vis->assignable_variables, assignable_vars, schemerlicht_vector);
  UNUSED(e);
  return 1;
  }

static void postvisit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_find_assignable_variables_visitor* vis = (schemerlicht_find_assignable_variables_visitor*)(v->impl);
  schemerlicht_vector* vec = schemerlicht_vector_back(&vis->assignable_variables, schemerlicht_vector);
  swap(e->expr.lambda.assignable_variables, *vec, schemerlicht_vector);
  schemerlicht_vector_destroy(ctxt, vec);
  schemerlicht_vector_pop_back(&vis->assignable_variables);
  vec = schemerlicht_vector_back(&vis->assignable_variables, schemerlicht_vector);
  schemerlicht_string* it = schemerlicht_vector_begin(&e->expr.lambda.assignable_variables, schemerlicht_string);
  schemerlicht_string* it_end = schemerlicht_vector_end(&e->expr.lambda.assignable_variables, schemerlicht_string);
  for (; it != it_end; ++it)
    {
    schemerlicht_string s;
    schemerlicht_string_copy(ctxt, &s, it);
    schemerlicht_string_vector_insert_sorted(ctxt, vec, &s);
    }
  }

static int previsit_set(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_find_assignable_variables_visitor* vis = (schemerlicht_find_assignable_variables_visitor*)(v->impl);
  schemerlicht_vector* vec = schemerlicht_vector_back(&vis->assignable_variables, schemerlicht_vector);
  schemerlicht_string s;
  schemerlicht_string_copy(ctxt, &s, &e->expr.set.name);
  schemerlicht_string_vector_insert_sorted(ctxt, vec, &s);
  return 1;
  }

static schemerlicht_find_assignable_variables_visitor* schemerlicht_find_assignable_variables_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_find_assignable_variables_visitor* v = schemerlicht_new(ctxt, schemerlicht_find_assignable_variables_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->previsit_let = previsit_let;
  v->visitor->postvisit_let = postvisit_let;
  v->visitor->previsit_lambda = previsit_lambda;
  v->visitor->postvisit_lambda = postvisit_lambda;
  v->visitor->previsit_set = previsit_set;
  schemerlicht_vector_init(ctxt, &v->assignable_variables, schemerlicht_vector);
  schemerlicht_vector assignable_vars;
  schemerlicht_vector_init(ctxt, &assignable_vars, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v->assignable_variables, assignable_vars, schemerlicht_vector);
  return v;
  }

static void schemerlicht_find_assignable_variables_visitor_free(schemerlicht_context* ctxt, schemerlicht_find_assignable_variables_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_vector* vec = schemerlicht_vector_at(&v->assignable_variables, 0, schemerlicht_vector);
    schemerlicht_string* it = schemerlicht_vector_begin(vec, schemerlicht_string);
    schemerlicht_string* it_end = schemerlicht_vector_end(vec, schemerlicht_string);
    for (; it != it_end; ++it)
      schemerlicht_string_destroy(ctxt, it);

    schemerlicht_vector_destroy(ctxt, vec);
    schemerlicht_vector_destroy(ctxt, &v->assignable_variables);
    schemerlicht_delete(ctxt, v);
    }
  }

void schemerlicht_find_assignable_variables(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_find_assignable_variables_visitor* v = schemerlicht_find_assignable_variables_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_find_assignable_variables_visitor_free(ctxt, v);
  }

void schemerlicht_assignable_variable_conversion(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_find_assignable_variables_visitor* v = schemerlicht_find_assignable_variables_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_find_assignable_variables_visitor_free(ctxt, v);
  }