#include "cps.h"
#include "vector.h"
#include "parser.h"
#include "tailcall.h"
#include "limits.h"
#include "copy.h"
#include "syscalls.h"

#include <string.h>
#include <stdio.h>

static int continuation_is_valid(schemerlicht_context* ctxt, schemerlicht_expression* continuation)
  {
  if (!(continuation->type == schemerlicht_type_lambda || continuation->type == schemerlicht_type_variable))
    return 0;
  schemerlicht_expression_tail_call_analysis(ctxt, continuation);
  return schemerlicht_expression_only_has_tail_calls(ctxt, continuation);
  }

static int continuation_is_lambda_with_one_parameter_without_free_vars(schemerlicht_expression* continuation)
  {
  if (continuation->type != schemerlicht_type_lambda)
    return 0;
  if (continuation->expr.lambda.variables.vector_size != 1)
    return 0;
  if (continuation->expr.lambda.free_variables.vector_size > 0)
    return 0;
  return 1;
  }

static int is_simple(schemerlicht_context* ctxt, schemerlicht_expression* e)
  {
  schemerlicht_vector expressions;
  schemerlicht_vector_init(ctxt, &expressions, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &expressions, *e, schemerlicht_expression);
  while (expressions.vector_size > 0)
    {
    schemerlicht_expression current = *schemerlicht_vector_back(&expressions, schemerlicht_expression);
    schemerlicht_assert(current.type >= 0 && current.type <= schemerlicht_type_set);
    schemerlicht_vector_pop_back(&expressions);
    switch (current.type)
      {
      case schemerlicht_type_literal:
        continue;
      case schemerlicht_type_quote:
        continue;
      case schemerlicht_type_variable:
        continue;
      case schemerlicht_type_primitive_call:
      {
      schemerlicht_expression* it = schemerlicht_vector_begin(&current.expr.prim.arguments, schemerlicht_expression);
      schemerlicht_expression* it_end = schemerlicht_vector_end(&current.expr.prim.arguments, schemerlicht_expression);
      for (; it != it_end; ++it)
        {
        schemerlicht_vector_push_back(ctxt, &expressions, *it, schemerlicht_expression);
        }
      continue;
      }
      case schemerlicht_type_foreign_call:
      {
      schemerlicht_expression* it = schemerlicht_vector_begin(&current.expr.foreign.arguments, schemerlicht_expression);
      schemerlicht_expression* it_end = schemerlicht_vector_end(&current.expr.foreign.arguments, schemerlicht_expression);
      for (; it != it_end; ++it)
        {
        schemerlicht_vector_push_back(ctxt, &expressions, *it, schemerlicht_expression);
        }
      continue;
      }
      case schemerlicht_type_set:
      {
      schemerlicht_expression* it = schemerlicht_vector_begin(&current.expr.set.value, schemerlicht_expression);
      schemerlicht_expression* it_end = schemerlicht_vector_end(&current.expr.set.value, schemerlicht_expression);
      for (; it != it_end; ++it)
        {
        schemerlicht_vector_push_back(ctxt, &expressions, *it, schemerlicht_expression);
        }
      continue;
      }
      case schemerlicht_type_if:
      {
      schemerlicht_expression* it = schemerlicht_vector_begin(&current.expr.i.arguments, schemerlicht_expression);
      schemerlicht_expression* it_end = schemerlicht_vector_end(&current.expr.i.arguments, schemerlicht_expression);
      for (; it != it_end; ++it)
        {
        schemerlicht_vector_push_back(ctxt, &expressions, *it, schemerlicht_expression);
        }
      continue;
      }
      case schemerlicht_type_let:
      {
      schemerlicht_let_binding* it = schemerlicht_vector_begin(&current.expr.let.bindings, schemerlicht_let_binding);
      schemerlicht_let_binding* it_end = schemerlicht_vector_end(&current.expr.let.bindings, schemerlicht_let_binding);
      for (; it != it_end; ++it)
        {
        schemerlicht_vector_push_back(ctxt, &expressions, it->binding_expr, schemerlicht_expression);
        }
      schemerlicht_expression* expr = schemerlicht_vector_at(&current.expr.let.body, 0, schemerlicht_expression);
      schemerlicht_vector_push_back(ctxt, &expressions, *expr, schemerlicht_expression);
      continue;
      }
      case schemerlicht_type_begin:
      {
      schemerlicht_expression* it = schemerlicht_vector_begin(&current.expr.beg.arguments, schemerlicht_expression);
      schemerlicht_expression* it_end = schemerlicht_vector_end(&current.expr.beg.arguments, schemerlicht_expression);
      for (; it != it_end; ++it)
        {
        schemerlicht_vector_push_back(ctxt, &expressions, *it, schemerlicht_expression);
        }
      continue;
      }
      case schemerlicht_type_nop: continue;
      }
    schemerlicht_vector_destroy(ctxt, &expressions);
    return 0;
    }
  schemerlicht_vector_destroy(ctxt, &expressions);
  return 1;
  }

static schemerlicht_string make_var_name(schemerlicht_context* ctxt, schemerlicht_memsize index)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "#%k");
  char buffer[20];
  memset(buffer, 0, 20 * sizeof(char));
  schemerlicht_memsize_to_char(buffer, index);
  schemerlicht_string_append_cstr(ctxt, &s, buffer);
  return s;
  }

typedef struct non_simple_var_struct
  {
  schemerlicht_memsize index;
  schemerlicht_string name;
  } non_simple_var_struct;

#define state_init 0
#define state_step_1 1
#define state_step_2 2
#define state_step_3 3
#define state_step_clean_up 4

typedef struct cps_conversion_state
  {
  schemerlicht_expression* expr;
  int state;
  schemerlicht_vector nonsimple_vars;
  schemerlicht_string nonsimple_fun_var;
  schemerlicht_memsize var_index;
  } cps_conversion_state;

static cps_conversion_state init_conversion_state(schemerlicht_expression* e, int state)
  {
  cps_conversion_state s;
  s.expr = e;
  s.state = state;
  s.nonsimple_vars.vector_ptr = NULL;
  s.nonsimple_vars.vector_size = 0;
  s.nonsimple_vars.vector_capacity = 0;
  s.nonsimple_fun_var = make_null_string();
  s.var_index = 0;
  return s;
  }

typedef struct cps_conversion_helper
  {
  schemerlicht_vector index;
  schemerlicht_vector expressions_to_treat;
  schemerlicht_vector continuation;
  } cps_conversion_helper;

static schemerlicht_expression* get_continuation(cps_conversion_helper* cps)
  {
  return schemerlicht_vector_back(&cps->continuation, schemerlicht_expression);
  }

static void cps_convert_literal_or_variable_or_nop_or_quote(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  if (continuation_is_lambda_with_one_parameter_without_free_vars(get_continuation(cps)))
    {
    schemerlicht_expression* lam = get_continuation(cps);
    schemerlicht_expression l = schemerlicht_init_let(ctxt);
    schemerlicht_let_binding bind;
    bind.binding_name = *schemerlicht_vector_at(&lam->expr.lambda.variables, 0, schemerlicht_string);
    bind.binding_expr = *e;
    schemerlicht_vector_push_back(ctxt, &l.expr.let.bindings, bind, schemerlicht_let_binding);
    schemerlicht_vector_push_back(ctxt, &l.expr.let.body, *schemerlicht_vector_at(&lam->expr.lambda.body, 0, schemerlicht_expression), schemerlicht_expression);
    *e = l;
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.assignable_variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.free_variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.body);
    schemerlicht_string_destroy(ctxt, &lam->expr.lambda.filename);
    }
  else
    {
    schemerlicht_expression* cont = get_continuation(cps);
    schemerlicht_expression f = schemerlicht_init_funcall(ctxt);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.fun, *cont, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.arguments, *e, schemerlicht_expression);
    *e = f;
    }
  }

static void cps_convert_set_simple(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  schemerlicht_assert(e->type == schemerlicht_type_set);
  if (continuation_is_lambda_with_one_parameter_without_free_vars(get_continuation(cps)))
    {
    schemerlicht_expression* lam = get_continuation(cps);
    schemerlicht_expression l = schemerlicht_init_let(ctxt);
    schemerlicht_let_binding bind;
    bind.binding_name = *schemerlicht_vector_at(&lam->expr.lambda.variables, 0, schemerlicht_string);
    bind.binding_expr = *e;
    schemerlicht_vector_push_back(ctxt, &l.expr.let.bindings, bind, schemerlicht_let_binding);
    schemerlicht_vector_push_back(ctxt, &l.expr.let.body, *schemerlicht_vector_at(&lam->expr.lambda.body, 0, schemerlicht_expression), schemerlicht_expression);
    *e = l;
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.assignable_variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.free_variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.body);
    schemerlicht_string_destroy(ctxt, &lam->expr.lambda.filename);
    }
  else
    {
    schemerlicht_expression* cont = get_continuation(cps);
    schemerlicht_expression f = schemerlicht_init_funcall(ctxt);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.fun, *cont, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.arguments, *e, schemerlicht_expression);
    *e = f;
    }
  }

