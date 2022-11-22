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

static void compile_expression(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e);

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
  if (idx != NULL && idx->type == schemerlicht_object_type_fixnum)
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
    if (k->type == schemerlicht_object_type_symbol)
      {
      printf("add to symbols list\n");
      }
    //if (k->type == schemerlicht_object_type_string || k->type == schemerlicht_object_type_lambda)
    //  {
    //  schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
    //  schemerlicht_set_object(heap_obj, k);
    //  ++ctxt->heap_pos;
    //  }
    return cast(int, new_id->value.fx);
    }
  }

static void compile_literal(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_literal);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  switch (e->expr.lit.type)
    {
    case schemerlicht_type_fixnum:
    {
    schemerlicht_fixnum fx = e->expr.lit.lit.fx.value;
    if (fx <= SCHEMERLICHT_MAXARG_sBx && fx >= -SCHEMERLICHT_MAXARG_sBx)
      {
      make_code_asbx(ctxt, fun, SCHEMERLICHT_OPCODE_SETFIXNUM, fun->freereg, cast(int, fx));
      return;
      }
    obj = make_schemerlicht_object_fixnum(fx);
    break;
    }
    case schemerlicht_type_flonum:
    {
    schemerlicht_flonum fl = e->expr.lit.lit.fl.value;
    obj = make_schemerlicht_object_flonum(fl);
    break;
    }
    case schemerlicht_type_true:
    {
    //obj = make_schemerlicht_object_true();
    make_code_ab(ctxt, fun, SCHEMERLICHT_OPCODE_SETTYPE, fun->freereg, schemerlicht_object_type_true);
    return;
    }
    case schemerlicht_type_false:
    {
    make_code_ab(ctxt, fun, SCHEMERLICHT_OPCODE_SETTYPE, fun->freereg, schemerlicht_object_type_false);
    return;
    }
    case schemerlicht_type_nil:
    {
    //obj = make_schemerlicht_object_nil();
    //break;
    make_code_ab(ctxt, fun, SCHEMERLICHT_OPCODE_SETTYPE, fun->freereg, schemerlicht_object_type_nil);
    return;
    }
    case schemerlicht_type_character:
    {
    //schemerlicht_byte b = e->expr.lit.lit.ch.value;
    //obj = make_schemerlicht_object_char(b);
    //break;        
    schemerlicht_byte b = e->expr.lit.lit.ch.value;
    make_code_ab(ctxt, fun, SCHEMERLICHT_OPCODE_SETCHAR, fun->freereg, b);
    return;
    }
    case schemerlicht_type_string:
    {
    schemerlicht_string s = e->expr.lit.lit.str.value;
    obj = make_schemerlicht_object_string(ctxt, s.string_ptr);
    break;
    }
    case schemerlicht_type_symbol:
    {
    schemerlicht_string s = e->expr.lit.lit.sym.value;
    obj = make_schemerlicht_object_symbol(ctxt, s.string_ptr);
    break;
    }
    default:
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
    }
  int k_pos = get_k(ctxt, fun, &obj);
  make_code_abx(ctxt, fun, SCHEMERLICHT_OPCODE_LOADK, fun->freereg, k_pos);
  }

static schemerlicht_object* find_primitive(schemerlicht_context* ctxt, schemerlicht_string* s)
  {
  schemerlicht_object key;
  key.type = schemerlicht_object_type_string;
  key.value.s = *s;
  schemerlicht_object* res = schemerlicht_map_get(ctxt, ctxt->global->primitives_map, &key);
  return res;
  }

