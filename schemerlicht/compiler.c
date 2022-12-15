#include "compiler.h"
#include "object.h"
#include "error.h"
#include "context.h"
#include "vm.h"
#include "primitives.h"
#include "environment.h"
#include "dump.h"
#include "foreign.h"

#include <string.h>

static void make_code_abx(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_opcode opc, int a, int bx)
  {
  schemerlicht_instruction i = 0;
  SCHEMERLICHT_SET_OPCODE(i, opc);
  SCHEMERLICHT_SETARG_A(i, a);
  SCHEMERLICHT_SETARG_Bx(i, bx);
  schemerlicht_vector_push_back(ctxt, &fun->code, i, schemerlicht_instruction);
  }

static void make_code_asbx(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_opcode opc, int a, int sbx)
  {
  schemerlicht_instruction i = 0;
  SCHEMERLICHT_SET_OPCODE(i, opc);
  SCHEMERLICHT_SETARG_A(i, a);
  SCHEMERLICHT_SETARG_sBx(i, sbx);
  schemerlicht_vector_push_back(ctxt, &fun->code, i, schemerlicht_instruction);
  }

static void make_code_ab(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_opcode opc, int a, int b)
  {
  schemerlicht_instruction i = 0;
  SCHEMERLICHT_SET_OPCODE(i, opc);
  SCHEMERLICHT_SETARG_A(i, a);
  SCHEMERLICHT_SETARG_B(i, b);
  schemerlicht_vector_push_back(ctxt, &fun->code, i, schemerlicht_instruction);
  }

static void make_code_abc(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_opcode opc, int a, int b, int c)
  {
  schemerlicht_instruction i = 0;
  SCHEMERLICHT_SET_OPCODE(i, opc);
  SCHEMERLICHT_SETARG_A(i, a);
  SCHEMERLICHT_SETARG_B(i, b);
  SCHEMERLICHT_SETARG_C(i, c);
  schemerlicht_vector_push_back(ctxt, &fun->code, i, schemerlicht_instruction);
  }

static int get_k(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_object* k)
  {
  const schemerlicht_object* idx = schemerlicht_map_get(ctxt, fun->constants_map, k);
  if (idx != NULL && schemerlicht_object_get_type(idx) == schemerlicht_object_type_fixnum)
    {
    schemerlicht_object_destroy(ctxt, k); // the key should be destroyed as the object was already added to the constants map
    return cast(int, idx->value.fx);
    }
  else
    {
    schemerlicht_object* new_id = schemerlicht_map_insert(ctxt, fun->constants_map, k);
    new_id->type = schemerlicht_object_type_fixnum;
    new_id->value.fx = fun->constants.vector_size;
    schemerlicht_vector_push_back(ctxt, &fun->constants, *k, schemerlicht_object);
    return cast(int, new_id->value.fx);
    }
  }


static schemerlicht_object* find_primitive(schemerlicht_context* ctxt, schemerlicht_string* s)
  {
  schemerlicht_object key;
  key.type = schemerlicht_object_type_string;
  key.value.s = *s;
  schemerlicht_object* res = schemerlicht_map_get(ctxt, ctxt->global->primitives_map, &key);
  return res;
  }

#define compiler_state_init 0
#define compiler_state_step_1 1
#define compiler_state_step_2 2
#define compiler_state_step_3 3

typedef struct compiler_if_state
  {
  schemerlicht_memsize first_jump_statement_pos;
  schemerlicht_memsize second_jump_statement_pos;
  schemerlicht_memsize target_first_jump_statement;
  } compiler_if_state;

typedef struct compiler_expression_state
  {
  schemerlicht_expression* expr;
  schemerlicht_function* fun;
  int state;
  int freereg;
  union
    {
    compiler_if_state if_state;
    schemerlicht_environment_entry lookup_entry;
    schemerlicht_function* lambda;
    schemerlicht_memsize foreign_fun_position;
    } aux;
  } compiler_expression_state;

static compiler_expression_state init_compiler_expression_state(schemerlicht_expression* e, schemerlicht_function* f, int freereg, int state)
  {
  compiler_expression_state s;
  s.expr = e;
  s.state = state;
  s.fun = f;
  s.freereg = freereg;
  return s;
  }

typedef struct compiler_helper
  {
  schemerlicht_vector expressions_to_treat;
  } compiler_helper;

