#include "constfold.h"
#include "context.h"
#include "visitor.h"
#include "primitives.h"
#include "stringvec.h"

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

// simple as in: no heap memory used
static int all_arguments_are_simple_literals(schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_primitive_call);
  schemerlicht_expression* it = schemerlicht_vector_begin(&e->expr.prim.arguments, schemerlicht_expression);
  schemerlicht_expression* it_end = schemerlicht_vector_end(&e->expr.prim.arguments, schemerlicht_expression);
  for (; it != it_end; ++it)
    {
    if (it->type != schemerlicht_type_literal)
      return 0;
    if (it->expr.lit.type == schemerlicht_type_string || it->expr.lit.type == schemerlicht_type_symbol)
      return 0;
    }
  return 1;
  }

static schemerlicht_object* find_primitive(schemerlicht_context* ctxt, schemerlicht_string* s)
  {
  schemerlicht_object key;
  key.type = schemerlicht_object_type_string;
  key.value.s = *s;
  schemerlicht_object* res = schemerlicht_map_get(ctxt, ctxt->global->primitives_map, &key);
  return res;
  }

static int prepare_primitive_call(schemerlicht_context* ctxt, schemerlicht_expression* e, schemerlicht_object* prim)
  {
  schemerlicht_assert(e->type == schemerlicht_type_primitive_call);
  schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
  schemerlicht_set_object(target, prim);

  schemerlicht_expression* it = schemerlicht_vector_begin(&e->expr.prim.arguments, schemerlicht_expression);
  schemerlicht_expression* it_end = schemerlicht_vector_end(&e->expr.prim.arguments, schemerlicht_expression);
  int index = 1;
  for (; it != it_end; ++it)
    {
    target = schemerlicht_vector_at(&ctxt->stack, index, schemerlicht_object);
    schemerlicht_assert(it->type == schemerlicht_type_literal);
    schemerlicht_assert(it->expr.lit.type != schemerlicht_type_string);
    switch (it->expr.lit.type)
      {
      case schemerlicht_type_fixnum:
        target->type = schemerlicht_object_type_fixnum;
        target->value.fx = it->expr.lit.lit.fx.value;
        break;
      case schemerlicht_type_flonum:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = it->expr.lit.lit.fl.value;
        break;
      case schemerlicht_type_character:
        target->type = schemerlicht_object_type_char;
        target->value.ch = it->expr.lit.lit.ch.value;
        break;
      case schemerlicht_type_true:
        target->type = schemerlicht_object_type_true;
        break;
      case schemerlicht_type_false:
        target->type = schemerlicht_object_type_false;
        break;
      case schemerlicht_type_nil:
        target->type = schemerlicht_object_type_nil;
        break;
      default:
        return 0;
      }
    ++index;
    }
  return 1;
  }

static void replace_expression_by_result(schemerlicht_context* ctxt, schemerlicht_expression* e, schemerlicht_object* res)
  {
  switch (res->type)
    {
    case schemerlicht_object_type_fixnum:
      schemerlicht_expression_destroy(ctxt, e);
      *e = schemerlicht_init_fixnum(ctxt);
      e->expr.lit.lit.fx.value = res->value.fx;
      break;
    case schemerlicht_object_type_flonum:
      schemerlicht_expression_destroy(ctxt, e);
      *e = schemerlicht_init_flonum(ctxt);
      e->expr.lit.lit.fl.value = res->value.fl;
      break;
    case schemerlicht_object_type_char:
      schemerlicht_expression_destroy(ctxt, e);
      *e = schemerlicht_init_char(ctxt);
      e->expr.lit.lit.ch.value = res->value.ch;
      break;
    case schemerlicht_object_type_true:
      schemerlicht_expression_destroy(ctxt, e);
      *e = schemerlicht_init_true(ctxt);
      break;
    case schemerlicht_object_type_false:
      schemerlicht_expression_destroy(ctxt, e);
      *e = schemerlicht_init_false(ctxt);
      break;
    case schemerlicht_object_type_nil:
      schemerlicht_expression_destroy(ctxt, e);
      *e = schemerlicht_init_nil(ctxt);
      break;
    default:
      break;
    }
  }

static void fold_primcall(schemerlicht_context* ctxt, schemerlicht_expression* e)
  {
  if (all_arguments_are_simple_literals(e))
    {
    schemerlicht_object* prim = find_primitive(ctxt, &e->expr.prim.name);
    if (prim != NULL)
      {
      if (prepare_primitive_call(ctxt, e, prim))
        {
        schemerlicht_call_primitive(ctxt, prim->value.fx, 0, cast(int, e->expr.prim.arguments.vector_size), 0);
        schemerlicht_object* result = schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
        replace_expression_by_result(ctxt, e, result);
        }
      }
    }
  }

