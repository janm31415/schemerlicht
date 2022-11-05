#include "foreign.h"
#include "context.h"
#include "map.h"
#include "error.h"

#include <string.h>

schemerlicht_external_function schemerlicht_external_function_init(schemerlicht_context* ctxt, const char* name, void* address, schemerlicht_foreign_argument_type ret_type)
  {
  schemerlicht_external_function ext;
  schemerlicht_string_init(ctxt, &ext.name, name);
  ext.address = address;
  ext.return_type = ret_type;
  //schemerlicht_vector_init(ctxt, &ext.arguments, schemerlicht_foreign_argument_type);
  return ext;
  }

//void schemerlicht_external_function_push_argument(schemerlicht_context* ctxt, schemerlicht_external_function* ext, schemerlicht_foreign_argument_type arg)
//  {
//  schemerlicht_vector_push_back(ctxt, &ext->arguments, arg, schemerlicht_foreign_argument_type);
//  }

void schemerlicht_external_function_destroy(schemerlicht_context* ctxt, schemerlicht_external_function* ext)
  {
  schemerlicht_string_destroy(ctxt, &ext->name);
  //schemerlicht_vector_destroy(ctxt, &ext->arguments);
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

static void* get_argument_pointer(schemerlicht_context* ctxt, int stack_offset)
  {
  schemerlicht_object* obj = schemerlicht_vector_at(&ctxt->stack, stack_offset, schemerlicht_object);
  switch (obj->type)
    {
    case schemerlicht_object_type_undefined:      
    case schemerlicht_object_type_true:
    case schemerlicht_object_type_false:
    case schemerlicht_object_type_nil:
    case schemerlicht_object_type_void:
    case schemerlicht_object_type_blocking:
      break;
    case schemerlicht_object_type_char:
      return cast(void*, &obj->value.ch);
    case schemerlicht_object_type_primitive:
    case schemerlicht_object_type_primitive_object:
    case schemerlicht_object_type_fixnum:
      return cast(void*, &obj->value.fx);
    case schemerlicht_object_type_flonum:
      return cast(void*, &obj->value.fl);
    case schemerlicht_object_type_string:
    case schemerlicht_object_type_symbol:
      return cast(void*, obj->value.s.string_ptr);
    case schemerlicht_object_type_closure:
    case schemerlicht_object_type_vector:
    case schemerlicht_object_type_pair:
      return cast(void*, &obj->value.v);    
    case schemerlicht_object_type_lambda:
      return obj->value.ptr;
    }
  return NULL;
  }

schemerlicht_object schemerlicht_call_external(schemerlicht_context* ctxt, schemerlicht_external_function* ext, int argument_stack_offset, int nr_of_args)
  {
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  switch (nr_of_args)
    {
    case 0:
    {
    switch (ext->return_type)
      {
      case schemerlicht_foreign_fixnum:
      {
        typedef schemerlicht_fixnum (*fun)();
        obj.type = schemerlicht_object_type_fixnum;
        obj.value.fx = (cast(fun, ext->address))();
        break;
      }
      case schemerlicht_foreign_flonum:
      {
      typedef schemerlicht_flonum(*fun)();
      obj.type = schemerlicht_object_type_flonum;
      obj.value.fl = (cast(fun, ext->address))();
      break;
      }
      case schemerlicht_foreign_char_pointer:
      {
      typedef const char*(*fun)();
      obj.type = schemerlicht_object_type_string;
      obj.value.s.string_ptr = cast(char*, (cast(fun, ext->address))());
      obj.value.s.string_length = cast(schemerlicht_memsize, strlen(obj.value.s.string_ptr));
      obj.value.s.string_capacity = 0;
      break;
      }
      case schemerlicht_foreign_void:
      {
      typedef void (*fun)();
      obj.type = schemerlicht_object_type_void;
      (cast(fun, ext->address))();
      break;
      }
      case schemerlicht_foreign_object:
      {
      typedef schemerlicht_object (*fun)();
      obj = (cast(fun, ext->address))();
      break;
      }
      }
    break;
    }
    case 1:
    {
    void* arg1 = get_argument_pointer(ctxt, argument_stack_offset);
    switch (ext->return_type)
      {
      case schemerlicht_foreign_fixnum:
      {
      typedef schemerlicht_fixnum(*fun)(void*);
      obj.type = schemerlicht_object_type_fixnum;
      obj.value.fx = (cast(fun, ext->address))(arg1);
      break;
      }
      case schemerlicht_foreign_flonum:
      {
      typedef schemerlicht_flonum(*fun)(void*);
      obj.type = schemerlicht_object_type_flonum;
      obj.value.fl = (cast(fun, ext->address))(arg1);
      break;
      }
      case schemerlicht_foreign_char_pointer:
      {
      typedef const char* (*fun)(void*);
      obj.type = schemerlicht_object_type_string;
      obj.value.s.string_ptr = cast(char*, (cast(fun, ext->address))(arg1));
      obj.value.s.string_length = cast(schemerlicht_memsize, strlen(obj.value.s.string_ptr));
      obj.value.s.string_capacity = 0;
      break;
      }
      case schemerlicht_foreign_void:
      {
      typedef void (*fun)(void*);
      obj.type = schemerlicht_object_type_void;
      (cast(fun, ext->address))(arg1);
      break;
      }
      case schemerlicht_foreign_object:
      {
      typedef schemerlicht_object(*fun)(void*);
      obj = (cast(fun, ext->address))(arg1);
      break;
      }
      }
    break;
    }
    default:
    {    
    schemerlicht_runerror(ctxt, "Too many parameters for foreign-call");
    break;
    }
    }  
  return obj;
  }