static void cps_convert_set_nonsimple(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  schemerlicht_assert(e->type == schemerlicht_type_set);

  schemerlicht_expression* e1 = schemerlicht_vector_at(&e->expr.set.value, 0, schemerlicht_expression);
  schemerlicht_memsize idx_val = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) + 1;
  schemerlicht_vector_push_back(ctxt, &cps->index, idx_val, schemerlicht_memsize);

  schemerlicht_expression l = schemerlicht_init_lambda(ctxt);
  schemerlicht_string lambda_var_name = make_var_name(ctxt, idx_val);
  schemerlicht_vector_push_back(ctxt, &l.expr.lambda.variables, lambda_var_name, schemerlicht_string);
  schemerlicht_expression new_s = schemerlicht_init_set(ctxt);
  new_s.expr.set.name = e->expr.set.name;
  new_s.expr.set.originates_from_define = e->expr.set.originates_from_define;
  new_s.expr.set.originates_from_quote = e->expr.set.originates_from_quote;
  schemerlicht_expression v = schemerlicht_init_variable(ctxt);
  //schemerlicht_string_destroy(ctxt, &v.expr.var.name);
  v.expr.var.name = make_var_name(ctxt, idx_val);
  schemerlicht_vector_push_back(ctxt, &new_s.expr.set.value, v, schemerlicht_expression);
  schemerlicht_expression b = schemerlicht_init_begin(ctxt);


  if (continuation_is_lambda_with_one_parameter_without_free_vars(get_continuation(cps)))
    {
    schemerlicht_expression* lam = get_continuation(cps);
    schemerlicht_expression let = schemerlicht_init_let(ctxt);
    schemerlicht_let_binding bind;
    bind.binding_name = *schemerlicht_vector_at(&lam->expr.lambda.variables, 0, schemerlicht_string);
    bind.binding_expr = new_s;
    schemerlicht_vector_push_back(ctxt, &let.expr.let.bindings, bind, schemerlicht_let_binding);
    schemerlicht_vector_push_back(ctxt, &let.expr.let.body, *schemerlicht_vector_at(&lam->expr.lambda.body, 0, schemerlicht_expression), schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &b.expr.beg.arguments, let, schemerlicht_expression);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.assignable_variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.free_variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.body);
    schemerlicht_string_destroy(ctxt, &lam->expr.lambda.filename);
    }
  else
    {
    schemerlicht_expression* cont = get_continuation(cps);
    schemerlicht_expression f = schemerlicht_init_funcall(ctxt);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.fun, *cont, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.arguments, new_s, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &b.expr.beg.arguments, f, schemerlicht_expression);
    }
  schemerlicht_vector_push_back(ctxt, &l.expr.lambda.body, b, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &cps->continuation, l, schemerlicht_expression);
  schemerlicht_assert(continuation_is_valid(ctxt, get_continuation(cps)));
  cps_conversion_state st = init_conversion_state(e, state_step_1);
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);

  cps_conversion_state st2 = init_conversion_state(e1, state_init);
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st2, cps_conversion_state);
  }

static void cps_convert_set_nonsimple_step1(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  schemerlicht_assert(e->type == schemerlicht_type_set);
  schemerlicht_memsize ind = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize);
  schemerlicht_vector_pop_back(&cps->index);
  *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) = ind;
  schemerlicht_vector_pop_back(&cps->continuation);

  schemerlicht_expression e1 = *schemerlicht_vector_at(&e->expr.set.value, 0, schemerlicht_expression);
  schemerlicht_string_destroy(ctxt, &e->expr.set.filename);
  schemerlicht_vector_destroy(ctxt, &e->expr.set.value);
  *e = e1;
  }

static void cps_convert_set(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  if (is_simple(ctxt, e))
    cps_convert_set_simple(ctxt, e, cps);
  else
    cps_convert_set_nonsimple(ctxt, e, cps);
  }

static void cps_convert_begin_simple(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  schemerlicht_assert(e->type == schemerlicht_type_begin);
  if (e->expr.beg.arguments.vector_size == 0)
    {
    schemerlicht_expression n = schemerlicht_init_nop(ctxt);
    schemerlicht_vector_push_back(ctxt, &e->expr.beg.arguments, n, schemerlicht_expression);
    }
  schemerlicht_assert(is_simple(ctxt, e));
  cps_conversion_state st = init_conversion_state(schemerlicht_vector_back(&e->expr.beg.arguments, schemerlicht_expression), state_init);
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);
  }

static void cps_convert_begin_nonsimple(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  schemerlicht_assert(e->type == schemerlicht_type_begin);
  schemerlicht_memsize first_non_simple_expr = 0;
  for (; first_non_simple_expr < e->expr.beg.arguments.vector_size; ++first_non_simple_expr)
    {
    if (!is_simple(ctxt, schemerlicht_vector_at(&e->expr.beg.arguments, first_non_simple_expr, schemerlicht_expression)))
      break;
    }
  if (first_non_simple_expr == e->expr.beg.arguments.vector_size - 1)
    {
    cps_conversion_state st = init_conversion_state(schemerlicht_vector_back(&e->expr.beg.arguments, schemerlicht_expression), state_init);
    schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);
    }
  else if (first_non_simple_expr > 0)
    {
    schemerlicht_expression new_b = schemerlicht_init_begin(ctxt);
    schemerlicht_expression* insert_pos = schemerlicht_vector_begin(&new_b.expr.beg.arguments, schemerlicht_expression);
    schemerlicht_expression* first = schemerlicht_vector_begin(&e->expr.beg.arguments, schemerlicht_expression);
    schemerlicht_expression* last = first + first_non_simple_expr;
    schemerlicht_vector_insert(ctxt, &new_b.expr.beg.arguments, &insert_pos, &first, &last, schemerlicht_expression);
    schemerlicht_vector_erase_range(&e->expr.beg.arguments, &first, &last, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &new_b.expr.beg.arguments, *e, schemerlicht_expression);
    *e = new_b;
    cps_conversion_state st = init_conversion_state(schemerlicht_vector_back(&e->expr.beg.arguments, schemerlicht_expression), state_init);
    schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);
    }
  else
    {
    schemerlicht_expression remainder = schemerlicht_init_begin(ctxt);
    schemerlicht_expression* insert_pos = schemerlicht_vector_begin(&remainder.expr.beg.arguments, schemerlicht_expression);
    schemerlicht_expression* first = schemerlicht_vector_begin(&e->expr.beg.arguments, schemerlicht_expression) + 1;
    schemerlicht_expression* last = schemerlicht_vector_end(&e->expr.beg.arguments, schemerlicht_expression);
    schemerlicht_vector_insert(ctxt, &remainder.expr.beg.arguments, &insert_pos, &first, &last, schemerlicht_expression);
    schemerlicht_vector_erase_range(&e->expr.beg.arguments, &first, &last, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &e->expr.beg.arguments, remainder, schemerlicht_expression);
    cps_conversion_state st = init_conversion_state(e, state_step_1);
    schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);
    cps_conversion_state st2 = init_conversion_state(schemerlicht_vector_back(&e->expr.beg.arguments, schemerlicht_expression), state_init);
    schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st2, cps_conversion_state);
    }
  }

static void cps_convert_begin_nonsimple_step1(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  schemerlicht_assert(e->type == schemerlicht_type_begin);
  schemerlicht_memsize idx_val = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) + 1;
  schemerlicht_vector_push_back(ctxt, &cps->index, idx_val, schemerlicht_memsize);
  schemerlicht_expression l = schemerlicht_init_lambda(ctxt);
  schemerlicht_expression* e2 = schemerlicht_vector_at(&e->expr.beg.arguments, 1, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &l.expr.lambda.variables, make_var_name(ctxt, idx_val), schemerlicht_string);
  if (e2->type == schemerlicht_type_begin)
    {
    schemerlicht_vector_push_back(ctxt, &l.expr.lambda.body, *e2, schemerlicht_expression);
    }
  else
    {
    schemerlicht_expression lb = schemerlicht_init_begin(ctxt);
    schemerlicht_vector_push_back(ctxt, &lb.expr.beg.arguments, *e2, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &l.expr.lambda.body, lb, schemerlicht_expression);
    //this scenario is triggered by CHECK_EQUAL("130", run("(letrec([f 12][g(lambda(n) (set! f n))])(g 130) f) "));
    }
  schemerlicht_vector_push_back(ctxt, &cps->continuation, l, schemerlicht_expression);
  schemerlicht_assert(continuation_is_valid(ctxt, get_continuation(cps)));
  schemerlicht_expression arg = *schemerlicht_vector_at(&e->expr.beg.arguments, 0, schemerlicht_expression);

  schemerlicht_vector_destroy(ctxt, &e->expr.beg.arguments);
  schemerlicht_string_destroy(ctxt, &e->expr.beg.filename);

  *e = arg;
  cps_conversion_state st = init_conversion_state(NULL, state_step_clean_up);
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);
  cps_conversion_state st2 = init_conversion_state(e, state_init);
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st2, cps_conversion_state);
  }

