#include "freevaranalysis.h"
#include "visitor.h"
#include "context.h"
#include "vector.h"
#include "stringvec.h"
#include "environment.h"

typedef struct schemerlicht_free_variable_analysis_visitor
  {
  schemerlicht_visitor* visitor;
  schemerlicht_vector local_variables;
  schemerlicht_vector free_variables;
  } schemerlicht_free_variable_analysis_visitor;

static void visit_variable(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_free_variable_analysis_visitor* vis = (schemerlicht_free_variable_analysis_visitor*)(v->impl);
  schemerlicht_vector* loc_vars = schemerlicht_vector_back(&vis->local_variables, schemerlicht_vector);
  int is_local = schemerlicht_string_vector_binary_search(loc_vars, &e->expr.var.name);
  if (is_local == 0)
    {
    schemerlicht_environment_entry entry;
    int is_global = schemerlicht_environment_find(&entry, ctxt, &e->expr.var.name);
    if (is_global == 0)
      {
      schemerlicht_vector* free_vars = schemerlicht_vector_back(&vis->free_variables, schemerlicht_vector);
      schemerlicht_string_vector_insert_sorted(ctxt, free_vars, &e->expr.var.name);
      }
    }
  }

static int previsit_set(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_free_variable_analysis_visitor* vis = (schemerlicht_free_variable_analysis_visitor*)(v->impl);
  schemerlicht_vector* loc_vars = schemerlicht_vector_back(&vis->local_variables, schemerlicht_vector);
  int is_local = schemerlicht_string_vector_binary_search(loc_vars, &e->expr.set.name);
  if (is_local==0)
    {
    schemerlicht_environment_entry entry;
    int is_global = schemerlicht_environment_find(&entry, ctxt, &e->expr.set.name);
    if (is_global == 0)
      {
      schemerlicht_vector* free_vars = schemerlicht_vector_back(&vis->free_variables, schemerlicht_vector);
      schemerlicht_string_vector_insert_sorted(ctxt, free_vars, &e->expr.set.name);
      }
    }
  return 1;
  }

static void postvisit_let_bindings(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_free_variable_analysis_visitor* vis = (schemerlicht_free_variable_analysis_visitor*)(v->impl);
  schemerlicht_vector free_vars, local_vars;
  schemerlicht_vector_init(ctxt, &free_vars, schemerlicht_string);
  schemerlicht_vector_init(ctxt, &local_vars, schemerlicht_string);

  schemerlicht_let_binding* it = schemerlicht_vector_begin(&e->expr.let.bindings, schemerlicht_let_binding);
  schemerlicht_let_binding* it_end = schemerlicht_vector_end(&e->expr.let.bindings, schemerlicht_let_binding);
  for (; it != it_end; ++it)
    {
    schemerlicht_string_vector_insert_sorted(ctxt, &local_vars, &it->binding_name);
    }

  schemerlicht_vector_push_back(ctxt, &vis->free_variables, free_vars, schemerlicht_vector);
  schemerlicht_vector_push_back(ctxt, &vis->local_variables, local_vars, schemerlicht_vector);
  }

static void postvisit_let(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_free_variable_analysis_visitor* vis = (schemerlicht_free_variable_analysis_visitor*)(v->impl);
  schemerlicht_vector* loc_vars = schemerlicht_vector_back(&vis->local_variables, schemerlicht_vector);
  schemerlicht_vector* free_vars = schemerlicht_vector_back(&vis->free_variables, schemerlicht_vector);
  schemerlicht_vector* parent_loc_vars = schemerlicht_vector_at(&vis->local_variables, vis->local_variables.vector_size - 2, schemerlicht_vector);
  schemerlicht_vector* parent_free_vars = schemerlicht_vector_at(&vis->free_variables, vis->free_variables.vector_size - 2, schemerlicht_vector);
  schemerlicht_string* it = schemerlicht_vector_begin(free_vars, schemerlicht_string);
  schemerlicht_string* it_end = schemerlicht_vector_end(free_vars, schemerlicht_string);
  for (; it != it_end; ++it)
    {    
    int is_local = schemerlicht_string_vector_binary_search(parent_loc_vars, it);
    if (!is_local)
      {
      schemerlicht_string_vector_insert_sorted(ctxt, parent_free_vars, it);
      }
    }

  schemerlicht_vector_destroy(ctxt, loc_vars);
  schemerlicht_vector_destroy(ctxt, free_vars);
  schemerlicht_vector_pop_back(&vis->free_variables);
  schemerlicht_vector_pop_back(&vis->local_variables);
  }

