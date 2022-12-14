#ifndef SCHEMERLICHT_STRING_H
#define SCHEMERLICHT_STRING_H

#include "schemerlicht.h"
#include "memory.h"

typedef struct schemerlicht_string
  {
  char* string_ptr;
  schemerlicht_memsize string_capacity; // the memory capacity of the string
  schemerlicht_memsize string_length; // the length of the string  
  } schemerlicht_string;

SCHEMERLICHT_API void schemerlicht_string_init(schemerlicht_context* ctxt, schemerlicht_string* str, const char* txt);

SCHEMERLICHT_API void schemerlicht_string_init_with_size(schemerlicht_context* ctxt, schemerlicht_string* str, schemerlicht_memsize size, char element);

SCHEMERLICHT_API void schemerlicht_string_copy(schemerlicht_context* ctxt, schemerlicht_string* str, const schemerlicht_string* str_to_copy);

SCHEMERLICHT_API void schemerlicht_string_init_ranged(schemerlicht_context* ctxt, schemerlicht_string* str, const char* from, const char* to);

SCHEMERLICHT_API void schemerlicht_string_destroy(schemerlicht_context* ctxt, schemerlicht_string* str);

SCHEMERLICHT_API char* schemerlicht_string_at(schemerlicht_string* str, schemerlicht_memsize index);

SCHEMERLICHT_API char* schemerlicht_string_c_str(schemerlicht_string* str);

SCHEMERLICHT_API char* schemerlicht_string_begin(schemerlicht_string* str);

SCHEMERLICHT_API char* schemerlicht_string_end(schemerlicht_string* str);

SCHEMERLICHT_API char* schemerlicht_string_front(schemerlicht_string* str);

SCHEMERLICHT_API char* schemerlicht_string_back(schemerlicht_string* str);

SCHEMERLICHT_API void schemerlicht_string_push_front(schemerlicht_context* ctxt, schemerlicht_string* str, char ch);

SCHEMERLICHT_API void schemerlicht_string_push_back(schemerlicht_context* ctxt, schemerlicht_string* str, char ch);

SCHEMERLICHT_API void schemerlicht_string_pop_back(schemerlicht_string* str);

SCHEMERLICHT_API void schemerlicht_string_append(schemerlicht_context* ctxt, schemerlicht_string* str, schemerlicht_string* append);

SCHEMERLICHT_API void schemerlicht_string_append_cstr(schemerlicht_context* ctxt, schemerlicht_string* str, const char* append);

SCHEMERLICHT_API void schemerlicht_string_clear(schemerlicht_string* str);

SCHEMERLICHT_API schemerlicht_string make_null_string();

SCHEMERLICHT_API int schemerlicht_string_compare_less(schemerlicht_string* left, schemerlicht_string* right);

#endif // SCHEMERLICHT_STRING_H