static void cps_clean_up(cps_conversion_helper* cps)
  {
  schemerlicht_memsize ind = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize);
  schemerlicht_vector_pop_back(&cps->index);
  *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) = ind;
  schemerlicht_vector_pop_back(&cps->continuation);
  }

static void cps_convert_begin(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  if (is_simple(ctxt, e) || e->expr.beg.arguments.vector_size == 0)
    cps_convert_begin_simple(ctxt, e, cps);
  else if (e->expr.beg.arguments.vector_size == 1)
    {
    cps_conversion_state st = init_conversion_state(schemerlicht_vector_at(&e->expr.beg.arguments, 0, schemerlicht_expression), state_init);
    schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);
    }
  else
    cps_convert_begin_nonsimple(ctxt, e, cps);
  }


static void cps_convert_lambda(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  schemerlicht_assert(e->type == schemerlicht_type_lambda);
  schemerlicht_memsize idx_val = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) + 1;
  schemerlicht_vector_push_back(ctxt, &cps->index, idx_val, schemerlicht_memsize);
  schemerlicht_expression v = schemerlicht_init_variable(ctxt);
  v.expr.var.name = make_var_name(ctxt, idx_val);
  schemerlicht_vector_push_back(ctxt, &cps->continuation, v, schemerlicht_expression);
  schemerlicht_assert(continuation_is_valid(ctxt, get_continuation(cps)));

  cps_conversion_state st = init_conversion_state(e, state_step_1);
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);

  cps_conversion_state st2 = init_conversion_state(schemerlicht_vector_at(&e->expr.lambda.body, 0, schemerlicht_expression), state_init);
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st2, cps_conversion_state);
  }

static void cps_convert_lambda_step1(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  schemerlicht_assert(e->type == schemerlicht_type_lambda);
  schemerlicht_memsize ind = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize);
  schemerlicht_vector_pop_back(&cps->index);
  schemerlicht_string k_name = make_var_name(ctxt, *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) + 1);
  *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) = ind;
  schemerlicht_vector_pop_back(&cps->continuation);
  schemerlicht_vector_push_front(ctxt, &e->expr.lambda.variables, k_name, schemerlicht_string);

  if (continuation_is_lambda_with_one_parameter_without_free_vars(get_continuation(cps)))
    {
    schemerlicht_expression* lam = get_continuation(cps);
    schemerlicht_expression let = schemerlicht_init_let(ctxt);
    schemerlicht_let_binding bind;
    bind.binding_name = *schemerlicht_vector_at(&lam->expr.lambda.variables, 0, schemerlicht_string);
    bind.binding_expr = *e;
    schemerlicht_vector_push_back(ctxt, &let.expr.let.bindings, bind, schemerlicht_let_binding);
    schemerlicht_vector_push_back(ctxt, &let.expr.let.body, *schemerlicht_vector_at(&lam->expr.lambda.body, 0, schemerlicht_expression), schemerlicht_expression);
    *e = let;
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.assignable_variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.free_variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.body);
    schemerlicht_string_destroy(ctxt, &lam->expr.lambda.filename);
    }
  else
    {
    schemerlicht_expression* cont = get_continuation(cps);
    schemerlicht_expression f = schemerlicht_init_funcall(ctxt);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.fun, *cont, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.arguments, *e, schemerlicht_expression);
    *e = f;
    }
  }

static void cps_convert_let_nonsimple_step1(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  schemerlicht_assert(e->type == schemerlicht_type_let);
  cps_conversion_state st = init_conversion_state(e, state_step_2);
  st.var_index = e->expr.let.bindings.vector_size - 1;
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);
  }

static void cps_convert_let_nonsimple_step2(schemerlicht_context* ctxt, cps_conversion_state* state, cps_conversion_helper* cps)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_let);

  schemerlicht_let_binding* bind = schemerlicht_vector_at(&state->expr->expr.let.bindings, state->var_index, schemerlicht_let_binding);
  schemerlicht_expression lam = schemerlicht_init_lambda(ctxt);
  schemerlicht_vector_push_back(ctxt, &lam.expr.lambda.variables, bind->binding_name, schemerlicht_string);
  schemerlicht_expression* body = schemerlicht_vector_at(&state->expr->expr.let.body, 0, schemerlicht_expression);
  if (body->type == schemerlicht_type_begin)
    {
    schemerlicht_vector_push_back(ctxt, &lam.expr.lambda.body, *body, schemerlicht_expression);
    }
  else
    {
    schemerlicht_expression b = schemerlicht_init_begin(ctxt);
    schemerlicht_vector_push_back(ctxt, &b.expr.beg.arguments, *body, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &lam.expr.lambda.body, b, schemerlicht_expression);
    }
  schemerlicht_memsize idx_val = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) + 1;
  schemerlicht_vector_push_back(ctxt, &cps->index, idx_val, schemerlicht_memsize);
  schemerlicht_vector_push_back(ctxt, &cps->continuation, lam, schemerlicht_expression);
  schemerlicht_assert(continuation_is_valid(ctxt, get_continuation(cps)));

  cps_conversion_state st = init_conversion_state(state->expr, state_step_3);
  st.var_index = state->var_index;
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);

  cps_conversion_state st2 = init_conversion_state(&bind->binding_expr, state_init);
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st2, cps_conversion_state);
  }

static void cps_convert_let_nonsimple_step3(schemerlicht_context* ctxt, cps_conversion_state* state, cps_conversion_helper* cps)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_let);
  schemerlicht_let_binding* bind = schemerlicht_vector_at(&state->expr->expr.let.bindings, state->var_index, schemerlicht_let_binding);
  schemerlicht_expression* body = schemerlicht_vector_at(&state->expr->expr.let.body, 0, schemerlicht_expression);
  *body = bind->binding_expr;
  schemerlicht_memsize ind = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize);
  schemerlicht_vector_pop_back(&cps->index);
  *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) = ind;
  schemerlicht_vector_pop_back(&cps->continuation);
  if (state->var_index)
    {
    cps_conversion_state st = init_conversion_state(state->expr, state_step_2);
    st.var_index = state->var_index - 1;
    schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);
    }
  else
    {
    schemerlicht_expression bod = *body;
    schemerlicht_vector_destroy(ctxt, &state->expr->expr.let.assignable_variables);
    schemerlicht_vector_destroy(ctxt, &state->expr->expr.let.bindings);
    schemerlicht_vector_destroy(ctxt, &state->expr->expr.let.body);
    schemerlicht_string_destroy(ctxt, &state->expr->expr.let.filename);
    schemerlicht_string_destroy(ctxt, &state->expr->expr.let.let_name);
    *state->expr = bod;
    }
  }

static void cps_convert_let_nonsimple(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  schemerlicht_assert(e->type == schemerlicht_type_let);
  cps_conversion_state st = init_conversion_state(e, state_step_1);
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);

  cps_conversion_state st2 = init_conversion_state(schemerlicht_vector_at(&e->expr.let.body, 0, schemerlicht_expression), state_init);
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st2, cps_conversion_state);
  }

static void cps_convert_let(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  schemerlicht_assert(e->type == schemerlicht_type_let);
  schemerlicht_vector simple;
  schemerlicht_vector_init_reserve(ctxt, &simple, e->expr.let.bindings.vector_size, int);
  int at_least_one_simple = 0;
  int all_simple = 1;
  for (schemerlicht_memsize j = 0; j < e->expr.let.bindings.vector_size; ++j)
    {
    schemerlicht_let_binding* b = schemerlicht_vector_at(&e->expr.let.bindings, j, schemerlicht_let_binding);
    int si = is_simple(ctxt, &b->binding_expr);
    at_least_one_simple |= si;
    all_simple &= si;
    schemerlicht_vector_push_back(ctxt, &simple, si, int);
    }
  if (e->expr.let.bindings.vector_size == 0)
    {
    at_least_one_simple = 1;
    all_simple = 1;
    }
  if (!at_least_one_simple)
    cps_convert_let_nonsimple(ctxt, e, cps);
  else if (all_simple)
    {
    cps_conversion_state st = init_conversion_state(schemerlicht_vector_at(&e->expr.let.body, 0, schemerlicht_expression), state_init);
    schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);
    }
  else
    {
    schemerlicht_vector simple_bindings;
    schemerlicht_vector nonsimple_bindings;
    schemerlicht_vector_init(ctxt, &simple_bindings, schemerlicht_let_binding);
    schemerlicht_vector_init(ctxt, &nonsimple_bindings, schemerlicht_let_binding);
    for (schemerlicht_memsize j = 0; j < e->expr.let.bindings.vector_size; ++j)
      {
      int si = *schemerlicht_vector_at(&simple, j, int);
      if (si)
        {
        schemerlicht_vector_push_back(ctxt, &simple_bindings, *schemerlicht_vector_at(&e->expr.let.bindings, j, schemerlicht_let_binding), schemerlicht_let_binding);
        }
      else
        {
        schemerlicht_vector_push_back(ctxt, &nonsimple_bindings, *schemerlicht_vector_at(&e->expr.let.bindings, j, schemerlicht_let_binding), schemerlicht_let_binding);
        }
      }
    schemerlicht_expression new_let = schemerlicht_init_let(ctxt);
    schemerlicht_swap(new_let.expr.let.bindings, nonsimple_bindings, schemerlicht_vector);
    schemerlicht_swap(e->expr.let.body, new_let.expr.let.body, schemerlicht_vector);
    schemerlicht_expression new_begin = schemerlicht_init_begin(ctxt);
    schemerlicht_vector_push_back(ctxt, &new_begin.expr.beg.arguments, new_let, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &e->expr.let.body, new_begin, schemerlicht_expression);
    schemerlicht_swap(e->expr.let.bindings, simple_bindings, schemerlicht_vector);
    schemerlicht_vector_destroy(ctxt, &simple_bindings);
    schemerlicht_vector_destroy(ctxt, &nonsimple_bindings);
    cps_convert_let_nonsimple(ctxt, schemerlicht_vector_at(&schemerlicht_vector_at(&e->expr.let.body, 0, schemerlicht_expression)->expr.beg.arguments, 0, schemerlicht_expression), cps);
    }

  schemerlicht_vector_destroy(ctxt, &simple);
  }

