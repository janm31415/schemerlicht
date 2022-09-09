#include "sobject.h"


int schemerlicht_objects_equal(const schemerlicht_object* obj1, const schemerlicht_object* obj2)
  {
  if (obj1->type != obj2->type)
    return 0;
  switch (obj1->type)
    {
    case schemerlicht_object_type_nil:
      return 1;
    case schemerlicht_object_type_fixnum:
      return obj1->value.fx == obj2->value.fx;
    case schemerlicht_object_type_flonum:
      return obj1->value.fl == obj2->value.fl;
    case schemerlicht_object_type_pointer:
    default:
      return obj1->value.ptr == obj2->value.ptr;
    }
  }

int schemerlicht_log2(uint32_t x)
  {
  static const uint8_t log_8[255] = {
    0,
    1,1,
    2,2,2,2,
    3,3,3,3,3,3,3,3,
    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
    };
  if (x >= 0x00010000) 
    {
    if (x >= 0x01000000) 
      return log_8[((x >> 24) & 0xff) - 1] + 24;
    else 
      return log_8[((x >> 16) & 0xff) - 1] + 16;
    }
  else {
    if (x >= 0x00000100) 
      return log_8[((x >> 8) & 0xff) - 1] + 8;
    else if (x) 
      return log_8[(x & 0xff) - 1];
    return -1;  /* special `log' for 0 */
    }
  }