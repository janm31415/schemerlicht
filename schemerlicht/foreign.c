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

#define SCHEMERLICHT_CALL_EXTERNAL(funargs, args) \
    switch (ext->return_type) \
      { \
      case schemerlicht_foreign_fixnum: \
      { \
      typedef schemerlicht_fixnum(*fun)funargs; \
      obj.type = schemerlicht_object_type_fixnum; \
      obj.value.fx = (cast(fun, ext->address))args; \
      break; \
      } \
      case schemerlicht_foreign_flonum: \
      { \
      typedef schemerlicht_flonum(*fun)funargs; \
      obj.type = schemerlicht_object_type_flonum; \
      obj.value.fl = (cast(fun, ext->address))args; \
      break; \
      } \
      case schemerlicht_foreign_char_pointer: \
      { \
      typedef const char* (*fun)funargs; \
      obj.type = schemerlicht_object_type_string; \
      obj.value.s.string_ptr = cast(char*, (cast(fun, ext->address))args); \
      obj.value.s.string_length = cast(schemerlicht_memsize, strlen(obj.value.s.string_ptr)); \
      obj.value.s.string_capacity = 0; \
      break; \
      } \
      case schemerlicht_foreign_void: \
      { \
      typedef void (*fun)funargs; \
      obj.type = schemerlicht_object_type_void; \
      (cast(fun, ext->address))args; \
      break; \
      } \
      case schemerlicht_foreign_object: \
      { \
      typedef schemerlicht_object(*fun)funargs; \
      obj = (cast(fun, ext->address))args; \
      break; \
      } \
      }

schemerlicht_object schemerlicht_call_external(schemerlicht_context* ctxt, schemerlicht_external_function* ext, int argument_stack_offset, int nr_of_args)
  {
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  switch (nr_of_args)
    {
    case 0:
    {
    SCHEMERLICHT_CALL_EXTERNAL((),());
    break;
    }
    case 1:
    {
    void* arg1 = get_argument_pointer(ctxt, argument_stack_offset);
    SCHEMERLICHT_CALL_EXTERNAL((void*), (arg1));    
    break;
    }
    case 2:
    {
    void* arg1 = get_argument_pointer(ctxt, argument_stack_offset);
    void* arg2 = get_argument_pointer(ctxt, argument_stack_offset+1);
    SCHEMERLICHT_CALL_EXTERNAL((void*,void*), (arg1,arg2));
    break;
    }
    case 3:
    {
    void* arg1 = get_argument_pointer(ctxt, argument_stack_offset);
    void* arg2 = get_argument_pointer(ctxt, argument_stack_offset + 1);
    void* arg3 = get_argument_pointer(ctxt, argument_stack_offset + 2);
    SCHEMERLICHT_CALL_EXTERNAL((void*, void*, void*), (arg1, arg2, arg3));
    break;
    }
    case 4:
    {
    void* arg1 = get_argument_pointer(ctxt, argument_stack_offset);
    void* arg2 = get_argument_pointer(ctxt, argument_stack_offset + 1);
    void* arg3 = get_argument_pointer(ctxt, argument_stack_offset + 2);
    void* arg4 = get_argument_pointer(ctxt, argument_stack_offset + 3);
    SCHEMERLICHT_CALL_EXTERNAL((void*, void*, void*, void*), (arg1, arg2, arg3, arg4));
    break;
    }
    case 5:
    {
    void* arg1 = get_argument_pointer(ctxt, argument_stack_offset);
    void* arg2 = get_argument_pointer(ctxt, argument_stack_offset + 1);
    void* arg3 = get_argument_pointer(ctxt, argument_stack_offset + 2);
    void* arg4 = get_argument_pointer(ctxt, argument_stack_offset + 3);
    void* arg5 = get_argument_pointer(ctxt, argument_stack_offset + 4);
    SCHEMERLICHT_CALL_EXTERNAL((void*, void*, void*, void*, void*), (arg1, arg2, arg3, arg4, arg5));
    break;
    }
    case 6:
    {
    void* arg1 = get_argument_pointer(ctxt, argument_stack_offset);
    void* arg2 = get_argument_pointer(ctxt, argument_stack_offset + 1);
    void* arg3 = get_argument_pointer(ctxt, argument_stack_offset + 2);
    void* arg4 = get_argument_pointer(ctxt, argument_stack_offset + 3);
    void* arg5 = get_argument_pointer(ctxt, argument_stack_offset + 4);
    void* arg6 = get_argument_pointer(ctxt, argument_stack_offset + 5);
    SCHEMERLICHT_CALL_EXTERNAL((void*, void*, void*, void*, void*, void*), (arg1, arg2, arg3, arg4, arg5, arg6));
    break;
    }
    case 7:
    {
    void* arg1 = get_argument_pointer(ctxt, argument_stack_offset);
    void* arg2 = get_argument_pointer(ctxt, argument_stack_offset + 1);
    void* arg3 = get_argument_pointer(ctxt, argument_stack_offset + 2);
    void* arg4 = get_argument_pointer(ctxt, argument_stack_offset + 3);
    void* arg5 = get_argument_pointer(ctxt, argument_stack_offset + 4);
    void* arg6 = get_argument_pointer(ctxt, argument_stack_offset + 5);
    void* arg7 = get_argument_pointer(ctxt, argument_stack_offset + 6);
    SCHEMERLICHT_CALL_EXTERNAL((void*, void*, void*, void*, void*, void*, void*), (arg1, arg2, arg3, arg4, arg5, arg6, arg7));
    break;
    }
    case 8:
    {
    void* arg1 = get_argument_pointer(ctxt, argument_stack_offset);
    void* arg2 = get_argument_pointer(ctxt, argument_stack_offset + 1);
    void* arg3 = get_argument_pointer(ctxt, argument_stack_offset + 2);
    void* arg4 = get_argument_pointer(ctxt, argument_stack_offset + 3);
    void* arg5 = get_argument_pointer(ctxt, argument_stack_offset + 4);
    void* arg6 = get_argument_pointer(ctxt, argument_stack_offset + 5);
    void* arg7 = get_argument_pointer(ctxt, argument_stack_offset + 6);
    void* arg8 = get_argument_pointer(ctxt, argument_stack_offset + 7);
    SCHEMERLICHT_CALL_EXTERNAL((void*, void*, void*, void*, void*, void*, void*, void*), (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
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