static void cps_convert_if(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  schemerlicht_assert(e->type == schemerlicht_type_if);
  schemerlicht_assert(e->expr.i.arguments.vector_size == 3);

  schemerlicht_expression* first = schemerlicht_vector_at(&e->expr.i.arguments, 0, schemerlicht_expression);

  if (!is_simple(ctxt, first))
    {
    cps_conversion_state st = init_conversion_state(e, state_step_1);
    schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);
    }

  schemerlicht_expression* cont = get_continuation(cps);
  schemerlicht_expression cont_copy = schemerlicht_expression_copy(ctxt, cont);
  schemerlicht_vector_push_back(ctxt, &cps->continuation, cont_copy, schemerlicht_expression);

  cps_conversion_state st2 = init_conversion_state(schemerlicht_vector_at(&e->expr.i.arguments, 1, schemerlicht_expression), state_init);
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st2, cps_conversion_state);

  cps_conversion_state st4 = init_conversion_state(e, state_step_2); // step 2 will pop the copy of the continuation
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st4, cps_conversion_state);

  cps_conversion_state st3 = init_conversion_state(schemerlicht_vector_at(&e->expr.i.arguments, 2, schemerlicht_expression), state_init);
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st3, cps_conversion_state);
  }

static void cps_convert_if_step2(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  UNUSED(ctxt);
  UNUSED(e);
  schemerlicht_assert(e->type == schemerlicht_type_if);
  schemerlicht_vector_pop_back(&cps->continuation); // pop our local copy of the continuation
  }

static void cps_convert_if_step1(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  schemerlicht_assert(e->type == schemerlicht_type_if);
  //schemerlicht_expression* first = schemerlicht_vector_at(&e->expr.i.arguments, 0, schemerlicht_expression);

  schemerlicht_memsize idx_val = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) + 1;
  schemerlicht_vector_push_back(ctxt, &cps->index, idx_val, schemerlicht_memsize);

  schemerlicht_expression l = schemerlicht_init_lambda(ctxt);
  schemerlicht_vector_push_back(ctxt, &l.expr.lambda.variables, make_var_name(ctxt, idx_val), schemerlicht_string);

  schemerlicht_expression v = schemerlicht_init_variable(ctxt);
  v.expr.var.name = make_var_name(ctxt, idx_val);

  schemerlicht_expression exp0 = *schemerlicht_vector_at(&e->expr.i.arguments, 0, schemerlicht_expression);
  *schemerlicht_vector_at(&e->expr.i.arguments, 0, schemerlicht_expression) = v;

  schemerlicht_expression b = schemerlicht_init_begin(ctxt);
  schemerlicht_vector_push_back(ctxt, &b.expr.beg.arguments, *e, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &l.expr.lambda.body, b, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &cps->continuation, l, schemerlicht_expression);
  schemerlicht_assert(continuation_is_valid(ctxt, get_continuation(cps)));

  *e = exp0;

  cps_conversion_state st = init_conversion_state(NULL, state_step_clean_up);
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);
  cps_conversion_state st2 = init_conversion_state(e, state_init);
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st2, cps_conversion_state);
  }

static void cps_convert_prim_simple(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  schemerlicht_assert(e->type == schemerlicht_type_primitive_call);
  if (continuation_is_lambda_with_one_parameter_without_free_vars(get_continuation(cps)))
    {
    schemerlicht_expression* lam = get_continuation(cps);
    schemerlicht_expression l = schemerlicht_init_let(ctxt);
    schemerlicht_let_binding bind;
    bind.binding_name = *schemerlicht_vector_at(&lam->expr.lambda.variables, 0, schemerlicht_string);
    bind.binding_expr = *e;
    schemerlicht_vector_push_back(ctxt, &l.expr.let.bindings, bind, schemerlicht_let_binding);
    schemerlicht_vector_push_back(ctxt, &l.expr.let.body, *schemerlicht_vector_at(&lam->expr.lambda.body, 0, schemerlicht_expression), schemerlicht_expression);
    *e = l;
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.assignable_variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.free_variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.body);
    schemerlicht_string_destroy(ctxt, &lam->expr.lambda.filename);
    }
  else
    {
    schemerlicht_expression* cont = get_continuation(cps);
    schemerlicht_expression f = schemerlicht_init_funcall(ctxt);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.fun, *cont, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.arguments, *e, schemerlicht_expression);
    *e = f;
    }
  }

static void cps_convert_prim_nonsimple(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  schemerlicht_assert(e->type == schemerlicht_type_primitive_call);
  if (e->expr.prim.arguments.vector_size == 0)
    {
    cps_convert_prim_simple(ctxt, e, cps);
    return;
    }
  schemerlicht_vector nonsimple_vars;
  schemerlicht_vector simple;
  schemerlicht_vector_init(ctxt, &nonsimple_vars, non_simple_var_struct);
  schemerlicht_vector_init_reserve(ctxt, &simple, e->expr.prim.arguments.vector_size, int);
  for (schemerlicht_memsize j = 0; j < e->expr.prim.arguments.vector_size; ++j)
    {
    schemerlicht_expression* arg = schemerlicht_vector_at(&e->expr.prim.arguments, j, schemerlicht_expression);
    int si = is_simple(ctxt, arg);
    schemerlicht_vector_push_back(ctxt, &simple, si, int);
    if (!si)
      {
      non_simple_var_struct ns;
      ns.index = j;
      ns.name = make_var_name(ctxt, *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) + j + 1);
      schemerlicht_vector_push_back(ctxt, &nonsimple_vars, ns, non_simple_var_struct);
      }
    }
  *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) += e->expr.prim.arguments.vector_size;

  schemerlicht_expression bottom_l = schemerlicht_init_lambda(ctxt);
  schemerlicht_string lambda_var_name;
  schemerlicht_string_copy(ctxt, &lambda_var_name, &schemerlicht_vector_back(&nonsimple_vars, non_simple_var_struct)->name);
  schemerlicht_vector_push_back(ctxt, &bottom_l.expr.lambda.variables, lambda_var_name, schemerlicht_string);
  schemerlicht_expression bottom_b = schemerlicht_init_begin(ctxt);
  schemerlicht_expression bottom_p = schemerlicht_init_primcall(ctxt);
  schemerlicht_string_destroy(ctxt, &bottom_p.expr.prim.name);
  bottom_p.expr.prim.name = e->expr.prim.name;
  for (schemerlicht_memsize j = 0; j < e->expr.prim.arguments.vector_size; ++j)
    {
    if (*schemerlicht_vector_at(&simple, j, int))
      {
      schemerlicht_vector_push_back(ctxt, &bottom_p.expr.prim.arguments, *schemerlicht_vector_at(&e->expr.prim.arguments, j, schemerlicht_expression), schemerlicht_expression);
      }
    else
      {
      schemerlicht_expression v = schemerlicht_init_variable(ctxt);
      //schemerlicht_string_destroy(ctxt, &v.expr.var.name);
      non_simple_var_struct* it = schemerlicht_vector_begin(&nonsimple_vars, non_simple_var_struct);
      non_simple_var_struct* it_end = schemerlicht_vector_end(&nonsimple_vars, non_simple_var_struct);
      for (; it != it_end; ++it)
        {
        if (it->index == j)
          {
          schemerlicht_string_copy(ctxt, &v.expr.var.name, &it->name);
          break;
          }
        }
      schemerlicht_vector_push_back(ctxt, &bottom_p.expr.prim.arguments, v, schemerlicht_expression);
      }
    }

  if (continuation_is_lambda_with_one_parameter_without_free_vars(get_continuation(cps)))
    {
    schemerlicht_expression* lam = get_continuation(cps);
    schemerlicht_expression l = schemerlicht_init_let(ctxt);
    schemerlicht_let_binding bind;
    bind.binding_name = *schemerlicht_vector_at(&lam->expr.lambda.variables, 0, schemerlicht_string);
    bind.binding_expr = bottom_p;
    schemerlicht_vector_push_back(ctxt, &l.expr.let.bindings, bind, schemerlicht_let_binding);
    schemerlicht_vector_push_back(ctxt, &l.expr.let.body, *schemerlicht_vector_at(&lam->expr.lambda.body, 0, schemerlicht_expression), schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &bottom_b.expr.beg.arguments, l, schemerlicht_expression);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.assignable_variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.free_variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.body);
    schemerlicht_string_destroy(ctxt, &lam->expr.lambda.filename);
    }
  else
    {
    schemerlicht_expression* cont = get_continuation(cps);
    schemerlicht_expression f = schemerlicht_init_funcall(ctxt);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.fun, *cont, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.arguments, bottom_p, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &bottom_b.expr.beg.arguments, f, schemerlicht_expression);
    }
  schemerlicht_vector_push_back(ctxt, &bottom_l.expr.lambda.body, bottom_b, schemerlicht_expression);
  schemerlicht_memsize idx_val = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) + 1;
  schemerlicht_vector_push_back(ctxt, &cps->index, idx_val, schemerlicht_memsize);
  schemerlicht_vector_push_back(ctxt, &cps->continuation, bottom_l, schemerlicht_expression);
  schemerlicht_assert(continuation_is_valid(ctxt, get_continuation(cps)));
  cps_conversion_state st = init_conversion_state(e, state_step_1);
  st.nonsimple_vars = nonsimple_vars;
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);

  cps_conversion_state st2 = init_conversion_state(schemerlicht_vector_at(&e->expr.prim.arguments, schemerlicht_vector_back(&nonsimple_vars, non_simple_var_struct)->index, schemerlicht_expression), state_init);
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st2, cps_conversion_state);

  schemerlicht_vector_destroy(ctxt, &simple);
  }

