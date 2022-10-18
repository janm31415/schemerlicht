#include "compiler.h"
#include "object.h"
#include "error.h"
#include "context.h"
#include "vm.h"

static void compile_fixnum(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_literal);
  schemerlicht_assert(e->expr.lit.type == schemerlicht_type_fixnum);  
  schemerlicht_memsize k_pos = fun->constants.vector_size;
  schemerlicht_fixnum fx = e->expr.lit.lit.fx.value;
  schemerlicht_object fx_obj = make_schemerlicht_object_fixnum(fx);
  schemerlicht_vector_push_back(ctxt, &fun->constants, fx_obj, schemerlicht_object);
  schemerlicht_instruction i = 0;
  SCHEMERLICHT_SET_OPCODE(i, SCHEMERLICHT_OPCODE_LOADK);
  SCHEMERLICHT_SETARG_Bx(i, k_pos);
  SCHEMERLICHT_SETARG_A(i, fun->freereg++);
  schemerlicht_vector_push_back(ctxt, &fun->code, i, schemerlicht_instruction);
  }

static void compile_flonum(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_literal);
  schemerlicht_assert(e->expr.lit.type == schemerlicht_type_flonum);
  schemerlicht_memsize k_pos = fun->constants.vector_size;
  schemerlicht_flonum fl = e->expr.lit.lit.fl.value;
  schemerlicht_object fl_obj = make_schemerlicht_object_flonum(fl);
  schemerlicht_vector_push_back(ctxt, &fun->constants, fl_obj, schemerlicht_object);
  schemerlicht_instruction i = 0;
  SCHEMERLICHT_SET_OPCODE(i, SCHEMERLICHT_OPCODE_LOADK);
  SCHEMERLICHT_SETARG_Bx(i, k_pos);
  SCHEMERLICHT_SETARG_A(i, fun->freereg++);
  schemerlicht_vector_push_back(ctxt, &fun->code, i, schemerlicht_instruction);
  }

static void compile_true(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_literal);
  schemerlicht_assert(e->expr.lit.type == schemerlicht_type_true);
  schemerlicht_memsize k_pos = fun->constants.vector_size;
  schemerlicht_object obj = make_schemerlicht_object_true();
  schemerlicht_vector_push_back(ctxt, &fun->constants, obj, schemerlicht_object);
  schemerlicht_instruction i = 0;
  SCHEMERLICHT_SET_OPCODE(i, SCHEMERLICHT_OPCODE_LOADK);
  SCHEMERLICHT_SETARG_Bx(i, k_pos);
  SCHEMERLICHT_SETARG_A(i, fun->freereg++);
  schemerlicht_vector_push_back(ctxt, &fun->code, i, schemerlicht_instruction);
  }

static void compile_false(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_literal);
  schemerlicht_assert(e->expr.lit.type == schemerlicht_type_false);
  schemerlicht_memsize k_pos = fun->constants.vector_size;
  schemerlicht_object obj = make_schemerlicht_object_false();
  schemerlicht_vector_push_back(ctxt, &fun->constants, obj, schemerlicht_object);
  schemerlicht_instruction i = 0;
  SCHEMERLICHT_SET_OPCODE(i, SCHEMERLICHT_OPCODE_LOADK);
  SCHEMERLICHT_SETARG_Bx(i, k_pos);
  SCHEMERLICHT_SETARG_A(i, fun->freereg++);
  schemerlicht_vector_push_back(ctxt, &fun->code, i, schemerlicht_instruction);
  }

static void compile_nil(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_literal);
  schemerlicht_assert(e->expr.lit.type == schemerlicht_type_nil);
  schemerlicht_memsize k_pos = fun->constants.vector_size;
  schemerlicht_object obj = make_schemerlicht_object_nil();
  schemerlicht_vector_push_back(ctxt, &fun->constants, obj, schemerlicht_object);
  schemerlicht_instruction i = 0;
  SCHEMERLICHT_SET_OPCODE(i, SCHEMERLICHT_OPCODE_LOADK);
  SCHEMERLICHT_SETARG_Bx(i, k_pos);
  SCHEMERLICHT_SETARG_A(i, fun->freereg++);
  schemerlicht_vector_push_back(ctxt, &fun->code, i, schemerlicht_instruction);
  }

static void compile_literal(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_literal);
  switch (e->expr.lit.type)
    {
    case schemerlicht_type_fixnum:
      compile_fixnum(ctxt, fun, e);
      break;
    case schemerlicht_type_flonum:
      compile_flonum(ctxt, fun, e);
      break;
    case schemerlicht_type_true:
      compile_true(ctxt, fun, e);
      break;
    case schemerlicht_type_false:
      compile_false(ctxt, fun, e);
      break;
    case schemerlicht_type_nil:
      compile_nil(ctxt, fun, e);
      break;
    default:
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
    }
  }

static void compile_expression(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  switch (e->type)
    {
    case schemerlicht_type_literal:
      compile_literal(ctxt, fun, e);
      break;
    default:
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
    }
  }

schemerlicht_function schemerlicht_compile_expression(schemerlicht_context* ctxt, schemerlicht_expression* e)
  {
  schemerlicht_function fun = schemerlicht_function_init(ctxt);
  compile_expression(ctxt, &fun, e);
  return fun;
  }