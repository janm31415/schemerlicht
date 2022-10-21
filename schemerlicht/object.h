#ifndef SCHEMERLICHT_OBJECT_H
#define SCHEMERLICHT_OBJECT_H

#include "schemerlicht.h"
#include "limits.h"
#include "string.h"
#include "vector.h"

#define schemerlicht_object_type_undefined 0
#define schemerlicht_object_type_char 1
#define schemerlicht_object_type_fixnum 2
#define schemerlicht_object_type_flonum 3
#define schemerlicht_object_type_string 4
#define schemerlicht_object_type_pointer 5
#define schemerlicht_object_type_true 6
#define schemerlicht_object_type_false 7
#define schemerlicht_object_type_symbol 8
#define schemerlicht_object_type_nil 9
#define schemerlicht_object_type_vector 10
#define schemerlicht_object_type_list 11

typedef union
  {
  schemerlicht_vector v;
  schemerlicht_string s;
  void* ptr;
  schemerlicht_fixnum fx;
  schemerlicht_flonum fl;    
  schemerlicht_byte ch;
  } schemerlicht_value;

typedef struct schemerlicht_object
  {  
  schemerlicht_value value;
  int type;
  } schemerlicht_object;

#define schemerlicht_set_object(obj1, obj2) \
  { \
  const schemerlicht_object* o2 = (obj2); \
  schemerlicht_object* o1 = (obj1); \
  o1->type = o2->type; \
  o1->value = o2->value; \
  }

int schemerlicht_objects_equal(const schemerlicht_object* obj1, const schemerlicht_object* obj2);

int schemerlicht_log2(uint32_t x);

schemerlicht_object make_schemerlicht_object_fixnum(schemerlicht_fixnum fx);
schemerlicht_object make_schemerlicht_object_flonum(schemerlicht_flonum fl);
schemerlicht_object make_schemerlicht_object_true();
schemerlicht_object make_schemerlicht_object_false();
schemerlicht_object make_schemerlicht_object_nil();
schemerlicht_object make_schemerlicht_object_char(schemerlicht_byte ch);
schemerlicht_object make_schemerlicht_object_string(schemerlicht_context* ctxt, const char* s);
schemerlicht_object make_schemerlicht_object_symbol(schemerlicht_context* ctxt, const char* s);
void schemerlicht_object_destroy(schemerlicht_context* ctxt, schemerlicht_object* obj);

schemerlicht_string schemerlicht_object_to_string(schemerlicht_context* ctxt, schemerlicht_object* obj);

#endif //SCHEMERLICHT_OBJECT_H