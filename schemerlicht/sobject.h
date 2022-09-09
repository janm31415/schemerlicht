#ifndef SCHEMERLICHT_OBJECT_H
#define SCHEMERLICHT_OBJECT_H

#include "schemerlicht.h"
#include "slimits.h"
#include "sstring.h"

#define schemerlicht_object_type_nil 0
#define schemerlicht_object_type_fixnum 1
#define schemerlicht_object_type_flonum 2
#define schemerlicht_object_type_string 3
#define schemerlicht_object_type_pointer 4

typedef union
  {
  void* ptr;
  schemerlicht_fixnum fx;
  schemerlicht_flonum fl;  
  schemerlicht_string s;
  } schemerlicht_value;

typedef struct schemerlicht_object
  {
  int type;
  schemerlicht_value value;
  } schemerlicht_object;

#define set_object(ctxt, obj1, obj2) \
  { \
  const schemerlicht_object* o2 = (obj2); \
  schemerlicht_object* o1 = (obj1); \
  o1->type = o2->type; \
  if (o1->type == schemerlicht_object_type_string) \
    schemerlicht_string_copy(ctxt, &(o1->value.s), &(o2->value.s)); \
  else \
    o1->value = o2->value; \
  }

int schemerlicht_objects_equal(const schemerlicht_object* obj1, const schemerlicht_object* obj2);

int schemerlicht_log2(uint32_t x);

schemerlicht_object make_schemerlicht_object_fixnum(schemerlicht_fixnum fx);
schemerlicht_object make_schemerlicht_object_flonum(schemerlicht_flonum fl);
schemerlicht_object make_schemerlicht_object_string(schemerlicht_context* ctxt, const char* s);
void destroy_schemerlicht_object(schemerlicht_context* ctxt, schemerlicht_object* obj);

#endif //SCHEMERLICHT_OBJECT_H