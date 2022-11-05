#include "primitives.h"
#include "context.h"
#include "object.h"
#include "error.h"
#include "gc.h"
#include "func.h"
#include "vm.h"

#include <string.h>
#include <time.h>

void schemerlicht_primitive_add1(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);

  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_ADD1);

  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    ret.value.fx = 0;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
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
      case schemerlicht_object_type_char:
        ret.type = schemerlicht_object_type_char;
        ret.value.ch = arg->value.ch + 1;
        break;
      default:
        break;
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fxadd1(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);

  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXADD1);
#if 1
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    ret.value.fx = 0;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object ret;
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_fixnum;
      ret.value.fx = arg->value.fx + 1;
      }
    else
      ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
#else
  schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_fixnum;
  ret.value.fx = arg->value.fx + 1;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fladd1(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);

  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLADD1);
#if 1
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_flonum;
    ret.value.fl = 0;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object ret;
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_flonum)
      {
      ret.type = schemerlicht_object_type_flonum;
      ret.value.fl = arg->value.fl + 1;
      }
    else
      ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
#else
  schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_flonum;
  ret.value.fl = arg->value.fl + 1.0;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_sub1(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);

  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_SUB1);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    ret.value.fx = 0;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        ret.type = schemerlicht_object_type_fixnum;
        ret.value.fx = arg->value.fx - 1;
        break;
      case schemerlicht_object_type_flonum:
        ret.type = schemerlicht_object_type_flonum;
        ret.value.fl = arg->value.fl - 1;
        break;
      case schemerlicht_object_type_char:
        ret.type = schemerlicht_object_type_char;
        ret.value.ch = arg->value.ch - 1;
        break;
      default:
        break;
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fxsub1(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);

  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXSUB1);
#if 1
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    ret.value.fx = 0;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object ret;
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_fixnum;
      ret.value.fx = arg->value.fx - 1;
      }
    else
      ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
#else
  schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_fixnum;
  ret.value.fx = arg->value.fx - 1;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flsub1(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);

  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLSUB1);
#if 1
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_flonum;
    ret.value.fl = 0;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object ret;
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_flonum)
      {
      ret.type = schemerlicht_object_type_flonum;
      ret.value.fl = arg->value.fl - 1;
      }
    else
      ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
#else
  schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_flonum;
  ret.value.fl = arg->value.fl - 1.0;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_add(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_ADD);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    ret.value.fx = 0;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    int type = schemerlicht_object_type_char;
    for (int j = 0; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
      if (arg->type != schemerlicht_object_type_fixnum && arg->type != schemerlicht_object_type_flonum && arg->type != schemerlicht_object_type_char)
        {
        schemerlicht_object ret;
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_set_object(ra, &ret);
        return;
        }
      if (type < arg->type)
        type = arg->type;
      }
    schemerlicht_object ret;
    ret.type = type;
    switch (type)
      {
      case schemerlicht_object_type_fixnum:
      {
      ret.value.fx = 0;
      for (int j = 0; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
        switch (arg->type)
          {
          case schemerlicht_object_type_fixnum:
            ret.value.fx += arg->value.fx;
            break;
          case schemerlicht_object_type_char:
            ret.value.fx += cast(schemerlicht_fixnum, arg->value.ch);
            break;
          default:
            schemerlicht_assert(0);
            break;
          }
        }
      break;
      }
      case schemerlicht_object_type_flonum:
      {
      ret.value.fl = 0;
      for (int j = 0; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
        switch (arg->type)
          {
          case schemerlicht_object_type_flonum:
            ret.value.fl += arg->value.fl;
            break;
          case schemerlicht_object_type_fixnum:
            ret.value.fl += cast(schemerlicht_flonum, arg->value.fx);
            break;
          case schemerlicht_object_type_char:
            ret.value.fl += cast(schemerlicht_flonum, arg->value.ch);
            break;
          default:
            schemerlicht_assert(0);
            break;
          }
        }
      break;
      }
      case schemerlicht_object_type_char:
      {
      ret.value.ch = 0;
      for (int j = 0; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
        ret.value.ch += arg->value.ch;
        }
      break;
      }
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fxadd(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXADD);
#if 1
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    ret.value.fx = 0;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    ret.value.fx = 0;
    for (int j = 0; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
      if (arg->type != schemerlicht_object_type_fixnum)
        {
        ret.type = schemerlicht_object_type_undefined;
        break;
        }
      ret.value.fx += arg->value.fx;
      }
    schemerlicht_set_object(ra, &ret);
    }
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_fixnum;
  ret.value.fx = arg0->value.fx + arg1->value.fx;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fladd(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLADD);
#if 1
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_flonum;
    ret.value.fl = 0;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_flonum;
    ret.value.fl = 0;
    for (int j = 0; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
      if (arg->type != schemerlicht_object_type_flonum)
        {
        ret.type = schemerlicht_object_type_undefined;
        break;
        }
      ret.value.fl += arg->value.fl;
      }
    schemerlicht_set_object(ra, &ret);
    }
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_flonum;
  ret.value.fl = arg0->value.fl + arg1->value.fl;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_sub(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_SUB);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    ret.value.fx = 0;
    schemerlicht_set_object(ra, &ret);
    }
  else if (b == 1)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (arg->type != schemerlicht_object_type_fixnum && arg->type != schemerlicht_object_type_flonum && arg->type != schemerlicht_object_type_char)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    schemerlicht_object ret;
    ret.type = arg->type;
    switch (arg->type)
      {
      case schemerlicht_object_type_flonum:
        ret.value.fl = -arg->value.fl;
        break;
      case schemerlicht_object_type_fixnum:
        ret.value.fx = -arg->value.fx;
        break;
      case schemerlicht_object_type_char:
        ret.value.ch = -arg->value.ch;
        break;
      default:
        schemerlicht_assert(0);
        break;
      }
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    int type = schemerlicht_object_type_char;
    for (int j = 0; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
      if (arg->type != schemerlicht_object_type_fixnum && arg->type != schemerlicht_object_type_flonum && arg->type != schemerlicht_object_type_char)
        {
        schemerlicht_object ret;
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_set_object(ra, &ret);
        return;
        }
      if (type < arg->type)
        type = arg->type;
      }
    schemerlicht_object ret;
    ret.type = type;
    switch (type)
      {
      case schemerlicht_object_type_fixnum:
      {
      schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
      switch (first_arg->type)
        {
        case schemerlicht_object_type_fixnum:
          ret.value.fx = first_arg->value.fx;
          break;
        case schemerlicht_object_type_char:
          ret.value.fx = cast(schemerlicht_fixnum, first_arg->value.ch);
          break;
        default:
          schemerlicht_assert(0);
          return;
        }
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
        switch (arg->type)
          {
          case schemerlicht_object_type_fixnum:
            ret.value.fx -= arg->value.fx;
            break;
          case schemerlicht_object_type_char:
            ret.value.fx -= cast(schemerlicht_fixnum, arg->value.ch);
            break;
          default:
            schemerlicht_assert(0);
            break;
          }
        }
      break;
      }
      case schemerlicht_object_type_flonum:
      {
      schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
      switch (first_arg->type)
        {
        case schemerlicht_object_type_flonum:
          ret.value.fl = first_arg->value.fl;
          break;
        case schemerlicht_object_type_fixnum:
          ret.value.fl = cast(schemerlicht_flonum, first_arg->value.fx);
          break;
        case schemerlicht_object_type_char:
          ret.value.fl = cast(schemerlicht_flonum, first_arg->value.ch);
          break;
        default:
          schemerlicht_assert(0);
          return;
        }
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
        switch (arg->type)
          {
          case schemerlicht_object_type_flonum:
            ret.value.fl -= arg->value.fl;
            break;
          case schemerlicht_object_type_fixnum:
            ret.value.fl -= cast(schemerlicht_flonum, arg->value.fx);
            break;
          case schemerlicht_object_type_char:
            ret.value.fl -= cast(schemerlicht_flonum, arg->value.ch);
            break;
          default:
            schemerlicht_assert(0);
            break;
          }
        }
      break;
      }
      case schemerlicht_object_type_char:
      {
      schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
      ret.value.ch = first_arg->value.ch;
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
        ret.value.ch -= arg->value.ch;
        }
      break;
      }
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fxsub(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXSUB);
#if 1
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    ret.value.fx = 0;
    schemerlicht_set_object(ra, &ret);
    }
  else if (b == 1)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (arg->type != schemerlicht_object_type_fixnum)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    ret.value.fx = -arg->value.fx;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    ret.value.fx = first_arg->value.fx;
    for (int j = 1; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
      if (arg->type != schemerlicht_object_type_fixnum)
        {
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_set_object(ra, &ret);
        return;
        }
      ret.value.fx -= arg->value.fx;
      }
    schemerlicht_set_object(ra, &ret);
    }
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_fixnum;
  ret.value.fx = arg0->value.fx - arg1->value.fx;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flsub(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLSUB);
#if 1
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_flonum;
    ret.value.fl = 0;
    schemerlicht_set_object(ra, &ret);
    }
  else if (b == 1)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (arg->type != schemerlicht_object_type_flonum)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_flonum;
    ret.value.fl = -arg->value.fl;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_flonum;
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_flonum)
      {
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    ret.value.fl = first_arg->value.fl;
    for (int j = 1; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
      if (arg->type != schemerlicht_object_type_flonum)
        {
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_set_object(ra, &ret);
        return;
        }
      ret.value.fl -= arg->value.fl;
      }
    schemerlicht_set_object(ra, &ret);
    }
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_flonum;
  ret.value.fl = arg0->value.fl - arg1->value.fl;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_mul(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_MUL);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    ret.value.fx = 1;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    int type = schemerlicht_object_type_char;
    for (int j = 0; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
      if (arg->type != schemerlicht_object_type_fixnum && arg->type != schemerlicht_object_type_flonum && arg->type != schemerlicht_object_type_char)
        {
        schemerlicht_object ret;
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_set_object(ra, &ret);
        return;
        }
      if (type < arg->type)
        type = arg->type;
      }
    schemerlicht_object ret;
    ret.type = type;
    switch (type)
      {
      case schemerlicht_object_type_fixnum:
      {
      schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
      switch (first_arg->type)
        {
        case schemerlicht_object_type_fixnum:
          ret.value.fx = first_arg->value.fx;
          break;
        case schemerlicht_object_type_char:
          ret.value.fx = cast(schemerlicht_fixnum, first_arg->value.ch);
          break;
        default:
          schemerlicht_assert(0);
          return;
        }
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
        switch (arg->type)
          {
          case schemerlicht_object_type_fixnum:
            ret.value.fx *= arg->value.fx;
            break;
          case schemerlicht_object_type_char:
            ret.value.fx *= cast(schemerlicht_fixnum, arg->value.ch);
            break;
          default:
            schemerlicht_assert(0);
            break;
          }
        }
      break;
      }
      case schemerlicht_object_type_flonum:
      {
      schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
      switch (first_arg->type)
        {
        case schemerlicht_object_type_flonum:
          ret.value.fl = first_arg->value.fl;
          break;
        case schemerlicht_object_type_fixnum:
          ret.value.fl = cast(schemerlicht_flonum, first_arg->value.fx);
          break;
        case schemerlicht_object_type_char:
          ret.value.fl = cast(schemerlicht_flonum, first_arg->value.ch);
          break;
        default:
          schemerlicht_assert(0);
          return;
        }
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
        switch (arg->type)
          {
          case schemerlicht_object_type_flonum:
            ret.value.fl *= arg->value.fl;
            break;
          case schemerlicht_object_type_fixnum:
            ret.value.fl *= cast(schemerlicht_flonum, arg->value.fx);
            break;
          case schemerlicht_object_type_char:
            ret.value.fl *= cast(schemerlicht_flonum, arg->value.ch);
            break;
          default:
            schemerlicht_assert(0);
            return;
          }
        }
      break;
      }
      case schemerlicht_object_type_char:
      {
      schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
      ret.value.ch = first_arg->value.ch;
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
        ret.value.ch *= arg->value.ch;
        }
      break;
      }
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fxmul(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXMUL);
#if 1
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    ret.value.fx = 1;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    ret.value.fx = first_arg->value.fx;
    for (int j = 1; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
      if (arg->type != schemerlicht_object_type_fixnum)
        {
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_set_object(ra, &ret);
        return;
        }
      ret.value.fx *= arg->value.fx;
      }
    schemerlicht_set_object(ra, &ret);
    }
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_fixnum;
  ret.value.fx = arg0->value.fx * arg1->value.fx;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flmul(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLMUL);
#if 1
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_flonum;
    ret.value.fl = 1;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_flonum;
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_flonum)
      {
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    ret.value.fl = first_arg->value.fl;
    for (int j = 1; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
      if (arg->type != schemerlicht_object_type_flonum)
        {
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_set_object(ra, &ret);
        return;
        }
      ret.value.fl *= arg->value.fl;
      }
    schemerlicht_set_object(ra, &ret);
    }
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_flonum;
  ret.value.fl = arg0->value.fl * arg1->value.fl;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_div(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_DIV);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    ret.value.fx = 1;
    schemerlicht_set_object(ra, &ret);
    }
  else if (b == 1)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (arg->type != schemerlicht_object_type_fixnum && arg->type != schemerlicht_object_type_flonum && arg->type != schemerlicht_object_type_char)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    schemerlicht_object ret;
    ret.type = arg->type;
    switch (arg->type)
      {
      case schemerlicht_object_type_flonum:
        ret.value.fl = 1.0 / arg->value.fl;
        break;
      case schemerlicht_object_type_fixnum:
        ret.value.fx = 1 / arg->value.fx;
        break;
      case schemerlicht_object_type_char:
        ret.value.ch = 1 / arg->value.ch;
        break;
      default:
        schemerlicht_assert(0);
        break;
      }
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    int type = schemerlicht_object_type_char;
    for (int j = 0; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
      if (arg->type != schemerlicht_object_type_fixnum && arg->type != schemerlicht_object_type_flonum && arg->type != schemerlicht_object_type_char)
        {
        schemerlicht_object ret;
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_set_object(ra, &ret);
        return;
        }
      if (type < arg->type)
        type = arg->type;
      }
    schemerlicht_object ret;
    ret.type = type;
    switch (type)
      {
      case schemerlicht_object_type_fixnum:
      {
      schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
      switch (first_arg->type)
        {
        case schemerlicht_object_type_fixnum:
          ret.value.fx = first_arg->value.fx;
          break;
        case schemerlicht_object_type_char:
          ret.value.fx = cast(schemerlicht_fixnum, first_arg->value.ch);
          break;
        default:
          schemerlicht_assert(0);
          return;
        }
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
        switch (arg->type)
          {
          case schemerlicht_object_type_fixnum:
            ret.value.fx /= arg->value.fx;
            break;
          case schemerlicht_object_type_char:
            ret.value.fx /= cast(schemerlicht_fixnum, arg->value.ch);
            break;
          default:
            schemerlicht_assert(0);
            break;
          }
        }
      break;
      }
      case schemerlicht_object_type_flonum:
      {
      schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
      switch (first_arg->type)
        {
        case schemerlicht_object_type_flonum:
          ret.value.fl = first_arg->value.fl;
          break;
        case schemerlicht_object_type_fixnum:
          ret.value.fl = cast(schemerlicht_flonum, first_arg->value.fx);
          break;
        case schemerlicht_object_type_char:
          ret.value.fl = cast(schemerlicht_flonum, first_arg->value.ch);
          break;
        default:
          schemerlicht_assert(0);
          return;
        }
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
        switch (arg->type)
          {
          case schemerlicht_object_type_flonum:
            ret.value.fl /= arg->value.fl;
            break;
          case schemerlicht_object_type_fixnum:
            ret.value.fl /= cast(schemerlicht_flonum, arg->value.fx);
            break;
          case schemerlicht_object_type_char:
            ret.value.fl /= cast(schemerlicht_flonum, arg->value.ch);
            break;
          default:
            schemerlicht_assert(0);
            break;
          }
        }
      break;
      }
      case schemerlicht_object_type_char:
      {
      schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
      ret.value.ch = first_arg->value.ch;
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
        ret.value.ch /= arg->value.ch;
        }
      break;
      }
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fxdiv(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXDIV);
#if 1
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    ret.value.fx = 1;
    schemerlicht_set_object(ra, &ret);
    }
  else if (b == 1)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (arg->type != schemerlicht_object_type_fixnum)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    ret.value.fx = 1 / arg->value.fx;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    ret.value.fx = first_arg->value.fx;
    for (int j = 1; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
      if (arg->type != schemerlicht_object_type_fixnum)
        {
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_set_object(ra, &ret);
        return;
        }
      ret.value.fx /= arg->value.fx;
      }
    schemerlicht_set_object(ra, &ret);
    }
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_fixnum;
  ret.value.fx = arg0->value.fx / arg1->value.fx;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fldiv(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLDIV);
