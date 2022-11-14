#include "constfold.h"
#include "context.h"
#include "visitor.h"

static int is_nonmutable(schemerlicht_expression* e)
  {
  // warning: recursive operation: todo, make non recursive
  switch (e->type)
    {
    case schemerlicht_type_literal:
    case schemerlicht_type_variable:
    case schemerlicht_type_quote:
    case schemerlicht_type_nop:
      return 1;
    case schemerlicht_type_primitive_call:
    {
    schemerlicht_expression* it = schemerlicht_vector_begin(&e->expr.prim.arguments, schemerlicht_expression);
    schemerlicht_expression* it_end = schemerlicht_vector_begin(&e->expr.prim.arguments, schemerlicht_expression);
    for (; it != it_end; ++it)
      {
      if (is_nonmutable(it) == 0)
        return 0;
      }
    return 1;
    }
    case schemerlicht_type_if:
    {
    schemerlicht_expression* it = schemerlicht_vector_begin(&e->expr.i.arguments, schemerlicht_expression);
    schemerlicht_expression* it_end = schemerlicht_vector_begin(&e->expr.i.arguments, schemerlicht_expression);
    for (; it != it_end; ++it)
      {
      if (is_nonmutable(it) == 0)
        return 0;
      }
    return 1;
    }
    case schemerlicht_type_begin:
    {
    schemerlicht_expression* it = schemerlicht_vector_begin(&e->expr.beg.arguments, schemerlicht_expression);
    schemerlicht_expression* it_end = schemerlicht_vector_begin(&e->expr.beg.arguments, schemerlicht_expression);
    for (; it != it_end; ++it)
      {
      if (is_nonmutable(it) == 0)
        return 0;
      }
    return 1;
    }
    case schemerlicht_type_foreign_call:
    {
    schemerlicht_expression* it = schemerlicht_vector_begin(&e->expr.foreign.arguments, schemerlicht_expression);
    schemerlicht_expression* it_end = schemerlicht_vector_begin(&e->expr.foreign.arguments, schemerlicht_expression);
    for (; it != it_end; ++it)
      {
      if (is_nonmutable(it) == 0)
        return 0;
      }
    return 1;
    }
    case schemerlicht_type_let:
    {
    schemerlicht_let_binding* it = schemerlicht_vector_begin(&e->expr.let.bindings, schemerlicht_let_binding);
    schemerlicht_let_binding* it_end = schemerlicht_vector_begin(&e->expr.let.bindings, schemerlicht_let_binding);
    for (; it != it_end; ++it)
      {
      if (is_nonmutable(&it->binding_expr) == 0)
        return 0;
      }
    schemerlicht_expression* body = schemerlicht_vector_begin(&e->expr.let.body, schemerlicht_expression);
    return is_nonmutable(body);
    }
    default:
      break;
    }
  return 0;
  }

typedef struct schemerlicht_constant_folding_visitor
  {
  schemerlicht_visitor* visitor;
  } schemerlicht_constant_folding_visitor;


static void postvisit_primcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  }

