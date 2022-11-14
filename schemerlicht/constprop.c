#include "constprop.h"
#include "context.h"
#include "visitor.h"
#include "map.h"
#include "copy.h"

typedef struct schemerlicht_is_mutable_variable_visitor
  {
  schemerlicht_visitor* visitor;
  schemerlicht_map* is_unmutable;
  } schemerlicht_is_mutable_variable_visitor;

static int previsit_let(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_is_mutable_variable_visitor* vis = (schemerlicht_is_mutable_variable_visitor*)(v->impl);
  schemerlicht_let_binding* it = schemerlicht_vector_begin(&e->expr.let.bindings, schemerlicht_let_binding);
  schemerlicht_let_binding* it_end = schemerlicht_vector_end(&e->expr.let.bindings, schemerlicht_let_binding);
  for (; it != it_end; ++it)
    {
    schemerlicht_object key;
    key.type = schemerlicht_object_type_string;
    key.value.s = it->binding_name;
    schemerlicht_object* value = schemerlicht_map_insert(ctxt, vis->is_unmutable, &key);
    if (value->type == schemerlicht_object_type_undefined)
      {
      value->type = schemerlicht_object_type_fixnum;
      value->value.fx = 1;
      }
    }
  return 1;
  }

static int previsit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_is_mutable_variable_visitor* vis = (schemerlicht_is_mutable_variable_visitor*)(v->impl);  
  schemerlicht_string* it = schemerlicht_vector_begin(&e->expr.lambda.variables, schemerlicht_string);
  schemerlicht_string* it_end = schemerlicht_vector_end(&e->expr.lambda.variables, schemerlicht_string);
  for (; it != it_end; ++it)
    {
    schemerlicht_object key;
    key.type = schemerlicht_object_type_string;
    key.value.s = *it;
    schemerlicht_object* value = schemerlicht_map_insert(ctxt, vis->is_unmutable, &key);
    value->type = schemerlicht_object_type_fixnum;
    value->value.fx = 0;
    }
  return 1;
  }

static void visit_variable(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_is_mutable_variable_visitor* vis = (schemerlicht_is_mutable_variable_visitor*)(v->impl);
  schemerlicht_object key;
  key.type = schemerlicht_object_type_string;
  key.value.s = e->expr.var.name;
  schemerlicht_object* value = schemerlicht_map_insert(ctxt, vis->is_unmutable, &key);
  if (value->type == schemerlicht_object_type_undefined)
    {
    value->type = schemerlicht_object_type_fixnum;
    value->value.fx = 0; // We get here if the variable was unknown so far. That means it could have been defined previously and might be mutable
    }
  }

static int previsit_set(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_is_mutable_variable_visitor* vis = (schemerlicht_is_mutable_variable_visitor*)(v->impl);
  schemerlicht_object key;
  key.type = schemerlicht_object_type_string;
  key.value.s = e->expr.set.name;
  schemerlicht_object* value = schemerlicht_map_insert(ctxt, vis->is_unmutable, &key);
  value->type = schemerlicht_object_type_fixnum;
  value->value.fx = 0;
  return 1;
  }


static schemerlicht_is_mutable_variable_visitor* schemerlicht_is_mutable_variable_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_is_mutable_variable_visitor* v = schemerlicht_new(ctxt, schemerlicht_is_mutable_variable_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->previsit_let = previsit_let;
  v->visitor->previsit_lambda = previsit_lambda;
  v->visitor->previsit_set = previsit_set;
  v->visitor->visit_variable = visit_variable;
  return v;
  }

static void schemerlicht_is_mutable_variable_visitor_free(schemerlicht_context* ctxt, schemerlicht_is_mutable_variable_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);    
    schemerlicht_delete(ctxt, v);
    }
  }

typedef struct schemerlicht_replace_variable_visitor
  {
  schemerlicht_visitor* visitor;  
  schemerlicht_string var_name;
  schemerlicht_expression replace_by_this_expr;
  } schemerlicht_replace_variable_visitor;

