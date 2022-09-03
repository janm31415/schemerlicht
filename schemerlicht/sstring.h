#ifndef SCHEMERLICHT_STRING_H
#define SCHEMERLICHT_STRING_H

#include "schemerlicht.h"
#include "smemory.h"

typedef struct schemerlicht_string
  {
  char* string_ptr;
  schemerlicht_memsize string_capacity; // the memory capacity of the string
  schemerlicht_memsize string_length; // the length of the string  
  } schemerlicht_string;

SCHEMERLICHT_API void schemerlicht_string_init(schemerlicht_context* ctxt, schemerlicht_string* str, const char* txt);

SCHEMERLICHT_API void schemerlicht_string_destroy(schemerlicht_context* ctxt, schemerlicht_string* str);

#endif // SCHEMERLICHT_STRING_H