static void postvisit_primcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  if (e->expr.prim.as_object || (e->expr.prim.arguments.vector_size == 0))
    return;
  if (schemerlicht_string_vector_binary_search(&ctxt->global->foldable_primitives, &e->expr.prim.name))
    {
    fold_primcall(ctxt, e);
    } 
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

static void foldable_prim_insert(schemerlicht_context* ctxt, schemerlicht_vector* v, const char* name)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, name);
  schemerlicht_vector_push_back(ctxt, v, s, schemerlicht_string);
  }

schemerlicht_vector schemerlicht_generate_foldable_primitives_set(schemerlicht_context* ctxt)
  {
  schemerlicht_vector v;
  schemerlicht_vector_init(ctxt, &v, schemerlicht_string);
  foldable_prim_insert(ctxt, &v, "+");
  foldable_prim_insert(ctxt, &v, "-");
  foldable_prim_insert(ctxt, &v, "*");
  foldable_prim_insert(ctxt, &v, "/");
  foldable_prim_insert(ctxt, &v, "add1");
  foldable_prim_insert(ctxt, &v, "sub1");
  foldable_prim_insert(ctxt, &v, "=");
  foldable_prim_insert(ctxt, &v, "!=");
  foldable_prim_insert(ctxt, &v, "<");
  foldable_prim_insert(ctxt, &v, ">");
  foldable_prim_insert(ctxt, &v, "<=");
  foldable_prim_insert(ctxt, &v, ">=");
  foldable_prim_insert(ctxt, &v,  "fixnum?");
  foldable_prim_insert(ctxt, &v,  "flonum?");
  foldable_prim_insert(ctxt, &v,  "null?");
  foldable_prim_insert(ctxt, &v,  "zero?");
  foldable_prim_insert(ctxt, &v,  "boolean?");
  foldable_prim_insert(ctxt, &v,  "char?");
  foldable_prim_insert(ctxt, &v,  "not");
  foldable_prim_insert(ctxt, &v,  "fxadd1");
  foldable_prim_insert(ctxt, &v,  "fxsub1");
  foldable_prim_insert(ctxt, &v,  "fx+");
  foldable_prim_insert(ctxt, &v,  "fx-");
  foldable_prim_insert(ctxt, &v,  "fx*");
  foldable_prim_insert(ctxt, &v,  "fx/");
  foldable_prim_insert(ctxt, &v,  "fx=?");
  foldable_prim_insert(ctxt, &v,  "fx<?");
  foldable_prim_insert(ctxt, &v,  "fx<=?");
  foldable_prim_insert(ctxt, &v,  "fx>?");
  foldable_prim_insert(ctxt, &v,  "fx>=?");
  foldable_prim_insert(ctxt, &v,  "fxzero?");
  foldable_prim_insert(ctxt, &v,  "fladd1");
  foldable_prim_insert(ctxt, &v,  "flsub1");
  foldable_prim_insert(ctxt, &v,  "fl+");
  foldable_prim_insert(ctxt, &v,  "fl-");
  foldable_prim_insert(ctxt, &v,  "fl*");
  foldable_prim_insert(ctxt, &v,  "fl/");
  foldable_prim_insert(ctxt, &v,  "fl=?");
  foldable_prim_insert(ctxt, &v,  "fl<?");
  foldable_prim_insert(ctxt, &v,  "fl<=?");
  foldable_prim_insert(ctxt, &v,  "fl>?");
  foldable_prim_insert(ctxt, &v,  "fl>=?");
  foldable_prim_insert(ctxt, &v,  "flzero?");
  foldable_prim_insert(ctxt, &v,  "char=?");
  foldable_prim_insert(ctxt, &v,  "char<?");
  foldable_prim_insert(ctxt, &v,  "char<=?");
  foldable_prim_insert(ctxt, &v,  "char>?");
  foldable_prim_insert(ctxt, &v,  "char>=?");
  foldable_prim_insert(ctxt, &v,  "char-ci=?");
  foldable_prim_insert(ctxt, &v,  "char-ci<?");
  foldable_prim_insert(ctxt, &v,  "char-ci<=?");
  foldable_prim_insert(ctxt, &v,  "char-ci>?");
  foldable_prim_insert(ctxt, &v,  "char-ci>=?");
  foldable_prim_insert(ctxt, &v,  "fixnum->char");
  foldable_prim_insert(ctxt, &v,  "integer->char");
  foldable_prim_insert(ctxt, &v,  "char->fixnum");
  foldable_prim_insert(ctxt, &v,  "char->integer");
  foldable_prim_insert(ctxt, &v,  "fixnum->flonum");
  foldable_prim_insert(ctxt, &v,  "flonum->fixnum");
  foldable_prim_insert(ctxt, &v,  "bitwise-and");
  foldable_prim_insert(ctxt, &v,  "bitwise-or");
  foldable_prim_insert(ctxt, &v,  "bitwise-not");
  foldable_prim_insert(ctxt, &v,  "bitwise-xor");
  foldable_prim_insert(ctxt, &v,  "arithmetic-shift");  
  foldable_prim_insert(ctxt, &v,  "vector?");
  foldable_prim_insert(ctxt, &v,  "pair?");      
  foldable_prim_insert(ctxt, &v,  "closure?");
  foldable_prim_insert(ctxt, &v,  "string?");
  foldable_prim_insert(ctxt, &v,  "eq?");
  foldable_prim_insert(ctxt, &v,  "eqv?");
  foldable_prim_insert(ctxt, &v,  "equal?");
  foldable_prim_insert(ctxt, &v,  "symbol?");
  foldable_prim_insert(ctxt, &v,  "procedure?");
  foldable_prim_insert(ctxt, &v,  "list?");
  foldable_prim_insert(ctxt, &v,  "max");
  foldable_prim_insert(ctxt, &v,  "min");
  foldable_prim_insert(ctxt, &v,  "exact?");
  foldable_prim_insert(ctxt, &v,  "inexact?");
  foldable_prim_insert(ctxt, &v,  "number?");
  foldable_prim_insert(ctxt, &v,  "rational?");
  foldable_prim_insert(ctxt, &v,  "positive?");
  foldable_prim_insert(ctxt, &v,  "negative?");
  foldable_prim_insert(ctxt, &v,  "odd?");
  foldable_prim_insert(ctxt, &v,  "even?");
  foldable_prim_insert(ctxt, &v,  "complex?");
  foldable_prim_insert(ctxt, &v,  "real?");
  foldable_prim_insert(ctxt, &v,  "integer?");
  foldable_prim_insert(ctxt, &v,  "abs");
  foldable_prim_insert(ctxt, &v,  "quotient");
  foldable_prim_insert(ctxt, &v,  "remainder");
  foldable_prim_insert(ctxt, &v,  "modulo");
  foldable_prim_insert(ctxt, &v,  "gcd");
  foldable_prim_insert(ctxt, &v,  "lcm");
  foldable_prim_insert(ctxt, &v,  "floor");
  foldable_prim_insert(ctxt, &v,  "ceiling");
  foldable_prim_insert(ctxt, &v,  "truncate");
  foldable_prim_insert(ctxt, &v,  "round");
  foldable_prim_insert(ctxt, &v,  "exp");
  foldable_prim_insert(ctxt, &v,  "expt");
  foldable_prim_insert(ctxt, &v,  "log");
  foldable_prim_insert(ctxt, &v,  "sin");
  foldable_prim_insert(ctxt, &v,  "cos");
  foldable_prim_insert(ctxt, &v,  "tan");
  foldable_prim_insert(ctxt, &v,  "asin");
  foldable_prim_insert(ctxt, &v,  "acos");
  foldable_prim_insert(ctxt, &v,  "atan");
  foldable_prim_insert(ctxt, &v,  "sqrt");
  foldable_prim_insert(ctxt, &v,  "exact->inexact");
  foldable_prim_insert(ctxt, &v,  "inexact->exact");  
  foldable_prim_insert(ctxt, &v,  "nan?");
  foldable_prim_insert(ctxt, &v,  "inf?");
  foldable_prim_insert(ctxt, &v,  "finite?");  
  foldable_prim_insert(ctxt, &v,  "char-upcase");
  foldable_prim_insert(ctxt, &v,  "char-downcase");
  foldable_prim_insert(ctxt, &v,  "char-upper-case?");
  foldable_prim_insert(ctxt, &v,  "char-lower-case?");
  foldable_prim_insert(ctxt, &v,  "char-alphabetic?");
  foldable_prim_insert(ctxt, &v,  "char-numeric?");
  foldable_prim_insert(ctxt, &v,  "char-whitespace?");
  schemerlicht_string_vector_sort(&v);
  return v;
  }