#if 1
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_flonum;
    ret.value.fl = 1;
    schemerlicht_set_object(ra, &ret);
    }
  else if (b == 1)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (arg->type != schemerlicht_object_type_flonum)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    ret.value.fl = 1.0 / arg->value.fl;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_flonum;
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_flonum)
      {
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    ret.value.fl = first_arg->value.fl;
    for (int j = 1; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
      if (arg->type != schemerlicht_object_type_flonum)
        {
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_set_object(ra, &ret);
        return;
        }
      ret.value.fl /= arg->value.fl;
      }
    schemerlicht_set_object(ra, &ret);
    }
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_flonum;
  ret.value.fl = arg0->value.fl / arg1->value.fl;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_equal(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  //schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  //schemerlicht_assert(ra->value.fx == SCHEMERLICHT_EQUAL);
  if (b > 1)
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    switch (first_arg->type)
      {
      case schemerlicht_object_type_fixnum:
      {
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
        switch (arg->type)
          {
          case schemerlicht_object_type_fixnum:
          {
          if (first_arg->value.fx != arg->value.fx)
            {
            schemerlicht_object ret;
            ret.type = schemerlicht_object_type_false;
            schemerlicht_set_object(ra, &ret);
            return;
            }
          break;
          }
          case schemerlicht_object_type_flonum:
          {
          if (first_arg->value.fx != arg->value.fl)
            {
            schemerlicht_object ret;
            ret.type = schemerlicht_object_type_false;
            schemerlicht_set_object(ra, &ret);
            return;
            }
          break;
          }
          case schemerlicht_object_type_char:
          {
          if (first_arg->value.fx != arg->value.ch)
            {
            schemerlicht_object ret;
            ret.type = schemerlicht_object_type_false;
            schemerlicht_set_object(ra, &ret);
            return;
            }
          break;
          }
          default:
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_undefined;
          schemerlicht_set_object(ra, &ret);
          return;
          }
          }
        }
      break;
      }
      case schemerlicht_object_type_flonum:
      {
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
        switch (arg->type)
          {
          case schemerlicht_object_type_fixnum:
          {
          if (first_arg->value.fl != arg->value.fx)
            {
            schemerlicht_object ret;
            ret.type = schemerlicht_object_type_false;
            schemerlicht_set_object(ra, &ret);
            return;
            }
          break;
          }
          case schemerlicht_object_type_flonum:
          {
          if (first_arg->value.fl != arg->value.fl)
            {
            schemerlicht_object ret;
            ret.type = schemerlicht_object_type_false;
            schemerlicht_set_object(ra, &ret);
            return;
            }
          break;
          }
          case schemerlicht_object_type_char:
          {
          if (first_arg->value.fl != arg->value.ch)
            {
            schemerlicht_object ret;
            ret.type = schemerlicht_object_type_false;
            schemerlicht_set_object(ra, &ret);
            return;
            }
          break;
          }
          default:
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_undefined;
          schemerlicht_set_object(ra, &ret);
          return;
          }
          }
        }
      break;
      }
      case schemerlicht_object_type_char:
      {
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
        switch (arg->type)
          {
          case schemerlicht_object_type_fixnum:
          {
          if (first_arg->value.ch != arg->value.fx)
            {
            schemerlicht_object ret;
            ret.type = schemerlicht_object_type_false;
            schemerlicht_set_object(ra, &ret);
            return;
            }
          break;
          }
          case schemerlicht_object_type_flonum:
          {
          if (first_arg->value.ch != arg->value.fl)
            {
            schemerlicht_object ret;
            ret.type = schemerlicht_object_type_false;
            schemerlicht_set_object(ra, &ret);
            return;
            }
          break;
          }
          case schemerlicht_object_type_char:
          {
          if (first_arg->value.ch != arg->value.ch)
            {
            schemerlicht_object ret;
            ret.type = schemerlicht_object_type_false;
            schemerlicht_set_object(ra, &ret);
            return;
            }
          break;
          }
          default:
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_undefined;
          schemerlicht_set_object(ra, &ret);
          return;
          }
          }
        }
      break;
      }
      default:
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
      }
    }
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_true;
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fxequal(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXEQUAL);
#if 1
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_true;
    schemerlicht_set_object(ra, &ret);
    return;
    }
  schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  if (first_arg->type != schemerlicht_object_type_fixnum)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    return;
    }
  for (int j = 1; j < b; ++j)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
    if (arg->type != schemerlicht_object_type_fixnum)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    if (first_arg->value.fx != arg->value.fx)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_false;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    }
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_true;
  schemerlicht_set_object(ra, &ret);
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = arg0->value.fx == arg1->value.fx ? schemerlicht_object_type_true : schemerlicht_object_type_false;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flequal(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLEQUAL);
#if 1
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_true;
    schemerlicht_set_object(ra, &ret);
    return;
    }
  schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  if (first_arg->type != schemerlicht_object_type_flonum)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    return;
    }
  for (int j = 1; j < b; ++j)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
    if (arg->type != schemerlicht_object_type_flonum)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    if (first_arg->value.fl != arg->value.fl)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_false;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    }
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_true;
  schemerlicht_set_object(ra, &ret);
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = arg0->value.fl == arg1->value.fl ? schemerlicht_object_type_true : schemerlicht_object_type_false;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_charequal(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHAREQUAL);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_true;
    schemerlicht_set_object(ra, &ret);
    return;
    }
  schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  if (first_arg->type != schemerlicht_object_type_char)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    return;
    }
  for (int j = 1; j < b; ++j)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
    if (arg->type != schemerlicht_object_type_char)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    if (first_arg->value.ch != arg->value.ch)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_false;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    }
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_true;
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_not_equal(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_NOT_EQUAL);
  schemerlicht_primitive_equal(ctxt, a, b, c);
  if (ra->type == schemerlicht_object_type_true)
    {
    ra->type = schemerlicht_object_type_false;
    }
  else if (ra->type == schemerlicht_object_type_false)
    {
    ra->type = schemerlicht_object_type_true;
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_less(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  //schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  //schemerlicht_assert(ra->value.fx == SCHEMERLICHT_LESS);
  for (int j = 0; j < b - 1; ++j)
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
    schemerlicht_object* second_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + 1, schemerlicht_object);
    switch (first_arg->type)
      {
      case schemerlicht_object_type_fixnum:
      {
      switch (second_arg->type)
        {
        case schemerlicht_object_type_fixnum:
        {
        if (first_arg->value.fx >= second_arg->value.fx)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &ret);
          return;
          }
        break;
        }
        case schemerlicht_object_type_flonum:
        {
        if (first_arg->value.fx >= second_arg->value.fl)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &ret);
          return;
          }
        break;
        }
        case schemerlicht_object_type_char:
        {
        if (first_arg->value.fx >= second_arg->value.ch)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &ret);
          return;
          }
        break;
        }
        default:
        {
        schemerlicht_object ret;
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_set_object(ra, &ret);
        return;
        }

        }
      break;
      }
      case schemerlicht_object_type_flonum:
      {

      switch (second_arg->type)
        {
        case schemerlicht_object_type_fixnum:
        {
        if (first_arg->value.fl >= second_arg->value.fx)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &ret);
          return;
          }
        break;
        }
        case schemerlicht_object_type_flonum:
        {
        if (first_arg->value.fl >= second_arg->value.fl)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &ret);
          return;
          }
        break;
        }
        case schemerlicht_object_type_char:
        {
        if (first_arg->value.fl >= second_arg->value.ch)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &ret);
          return;
          }
        break;
        }
        default:
        {
        schemerlicht_object ret;
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_set_object(ra, &ret);
        return;
        }

        }
      break;
      }
      case schemerlicht_object_type_char:
      {
      switch (second_arg->type)
        {
        case schemerlicht_object_type_fixnum:
        {
        if (first_arg->value.ch >= second_arg->value.fx)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &ret);
          return;
          }
        break;
        }
        case schemerlicht_object_type_flonum:
        {
        if (first_arg->value.ch >= second_arg->value.fl)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &ret);
          return;
          }
        break;
        }
        case schemerlicht_object_type_char:
        {
        if (first_arg->value.ch >= second_arg->value.ch)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &ret);
          return;
          }
        break;
        }
        default:
        {
        schemerlicht_object ret;
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_set_object(ra, &ret);
        return;
        }
        }
      break;
      }
      default:
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
      }
    }
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_true;
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fxless(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
#if 1
  for (int j = 0; j < b - 1; ++j)
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
    schemerlicht_object* second_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + 1, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_fixnum || second_arg->type != schemerlicht_object_type_fixnum)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    if (first_arg->value.fx >= second_arg->value.fx)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_false;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    }
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_true;
  schemerlicht_set_object(ra, &ret);
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = arg0->value.fx < arg1->value.fx ? schemerlicht_object_type_true : schemerlicht_object_type_false;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flless(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
#if 1
  for (int j = 0; j < b - 1; ++j)
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
    schemerlicht_object* second_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + 1, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_flonum || second_arg->type != schemerlicht_object_type_flonum)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    if (first_arg->value.fl >= second_arg->value.fl)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_false;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    }
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_true;
  schemerlicht_set_object(ra, &ret);
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = arg0->value.fl < arg1->value.fl ? schemerlicht_object_type_true : schemerlicht_object_type_false;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_charless(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  for (int j = 0; j < b - 1; ++j)
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
    schemerlicht_object* second_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + 1, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_char || second_arg->type != schemerlicht_object_type_char)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    if (first_arg->value.ch >= second_arg->value.ch)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_false;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    }
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_true;
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_geq(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_GEQ);
  schemerlicht_primitive_less(ctxt, a, b, c);
  if (ra->type == schemerlicht_object_type_true)
    {
    ra->type = schemerlicht_object_type_false;
    }
  else if (ra->type == schemerlicht_object_type_false)
    {
    ra->type = schemerlicht_object_type_true;
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fxgeq(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
#if 1
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXGEQ);
  schemerlicht_primitive_fxless(ctxt, a, b, c);
  if (ra->type == schemerlicht_object_type_true)
    {
    ra->type = schemerlicht_object_type_false;
    }
  else if (ra->type == schemerlicht_object_type_false)
    {
    ra->type = schemerlicht_object_type_true;
    }
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = arg0->value.fx >= arg1->value.fx ? schemerlicht_object_type_true : schemerlicht_object_type_false;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flgeq(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
#if 1
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLGEQ);
  schemerlicht_primitive_flless(ctxt, a, b, c);
  if (ra->type == schemerlicht_object_type_true)
    {
    ra->type = schemerlicht_object_type_false;
    }
  else if (ra->type == schemerlicht_object_type_false)
    {
    ra->type = schemerlicht_object_type_true;
    }
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = arg0->value.fl >= arg1->value.fl ? schemerlicht_object_type_true : schemerlicht_object_type_false;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_chargeq(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHARGEQ);
  schemerlicht_primitive_charless(ctxt, a, b, c);
  if (ra->type == schemerlicht_object_type_true)
    {
    ra->type = schemerlicht_object_type_false;
    }
  else if (ra->type == schemerlicht_object_type_false)
    {
    ra->type = schemerlicht_object_type_true;
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_greater(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  //schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  //schemerlicht_assert(ra->value.fx == SCHEMERLICHT_GREATER);
  for (int j = 0; j < b - 1; ++j)
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
    schemerlicht_object* second_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + 1, schemerlicht_object);
    switch (first_arg->type)
      {
      case schemerlicht_object_type_fixnum:
      {
      switch (second_arg->type)
        {
        case schemerlicht_object_type_fixnum:
        {
        if (first_arg->value.fx <= second_arg->value.fx)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &ret);
          return;
          }
        break;
        }
        case schemerlicht_object_type_flonum:
        {
        if (first_arg->value.fx <= second_arg->value.fl)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &ret);
          return;
          }
        break;
        }
        case schemerlicht_object_type_char:
        {
        if (first_arg->value.fx <= second_arg->value.ch)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &ret);
          return;
          }
        break;
        }
        default:
        {
        schemerlicht_object ret;
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_set_object(ra, &ret);
        return;
        }

        }
      break;
      }
      case schemerlicht_object_type_flonum:
      {

      switch (second_arg->type)
        {
        case schemerlicht_object_type_fixnum:
        {
        if (first_arg->value.fl <= second_arg->value.fx)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &ret);
          return;
          }
        break;
        }
        case schemerlicht_object_type_flonum:
        {
        if (first_arg->value.fl <= second_arg->value.fl)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &ret);
          return;
          }
        break;
        }
        case schemerlicht_object_type_char:
        {
        if (first_arg->value.fl <= second_arg->value.ch)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &ret);
          return;
          }
        break;
        }
        default:
        {
        schemerlicht_object ret;
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_set_object(ra, &ret);
        return;
        }

        }
      break;
      }
      case schemerlicht_object_type_char:
      {
      switch (second_arg->type)
        {
        case schemerlicht_object_type_fixnum:
        {
        if (first_arg->value.ch <= second_arg->value.fx)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &ret);
          return;
          }
        break;
        }
        case schemerlicht_object_type_flonum:
        {
        if (first_arg->value.ch <= second_arg->value.fl)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &ret);
          return;
          }
        break;
        }
        case schemerlicht_object_type_char:
        {
        if (first_arg->value.ch <= second_arg->value.ch)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &ret);
          return;
          }
        break;
        }
        default:
        {
        schemerlicht_object ret;
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_set_object(ra, &ret);
        return;
        }
        }
      break;
      }
      default:
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
      }
    }
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_true;
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fxgreater(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
#if 1
  for (int j = 0; j < b - 1; ++j)
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
    schemerlicht_object* second_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + 1, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_fixnum || second_arg->type != schemerlicht_object_type_fixnum)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    if (first_arg->value.fx <= second_arg->value.fx)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_false;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    }
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_true;
  schemerlicht_set_object(ra, &ret);
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = arg0->value.fx > arg1->value.fx ? schemerlicht_object_type_true : schemerlicht_object_type_false;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flgreater(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
#if 1
  for (int j = 0; j < b - 1; ++j)
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
    schemerlicht_object* second_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + 1, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_flonum || second_arg->type != schemerlicht_object_type_flonum)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    if (first_arg->value.fl <= second_arg->value.fl)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_false;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    }
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_true;
  schemerlicht_set_object(ra, &ret);
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = arg0->value.fl > arg1->value.fl ? schemerlicht_object_type_true : schemerlicht_object_type_false;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_chargreater(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  for (int j = 0; j < b - 1; ++j)
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
    schemerlicht_object* second_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + 1, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_char || second_arg->type != schemerlicht_object_type_char)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    if (first_arg->value.ch <= second_arg->value.ch)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_false;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    }
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_true;
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_leq(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_LEQ);
  schemerlicht_primitive_greater(ctxt, a, b, c);
  if (ra->type == schemerlicht_object_type_true)
    {
    ra->type = schemerlicht_object_type_false;
    }
  else if (ra->type == schemerlicht_object_type_false)
    {
    ra->type = schemerlicht_object_type_true;
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fxleq(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
#if 1
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXLEQ);
  schemerlicht_primitive_fxgreater(ctxt, a, b, c);
  if (ra->type == schemerlicht_object_type_true)
    {
    ra->type = schemerlicht_object_type_false;
    }
  else if (ra->type == schemerlicht_object_type_false)
    {
    ra->type = schemerlicht_object_type_true;
    }
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = arg0->value.fx <= arg1->value.fx ? schemerlicht_object_type_true : schemerlicht_object_type_false;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flleq(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
#if 1
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLLEQ);
  schemerlicht_primitive_flgreater(ctxt, a, b, c);
  if (ra->type == schemerlicht_object_type_true)
    {
    ra->type = schemerlicht_object_type_false;
    }
  else if (ra->type == schemerlicht_object_type_false)
    {
    ra->type = schemerlicht_object_type_true;
    }
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = arg0->value.fl <= arg1->value.fl ? schemerlicht_object_type_true : schemerlicht_object_type_false;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_charleq(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHARLEQ);
  schemerlicht_primitive_chargreater(ctxt, a, b, c);
  if (ra->type == schemerlicht_object_type_true)
    {
    ra->type = schemerlicht_object_type_false;
    }
  else if (ra->type == schemerlicht_object_type_false)
    {
    ra->type = schemerlicht_object_type_true;
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_fixnum(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_FIXNUM);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object ret;
    if (first_arg->type == schemerlicht_object_type_fixnum)
      ret.type = schemerlicht_object_type_true;
    else
      ret.type = schemerlicht_object_type_false;
    schemerlicht_set_object(ra, &ret);
    }
  }
