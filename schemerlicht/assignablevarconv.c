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


typedef struct schemerlicht_convert_assignable_variables_visitor
  {
  schemerlicht_visitor* visitor;
  schemerlicht_vector assignable_variables;
  } schemerlicht_convert_assignable_variables_visitor;

static int is_assignable_variable(schemerlicht_visitor* v, schemerlicht_string* s)
  {
  schemerlicht_convert_assignable_variables_visitor* vis = (schemerlicht_convert_assignable_variables_visitor*)(v->impl);

  schemerlicht_vector* first = schemerlicht_vector_begin(&vis->assignable_variables, schemerlicht_vector);
  schemerlicht_vector* last = schemerlicht_vector_end(&vis->assignable_variables, schemerlicht_vector);
  while (last != first)
    {
    --last;
    if (schemerlicht_string_vector_binary_search(last, s))
      return 1;
    }
  return 0;
  }

static int previsit_let_assvarconv(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_vector assignable_vars;
  schemerlicht_vector_init(ctxt, &assignable_vars, schemerlicht_string);  

  schemerlicht_vector new_bindings;
  schemerlicht_vector_init(ctxt, &new_bindings, schemerlicht_let_binding);


  schemerlicht_let_binding* it = schemerlicht_vector_begin(&e->expr.let.bindings, schemerlicht_let_binding);
  schemerlicht_let_binding* it_end = schemerlicht_vector_end(&e->expr.let.bindings, schemerlicht_let_binding);

  for (; it != it_end; ++it)
    {
    if (schemerlicht_string_vector_binary_search(&e->expr.let.assignable_variables, &it->binding_name))
      {
      schemerlicht_vector_push_back(ctxt, &assignable_vars, it->binding_name, schemerlicht_string);
      schemerlicht_string adapted;
      schemerlicht_string_init(ctxt, &adapted, "#%");
      schemerlicht_string_append(ctxt, &adapted, &it->binding_name);
      schemerlicht_expression var = schemerlicht_init_variable(ctxt);
      var.expr.var.name = adapted;
      schemerlicht_expression prim = schemerlicht_init_primcall(ctxt);
      schemerlicht_string_init(ctxt, &prim.expr.prim.name, "vector");
      schemerlicht_vector_push_back(ctxt, &prim.expr.prim.arguments, var, schemerlicht_expression);
      schemerlicht_let_binding new_binding;
      new_binding.binding_expr = prim;
      schemerlicht_string_copy(ctxt, &new_binding.binding_name, &it->binding_name);
      schemerlicht_vector_push_back(ctxt, &new_bindings, new_binding, schemerlicht_let_binding);
      schemerlicht_string_copy(ctxt, &it->binding_name, &adapted);
      }
    }
  if (new_bindings.vector_size == 0)
    {
    schemerlicht_vector_destroy(ctxt, &new_bindings);
    }
  else
    {
    schemerlicht_expression new_let = schemerlicht_init_let(ctxt);
    swap(new_let.expr.let.bindings, new_bindings, schemerlicht_vector);
    swap(new_let.expr.let.body, e->expr.let.body, schemerlicht_vector);
    schemerlicht_vector_destroy(ctxt, &new_bindings);
    schemerlicht_expression new_begin = schemerlicht_init_begin(ctxt);
    schemerlicht_vector_push_back(ctxt, &new_begin.expr.beg.arguments, new_let, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &e->expr.let.body, new_begin, schemerlicht_expression);
    }

  schemerlicht_convert_assignable_variables_visitor* vis = (schemerlicht_convert_assignable_variables_visitor*)(v->impl);
  schemerlicht_vector_push_back(ctxt, &vis->assignable_variables, assignable_vars, schemerlicht_vector);

  return 1;
  }

static void postvisit_let_assvarconv(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_convert_assignable_variables_visitor* vis = (schemerlicht_convert_assignable_variables_visitor*)(v->impl);
  schemerlicht_vector* vec = schemerlicht_vector_back(&vis->assignable_variables, schemerlicht_vector);
  schemerlicht_string* it = schemerlicht_vector_begin(vec, schemerlicht_string);
  schemerlicht_string* it_end = schemerlicht_vector_end(vec, schemerlicht_string);
  for (; it != it_end; ++it)
    schemerlicht_string_destroy(ctxt, it);
  schemerlicht_vector_destroy(ctxt, vec);
  schemerlicht_vector_pop_back(&vis->assignable_variables);
  
  }

static int previsit_lambda_assvarconv(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_vector assignable_vars;
  schemerlicht_vector_init(ctxt, &assignable_vars, schemerlicht_string);
  schemerlicht_convert_assignable_variables_visitor* vis = (schemerlicht_convert_assignable_variables_visitor*)(v->impl);
  schemerlicht_vector_push_back(ctxt, &vis->assignable_variables, assignable_vars, schemerlicht_vector);
  UNUSED(e);
  return 1;
  }

