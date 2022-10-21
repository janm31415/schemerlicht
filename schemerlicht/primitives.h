#ifndef SCHEMERLICHT_PRIMITIVES_H
#define SCHEMERLICHT_PRIMITIVES_H

#include "schemerlicht.h"
#include "map.h"

typedef enum
  {
  SCHEMERLICHT_ADD1,
  SCHEMERLICHT_SUB1,
  SCHEMERLICHT_ADD,
  SCHEMERLICHT_SUB,
  SCHEMERLICHT_MUL,
  SCHEMERLICHT_DIV,
  SCHEMERLICHT_EQUAL,
  SCHEMERLICHT_NOT_EQUAL,
  SCHEMERLICHT_LESS,
  SCHEMERLICHT_LEQ,
  SCHEMERLICHT_GREATER,
  SCHEMERLICHT_GEQ,
  SCHEMERLICHT_FXADD1,
  SCHEMERLICHT_FXSUB1,
  SCHEMERLICHT_FXADD,
  SCHEMERLICHT_FXSUB,
  SCHEMERLICHT_FXMUL,
  SCHEMERLICHT_FXDIV,
  SCHEMERLICHT_FXEQUAL,
  SCHEMERLICHT_FXLESS,
  SCHEMERLICHT_FXLEQ,
  SCHEMERLICHT_FXGREATER,
  SCHEMERLICHT_FXGEQ,
  SCHEMERLICHT_FXZERO,
  SCHEMERLICHT_FLADD1,
  SCHEMERLICHT_FLSUB1,
  SCHEMERLICHT_FLADD,
  SCHEMERLICHT_FLSUB,
  SCHEMERLICHT_FLMUL,
  SCHEMERLICHT_FLDIV,
  SCHEMERLICHT_FLEQUAL,
  SCHEMERLICHT_FLLESS,
  SCHEMERLICHT_FLLEQ,
  SCHEMERLICHT_FLGREATER,
  SCHEMERLICHT_FLGEQ,
  SCHEMERLICHT_FLZERO,
  SCHEMERLICHT_CHAREQUAL,
  SCHEMERLICHT_CHARLESS,
  SCHEMERLICHT_CHARLEQ,
  SCHEMERLICHT_CHARGREATER,
  SCHEMERLICHT_CHARGEQ,
  SCHEMERLICHT_IS_FIXNUM,
  SCHEMERLICHT_IS_FLONUM,
  SCHEMERLICHT_IS_NULL,
  SCHEMERLICHT_IS_ZERO,
  SCHEMERLICHT_IS_BOOLEAN,
  SCHEMERLICHT_IS_CHAR,
  SCHEMERLICHT_NOT
  } schemerlicht_primitives;

void schemerlicht_primitive_add1(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_sub1(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_add(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_sub(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_mul(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_div(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_equal(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_not_equal(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_less(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_leq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_greater(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_geq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_fixnum(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_flonum(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_null(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_zero(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_boolean(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_char(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_not(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxadd1(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxsub1(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxadd(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxsub(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxmul(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxdiv(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxequal(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxless(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxleq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxgreater(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxgeq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxzero(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fladd1(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flsub1(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fladd(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flsub(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flmul(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fldiv(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flequal(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flless(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flleq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flgreater(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flgeq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flzero(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_charequal(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_charless(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_charleq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_chargreater(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_chargeq(schemerlicht_context* ctxt, int a, int b, int c);

void schemerlicht_call_primitive(schemerlicht_context* ctxt, schemerlicht_fixnum prim_id, int a, int b, int c);

schemerlicht_map* generate_primitives_map(schemerlicht_context* ctxt);

#endif //SCHEMERLICHT_PRIMITIVES_H