static void postvisit_if(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->expr.i.arguments.vector_size == 3);
  if (schemerlicht_vector_begin(&e->expr.i.arguments, schemerlicht_expression)->type == schemerlicht_type_literal)
    {
    schemerlicht_expression* lit = schemerlicht_vector_begin(&e->expr.i.arguments, schemerlicht_expression);
    const int idx = lit->expr.lit.type == schemerlicht_type_false ? 2 : 1;
    const int other_idx = lit->expr.lit.type == schemerlicht_type_false ? 1 : 2;
    schemerlicht_expression res = *schemerlicht_vector_at(&e->expr.i.arguments, idx, schemerlicht_expression);
    schemerlicht_string_destroy(ctxt, &e->expr.i.filename);
    schemerlicht_expression* other_branch = schemerlicht_vector_at(&e->expr.i.arguments, other_idx, schemerlicht_expression);
    schemerlicht_expression_destroy(ctxt, other_branch);
    schemerlicht_expression_destroy(ctxt, lit);
    schemerlicht_vector_destroy(ctxt, &e->expr.i.arguments);
    *e = res;
    }

  if (e->type == schemerlicht_type_if)
    {
    schemerlicht_expression* arg0 = schemerlicht_vector_at(&e->expr.i.arguments, 0, schemerlicht_expression);
    schemerlicht_expression* arg1 = schemerlicht_vector_at(&e->expr.i.arguments, 1, schemerlicht_expression);
    schemerlicht_expression* arg2 = schemerlicht_vector_at(&e->expr.i.arguments, 2, schemerlicht_expression);
    if (arg1->type == schemerlicht_type_literal && arg2->type == schemerlicht_type_literal)
      {
      if (arg2->expr.lit.type == arg1->expr.lit.type)
        {
        int equal_values = 0;
        switch (arg1->expr.lit.type)
          {
          case schemerlicht_type_fixnum:
            equal_values = arg1->expr.lit.lit.fx.value == arg2->expr.lit.lit.fx.value ? 1 : 0;
            break;
          case schemerlicht_type_flonum:
            equal_values = arg1->expr.lit.lit.fl.value == arg2->expr.lit.lit.fl.value ? 1 : 0;
            break;
          case schemerlicht_type_character:
            equal_values = arg1->expr.lit.lit.fl.value == arg2->expr.lit.lit.fl.value ? 1 : 0;
            break;
          case schemerlicht_type_true:
          case schemerlicht_type_false:
          case schemerlicht_type_nil:
            equal_values = 1;
            break;
          case schemerlicht_type_string:
            equal_values = strcmp(arg1->expr.lit.lit.sym.value.string_ptr, arg2->expr.lit.lit.sym.value.string_ptr) == 0 ? 1 : 0;
            break;
          case schemerlicht_type_symbol:
            equal_values = strcmp(arg1->expr.lit.lit.str.value.string_ptr, arg2->expr.lit.lit.str.value.string_ptr) == 0 ? 1 : 0;
            break;
          default:
            break;
          }
        if (equal_values && is_nonmutable(arg0))
          {
          schemerlicht_expression value = *arg1;
          schemerlicht_expression_destroy(ctxt, arg0);
          schemerlicht_expression_destroy(ctxt, arg2);
          schemerlicht_vector_destroy(ctxt, &e->expr.i.arguments);
          schemerlicht_string_destroy(ctxt, &e->expr.i.filename);
          *e = value;
          }
        }     
      }
    }

  if (e->type == schemerlicht_type_if)
    {
    // rewrite if (if (ops? x) #t #f ) ... to if (ops? x) ...
    schemerlicht_expression* arg0 = schemerlicht_vector_begin(&e->expr.i.arguments, schemerlicht_expression);
    if (arg0->type == schemerlicht_type_if)
      {
      schemerlicht_expression* arg1 = schemerlicht_vector_at(&arg0->expr.i.arguments, 1, schemerlicht_expression);
      schemerlicht_expression* arg2 = schemerlicht_vector_at(&arg0->expr.i.arguments, 2, schemerlicht_expression);
      if (arg1->type == schemerlicht_type_literal && arg2->type == schemerlicht_type_literal)
        {
        if (arg1->expr.lit.type == schemerlicht_type_true && arg2->expr.lit.type == schemerlicht_type_false)
          {
          schemerlicht_expression move_up = *schemerlicht_vector_at(&arg0->expr.i.arguments, 0, schemerlicht_expression);
          schemerlicht_expression_destroy(ctxt, arg1);
          schemerlicht_expression_destroy(ctxt, arg2);
          schemerlicht_vector_destroy(ctxt, &arg0->expr.i.arguments);
          schemerlicht_string_destroy(ctxt, &arg0->expr.i.filename);
          *arg0 = move_up;
          }
        }
      }
    }
  }

static schemerlicht_constant_folding_visitor* schemerlicht_constant_folding_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_constant_folding_visitor* v = schemerlicht_new(ctxt, schemerlicht_constant_folding_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->postvisit_primcall = postvisit_primcall;
  v->visitor->postvisit_if = postvisit_if;
  return v;
  }

static void schemerlicht_constant_folding_visitor_free(schemerlicht_context* ctxt, schemerlicht_constant_folding_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }

void schemerlicht_constant_folding(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_constant_folding_visitor* v = schemerlicht_constant_folding_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_constant_folding_visitor_free(ctxt, v);
  }