static void cps_convert_prim_nonsimple_step1(schemerlicht_context* ctxt, cps_conversion_state* state, cps_conversion_helper* cps)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_primitive_call);
  cps_conversion_state st = init_conversion_state(state->expr, state_step_3);
  st.nonsimple_vars = state->nonsimple_vars;
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);

  if (st.nonsimple_vars.vector_size > 1)
    {
    cps_conversion_state st2 = init_conversion_state(state->expr, state_step_2);
    st2.nonsimple_vars = state->nonsimple_vars;
    st2.var_index = st.nonsimple_vars.vector_size - 2;
    schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st2, cps_conversion_state);
    }
  }

static void cps_convert_prim_nonsimple_step2(schemerlicht_context* ctxt, cps_conversion_state* state, cps_conversion_helper* cps)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_primitive_call);
  if (state->var_index > 0)
    {
    cps_conversion_state st = init_conversion_state(state->expr, state_step_2);
    st.nonsimple_vars = state->nonsimple_vars;
    st.var_index = state->var_index - 1;
    schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);
    }

  schemerlicht_memsize ind = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize);
  schemerlicht_vector_pop_back(&cps->index);
  *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) = ind;
  schemerlicht_vector_pop_back(&cps->continuation);

  non_simple_var_struct* it = schemerlicht_vector_begin(&state->nonsimple_vars, non_simple_var_struct) + state->var_index;
  non_simple_var_struct* prev_it = it + 1;
  schemerlicht_string name;
  schemerlicht_string_copy(ctxt, &name, &it->name);

  schemerlicht_expression l = schemerlicht_init_lambda(ctxt);
  schemerlicht_vector_push_back(ctxt, &l.expr.lambda.variables, name, schemerlicht_string);
  schemerlicht_expression b = schemerlicht_init_begin(ctxt);
  schemerlicht_vector_push_back(ctxt, &b.expr.beg.arguments, *schemerlicht_vector_at(&state->expr->expr.prim.arguments, prev_it->index, schemerlicht_expression), schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &l.expr.lambda.body, b, schemerlicht_expression);

  schemerlicht_memsize idx_val = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) + 1;
  schemerlicht_vector_push_back(ctxt, &cps->index, idx_val, schemerlicht_memsize);
  schemerlicht_vector_push_back(ctxt, &cps->continuation, l, schemerlicht_expression);
  schemerlicht_assert(continuation_is_valid(ctxt, get_continuation(cps)));
  cps_conversion_state st = init_conversion_state(schemerlicht_vector_at(&state->expr->expr.prim.arguments, it->index, schemerlicht_expression), state_init);
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);
  }

static void cps_convert_prim_nonsimple_step3(schemerlicht_context* ctxt, cps_conversion_state* state, cps_conversion_helper* cps)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_primitive_call);
  schemerlicht_memsize ind = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize);
  schemerlicht_vector_pop_back(&cps->index);
  *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) = ind;
  schemerlicht_vector_pop_back(&cps->continuation);
  schemerlicht_expression e = *schemerlicht_vector_at(&state->expr->expr.prim.arguments, schemerlicht_vector_at(&state->nonsimple_vars, 0, non_simple_var_struct)->index, schemerlicht_expression);
  schemerlicht_string_destroy(ctxt, &state->expr->expr.prim.filename);
  schemerlicht_vector_destroy(ctxt, &state->expr->expr.prim.arguments);
  *state->expr = e;
  non_simple_var_struct* it = schemerlicht_vector_begin(&state->nonsimple_vars, non_simple_var_struct);
  non_simple_var_struct* it_end = schemerlicht_vector_end(&state->nonsimple_vars, non_simple_var_struct);
  for (; it != it_end; ++it)
    schemerlicht_string_destroy(ctxt, &it->name);
  schemerlicht_vector_destroy(ctxt, &state->nonsimple_vars);
  }

static void cps_convert_prim(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  if (is_simple(ctxt, e))
    cps_convert_prim_simple(ctxt, e, cps);
  else
    cps_convert_prim_nonsimple(ctxt, e, cps);
  }

static void cps_convert_foreign_simple(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  schemerlicht_assert(e->type == schemerlicht_type_foreign_call);
  if (continuation_is_lambda_with_one_parameter_without_free_vars(get_continuation(cps)))
    {
    schemerlicht_expression* lam = get_continuation(cps);
    schemerlicht_expression l = schemerlicht_init_let(ctxt);
    schemerlicht_let_binding bind;
    bind.binding_name = *schemerlicht_vector_at(&lam->expr.lambda.variables, 0, schemerlicht_string);
    bind.binding_expr = *e;
    schemerlicht_vector_push_back(ctxt, &l.expr.let.bindings, bind, schemerlicht_let_binding);
    schemerlicht_vector_push_back(ctxt, &l.expr.let.body, *schemerlicht_vector_at(&lam->expr.lambda.body, 0, schemerlicht_expression), schemerlicht_expression);
    *e = l;
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.assignable_variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.free_variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.body);
    schemerlicht_string_destroy(ctxt, &lam->expr.lambda.filename);
    }
  else
    {
    schemerlicht_expression* cont = get_continuation(cps);
    schemerlicht_expression f = schemerlicht_init_funcall(ctxt);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.fun, *cont, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.arguments, *e, schemerlicht_expression);
    *e = f;
    }
  }

