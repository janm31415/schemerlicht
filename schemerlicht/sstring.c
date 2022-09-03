#include "sstring.h"

#include <string.h>

/*
* assumes that each const char* string is terminated by '/0'
*/
schemerlicht_memsize get_string_length(const char* txt)
  {
  schemerlicht_memsize length = 0;
  const char* string_it = txt;
  for (; (*string_it); ++string_it)
    ++length;
  return length;
  }

void schemerlicht_string_init(schemerlicht_context* ctxt, schemerlicht_string* str, const char* txt)
  {
  str->string_length = get_string_length(txt);
  str->string_ptr = schemerlicht_newvector(ctxt, str->string_length+1, char);
  str->string_capacity = str->string_length+1;
  memcpy(str->string_ptr, txt, str->string_capacity * sizeof(char));
  }

void schemerlicht_string_destroy(schemerlicht_context* ctxt, schemerlicht_string* str)
  {
  schemerlicht_realloc(ctxt, str->string_ptr, str->string_capacity * sizeof(char), 0);
  }