////////////////////////////////////////////////////
void schemerlicht_primitive_is_flonum(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_FLONUM);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object ret;
    if (first_arg->type == schemerlicht_object_type_flonum)
      ret.type = schemerlicht_object_type_true;
    else
      ret.type = schemerlicht_object_type_false;
    schemerlicht_set_object(ra, &ret);
    }
  }
////////////////////////////////////////////////////
void schemerlicht_primitive_is_null(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_NULL);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object ret;
    if (first_arg->type == schemerlicht_object_type_nil)
      ret.type = schemerlicht_object_type_true;
    else
      ret.type = schemerlicht_object_type_false;
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_zero(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_ZERO);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_true;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_false;
    switch (first_arg->type)
      {
      case schemerlicht_object_type_fixnum:
        if (first_arg->value.fx == 0)
          ret.type = schemerlicht_object_type_true;
        break;
      case schemerlicht_object_type_flonum:
        if (first_arg->value.fl == 0)
          ret.type = schemerlicht_object_type_true;
        break;
      case schemerlicht_object_type_char:
        if (first_arg->value.ch == 0)
          ret.type = schemerlicht_object_type_true;
        break;
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fxzero(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXZERO);
#if 1
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_true;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_false;
    if (first_arg->type == schemerlicht_object_type_fixnum && first_arg->value.fx == 0)
      ret.type = schemerlicht_object_type_true;
    else
      ret.type = schemerlicht_object_type_false;
    schemerlicht_set_object(ra, &ret);
    }
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = arg0->value.fx == 0 ? schemerlicht_object_type_true : schemerlicht_object_type_false;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flzero(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLZERO);
#if 1
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_true;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_false;
    if (first_arg->type == schemerlicht_object_type_flonum && first_arg->value.fl == 0)
      ret.type = schemerlicht_object_type_true;
    else
      ret.type = schemerlicht_object_type_false;
    schemerlicht_set_object(ra, &ret);
    }
#else
  schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object ret;
  ret.type = arg0->value.fl == 0.0 ? schemerlicht_object_type_true : schemerlicht_object_type_false;
  schemerlicht_set_object(ra, &ret);
#endif
  }

////////////////////////////////////////////////////
void schemerlicht_primitive_is_boolean(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_BOOLEAN);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object ret;
    if (first_arg->type == schemerlicht_object_type_true || first_arg->type == schemerlicht_object_type_false)
      ret.type = schemerlicht_object_type_true;
    else
      ret.type = schemerlicht_object_type_false;
    schemerlicht_set_object(ra, &ret);
    }
  }
////////////////////////////////////////////////////
void schemerlicht_primitive_is_char(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_CHAR);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object ret;
    if (first_arg->type == schemerlicht_object_type_char)
      ret.type = schemerlicht_object_type_true;
    else
      ret.type = schemerlicht_object_type_false;
    schemerlicht_set_object(ra, &ret);
    }
  }
////////////////////////////////////////////////////
void schemerlicht_primitive_not(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_NOT);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_false;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object ret;
    if (first_arg->type == schemerlicht_object_type_false)
      ret.type = schemerlicht_object_type_true;
    else
      ret.type = schemerlicht_object_type_false;
    schemerlicht_set_object(ra, &ret);
    }
  }
////////////////////////////////////////////////////