static void compile_prim(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_primitive_call);
  if (e->expr.prim.as_object)
    {
    schemerlicht_object* prim = find_primitive(ctxt, &e->expr.prim.name);
    if (prim == NULL)
      {
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
      }
    //schemerlicht_object prim_as_object = *prim;
    //prim_as_object.type = schemerlicht_object_type_primitive_object; // change type to object
    //int k_pos = get_k(ctxt, fun, &prim_as_object);
    //make_code_abx(ctxt, fun, SCHEMERLICHT_OPCODE_LOADK, fun->freereg, k_pos);
    make_code_ab(ctxt, fun, SCHEMERLICHT_OPCODE_SETPRIMOBJ, fun->freereg, cast(int, prim->value.fx));
    }
  else
    {
    schemerlicht_object* prim = find_primitive(ctxt, &e->expr.prim.name);
    if (prim == NULL)
      {
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
      }
    schemerlicht_assert(prim->type == schemerlicht_object_type_primitive);
    const schemerlicht_memsize nr_prim_args = e->expr.prim.arguments.vector_size;
    for (schemerlicht_memsize i = 0; i < nr_prim_args; ++i)
      {
      schemerlicht_expression* arg = schemerlicht_vector_at(&e->expr.prim.arguments, i, schemerlicht_expression);
      ++fun->freereg;
      compile_expression(ctxt, fun, arg);
      }
    fun->freereg -= nr_prim_args;
    if (strcmp(e->expr.prim.name.string_ptr, "halt") == 0)
      {
      make_code_ab(ctxt, fun, SCHEMERLICHT_OPCODE_RETURN, fun->freereg + 1, 1);
      }
    else
      {
      //int k_pos = get_k(ctxt, fun, prim); // will be added to the constants list

      //make_code_abx(ctxt, fun, SCHEMERLICHT_OPCODE_LOADK, fun->freereg, k_pos);
      make_code_ab(ctxt, fun, SCHEMERLICHT_OPCODE_SETPRIM, fun->freereg, cast(int, prim->value.fx));
      make_code_abc(ctxt, fun, SCHEMERLICHT_OPCODE_CALL, fun->freereg, nr_prim_args, 0);
      }
    }
  }

static void compile_if(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_if);
  if (e->expr.i.arguments.vector_size != 3)
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
  schemerlicht_expression* expr_test = schemerlicht_vector_at(&e->expr.i.arguments, 0, schemerlicht_expression);
  schemerlicht_expression* expr_then_branch = schemerlicht_vector_at(&e->expr.i.arguments, 1, schemerlicht_expression);
  schemerlicht_expression* expr_else_branch = schemerlicht_vector_at(&e->expr.i.arguments, 2, schemerlicht_expression);
  compile_expression(ctxt, fun, expr_test);
  make_code_ab(ctxt, fun, SCHEMERLICHT_OPCODE_EQTYPE, fun->freereg, schemerlicht_object_type_false);
  schemerlicht_instruction i1 = 0;
  SCHEMERLICHT_SET_OPCODE(i1, SCHEMERLICHT_OPCODE_JMP);
  schemerlicht_vector_push_back(ctxt, &fun->code, i1, schemerlicht_instruction);
  schemerlicht_memsize first_jump_statement_pos = fun->code.vector_size - 1;
  compile_expression(ctxt, fun, expr_else_branch);
  schemerlicht_instruction i2 = 0;
  SCHEMERLICHT_SET_OPCODE(i2, SCHEMERLICHT_OPCODE_JMP);
  schemerlicht_vector_push_back(ctxt, &fun->code, i2, schemerlicht_instruction);
  schemerlicht_memsize second_jump_statement_pos = fun->code.vector_size - 1;
  schemerlicht_memsize target_first_jump_statement = fun->code.vector_size;
  compile_expression(ctxt, fun, expr_then_branch);
  schemerlicht_memsize target_second_jump_statement = fun->code.vector_size;
  int first_jump_statement_offset = (int)target_first_jump_statement - (int)first_jump_statement_pos - 1;
  int second_jump_statement_offset = (int)target_second_jump_statement - (int)second_jump_statement_pos - 1;
  schemerlicht_instruction* first_jump = schemerlicht_vector_at(&fun->code, first_jump_statement_pos, schemerlicht_instruction);
  schemerlicht_instruction* second_jump = schemerlicht_vector_at(&fun->code, second_jump_statement_pos, schemerlicht_instruction);
  SCHEMERLICHT_SETARG_sBx(*first_jump, first_jump_statement_offset);
  SCHEMERLICHT_SETARG_sBx(*second_jump, second_jump_statement_offset);
  }

