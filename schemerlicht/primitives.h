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
  SCHEMERLICHT_NOT_EQUAL
  } schemerlicht_primitives;

void schemerlicht_primitive_add1(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_sub1(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_add(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_sub(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_mul(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_div(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_equal(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_not_equal(schemerlicht_context* ctxt, int a, int b, int c);

void schemerlicht_call_primitive(schemerlicht_context* ctxt, schemerlicht_fixnum prim_id, int a, int b, int c);

schemerlicht_map* generate_primitives_map(schemerlicht_context* ctxt);

#endif //SCHEMERLICHT_PRIMITIVES_H