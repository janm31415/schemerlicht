#ifndef SCHEMERLICHT_FOREIGN_H
#define SCHEMERLICHT_FOREIGN_H

#include "schemerlicht.h"
#include "string.h"
#include "vector.h"
#include "object.h"

//typedef enum schemerlicht_foreign_return_type
//  {
//  schemerlicht_foreign_char_pointer,
//  schemerlicht_foreign_flonum,
//  schemerlicht_foreign_fixnum,
//  schemerlicht_foreign_void,
//  schemerlicht_foreign_object
//  } schemerlicht_foreign_return_type;

typedef struct schemerlicht_external_function
  {
  schemerlicht_string name;
  SCHEMERLICHT_FUNPTR(address);
  //schemerlicht_vector arguments;
  schemerlicht_foreign_return_type return_type;
  } schemerlicht_external_function;

schemerlicht_external_function schemerlicht_external_function_init(schemerlicht_context* ctxt, const char* name, SCHEMERLICHT_FUNPTR(address), schemerlicht_foreign_return_type ret_type);
//void schemerlicht_external_function_push_argument(schemerlicht_context* ctxt, schemerlicht_external_function* ext, schemerlicht_foreign_return_type arg);

void schemerlicht_external_function_destroy(schemerlicht_context* ctxt, schemerlicht_external_function* ext);

void schemerlicht_register_external_function(schemerlicht_context* ctxt, schemerlicht_external_function* ext);

schemerlicht_object schemerlicht_call_external(schemerlicht_context* ctxt, schemerlicht_external_function* ext, int argument_stack_offset, int nr_of_args);

#endif //SCHEMERLICHT_FOREIGN_H