static void visit_variable_replace_visitor(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_replace_variable_visitor* vis = (schemerlicht_replace_variable_visitor*)(v->impl);
  if (strcmp(e->expr.var.name.string_ptr, vis->var_name.string_ptr) == 0)
    {
    schemerlicht_expression_destroy(ctxt, e);
    *e = schemerlicht_expression_copy(ctxt, &vis->replace_by_this_expr);
    }
  }

static schemerlicht_replace_variable_visitor* schemerlicht_replace_variable_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_is_mutable_variable_visitor* v = schemerlicht_new(ctxt, schemerlicht_replace_variable_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v); 
  v->visitor->visit_variable = visit_variable_replace_visitor;
  return v;
  }

static void schemerlicht_replace_variable_visitor_free(schemerlicht_context* ctxt, schemerlicht_replace_variable_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }

typedef struct schemerlicht_constant_propagation_visitor
  {
  schemerlicht_visitor* visitor;
  schemerlicht_map* is_unmutable;
  } schemerlicht_constant_propagation_visitor;

static void postvisit_let(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_constant_propagation_visitor* vis = (schemerlicht_constant_propagation_visitor*)(v->impl);
  schemerlicht_let_binding* it = schemerlicht_vector_begin(&e->expr.let.bindings, schemerlicht_let_binding);
  schemerlicht_let_binding* it_end = schemerlicht_vector_end(&e->expr.let.bindings, schemerlicht_let_binding);
  while (it != it_end)
    {
    schemerlicht_object key;
    key.type = schemerlicht_object_type_string;
    key.value.s = it->binding_name;
    schemerlicht_object* value = schemerlicht_map_get(ctxt, vis->is_unmutable, &key);
    if (value != NULL && value->value.fx)
      {
      if (it->binding_expr.type == schemerlicht_type_literal)
        {
        if (it->binding_expr.expr.lit.type != schemerlicht_type_string)
          {                    
          schemerlicht_replace_variable_visitor* repl = schemerlicht_replace_variable_visitor_new(ctxt);
          repl->var_name = it->binding_name;
          repl->replace_by_this_expr = it->binding_expr;
          schemerlicht_expression* body = schemerlicht_vector_begin(&e->expr.let.body, schemerlicht_expression);
          schemerlicht_visit_expression(ctxt, repl->visitor, body);
          schemerlicht_replace_variable_visitor_free(ctxt, repl);
          schemerlicht_expression_destroy(ctxt, &it->binding_expr);
          schemerlicht_string_destroy(ctxt, &it->binding_name);
          schemerlicht_vector_erase(&e->expr.let.bindings, &it, schemerlicht_let_binding);          
          it_end = schemerlicht_vector_end(&e->expr.let.bindings, schemerlicht_let_binding);
          }
        else
          {
          ++it;
          }
        }
      else
        {
        ++it;
        }
      }
    else
      {
      ++it;
      }
    }
  }

static schemerlicht_constant_propagation_visitor* schemerlicht_constant_propagation_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_constant_propagation_visitor* v = schemerlicht_new(ctxt, schemerlicht_constant_propagation_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->postvisit_let = postvisit_let;
  return v;
  }

static void schemerlicht_constant_propagation_visitor_free(schemerlicht_context* ctxt, schemerlicht_constant_propagation_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }

void schemerlicht_constant_propagation(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_map* is_unmutable = schemerlicht_map_new(ctxt, 0, 8);
  schemerlicht_is_mutable_variable_visitor* v1 = schemerlicht_is_mutable_variable_visitor_new(ctxt);
  v1->is_unmutable = is_unmutable;
  schemerlicht_visit_program(ctxt, v1->visitor, program);
  schemerlicht_is_mutable_variable_visitor_free(ctxt, v1);

  schemerlicht_constant_propagation_visitor* v2 = schemerlicht_constant_propagation_visitor_new(ctxt);
  v2->is_unmutable = is_unmutable;
  schemerlicht_visit_program(ctxt, v2->visitor, program);
  schemerlicht_constant_propagation_visitor_free(ctxt, v2);

  schemerlicht_map_free(ctxt, is_unmutable);
  }