static void compile_set(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_set);
  schemerlicht_environment_entry lookup_entry;
  int find_var = schemerlicht_environment_find(&lookup_entry, ctxt, &e->expr.set.name);
  if (find_var == 0)
    {
    schemerlicht_compile_error_cstr(ctxt, SCHEMERLICHT_ERROR_VARIABLE_UNKNOWN, e->expr.set.line_nr, e->expr.set.column_nr, e->expr.set.name.string_ptr);
    }
  else
    {
    schemerlicht_expression* value_expr = schemerlicht_vector_at(&e->expr.set.value, 0, schemerlicht_expression);
    compile_expression(ctxt, fun, value_expr);
    if (lookup_entry.type == SCHEMERLICHT_ENV_TYPE_GLOBAL)
      {
      make_code_abx(ctxt, fun, SCHEMERLICHT_OPCODE_STOREGLOBAL, fun->freereg, cast(int, lookup_entry.position));
      }
    else
      {
      make_code_ab(ctxt, fun, SCHEMERLICHT_OPCODE_MOVE, cast(int, lookup_entry.position), fun->freereg);
      }
    }
  }

static void compile_variable(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_variable);
  schemerlicht_environment_entry lookup_entry;
  int find_var = schemerlicht_environment_find(&lookup_entry, ctxt, &e->expr.var.name);
  if (find_var == 0)
    {
    /*
    This variable was not found in the environment. However: it is possible that this variable will be defined by a load at runtime.
    Therefore we now allocate a global position for this variable. It might be filled in later by load.
    Other possibility is that the variable is "apply", as this is a special case primitive.
    */
    if (strcmp(e->expr.var.name.string_ptr, "apply") == 0)
      {
      make_code_ab(ctxt, fun, SCHEMERLICHT_OPCODE_SETPRIMOBJ, fun->freereg, cast(int, SCHEMERLICHT_APPLY));
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
      schemerlicht_string_copy(ctxt, &s, &e->expr.var.name);
      schemerlicht_environment_add_to_base(ctxt, &s, entry);
      make_code_abx(ctxt, fun, SCHEMERLICHT_OPCODE_LOADGLOBAL, fun->freereg, cast(int, entry.position));
      }
    }
  else
    {
    if (lookup_entry.type == SCHEMERLICHT_ENV_TYPE_STACK)
      {
      make_code_ab(ctxt, fun, SCHEMERLICHT_OPCODE_MOVE, fun->freereg, cast(int, lookup_entry.position));
      }
    else
      {
      make_code_abx(ctxt, fun, SCHEMERLICHT_OPCODE_LOADGLOBAL, fun->freereg, cast(int, lookup_entry.position));
      }
    }
  }

static void compile_begin(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_begin);
  const schemerlicht_memsize nr_exprs = e->expr.beg.arguments.vector_size;
  for (schemerlicht_memsize i = 0; i < nr_exprs; ++i)
    {
    schemerlicht_expression* expr = schemerlicht_vector_at(&e->expr.beg.arguments, i, schemerlicht_expression);
    compile_expression(ctxt, fun, expr);
    }
  }

static void compile_foreign(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_foreign_call);
  schemerlicht_object key;
  key.type = schemerlicht_object_type_string;
  key.value.s = e->expr.foreign.name;
  schemerlicht_object* pos = schemerlicht_map_get(ctxt, ctxt->externals_map, &key);
  if (pos == NULL)
    {
    schemerlicht_compile_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXTERNAL_UNKNOWN, e->expr.foreign.line_nr, e->expr.foreign.column_nr, e->expr.foreign.name.string_ptr);
    }
  else
    {
    schemerlicht_assert(pos->type == schemerlicht_object_type_fixnum);
    const schemerlicht_memsize position = cast(schemerlicht_memsize, pos->value.fx);
    schemerlicht_external_function* ext = schemerlicht_vector_at(&ctxt->externals, position, schemerlicht_external_function);
    const schemerlicht_memsize nr_args = e->expr.foreign.arguments.vector_size;
    //if (nr_args != ext->arguments.vector_size)
    //  {
    //  schemerlicht_compile_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS, e->expr.foreign.line_nr, e->expr.foreign.column_nr, e->expr.foreign.name.string_ptr);
    //  return;
    //  }
    for (schemerlicht_memsize i = 0; i < nr_args; ++i)
      {
      schemerlicht_expression* arg = schemerlicht_vector_at(&e->expr.foreign.arguments, i, schemerlicht_expression);
      ++fun->freereg;
      compile_expression(ctxt, fun, arg);
      }
    fun->freereg -= nr_args;
    //int k_pos = get_k(ctxt, fun, prim); // will be added to the constants list

    //make_code_abx(ctxt, fun, SCHEMERLICHT_OPCODE_LOADK, fun->freereg, k_pos);
    make_code_asbx(ctxt, fun, SCHEMERLICHT_OPCODE_SETFIXNUM, fun->freereg, cast(int, position));
    make_code_ab(ctxt, fun, SCHEMERLICHT_OPCODE_CALL_FOREIGN, fun->freereg, nr_args);
    }
  }