static void compile_literal_iterative(schemerlicht_context* ctxt, compiler_expression_state* state)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_literal);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  switch (state->expr->expr.lit.type)
    {
    case schemerlicht_type_fixnum:
    {
    schemerlicht_fixnum fx = state->expr->expr.lit.lit.fx.value;
    if (fx <= SCHEMERLICHT_MAXARG_sBx && fx >= -SCHEMERLICHT_MAXARG_sBx)
      {
      make_code_asbx(ctxt, state->fun, SCHEMERLICHT_OPCODE_SETFIXNUM, state->freereg, cast(int, fx));
      return;
      }
    obj = make_schemerlicht_object_fixnum(fx);
    break;
    }
    case schemerlicht_type_flonum:
    {
    schemerlicht_flonum fl = state->expr->expr.lit.lit.fl.value;
    obj = make_schemerlicht_object_flonum(fl);
    break;
    }
    case schemerlicht_type_true:
    {
    make_code_ab(ctxt, state->fun, SCHEMERLICHT_OPCODE_SETTYPE, state->freereg, schemerlicht_object_type_true);
    return;
    }
    case schemerlicht_type_false:
    {
    make_code_ab(ctxt, state->fun, SCHEMERLICHT_OPCODE_SETTYPE, state->freereg, schemerlicht_object_type_false);
    return;
    }
    case schemerlicht_type_nil:
    {
    make_code_ab(ctxt, state->fun, SCHEMERLICHT_OPCODE_SETTYPE, state->freereg, schemerlicht_object_type_nil);
    return;
    }
    case schemerlicht_type_character:
    {
    schemerlicht_byte b = state->expr->expr.lit.lit.ch.value;
    make_code_ab(ctxt, state->fun, SCHEMERLICHT_OPCODE_SETCHAR, state->freereg, b);
    return;
    }
    case schemerlicht_type_string:
    {
    schemerlicht_string s = state->expr->expr.lit.lit.str.value;
    obj = make_schemerlicht_object_string(ctxt, s.string_ptr);
    break;
    }
    case schemerlicht_type_symbol:
    {
    schemerlicht_string s = state->expr->expr.lit.lit.sym.value;
    obj = make_schemerlicht_object_symbol(ctxt, s.string_ptr);
    break;
    }
    default:
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
    }
  int k_pos = get_k(ctxt, state->fun, &obj);
  make_code_abx(ctxt, state->fun, SCHEMERLICHT_OPCODE_LOADK, state->freereg, k_pos);
  }


static void compile_variable_iterative(schemerlicht_context* ctxt, compiler_expression_state* state)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_variable);
  schemerlicht_environment_entry lookup_entry;
  int find_var = schemerlicht_environment_find(&lookup_entry, ctxt, &state->expr->expr.var.name);
  if (find_var == 0)
    {
    /*
    This variable was not found in the environment. However: it is possible that this variable will be defined by a load at runtime.
    Therefore we now allocate a global position for this variable. It might be filled in later by load.
    Other possibility is that the variable is "apply", as this is a special case primitive.
    */
    if (strcmp(state->expr->expr.var.name.string_ptr, "apply") == 0)
      {
      make_code_ab(ctxt, state->fun, SCHEMERLICHT_OPCODE_SETPRIMOBJ, state->freereg, cast(int, SCHEMERLICHT_APPLY));
      }
    else
      {
      schemerlicht_environment_entry entry;
      entry.type = SCHEMERLICHT_ENV_TYPE_GLOBAL;
      entry.position = ctxt->globals.vector_size;
      schemerlicht_object obj;
      obj.type = schemerlicht_object_type_unassigned;
      obj.value.fx = entry.position;
      schemerlicht_vector_push_back(ctxt, &ctxt->globals, obj, schemerlicht_object);
      schemerlicht_string s;
      schemerlicht_string_copy(ctxt, &s, &state->expr->expr.var.name);
      schemerlicht_environment_add_to_base(ctxt, &s, entry);
      make_code_abx(ctxt, state->fun, SCHEMERLICHT_OPCODE_LOADGLOBAL, state->freereg, cast(int, entry.position));
      }
    }
  else
    {
    if (lookup_entry.type == SCHEMERLICHT_ENV_TYPE_STACK)
      {
      make_code_ab(ctxt, state->fun, SCHEMERLICHT_OPCODE_MOVE, state->freereg, cast(int, lookup_entry.position));
      }
    else
      {
      make_code_abx(ctxt, state->fun, SCHEMERLICHT_OPCODE_LOADGLOBAL, state->freereg, cast(int, lookup_entry.position));
      }
    }
  }

