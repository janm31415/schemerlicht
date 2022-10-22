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
  } schemerlicht_function;


schemerlicht_function schemerlicht_function_init(schemerlicht_context* ctxt);
void schemerlicht_function_destroy(schemerlicht_context* ctxt, schemerlicht_function* f);

#endif //SCHEMERLICHT_FUNC_H