static void cps_convert_foreign_nonsimple(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  schemerlicht_assert(e->type == schemerlicht_type_foreign_call);
  if (e->expr.foreign.arguments.vector_size == 0)
    {
    cps_convert_foreign_simple(ctxt, e, cps);
    return;
    }
  schemerlicht_vector nonsimple_vars;
  schemerlicht_vector simple;
  schemerlicht_vector_init(ctxt, &nonsimple_vars, non_simple_var_struct);
  schemerlicht_vector_init_reserve(ctxt, &simple, e->expr.foreign.arguments.vector_size, int);
  for (schemerlicht_memsize j = 0; j < e->expr.foreign.arguments.vector_size; ++j)
    {
    schemerlicht_expression* arg = schemerlicht_vector_at(&e->expr.foreign.arguments, j, schemerlicht_expression);
    int si = is_simple(ctxt, arg);
    schemerlicht_vector_push_back(ctxt, &simple, si, int);
    if (!si)
      {
      non_simple_var_struct ns;
      ns.index = j;
      ns.name = make_var_name(ctxt, *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) + j + 1);
      schemerlicht_vector_push_back(ctxt, &nonsimple_vars, ns, non_simple_var_struct);
      }
    }
  *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) += e->expr.foreign.arguments.vector_size;

  schemerlicht_expression bottom_l = schemerlicht_init_lambda(ctxt);
  schemerlicht_string lambda_var_name;
  schemerlicht_string_copy(ctxt, &lambda_var_name, &schemerlicht_vector_back(&nonsimple_vars, non_simple_var_struct)->name);
  schemerlicht_vector_push_back(ctxt, &bottom_l.expr.lambda.variables, lambda_var_name, schemerlicht_string);
  schemerlicht_expression bottom_b = schemerlicht_init_begin(ctxt);
  schemerlicht_expression bottom_p = schemerlicht_init_foreign(ctxt);
  schemerlicht_string_destroy(ctxt, &bottom_p.expr.foreign.name);
  bottom_p.expr.foreign.name = e->expr.foreign.name;
  for (schemerlicht_memsize j = 0; j < e->expr.foreign.arguments.vector_size; ++j)
    {
    if (*schemerlicht_vector_at(&simple, j, int))
      {
      schemerlicht_vector_push_back(ctxt, &bottom_p.expr.foreign.arguments, *schemerlicht_vector_at(&e->expr.foreign.arguments, j, schemerlicht_expression), schemerlicht_expression);
      }
    else
      {
      schemerlicht_expression v = schemerlicht_init_variable(ctxt);
      //schemerlicht_string_destroy(ctxt, &v.expr.var.name);
      non_simple_var_struct* it = schemerlicht_vector_begin(&nonsimple_vars, non_simple_var_struct);
      non_simple_var_struct* it_end = schemerlicht_vector_end(&nonsimple_vars, non_simple_var_struct);
      for (; it != it_end; ++it)
        {
        if (it->index == j)
          {
          schemerlicht_string_copy(ctxt, &v.expr.var.name, &it->name);
          break;
          }
        }
      schemerlicht_vector_push_back(ctxt, &bottom_p.expr.foreign.arguments, v, schemerlicht_expression);
      }
    }

  if (continuation_is_lambda_with_one_parameter_without_free_vars(get_continuation(cps)))
    {
    schemerlicht_expression* lam = get_continuation(cps);
    schemerlicht_expression l = schemerlicht_init_let(ctxt);
    schemerlicht_let_binding bind;
    bind.binding_name = *schemerlicht_vector_at(&lam->expr.lambda.variables, 0, schemerlicht_string);
    bind.binding_expr = bottom_p;
    schemerlicht_vector_push_back(ctxt, &l.expr.let.bindings, bind, schemerlicht_let_binding);
    schemerlicht_vector_push_back(ctxt, &l.expr.let.body, *schemerlicht_vector_at(&lam->expr.lambda.body, 0, schemerlicht_expression), schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &bottom_b.expr.beg.arguments, l, schemerlicht_expression);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.assignable_variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.free_variables);
    schemerlicht_vector_destroy(ctxt, &lam->expr.lambda.body);
    schemerlicht_string_destroy(ctxt, &lam->expr.lambda.filename);
    }
  else
    {
    schemerlicht_expression* cont = get_continuation(cps);
    schemerlicht_expression f = schemerlicht_init_funcall(ctxt);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.fun, *cont, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.arguments, bottom_p, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &bottom_b.expr.beg.arguments, f, schemerlicht_expression);
    }
  schemerlicht_vector_push_back(ctxt, &bottom_l.expr.lambda.body, bottom_b, schemerlicht_expression);
  schemerlicht_memsize idx_val = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) + 1;
  schemerlicht_vector_push_back(ctxt, &cps->index, idx_val, schemerlicht_memsize);
  schemerlicht_vector_push_back(ctxt, &cps->continuation, bottom_l, schemerlicht_expression);
  schemerlicht_assert(continuation_is_valid(ctxt, get_continuation(cps)));
  cps_conversion_state st = init_conversion_state(e, state_step_1);
  st.nonsimple_vars = nonsimple_vars;
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);

  cps_conversion_state st2 = init_conversion_state(schemerlicht_vector_at(&e->expr.foreign.arguments, schemerlicht_vector_back(&nonsimple_vars, non_simple_var_struct)->index, schemerlicht_expression), state_init);
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st2, cps_conversion_state);

  schemerlicht_vector_destroy(ctxt, &simple);
  }

static void cps_convert_foreign_nonsimple_step1(schemerlicht_context* ctxt, cps_conversion_state* state, cps_conversion_helper* cps)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_foreign_call);
  cps_conversion_state st = init_conversion_state(state->expr, state_step_3);
  st.nonsimple_vars = state->nonsimple_vars;
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);

  if (st.nonsimple_vars.vector_size > 1)
    {
    cps_conversion_state st2 = init_conversion_state(state->expr, state_step_2);
    st2.nonsimple_vars = state->nonsimple_vars;
    st2.var_index = st.nonsimple_vars.vector_size - 2;
    schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st2, cps_conversion_state);
    }
  }

static void cps_convert_foreign_nonsimple_step2(schemerlicht_context* ctxt, cps_conversion_state* state, cps_conversion_helper* cps)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_foreign_call);
  if (state->var_index > 0)
    {
    cps_conversion_state st = init_conversion_state(state->expr, state_step_2);
    st.nonsimple_vars = state->nonsimple_vars;
    st.var_index = state->var_index - 1;
    schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);
    }

  schemerlicht_memsize ind = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize);
  schemerlicht_vector_pop_back(&cps->index);
  *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) = ind;
  schemerlicht_vector_pop_back(&cps->continuation);

  non_simple_var_struct* it = schemerlicht_vector_begin(&state->nonsimple_vars, non_simple_var_struct) + state->var_index;
  non_simple_var_struct* prev_it = it + 1;
  schemerlicht_string name;
  schemerlicht_string_copy(ctxt, &name, &it->name);

  schemerlicht_expression l = schemerlicht_init_lambda(ctxt);
  schemerlicht_vector_push_back(ctxt, &l.expr.lambda.variables, name, schemerlicht_string);
  schemerlicht_expression b = schemerlicht_init_begin(ctxt);
  schemerlicht_vector_push_back(ctxt, &b.expr.beg.arguments, *schemerlicht_vector_at(&state->expr->expr.foreign.arguments, prev_it->index, schemerlicht_expression), schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &l.expr.lambda.body, b, schemerlicht_expression);

  schemerlicht_memsize idx_val = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) + 1;
  schemerlicht_vector_push_back(ctxt, &cps->index, idx_val, schemerlicht_memsize);
  schemerlicht_vector_push_back(ctxt, &cps->continuation, l, schemerlicht_expression);
  schemerlicht_assert(continuation_is_valid(ctxt, get_continuation(cps)));
  cps_conversion_state st = init_conversion_state(schemerlicht_vector_at(&state->expr->expr.foreign.arguments, it->index, schemerlicht_expression), state_init);
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);
  }

static void cps_convert_foreign_nonsimple_step3(schemerlicht_context* ctxt, cps_conversion_state* state, cps_conversion_helper* cps)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_foreign_call);
  schemerlicht_memsize ind = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize);
  schemerlicht_vector_pop_back(&cps->index);
  *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) = ind;
  schemerlicht_vector_pop_back(&cps->continuation);
  schemerlicht_expression e = *schemerlicht_vector_at(&state->expr->expr.foreign.arguments, schemerlicht_vector_at(&state->nonsimple_vars, 0, non_simple_var_struct)->index, schemerlicht_expression);
  schemerlicht_string_destroy(ctxt, &state->expr->expr.foreign.filename);
  schemerlicht_vector_destroy(ctxt, &state->expr->expr.foreign.arguments);
  *state->expr = e;
  non_simple_var_struct* it = schemerlicht_vector_begin(&state->nonsimple_vars, non_simple_var_struct);
  non_simple_var_struct* it_end = schemerlicht_vector_end(&state->nonsimple_vars, non_simple_var_struct);
  for (; it != it_end; ++it)
    schemerlicht_string_destroy(ctxt, &it->name);
  schemerlicht_vector_destroy(ctxt, &state->nonsimple_vars);
  }

static void cps_convert_foreign(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  if (is_simple(ctxt, e))
    cps_convert_foreign_simple(ctxt, e, cps);
  else
    cps_convert_foreign_nonsimple(ctxt, e, cps);
  }