static void compile_begin_iterative(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_begin);
  const schemerlicht_memsize nr_exprs = state->expr->expr.beg.arguments.vector_size;
  for (schemerlicht_memsize i = 0; i < nr_exprs; ++i)
    {
    schemerlicht_expression* expr = schemerlicht_vector_at(&state->expr->expr.beg.arguments, nr_exprs - i - 1, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, init_compiler_expression_state(expr, state->fun, state->freereg, compiler_state_init), compiler_expression_state);
    }
  }

static void compile_let_iterative(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_let);
  schemerlicht_assert(state->expr->expr.let.bt == schemerlicht_bt_let);
  schemerlicht_assert(state->state == compiler_state_init);
  const schemerlicht_memsize nr_let_bindings = state->expr->expr.let.bindings.vector_size;

  schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, init_compiler_expression_state(state->expr, state->fun, state->freereg + nr_let_bindings, compiler_state_step_1), compiler_expression_state);

  for (schemerlicht_memsize i = 0; i < nr_let_bindings; ++i)
    {
    schemerlicht_memsize idx = nr_let_bindings - i - 1;
    schemerlicht_let_binding* binding = schemerlicht_vector_at(&state->expr->expr.let.bindings, idx, schemerlicht_let_binding);
    schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, init_compiler_expression_state(&binding->binding_expr, state->fun, state->freereg + idx, compiler_state_init), compiler_expression_state);
    }
  }

static void compile_let_iterative_step_1(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_let);
  schemerlicht_assert(state->expr->expr.let.bt == schemerlicht_bt_let);
  schemerlicht_assert(state->state == compiler_state_step_1);
  const schemerlicht_memsize nr_let_bindings = state->expr->expr.let.bindings.vector_size;
  schemerlicht_environment_push_child(ctxt);
  for (schemerlicht_memsize i = 0; i < nr_let_bindings; ++i)
    {
    schemerlicht_let_binding* binding = schemerlicht_vector_at(&state->expr->expr.let.bindings, i, schemerlicht_let_binding);
    schemerlicht_string name;
    schemerlicht_string_copy(ctxt, &name, &binding->binding_name);
    schemerlicht_environment_entry entry;
    entry.type = SCHEMERLICHT_ENV_TYPE_STACK;
    entry.position = cast(schemerlicht_fixnum, state->freereg + cast(int, i) - cast(int, nr_let_bindings));
    schemerlicht_environment_add(ctxt, &name, entry);
    }
  schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, init_compiler_expression_state(state->expr, state->fun, state->freereg, compiler_state_step_2), compiler_expression_state);
  schemerlicht_expression* body_expr = schemerlicht_vector_at(&state->expr->expr.let.body, 0, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, init_compiler_expression_state(body_expr, state->fun, state->freereg, compiler_state_init), compiler_expression_state);
  }

static void compile_let_iterative_step_2(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  UNUSED(helper);
  schemerlicht_assert(state->expr->type == schemerlicht_type_let);
  schemerlicht_assert(state->expr->expr.let.bt == schemerlicht_bt_let);
  schemerlicht_assert(state->state == compiler_state_step_2);
  const schemerlicht_memsize nr_let_bindings = state->expr->expr.let.bindings.vector_size;
  schemerlicht_environment_pop_child(ctxt);
  make_code_ab(ctxt, state->fun, SCHEMERLICHT_OPCODE_MOVE, state->freereg - nr_let_bindings, state->freereg);
  }

static void compile_prim_iterative(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_primitive_call);
  schemerlicht_assert(state->state == compiler_state_init);
  if (state->expr->expr.prim.as_object)
    {
    schemerlicht_object* prim = find_primitive(ctxt, &state->expr->expr.prim.name);
    if (prim == NULL)
      {
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
      }
    make_code_ab(ctxt, state->fun, SCHEMERLICHT_OPCODE_SETPRIMOBJ, state->freereg, cast(int, prim->value.fx));
    }
  else
    {
    schemerlicht_object* prim = find_primitive(ctxt, &state->expr->expr.prim.name);
    if (prim == NULL)
      {
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
      }
    schemerlicht_assert(schemerlicht_object_get_type(prim) == schemerlicht_object_type_primitive);
    const schemerlicht_memsize nr_prim_args = state->expr->expr.prim.arguments.vector_size;
    schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, init_compiler_expression_state(state->expr, state->fun, state->freereg, compiler_state_step_1), compiler_expression_state);

    for (schemerlicht_memsize i = 0; i < nr_prim_args; ++i)
      {
      schemerlicht_memsize idx = nr_prim_args - i - 1;
      schemerlicht_expression* arg = schemerlicht_vector_at(&state->expr->expr.prim.arguments, idx, schemerlicht_expression);
      schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, init_compiler_expression_state(arg, state->fun, state->freereg + idx + 1, compiler_state_init), compiler_expression_state);
      }
    }
  }

