#include "alpha.h"
#include "context.h"
#include "visitor.h"
#include "string.h"
#include "map.h"
#include "syscalls.h"
#include <stdio.h>

schemerlicht_string schemerlicht_make_alpha_name(schemerlicht_context* ctxt, schemerlicht_string* original, schemerlicht_memsize index)
  {
  char buffer[256];
  schemerlicht_memsize_to_char(buffer, index);  
  schemerlicht_string new_name;
  schemerlicht_string_init(ctxt, &new_name, "%");
  schemerlicht_string_append(ctxt, &new_name, original);
  schemerlicht_string_push_back(ctxt, &new_name, '_');
  schemerlicht_string_append_cstr(ctxt, &new_name, buffer);
  return new_name;
  }

schemerlicht_string schemerlicht_get_original_name_from_alpha(schemerlicht_context* ctxt, schemerlicht_string* alpha_name)
  {
  char* it = schemerlicht_string_begin(alpha_name);
  char* it_end = schemerlicht_string_end(alpha_name);
  while (*it_end != '_')
    --it_end;
  ++it;
  schemerlicht_string original;
  schemerlicht_string_init_ranged(ctxt, &original, it, it_end);
  return original;
  }

typedef struct schemerlicht_alpha_conversion_visitor
  {
  schemerlicht_memsize index;
  schemerlicht_vector variables; // linked chain of variable maps
  schemerlicht_visitor* visitor;
  } schemerlicht_alpha_conversion_visitor;

static void push_variables_child(schemerlicht_context* ctxt, schemerlicht_alpha_conversion_visitor* v)
  {
  schemerlicht_map* child_vars = schemerlicht_map_new(ctxt, 0, 3);
  schemerlicht_vector_push_back(ctxt, &v->variables, child_vars, schemerlicht_map*);
  }

static void pop_variables_child(schemerlicht_context* ctxt, schemerlicht_alpha_conversion_visitor* v)
  {
  schemerlicht_map** child_map = schemerlicht_vector_back(&v->variables, schemerlicht_map*);
  schemerlicht_map_keys_free(ctxt, *child_map);
  //schemerlicht_map_values_free(ctxt, *child_map);
  schemerlicht_map_free(ctxt, *child_map);
  schemerlicht_vector_pop_back(&v->variables);
  }

static void add_variable(schemerlicht_context* ctxt, schemerlicht_alpha_conversion_visitor* v, schemerlicht_string* var_name, schemerlicht_string* alpha_name)
  {
  schemerlicht_assert(v->variables.vector_size > 0);
  //schemerlicht_string alpha_name = schemerlicht_make_alpha_name(ctxt, var_name, v->index++);
  schemerlicht_map** active_map = schemerlicht_vector_back(&v->variables, schemerlicht_map*);
  schemerlicht_object key;
  key.type = schemerlicht_object_type_string;
  key.value.s = *var_name;
  schemerlicht_object* obj = schemerlicht_map_insert(ctxt, *active_map, &key);
  obj->type = schemerlicht_object_type_string;
  obj->value.s = *alpha_name;
  }

static int find_variable(schemerlicht_string* alpha_name, schemerlicht_context* ctxt, schemerlicht_alpha_conversion_visitor* v, schemerlicht_string* var_name)
  {
  schemerlicht_object key;
  key.type = schemerlicht_object_type_string;
  key.value.s = *var_name;
  schemerlicht_map** map_it = schemerlicht_vector_begin(&v->variables, schemerlicht_map*);
  schemerlicht_map** map_it_end = schemerlicht_vector_end(&v->variables, schemerlicht_map*);
  schemerlicht_map** map_rit = map_it_end - 1;
  schemerlicht_map** map_rit_end = map_it - 1;
  for (; map_rit != map_rit_end; --map_rit)
    {
    schemerlicht_object* obj = schemerlicht_map_get(ctxt, *map_rit, &key);
    if (obj != NULL)
      {
      *alpha_name = obj->value.s;
      return 1;
      }
    }
  return 0;
  }

static void visit_variable(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_alpha_conversion_visitor* vis = (schemerlicht_alpha_conversion_visitor*)(v->impl);
  schemerlicht_string alpha_name;
  if (find_variable(&alpha_name, ctxt, vis, &e->expr.var.name))
    {
    schemerlicht_string_clear(&e->expr.var.name);
    schemerlicht_string_append(ctxt, &e->expr.var.name, &alpha_name);
    }
  }

static int previsit_set(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  if (e->expr.set.originates_from_define != 0 || e->expr.set.originates_from_quote != 0)
    return 1;
  schemerlicht_alpha_conversion_visitor* vis = (schemerlicht_alpha_conversion_visitor*)(v->impl);
  schemerlicht_string alpha_name;
  if (find_variable(&alpha_name, ctxt, vis, &e->expr.set.name))
    {
    schemerlicht_string_clear(&e->expr.set.name);
    schemerlicht_string_append(ctxt, &e->expr.set.name, &alpha_name);
    }
  return 1;
  }

