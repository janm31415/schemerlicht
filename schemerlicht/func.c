#include "func.h"
#include "vector.h"
#include "object.h"

schemerlicht_function schemerlicht_function_init(schemerlicht_context* ctxt)
  {
  schemerlicht_function fun;
  schemerlicht_vector_init(ctxt, &fun.constants, schemerlicht_object);
  schemerlicht_vector_init(ctxt, &fun.code, schemerlicht_instruction);
  return fun;
  }

void schemerlicht_function_destroy(schemerlicht_context* ctxt, schemerlicht_function* f)
  {
  schemerlicht_object* it = schemerlicht_vector_begin(&f->constants, schemerlicht_object);
  schemerlicht_object* it_end = schemerlicht_vector_end(&f->constants, schemerlicht_object);
  for (; it != it_end; ++it)
    {
    schemerlicht_object_destroy(ctxt, it);
    }
  schemerlicht_vector_destroy(ctxt, &f->constants);
  schemerlicht_vector_destroy(ctxt, &f->code);
  }