static void compile_prim_iterative_step_1(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  UNUSED(helper);
  schemerlicht_assert(state->expr->type == schemerlicht_type_primitive_call);
  schemerlicht_assert(state->state == compiler_state_step_1);
  schemerlicht_object* prim = find_primitive(ctxt, &state->expr->expr.prim.name);
  if (prim == NULL)
    {
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
    }
  schemerlicht_assert(schemerlicht_object_get_type(prim) == schemerlicht_object_type_primitive);
  const schemerlicht_memsize nr_prim_args = state->expr->expr.prim.arguments.vector_size;
  if (strcmp(state->expr->expr.prim.name.string_ptr, "halt") == 0)
    {
    make_code_ab(ctxt, state->fun, SCHEMERLICHT_OPCODE_RETURN, state->freereg + 1, 1);
    }
  else
    {
    make_code_ab(ctxt, state->fun, SCHEMERLICHT_OPCODE_SETPRIM, state->freereg, cast(int, prim->value.fx));
    make_code_abc(ctxt, state->fun, SCHEMERLICHT_OPCODE_CALL, state->freereg, nr_prim_args, 0);
    }
  }

static void compile_if_iterative(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_if);
  schemerlicht_assert(state->state == compiler_state_init);
  if (state->expr->expr.i.arguments.vector_size != 3)
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
  schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, init_compiler_expression_state(state->expr, state->fun, state->freereg, compiler_state_step_1), compiler_expression_state);
  schemerlicht_expression* expr_test = schemerlicht_vector_at(&state->expr->expr.i.arguments, 0, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, init_compiler_expression_state(expr_test, state->fun, state->freereg, compiler_state_init), compiler_expression_state);
  }

static void compile_if_iterative_step_1(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_if);
  schemerlicht_assert(state->state == compiler_state_step_1);
  make_code_ab(ctxt, state->fun, SCHEMERLICHT_OPCODE_EQTYPE, state->freereg, schemerlicht_object_type_false);
  compiler_expression_state new_state = init_compiler_expression_state(state->expr, state->fun, state->freereg, compiler_state_step_2);
  schemerlicht_instruction i1 = 0;
  SCHEMERLICHT_SET_OPCODE(i1, SCHEMERLICHT_OPCODE_JMP);
  schemerlicht_vector_push_back(ctxt, &state->fun->code, i1, schemerlicht_instruction);
  new_state.aux.if_state.first_jump_statement_pos = state->fun->code.vector_size - 1;
  schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, new_state, compiler_expression_state);
  schemerlicht_expression* expr_else_branch = schemerlicht_vector_at(&state->expr->expr.i.arguments, 2, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, init_compiler_expression_state(expr_else_branch, state->fun, state->freereg, compiler_state_init), compiler_expression_state);
  }

static void compile_if_iterative_step_2(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_if);
  schemerlicht_assert(state->state == compiler_state_step_2);
  compiler_expression_state new_state = init_compiler_expression_state(state->expr, state->fun, state->freereg, compiler_state_step_3);
  new_state.aux.if_state.first_jump_statement_pos = state->aux.if_state.first_jump_statement_pos;
  schemerlicht_instruction i2 = 0;
  SCHEMERLICHT_SET_OPCODE(i2, SCHEMERLICHT_OPCODE_JMP);
  schemerlicht_vector_push_back(ctxt, &state->fun->code, i2, schemerlicht_instruction);
  new_state.aux.if_state.second_jump_statement_pos = state->fun->code.vector_size - 1;
  new_state.aux.if_state.target_first_jump_statement = state->fun->code.vector_size;
  schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, new_state, compiler_expression_state);
  schemerlicht_expression* expr_then_branch = schemerlicht_vector_at(&state->expr->expr.i.arguments, 1, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, init_compiler_expression_state(expr_then_branch, state->fun, state->freereg, compiler_state_init), compiler_expression_state);
  }