void schemerlicht_primitive_fixnum_char(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FIXNUM_CHAR);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object ret;
    if (first_arg->type == schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_char;
      ret.value.ch = cast(schemerlicht_byte, first_arg->value.fx);
      }
    else
      {
      ret.type = schemerlicht_object_type_undefined;
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fixnum_flonum(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FIXNUM_FLONUM);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object ret;
    if (first_arg->type == schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_flonum;
      ret.value.fl = cast(double, first_arg->value.fx);
      }
    else
      {
      ret.type = schemerlicht_object_type_undefined;
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_char_fixnum(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHAR_FIXNUM);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object ret;
    if (first_arg->type == schemerlicht_object_type_char)
      {
      ret.type = schemerlicht_object_type_fixnum;
      ret.value.fx = cast(schemerlicht_fixnum, first_arg->value.ch);
      }
    else
      {
      ret.type = schemerlicht_object_type_undefined;
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flonum_fixnum(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLONUM_FIXNUM);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object ret;
    if (first_arg->type == schemerlicht_object_type_flonum)
      {
      ret.type = schemerlicht_object_type_fixnum;
      ret.value.fx = cast(schemerlicht_fixnum, first_arg->value.fl);
      }
    else
      {
      ret.type = schemerlicht_object_type_undefined;
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_bitwise_and(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_BITWISE_AND);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object ret;
    if (first_arg->type == schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_fixnum;
      ret.value.fx = first_arg->value.fx;
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* next_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
        if (next_arg->type == schemerlicht_object_type_fixnum)
          {
          ret.value.fx &= next_arg->value.fx;
          }
        else
          {
          ret.type = schemerlicht_object_type_undefined;
          break;
          }
        }
      }
    else
      {
      ret.type = schemerlicht_object_type_undefined;
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_bitwise_or(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_BITWISE_OR);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object ret;
    if (first_arg->type == schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_fixnum;
      ret.value.fx = first_arg->value.fx;
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* next_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
        if (next_arg->type == schemerlicht_object_type_fixnum)
          {
          ret.value.fx |= next_arg->value.fx;
          }
        else
          {
          ret.type = schemerlicht_object_type_undefined;
          break;
          }
        }
      }
    else
      {
      ret.type = schemerlicht_object_type_undefined;
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_bitwise_not(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_BITWISE_NOT);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object ret;
    if (first_arg->type == schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_fixnum;
      ret.value.fx = ~first_arg->value.fx;
      }
    else
      {
      ret.type = schemerlicht_object_type_undefined;
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_bitwise_xor(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_BITWISE_XOR);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object ret;
    if (first_arg->type == schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_fixnum;
      ret.value.fx = first_arg->value.fx;
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* next_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
        if (next_arg->type == schemerlicht_object_type_fixnum)
          {
          ret.value.fx ^= next_arg->value.fx;
          }
        else
          {
          ret.type = schemerlicht_object_type_undefined;
          break;
          }
        }
      }
    else
      {
      ret.type = schemerlicht_object_type_undefined;
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_arithmetic_shift(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_ARITHMETIC_SHIFT);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object ret;
    if (first_arg->type == schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_fixnum;
      ret.value.fx = first_arg->value.fx;
      if (b > 1)
        {
        schemerlicht_object* next_arg = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
        if (next_arg->type == schemerlicht_object_type_fixnum)
          {
          if (next_arg->value.fx >= 0)
            ret.value.fx <<= next_arg->value.fx;
          else
            ret.value.fx >>= (-next_arg->value.fx);
          }
        else
          {
          ret.type = schemerlicht_object_type_undefined;
          }
        }
      }
    else
      {
      ret.type = schemerlicht_object_type_undefined;
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_vector(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_VECTOR);
  schemerlicht_object v = make_schemerlicht_object_vector(ctxt, b);
  for (int j = 0; j < b; ++j)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
    schemerlicht_object* obj_at_pos = schemerlicht_vector_at(&v.value.v, j, schemerlicht_object);
    schemerlicht_set_object(obj_at_pos, arg);
    }
  schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
  schemerlicht_set_object(heap_obj, &v);
  ++ctxt->heap_pos;
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_vector_ref(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_VECTORREF);
  schemerlicht_object ret;
  if (b < 2)
    {
    ret.type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* pos = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    if (v->type != schemerlicht_object_type_vector || pos->type != schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_undefined;
      }
    else
      {
      if (pos->value.fx < 0 || pos->value.fx >= v->value.v.vector_size) // out of bounds
        {
        ret.type = schemerlicht_object_type_undefined;
        }
      else
        {
        ret = *schemerlicht_vector_at(&v->value.v, pos->value.fx, schemerlicht_object);
        }
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_vector_set(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_VECTORSET);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_void;
  if (b > 2)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* pos = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    schemerlicht_object* value = schemerlicht_vector_at(&ctxt->stack, a + 3 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_vector && pos->type == schemerlicht_object_type_fixnum)
      {
      if (pos->value.fx >= 0 && pos->value.fx < v->value.v.vector_size)
        {
        schemerlicht_object* value_to_change = schemerlicht_vector_at(&v->value.v, pos->value.fx, schemerlicht_object);
        schemerlicht_set_object(value_to_change, value);
        }
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_vector(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_VECTOR);
  schemerlicht_object ret;
  if (b > 0)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_vector)
      ret.type = schemerlicht_object_type_true;
    else
      ret.type = schemerlicht_object_type_false;
    }
  else
    {
    ret.type = schemerlicht_object_type_false;
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_pair(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_PAIR);
  schemerlicht_object ret;
  if (b > 0)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_pair)
      ret.type = schemerlicht_object_type_true;
    else
      ret.type = schemerlicht_object_type_false;
    }
  else
    {
    ret.type = schemerlicht_object_type_false;
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_cons(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CONS);
  schemerlicht_object v = make_schemerlicht_object_pair(ctxt);
  schemerlicht_object* v0 = schemerlicht_vector_at(&v.value.v, 0, schemerlicht_object);
  schemerlicht_object* v1 = schemerlicht_vector_at(&v.value.v, 1, schemerlicht_object);
  if (b == 0)
    {
    v0->type = schemerlicht_object_type_undefined;
    v1->type = schemerlicht_object_type_undefined;
    }
  else if (b == 1)
    {
    schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_set_object(v0, arg0);
    v1->type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    schemerlicht_set_object(v0, arg0);
    schemerlicht_set_object(v1, arg1);
    }
  schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
  schemerlicht_set_object(heap_obj, &v);
  ++ctxt->heap_pos;
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_list(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_LIST);
  if (b == 0)
    {
    schemerlicht_object v;
    v.type = schemerlicht_object_type_nil;
    schemerlicht_set_object(ra, &v);
    }
  else
    {
    schemerlicht_object obj1 = make_schemerlicht_object_pair(ctxt);
    schemerlicht_object* v0 = schemerlicht_vector_at(&obj1.value.v, 0, schemerlicht_object);
    schemerlicht_object* v1 = schemerlicht_vector_at(&obj1.value.v, 1, schemerlicht_object);
    v1->type = schemerlicht_object_type_nil;
    schemerlicht_object* last_arg = schemerlicht_vector_at(&ctxt->stack, a + b + c, schemerlicht_object);
    schemerlicht_set_object(v0, last_arg);
    schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
    schemerlicht_set_object(heap_obj, &obj1);
    ++ctxt->heap_pos;
    for (int j = b - 1; j >= 1; --j)
      {
      schemerlicht_object obj2 = make_schemerlicht_object_pair(ctxt);
      v0 = schemerlicht_vector_at(&obj2.value.v, 0, schemerlicht_object);
      v1 = schemerlicht_vector_at(&obj2.value.v, 1, schemerlicht_object);
      schemerlicht_set_object(v1, heap_obj);
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + j + c, schemerlicht_object);
      schemerlicht_set_object(v0, arg);
      heap_obj = &ctxt->heap[ctxt->heap_pos];
      schemerlicht_set_object(heap_obj, &obj2);
      ++ctxt->heap_pos;
      }
    schemerlicht_set_object(ra, heap_obj);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_cdr(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CDR);
  schemerlicht_object ret;
  if (b < 1)
    {
    ret.type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (v->type != schemerlicht_object_type_pair)
      {
      ret.type = schemerlicht_object_type_undefined;
      }
    else
      {
      ret = *schemerlicht_vector_at(&v->value.v, 1, schemerlicht_object);
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_car(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CAR);
  schemerlicht_object ret;
  if (b < 1)
    {
    ret.type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (v->type != schemerlicht_object_type_pair)
      {
      ret.type = schemerlicht_object_type_undefined;
      }
    else
      {
      ret = *schemerlicht_vector_at(&v->value.v, 0, schemerlicht_object);
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_halt(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  UNUSED(b);
  UNUSED(a);
  UNUSED(ctxt);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_closure(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CLOSURE);
  schemerlicht_object v = make_schemerlicht_object_closure(ctxt, b);
  schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
  schemerlicht_set_object(heap_obj, &v);
  ++ctxt->heap_pos;
  memcpy(heap_obj->value.v.vector_ptr, cast(schemerlicht_object*, ctxt->stack.vector_ptr) + a + c + 1, b * sizeof(schemerlicht_object));
#if 0
  for (int j = 0; j < b; ++j)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
    schemerlicht_object* obj_at_pos = schemerlicht_vector_at(&heap_obj->value.v, j, schemerlicht_object);
    schemerlicht_set_object(obj_at_pos, arg);
    }
  //schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
  //schemerlicht_set_object(heap_obj, &v);
  //++ctxt->heap_pos;
#endif
  schemerlicht_set_object(ra, heap_obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_closure_ref(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CLOSUREREF);
#if 0
  schemerlicht_object ret;
  if (b < 2)
    {
    ret.type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* pos = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    if (v->type != schemerlicht_object_type_closure || pos->type != schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_undefined;
      }
    else
      {
      if (pos->value.fx < 0 || pos->value.fx >= v->value.v.vector_size) // out of bounds
        {
        ret.type = schemerlicht_object_type_undefined;
        }
      else
        {
        ret = *schemerlicht_vector_at(&v->value.v, pos->value.fx, schemerlicht_object);
        }
      }
    }
  schemerlicht_set_object(ra, &ret);
#else
  schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  schemerlicht_object* pos = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
  schemerlicht_set_object(ra, schemerlicht_vector_at(&v->value.v, pos->value.fx, schemerlicht_object));
  UNUSED(b);
#endif
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_closure(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_CLOSURE);
  schemerlicht_object ret;
  if (b > 0)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_closure)
      ret.type = schemerlicht_object_type_true;
    else
      ret.type = schemerlicht_object_type_false;
    }
  else
    {
    ret.type = schemerlicht_object_type_false;
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_procedure(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_PROCEDURE);
  schemerlicht_object ret;
  if (b > 0)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_closure || v->type == schemerlicht_object_type_primitive || v->type == schemerlicht_object_type_lambda || v->type == schemerlicht_object_type_primitive_object)
      ret.type = schemerlicht_object_type_true;
    else
      ret.type = schemerlicht_object_type_false;
    }
  else
    {
    ret.type = schemerlicht_object_type_false;
    }
  schemerlicht_set_object(ra, &ret);
  }


////////////////////////////////////////////////////

void schemerlicht_primitive_make_vector(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_MAKE_VECTOR);
  schemerlicht_memsize vector_length = 0;
  if (b > 0)
    {
    schemerlicht_object* rb = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    switch (rb->type)
      {
      case schemerlicht_object_type_fixnum:
        if (rb->value.fx > 0)
          vector_length = cast(schemerlicht_memsize, rb->value.fx);
        break;
      case schemerlicht_object_type_flonum:
        if (rb->value.fl > 0)
          vector_length = cast(schemerlicht_memsize, rb->value.fl);
        break;
      }
    }
  schemerlicht_object v = make_schemerlicht_object_vector(ctxt, vector_length);
  schemerlicht_object dummy;
  dummy.type = schemerlicht_object_type_undefined;
  schemerlicht_object* fill_obj = &dummy;
  if (b > 1)
    {
    fill_obj = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    }
  for (schemerlicht_memsize j = 0; j < vector_length; ++j)
    {
    schemerlicht_object* obj_at_pos = schemerlicht_vector_at(&v.value.v, j, schemerlicht_object);
    schemerlicht_set_object(obj_at_pos, fill_obj);
    }
  schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
  schemerlicht_set_object(heap_obj, &v);
  ++ctxt->heap_pos;
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_make_string(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_MAKE_STRING);
  schemerlicht_object v;
  v.type = schemerlicht_object_type_string;
  schemerlicht_memsize string_length = 0;
  if (b > 0)
    {
    schemerlicht_object* rb = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    switch (rb->type)
      {
      case schemerlicht_object_type_fixnum:
        if (rb->value.fx > 0)
          string_length = cast(schemerlicht_memsize, rb->value.fx);
        break;
      case schemerlicht_object_type_flonum:
        if (rb->value.fl > 0)
          string_length = cast(schemerlicht_memsize, rb->value.fl);
        break;
      }
    }
  char ch = '#';
  if (b > 1)
    {
    schemerlicht_object* character = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    if (character->type == schemerlicht_object_type_char)
      ch = character->value.ch;
    }
  schemerlicht_string_init_with_size(ctxt, &v.value.s, string_length, ch);
  schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
  schemerlicht_set_object(heap_obj, &v);
  ++ctxt->heap_pos;
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_vector_length(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_VECTOR_LENGTH);
  schemerlicht_object v;
  v.type = schemerlicht_object_type_fixnum;
  v.value.fx = 0;
  if (b > 0)
    {
    schemerlicht_object* rb = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (rb->type == schemerlicht_object_type_vector)
      v.value.fx = rb->value.v.vector_size;
    }
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_length(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_STRING_LENGTH);
  schemerlicht_object v;
  v.type = schemerlicht_object_type_fixnum;
  v.value.fx = 0;
  if (b > 0)
    {
    schemerlicht_object* rb = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (rb->type == schemerlicht_object_type_string)
      v.value.fx = rb->value.s.string_length;
    }
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_ref(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_STRING_REF);
  schemerlicht_object ret;
  if (b < 2)
    {
    ret.type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* pos = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    if (v->type != schemerlicht_object_type_string || pos->type != schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_undefined;
      }
    else
      {
      if (pos->value.fx < 0 || pos->value.fx >= v->value.s.string_length) // out of bounds
        {
        ret.type = schemerlicht_object_type_undefined;
        }
      else
        {
        char* ch = schemerlicht_string_at(&v->value.s, cast(schemerlicht_memsize, pos->value.fx));
        ret.type = schemerlicht_object_type_char;
        ret.value.ch = *ch;
        }
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_set(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_STRING_SET);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_void;
  if (b > 2)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* pos = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    schemerlicht_object* value = schemerlicht_vector_at(&ctxt->stack, a + 3 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_string && pos->type == schemerlicht_object_type_fixnum && value->type == schemerlicht_object_type_char)
      {
      if (pos->value.fx >= 0 && pos->value.fx < v->value.s.string_length)
        {
        char* value_to_change = schemerlicht_string_at(&v->value.s, cast(schemerlicht_memsize, pos->value.fx));
        *value_to_change = value->value.ch;
        }
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_hash(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_STRING_HASH);
  schemerlicht_object ret;
  if (b == 0)
    {
    ret.type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* rb = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (rb->type == schemerlicht_object_type_string)
      {
      ret.type = schemerlicht_object_type_fixnum;
      ret.value.fx = 5381;
      char* str = rb->value.s.string_ptr;
      int ch;
      while ((ch = *str++))
        ret.value.fx = ((ret.value.fx << 5) + ret.value.fx) + ch; /* ret.value.fx * 33 + ch */
      }
    else
      {
      ret.type = schemerlicht_object_type_undefined;
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_string(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_STRING);
  schemerlicht_object ret;
  if (b > 0)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_string)
      ret.type = schemerlicht_object_type_true;
    else
      ret.type = schemerlicht_object_type_false;
    }
  else
    {
    ret.type = schemerlicht_object_type_false;
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_eq(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_EQ);
  int result = 0;
  if (b >= 2)
    {
    schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    result = schemerlicht_objects_eq(arg0, arg1);
    }
  schemerlicht_object ret;
  ret.type = result ? schemerlicht_object_type_true : schemerlicht_object_type_false;
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_eqv(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_EQV);
  int result = 0;
  if (b >= 2)
    {
    schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    result = schemerlicht_objects_eqv(arg0, arg1);
    }
  schemerlicht_object ret;
  ret.type = result ? schemerlicht_object_type_true : schemerlicht_object_type_false;
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_equal(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_EQUAL);
  int result = 0;
  if (b >= 2)
    {
    schemerlicht_object* arg0 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    result = schemerlicht_objects_equal(ctxt, arg0, arg1);
    }
  schemerlicht_object ret;
  ret.type = result ? schemerlicht_object_type_true : schemerlicht_object_type_false;
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_STRING);
  schemerlicht_object v;
  v.type = schemerlicht_object_type_string;
  schemerlicht_string_init_with_size(ctxt, &v.value.s, b, '#');
  for (int j = 0; j < b; ++j)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_char)
      {
      char* ch = schemerlicht_string_at(&v.value.s, j);
      *ch = arg->value.ch;
      }
    }
  schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
  schemerlicht_set_object(heap_obj, &v);
  ++ctxt->heap_pos;
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_to_symbol(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_STRING_TO_SYMBOL);
  if (b == 0)
    {
    schemerlicht_object v;
    v.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &v);
    }
  else
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_string)
      {
      schemerlicht_object* symbol = schemerlicht_map_get(ctxt, ctxt->string_to_symbol, arg);
      if (symbol != NULL)
        {
        schemerlicht_set_object(ra, symbol);
        }
      else
        {
        schemerlicht_object key;
        key.type = schemerlicht_object_type_string;
        schemerlicht_string_copy(ctxt, &key.value.s, &arg->value.s);
        schemerlicht_object* new_symbol = schemerlicht_map_insert(ctxt, ctxt->string_to_symbol, &key);
        new_symbol->type = schemerlicht_object_type_symbol;
        schemerlicht_string_copy(ctxt, &new_symbol->value.s, &arg->value.s);
        schemerlicht_set_object(ra, new_symbol);
        }
      }
    else
      {
      schemerlicht_object v;
      v.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &v);
      }
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_symbol_to_string(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_SYMBOL_TO_STRING);
  if (b == 0)
    {
    schemerlicht_object v;
    v.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &v);
    }
  else
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_symbol)
      {
      schemerlicht_object v;
      v.type = schemerlicht_object_type_string;
      schemerlicht_string_copy(ctxt, &v.value.s, &arg->value.s);
      schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
      schemerlicht_set_object(heap_obj, &v);
      ++ctxt->heap_pos;

      schemerlicht_set_object(ra, heap_obj);
      }
    else
      {
      schemerlicht_object v;
      v.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &v);
      }
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_length(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_LENGTH);
  schemerlicht_object ret;
  if (b == 0)
    {
    ret.type = schemerlicht_object_type_undefined;
    }
  else
    {
    ret.type = schemerlicht_object_type_fixnum;
    schemerlicht_memsize length = 0;
    schemerlicht_object* p = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (p->type != schemerlicht_object_type_nil && p->type != schemerlicht_object_type_pair)
      {
      ret.type = schemerlicht_object_type_undefined;
      }
    else
      {
      while (p->type != schemerlicht_object_type_nil)
        {
        ++length;
        p = schemerlicht_vector_at(&p->value.v, 1, schemerlicht_object);
        if (p->type != schemerlicht_object_type_nil && p->type != schemerlicht_object_type_pair)
          {
          ret.type = schemerlicht_object_type_undefined;
          break;
          }
        }
      if (ret.type == schemerlicht_object_type_fixnum)
        ret.value.fx = length;
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_set_car(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_SET_CAR);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_void;
  if (b > 1)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* value = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_pair)
      {
      schemerlicht_assert(v->value.v.vector_size == 2);
      schemerlicht_object* value_to_change = schemerlicht_vector_at(&v->value.v, 0, schemerlicht_object);
      schemerlicht_set_object(value_to_change, value);
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_set_cdr(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_SET_CDR);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_void;
  if (b > 1)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* value = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_pair)
      {
      schemerlicht_assert(v->value.v.vector_size == 2);
      schemerlicht_object* value_to_change = schemerlicht_vector_at(&v->value.v, 1, schemerlicht_object);
      schemerlicht_set_object(value_to_change, value);
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_symbol(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_SYMBOL);
  schemerlicht_object ret;
  if (b > 0)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_symbol)
      ret.type = schemerlicht_object_type_true;
    else
      ret.type = schemerlicht_object_type_false;
    }
  else
    {
    ret.type = schemerlicht_object_type_false;
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_reclaim(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_RECLAIM);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_void;
  UNUSED(b);
  UNUSED(c);
  schemerlicht_check_garbage_collection(ctxt);
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_memv(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_MEMV);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_false;
  if (b >= 2)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* lst = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    if (schemerlicht_objects_eqv(v, lst))
      {
      ret = *lst;
      }
    else
      {
      while (lst->type == schemerlicht_object_type_pair)
        {
        schemerlicht_object* v0 = schemerlicht_vector_at(&lst->value.v, 0, schemerlicht_object);
        schemerlicht_object* v1 = schemerlicht_vector_at(&lst->value.v, 1, schemerlicht_object);
        if (schemerlicht_objects_eqv(v, v0))
          {
          ret = *lst;
          break;
          }
        lst = v1;
        }
      if (lst->type != schemerlicht_object_type_pair)
        {
        if (schemerlicht_objects_eqv(v, lst))
          ret = *lst;
        }
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_memq(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_MEMQ);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_false;
  if (b >= 2)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* lst = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    if (schemerlicht_objects_eq(v, lst))
      {
      ret = *lst;
      }
    else
      {
      while (lst->type == schemerlicht_object_type_pair)
        {
        schemerlicht_object* v0 = schemerlicht_vector_at(&lst->value.v, 0, schemerlicht_object);
        schemerlicht_object* v1 = schemerlicht_vector_at(&lst->value.v, 1, schemerlicht_object);
        if (schemerlicht_objects_eq(v, v0))
          {
          ret = *lst;
          break;
          }
        lst = v1;
        }
      if (lst->type != schemerlicht_object_type_pair)
        {
        if (schemerlicht_objects_eq(v, lst))
          ret = *lst;
        }
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_member(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_MEMBER);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_false;
  if (b >= 2)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* lst = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    if (schemerlicht_objects_equal(ctxt, v, lst))
      {
      ret = *lst;
      }
    else
      {
      while (lst->type == schemerlicht_object_type_pair)
        {
        schemerlicht_object* v0 = schemerlicht_vector_at(&lst->value.v, 0, schemerlicht_object);
        schemerlicht_object* v1 = schemerlicht_vector_at(&lst->value.v, 1, schemerlicht_object);
        if (schemerlicht_objects_equal(ctxt, v, v0))
          {
          ret = *lst;
          break;
          }
        lst = v1;
        }
      if (lst->type != schemerlicht_object_type_pair)
        {
        if (schemerlicht_objects_equal(ctxt, v, lst))
          ret = *lst;
        }
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_assv(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_ASSV);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_false;
  if (b >= 2)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* lst = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    while (lst->type == schemerlicht_object_type_pair)
      {
      schemerlicht_object* v0 = schemerlicht_vector_at(&lst->value.v, 0, schemerlicht_object);
      schemerlicht_object* v1 = schemerlicht_vector_at(&lst->value.v, 1, schemerlicht_object);
      if (v0->type == schemerlicht_object_type_pair)
        {
        schemerlicht_object* v00 = schemerlicht_vector_at(&v0->value.v, 0, schemerlicht_object);
        if (schemerlicht_objects_eqv(v, v00))
          {
          ret = *v0;
          break;
          }
        }
      lst = v1;
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_assq(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_ASSQ);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_false;
  if (b >= 2)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* lst = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    while (lst->type == schemerlicht_object_type_pair)
      {
      schemerlicht_object* v0 = schemerlicht_vector_at(&lst->value.v, 0, schemerlicht_object);
      schemerlicht_object* v1 = schemerlicht_vector_at(&lst->value.v, 1, schemerlicht_object);
      if (v0->type == schemerlicht_object_type_pair)
        {
        schemerlicht_object* v00 = schemerlicht_vector_at(&v0->value.v, 0, schemerlicht_object);
        if (schemerlicht_objects_eq(v, v00))
          {
          ret = *v0;
          break;
          }
        }
      lst = v1;
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_assoc(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_ASSOC);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_false;
  if (b >= 2)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* lst = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    while (lst->type == schemerlicht_object_type_pair)
      {
      schemerlicht_object* v0 = schemerlicht_vector_at(&lst->value.v, 0, schemerlicht_object);
      schemerlicht_object* v1 = schemerlicht_vector_at(&lst->value.v, 1, schemerlicht_object);
      if (v0->type == schemerlicht_object_type_pair)
        {
        schemerlicht_object* v00 = schemerlicht_vector_at(&v0->value.v, 0, schemerlicht_object);
        if (schemerlicht_objects_equal(ctxt, v, v00))
          {
          ret = *v0;
          break;
          }
        }
      lst = v1;
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_append(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_STRING_APPEND);
  schemerlicht_object v;
  v.type = schemerlicht_object_type_string;
  schemerlicht_string_init(ctxt, &v.value.s, "");
  for (int i = 0; i < b; ++i)
    {
    schemerlicht_object* rb = schemerlicht_vector_at(&ctxt->stack, a + c + i + 1, schemerlicht_object);
    if (rb->type == schemerlicht_object_type_string)
      {
      schemerlicht_string_append(ctxt, &v.value.s, &rb->value.s);
      }
    }
  schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
  schemerlicht_set_object(heap_obj, &v);
  ++ctxt->heap_pos;
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_copy(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_STRING_COPY);

  schemerlicht_object v;
  if (b > 0)
    {
    schemerlicht_object* rb = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    if (rb->type == schemerlicht_object_type_string)
      {
      v.type = schemerlicht_object_type_string;
      schemerlicht_string_copy(ctxt, &v.value.s, &rb->value.s);
      schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
      schemerlicht_set_object(heap_obj, &v);
      ++ctxt->heap_pos;
      }
    else
      {
      v.type = schemerlicht_object_type_undefined;
      }
    }
  else
    {
    v.type = schemerlicht_object_type_undefined;
    }
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_fill(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_STRING_FILL);
  schemerlicht_object v;
  if (b < 2)
    {
    v.type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* str = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* chr = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    if (str->type == schemerlicht_object_type_string && chr->type == schemerlicht_object_type_char)
      {
      memset(str->value.s.string_ptr, chr->value.ch, str->value.s.string_length);
      v = *str;
      }
    else
      {
      v.type = schemerlicht_object_type_undefined;
      }
    }
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_vector_fill(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_VECTOR_FILL);
  schemerlicht_object v;
  if (b < 2)
    {
    v.type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* vec = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* obj = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    if (vec->type == schemerlicht_object_type_vector)
      {
      schemerlicht_object* it = schemerlicht_vector_begin(&vec->value.v, schemerlicht_object);
      schemerlicht_object* it_end = schemerlicht_vector_end(&vec->value.v, schemerlicht_object);
      for (; it != it_end; ++it)
        {
        *it = *obj;
        }
      v.type = schemerlicht_object_type_void;
      }
    else
      {
      v.type = schemerlicht_object_type_undefined;
      }
    }
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_current_seconds(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CURRENT_SECONDS);
  schemerlicht_object v;
  v.type = schemerlicht_object_type_flonum;
  v.value.fl = cast(schemerlicht_flonum, clock()) / cast(schemerlicht_flonum, CLOCKS_PER_SEC);
  schemerlicht_set_object(ra, &v);
  UNUSED(c);
  UNUSED(b);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_current_milliseconds(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CURRENT_MILLISECONDS);
  schemerlicht_object v;
  v.type = schemerlicht_object_type_flonum;
  v.value.fl = cast(schemerlicht_flonum, clock()) * 1000.0 / cast(schemerlicht_flonum, CLOCKS_PER_SEC);
  schemerlicht_set_object(ra, &v);
  UNUSED(c);
  UNUSED(b);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_list(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_LIST);
  schemerlicht_object v;
  v.type = schemerlicht_object_type_false;
  if (b > 0)
    {
    schemerlicht_object* rb = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    while (rb->type == schemerlicht_object_type_pair)
      {
      rb = schemerlicht_vector_at(&rb->value.v, 1, schemerlicht_object);
      }
    if (rb->type == schemerlicht_object_type_nil)
      v.type = schemerlicht_object_type_true;
    }
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_equal(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_STRING_EQUAL);
  schemerlicht_object v;
  v.type = schemerlicht_object_type_undefined;
  if (b > 1)
    {
    schemerlicht_object* s1 = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* s2 = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    if (s1->type == schemerlicht_object_type_string && s2->type == schemerlicht_object_type_string)
      {
      if (strcmp(s1->value.s.string_ptr, s2->value.s.string_ptr) == 0)
        v.type = schemerlicht_object_type_true;
      else
        v.type = schemerlicht_object_type_false;
      }
    }
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_less(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_object v;
  v.type = schemerlicht_object_type_undefined;
  if (b > 1)
    {
    schemerlicht_object* s1 = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* s2 = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    if (s1->type == schemerlicht_object_type_string && s2->type == schemerlicht_object_type_string)
      {
      schemerlicht_memsize sz = s1->value.s.string_length;
      if (s2->value.s.string_length < sz)
        sz = s2->value.s.string_length;
      for (schemerlicht_memsize i = 0; i < sz; ++i)
        {
        if (s1->value.s.string_ptr[i] < s2->value.s.string_ptr[i])
          {
          v.type = schemerlicht_object_type_true;
          schemerlicht_set_object(ra, &v);
          return;
          }
        else if (s1->value.s.string_ptr[i] > s2->value.s.string_ptr[i])
          {
          v.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &v);
          return;
          }
        }
      if (s1->value.s.string_length < s2->value.s.string_length)
        v.type = schemerlicht_object_type_true;
      else
        v.type = schemerlicht_object_type_false;
      }
    }
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_greater(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_object v;
  v.type = schemerlicht_object_type_undefined;
  if (b > 1)
    {
    schemerlicht_object* s1 = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* s2 = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    if (s1->type == schemerlicht_object_type_string && s2->type == schemerlicht_object_type_string)
      {
      schemerlicht_memsize sz = s1->value.s.string_length;
      if (s2->value.s.string_length < sz)
        sz = s2->value.s.string_length;
      for (schemerlicht_memsize i = 0; i < sz; ++i)
        {
        if (s1->value.s.string_ptr[i] > s2->value.s.string_ptr[i])
          {
          v.type = schemerlicht_object_type_true;
          schemerlicht_set_object(ra, &v);
          return;
          }
        else if (s1->value.s.string_ptr[i] < s2->value.s.string_ptr[i])
          {
          v.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &v);
          return;
          }
        }
      if (s1->value.s.string_length > s2->value.s.string_length)
        v.type = schemerlicht_object_type_true;
      else
        v.type = schemerlicht_object_type_false;
      }
    }
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_leq(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_STRING_LEQ);
  schemerlicht_primitive_string_greater(ctxt, a, b, c);
  if (ra->type == schemerlicht_object_type_true)
    {
    ra->type = schemerlicht_object_type_false;
    }
  else if (ra->type == schemerlicht_object_type_false)
    {
    ra->type = schemerlicht_object_type_true;
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_geq(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_STRING_GEQ);
  schemerlicht_primitive_string_less(ctxt, a, b, c);
  if (ra->type == schemerlicht_object_type_true)
    {
    ra->type = schemerlicht_object_type_false;
    }
  else if (ra->type == schemerlicht_object_type_false)
    {
    ra->type = schemerlicht_object_type_true;
    }
  }

////////////////////////////////////////////////////

static char tolower(char ch)
  {
  if (ch >= 'A' && ch <= 'Z')
    ch += ('a' - 'A');
  return ch;
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_ci_equal(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_STRING_CI_EQUAL);
  schemerlicht_object v;
  v.type = schemerlicht_object_type_undefined;
  if (b > 1)
    {
    schemerlicht_object* s1 = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* s2 = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    if (s1->type == schemerlicht_object_type_string && s2->type == schemerlicht_object_type_string)
      {
      if (s1->value.s.string_length != s2->value.s.string_length)
        v.type = schemerlicht_object_type_false;
      else
        {
        const schemerlicht_memsize sz = s1->value.s.string_length;
        for (schemerlicht_memsize i = 0; i < sz; ++i)
          {
          if (tolower(s1->value.s.string_ptr[i]) != tolower(s2->value.s.string_ptr[i]))
            {
            v.type = schemerlicht_object_type_false;
            schemerlicht_set_object(ra, &v);
            return;
            }
          }
        v.type = schemerlicht_object_type_true;
        }
      }
    }
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_ci_less(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_object v;
  v.type = schemerlicht_object_type_undefined;
  if (b > 1)
    {
    schemerlicht_object* s1 = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* s2 = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    if (s1->type == schemerlicht_object_type_string && s2->type == schemerlicht_object_type_string)
      {
      schemerlicht_memsize sz = s1->value.s.string_length;
      if (s2->value.s.string_length < sz)
        sz = s2->value.s.string_length;
      for (schemerlicht_memsize i = 0; i < sz; ++i)
        {
        if (tolower(s1->value.s.string_ptr[i]) < tolower(s2->value.s.string_ptr[i]))
          {
          v.type = schemerlicht_object_type_true;
          schemerlicht_set_object(ra, &v);
          return;
          }
        else if (tolower(s1->value.s.string_ptr[i]) > tolower(s2->value.s.string_ptr[i]))
          {
          v.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &v);
          return;
          }
        }
      if (s1->value.s.string_length < s2->value.s.string_length)
        v.type = schemerlicht_object_type_true;
      else
        v.type = schemerlicht_object_type_false;
      }
    }
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_ci_greater(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_object v;
  v.type = schemerlicht_object_type_undefined;
  if (b > 1)
    {
    schemerlicht_object* s1 = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* s2 = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    if (s1->type == schemerlicht_object_type_string && s2->type == schemerlicht_object_type_string)
      {
      schemerlicht_memsize sz = s1->value.s.string_length;
      if (s2->value.s.string_length < sz)
        sz = s2->value.s.string_length;
      for (schemerlicht_memsize i = 0; i < sz; ++i)
        {
        if (tolower(s1->value.s.string_ptr[i]) > tolower(s2->value.s.string_ptr[i]))
          {
          v.type = schemerlicht_object_type_true;
          schemerlicht_set_object(ra, &v);
          return;
          }
        else if (tolower(s1->value.s.string_ptr[i]) < tolower(s2->value.s.string_ptr[i]))
          {
          v.type = schemerlicht_object_type_false;
          schemerlicht_set_object(ra, &v);
          return;
          }
        }
      if (s1->value.s.string_length > s2->value.s.string_length)
        v.type = schemerlicht_object_type_true;
      else
        v.type = schemerlicht_object_type_false;
      }
    }
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_ci_leq(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_STRING_CI_LEQ);
  schemerlicht_primitive_string_ci_greater(ctxt, a, b, c);
  if (ra->type == schemerlicht_object_type_true)
    {
    ra->type = schemerlicht_object_type_false;
    }
  else if (ra->type == schemerlicht_object_type_false)
    {
    ra->type = schemerlicht_object_type_true;
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_ci_geq(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_STRING_CI_GEQ);
  schemerlicht_primitive_string_ci_less(ctxt, a, b, c);
  if (ra->type == schemerlicht_object_type_true)
    {
    ra->type = schemerlicht_object_type_false;
    }
  else if (ra->type == schemerlicht_object_type_false)
    {
    ra->type = schemerlicht_object_type_true;
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_substring(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_SUBSTRING);
  schemerlicht_object v;
  v.type = schemerlicht_object_type_undefined;
  if (b > 2)
    {
    schemerlicht_object* str = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* from = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    schemerlicht_object* to = schemerlicht_vector_at(&ctxt->stack, a + c + 3, schemerlicht_object);
    if (str->type == schemerlicht_object_type_string && from->type == schemerlicht_object_type_fixnum && to->type == schemerlicht_object_type_fixnum)
      {
      if (from->value.fx >= 0 && from->value.fx <= to->value.fx)
        {
        const schemerlicht_memsize f = cast(schemerlicht_memsize, from->value.fx);
        schemerlicht_memsize t = cast(schemerlicht_memsize, to->value.fx);
        if (t > str->value.s.string_length)
          t = str->value.s.string_length;
        v.type = schemerlicht_object_type_string;
        schemerlicht_string_init_ranged(ctxt, &v.value.s, str->value.s.string_ptr + f, str->value.s.string_ptr + t);
        schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
        schemerlicht_set_object(heap_obj, &v);
        ++ctxt->heap_pos;
        }
      }
    }
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_max(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_MAX);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_fixnum &&
      first_arg->type != schemerlicht_object_type_flonum &&
      first_arg->type != schemerlicht_object_type_char
      )
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    schemerlicht_object ret = *first_arg;
    for (int j = 1; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);

      switch (arg->type)
        {
        case schemerlicht_object_type_flonum:
        {
        switch (ret.type)
          {
          case schemerlicht_object_type_flonum:
            if (arg->value.fl > ret.value.fl)
              ret = *arg;
            break;
          case schemerlicht_object_type_fixnum:
            if (arg->value.fl > ret.value.fx)
              ret = *arg;
            break;
          case schemerlicht_object_type_char:
            if (arg->value.fl > ret.value.ch)
              ret = *arg;
            break;
          }
        break;
        }
        case schemerlicht_object_type_fixnum:
        {
        switch (ret.type)
          {
          case schemerlicht_object_type_flonum:
            if (arg->value.fx > ret.value.fl)
              ret = *arg;
            break;
          case schemerlicht_object_type_fixnum:
            if (arg->value.fx > ret.value.fx)
              ret = *arg;
            break;
          case schemerlicht_object_type_char:
            if (arg->value.fx > ret.value.ch)
              ret = *arg;
            break;
          }
        break;
        }
        case schemerlicht_object_type_char:
        {
        switch (ret.type)
          {
          case schemerlicht_object_type_flonum:
            if (arg->value.ch > ret.value.fl)
              ret = *arg;
            break;
          case schemerlicht_object_type_fixnum:
            if (arg->value.ch > ret.value.fx)
              ret = *arg;
            break;
          case schemerlicht_object_type_char:
            if (arg->value.ch > ret.value.ch)
              ret = *arg;
            break;
          }
        break;
        }
        default:
        {
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_set_object(ra, &ret);
        return;
        }
        }
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_min(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_MIN);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_fixnum &&
      first_arg->type != schemerlicht_object_type_flonum &&
      first_arg->type != schemerlicht_object_type_char
      )
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    schemerlicht_object ret = *first_arg;
    for (int j = 1; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j + c, schemerlicht_object);

      switch (arg->type)
        {
        case schemerlicht_object_type_flonum:
        {
        switch (ret.type)
          {
          case schemerlicht_object_type_flonum:
            if (arg->value.fl < ret.value.fl)
              ret = *arg;
            break;
          case schemerlicht_object_type_fixnum:
            if (arg->value.fl < ret.value.fx)
              ret = *arg;
            break;
          case schemerlicht_object_type_char:
            if (arg->value.fl < ret.value.ch)
              ret = *arg;
            break;
          }
        break;
        }
        case schemerlicht_object_type_fixnum:
        {
        switch (ret.type)
          {
          case schemerlicht_object_type_flonum:
            if (arg->value.fx < ret.value.fl)
              ret = *arg;
            break;
          case schemerlicht_object_type_fixnum:
            if (arg->value.fx < ret.value.fx)
              ret = *arg;
            break;
          case schemerlicht_object_type_char:
            if (arg->value.fx < ret.value.ch)
              ret = *arg;
            break;
          }
        break;
        }
        case schemerlicht_object_type_char:
        {
        switch (ret.type)
          {
          case schemerlicht_object_type_flonum:
            if (arg->value.ch < ret.value.fl)
              ret = *arg;
            break;
          case schemerlicht_object_type_fixnum:
            if (arg->value.ch < ret.value.fx)
              ret = *arg;
            break;
          case schemerlicht_object_type_char:
            if (arg->value.ch < ret.value.ch)
              ret = *arg;
            break;
          }
        break;
        }
        default:
        {
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_set_object(ra, &ret);
        return;
        }
        }
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_apply(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_APPLY);

  if (b > 1)
    {
    schemerlicht_object* last_arg = schemerlicht_vector_at(&ctxt->stack, a + c + b, schemerlicht_object);
    --b;
    while (last_arg->type == schemerlicht_object_type_pair)
      {
      schemerlicht_object* v0 = schemerlicht_vector_at(&last_arg->value.v, 0, schemerlicht_object);
      schemerlicht_object* v1 = schemerlicht_vector_at(&last_arg->value.v, 1, schemerlicht_object);
      ++b;
      schemerlicht_object* stack_pos = schemerlicht_vector_at(&ctxt->stack, a + c + b, schemerlicht_object);
      *stack_pos = *v0;
      last_arg = v1;
      }

    //stack situation so far:
    // a: prim call apply
    // a + c + 1: operator arg
    // a + c + 2: first arg for op
    // a + c + 3: second arg for op
    // ...
    // a + c + b: last arg

    schemerlicht_object* op = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    if (op->type == schemerlicht_object_type_primitive || op->type == schemerlicht_object_type_primitive_object)
      {
      schemerlicht_call_primitive(ctxt, op->value.fx, a + c + 1, b - 1, 0);
      schemerlicht_set_object(ra, op);
      }
    else if (op->type == schemerlicht_object_type_closure)
      {
      schemerlicht_assert(a == 0); // by construction apply is always represented as a funcall. Because of cps construction it means that all registers are available
      schemerlicht_assert(c == 1); // as apply is represented as object, it means that c == 1 because the vm wants to skip the continuation when calling the primitive apply.
      schemerlicht_object* continuation = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object); // save the original closure
      schemerlicht_object original_continuation = *continuation;
      schemerlicht_object oper = *op;
      // TODO later: move dummy continuation to the context so that we don't need to recreate it all the time for apply
      schemerlicht_object dummy_continuation = make_schemerlicht_object_closure(ctxt, 1);
      schemerlicht_object dummy_lambda;
      dummy_lambda.type = schemerlicht_object_type_lambda;
      schemerlicht_function* dummy_fun = schemerlicht_function_new(ctxt);
      dummy_lambda.value.ptr = dummy_fun;
      schemerlicht_set_object(schemerlicht_vector_begin(&dummy_continuation.value.v, schemerlicht_object), &dummy_lambda);

      //swap place of continuation and operator
      *op = dummy_continuation;
      *continuation = oper;

      // stack state (with guaranteed a == 0):
      // a + 1 : operator closure
      // a + 2 : dummy continuation
      // a + 3 : first arg for operator
      // a + 4 : second arg for operator
      // ...
      // a + 1 + b : last arg for operator

      // move down 
      for (int i = 0; i < b + 1; ++i)
        {
        schemerlicht_object* ri = schemerlicht_vector_at(&ctxt->stack, i, schemerlicht_object);
        schemerlicht_object* ri_next = schemerlicht_vector_at(&ctxt->stack, i + 1, schemerlicht_object);
        *ri = *ri_next;
        }
      schemerlicht_assert(oper.type == schemerlicht_object_type_closure);
      schemerlicht_object* lambda_obj = schemerlicht_vector_at(&oper.value.v, 0, schemerlicht_object);
      schemerlicht_assert(lambda_obj->type == schemerlicht_object_type_lambda);
      schemerlicht_function* lambda = cast(schemerlicht_function*, lambda_obj->value.ptr);
      schemerlicht_run(ctxt, lambda);

      schemerlicht_object ret = *schemerlicht_vector_at(&ctxt->stack, 1, schemerlicht_object); // return value is at position 1, as our fake continuation lambda is simply empty, which means: R0 == lambda itself, R1 == first lambda arg (which is return value)
      schemerlicht_set_object(ra, &ret);
      *continuation = original_continuation;
      schemerlicht_object_destroy(ctxt, &dummy_continuation);
      schemerlicht_function_free(ctxt, dummy_fun);
      }
    else if (op->type == schemerlicht_object_type_lambda)
      {
      schemerlicht_assert(a == 0); // by construction apply is always represented as a funcall. Because of cps construction it means that all registers are available
      schemerlicht_assert(c == 1); // as apply is represented as object, it means that c == 1 because the vm wants to skip the continuation when calling the primitive apply.
      schemerlicht_object* continuation = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object); // save the original closure
      schemerlicht_object original_continuation = *continuation;
      schemerlicht_object oper = *op;
      // TODO later: move dummy continuation to the context so that we don't need to recreate it all the time for apply
      schemerlicht_object dummy_continuation = make_schemerlicht_object_closure(ctxt, 1);
      schemerlicht_object dummy_lambda;
      dummy_lambda.type = schemerlicht_object_type_lambda;
      schemerlicht_function* dummy_fun = schemerlicht_function_new(ctxt);
      dummy_lambda.value.ptr = dummy_fun;
      schemerlicht_set_object(schemerlicht_vector_begin(&dummy_continuation.value.v, schemerlicht_object), &dummy_lambda);

      //swap place of continuation and operator
      *op = dummy_continuation;
      *continuation = oper;

      // stack state (with guaranteed a == 0):
      // a + 1 : operator closure
      // a + 2 : dummy continuation
      // a + 3 : first arg for operator
      // a + 4 : second arg for operator
      // ...
      // a + 1 + b : last arg for operator

      // move down 
      for (int i = 0; i < b + 1; ++i)
        {
        schemerlicht_object* ri = schemerlicht_vector_at(&ctxt->stack, i, schemerlicht_object);
        schemerlicht_object* ri_next = schemerlicht_vector_at(&ctxt->stack, i + 1, schemerlicht_object);
        *ri = *ri_next;
        }
      schemerlicht_assert(oper.type == schemerlicht_object_type_lambda);
      schemerlicht_function* lambda = cast(schemerlicht_function*, oper.value.ptr);
      schemerlicht_run(ctxt, lambda);

      schemerlicht_object ret = *schemerlicht_vector_at(&ctxt->stack, 1, schemerlicht_object); // return value is at position 1, as our fake continuation lambda is simply empty, which means: R0 == lambda itself, R1 == first lambda arg (which is return value)
      schemerlicht_set_object(ra, &ret);
      *continuation = original_continuation;
      schemerlicht_object_destroy(ctxt, &dummy_continuation);
      schemerlicht_function_free(ctxt, dummy_fun);
      }
    else
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      }
    }
  else
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  }
////////////////////////////////////////////////////

void schemerlicht_primitive_append(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_APPEND);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
    ++ctxt->heap_pos;
    heap_obj->type = schemerlicht_object_type_nil;
    schemerlicht_object* last_list_obj = heap_obj;
    for (int i = 0; i < b; ++i)
      {
      schemerlicht_object* current_list = schemerlicht_vector_at(&ctxt->stack, a + c + 1 + i, schemerlicht_object);
      if (current_list->type != schemerlicht_object_type_pair && current_list->type != schemerlicht_object_type_nil)
        {
        schemerlicht_runerror(ctxt, "non list object as argument to append");
        break;
        }
      while (current_list->type == schemerlicht_object_type_pair)
        {
        schemerlicht_object* v0 = schemerlicht_vector_at(&current_list->value.v, 0, schemerlicht_object);
        schemerlicht_object* v1 = schemerlicht_vector_at(&current_list->value.v, 1, schemerlicht_object);
        *last_list_obj = make_schemerlicht_object_pair(ctxt);
        *schemerlicht_vector_at(&last_list_obj->value.v, 0, schemerlicht_object) = *v0;
        last_list_obj = schemerlicht_vector_at(&last_list_obj->value.v, 1, schemerlicht_object);
        last_list_obj->type = schemerlicht_object_type_nil;
        current_list = v1;
        }
      if (current_list->type != schemerlicht_object_type_nil)
        {
        schemerlicht_runerror(ctxt, "non list object as argument to append");
        break;
        }
      }
    schemerlicht_set_object(ra, heap_obj);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_exact(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_EXACT);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_false;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_fixnum)
      {
      obj.type = schemerlicht_object_type_true;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_inexact(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_INEXACT);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_false;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_flonum)
      {
      obj.type = schemerlicht_object_type_true;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_number(schemerlicht_context* ctxt, int a, int b, int c)
  {  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_NUMBER);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_false;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_fixnum || arg->type == schemerlicht_object_type_flonum)
      {
      obj.type = schemerlicht_object_type_true;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_rational(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_RATIONAL);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_false;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_fixnum || arg->type == schemerlicht_object_type_flonum)
      {
      obj.type = schemerlicht_object_type_true;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_positive(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_POSITIVE);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_false;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        if (arg->value.fx >= 0)
          obj.type = schemerlicht_object_type_true;
        break;
      case schemerlicht_object_type_flonum:
        if (arg->value.fl >= 0)
          obj.type = schemerlicht_object_type_true;
        break;
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_negative(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_NEGATIVE);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_false;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        if (arg->value.fx < 0)
          obj.type = schemerlicht_object_type_true;
        break;
      case schemerlicht_object_type_flonum:
        if (arg->value.fl < 0)
          obj.type = schemerlicht_object_type_true;
        break;
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_odd(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_ODD);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_false;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        if ((arg->value.fx % 2) != 0)
          obj.type = schemerlicht_object_type_true;
        break;
      case schemerlicht_object_type_flonum:
      {
      schemerlicht_fixnum fx = cast(schemerlicht_fixnum, arg->value.fl);
      if ((arg->value.fl == cast(schemerlicht_flonum, fx)) && ((arg->value.fx % 2) != 0))
        obj.type = schemerlicht_object_type_true;
      break;
      }
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_even(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_EVEN);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_false;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        if ((arg->value.fx % 2) == 0)
          obj.type = schemerlicht_object_type_true;
        break;
      case schemerlicht_object_type_flonum:
      {
      schemerlicht_fixnum fx = cast(schemerlicht_fixnum, arg->value.fl);
      if ((arg->value.fl == cast(schemerlicht_flonum, fx)) && ((arg->value.fx % 2) == 0))
        obj.type = schemerlicht_object_type_true;
      break;
      }
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_complex(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_COMPLEX);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_false;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_fixnum || arg->type == schemerlicht_object_type_flonum)
      {
      obj.type = schemerlicht_object_type_true;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_real(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_REAL);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_false;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_fixnum || arg->type == schemerlicht_object_type_flonum)
      {
      obj.type = schemerlicht_object_type_true;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_integer(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_INTEGER);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_false;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        obj.type = schemerlicht_object_type_true;
        break;
      case schemerlicht_object_type_flonum:
      {
      schemerlicht_fixnum fx = cast(schemerlicht_fixnum, arg->value.fl);
      if ((arg->value.fl == cast(schemerlicht_flonum, fx)))
        obj.type = schemerlicht_object_type_true;
      break;
      }
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

void schemerlicht_primitive_abs(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_ABS);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        obj.type = schemerlicht_object_type_fixnum;
        obj.value.fx = arg->value.fx < 0 ? -arg->value.fx : arg->value.fx;
        break;
      case schemerlicht_object_type_flonum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = arg->value.fl < 0 ? -arg->value.fl : arg->value.fl;
        break;
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_quotient(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_QUOTIENT);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 1)
    {
    schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* arg2 = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    if ((arg1->type == schemerlicht_object_type_fixnum || arg1->type == schemerlicht_object_type_flonum) && (arg2->type == schemerlicht_object_type_fixnum || arg2->type == schemerlicht_object_type_flonum))
      {
      schemerlicht_fixnum fx1 = arg1->value.fx;
      schemerlicht_fixnum fx2 = arg2->value.fx;
      if (arg1->type == schemerlicht_object_type_flonum)
        fx1 = cast(schemerlicht_fixnum, arg1->value.fl);
      if (arg2->type == schemerlicht_object_type_flonum)
        fx2 = cast(schemerlicht_fixnum, arg2->value.fl);
      if (fx2 != 0)
        {
        obj.type = schemerlicht_object_type_fixnum;
        obj.value.fx = fx1 / fx2;
        }
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_remainder(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_REMAINDER);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 1)
    {
    schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* arg2 = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    if ((arg1->type == schemerlicht_object_type_fixnum || arg1->type == schemerlicht_object_type_flonum) && (arg2->type == schemerlicht_object_type_fixnum || arg2->type == schemerlicht_object_type_flonum))
      {
      schemerlicht_fixnum fx1 = arg1->value.fx;
      schemerlicht_fixnum fx2 = arg2->value.fx;
      if (arg1->type == schemerlicht_object_type_flonum)
        fx1 = cast(schemerlicht_fixnum, arg1->value.fl);
      if (arg2->type == schemerlicht_object_type_flonum)
        fx2 = cast(schemerlicht_fixnum, arg2->value.fl);
      if (fx2 != 0)
        {
        obj.type = schemerlicht_object_type_fixnum;
        obj.value.fx = fx1 % fx2;
        }
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_modulo(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_MODULO);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 1)
    {
    schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* arg2 = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    if ((arg1->type == schemerlicht_object_type_fixnum || arg1->type == schemerlicht_object_type_flonum) && (arg2->type == schemerlicht_object_type_fixnum || arg2->type == schemerlicht_object_type_flonum))
      {
      schemerlicht_fixnum fx1 = arg1->value.fx;
      schemerlicht_fixnum fx2 = arg2->value.fx;
      if (arg1->type == schemerlicht_object_type_flonum)
        fx1 = cast(schemerlicht_fixnum, arg1->value.fl);
      if (arg2->type == schemerlicht_object_type_flonum)
        fx2 = cast(schemerlicht_fixnum, arg2->value.fl);
      if (fx2 != 0)
        {
        obj.type = schemerlicht_object_type_fixnum;
        obj.value.fx = fx1 % fx2;
        if ((obj.value.fx < 0 && fx2 > 0) || (obj.value.fx > 0 && fx2 < 0))
          obj.value.fx += fx2;
        }
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

static schemerlicht_fixnum gcd(schemerlicht_fixnum a, schemerlicht_fixnum b)
  {
  a = a < 0 ? -a : a;
  b = b < 0 ? -b : b;
  /*
  for (;;)
    {
    if (a == 0)
      return b;
    if (b == 0)
      return a;
    if (a == b)
      return a;
    if (a > b)
      a -= b;
    else
      b -= a;
    }
    */
  while ((a%b)>0)
    {
    schemerlicht_fixnum R = a%b;
    a = b;
    b = R;
    }
  return b;
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_gcd(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_GCD);
  if (b == 0)
    {
    schemerlicht_object obj;
    obj.type = schemerlicht_object_type_fixnum;
    obj.value.fx = 0;
    schemerlicht_set_object(ra, &obj);
    }
  else if (b == 1)
    {
    schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    if (arg1->type == schemerlicht_object_type_fixnum || arg1->type == schemerlicht_object_type_flonum)
      {
      schemerlicht_set_object(ra, arg1);
      }
    else
      {
      schemerlicht_object obj;
      obj.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &obj);
      }
    }
  else
    {
    schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* arg2 = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    if ((arg1->type == schemerlicht_object_type_fixnum || arg1->type == schemerlicht_object_type_flonum) && (arg2->type == schemerlicht_object_type_fixnum || arg2->type == schemerlicht_object_type_flonum))
      {
      schemerlicht_fixnum fx1 = arg1->value.fx;
      schemerlicht_fixnum fx2 = arg2->value.fx;
      if (arg1->type == schemerlicht_object_type_flonum)
        fx1 = cast(schemerlicht_fixnum, arg1->value.fl);
      if (arg2->type == schemerlicht_object_type_flonum)
        fx2 = cast(schemerlicht_fixnum, arg2->value.fl);
      schemerlicht_object obj;
      obj.type = schemerlicht_object_type_fixnum;
      obj.value.fx = gcd(fx1, fx2);
      for (int i = 2; i < b; ++i)
        {
        schemerlicht_object* argi = schemerlicht_vector_at(&ctxt->stack, a + c + 1 + i, schemerlicht_object);
        if (argi->type == schemerlicht_object_type_fixnum || argi->type == schemerlicht_object_type_flonum)
          {
          schemerlicht_fixnum fxi = argi->value.fx;
          if (argi->type == schemerlicht_object_type_flonum)
            fxi = cast(schemerlicht_fixnum, argi->value.fl);
          obj.value.fx = gcd(obj.value.fx, fxi);
          }
        }
      schemerlicht_set_object(ra, &obj);
      }
    else
      {
      schemerlicht_object obj;
      obj.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &obj);
      }
    }
  }

////////////////////////////////////////////////////

static schemerlicht_fixnum lcm(schemerlicht_fixnum a, schemerlicht_fixnum b)
  {
  a = a < 0 ? -a : a;
  b = b < 0 ? -b : b;
  return a*b/gcd(a,b);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_lcm(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_LCM);
  if (b == 0)
    {
    schemerlicht_object obj;
    obj.type = schemerlicht_object_type_fixnum;
    obj.value.fx = 1;
    schemerlicht_set_object(ra, &obj);
    }
  else if (b == 1)
    {
    schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    if (arg1->type == schemerlicht_object_type_fixnum || arg1->type == schemerlicht_object_type_flonum)
      {
      schemerlicht_set_object(ra, arg1);
      }
    else
      {
      schemerlicht_object obj;
      obj.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &obj);
      }
    }
  else
    {
    schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* arg2 = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    if ((arg1->type == schemerlicht_object_type_fixnum || arg1->type == schemerlicht_object_type_flonum) && (arg2->type == schemerlicht_object_type_fixnum || arg2->type == schemerlicht_object_type_flonum))
      {
      schemerlicht_fixnum fx1 = arg1->value.fx;
      schemerlicht_fixnum fx2 = arg2->value.fx;
      if (arg1->type == schemerlicht_object_type_flonum)
        fx1 = cast(schemerlicht_fixnum, arg1->value.fl);
      if (arg2->type == schemerlicht_object_type_flonum)
        fx2 = cast(schemerlicht_fixnum, arg2->value.fl);
      schemerlicht_object obj;
      obj.type = schemerlicht_object_type_fixnum;
      obj.value.fx = lcm(fx1, fx2);
      for (int i = 2; i < b; ++i)
        {
        schemerlicht_object* argi = schemerlicht_vector_at(&ctxt->stack, a + c + 1 + i, schemerlicht_object);
        if (argi->type == schemerlicht_object_type_fixnum || argi->type == schemerlicht_object_type_flonum)
          {
          schemerlicht_fixnum fxi = argi->value.fx;
          if (argi->type == schemerlicht_object_type_flonum)
            fxi = cast(schemerlicht_fixnum, argi->value.fl);
          obj.value.fx = lcm(obj.value.fx, fxi);
          }
        }
      schemerlicht_set_object(ra, &obj);
      }
    else
      {
      schemerlicht_object obj;
      obj.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &obj);
      }
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_floor(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLOOR);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_ceiling(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CEILING);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_truncate(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_TRUNCATE);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_round(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_ROUND);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_exp(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_EXP);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_expt(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_EXPT);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_log(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_LOG);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_sin(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_SIN);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_cos(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_COS);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_tan(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_TAN);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_asin(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_ASIN);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_acos(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_ACOS);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_atan(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_ATAN);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_sqrt(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_SQRT);
  }

////////////////////////////////////////////////////

void schemerlicht_call_primitive(schemerlicht_context* ctxt, schemerlicht_fixnum prim_id, int a, int b, int c)
  {
#if 0
  void* fun_ptr = g_prim_arr[prim_id];
  ((void (*)(schemerlicht_context*, int, int, int))fun_ptr)(ctxt, a, b, c);
#else
  switch (prim_id)
    {
    case SCHEMERLICHT_ADD1:
      schemerlicht_primitive_add1(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_SUB1:
      schemerlicht_primitive_sub1(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_ADD:
      schemerlicht_primitive_add(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_SUB:
      schemerlicht_primitive_sub(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_MUL:
      schemerlicht_primitive_mul(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_DIV:
      schemerlicht_primitive_div(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_EQUAL:
      schemerlicht_primitive_equal(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_NOT_EQUAL:
      schemerlicht_primitive_not_equal(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_LESS:
      schemerlicht_primitive_less(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_LEQ:
      schemerlicht_primitive_leq(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_GREATER:
      schemerlicht_primitive_greater(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_GEQ:
      schemerlicht_primitive_geq(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_FIXNUM:
      schemerlicht_primitive_is_fixnum(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_FLONUM:
      schemerlicht_primitive_is_flonum(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_NULL:
      schemerlicht_primitive_is_null(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_ZERO:
      schemerlicht_primitive_is_zero(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_BOOLEAN:
      schemerlicht_primitive_is_boolean(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_CHAR:
      schemerlicht_primitive_is_char(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_NOT:
      schemerlicht_primitive_not(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FXADD1:
      schemerlicht_primitive_fxadd1(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FXSUB1:
      schemerlicht_primitive_fxsub1(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FXADD:
      schemerlicht_primitive_fxadd(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FXSUB:
      schemerlicht_primitive_fxsub(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FXMUL:
      schemerlicht_primitive_fxmul(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FXDIV:
      schemerlicht_primitive_fxdiv(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FXEQUAL:
      schemerlicht_primitive_fxequal(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FXLESS:
      schemerlicht_primitive_fxless(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FXLEQ:
      schemerlicht_primitive_fxleq(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FXGREATER:
      schemerlicht_primitive_fxgreater(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FXGEQ:
      schemerlicht_primitive_fxgeq(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FXZERO:
      schemerlicht_primitive_fxzero(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FLADD1:
      schemerlicht_primitive_fladd1(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FLSUB1:
      schemerlicht_primitive_flsub1(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FLADD:
      schemerlicht_primitive_fladd(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FLSUB:
      schemerlicht_primitive_flsub(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FLMUL:
      schemerlicht_primitive_flmul(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FLDIV:
      schemerlicht_primitive_fldiv(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FLEQUAL:
      schemerlicht_primitive_flequal(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FLLESS:
      schemerlicht_primitive_flless(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FLLEQ:
      schemerlicht_primitive_flleq(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FLGREATER:
      schemerlicht_primitive_flgreater(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FLGEQ:
      schemerlicht_primitive_flgeq(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FLZERO:
      schemerlicht_primitive_flzero(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CHAREQUAL:
      schemerlicht_primitive_charequal(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CHARLESS:
      schemerlicht_primitive_charless(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CHARLEQ:
      schemerlicht_primitive_charleq(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CHARGREATER:
      schemerlicht_primitive_chargreater(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CHARGEQ:
      schemerlicht_primitive_chargeq(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FIXNUM_CHAR:
      schemerlicht_primitive_fixnum_char(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CHAR_FIXNUM:
      schemerlicht_primitive_char_fixnum(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FIXNUM_FLONUM:
      schemerlicht_primitive_fixnum_flonum(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FLONUM_FIXNUM:
      schemerlicht_primitive_flonum_fixnum(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_BITWISE_AND:
      schemerlicht_primitive_bitwise_and(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_BITWISE_OR:
      schemerlicht_primitive_bitwise_or(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_BITWISE_NOT:
      schemerlicht_primitive_bitwise_not(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_BITWISE_XOR:
      schemerlicht_primitive_bitwise_xor(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_ARITHMETIC_SHIFT:
      schemerlicht_primitive_arithmetic_shift(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_VECTOR:
      schemerlicht_primitive_vector(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_VECTORREF:
      schemerlicht_primitive_vector_ref(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_VECTORSET:
      schemerlicht_primitive_vector_set(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_VECTOR:
      schemerlicht_primitive_is_vector(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_PAIR:
      schemerlicht_primitive_is_pair(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CONS:
      schemerlicht_primitive_cons(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CDR:
      schemerlicht_primitive_cdr(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CAR:
      schemerlicht_primitive_car(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_HALT:
      schemerlicht_primitive_halt(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CLOSURE:
      schemerlicht_primitive_closure(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CLOSUREREF:
      schemerlicht_primitive_closure_ref(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_CLOSURE:
      schemerlicht_primitive_is_closure(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_LIST:
      schemerlicht_primitive_list(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_MAKE_VECTOR:
      schemerlicht_primitive_make_vector(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_MAKE_STRING:
      schemerlicht_primitive_make_string(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_VECTOR_LENGTH:
      schemerlicht_primitive_vector_length(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_LENGTH:
      schemerlicht_primitive_string_length(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_REF:
      schemerlicht_primitive_string_ref(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_SET:
      schemerlicht_primitive_string_set(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_HASH:
      schemerlicht_primitive_string_hash(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_STRING:
      schemerlicht_primitive_is_string(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_EQ:
      schemerlicht_primitive_eq(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_EQV:
      schemerlicht_primitive_eqv(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_EQUAL:
      schemerlicht_primitive_is_equal(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING:
      schemerlicht_primitive_string(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_TO_SYMBOL:
      schemerlicht_primitive_string_to_symbol(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_LENGTH:
      schemerlicht_primitive_length(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_SET_CAR:
      schemerlicht_primitive_set_car(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_SET_CDR:
      schemerlicht_primitive_set_cdr(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_SYMBOL:
      schemerlicht_primitive_is_symbol(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_PROCEDURE:
      schemerlicht_primitive_is_procedure(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_RECLAIM:
      schemerlicht_primitive_reclaim(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_MEMV:
      schemerlicht_primitive_memv(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_MEMQ:
      schemerlicht_primitive_memq(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_MEMBER:
      schemerlicht_primitive_member(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_ASSV:
      schemerlicht_primitive_assv(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_ASSQ:
      schemerlicht_primitive_assq(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_ASSOC:
      schemerlicht_primitive_assoc(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_SYMBOL_TO_STRING:
      schemerlicht_primitive_symbol_to_string(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_COPY:
      schemerlicht_primitive_string_copy(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_APPEND:
      schemerlicht_primitive_string_append(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_FILL:
      schemerlicht_primitive_string_fill(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_VECTOR_FILL:
      schemerlicht_primitive_vector_fill(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CURRENT_SECONDS:
      schemerlicht_primitive_current_seconds(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CURRENT_MILLISECONDS:
      schemerlicht_primitive_current_milliseconds(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_LIST:
      schemerlicht_primitive_is_list(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_EQUAL:
      schemerlicht_primitive_string_equal(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_LESS:
      schemerlicht_primitive_string_less(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_GREATER:
      schemerlicht_primitive_string_greater(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_LEQ:
      schemerlicht_primitive_string_leq(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_GEQ:
      schemerlicht_primitive_string_geq(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_CI_EQUAL:
      schemerlicht_primitive_string_ci_equal(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_CI_LESS:
      schemerlicht_primitive_string_ci_less(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_CI_GREATER:
      schemerlicht_primitive_string_ci_greater(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_CI_LEQ:
      schemerlicht_primitive_string_ci_leq(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_CI_GEQ:
      schemerlicht_primitive_string_ci_geq(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_SUBSTRING:
      schemerlicht_primitive_substring(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_MAX:
      schemerlicht_primitive_max(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_MIN:
      schemerlicht_primitive_min(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_APPLY:
      schemerlicht_primitive_apply(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_APPEND:
      schemerlicht_primitive_append(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_EXACT:
      schemerlicht_primitive_is_exact(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_INEXACT:
      schemerlicht_primitive_is_inexact(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_NUMBER:
      schemerlicht_primitive_is_number(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_RATIONAL:
      schemerlicht_primitive_is_rational(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_POSITIVE:
      schemerlicht_primitive_is_positive(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_NEGATIVE:
      schemerlicht_primitive_is_negative(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_ODD:
      schemerlicht_primitive_is_odd(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_EVEN:
      schemerlicht_primitive_is_even(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_COMPLEX:
      schemerlicht_primitive_is_complex(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_REAL:
      schemerlicht_primitive_is_real(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_INTEGER:
      schemerlicht_primitive_is_integer(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_ABS:
      schemerlicht_primitive_abs(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_QUOTIENT:
      schemerlicht_primitive_quotient(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_REMAINDER:
      schemerlicht_primitive_remainder(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_MODULO:
      schemerlicht_primitive_modulo(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_GCD:
      schemerlicht_primitive_gcd(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_LCM:
      schemerlicht_primitive_lcm(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FLOOR:
      schemerlicht_primitive_floor(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CEILING:
      schemerlicht_primitive_ceiling(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_TRUNCATE:
      schemerlicht_primitive_truncate(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_ROUND:
      schemerlicht_primitive_round(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_EXP:
      schemerlicht_primitive_exp(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_EXPT:
      schemerlicht_primitive_expt(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_LOG:
      schemerlicht_primitive_log(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_SIN:
      schemerlicht_primitive_sin(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_COS:
      schemerlicht_primitive_cos(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_TAN:
      schemerlicht_primitive_tan(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_ASIN:
      schemerlicht_primitive_asin(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_ACOS:
      schemerlicht_primitive_acos(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_ATAN:
      schemerlicht_primitive_atan(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_SQRT:
      schemerlicht_primitive_sqrt(ctxt, a, b, c);
      break;
    default:
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
      break;
    }
#endif
  }

static void map_insert(schemerlicht_context* ctxt, schemerlicht_map* m, const char* str, int value)
  {
  schemerlicht_object* obj = schemerlicht_map_insert_string(ctxt, m, str);
  obj->type = schemerlicht_object_type_primitive;
  obj->value.fx = (schemerlicht_fixnum)value;
  }

schemerlicht_map* generate_primitives_map(schemerlicht_context* ctxt)
  {
#if 0
  init_primitive_ptrs(&g_prim_arr[0], ctxt);
#endif
  schemerlicht_map* m = schemerlicht_map_new(ctxt, 0, 8);
  map_insert(ctxt, m, "add1", SCHEMERLICHT_ADD1);
  map_insert(ctxt, m, "sub1", SCHEMERLICHT_SUB1);
  map_insert(ctxt, m, "+", SCHEMERLICHT_ADD);
  map_insert(ctxt, m, "-", SCHEMERLICHT_SUB);
  map_insert(ctxt, m, "*", SCHEMERLICHT_MUL);
  map_insert(ctxt, m, "/", SCHEMERLICHT_DIV);
  map_insert(ctxt, m, "=", SCHEMERLICHT_EQUAL);
  map_insert(ctxt, m, "!=", SCHEMERLICHT_NOT_EQUAL);
  map_insert(ctxt, m, "<", SCHEMERLICHT_LESS);
  map_insert(ctxt, m, "<=", SCHEMERLICHT_LEQ);
  map_insert(ctxt, m, ">", SCHEMERLICHT_GREATER);
  map_insert(ctxt, m, ">=", SCHEMERLICHT_GEQ);
  map_insert(ctxt, m, "fixnum?", SCHEMERLICHT_IS_FIXNUM);
  map_insert(ctxt, m, "flonum?", SCHEMERLICHT_IS_FLONUM);
  map_insert(ctxt, m, "null?", SCHEMERLICHT_IS_NULL);
  map_insert(ctxt, m, "zero?", SCHEMERLICHT_IS_ZERO);
  map_insert(ctxt, m, "boolean?", SCHEMERLICHT_IS_BOOLEAN);
  map_insert(ctxt, m, "char?", SCHEMERLICHT_IS_CHAR);
  map_insert(ctxt, m, "not", SCHEMERLICHT_NOT);
  map_insert(ctxt, m, "fxadd1", SCHEMERLICHT_FXADD1);
  map_insert(ctxt, m, "fxsub1", SCHEMERLICHT_FXSUB1);
  map_insert(ctxt, m, "fx+", SCHEMERLICHT_FXADD);
  map_insert(ctxt, m, "fx-", SCHEMERLICHT_FXSUB);
  map_insert(ctxt, m, "fx*", SCHEMERLICHT_FXMUL);
  map_insert(ctxt, m, "fx/", SCHEMERLICHT_FXDIV);
  map_insert(ctxt, m, "fx=?", SCHEMERLICHT_FXEQUAL);
  map_insert(ctxt, m, "fx<?", SCHEMERLICHT_FXLESS);
  map_insert(ctxt, m, "fx<=?", SCHEMERLICHT_FXLEQ);
  map_insert(ctxt, m, "fx>?", SCHEMERLICHT_FXGREATER);
  map_insert(ctxt, m, "fx>=?", SCHEMERLICHT_FXGEQ);
  map_insert(ctxt, m, "fxzero?", SCHEMERLICHT_FXZERO);
  map_insert(ctxt, m, "fladd1", SCHEMERLICHT_FLADD1);
  map_insert(ctxt, m, "flsub1", SCHEMERLICHT_FLSUB1);
  map_insert(ctxt, m, "fl+", SCHEMERLICHT_FLADD);
  map_insert(ctxt, m, "fl-", SCHEMERLICHT_FLSUB);
  map_insert(ctxt, m, "fl*", SCHEMERLICHT_FLMUL);
  map_insert(ctxt, m, "fl/", SCHEMERLICHT_FLDIV);
  map_insert(ctxt, m, "fl=?", SCHEMERLICHT_FLEQUAL);
  map_insert(ctxt, m, "fl<?", SCHEMERLICHT_FLLESS);
  map_insert(ctxt, m, "fl<=?", SCHEMERLICHT_FLLEQ);
  map_insert(ctxt, m, "fl>?", SCHEMERLICHT_FLGREATER);
  map_insert(ctxt, m, "fl>=?", SCHEMERLICHT_FLGEQ);
  map_insert(ctxt, m, "flzero?", SCHEMERLICHT_FLZERO);
  map_insert(ctxt, m, "char=?", SCHEMERLICHT_CHAREQUAL);
  map_insert(ctxt, m, "char<?", SCHEMERLICHT_CHARLESS);
  map_insert(ctxt, m, "char<=?", SCHEMERLICHT_CHARLEQ);
  map_insert(ctxt, m, "char>?", SCHEMERLICHT_CHARGREATER);
  map_insert(ctxt, m, "char>=?", SCHEMERLICHT_CHARGEQ);
  map_insert(ctxt, m, "fixnum->char", SCHEMERLICHT_FIXNUM_CHAR);
  map_insert(ctxt, m, "char->fixnum", SCHEMERLICHT_CHAR_FIXNUM);
  map_insert(ctxt, m, "fixnum->flonum", SCHEMERLICHT_FIXNUM_FLONUM);
  map_insert(ctxt, m, "flonum->fixnum", SCHEMERLICHT_FLONUM_FIXNUM);
  map_insert(ctxt, m, "bitwise-and", SCHEMERLICHT_BITWISE_AND);
  map_insert(ctxt, m, "bitwise-or", SCHEMERLICHT_BITWISE_OR);
  map_insert(ctxt, m, "bitwise-not", SCHEMERLICHT_BITWISE_NOT);
  map_insert(ctxt, m, "bitwise-xor", SCHEMERLICHT_BITWISE_XOR);
  map_insert(ctxt, m, "arithmetic-shift", SCHEMERLICHT_ARITHMETIC_SHIFT);
  map_insert(ctxt, m, "vector", SCHEMERLICHT_VECTOR);
  map_insert(ctxt, m, "vector-ref", SCHEMERLICHT_VECTORREF);
  map_insert(ctxt, m, "vector-set!", SCHEMERLICHT_VECTORSET);
  map_insert(ctxt, m, "vector?", SCHEMERLICHT_IS_VECTOR);
  map_insert(ctxt, m, "pair?", SCHEMERLICHT_IS_PAIR);
  map_insert(ctxt, m, "cons", SCHEMERLICHT_CONS);
  map_insert(ctxt, m, "car", SCHEMERLICHT_CAR);
  map_insert(ctxt, m, "cdr", SCHEMERLICHT_CDR);
  map_insert(ctxt, m, "halt", SCHEMERLICHT_HALT);
  map_insert(ctxt, m, "closure", SCHEMERLICHT_CLOSURE);
  map_insert(ctxt, m, "closure-ref", SCHEMERLICHT_CLOSUREREF);
  map_insert(ctxt, m, "closure?", SCHEMERLICHT_IS_CLOSURE);
  map_insert(ctxt, m, "list", SCHEMERLICHT_LIST);
  map_insert(ctxt, m, "make-vector", SCHEMERLICHT_MAKE_VECTOR);
  map_insert(ctxt, m, "make-string", SCHEMERLICHT_MAKE_STRING);
  map_insert(ctxt, m, "vector-length", SCHEMERLICHT_VECTOR_LENGTH);
  map_insert(ctxt, m, "string-length", SCHEMERLICHT_STRING_LENGTH);
  map_insert(ctxt, m, "string-ref", SCHEMERLICHT_STRING_REF);
  map_insert(ctxt, m, "string-set!", SCHEMERLICHT_STRING_SET);
  map_insert(ctxt, m, "string-hash", SCHEMERLICHT_STRING_HASH);
  map_insert(ctxt, m, "string?", SCHEMERLICHT_IS_STRING);
  map_insert(ctxt, m, "eq?", SCHEMERLICHT_EQ);
  map_insert(ctxt, m, "eqv?", SCHEMERLICHT_EQV);
  map_insert(ctxt, m, "equal?", SCHEMERLICHT_IS_EQUAL);
  map_insert(ctxt, m, "string", SCHEMERLICHT_STRING);
  map_insert(ctxt, m, "string->symbol", SCHEMERLICHT_STRING_TO_SYMBOL);
  map_insert(ctxt, m, "length", SCHEMERLICHT_LENGTH);
  map_insert(ctxt, m, "set-car!", SCHEMERLICHT_SET_CAR);
  map_insert(ctxt, m, "set-cdr!", SCHEMERLICHT_SET_CDR);
  map_insert(ctxt, m, "symbol?", SCHEMERLICHT_IS_SYMBOL);
  map_insert(ctxt, m, "procedure?", SCHEMERLICHT_IS_PROCEDURE);
  map_insert(ctxt, m, "reclaim", SCHEMERLICHT_RECLAIM);
  map_insert(ctxt, m, "memv", SCHEMERLICHT_MEMV);
  map_insert(ctxt, m, "memq", SCHEMERLICHT_MEMQ);
  map_insert(ctxt, m, "member", SCHEMERLICHT_MEMBER);
  map_insert(ctxt, m, "assv", SCHEMERLICHT_ASSV);
  map_insert(ctxt, m, "assq", SCHEMERLICHT_ASSQ);
  map_insert(ctxt, m, "assoc", SCHEMERLICHT_ASSOC);
  map_insert(ctxt, m, "symbol->string", SCHEMERLICHT_SYMBOL_TO_STRING);
  map_insert(ctxt, m, "string-append", SCHEMERLICHT_STRING_APPEND);
  map_insert(ctxt, m, "string-copy", SCHEMERLICHT_STRING_COPY);
  map_insert(ctxt, m, "string-fill!", SCHEMERLICHT_STRING_FILL);
  map_insert(ctxt, m, "vector-fill!", SCHEMERLICHT_VECTOR_FILL);
  map_insert(ctxt, m, "current-seconds", SCHEMERLICHT_CURRENT_SECONDS);
  map_insert(ctxt, m, "current-milliseconds", SCHEMERLICHT_CURRENT_MILLISECONDS);
  map_insert(ctxt, m, "list?", SCHEMERLICHT_IS_LIST);
  map_insert(ctxt, m, "string=?", SCHEMERLICHT_STRING_EQUAL);
  map_insert(ctxt, m, "string<?", SCHEMERLICHT_STRING_LESS);
  map_insert(ctxt, m, "string>?", SCHEMERLICHT_STRING_GREATER);
  map_insert(ctxt, m, "string<=?", SCHEMERLICHT_STRING_LEQ);
  map_insert(ctxt, m, "string>=?", SCHEMERLICHT_STRING_GEQ);
  map_insert(ctxt, m, "string-ci=?", SCHEMERLICHT_STRING_CI_EQUAL);
  map_insert(ctxt, m, "string-ci<?", SCHEMERLICHT_STRING_CI_LESS);
  map_insert(ctxt, m, "string-ci>?", SCHEMERLICHT_STRING_CI_GREATER);
  map_insert(ctxt, m, "string-ci<=?", SCHEMERLICHT_STRING_CI_LEQ);
  map_insert(ctxt, m, "string-ci>=?", SCHEMERLICHT_STRING_CI_GEQ);
  map_insert(ctxt, m, "substring", SCHEMERLICHT_SUBSTRING);
  map_insert(ctxt, m, "max", SCHEMERLICHT_MAX);
  map_insert(ctxt, m, "min", SCHEMERLICHT_MIN);
  map_insert(ctxt, m, "apply", SCHEMERLICHT_APPLY);
  map_insert(ctxt, m, "append", SCHEMERLICHT_APPEND);
  map_insert(ctxt, m, "exact?", SCHEMERLICHT_IS_EXACT);
  map_insert(ctxt, m, "inexact?", SCHEMERLICHT_IS_INEXACT);
  map_insert(ctxt, m, "number?", SCHEMERLICHT_IS_NUMBER);
  map_insert(ctxt, m, "rational?", SCHEMERLICHT_IS_RATIONAL);
  map_insert(ctxt, m, "positive?", SCHEMERLICHT_IS_POSITIVE);
  map_insert(ctxt, m, "negative?", SCHEMERLICHT_IS_NEGATIVE);
  map_insert(ctxt, m, "odd?", SCHEMERLICHT_IS_ODD);
  map_insert(ctxt, m, "even?", SCHEMERLICHT_IS_EVEN);
  map_insert(ctxt, m, "complex?", SCHEMERLICHT_IS_COMPLEX);
  map_insert(ctxt, m, "real?", SCHEMERLICHT_IS_REAL);
  map_insert(ctxt, m, "integer?", SCHEMERLICHT_IS_INTEGER);
  map_insert(ctxt, m, "abs", SCHEMERLICHT_ABS);
  map_insert(ctxt, m, "quotient", SCHEMERLICHT_QUOTIENT);
  map_insert(ctxt, m, "remainder", SCHEMERLICHT_REMAINDER);
  map_insert(ctxt, m, "modulo", SCHEMERLICHT_MODULO);
  map_insert(ctxt, m, "gcd", SCHEMERLICHT_GCD);
  map_insert(ctxt, m, "lcm", SCHEMERLICHT_LCM);
  map_insert(ctxt, m, "floor", SCHEMERLICHT_FLOOR);
  map_insert(ctxt, m, "ceiling", SCHEMERLICHT_CEILING);
  map_insert(ctxt, m, "truncate", SCHEMERLICHT_TRUNCATE);
  map_insert(ctxt, m, "round", SCHEMERLICHT_ROUND);
  map_insert(ctxt, m, "exp", SCHEMERLICHT_EXP);
  map_insert(ctxt, m, "expt", SCHEMERLICHT_EXPT);
  map_insert(ctxt, m, "log", SCHEMERLICHT_LOG);
  map_insert(ctxt, m, "sin", SCHEMERLICHT_SIN);
  map_insert(ctxt, m, "cos", SCHEMERLICHT_COS);
  map_insert(ctxt, m, "tan", SCHEMERLICHT_TAN);
  map_insert(ctxt, m, "asin", SCHEMERLICHT_ASIN);
  map_insert(ctxt, m, "acos", SCHEMERLICHT_ACOS);
  map_insert(ctxt, m, "atan", SCHEMERLICHT_ATAN);
  map_insert(ctxt, m, "sqrt", SCHEMERLICHT_SQRT);
  return m;
  }
