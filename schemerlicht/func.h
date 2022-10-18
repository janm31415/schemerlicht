#ifndef SCHEMERLICHT_FUNC_H
#define SCHEMERLICHT_FUNC_H

#include "schemerlicht.h"
#include "vector.h"

typedef struct schemerlicht_function
  {
  schemerlicht_vector constants;
  schemerlicht_vector code;
  } schemerlicht_function;


schemerlicht_function schemerlicht_function_init(schemerlicht_context* ctxt);
void schemerlicht_function_destroy(schemerlicht_context* ctxt, schemerlicht_function* f);

#endif //SCHEMERLICHT_FUNC_H