static void compile_if_iterative_step_3(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  UNUSED(helper);
  UNUSED(ctxt);
  schemerlicht_assert(state->expr->type == schemerlicht_type_if);
  schemerlicht_assert(state->state == compiler_state_step_3);
  schemerlicht_memsize target_second_jump_statement = state->fun->code.vector_size;
  int first_jump_statement_offset = (int)state->aux.if_state.target_first_jump_statement - (int)state->aux.if_state.first_jump_statement_pos - 1;
  int second_jump_statement_offset = (int)target_second_jump_statement - (int)state->aux.if_state.second_jump_statement_pos - 1;
  schemerlicht_instruction* first_jump = schemerlicht_vector_at(&state->fun->code, state->aux.if_state.first_jump_statement_pos, schemerlicht_instruction);
  schemerlicht_instruction* second_jump = schemerlicht_vector_at(&state->fun->code, state->aux.if_state.second_jump_statement_pos, schemerlicht_instruction);
  SCHEMERLICHT_SETARG_sBx(*first_jump, first_jump_statement_offset);
  SCHEMERLICHT_SETARG_sBx(*second_jump, second_jump_statement_offset);
  }

static void compile_set_iterative(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_set);
  schemerlicht_assert(state->state == compiler_state_init);
  schemerlicht_environment_entry lookup_entry;
  int find_var = schemerlicht_environment_find(&lookup_entry, ctxt, &state->expr->expr.set.name);
  if (find_var == 0)
    {
    schemerlicht_compile_error_cstr(ctxt, SCHEMERLICHT_ERROR_VARIABLE_UNKNOWN, state->expr->expr.set.line_nr, state->expr->expr.set.column_nr, &state->expr->expr.set.filename, state->expr->expr.set.name.string_ptr);
    }
  else
    {
    compiler_expression_state new_state = init_compiler_expression_state(state->expr, state->fun, state->freereg, compiler_state_step_1);
    new_state.aux.lookup_entry = lookup_entry;
    schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, new_state, compiler_expression_state);
    schemerlicht_expression* value_expr = schemerlicht_vector_at(&state->expr->expr.set.value, 0, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, init_compiler_expression_state(value_expr, state->fun, state->freereg, compiler_state_init), compiler_expression_state);
    }
  }

static void compile_set_iterative_step_1(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  UNUSED(helper);
  schemerlicht_assert(state->expr->type == schemerlicht_type_set);
  schemerlicht_assert(state->state == compiler_state_step_1);

  if (state->aux.lookup_entry.type == SCHEMERLICHT_ENV_TYPE_GLOBAL)
    {
    make_code_abx(ctxt, state->fun, SCHEMERLICHT_OPCODE_STOREGLOBAL, state->freereg, cast(int, state->aux.lookup_entry.position));
    }
  else
    {
    make_code_ab(ctxt, state->fun, SCHEMERLICHT_OPCODE_MOVE, cast(int, state->aux.lookup_entry.position), state->freereg);
    }
  }

static void compile_lambda_iterative(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_lambda);
  schemerlicht_assert(state->state == compiler_state_init);
  schemerlicht_function* new_fun = schemerlicht_function_new(ctxt);
  schemerlicht_environment_push_child(ctxt);
  for (schemerlicht_memsize i = 0; i < state->expr->expr.lambda.variables.vector_size; ++i)
    {
    schemerlicht_environment_entry entry;
    entry.type = SCHEMERLICHT_ENV_TYPE_STACK;
    entry.position = i;
    schemerlicht_string* var_name = schemerlicht_vector_at(&state->expr->expr.lambda.variables, i, schemerlicht_string);
    schemerlicht_string entry_name;
    schemerlicht_string_copy(ctxt, &entry_name, var_name);
    schemerlicht_environment_add(ctxt, &entry_name, entry);
    }
  if (state->expr->expr.lambda.variable_arity)
    {
    make_code_ab(ctxt, new_fun, SCHEMERLICHT_OPCODE_LIST_STACK, state->expr->expr.lambda.variables.vector_size - 1, 0);
    }
  compiler_expression_state new_state = init_compiler_expression_state(state->expr, state->fun, state->freereg, compiler_state_step_1);
  new_state.aux.lambda = new_fun;
  schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, new_state, compiler_expression_state);
  schemerlicht_expression* body_expr = schemerlicht_vector_at(&state->expr->expr.lambda.body, 0, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, init_compiler_expression_state(body_expr, new_fun, state->expr->expr.lambda.variables.vector_size, compiler_state_init), compiler_expression_state);
  }