static void cps_convert_funcall(schemerlicht_context* ctxt, schemerlicht_expression* e, cps_conversion_helper* cps)
  {
  schemerlicht_assert(e->type == schemerlicht_type_funcall);

  schemerlicht_vector nonsimple_vars;
  schemerlicht_vector simple;
  schemerlicht_vector_init(ctxt, &nonsimple_vars, non_simple_var_struct);
  schemerlicht_vector_init_reserve(ctxt, &simple, e->expr.funcall.arguments.vector_size, int);

  schemerlicht_string r0 = make_var_name(ctxt, *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) + 1);

  for (schemerlicht_memsize j = 0; j < e->expr.funcall.arguments.vector_size; ++j)
    {
    schemerlicht_expression* arg = schemerlicht_vector_at(&e->expr.funcall.arguments, j, schemerlicht_expression);
    int si = is_simple(ctxt, arg);
    schemerlicht_vector_push_back(ctxt, &simple, si, int);
    if (!si)
      {
      non_simple_var_struct ns;
      ns.index = j;
      ns.name = make_var_name(ctxt, *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) + j + 2);
      schemerlicht_vector_push_back(ctxt, &nonsimple_vars, ns, non_simple_var_struct);
      }
    }
  *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) += e->expr.funcall.arguments.vector_size + 1;

  schemerlicht_expression bottom_l = schemerlicht_init_lambda(ctxt);
  schemerlicht_string lambda_var_name;
  if (nonsimple_vars.vector_size == 0)
    {
    schemerlicht_string_copy(ctxt, &lambda_var_name, &r0);
    }
  else
    {
    schemerlicht_string_copy(ctxt, &lambda_var_name, &schemerlicht_vector_back(&nonsimple_vars, non_simple_var_struct)->name);
    }
  schemerlicht_vector_push_back(ctxt, &bottom_l.expr.lambda.variables, lambda_var_name, schemerlicht_string);
  schemerlicht_expression bottom_b = schemerlicht_init_begin(ctxt);
  schemerlicht_expression bottom_f = schemerlicht_init_funcall(ctxt);
  schemerlicht_expression bottom_v = schemerlicht_init_variable(ctxt);
  schemerlicht_string_destroy(ctxt, &bottom_v.expr.var.name);
  schemerlicht_string_copy(ctxt, &bottom_v.expr.var.name, &r0);
  schemerlicht_vector_push_back(ctxt, &bottom_f.expr.funcall.fun, bottom_v, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &bottom_f.expr.funcall.arguments, *schemerlicht_vector_back(&cps->continuation, schemerlicht_expression), schemerlicht_expression);
  for (schemerlicht_memsize j = 0; j < e->expr.funcall.arguments.vector_size; ++j)
    {
    if (*schemerlicht_vector_at(&simple, j, int))
      {
      schemerlicht_vector_push_back(ctxt, &bottom_f.expr.funcall.arguments, *schemerlicht_vector_at(&e->expr.funcall.arguments, j, schemerlicht_expression), schemerlicht_expression);
      }
    else
      {
      schemerlicht_expression v = schemerlicht_init_variable(ctxt);
      //schemerlicht_string_destroy(ctxt, &v.expr.var.name);
      non_simple_var_struct* it = schemerlicht_vector_begin(&nonsimple_vars, non_simple_var_struct);
      non_simple_var_struct* it_end = schemerlicht_vector_end(&nonsimple_vars, non_simple_var_struct);
      for (; it != it_end; ++it)
        {
        if (it->index == j)
          {
          schemerlicht_string_copy(ctxt, &v.expr.var.name, &it->name);
          break;
          }
        }
      schemerlicht_vector_push_back(ctxt, &bottom_f.expr.funcall.arguments, v, schemerlicht_expression);
      }
    }

  schemerlicht_vector_push_back(ctxt, &bottom_b.expr.beg.arguments, bottom_f, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &bottom_l.expr.lambda.body, bottom_b, schemerlicht_expression);
  schemerlicht_memsize idx_val = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) + 1;
  schemerlicht_vector_push_back(ctxt, &cps->index, idx_val, schemerlicht_memsize);
  schemerlicht_vector_push_back(ctxt, &cps->continuation, bottom_l, schemerlicht_expression);
  schemerlicht_assert(continuation_is_valid(ctxt, get_continuation(cps)));
  cps_conversion_state st = init_conversion_state(e, state_step_1);
  st.nonsimple_vars = nonsimple_vars;
  st.nonsimple_fun_var = r0;
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);

  cps_conversion_state st2 = init_conversion_state(NULL, state_init);
  if (nonsimple_vars.vector_size == 0)
    {
    st2.expr = schemerlicht_vector_at(&e->expr.funcall.fun, 0, schemerlicht_expression);
    }
  else
    {
    st2.expr = schemerlicht_vector_at(&e->expr.funcall.arguments, schemerlicht_vector_back(&nonsimple_vars, non_simple_var_struct)->index, schemerlicht_expression);
    }
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st2, cps_conversion_state);
  schemerlicht_vector_destroy(ctxt, &simple);
  }

static void cps_convert_funcall_step1(schemerlicht_context* ctxt, cps_conversion_state* state, cps_conversion_helper* cps)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_funcall);
  cps_conversion_state st = init_conversion_state(state->expr, state_step_3);
  st.nonsimple_vars = state->nonsimple_vars;
  st.nonsimple_fun_var = state->nonsimple_fun_var;
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);

  if (st.nonsimple_vars.vector_size > 0)
    {
    cps_conversion_state st2 = init_conversion_state(state->expr, state_step_2);
    st2.nonsimple_vars = state->nonsimple_vars;
    st2.nonsimple_fun_var = state->nonsimple_fun_var;
    st2.var_index = st.nonsimple_vars.vector_size - 1;
    schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st2, cps_conversion_state);
    }
  }

static void cps_convert_funcall_step2(schemerlicht_context* ctxt, cps_conversion_state* state, cps_conversion_helper* cps)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_funcall);
  if (state->var_index > 0)
    {
    cps_conversion_state st = init_conversion_state(state->expr, state_step_2);
    st.nonsimple_vars = state->nonsimple_vars;
    st.nonsimple_fun_var = state->nonsimple_fun_var;
    st.var_index = state->var_index - 1;
    schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);
    }

  schemerlicht_memsize ind = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize);
  schemerlicht_vector_pop_back(&cps->index);
  *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) = ind;
  schemerlicht_vector_pop_back(&cps->continuation);

  non_simple_var_struct* it = schemerlicht_vector_begin(&state->nonsimple_vars, non_simple_var_struct) + state->var_index;
  schemerlicht_string name;
  if (state->var_index > 0)
    {
    non_simple_var_struct* it2 = it - 1;
    schemerlicht_string_copy(ctxt, &name, &it2->name);
    }
  else
    {
    schemerlicht_string_copy(ctxt, &name, &state->nonsimple_fun_var);
    }
  schemerlicht_expression l = schemerlicht_init_lambda(ctxt);
  schemerlicht_vector_push_back(ctxt, &l.expr.lambda.variables, name, schemerlicht_string);
  schemerlicht_expression b = schemerlicht_init_begin(ctxt);
  schemerlicht_vector_push_back(ctxt, &b.expr.beg.arguments, *schemerlicht_vector_at(&state->expr->expr.funcall.arguments, it->index, schemerlicht_expression), schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &l.expr.lambda.body, b, schemerlicht_expression);

  schemerlicht_memsize idx_val = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) + 1;
  schemerlicht_vector_push_back(ctxt, &cps->index, idx_val, schemerlicht_memsize);
  schemerlicht_vector_push_back(ctxt, &cps->continuation, l, schemerlicht_expression);
  schemerlicht_assert(continuation_is_valid(ctxt, get_continuation(cps)));
  cps_conversion_state st = init_conversion_state(NULL, state_init);
  if (state->var_index > 0)
    {
    non_simple_var_struct* it2 = schemerlicht_vector_begin(&state->nonsimple_vars, non_simple_var_struct) + (state->var_index - 1);
    st.expr = schemerlicht_vector_at(&state->expr->expr.funcall.arguments, it2->index, schemerlicht_expression);
    }
  else
    {
    st.expr = schemerlicht_vector_at(&state->expr->expr.funcall.fun, 0, schemerlicht_expression);
    }
  schemerlicht_vector_push_back(ctxt, &cps->expressions_to_treat, st, cps_conversion_state);
  }

static void cps_convert_funcall_step3(schemerlicht_context* ctxt, cps_conversion_state* state, cps_conversion_helper* cps)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_funcall);
  schemerlicht_memsize ind = *schemerlicht_vector_back(&cps->index, schemerlicht_memsize);
  schemerlicht_vector_pop_back(&cps->index);
  *schemerlicht_vector_back(&cps->index, schemerlicht_memsize) = ind;
  schemerlicht_vector_pop_back(&cps->continuation);
  schemerlicht_expression e = *schemerlicht_vector_at(&state->expr->expr.funcall.fun, 0, schemerlicht_expression);
  schemerlicht_string_destroy(ctxt, &state->expr->expr.funcall.filename);
  schemerlicht_vector_destroy(ctxt, &state->expr->expr.funcall.arguments);
  schemerlicht_vector_destroy(ctxt, &state->expr->expr.funcall.fun);
  *state->expr = e;
  non_simple_var_struct* it = schemerlicht_vector_begin(&state->nonsimple_vars, non_simple_var_struct);
  non_simple_var_struct* it_end = schemerlicht_vector_end(&state->nonsimple_vars, non_simple_var_struct);
  for (; it != it_end; ++it)
    schemerlicht_string_destroy(ctxt, &it->name);
  schemerlicht_vector_destroy(ctxt, &state->nonsimple_vars);
  schemerlicht_string_destroy(ctxt, &state->nonsimple_fun_var);
  }

