#include "foreign.h"
#include "context.h"
#include "map.h"

schemerlicht_external_function schemerlicht_external_function_init(schemerlicht_context* ctxt, const char* name, void* address, schemerlicht_foreign_argument_type ret_type)
  {
  schemerlicht_external_function ext;
  schemerlicht_string_init(ctxt, &ext.name, name);
  ext.address = address;
  ext.return_type = ret_type;
  schemerlicht_vector_init(ctxt, &ext.arguments, schemerlicht_foreign_argument_type);
  return ext;
  }

void schemerlicht_external_function_push_argument(schemerlicht_context* ctxt, schemerlicht_external_function* ext, schemerlicht_foreign_argument_type arg)
  {
  schemerlicht_vector_push_back(ctxt, &ext->arguments, arg, schemerlicht_foreign_argument_type);
  }

void schemerlicht_external_function_destroy(schemerlicht_context* ctxt, schemerlicht_external_function* ext)
  {
  schemerlicht_string_destroy(ctxt, &ext->name);
  schemerlicht_vector_destroy(ctxt, &ext->arguments);
  }

void schemerlicht_register_external_function(schemerlicht_context* ctxt, schemerlicht_external_function* ext)
  {  
  schemerlicht_object key;
  key.type = schemerlicht_object_type_string;
  key.value.s = ext->name;
  schemerlicht_object* pos = schemerlicht_map_insert(ctxt, ctxt->externals_map, &key);
  pos->type = schemerlicht_object_type_fixnum;
  pos->value.fx = cast(schemerlicht_fixnum, ctxt->externals.vector_size);
  schemerlicht_vector_push_back(ctxt, &ctxt->externals, *ext, schemerlicht_external_function);
  }

schemerlicht_object schemerlicht_call_external(schemerlicht_context* ctxt, schemerlicht_external_function* ext, int argument_stack_offset)
  {
  typedef schemerlicht_fixnum (*fun)();
  schemerlicht_fixnum result = (cast(fun, ext->address))();
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_fixnum;
  obj.value.fx = result;
  return obj;
  }