static void compile_lambda_iterative_step_1(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  UNUSED(helper);
  schemerlicht_assert(state->expr->type == schemerlicht_type_lambda);
  schemerlicht_assert(state->state == compiler_state_step_1);
  schemerlicht_environment_pop_child(ctxt);

#ifdef SCHEMERLICHT_DEBUG_LAMBDA_DEFINITION
  schemerlicht_expression* body_expr = schemerlicht_vector_at(&state->expr->expr.lambda.body, 0, schemerlicht_expression);
  schemerlicht_string tmp = schemerlicht_dump_expression(ctxt, body_expr, 0);
  schemerlicht_string_append(ctxt, &state->aux.lambda->function_definition, &tmp);
  schemerlicht_string_destroy(ctxt, &tmp);
#endif

  schemerlicht_vector_push_back(ctxt, &state->fun->lambdas, state->aux.lambda, schemerlicht_function*);
  schemerlicht_object lambda_obj;
  lambda_obj.type = schemerlicht_object_type_lambda;
  lambda_obj.value.ptr = (void*)state->aux.lambda;
  int k_pos = get_k(ctxt, state->fun, &lambda_obj);
  make_code_abx(ctxt, state->fun, SCHEMERLICHT_OPCODE_LOADK, state->freereg, k_pos);
  }

static void compile_funcall_iterative(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_funcall);
  schemerlicht_assert(state->state == compiler_state_init);
  schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, init_compiler_expression_state(state->expr, state->fun, state->freereg, compiler_state_step_1), compiler_expression_state);
  schemerlicht_expression* body_expr = schemerlicht_vector_at(&state->expr->expr.funcall.fun, 0, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, init_compiler_expression_state(body_expr, state->fun, state->freereg, compiler_state_init), compiler_expression_state);
  }

static void compile_funcall_iterative_step_1(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_funcall);
  schemerlicht_assert(state->state == compiler_state_step_1);
  schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, init_compiler_expression_state(state->expr, state->fun, state->freereg, compiler_state_step_2), compiler_expression_state);
  const schemerlicht_memsize nr_args = state->expr->expr.funcall.arguments.vector_size;
  for (schemerlicht_memsize i = 0; i < nr_args; ++i)
    {
    schemerlicht_memsize idx = nr_args - i - 1;
    schemerlicht_expression* arg = schemerlicht_vector_at(&state->expr->expr.funcall.arguments, idx, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, init_compiler_expression_state(arg, state->fun, state->freereg + idx + 1, compiler_state_init), compiler_expression_state);
    }
  }

static void compile_funcall_iterative_step_2(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  UNUSED(helper);
  schemerlicht_assert(state->expr->type == schemerlicht_type_funcall);
  schemerlicht_assert(state->state == compiler_state_step_2);
  const schemerlicht_memsize nr_args = state->expr->expr.funcall.arguments.vector_size;
  if (state->freereg > 0) // all lambda input variables should be in the first couple of registers.
    {
    make_code_ab(ctxt, state->fun, SCHEMERLICHT_OPCODE_MOVETOP, state->freereg, nr_args);
    }
  else
    {
    make_code_ab(ctxt, state->fun, SCHEMERLICHT_OPCODE_SETTYPE, nr_args + 1, schemerlicht_object_type_blocking);
    }
  make_code_abc(ctxt, state->fun, SCHEMERLICHT_OPCODE_CALL, 0, nr_args, 0);
  }

static void compile_foreign_iterative(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  schemerlicht_assert(state->expr->type == schemerlicht_type_foreign_call);
  schemerlicht_assert(state->state == compiler_state_init);
  schemerlicht_object key;
  key.type = schemerlicht_object_type_string;
  key.value.s = state->expr->expr.foreign.name;
  schemerlicht_object* pos = schemerlicht_map_get(ctxt, ctxt->externals_map, &key);
  if (pos == NULL)
    {
    schemerlicht_compile_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXTERNAL_UNKNOWN, state->expr->expr.foreign.line_nr, state->expr->expr.foreign.column_nr, &state->expr->expr.foreign.filename, state->expr->expr.foreign.name.string_ptr);
    }
  else
    {
    schemerlicht_assert(schemerlicht_object_get_type(pos) == schemerlicht_object_type_fixnum);
    const schemerlicht_memsize position = cast(schemerlicht_memsize, pos->value.fx);
    //schemerlicht_external_function* ext = schemerlicht_vector_at(&ctxt->externals, position, schemerlicht_external_function);
    const schemerlicht_memsize nr_args = state->expr->expr.foreign.arguments.vector_size;

    compiler_expression_state new_state = init_compiler_expression_state(state->expr, state->fun, state->freereg, compiler_state_step_1);
    new_state.aux.foreign_fun_position = position;
    schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, new_state, compiler_expression_state);
    for (schemerlicht_memsize i = 0; i < nr_args; ++i)
      {
      schemerlicht_memsize idx = nr_args - i - 1;
      schemerlicht_expression* arg = schemerlicht_vector_at(&state->expr->expr.foreign.arguments, idx, schemerlicht_expression);
      schemerlicht_vector_push_back(ctxt, &helper->expressions_to_treat, init_compiler_expression_state(arg, state->fun, state->freereg + idx + 1, compiler_state_init), compiler_expression_state);
      }
    }
  }