static void compile_funcall(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_funcall);
  const schemerlicht_memsize nr_args = e->expr.funcall.arguments.vector_size;
  schemerlicht_expression* body_expr = schemerlicht_vector_at(&e->expr.funcall.fun, 0, schemerlicht_expression);
  compile_expression(ctxt, fun, body_expr);
  ++fun->freereg;

  for (schemerlicht_memsize i = 0; i < nr_args; ++i)
    {
    schemerlicht_expression* arg = schemerlicht_vector_at(&e->expr.funcall.arguments, i, schemerlicht_expression);
    compile_expression(ctxt, fun, arg);
    ++fun->freereg;
    }

  fun->freereg -= nr_args + 1;

  if (fun->freereg > 0) // all lambda input variables should be in the first couple of registers.
    {
#if 0
    make_code_ab(ctxt, fun, SCHEMERLICHT_OPCODE_MOVE, 0, fun->freereg); // closure at R(0)
    for (schemerlicht_memsize i = 0; i < nr_args; ++i)
      {
      make_code_ab(ctxt, fun, SCHEMERLICHT_OPCODE_MOVE, i + 1, fun->freereg + i + 1);
      }
#else
    make_code_ab(ctxt, fun, SCHEMERLICHT_OPCODE_MOVETOP, fun->freereg, nr_args);
#endif
    }
  else
    {
    make_code_ab(ctxt, fun, SCHEMERLICHT_OPCODE_SETTYPE, nr_args+1, schemerlicht_object_type_blocking);
    }
  make_code_abc(ctxt, fun, SCHEMERLICHT_OPCODE_CALL, 0, nr_args, 0);
  }

static void compile_lambda(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_lambda);
  schemerlicht_function* new_fun = schemerlicht_function_new(ctxt);
  schemerlicht_environment_push_child(ctxt);

  for (schemerlicht_memsize i = 0; i < e->expr.lambda.variables.vector_size; ++i)
    {
    schemerlicht_environment_entry entry;
    entry.type = SCHEMERLICHT_ENV_TYPE_STACK;
    entry.position = i;
    schemerlicht_string* var_name = schemerlicht_vector_at(&e->expr.lambda.variables, i, schemerlicht_string);
    schemerlicht_string entry_name;
    schemerlicht_string_copy(ctxt, &entry_name, var_name);
    schemerlicht_environment_add(ctxt, &entry_name, entry);
    }
  if (e->expr.lambda.variable_arity)
    {
    make_code_ab(ctxt, new_fun, SCHEMERLICHT_OPCODE_LIST_STACK, e->expr.lambda.variables.vector_size - 1, 0);
    }
  new_fun->freereg = e->expr.lambda.variables.vector_size;
  schemerlicht_expression* body_expr = schemerlicht_vector_at(&e->expr.lambda.body, 0, schemerlicht_expression);
  compile_expression(ctxt, new_fun, body_expr);
  schemerlicht_environment_pop_child(ctxt);

#ifdef SCHEMERLICHT_DEBUG_LAMBDA_DEFINITION
  schemerlicht_string tmp = schemerlicht_dump_expression(ctxt, body_expr);
  schemerlicht_string_append(ctxt, &new_fun->function_definition, &tmp);
  schemerlicht_string_destroy(ctxt, &tmp);
