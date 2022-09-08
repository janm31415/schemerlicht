#ifndef SCHEMERLICHT_OBJECT_H
#define SCHEMERLICHT_OBJECT_H

#include "schemerlicht.h"
#include "slimits.h"

typedef union
  {
  void* ptr;
  schemerlicht_fixnum fx;
  schemerlicht_flonum fl;  
  } schemerlicht_value;

typedef struct schemerlicht_object
  {
  int type;
  schemerlicht_value value;
  } schemerlicht_object;

#endif //SCHEMERLICHT_OBJECT_H