static void compile_foreign_iterative_step_1(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  UNUSED(helper);
  schemerlicht_assert(state->expr->type == schemerlicht_type_foreign_call);
  schemerlicht_assert(state->state == compiler_state_step_1);
  const schemerlicht_memsize nr_args = state->expr->expr.foreign.arguments.vector_size;
  make_code_asbx(ctxt, state->fun, SCHEMERLICHT_OPCODE_SETFIXNUM, state->freereg, cast(int, state->aux.foreign_fun_position));
  make_code_ab(ctxt, state->fun, SCHEMERLICHT_OPCODE_CALL_FOREIGN, state->freereg, nr_args);

  }

static void treat_compiler_state_init(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  if (ctxt->number_of_compile_errors > 0)
    return;
  switch (state->expr->type)
    {
    case schemerlicht_type_literal:
      compile_literal_iterative(ctxt, state);
      break;
    case schemerlicht_type_begin:
      compile_begin_iterative(ctxt, state, helper);
      break;
    case schemerlicht_type_let:
      compile_let_iterative(ctxt, state, helper);
      break;
    case schemerlicht_type_primitive_call:
      compile_prim_iterative(ctxt, state, helper);
      break;
    case schemerlicht_type_variable:
      compile_variable_iterative(ctxt, state);
      break;
    case schemerlicht_type_if:
      compile_if_iterative(ctxt, state, helper);
      break;
    case schemerlicht_type_set:
      compile_set_iterative(ctxt, state, helper);
      break;
    case schemerlicht_type_lambda:
      compile_lambda_iterative(ctxt, state, helper);
      break;
    case schemerlicht_type_funcall:
      compile_funcall_iterative(ctxt, state, helper);
      break;
    case schemerlicht_type_foreign_call:
      compile_foreign_iterative(ctxt, state, helper);
      break;
    case schemerlicht_type_nop:
      make_code_ab(ctxt, state->fun, SCHEMERLICHT_OPCODE_SETTYPE, state->freereg, schemerlicht_object_type_undefined);
      break;
    default:
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
    }
  }


static void treat_compiler_state_step_1(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  if (ctxt->number_of_compile_errors > 0)
    return;
  switch (state->expr->type)
    {
    case schemerlicht_type_let:
      compile_let_iterative_step_1(ctxt, state, helper);
      break;
    case schemerlicht_type_primitive_call:
      compile_prim_iterative_step_1(ctxt, state, helper);
      break;
    case schemerlicht_type_if:
      compile_if_iterative_step_1(ctxt, state, helper);
      break;
    case schemerlicht_type_set:
      compile_set_iterative_step_1(ctxt, state, helper);
      break;
    case schemerlicht_type_lambda:
      compile_lambda_iterative_step_1(ctxt, state, helper);
      break;
    case schemerlicht_type_funcall:
      compile_funcall_iterative_step_1(ctxt, state, helper);
      break;
    case schemerlicht_type_foreign_call:
      compile_foreign_iterative_step_1(ctxt, state, helper);
      break;
    default:
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
    }
  }

static void treat_compiler_state_step_2(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  if (ctxt->number_of_compile_errors > 0)
    return;
  switch (state->expr->type)
    {
    case schemerlicht_type_let:
      compile_let_iterative_step_2(ctxt, state, helper);
      break;
    case schemerlicht_type_if:
      compile_if_iterative_step_2(ctxt, state, helper);
      break;
    case schemerlicht_type_funcall:
      compile_funcall_iterative_step_2(ctxt, state, helper);
      break;
    default:
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
    }
  }

static void treat_compiler_state_step_3(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  if (ctxt->number_of_compile_errors > 0)
    return;
  switch (state->expr->type)
    {
    case schemerlicht_type_if:
      compile_if_iterative_step_3(ctxt, state, helper);
      break;
    default:
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
    }
  }