static void postvisit_primcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_alpha_conversion_visitor* vis = (schemerlicht_alpha_conversion_visitor*)(v->impl);
  schemerlicht_string alpha_name;
  if (find_variable(&alpha_name, ctxt, vis, &e->expr.prim.name))
    {
    schemerlicht_expression var = schemerlicht_init_variable(ctxt);
    schemerlicht_string_copy(ctxt, &var.expr.var.name, &alpha_name);
    schemerlicht_string_destroy(ctxt, &e->expr.prim.name);
    var.expr.var.filename = e->expr.prim.filename;
    var.expr.var.line_nr = e->expr.prim.line_nr;
    var.expr.var.column_nr = e->expr.prim.column_nr;
    if (e->expr.prim.as_object)
      {
      schemerlicht_vector_destroy(ctxt, &e->expr.prim.arguments);
      *e = var;
      }
    else
      {
      schemerlicht_expression fun = schemerlicht_init_funcall(ctxt);
      schemerlicht_vector_destroy(ctxt, &fun.expr.funcall.arguments);
      fun.expr.funcall.arguments = e->expr.prim.arguments;
      fun.expr.funcall.line_nr = e->expr.prim.line_nr;
      fun.expr.funcall.column_nr = e->expr.prim.column_nr;
      schemerlicht_vector_push_back(ctxt, &fun.expr.funcall.fun, var, schemerlicht_expression);
      *e = fun;
      }
    }  
  }

static int previsit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_alpha_conversion_visitor* vis = (schemerlicht_alpha_conversion_visitor*)(v->impl);
  push_variables_child(ctxt, vis);
  schemerlicht_string* it = schemerlicht_vector_begin(&e->expr.lambda.variables, schemerlicht_string);
  schemerlicht_string* it_end = schemerlicht_vector_end(&e->expr.lambda.variables, schemerlicht_string);
  for (; it != it_end; ++it)
    {
    schemerlicht_string alpha_name = schemerlicht_make_alpha_name(ctxt, it, vis->index++);
    add_variable(ctxt, vis, it, &alpha_name);
    *it = alpha_name;
    }
  return 1;
  }

static void postvisit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_alpha_conversion_visitor* vis = (schemerlicht_alpha_conversion_visitor*)(v->impl);
  pop_variables_child(ctxt, vis);
  }

static void postvisit_let_bindings(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_alpha_conversion_visitor* vis = (schemerlicht_alpha_conversion_visitor*)(v->impl);
  push_variables_child(ctxt, vis);
  const schemerlicht_memsize nr_let_bindings = e->expr.let.bindings.vector_size;
  for (schemerlicht_memsize i = 0; i < nr_let_bindings; ++i)
    {
    schemerlicht_let_binding* binding = schemerlicht_vector_at(&e->expr.let.bindings, i, schemerlicht_let_binding);
    schemerlicht_string alpha_name = schemerlicht_make_alpha_name(ctxt, &binding->binding_name, vis->index++);
    add_variable(ctxt, vis, &binding->binding_name, &alpha_name);
    binding->binding_name = alpha_name;
    }
  }
static void postvisit_let(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_alpha_conversion_visitor* vis = (schemerlicht_alpha_conversion_visitor*)(v->impl);
  pop_variables_child(ctxt, vis);
  UNUSED(e);
  }

static schemerlicht_alpha_conversion_visitor* schemerlicht_alpha_conversion_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_alpha_conversion_visitor* v = schemerlicht_new(ctxt, schemerlicht_alpha_conversion_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->visit_variable = visit_variable;
  v->visitor->previsit_set = previsit_set;
  v->visitor->previsit_lambda = previsit_lambda;
  v->visitor->postvisit_lambda = postvisit_lambda;
  v->visitor->postvisit_let_bindings = postvisit_let_bindings;
  v->visitor->postvisit_let = postvisit_let;
  v->visitor->postvisit_primcall = postvisit_primcall;
  return v;
  }

static void schemerlicht_alpha_conversion_visitor_free(schemerlicht_context* ctxt, schemerlicht_alpha_conversion_visitor* v)
  {
  if (v)
    {
    schemerlicht_vector_destroy(ctxt, &v->variables);
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }


void schemerlicht_alpha_conversion(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_alpha_conversion_visitor* v = schemerlicht_alpha_conversion_visitor_new(ctxt);
  v->index = 0;
  schemerlicht_vector_init(ctxt, &v->variables, schemerlicht_map*);
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_alpha_conversion_visitor_free(ctxt, v);
  }