static int previsit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_free_variable_analysis_visitor* vis = (schemerlicht_free_variable_analysis_visitor*)(v->impl);
  schemerlicht_vector free_vars, local_vars;
  schemerlicht_vector_init(ctxt, &free_vars, schemerlicht_string);
  schemerlicht_vector_init(ctxt, &local_vars, schemerlicht_string);
  
  schemerlicht_string* it = schemerlicht_vector_begin(&e->expr.lambda.variables, schemerlicht_string);
  schemerlicht_string* it_end = schemerlicht_vector_end(&e->expr.lambda.variables, schemerlicht_string);
  for (; it != it_end; ++it)
    {
    schemerlicht_string_vector_insert_sorted(ctxt, &local_vars, it);
    }

  schemerlicht_vector_push_back(ctxt, &vis->free_variables, free_vars, schemerlicht_vector);
  schemerlicht_vector_push_back(ctxt, &vis->local_variables, local_vars, schemerlicht_vector);
  return 1;
  }

static void postvisit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_free_variable_analysis_visitor* vis = (schemerlicht_free_variable_analysis_visitor*)(v->impl);
  schemerlicht_vector* loc_vars = schemerlicht_vector_back(&vis->local_variables, schemerlicht_vector);
  schemerlicht_vector* free_vars = schemerlicht_vector_back(&vis->free_variables, schemerlicht_vector);
  schemerlicht_vector* parent_loc_vars = schemerlicht_vector_at(&vis->local_variables, vis->local_variables.vector_size-2, schemerlicht_vector);
  schemerlicht_vector* parent_free_vars = schemerlicht_vector_at(&vis->free_variables, vis->free_variables.vector_size-2, schemerlicht_vector);
  schemerlicht_string* it = schemerlicht_vector_begin(free_vars, schemerlicht_string);
  schemerlicht_string* it_end = schemerlicht_vector_end(free_vars, schemerlicht_string);
  for (; it != it_end; ++it)
    {
    schemerlicht_string free_var;
    schemerlicht_string_copy(ctxt, &free_var, it);
    schemerlicht_string_vector_insert_sorted(ctxt, &e->expr.lambda.free_variables, &free_var);
    int is_local = schemerlicht_string_vector_binary_search(parent_loc_vars, it);
    if (!is_local)
      {
      schemerlicht_string_vector_insert_sorted(ctxt, parent_free_vars, it);
      }
    }

  schemerlicht_vector_destroy(ctxt, loc_vars);
  schemerlicht_vector_destroy(ctxt, free_vars);
  schemerlicht_vector_pop_back(&vis->free_variables);
  schemerlicht_vector_pop_back(&vis->local_variables);
  }
static schemerlicht_free_variable_analysis_visitor* schemerlicht_free_variable_analysis_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_free_variable_analysis_visitor* v = schemerlicht_new(ctxt, schemerlicht_free_variable_analysis_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->visit_variable = visit_variable;
  v->visitor->previsit_set = previsit_set;
  v->visitor->postvisit_let_bindings = postvisit_let_bindings;
  v->visitor->postvisit_let = postvisit_let;
  v->visitor->previsit_lambda = previsit_lambda;
  v->visitor->postvisit_lambda = postvisit_lambda;
  schemerlicht_vector_init(ctxt, &v->free_variables, schemerlicht_vector);
  schemerlicht_vector_init(ctxt, & v->local_variables, schemerlicht_vector);
  schemerlicht_vector free_vars, local_vars;
  schemerlicht_vector_init(ctxt, &free_vars, schemerlicht_string);
  schemerlicht_vector_init(ctxt, &local_vars, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v->free_variables, free_vars, schemerlicht_vector);
  schemerlicht_vector_push_back(ctxt, &v->local_variables, local_vars, schemerlicht_vector);
  return v;
  }

static void schemerlicht_free_variable_analysis_visitor_free(schemerlicht_context* ctxt, schemerlicht_free_variable_analysis_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);    
    schemerlicht_vector* it = schemerlicht_vector_begin(&v->free_variables, schemerlicht_vector);
    schemerlicht_vector* it_end = schemerlicht_vector_end(&v->free_variables, schemerlicht_vector);
    for (; it != it_end; ++it)
      {
      schemerlicht_vector_destroy(ctxt, it);
      }
    it = schemerlicht_vector_begin(&v->local_variables, schemerlicht_vector);
    it_end = schemerlicht_vector_end(&v->local_variables, schemerlicht_vector);
    for (; it != it_end; ++it)
      {
      schemerlicht_vector_destroy(ctxt, it);
      }
    schemerlicht_vector_destroy(ctxt, &v->free_variables);
    schemerlicht_vector_destroy(ctxt, &v->local_variables);
    schemerlicht_delete(ctxt, v);
    }
  }

void schemerlicht_free_variable_analysis(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_free_variable_analysis_visitor* v = schemerlicht_free_variable_analysis_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_free_variable_analysis_visitor_free(ctxt, v);
  }