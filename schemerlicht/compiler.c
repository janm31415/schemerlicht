#include "compiler.h"
#include "object.h"
#include "error.h"
#include "context.h"

static void compile_fixnum(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_literal);
  schemerlicht_assert(e->expr.lit.type == schemerlicht_type_fixnum);
  schemerlicht_fixnum fx = e->expr.lit.lit.fx.value;
  }

static void compile_literal(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->type == schemerlicht_type_literal);
  switch (e->expr.lit.type)
    {
    case schemerlicht_type_fixnum:
      compile_fixnum(ctxt, fun, e);
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