static void postvisit_lambda_assvarconv(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_convert_assignable_variables_visitor* vis = (schemerlicht_convert_assignable_variables_visitor*)(v->impl);
  schemerlicht_vector* vec = schemerlicht_vector_back(&vis->assignable_variables, schemerlicht_vector);
  schemerlicht_string* it = schemerlicht_vector_begin(vec, schemerlicht_string);
  schemerlicht_string* it_end = schemerlicht_vector_end(vec, schemerlicht_string);
  for (; it != it_end; ++it)
    schemerlicht_string_destroy(ctxt, it);
  schemerlicht_vector_destroy(ctxt, vec);
  schemerlicht_vector_pop_back(&vis->assignable_variables);
  }

static void postvisit_set_assvarconv(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  if (e->expr.set.originates_from_define || e->expr.set.originates_from_quote)
    return;
  //schemerlicht_convert_assignable_variables_visitor* vis = (schemerlicht_convert_assignable_variables_visitor*)(v->impl);
  //schemerlicht_vector* vec = schemerlicht_vector_back(&vis->assignable_variables, schemerlicht_vector);
  //if (schemerlicht_string_vector_binary_search(vec, &e->expr.set.name))
  if (is_assignable_variable(v, &e->expr.set.name))
    {
    schemerlicht_expression prim = schemerlicht_init_primcall(ctxt);
    schemerlicht_string_init(ctxt, &prim.expr.prim.name, "vector-set!");
   //schemerlicht_vector_push_back(ctxt, &prim.expr.prim.arguments, *e, schemerlicht_expression);
    schemerlicht_expression var = schemerlicht_init_variable(ctxt);
    var.expr.var.name = e->expr.set.name;
    schemerlicht_vector_push_back(ctxt, &prim.expr.prim.arguments, var, schemerlicht_expression);
    schemerlicht_expression fx = schemerlicht_init_fixnum(ctxt);
    schemerlicht_vector_push_back(ctxt, &prim.expr.prim.arguments, fx, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &prim.expr.prim.arguments, *schemerlicht_vector_at(&e->expr.set.value, 0, schemerlicht_expression), schemerlicht_expression);
    schemerlicht_vector_destroy(ctxt, &e->expr.set.value);
    schemerlicht_string_destroy(ctxt, &e->expr.set.filename);
    *e = prim;
    }
  //return;
  }

static void visit_variable_assvarconv(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  //schemerlicht_convert_assignable_variables_visitor* vis = (schemerlicht_convert_assignable_variables_visitor*)(v->impl);
  //schemerlicht_vector* vec = schemerlicht_vector_back(&vis->assignable_variables, schemerlicht_vector);
  //if (schemerlicht_string_vector_binary_search(vec, &e->expr.var.name))
  if (is_assignable_variable(v, &e->expr.var.name))
    {
    schemerlicht_expression prim = schemerlicht_init_primcall(ctxt);
    schemerlicht_string_init(ctxt, &prim.expr.prim.name, "vector-ref");
    schemerlicht_vector_push_back(ctxt, &prim.expr.prim.arguments, *e, schemerlicht_expression);
    schemerlicht_expression fx = schemerlicht_init_fixnum(ctxt);
    schemerlicht_vector_push_back(ctxt, &prim.expr.prim.arguments, fx, schemerlicht_expression);
    *e = prim;
    }
  }

static schemerlicht_convert_assignable_variables_visitor* schemerlicht_convert_assignable_variables_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_convert_assignable_variables_visitor* v = schemerlicht_new(ctxt, schemerlicht_convert_assignable_variables_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->previsit_let = previsit_let_assvarconv;
  v->visitor->postvisit_let = postvisit_let_assvarconv;
  v->visitor->previsit_lambda = previsit_lambda_assvarconv;
  v->visitor->postvisit_lambda = postvisit_lambda_assvarconv;
  v->visitor->postvisit_set = postvisit_set_assvarconv;
  v->visitor->visit_variable = visit_variable_assvarconv;
  schemerlicht_vector_init(ctxt, &v->assignable_variables, schemerlicht_vector);
  schemerlicht_vector assignable_vars;
  schemerlicht_vector_init(ctxt, &assignable_vars, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v->assignable_variables, assignable_vars, schemerlicht_vector);
  return v;
  }

static void schemerlicht_convert_assignable_variables_visitor_free(schemerlicht_context* ctxt, schemerlicht_convert_assignable_variables_visitor* v)
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

void schemerlicht_assignable_variable_conversion(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_find_assignable_variables_visitor* v = schemerlicht_find_assignable_variables_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_find_assignable_variables_visitor_free(ctxt, v);
  schemerlicht_convert_assignable_variables_visitor* v2 = schemerlicht_convert_assignable_variables_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v2->visitor, program);
  schemerlicht_convert_assignable_variables_visitor_free(ctxt, v2);
  }