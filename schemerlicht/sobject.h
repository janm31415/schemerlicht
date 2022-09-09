#ifndef SCHEMERLICHT_OBJECT_H
#define SCHEMERLICHT_OBJECT_H

#include "schemerlicht.h"
#include "slimits.h"

#define schemerlicht_object_type_nil 0
#define schemerlicht_object_type_fixnum 1
#define schemerlicht_object_type_flonum 2
#define schemerlicht_object_type_pointer 3

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

#define set_object(obj1, obj2) \
  { \
  const schemerlicht_object* o2 = (obj2); \
  schemerlicht_object* o1 = (obj1); \
  o1->type = o2->type; \
  o1->value = o2->value; \
  }

int schemerlicht_objects_equal(const schemerlicht_object* obj1, const schemerlicht_object* obj2);

int schemerlicht_log2(uint32_t x);

#endif //SCHEMERLICHT_OBJECT_H