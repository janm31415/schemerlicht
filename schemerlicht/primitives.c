#include "primitives.h"
#include "context.h"
#include "object.h"
#include "error.h"

void schemerlicht_primitive_add1(schemerlicht_context* ctxt, int a, int b, int c)
  {    
  //R(A), ... ,R(A+C-2) := R(A)(R(A+1), ... ,R(A+B-1))

  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);

  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_ADD1);

  schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a+1, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_nil;
  switch (arg->type)
    {
    case schemerlicht_object_type_fixnum:
      ret.type = schemerlicht_object_type_fixnum;
      ret.value.fx = arg->value.fx + 1;
      break;
    case schemerlicht_object_type_flonum:
      ret.type = schemerlicht_object_type_flonum;
      ret.value.fl = arg->value.fl + 1;
      break;
    default:
      break;
    }
  schemerlicht_set_object(ra, &ret);  
  }



void schemerlicht_call_primitive(schemerlicht_context* ctxt, schemerlicht_fixnum prim_id, int a, int b, int c)
  {
  switch (prim_id)
    {
    case SCHEMERLICHT_ADD1:
      schemerlicht_primitive_add1(ctxt, a, b, c);
      break;
    default:
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
      break;
    }
  }