static void treat_compiler_state(schemerlicht_context* ctxt, compiler_expression_state* state, compiler_helper* helper)
  {
  switch (state->state)
    {
    case compiler_state_init:
      treat_compiler_state_init(ctxt, state, helper); break;
    case compiler_state_step_1:
      treat_compiler_state_step_1(ctxt, state, helper); break;
    case compiler_state_step_2:
      treat_compiler_state_step_2(ctxt, state, helper); break;
    case compiler_state_step_3:
      treat_compiler_state_step_3(ctxt, state, helper); break;
    default:
      break;
    }
  }

static void treat_compiler_expressions(schemerlicht_context* ctxt, compiler_helper* helper)
  {
  while (helper->expressions_to_treat.vector_size > 0)
    {
    compiler_expression_state c_state = *schemerlicht_vector_back(&helper->expressions_to_treat, compiler_expression_state);
    schemerlicht_vector_pop_back(&helper->expressions_to_treat);
    treat_compiler_state(ctxt, &c_state, helper);
    }
  }

static void compile_expression_iterative(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  compiler_helper helper;
  schemerlicht_vector_init(ctxt, &helper.expressions_to_treat, compiler_expression_state);
  schemerlicht_vector_push_back(ctxt, &helper.expressions_to_treat, init_compiler_expression_state(e, fun, 0, compiler_state_init), compiler_expression_state);

  treat_compiler_expressions(ctxt, &helper);

  schemerlicht_vector_destroy(ctxt, &helper.expressions_to_treat);
  }


schemerlicht_function* schemerlicht_compile_expression(schemerlicht_context* ctxt, schemerlicht_expression* e)
  {
  schemerlicht_compile_errors_clear(ctxt);
  schemerlicht_function* fun = schemerlicht_function_new(ctxt);
  compile_expression_iterative(ctxt, fun, e);
  return fun;
  }

static void add_return_statements_to_lambdas(schemerlicht_context* ctxt, schemerlicht_vector* compiled_program)
  {
  schemerlicht_function** it = schemerlicht_vector_begin(compiled_program, schemerlicht_function*);
  schemerlicht_function** it_end = schemerlicht_vector_end(compiled_program, schemerlicht_function*);
  for (; it != it_end; ++it)
    {
    make_code_ab(ctxt, (*it), SCHEMERLICHT_OPCODE_RETURN, 0, 1);
    }
  }

schemerlicht_vector schemerlicht_compile_program(schemerlicht_context* ctxt, schemerlicht_program* prog)
  {
  schemerlicht_vector compiled;
  schemerlicht_vector_init(ctxt, &compiled, schemerlicht_function*);
  schemerlicht_expression* it = schemerlicht_vector_begin(&prog->expressions, schemerlicht_expression);
  schemerlicht_expression* it_end = schemerlicht_vector_end(&prog->expressions, schemerlicht_expression);
  for (; it != it_end; ++it)
    {
    schemerlicht_function* fun = schemerlicht_compile_expression(ctxt, it);
    schemerlicht_vector_push_back(ctxt, &compiled, fun, schemerlicht_function*);
    }
  if (prog->cps_converted == 0)
    {
    add_return_statements_to_lambdas(ctxt, &compiled);
    }
  return compiled;
  }

void schemerlicht_compiled_program_destroy(schemerlicht_context* ctxt, schemerlicht_vector* compiled_program)
  {
  schemerlicht_function** it = schemerlicht_vector_begin(compiled_program, schemerlicht_function*);
  schemerlicht_function** it_end = schemerlicht_vector_end(compiled_program, schemerlicht_function*);
  for (; it != it_end; ++it)
    {
    schemerlicht_function_free(ctxt, *it);
    }
  schemerlicht_vector_destroy(ctxt, compiled_program);
  }

void schemerlicht_compiled_program_register(schemerlicht_context* ctxt, schemerlicht_vector* compiled_program)
  {
  schemerlicht_function** it = schemerlicht_vector_begin(compiled_program, schemerlicht_function*);
  schemerlicht_function** it_end = schemerlicht_vector_end(compiled_program, schemerlicht_function*);
  schemerlicht_function** where_it = schemerlicht_vector_end(&ctxt->lambdas, schemerlicht_function*);
  schemerlicht_vector_insert(ctxt, &ctxt->lambdas, &where_it, &it, &it_end, schemerlicht_function*);
  schemerlicht_vector_destroy(ctxt, compiled_program);
  }