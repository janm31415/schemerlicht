#ifndef SCHEMERLICHT_FUNC_H
#define SCHEMERLICHT_FUNC_H

#include "schemerlicht.h"
#include "vector.h"
#include "map.h"

typedef struct schemerlicht_function
  {
  schemerlicht_map* constants_map;
  schemerlicht_vector constants;
  schemerlicht_vector code;
  schemerlicht_vector lambdas; //functions defined inside the function
  int number_of_constants;
  int freereg;  /* first free register */
#ifdef SCHEMERLICHT_DEBUG_LAMBDA_DEFINITION
  schemerlicht_string function_definition;
#endif
  } schemerlicht_function;


schemerlicht_function* schemerlicht_function_new(schemerlicht_context* ctxt);
void schemerlicht_function_free(schemerlicht_context* ctxt, schemerlicht_function* f);

#endif //SCHEMERLICHT_FUNC_H