#endif

  schemerlicht_vector_push_back(ctxt, &fun->lambdas, new_fun, schemerlicht_function*);
  schemerlicht_object lambda_obj;
  lambda_obj.type = schemerlicht_object_type_lambda;
  lambda_obj.value.ptr = (void*)new_fun;
  int k_pos = get_k(ctxt, fun, &lambda_obj);
  make_code_abx(ctxt, fun, SCHEMERLICHT_OPCODE_LOADK, fun->freereg, k_pos);
  }

static void compile_let(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_let);
  schemerlicht_assert(e->expr.let.bt == schemerlicht_bt_let);
  const schemerlicht_memsize nr_let_bindings = e->expr.let.bindings.vector_size;
  for (schemerlicht_memsize i = 0; i < nr_let_bindings; ++i)
    {
    schemerlicht_let_binding* binding = schemerlicht_vector_at(&e->expr.let.bindings, i, schemerlicht_let_binding);
    compile_expression(ctxt, fun, &binding->binding_expr);
    ++fun->freereg;
    }
  schemerlicht_environment_push_child(ctxt);
  for (schemerlicht_memsize i = 0; i < nr_let_bindings; ++i)
    {
    schemerlicht_let_binding* binding = schemerlicht_vector_at(&e->expr.let.bindings, i, schemerlicht_let_binding);
    schemerlicht_string name;
    schemerlicht_string_copy(ctxt, &name, &binding->binding_name);
    schemerlicht_environment_entry entry;
    entry.type = SCHEMERLICHT_ENV_TYPE_STACK;
    entry.position = cast(schemerlicht_fixnum, fun->freereg + cast(int, i) - cast(int, nr_let_bindings));
    schemerlicht_environment_add(ctxt, &name, entry);
    }
  schemerlicht_expression* body_expr = schemerlicht_vector_at(&e->expr.let.body, 0, schemerlicht_expression);
  compile_expression(ctxt, fun, body_expr);
  schemerlicht_environment_pop_child(ctxt);
  make_code_ab(ctxt, fun, SCHEMERLICHT_OPCODE_MOVE, fun->freereg - nr_let_bindings, fun->freereg);
  fun->freereg -= nr_let_bindings;
  }

static void compile_expression(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  if (ctxt->number_of_compile_errors > 0)
    return;
  int first_free_reg = fun->freereg;
  switch (e->type)
    {
    case schemerlicht_type_literal:
      compile_literal(ctxt, fun, e);
      break;
    case schemerlicht_type_primitive_call:
      compile_prim(ctxt, fun, e);
      break;
    case schemerlicht_type_if:
      compile_if(ctxt, fun, e);
      break;
    case schemerlicht_type_let:
      compile_let(ctxt, fun, e);
      break;
    case schemerlicht_type_begin:
      compile_begin(ctxt, fun, e);
      break;
    case schemerlicht_type_variable:
      compile_variable(ctxt, fun, e);
      break;
    case schemerlicht_type_set:
      compile_set(ctxt, fun, e);
      break;
    case schemerlicht_type_lambda:
      compile_lambda(ctxt, fun, e);
      break;
    case schemerlicht_type_funcall:
      compile_funcall(ctxt, fun, e);
      break;
    case schemerlicht_type_foreign_call:
      compile_foreign(ctxt, fun, e);
      break;
    case schemerlicht_type_nop:
      make_code_ab(ctxt, fun, SCHEMERLICHT_OPCODE_SETTYPE, fun->freereg, schemerlicht_object_type_undefined);
      break;
    default:
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
    }
  UNUSED(first_free_reg);
  schemerlicht_assert(fun->freereg == first_free_reg);
  }

schemerlicht_function* schemerlicht_compile_expression(schemerlicht_context* ctxt, schemerlicht_expression* e)
  {
  schemerlicht_compile_errors_clear(ctxt);
  schemerlicht_function* fun = schemerlicht_function_new(ctxt);
  compile_expression(ctxt, fun, e);
  return fun;
  }
