#include "compiler.h"
#include "object.h"
#include "error.h"
#include "context.h"
#include "vm.h"
#include "primitives.h"

static void compile_expression(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e);

static int get_k(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_object* k)
  {
  const schemerlicht_object* idx = schemerlicht_map_get(fun->constants_map, k);
  if (idx != NULL && idx->type == schemerlicht_object_type_fixnum)
    return cast(int, idx->value.fx);
  else
    {
    schemerlicht_object* new_id = schemerlicht_map_insert(ctxt, fun->constants_map, k);
    new_id->type = schemerlicht_object_type_fixnum;
    new_id->value.fx = fun->constants.vector_size;
    schemerlicht_vector_push_back(ctxt, &fun->constants, *k, schemerlicht_object);
    return cast(int, new_id->value.fx);
    }
  }

static void compile_literal(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_literal);  
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_nil;
  switch (e->expr.lit.type)
    {
    case schemerlicht_type_fixnum:
    {
    schemerlicht_fixnum fx = e->expr.lit.lit.fx.value;
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
    obj = make_schemerlicht_object_true();
    break;
    }
    case schemerlicht_type_false:
    {
    obj = make_schemerlicht_object_false();
    break;
    }
    case schemerlicht_type_nil:
    {
    obj = make_schemerlicht_object_nil();
    break;
    }
    case schemerlicht_type_character:
    {
    schemerlicht_byte b = e->expr.lit.lit.ch.value;
    obj = make_schemerlicht_object_char(b);
    break;
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
  schemerlicht_instruction i = 0;
  SCHEMERLICHT_SET_OPCODE(i, SCHEMERLICHT_OPCODE_LOADK);
  SCHEMERLICHT_SETARG_Bx(i, k_pos);
  SCHEMERLICHT_SETARG_A(i, fun->freereg);
  schemerlicht_vector_push_back(ctxt, &fun->code, i, schemerlicht_instruction);
  }

static void compile_prim(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_primitive_call);
  if (e->expr.prim.as_object)
    {
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
    }
  else
    {
    const schemerlicht_memsize nr_prim_args = e->expr.prim.arguments.vector_size;
    for (schemerlicht_memsize i = 0; i < nr_prim_args; ++i)
      {
      schemerlicht_expression* arg = schemerlicht_vector_at(&e->expr.prim.arguments, i, schemerlicht_expression);
      ++fun->freereg;
      compile_expression(ctxt, fun, arg);
      }
    fun->freereg -= nr_prim_args;    
    schemerlicht_object primitive_function_pointer_object;
    primitive_function_pointer_object.type = schemerlicht_object_type_fixnum;
    primitive_function_pointer_object.value.fx = SCHEMERLICHT_ADD1; // id of the primitive
    int k_pos = get_k(ctxt, fun, &primitive_function_pointer_object); // will be added to the constants list
    schemerlicht_instruction i0 = 0;
    SCHEMERLICHT_SET_OPCODE(i0, SCHEMERLICHT_OPCODE_LOADK);
    SCHEMERLICHT_SETARG_Bx(i0, k_pos);
    SCHEMERLICHT_SETARG_A(i0, fun->freereg);
    schemerlicht_vector_push_back(ctxt, &fun->code, i0, schemerlicht_instruction);
    schemerlicht_instruction i1 = 0;
    SCHEMERLICHT_SET_OPCODE(i1, SCHEMERLICHT_OPCODE_CALL);
    SCHEMERLICHT_SETARG_A(i1, fun->freereg);
    SCHEMERLICHT_SETARG_B(i1, 2);
    SCHEMERLICHT_SETARG_C(i1, 2);
    schemerlicht_vector_push_back(ctxt, &fun->code, i1, schemerlicht_instruction);
    }
  }

static void compile_expression(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  int first_free_reg = fun->freereg;
  switch (e->type)
    {
    case schemerlicht_type_literal:
      compile_literal(ctxt, fun, e);
      break;
    case schemerlicht_type_primitive_call:
      compile_prim(ctxt, fun, e);
      break;
    default:
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
    }
  UNUSED(first_free_reg);
  schemerlicht_assert(fun->freereg == first_free_reg);
  }

schemerlicht_function schemerlicht_compile_expression(schemerlicht_context* ctxt, schemerlicht_expression* e)
  {
  schemerlicht_function fun = schemerlicht_function_init(ctxt);
  compile_expression(ctxt, &fun, e);
  return fun;
  }