static void treat_cps_state_step_init(schemerlicht_context* ctxt, cps_conversion_state* cps_state, cps_conversion_helper* cps)
  {
  switch (cps_state->expr->type)
    {
    case schemerlicht_type_literal:
    {
    cps_convert_literal_or_variable_or_nop_or_quote(ctxt, cps_state->expr, cps);
    break;
    }
    case schemerlicht_type_variable:
    {
    cps_convert_literal_or_variable_or_nop_or_quote(ctxt, cps_state->expr, cps);
    break;
    }
    case schemerlicht_type_nop:
    {
    cps_convert_literal_or_variable_or_nop_or_quote(ctxt, cps_state->expr, cps);
    break;
    }
    case schemerlicht_type_quote:
    {
    cps_convert_literal_or_variable_or_nop_or_quote(ctxt, cps_state->expr, cps);
    break;
    }
    case schemerlicht_type_primitive_call:
    {
    cps_convert_prim(ctxt, cps_state->expr, cps);
    break;
    }
    case schemerlicht_type_foreign_call:
    {
    cps_convert_foreign(ctxt, cps_state->expr, cps);
    break;
    }
    case schemerlicht_type_funcall:
    {
    cps_convert_funcall(ctxt, cps_state->expr, cps);
    break;
    }
    case schemerlicht_type_set:
    {
    cps_convert_set(ctxt, cps_state->expr, cps);
    break;
    }
    case schemerlicht_type_begin:
    {
    cps_convert_begin(ctxt, cps_state->expr, cps);
    break;
    }
    case schemerlicht_type_lambda:
    {
    cps_convert_lambda(ctxt, cps_state->expr, cps);
    break;
    }
    case schemerlicht_type_let:
    {
    cps_convert_let(ctxt, cps_state->expr, cps);
    break;
    }
    case schemerlicht_type_if:
    {
    cps_convert_if(ctxt, cps_state->expr, cps);
    break;
    }
    default: assert(0); // not implemented
    }
  }

static void treat_cps_state_step_1(schemerlicht_context* ctxt, cps_conversion_state* cps_state, cps_conversion_helper* cps)
  {
  switch (cps_state->expr->type)
    {
    case schemerlicht_type_primitive_call:
    {
    cps_convert_prim_nonsimple_step1(ctxt, cps_state, cps);
    break;
    }
    case schemerlicht_type_foreign_call:
    {
    cps_convert_foreign_nonsimple_step1(ctxt, cps_state, cps);
    break;
    }
    case schemerlicht_type_funcall:
    {
    cps_convert_funcall_step1(ctxt, cps_state, cps);
    break;
    }
    case schemerlicht_type_set:
    {
    cps_convert_set_nonsimple_step1(ctxt, cps_state->expr, cps);
    break;
    }
    case schemerlicht_type_begin:
    {
    cps_convert_begin_nonsimple_step1(ctxt, cps_state->expr, cps);
    break;
    }
    case schemerlicht_type_lambda:
    {
    cps_convert_lambda_step1(ctxt, cps_state->expr, cps);
    break;
    }
    case schemerlicht_type_let:
    {
    cps_convert_let_nonsimple_step1(ctxt, cps_state->expr, cps);
    break;
    }
    case schemerlicht_type_if:
    {
    cps_convert_if_step1(ctxt, cps_state->expr, cps);
    break;
    }
    default: assert(0); // not implemented
    }
  }

static void treat_cps_state_step_2(schemerlicht_context* ctxt, cps_conversion_state* cps_state, cps_conversion_helper* cps)
  {
  switch (cps_state->expr->type)
    {
    case schemerlicht_type_primitive_call:
    {
    cps_convert_prim_nonsimple_step2(ctxt, cps_state, cps);
    break;
    }
    case schemerlicht_type_foreign_call:
    {
    cps_convert_foreign_nonsimple_step2(ctxt, cps_state, cps);
    break;
    }
    case schemerlicht_type_funcall:
    {
    cps_convert_funcall_step2(ctxt, cps_state, cps);
    break;
    }
    case schemerlicht_type_let:
    {
    cps_convert_let_nonsimple_step2(ctxt, cps_state, cps);
    break;
    }
    case schemerlicht_type_if:
    {
    cps_convert_if_step2(ctxt, cps_state->expr, cps);
    break;
    }
    default: assert(0); // not implemented
    }
  }

static void treat_cps_state_step_3(schemerlicht_context* ctxt, cps_conversion_state* cps_state, cps_conversion_helper* cps)
  {
  switch (cps_state->expr->type)
    {
    case schemerlicht_type_primitive_call:
    {
    cps_convert_prim_nonsimple_step3(ctxt, cps_state, cps);
    break;
    }
    case schemerlicht_type_foreign_call:
    {
    cps_convert_foreign_nonsimple_step3(ctxt, cps_state, cps);
    break;
    }
    case schemerlicht_type_funcall:
    {
    cps_convert_funcall_step3(ctxt, cps_state, cps);
    break;
    }
    case schemerlicht_type_let:
    {
    cps_convert_let_nonsimple_step3(ctxt, cps_state, cps);
    break;
    }
    default: assert(0); // not implemented
    }
  }

static void treat_cps_state(schemerlicht_context* ctxt, cps_conversion_state* cps_state, cps_conversion_helper* cps)
  {
  switch (cps_state->state)
    {
    case state_init:
      treat_cps_state_step_init(ctxt, cps_state, cps); break;
    case state_step_1:
      treat_cps_state_step_1(ctxt, cps_state, cps); break;
    case state_step_2:
      treat_cps_state_step_2(ctxt, cps_state, cps); break;
    case state_step_3:
      treat_cps_state_step_3(ctxt, cps_state, cps); break;
    case state_step_clean_up:
      cps_clean_up(cps); break;
    }
  }

static void treat_cps_expressions(schemerlicht_context* ctxt, cps_conversion_helper* cps)
  {
  while (cps->expressions_to_treat.vector_size > 0)
    {
    cps_conversion_state cps_state = *schemerlicht_vector_back(&cps->expressions_to_treat, cps_conversion_state);
    schemerlicht_assert(cps_state.expr == 0 || (cps_state.expr->type >= 0 && cps_state.expr->type <= schemerlicht_type_set));
    schemerlicht_vector_pop_back(&cps->expressions_to_treat);
    treat_cps_state(ctxt, &cps_state, cps);
    }
  }

static void cps_start(schemerlicht_context* ctxt, schemerlicht_expression* e)
  {
  cps_conversion_helper cps;
  schemerlicht_vector_init(ctxt, &cps.index, schemerlicht_memsize);
  schemerlicht_vector_init(ctxt, &cps.expressions_to_treat, cps_conversion_state);
  schemerlicht_vector_init(ctxt, &cps.continuation, schemerlicht_expression);

  schemerlicht_vector_push_back(ctxt, &cps.index, 0, schemerlicht_memsize);
  schemerlicht_expression l = schemerlicht_init_lambda(ctxt);
  schemerlicht_string name = make_var_name(ctxt, 0);
  schemerlicht_vector_push_back(ctxt, &l.expr.lambda.variables, name, schemerlicht_string);
  schemerlicht_expression p = schemerlicht_init_primcall(ctxt);
  schemerlicht_string_append_cstr(ctxt, &p.expr.prim.name, "halt");
  schemerlicht_expression v = schemerlicht_init_variable(ctxt);
  schemerlicht_string_copy(ctxt, &v.expr.var.name, &name);
  schemerlicht_vector_push_back(ctxt, &p.expr.prim.arguments, v, schemerlicht_expression);
  schemerlicht_expression b = schemerlicht_init_begin(ctxt);
  schemerlicht_vector_push_back(ctxt, &b.expr.beg.arguments, p, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &l.expr.lambda.body, b, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &cps.continuation, l, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &cps.expressions_to_treat, init_conversion_state(e, state_init), cps_conversion_state);

  schemerlicht_assert(continuation_is_valid(ctxt, get_continuation(&cps)));

  treat_cps_expressions(ctxt, &cps);

  schemerlicht_vector_destroy(ctxt, &cps.index);
  schemerlicht_vector_destroy(ctxt, &cps.expressions_to_treat);
  schemerlicht_vector_destroy(ctxt, &cps.continuation);
  }

void schemerlicht_continuation_passing_style(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_assert(program->expressions.vector_size <= 1);
  program->cps_converted = 1;
  if (program->expressions.vector_size == 0)
    return;
  schemerlicht_expression* e = schemerlicht_vector_at(&program->expressions, 0, schemerlicht_expression);
  if (e->type == schemerlicht_type_begin)
    {
    schemerlicht_expression* it = schemerlicht_vector_begin(&e->expr.beg.arguments, schemerlicht_expression);
    schemerlicht_expression* it_end = schemerlicht_vector_end(&e->expr.beg.arguments, schemerlicht_expression);
    for (; it != it_end; ++it)
      {
      cps_start(ctxt, it);
      }
    schemerlicht_vector new_expressions = e->expr.beg.arguments;
    schemerlicht_string_destroy(ctxt, &e->expr.beg.filename);
    schemerlicht_vector_destroy(ctxt, &program->expressions);
    program->expressions = new_expressions;
    }
  else
    {
    cps_start(ctxt, e);
    }  
  //cps_start(ctxt, e);
  }