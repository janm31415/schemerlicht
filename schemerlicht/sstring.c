#include "sstring.h"

#include <string.h>

void schemerlicht_string_init(schemerlicht_context* ctxt, schemerlicht_string* str, const char* txt)
  {
  str->string_length = cast(schemerlicht_memsize, strlen(txt));
  str->string_ptr = schemerlicht_newvector(ctxt, str->string_length + 1, char);
  str->string_capacity = str->string_length + 1;
  memcpy(str->string_ptr, txt, str->string_capacity * sizeof(char));
  }

void schemerlicht_string_copy(schemerlicht_context* ctxt, schemerlicht_string* str, const schemerlicht_string* str_to_copy)
  {
  str->string_length = str_to_copy->string_length;
  str->string_ptr = schemerlicht_newvector(ctxt, str_to_copy->string_capacity, char);
  str->string_capacity = str_to_copy->string_capacity;
  memcpy(str->string_ptr, str_to_copy->string_ptr, str->string_capacity * sizeof(char));
  }

void schemerlicht_string_init_ranged(schemerlicht_context* ctxt, schemerlicht_string* str, const char* from, const char* to)
  {
  str->string_length = cast(schemerlicht_memsize, to-from);
  str->string_ptr = schemerlicht_newvector(ctxt, str->string_length + 1, char);
  str->string_capacity = str->string_length + 1;
  memcpy(str->string_ptr, from, str->string_length * sizeof(char));
  str->string_ptr[str->string_length] = 0;
  }

void schemerlicht_string_destroy(schemerlicht_context* ctxt, schemerlicht_string* str)
  {
  schemerlicht_realloc(ctxt, str->string_ptr, str->string_capacity * sizeof(char), 0);
  }

char* schemerlicht_string_at(schemerlicht_string* str, schemerlicht_memsize index)
  {
  return str->string_ptr + index;
  }

char* schemerlicht_string_c_str(schemerlicht_string* str)
  {
  return str->string_ptr;
  }

char* schemerlicht_string_begin(schemerlicht_string* str)
  {
  return str->string_ptr;
  }

char* schemerlicht_string_end(schemerlicht_string* str)
  {
  return str->string_ptr + str->string_length;
  }

char* schemerlicht_string_front(schemerlicht_string* str)
  {
  return str->string_ptr;
  }

char* schemerlicht_string_back(schemerlicht_string* str)
  {
  return str->string_ptr + (str->string_length - 1);
  }

void schemerlicht_string_push_front(schemerlicht_context* ctxt, schemerlicht_string* str, char ch)
  {
  schemerlicht_growvector(ctxt, str->string_ptr, str->string_length + 2, str->string_capacity, char);
  memmove(str->string_ptr+1, str->string_ptr, str->string_length);
  *(str->string_ptr) = ch;
  ++(str->string_length);
  *(str->string_ptr + str->string_length) = 0;
  }

void schemerlicht_string_push_back(schemerlicht_context* ctxt, schemerlicht_string* str, char ch)
  {
  schemerlicht_growvector(ctxt, str->string_ptr, str->string_length + 2, str->string_capacity, char);
  *(str->string_ptr + str->string_length) = ch;
  ++(str->string_length);
  *(str->string_ptr + str->string_length) = 0;
  }

void schemerlicht_string_pop_back(schemerlicht_string* str)
  {
  if (str->string_length > 0)
    {
    --str->string_length;
    *(str->string_ptr + str->string_length) = 0;
    }
  }

void schemerlicht_string_append(schemerlicht_context* ctxt, schemerlicht_string* str, schemerlicht_string* append)
  {
  //schemerlicht_growvector(ctxt, str->string_ptr, str->string_length + append->string_length + 1, str->string_capacity, char);
  schemerlicht_reallocvector(ctxt, str->string_ptr, str->string_capacity, str->string_length + append->string_length + 1, char);
  memcpy(str->string_ptr + str->string_length, append->string_ptr, (append->string_length+1) * sizeof(char));
  str->string_capacity = str->string_length + append->string_length + 1;
  str->string_length += append->string_length;
  }

void schemerlicht_string_append_cstr(schemerlicht_context* ctxt, schemerlicht_string* str, const char* append)
  {
  schemerlicht_memsize append_length = cast(schemerlicht_memsize, strlen(append));
  schemerlicht_reallocvector(ctxt, str->string_ptr, str->string_capacity, str->string_length + append_length + 1, char);
  memcpy(str->string_ptr + str->string_length, append, (append_length + 1) * sizeof(char));
  str->string_capacity = str->string_length + append_length + 1;
  str->string_length += append_length;  
  }

void schemerlicht_string_clear(schemerlicht_string* str)
  {
  if (str->string_capacity > 0)
    {
    str->string_ptr[0] = 0;
    str->string_length = 0;
    }
  }

schemerlicht_string make_null_string()
  {
  schemerlicht_string s;
  s.string_capacity = 0;
  s.string_length = 0;
  s.string_ptr = NULL;
  return s;
  }