#include "primitives.h"
#include "context.h"
#include "object.h"
#include "error.h"
#include "gc.h"
#include "func.h"
#include "vm.h"
#include "syscalls.h"
#include "reader.h"
#include "preprocess.h"
#include "compiler.h"
#include "callcc.h"
#include "r5rs.h"
#include "inputoutput.h"
#include "modules.h"
#include "environment.h"
#include "dump.h"

#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
            if (arg->value.fx != 0)
              ret.value.fx /= arg->value.fx;
            else
              ret.type = schemerlicht_object_type_undefined;
            break;
          case schemerlicht_object_type_char:
            if (arg->value.ch != 0)
              ret.value.fx /= cast(schemerlicht_fixnum, arg->value.ch);
            else
              ret.type = schemerlicht_object_type_undefined;
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
        if (arg->value.ch != 0)
          ret.value.ch /= arg->value.ch;
        else
          ret.type = schemerlicht_object_type_undefined;
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

static char to_lower(char ch)
  {
  if (ch >= 'A' && ch <= 'Z')
    ch += ('a' - 'A');
  return ch;
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_charciequal(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHARCIEQUAL);
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
    if (to_lower(first_arg->value.ch) != to_lower(arg->value.ch))
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
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_LESS);
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
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXLESS);
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
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLLESS);
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
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHARLESS);
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

void schemerlicht_primitive_charciless(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHARCILESS);
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
    if (to_lower(first_arg->value.ch) >= to_lower(second_arg->value.ch))
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
        if (first_arg->value.fx < second_arg->value.fx)
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
        if (first_arg->value.fx < second_arg->value.fl)
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
        if (first_arg->value.fx < second_arg->value.ch)
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
        if (first_arg->value.fl < second_arg->value.fx)
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
        if (first_arg->value.fl < second_arg->value.fl)
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
        if (first_arg->value.fl < second_arg->value.ch)
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
        if (first_arg->value.ch < second_arg->value.fx)
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
        if (first_arg->value.ch < second_arg->value.fl)
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
        if (first_arg->value.ch < second_arg->value.ch)
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

void schemerlicht_primitive_fxgeq(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXGEQ);
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
    if (first_arg->value.fx < second_arg->value.fx)
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

void schemerlicht_primitive_flgeq(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLGEQ);
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
    if (first_arg->value.fl < second_arg->value.fl)
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

void schemerlicht_primitive_chargeq(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHARGEQ);
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
    if (first_arg->value.ch < second_arg->value.ch)
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

void schemerlicht_primitive_charcigeq(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHARCIGEQ);
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
    if (to_lower(first_arg->value.ch) < to_lower(second_arg->value.ch))
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

void schemerlicht_primitive_greater(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_GREATER);
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
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXGREATER);
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
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLGREATER);
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
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHARGREATER);
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

void schemerlicht_primitive_charcigreater(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHARCIGREATER);
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
    if (to_lower(first_arg->value.ch) <= to_lower(second_arg->value.ch))
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
        if (first_arg->value.fx > second_arg->value.fx)
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
        if (first_arg->value.fx > second_arg->value.fl)
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
        if (first_arg->value.fx > second_arg->value.ch)
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
        if (first_arg->value.fl > second_arg->value.fx)
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
        if (first_arg->value.fl > second_arg->value.fl)
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
        if (first_arg->value.fl > second_arg->value.ch)
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
        if (first_arg->value.ch > second_arg->value.fx)
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
        if (first_arg->value.ch > second_arg->value.fl)
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
        if (first_arg->value.ch > second_arg->value.ch)
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

void schemerlicht_primitive_fxleq(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXLEQ);
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
    if (first_arg->value.fx > second_arg->value.fx)
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

void schemerlicht_primitive_flleq(schemerlicht_context* ctxt, int a, int b, int c)
  {

  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLLEQ);
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
    if (first_arg->value.fl > second_arg->value.fl)
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

void schemerlicht_primitive_charleq(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHARLEQ);
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
    if (first_arg->value.ch > second_arg->value.ch)
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

void schemerlicht_primitive_charcileq(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHARCILEQ);
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
    if (to_lower(first_arg->value.ch) > to_lower(second_arg->value.ch))
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
    schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS, -1, -1, "null? needs an argument");
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
    else if (first_arg->type == schemerlicht_object_type_flonum)
      {
      ret.type = schemerlicht_object_type_flonum;
      ret.value.fl = first_arg->value.fl;
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
    else if (first_arg->type == schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_fixnum;
      ret.value.fx = first_arg->value.fx;
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
    schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS, -1, -1, "cons needs two arguments");
    v0->type = schemerlicht_object_type_undefined;
    v1->type = schemerlicht_object_type_undefined;
    }
  else if (b == 1)
    {
    schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS, -1, -1, "cons needs two arguments");
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
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, -1, -1, "cdr on an object that is not a pair");
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
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, -1, -1, "car on an object that is not a pair");
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
  schemerlicht_object* lambda = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
  if (lambda->type != schemerlicht_object_type_lambda)
    {
    schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, -1, -1, "closure without a lambda as first argument");
    }
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
#if 1
  schemerlicht_object ret;
  if (b < 2)
    {
    schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS, -1, -1, "closure-ref expects 2 arguments.");
    ret.type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* pos = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    if (v->type != schemerlicht_object_type_closure || pos->type != schemerlicht_object_type_fixnum)
      {
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, -1, -1, "closure-ref expects a closure and an index as argument.");
      ret.type = schemerlicht_object_type_undefined;
      }
    else
      {
      if (pos->value.fx < 0 || pos->value.fx >= v->value.v.vector_size) // out of bounds
        {
        schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, -1, -1, "closure-ref index is out of bounds.");
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

void schemerlicht_primitive_reclaim_garbage(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_RECLAIM_GARBAGE);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_void;
  UNUSED(b);
  UNUSED(c);
  schemerlicht_collect_garbage(ctxt);
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
          if (to_lower(s1->value.s.string_ptr[i]) != to_lower(s2->value.s.string_ptr[i]))
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
        if (to_lower(s1->value.s.string_ptr[i]) < to_lower(s2->value.s.string_ptr[i]))
          {
          v.type = schemerlicht_object_type_true;
          schemerlicht_set_object(ra, &v);
          return;
          }
        else if (to_lower(s1->value.s.string_ptr[i]) > to_lower(s2->value.s.string_ptr[i]))
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
        if (to_lower(s1->value.s.string_ptr[i]) > to_lower(s2->value.s.string_ptr[i]))
          {
          v.type = schemerlicht_object_type_true;
          schemerlicht_set_object(ra, &v);
          return;
          }
        else if (to_lower(s1->value.s.string_ptr[i]) < to_lower(s2->value.s.string_ptr[i]))
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
    if (last_arg->type != schemerlicht_object_type_pair && last_arg->type != schemerlicht_object_type_nil)
      {
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, -1, -1, "last argument of apply should be a list.");
      ra->type = schemerlicht_object_type_undefined;
      return;
      }
    --b;
    while (last_arg->type == schemerlicht_object_type_pair)
      {
      if (a + c + b >= schemerlicht_maxstack)
        {
        schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_MEMORY, -1, -1, "stack overflow in apply.");
        ra->type = schemerlicht_object_type_undefined;
        return;
        }
      schemerlicht_object* v0 = schemerlicht_vector_at(&last_arg->value.v, 0, schemerlicht_object);
      schemerlicht_object* v1 = schemerlicht_vector_at(&last_arg->value.v, 1, schemerlicht_object);
      ++b;
      schemerlicht_object* stack_pos = schemerlicht_vector_at(&ctxt->stack, a + c + b, schemerlicht_object);
      *stack_pos = *v0;
      last_arg = v1;
      }
    //schemerlicht_vector_at(&ctxt->stack, a + c + b + 1, schemerlicht_object)->type = schemerlicht_object_type_blocking;

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
      //schemerlicht_vector_push_back(ctxt, &ctxt->gcsave_list, *continuation, schemerlicht_object);
      schemerlicht_object oper = *op;


      //swap place of continuation and operator
      *op = ctxt->empty_continuation;
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
      schemerlicht_vector_at(&ctxt->stack, b + 1, schemerlicht_object)->type = schemerlicht_object_type_blocking; // Very important! variable arity lambdas should know till where the list goes

      schemerlicht_assert(oper.type == schemerlicht_object_type_closure);
      schemerlicht_object* lambda_obj = schemerlicht_vector_at(&oper.value.v, 0, schemerlicht_object);
      schemerlicht_assert(lambda_obj->type == schemerlicht_object_type_lambda);
      schemerlicht_function* lambda = cast(schemerlicht_function*, lambda_obj->value.ptr);
      schemerlicht_run(ctxt, lambda);

      schemerlicht_object ret = *schemerlicht_vector_at(&ctxt->stack, 1, schemerlicht_object); // return value is at position 1, as our fake continuation lambda is simply empty, which means: R0 == lambda itself, R1 == first lambda arg (which is return value)
      schemerlicht_set_object(ra, &ret);
      *continuation = original_continuation;
      //schemerlicht_vector_pop_back(&ctxt->gcsave_list);

      //schemerlicht_object* lam = schemerlicht_vector_begin(&continuation->value.v, schemerlicht_object);
      //schemerlicht_assert(lam->type == schemerlicht_object_type_lambda);
      //schemerlicht_assert(((schemerlicht_function*)lam->value.ptr)->function_definition.string_length < 1024);
      }
    else if (op->type == schemerlicht_object_type_lambda)
      {
      schemerlicht_assert(a == 0); // by construction apply is always represented as a funcall. Because of cps construction it means that all registers are available
      schemerlicht_assert(c == 1); // as apply is represented as object, it means that c == 1 because the vm wants to skip the continuation when calling the primitive apply.
      schemerlicht_object* continuation = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object); // save the original closure
      schemerlicht_object original_continuation = *continuation;
      //schemerlicht_vector_push_back(ctxt, &ctxt->gcsave_list, *continuation, schemerlicht_object);
      /*
      int cnt = 0;
      int found_block = 0;
      while (found_block == 0)
        {
        schemerlicht_object* stack_item = schemerlicht_vector_at(&ctxt->stack, a + 2 + cnt, schemerlicht_object);
        if (stack_item->type == schemerlicht_object_type_blocking)
          {
          found_block = 1;
          }
        else
          {
          ++cnt;
          schemerlicht_vector_push_back(ctxt, &ctxt->gcsave_list, *stack_item, schemerlicht_object);
          }
        }
        */
      schemerlicht_object oper = *op;

      //swap place of continuation and operator
      *op = ctxt->empty_continuation;
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
      schemerlicht_vector_at(&ctxt->stack, b + 1, schemerlicht_object)->type = schemerlicht_object_type_blocking; // Very important! variable arity lambdas should know till where the list goes
      schemerlicht_assert(oper.type == schemerlicht_object_type_lambda);
      schemerlicht_function* lambda = cast(schemerlicht_function*, oper.value.ptr);
      schemerlicht_run(ctxt, lambda);

      schemerlicht_object* ret = schemerlicht_vector_at(&ctxt->stack, 1, schemerlicht_object); // return value is at position 1, as our fake continuation lambda is simply empty, which means: R0 == lambda itself, R1 == first lambda arg (which is return value)
      schemerlicht_set_object(ra, ret);
      *continuation = original_continuation;
      /*
      for (int i = 0; i < cnt; ++i)
        {
        schemerlicht_object* stack_item = schemerlicht_vector_back(&ctxt->gcsave_list, schemerlicht_object);
        schemerlicht_set_object(schemerlicht_vector_at(&ctxt->stack, a + 2 + (cnt-i-1), schemerlicht_object), stack_item);
        schemerlicht_vector_pop_back(&ctxt->gcsave_list);
        }
      */
      //schemerlicht_vector_pop_back(&ctxt->gcsave_list);
      //schemerlicht_object* lam = schemerlicht_vector_begin(&continuation->value.v, schemerlicht_object);
      //schemerlicht_assert(lam->type == schemerlicht_object_type_lambda);
      //schemerlicht_assert(((schemerlicht_function*)lam->value.ptr)->function_definition.string_length < 1024);
      }
    else
      {
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, -1, -1, "attempt to use apply with non-procedure.");
      ra->type = schemerlicht_object_type_undefined;
      }
    }
  else
    {
    ra->type = schemerlicht_object_type_undefined;
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
    schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS, -1, -1, "append needs at least one list argument");
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
        if (i == (b - 1))
          {
          *last_list_obj = *current_list;
          }
        else
          {
          schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, -1, -1, "non list object as argument to append");
          break;
          }
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
        if (i == (b - 1)) // last object to append does not need to be a list
          {
          if (v1->type != schemerlicht_object_type_pair)
            {
            schemerlicht_set_object(last_list_obj, v1);
            }
          }
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
    if (arg->type == schemerlicht_object_type_fixnum)
      {
      obj.type = schemerlicht_object_type_true;
      }
    else if (arg->type == schemerlicht_object_type_flonum)
      {
      if ((arg->value.fl == arg->value.fl) && isfinite(arg->value.fl))
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
        if (arg->value.fx > 0)
          obj.type = schemerlicht_object_type_true;
        break;
      case schemerlicht_object_type_flonum:
        if (arg->value.fl > 0)
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
      if (arg->value.fl == cast(schemerlicht_flonum, fx))
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
  if (b == 0)
    return a;
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
  while ((a % b) > 0)
    {
    schemerlicht_fixnum R = a % b;
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
  return a * b / gcd(a, b);
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

  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        obj.type = schemerlicht_object_type_fixnum;
        obj.value.fx = arg->value.fx;
        break;
      case schemerlicht_object_type_flonum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = floor(arg->value.fl);
        break;
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_ceiling(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CEILING);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        obj.type = schemerlicht_object_type_fixnum;
        obj.value.fx = arg->value.fx;
        break;
      case schemerlicht_object_type_flonum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = ceil(arg->value.fl);
        break;
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_truncate(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_TRUNCATE);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        obj.type = schemerlicht_object_type_fixnum;
        obj.value.fx = arg->value.fx;
        break;
      case schemerlicht_object_type_flonum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = trunc(arg->value.fl);
        break;
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_round(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_ROUND);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        obj.type = schemerlicht_object_type_fixnum;
        obj.value.fx = arg->value.fx;
        break;
      case schemerlicht_object_type_flonum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = round(arg->value.fl);
        break;
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_exp(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_EXP);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = exp(cast(schemerlicht_flonum, arg->value.fx));
        break;
      case schemerlicht_object_type_flonum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = exp(arg->value.fl);
        break;
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_expt(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_EXPT);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 1)
    {
    schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* arg2 = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    if (arg1->type == schemerlicht_object_type_fixnum && arg2->type == schemerlicht_object_type_fixnum)
      {
      obj.type = schemerlicht_object_type_fixnum;
      obj.value.fx = pow(arg1->value.fx, arg2->value.fx);
      }
    else if ((arg1->type == schemerlicht_object_type_fixnum || arg1->type == schemerlicht_object_type_flonum) && (arg2->type == schemerlicht_object_type_fixnum || arg2->type == schemerlicht_object_type_flonum))
      {
      schemerlicht_flonum fl1 = arg1->value.fl;
      schemerlicht_flonum fl2 = arg2->value.fl;
      if (arg1->type == schemerlicht_object_type_fixnum)
        fl1 = cast(schemerlicht_flonum, arg1->value.fx);
      if (arg2->type == schemerlicht_object_type_fixnum)
        fl2 = cast(schemerlicht_flonum, arg2->value.fx);
      obj.type = schemerlicht_object_type_flonum;
      obj.value.fl = pow(fl1, fl2);
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_log(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_LOG);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = log(cast(schemerlicht_flonum, arg->value.fx));
        break;
      case schemerlicht_object_type_flonum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = log(arg->value.fl);
        break;
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_sin(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_SIN);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = sin(cast(schemerlicht_flonum, arg->value.fx));
        break;
      case schemerlicht_object_type_flonum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = sin(arg->value.fl);
        break;
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_cos(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_COS);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = cos(cast(schemerlicht_flonum, arg->value.fx));
        break;
      case schemerlicht_object_type_flonum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = cos(arg->value.fl);
        break;
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_tan(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_TAN);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = tan(cast(schemerlicht_flonum, arg->value.fx));
        break;
      case schemerlicht_object_type_flonum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = tan(arg->value.fl);
        break;
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_asin(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_ASIN);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = asin(cast(schemerlicht_flonum, arg->value.fx));
        break;
      case schemerlicht_object_type_flonum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = asin(arg->value.fl);
        break;
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_acos(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_ACOS);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = acos(cast(schemerlicht_flonum, arg->value.fx));
        break;
      case schemerlicht_object_type_flonum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = acos(arg->value.fl);
        break;
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_atan(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_ATAN);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b == 1)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = atan(cast(schemerlicht_flonum, arg->value.fx));
        break;
      case schemerlicht_object_type_flonum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = atan(arg->value.fl);
        break;
      default:
        break;
      }
    }
  else
    {
    schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    schemerlicht_object* arg2 = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
    if ((arg1->type == schemerlicht_object_type_fixnum || arg1->type == schemerlicht_object_type_flonum) && (arg2->type == schemerlicht_object_type_fixnum || arg2->type == schemerlicht_object_type_flonum))
      {
      schemerlicht_flonum fl1 = arg1->value.fl;
      schemerlicht_flonum fl2 = arg2->value.fl;
      if (arg1->type == schemerlicht_object_type_fixnum)
        fl1 = cast(schemerlicht_flonum, arg1->value.fx);
      if (arg2->type == schemerlicht_object_type_fixnum)
        fl2 = cast(schemerlicht_flonum, arg2->value.fx);
      obj.type = schemerlicht_object_type_flonum;
      obj.value.fl = atan2(fl1, fl2);
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_sqrt(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_SQRT);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = sqrt(cast(schemerlicht_flonum, arg->value.fx));
        break;
      case schemerlicht_object_type_flonum:
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = sqrt(arg->value.fl);
        break;
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

static void to_binary(char* str, schemerlicht_fixnum a)
  {
  if (a == 0)
    {
    str[0] = '0';
    str[1] = 0;
    }
  else
    {
    int len = ceil(log2(a));
    char* s = str + len;
    *s-- = 0;
    while (a > 0)
      {
      *s-- = a % 2 + '0';
      a /= 2;
      }
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_number_string(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_NUMBER_STRING);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_fixnum || arg->type == schemerlicht_object_type_flonum)
      {
      int base = 10;
      if (b > 1)
        {
        schemerlicht_object* arg_base = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
        if (arg_base->type == schemerlicht_object_type_fixnum || arg_base->type == schemerlicht_object_type_flonum)
          {
          schemerlicht_fixnum fx = arg_base->value.fx;
          if (arg_base->type == schemerlicht_object_type_flonum)
            fx = cast(schemerlicht_fixnum, arg_base->value.fl);
          switch (fx)
            {
            case 2: base = 2; break;
            case 8: base = 8; break;
            case 10: base = 10; break;
            case 16: base = 16; break;
            }
          }
        }
      if (arg->type == schemerlicht_object_type_fixnum)
        {
        char buffer[256];
        switch (base)
          {
          case 2:
            to_binary(buffer, arg->value.fx);
            break;
          case 8:
            sprintf(buffer, "%llo", arg->value.fx);
            break;
          case 16:
            sprintf(buffer, "%llx", arg->value.fx);
            break;
          default:
            sprintf(buffer, "%lld", arg->value.fx);
            break;
          }
        obj.type = schemerlicht_object_type_string;
        schemerlicht_string_init(ctxt, &obj.value.s, buffer);
        }
      else if (arg->type == schemerlicht_object_type_flonum)
        {
        if (arg->value.fl != arg->value.fl)
          {
          obj.type = schemerlicht_object_type_string;
          schemerlicht_string_init(ctxt, &obj.value.s, "+nan.0");
          }
        else if (isfinite(arg->value.fl))
          {
          char buffer[256];
          sprintf(buffer, "%.20g", arg->value.fl);
          obj.type = schemerlicht_object_type_string;
          schemerlicht_string_init(ctxt, &obj.value.s, buffer);
          }
        else if (arg->value.fl < 0)
          {
          obj.type = schemerlicht_object_type_string;
          schemerlicht_string_init(ctxt, &obj.value.s, "-inf.0");
          }
        else
          {
          obj.type = schemerlicht_object_type_string;
          schemerlicht_string_init(ctxt, &obj.value.s, "+inf.0");
          }
        }
      schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
      ++ctxt->heap_pos;
      schemerlicht_set_object(heap_obj, &obj);
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_number(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_STRING_NUMBER);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_false;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_string && arg->value.s.string_length > 0)
      {
      int base = 10;
      if (b > 1)
        {
        schemerlicht_object* arg_base = schemerlicht_vector_at(&ctxt->stack, a + c + 2, schemerlicht_object);
        if (arg_base->type == schemerlicht_object_type_fixnum || arg_base->type == schemerlicht_object_type_flonum)
          {
          schemerlicht_fixnum fx = arg_base->value.fx;
          if (arg_base->type == schemerlicht_object_type_flonum)
            fx = cast(schemerlicht_fixnum, arg_base->value.fl);
          switch (fx)
            {
            case 2: base = 2; break;
            case 8: base = 8; break;
            case 10: base = 10; break;
            case 16: base = 16; break;
            }
          }
        }
      char* endptr;
      obj.type = schemerlicht_object_type_fixnum;
      obj.value.fx = strtoll(arg->value.s.string_ptr, &endptr, base);
      uintptr_t dist = endptr - arg->value.s.string_ptr;
      if (dist < arg->value.s.string_length)
        {
        obj.type = schemerlicht_object_type_flonum;
        obj.value.fl = strtod(arg->value.s.string_ptr, &endptr);
        dist = endptr - arg->value.s.string_ptr;
        if (dist < arg->value.s.string_length)
          obj.type = schemerlicht_object_type_false;
        }
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_nan(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_NAN);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        obj.type = schemerlicht_object_type_false;
        break;
      case schemerlicht_object_type_flonum:
        obj.type = arg->value.fl == arg->value.fl ? schemerlicht_object_type_false : schemerlicht_object_type_true;
        break;
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_inf(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_INF);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        obj.type = schemerlicht_object_type_false;
        break;
      case schemerlicht_object_type_flonum:
        obj.type = isfinite(arg->value.fl) ? schemerlicht_object_type_false : (arg->value.fl == arg->value.fl ? schemerlicht_object_type_true : schemerlicht_object_type_false);
        break;
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_finite(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_FINITE);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + c + 1, schemerlicht_object);
    switch (arg->type)
      {
      case schemerlicht_object_type_fixnum:
        obj.type = schemerlicht_object_type_true;
        break;
      case schemerlicht_object_type_flonum:
        obj.type = isfinite(arg->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false;
        break;
      default:
        break;
      }
    }
  schemerlicht_set_object(ra, &obj);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_list_ref(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_LIST_REF);
  schemerlicht_object ret;
  if (b < 2)
    {
    ret.type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* l = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* pos = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    if ((l->type != schemerlicht_object_type_pair && l->type != schemerlicht_object_type_nil) || pos->type != schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "list-ref: expects a list and fixnum as arguments");
      }
    else
      {
      schemerlicht_fixnum idx = pos->value.fx;
      if (idx < 0)
        {
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "list-ref: position is negative");
        }
      else
        {
        if (idx == 0)
          {
          if (l->type == schemerlicht_object_type_nil)
            {
            schemerlicht_set_object(&ret, l);
            }
          else
            {
            schemerlicht_object* v0 = schemerlicht_vector_at(&l->value.v, 0, schemerlicht_object);
            schemerlicht_set_object(&ret, v0);
            }
          }
        else
          {
          while (idx > 0 && (l->type == schemerlicht_object_type_pair))
            {
            schemerlicht_object* v1 = schemerlicht_vector_at(&l->value.v, 1, schemerlicht_object);
            l = v1;
            --idx;
            }
          if (idx > 0)
            {
            ret.type = schemerlicht_object_type_undefined;
            if (l->type == schemerlicht_object_type_pair)
              schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "list-ref: position is out of bounds");
            else
              schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "list-ref: not a valid list");
            }
          else
            {
            if (l->type == schemerlicht_object_type_nil)
              {
              schemerlicht_set_object(&ret, l);
              }
            else if (l->type == schemerlicht_object_type_pair)
              {
              schemerlicht_object* v0 = schemerlicht_vector_at(&l->value.v, 0, schemerlicht_object);
              schemerlicht_set_object(&ret, v0);
              }
            else
              {
              ret.type = schemerlicht_object_type_undefined;
              schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "list-ref: not a valid list");
              }
            }
          }
        }
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_list_tail(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_LIST_TAIL);
  schemerlicht_object ret;
  if (b < 2)
    {
    ret.type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* l = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* pos = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    if ((l->type != schemerlicht_object_type_pair && l->type != schemerlicht_object_type_nil) || pos->type != schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "list-tail: expects a list and fixnum as arguments");
      }
    else
      {
      schemerlicht_fixnum idx = pos->value.fx;
      if (idx < 0)
        {
        ret.type = schemerlicht_object_type_undefined;
        schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "list-tail: position is negative");
        }
      else
        {
        if (idx == 0)
          {
          schemerlicht_set_object(&ret, l);
          }
        else
          {
          while (idx > 0 && (l->type == schemerlicht_object_type_pair))
            {
            schemerlicht_object* v1 = schemerlicht_vector_at(&l->value.v, 1, schemerlicht_object);
            l = v1;
            --idx;
            }
          if (idx > 0)
            {
            ret.type = schemerlicht_object_type_undefined;
            if (l->type == schemerlicht_object_type_pair)
              schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "list-tail: position is out of bounds");
            else
              schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "list-tail: not a valid list");
            }
          else
            {
            schemerlicht_set_object(&ret, l);
            }
          }
        }
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_reverse(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_REVERSE);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "reverse: expects a list as argument");
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* l = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (l->type != schemerlicht_object_type_nil && l->type != schemerlicht_object_type_pair)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "reverse: expects a list as argument");
      schemerlicht_set_object(ra, &ret);
      }
    else
      {
      schemerlicht_vector v;
      schemerlicht_vector_init(ctxt, &v, schemerlicht_object);
      while (l->type != schemerlicht_object_type_nil)
        {
        schemerlicht_object* v0 = schemerlicht_vector_at(&l->value.v, 0, schemerlicht_object);
        schemerlicht_object* v1 = schemerlicht_vector_at(&l->value.v, 1, schemerlicht_object);
        schemerlicht_vector_push_back(ctxt, &v, *v0, schemerlicht_object);
        l = v1;
        if (l->type != schemerlicht_object_type_nil && l->type != schemerlicht_object_type_pair)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_undefined;
          schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "reverse: not a valid list");
          schemerlicht_set_object(ra, &ret);
          schemerlicht_vector_destroy(ctxt, &v);
          return;
          }
        }
      if (v.vector_size == 0)
        {
        schemerlicht_object ret;
        ret.type = schemerlicht_object_type_nil;
        schemerlicht_set_object(ra, &ret);
        }
      else
        {
        schemerlicht_object obj1 = make_schemerlicht_object_pair(ctxt);
        schemerlicht_object* v0 = schemerlicht_vector_at(&obj1.value.v, 0, schemerlicht_object);
        schemerlicht_object* v1 = schemerlicht_vector_at(&obj1.value.v, 1, schemerlicht_object);
        v1->type = schemerlicht_object_type_nil;
        schemerlicht_object* first_arg = schemerlicht_vector_begin(&v, schemerlicht_object);
        schemerlicht_set_object(v0, first_arg);
        schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
        schemerlicht_set_object(heap_obj, &obj1);
        ++ctxt->heap_pos;
        for (schemerlicht_memsize j = 1; j < v.vector_size; ++j)
          {
          schemerlicht_object obj2 = make_schemerlicht_object_pair(ctxt);
          v0 = schemerlicht_vector_at(&obj2.value.v, 0, schemerlicht_object);
          v1 = schemerlicht_vector_at(&obj2.value.v, 1, schemerlicht_object);
          schemerlicht_set_object(v1, heap_obj);
          schemerlicht_object* arg = schemerlicht_vector_at(&v, j, schemerlicht_object);
          schemerlicht_set_object(v0, arg);
          heap_obj = &ctxt->heap[ctxt->heap_pos];
          schemerlicht_set_object(heap_obj, &obj2);
          ++ctxt->heap_pos;
          }
        schemerlicht_set_object(ra, heap_obj);
        }
      schemerlicht_vector_destroy(ctxt, &v);
      }
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_char_upcase(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHAR_UPCASE);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* ch = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (ch->type == schemerlicht_object_type_char)
      {
      ret.type = schemerlicht_object_type_char;
      if (ch->value.ch >= 'a' && ch->value.ch <= 'z')
        ret.value.ch = ch->value.ch - ('a' - 'A');
      else
        ret.value.ch = ch->value.ch;
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_char_downcase(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHAR_DOWNCASE);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_undefined;
  if (b > 0)
    {
    schemerlicht_object* ch = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (ch->type == schemerlicht_object_type_char)
      {
      ret.type = schemerlicht_object_type_char;
      if (ch->value.ch >= 'A' && ch->value.ch <= 'Z')
        ret.value.ch = ch->value.ch + ('a' - 'A');
      else
        ret.value.ch = ch->value.ch;
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_char_is_upper(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHAR_IS_UPPER);
  schemerlicht_object ret;
  if (b == 0)
    {
    ret.type = schemerlicht_object_type_undefined;
    }
  else
    {
    ret.type = schemerlicht_object_type_false;
    schemerlicht_object* ch = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (ch->type == schemerlicht_object_type_char)
      {
      if (ch->value.ch >= 'A' && ch->value.ch <= 'Z')
        ret.type = schemerlicht_object_type_true;
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_char_is_lower(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHAR_IS_LOWER);
  schemerlicht_object ret;
  if (b == 0)
    {
    ret.type = schemerlicht_object_type_undefined;
    }
  else
    {
    ret.type = schemerlicht_object_type_false;
    schemerlicht_object* ch = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (ch->type == schemerlicht_object_type_char)
      {
      if (ch->value.ch >= 'a' && ch->value.ch <= 'z')
        ret.type = schemerlicht_object_type_true;
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_char_is_alpha(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHAR_IS_ALPHA);
  schemerlicht_object ret;
  if (b == 0)
    {
    ret.type = schemerlicht_object_type_undefined;
    }
  else
    {
    ret.type = schemerlicht_object_type_false;
    schemerlicht_object* ch = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (ch->type == schemerlicht_object_type_char)
      {
      if ((ch->value.ch >= 'A' && ch->value.ch <= 'Z') || (ch->value.ch >= 'a' && ch->value.ch <= 'z'))
        ret.type = schemerlicht_object_type_true;
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_char_is_numeric(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHAR_IS_NUMERIC);
  schemerlicht_object ret;
  if (b == 0)
    {
    ret.type = schemerlicht_object_type_undefined;
    }
  else
    {
    ret.type = schemerlicht_object_type_false;
    schemerlicht_object* ch = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (ch->type == schemerlicht_object_type_char)
      {
      if (ch->value.ch >= '0' && ch->value.ch <= '9')
        ret.type = schemerlicht_object_type_true;
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_char_is_whitespace(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHAR_IS_WHITESPACE);
  schemerlicht_object ret;
  if (b == 0)
    {
    ret.type = schemerlicht_object_type_undefined;
    }
  else
    {
    ret.type = schemerlicht_object_type_false;
    schemerlicht_object* ch = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (ch->type == schemerlicht_object_type_char)
      {
      switch (ch->value.ch)
        {
        case 9:
        case 10:
        case 12:
        case 13:
        case 32:
          ret.type = schemerlicht_object_type_true;
          break;
        default:
          break;
        }
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_string_list(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_STRING_LIST);
  schemerlicht_object ret;
  if (b == 0)
    {
    ret.type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_string)
      {
      if (v->value.s.string_length == 0)
        {
        ret.type = schemerlicht_object_type_nil;
        schemerlicht_set_object(ra, &ret);
        }
      else
        {
        schemerlicht_object obj1 = make_schemerlicht_object_pair(ctxt);
        schemerlicht_object* v0 = schemerlicht_vector_at(&obj1.value.v, 0, schemerlicht_object);
        schemerlicht_object* v1 = schemerlicht_vector_at(&obj1.value.v, 1, schemerlicht_object);
        v1->type = schemerlicht_object_type_nil;
        char* last_arg = schemerlicht_string_back(&v->value.s);
        v0->type = schemerlicht_object_type_char;
        v0->value.ch = *last_arg;
        schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
        schemerlicht_set_object(heap_obj, &obj1);
        ++ctxt->heap_pos;
        for (schemerlicht_memsize j = v->value.s.string_length - 1; j >= 1; --j)
          {
          schemerlicht_object obj2 = make_schemerlicht_object_pair(ctxt);
          v0 = schemerlicht_vector_at(&obj2.value.v, 0, schemerlicht_object);
          v1 = schemerlicht_vector_at(&obj2.value.v, 1, schemerlicht_object);
          schemerlicht_set_object(v1, heap_obj);
          char* arg = schemerlicht_string_at(&v->value.s, j - 1);
          v0->type = schemerlicht_object_type_char;
          v0->value.ch = *arg;
          heap_obj = &ctxt->heap[ctxt->heap_pos];
          schemerlicht_set_object(heap_obj, &obj2);
          ++ctxt->heap_pos;
          }
        schemerlicht_set_object(&ret, heap_obj);
        }
      }
    else
      {
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "vector->list: argument was not a vector");
      ret.type = schemerlicht_object_type_undefined;
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_list_string(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_LIST_STRING);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "list->string: expects a list as argument");
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* l = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (l->type != schemerlicht_object_type_nil && l->type != schemerlicht_object_type_pair)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "list->string: expects a list as argument");
      schemerlicht_set_object(ra, &ret);
      }
    else
      {
      schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
      ++ctxt->heap_pos;
      *heap_obj = make_schemerlicht_object_string(ctxt, "");
      while (l->type != schemerlicht_object_type_nil)
        {
        schemerlicht_object* v0 = schemerlicht_vector_at(&l->value.v, 0, schemerlicht_object);
        schemerlicht_object* v1 = schemerlicht_vector_at(&l->value.v, 1, schemerlicht_object);
        if (v0->type != schemerlicht_object_type_char)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_undefined;
          schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "list->string: list should contain characters only");
          schemerlicht_set_object(ra, &ret);
          return;
          }
        schemerlicht_string_push_back(ctxt, &heap_obj->value.s, v0->value.ch);
        l = v1;
        if (l->type != schemerlicht_object_type_nil && l->type != schemerlicht_object_type_pair)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_undefined;
          schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "list->string: not a valid list");
          schemerlicht_set_object(ra, &ret);
          return;
          }
        }
      schemerlicht_set_object(ra, heap_obj);
      }
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_vector_list(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_VECTOR_LIST);
  schemerlicht_object ret;
  if (b == 0)
    {
    ret.type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_vector)
      {
      if (v->value.v.vector_size == 0)
        {
        ret.type = schemerlicht_object_type_nil;
        schemerlicht_set_object(ra, &ret);
        }
      else
        {
        schemerlicht_object obj1 = make_schemerlicht_object_pair(ctxt);
        schemerlicht_object* v0 = schemerlicht_vector_at(&obj1.value.v, 0, schemerlicht_object);
        schemerlicht_object* v1 = schemerlicht_vector_at(&obj1.value.v, 1, schemerlicht_object);
        v1->type = schemerlicht_object_type_nil;
        schemerlicht_object* last_arg = schemerlicht_vector_back(&v->value.v, schemerlicht_object);
        schemerlicht_set_object(v0, last_arg);
        schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
        schemerlicht_set_object(heap_obj, &obj1);
        ++ctxt->heap_pos;
        for (schemerlicht_memsize j = v->value.v.vector_size - 1; j >= 1; --j)
          {
          schemerlicht_object obj2 = make_schemerlicht_object_pair(ctxt);
          v0 = schemerlicht_vector_at(&obj2.value.v, 0, schemerlicht_object);
          v1 = schemerlicht_vector_at(&obj2.value.v, 1, schemerlicht_object);
          schemerlicht_set_object(v1, heap_obj);
          schemerlicht_object* arg = schemerlicht_vector_at(&v->value.v, j - 1, schemerlicht_object);
          schemerlicht_set_object(v0, arg);
          heap_obj = &ctxt->heap[ctxt->heap_pos];
          schemerlicht_set_object(heap_obj, &obj2);
          ++ctxt->heap_pos;
          }
        schemerlicht_set_object(&ret, heap_obj);
        }
      }
    else
      {
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "vector->list: argument was not a vector");
      ret.type = schemerlicht_object_type_undefined;
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_list_vector(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_LIST_VECTOR);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "list->vector: expects a list as argument");
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* l = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (l->type != schemerlicht_object_type_nil && l->type != schemerlicht_object_type_pair)
      {
      schemerlicht_object ret;
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "list->vector: expects a list as argument");
      schemerlicht_set_object(ra, &ret);
      }
    else
      {
      schemerlicht_vector v;
      schemerlicht_vector_init(ctxt, &v, schemerlicht_object);
      while (l->type != schemerlicht_object_type_nil)
        {
        schemerlicht_object* v0 = schemerlicht_vector_at(&l->value.v, 0, schemerlicht_object);
        schemerlicht_object* v1 = schemerlicht_vector_at(&l->value.v, 1, schemerlicht_object);
        schemerlicht_vector_push_back(ctxt, &v, *v0, schemerlicht_object);
        l = v1;
        if (l->type != schemerlicht_object_type_nil && l->type != schemerlicht_object_type_pair)
          {
          schemerlicht_object ret;
          ret.type = schemerlicht_object_type_undefined;
          schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "list->vector: not a valid list");
          schemerlicht_set_object(ra, &ret);
          schemerlicht_vector_destroy(ctxt, &v);
          return;
          }
        }
      schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
      ++ctxt->heap_pos;
      *heap_obj = make_schemerlicht_object_vector(ctxt, v.vector_size);
      schemerlicht_object* vit = schemerlicht_vector_begin(&v, schemerlicht_object);
      schemerlicht_object* vit_end = schemerlicht_vector_end(&v, schemerlicht_object);
      schemerlicht_object* it = schemerlicht_vector_begin(&heap_obj->value.v, schemerlicht_object);
      for (; vit != vit_end; ++vit, ++it)
        {
        schemerlicht_set_object(it, vit);
        }
      schemerlicht_vector_destroy(ctxt, &v);
      schemerlicht_set_object(ra, heap_obj);
      }
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_make_promise(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_MAKE_PROMISE);
  schemerlicht_object v = make_schemerlicht_object_promise(ctxt);
  if (b > 0)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* obj_at_pos = schemerlicht_vector_begin(&v.value.v, schemerlicht_object);
    schemerlicht_set_object(obj_at_pos, arg);
    }
  else
    {
    schemerlicht_object* obj_at_pos = schemerlicht_vector_begin(&v.value.v, schemerlicht_object);
    obj_at_pos->type = schemerlicht_object_type_undefined;
    }
  schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
  schemerlicht_set_object(heap_obj, &v);
  ++ctxt->heap_pos;
  schemerlicht_set_object(ra, &v);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_promise(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_PROMISE);
  schemerlicht_object ret;
  if (b > 0)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_promise)
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

void schemerlicht_primitive_slot_ref(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_SLOT_REF);
  schemerlicht_object ret;
  if (b < 2)
    {
    ret.type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* pos = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    if (pos->type != schemerlicht_object_type_fixnum)
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

void schemerlicht_primitive_slot_set(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_SLOT_SET);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_void;
  if (b > 2)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* pos = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    schemerlicht_object* value = schemerlicht_vector_at(&ctxt->stack, a + 3 + c, schemerlicht_object);
    if (pos->type == schemerlicht_object_type_fixnum)
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

void schemerlicht_primitive_make_port(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_MAKE_PORT);
  if (b < 6)
    {
    ra->type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object v = make_schemerlicht_object_port(ctxt);

    for (int i = 0; i < 6; ++i)
      {
      schemerlicht_object* ri = schemerlicht_vector_at(&ctxt->stack, a + c + 1 + i, schemerlicht_object);
      schemerlicht_object* oi = schemerlicht_vector_at(&v.value.v, i, schemerlicht_object);
      schemerlicht_set_object(oi, ri);
      switch (i)
        {
        case 0:
          if (ri->type != schemerlicht_object_type_true && ri->type != schemerlicht_object_type_false)
            {
            schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "first argument of %make-port should be #t or #f.");
            }
          break;
        case 1:
          if (ri->type != schemerlicht_object_type_string)
            {
            schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "second argument of %make-port should be a string indicating the port name.");
            }
          break;
        case 2:
          if (ri->type != schemerlicht_object_type_fixnum)
            {
            schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "third argument of %make-port should be a fixnum representing the file descriptor.");
            }
          break;
        case 3:
          if (ri->type != schemerlicht_object_type_string)
            {
            schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "fourth argument of %make-port should be a string representing the port buffer.");
            }
          break;
        case 4:
          if (ri->type != schemerlicht_object_type_fixnum)
            {
            schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "fifth argument of %make-port should be an index pointing to the next position in the buffer.");
            }
          break;
        case 5:
          if (ri->type != schemerlicht_object_type_fixnum)
            {
            schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "sixth argument of %make-port should be an index representing the size of the buffer.");
            }
          schemerlicht_object* bytes_read = schemerlicht_vector_at(&v.value.v, 6, schemerlicht_object);
          bytes_read->type = schemerlicht_object_type_fixnum;
          bytes_read->value.fx = ri->value.fx;
          break;
        default:
          break;
        }
      }

    schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
    schemerlicht_set_object(heap_obj, &v);
    ++ctxt->heap_pos;
    schemerlicht_set_object(ra, &v);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_port(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_PORT);
  schemerlicht_object ret;
  if (b > 0)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_port)
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

void schemerlicht_primitive_write_char(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_WRITE_CHAR);
  if (b > 1)
    {
    schemerlicht_object* ch = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* p = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    if (ch->type == schemerlicht_object_type_char && p->type == schemerlicht_object_type_port && schemerlicht_vector_begin(&p->value.v, schemerlicht_object)->type != schemerlicht_object_type_true)
      {
      int fileid = cast(int, schemerlicht_vector_at(&p->value.v, 2, schemerlicht_object)->value.fx);
      if (fileid == SCHEMERLICHT_STRING_PORT_ID)
        {
        schemerlicht_fixnum required_length = schemerlicht_vector_at(&p->value.v, 4, schemerlicht_object)->value.fx + 1;
        schemerlicht_fixnum available_length = schemerlicht_vector_at(&p->value.v, 5, schemerlicht_object)->value.fx;
        if (required_length > available_length)
          {
          schemerlicht_object* s = schemerlicht_vector_at(&p->value.v, 3, schemerlicht_object);
          schemerlicht_string new_string;
          schemerlicht_string_init_with_size(ctxt, &new_string, 256 + s->value.s.string_length, 0);
          s->value.s = new_string; // old string is on the heap and will be cleaned up by gc
          schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
          schemerlicht_set_object(heap_obj, s);
          ++ctxt->heap_pos;
          schemerlicht_vector_at(&p->value.v, 5, schemerlicht_object)->value.fx = new_string.string_length;
          }
        }
      schemerlicht_fixnum current_pos = schemerlicht_vector_at(&p->value.v, 4, schemerlicht_object)->value.fx;
      schemerlicht_fixnum buffer_length = schemerlicht_vector_at(&p->value.v, 5, schemerlicht_object)->value.fx;
      schemerlicht_object* buffer = schemerlicht_vector_at(&p->value.v, 3, schemerlicht_object);
      if (current_pos >= buffer_length)
        {
        schemerlicht_write(fileid, buffer->value.s.string_ptr, current_pos);
        current_pos = 0;
        }
      buffer->value.s.string_ptr[current_pos++] = ch->value.ch;
      schemerlicht_vector_at(&p->value.v, 4, schemerlicht_object)->value.fx = current_pos;
      }
    else
      {
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "%write-char expects a character and a port as arguments.");
      }
    }
  ra->type = schemerlicht_object_type_void;
  }


////////////////////////////////////////////////////

void schemerlicht_primitive_flush_output_port(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLUSH_OUTPUT_PORT);
  if (b > 0)
    {
    schemerlicht_object* p = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (p->type == schemerlicht_object_type_port && schemerlicht_vector_begin(&p->value.v, schemerlicht_object)->type != schemerlicht_object_type_true)
      {
      int fileid = cast(int, schemerlicht_vector_at(&p->value.v, 2, schemerlicht_object)->value.fx);
      if (fileid >= 0)
        {
        schemerlicht_fixnum current_pos = schemerlicht_vector_at(&p->value.v, 4, schemerlicht_object)->value.fx;
        schemerlicht_fixnum buffer_length = schemerlicht_vector_at(&p->value.v, 5, schemerlicht_object)->value.fx;
        schemerlicht_object* buffer = schemerlicht_vector_at(&p->value.v, 3, schemerlicht_object);
        schemerlicht_write(fileid, buffer->value.s.string_ptr, current_pos);
        schemerlicht_vector_at(&p->value.v, 4, schemerlicht_object)->value.fx = 0;
        }
      }
    else
      {
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "%flush-output-port expects and output port as argument.");
      }
    }
  ra->type = schemerlicht_object_type_void;
  }

////////////////////////////////////////////////////

static int get_char(schemerlicht_byte* ch, schemerlicht_object* p, int read)
  {
  schemerlicht_assert(p->type == schemerlicht_object_type_port);
  schemerlicht_assert(schemerlicht_vector_begin(&p->value.v, schemerlicht_object)->type == schemerlicht_object_type_true);
  int fileid = cast(int, schemerlicht_vector_at(&p->value.v, 2, schemerlicht_object)->value.fx);
  schemerlicht_fixnum current_pos = schemerlicht_vector_at(&p->value.v, 4, schemerlicht_object)->value.fx;
  schemerlicht_fixnum buffer_length = schemerlicht_vector_at(&p->value.v, 5, schemerlicht_object)->value.fx;
  schemerlicht_fixnum bytes_read = schemerlicht_vector_at(&p->value.v, 6, schemerlicht_object)->value.fx;
  schemerlicht_object* buffer = schemerlicht_vector_at(&p->value.v, 3, schemerlicht_object);
  if (current_pos >= bytes_read)
    { // read buffer
    int bytes_last_read = schemerlicht_read(fileid, buffer->value.s.string_ptr, buffer_length);
    current_pos = 0;
    schemerlicht_vector_at(&p->value.v, 4, schemerlicht_object)->value.fx = 0;
    schemerlicht_vector_at(&p->value.v, 6, schemerlicht_object)->value.fx = cast(schemerlicht_fixnum, bytes_last_read);
    if (bytes_last_read == 0)
      {
      return 0;
      }
    }
  *ch = buffer->value.s.string_ptr[current_pos];
  schemerlicht_vector_at(&p->value.v, 4, schemerlicht_object)->value.fx = current_pos + read;
  return 1;
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_read_char(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_READ_CHAR);
  if (b > 0)
    {
    schemerlicht_object* p = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (p->type == schemerlicht_object_type_port && schemerlicht_vector_begin(&p->value.v, schemerlicht_object)->type == schemerlicht_object_type_true)
      {
      if (get_char(&ra->value.ch, p, 1))
        {
        ra->type = schemerlicht_object_type_char;
        }
      else
        {
        ra->type = schemerlicht_object_type_eof;
        }
      }
    else
      {
      ra->type = schemerlicht_object_type_undefined;
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "%read-char expects a port as argument.");
      }
    }
  else
    {
    ra->type = schemerlicht_object_type_undefined;
    schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "%read-char expects a port as argument.");
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_peek_char(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_PEEK_CHAR);
  if (b > 0)
    {
    schemerlicht_object* p = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (p->type == schemerlicht_object_type_port && schemerlicht_vector_begin(&p->value.v, schemerlicht_object)->type == schemerlicht_object_type_true)
      {
      if (get_char(&ra->value.ch, p, 0))
        {
        ra->type = schemerlicht_object_type_char;
        }
      else
        {
        ra->type = schemerlicht_object_type_eof;
        }
      }
    else
      {
      ra->type = schemerlicht_object_type_undefined;
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "%peek-char expects a port as argument.");
      }
    }
  else
    {
    ra->type = schemerlicht_object_type_undefined;
    schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "%peek-char expects a port as argument.");
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_input_port(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_INPUT_PORT);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_false;
  if (b > 0)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_port)
      {
      schemerlicht_object* is_input = schemerlicht_vector_at(&v->value.v, 0, schemerlicht_object);
      if (is_input->type == schemerlicht_object_type_true)
        ret.type = schemerlicht_object_type_true;
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_output_port(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_OUTPUT_PORT);
  schemerlicht_object ret;
  ret.type = schemerlicht_object_type_false;
  if (b > 0)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_port)
      {
      schemerlicht_object* is_input = schemerlicht_vector_at(&v->value.v, 0, schemerlicht_object);
      if (is_input->type != schemerlicht_object_type_true)
        ret.type = schemerlicht_object_type_true;
      }
    }
  schemerlicht_set_object(ra, &ret);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_open_input_file(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_OPEN_INPUT_FILE);
  if (b < 1)
    {
    ra->type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* fn = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (fn->type != schemerlicht_object_type_string)
      {
      ra->type = schemerlicht_object_type_undefined;
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "open-input-file expects a string as argument.");
      }
    else
      {
      schemerlicht_object v = make_schemerlicht_object_port(ctxt);
      schemerlicht_vector_at(&v.value.v, 0, schemerlicht_object)->type = schemerlicht_object_type_true;
      schemerlicht_object* filename = schemerlicht_vector_at(&v.value.v, 1, schemerlicht_object);
      schemerlicht_set_object(filename, fn);
      schemerlicht_object* id = schemerlicht_vector_at(&v.value.v, 2, schemerlicht_object);
      id->type = schemerlicht_object_type_fixnum;
      id->value.fx = cast(schemerlicht_fixnum, schemerlicht_open_input_file(fn->value.s.string_ptr));

      schemerlicht_object* str = schemerlicht_vector_at(&v.value.v, 3, schemerlicht_object);
      str->type = schemerlicht_object_type_string;
      schemerlicht_string_init_with_size(ctxt, &str->value.s, 4096, 0);
      schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
      schemerlicht_set_object(heap_obj, str);
      ++ctxt->heap_pos;

      schemerlicht_object* pos = schemerlicht_vector_at(&v.value.v, 4, schemerlicht_object);
      pos->type = schemerlicht_object_type_fixnum;
      pos->value.fx = 4096;

      schemerlicht_object* sz = schemerlicht_vector_at(&v.value.v, 5, schemerlicht_object);
      sz->type = schemerlicht_object_type_fixnum;
      sz->value.fx = 4096;

      schemerlicht_object* bytes_read = schemerlicht_vector_at(&v.value.v, 6, schemerlicht_object);
      bytes_read->type = schemerlicht_object_type_fixnum;
      bytes_read->value.fx = 4096;

      heap_obj = &ctxt->heap[ctxt->heap_pos];
      schemerlicht_set_object(heap_obj, &v);
      ++ctxt->heap_pos;
      schemerlicht_set_object(ra, &v);
      }
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_open_output_file(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_OPEN_OUTPUT_FILE);
  if (b < 1)
    {
    ra->type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* fn = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (fn->type != schemerlicht_object_type_string)
      {
      ra->type = schemerlicht_object_type_undefined;
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "open-input-file expects a string as argument.");
      }
    else
      {
      schemerlicht_object v = make_schemerlicht_object_port(ctxt);
      schemerlicht_vector_at(&v.value.v, 0, schemerlicht_object)->type = schemerlicht_object_type_false;
      schemerlicht_object* filename = schemerlicht_vector_at(&v.value.v, 1, schemerlicht_object);
      schemerlicht_set_object(filename, fn);
      schemerlicht_object* id = schemerlicht_vector_at(&v.value.v, 2, schemerlicht_object);
      id->type = schemerlicht_object_type_fixnum;
      id->value.fx = cast(schemerlicht_fixnum, schemerlicht_open_output_file(fn->value.s.string_ptr));

      schemerlicht_object* str = schemerlicht_vector_at(&v.value.v, 3, schemerlicht_object);
      str->type = schemerlicht_object_type_string;
      schemerlicht_string_init_with_size(ctxt, &str->value.s, 4096, 0);
      schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
      schemerlicht_set_object(heap_obj, str);
      ++ctxt->heap_pos;

      schemerlicht_object* pos = schemerlicht_vector_at(&v.value.v, 4, schemerlicht_object);
      pos->type = schemerlicht_object_type_fixnum;
      pos->value.fx = 0;

      schemerlicht_object* sz = schemerlicht_vector_at(&v.value.v, 5, schemerlicht_object);
      sz->type = schemerlicht_object_type_fixnum;
      sz->value.fx = 4096;

      heap_obj = &ctxt->heap[ctxt->heap_pos];
      schemerlicht_set_object(heap_obj, &v);
      ++ctxt->heap_pos;
      schemerlicht_set_object(ra, &v);
      }
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_close_input_port(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CLOSE_INPUT_PORT);
  if (b < 1)
    {
    ra->type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* p = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (p->type == schemerlicht_object_type_port && schemerlicht_vector_at(&p->value.v, 0, schemerlicht_object)->type == schemerlicht_object_type_true)
      {
      ra->type = schemerlicht_object_type_void;
      schemerlicht_object* id = schemerlicht_vector_at(&p->value.v, 2, schemerlicht_object);
      schemerlicht_close_file(id->value.fx);
      }
    else
      {
      ra->type = schemerlicht_object_type_undefined;
      }
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_close_output_port(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CLOSE_OUTPUT_PORT);
  if (b < 1)
    {
    ra->type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* p = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (p->type == schemerlicht_object_type_port && schemerlicht_vector_at(&p->value.v, 0, schemerlicht_object)->type != schemerlicht_object_type_true)
      {
      ra->type = schemerlicht_object_type_void;
      schemerlicht_object* id = schemerlicht_vector_at(&p->value.v, 2, schemerlicht_object);
      schemerlicht_close_file(id->value.fx);
      }
    else
      {
      ra->type = schemerlicht_object_type_undefined;
      }
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_eof(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_EOF);
  schemerlicht_object ret;
  if (b > 0)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_eof)
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

void schemerlicht_primitive_is_char_ready(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_CHAR_READY);
  if (b > 0)
    {
    schemerlicht_object* p = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (p->type == schemerlicht_object_type_port && schemerlicht_vector_at(&p->value.v, 0, schemerlicht_object)->type == schemerlicht_object_type_true)
      {
      schemerlicht_fixnum current_pos = schemerlicht_vector_at(&p->value.v, 4, schemerlicht_object)->value.fx;
      //schemerlicht_fixnum buffer_length = schemerlicht_vector_at(&p->value.v, 5, schemerlicht_object)->value.fx;
      schemerlicht_fixnum bytes_read = schemerlicht_vector_at(&p->value.v, 6, schemerlicht_object)->value.fx;
      if (bytes_read == 0) // eof
        ra->type = schemerlicht_object_type_true;
      else
        {
        if (current_pos >= bytes_read)
          ra->type = schemerlicht_object_type_false;
        else
          ra->type = schemerlicht_object_type_true;
        }
      }
    else
      {
      ra->type = schemerlicht_object_type_undefined;
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "%char-ready expects an input port as argument.");
      }
    }
  else
    {
    ra->type = schemerlicht_object_type_undefined;
    schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "%char-ready expects an input port as argument.");
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_write(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_WRITE);
  if (b < 2)
    {
    schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "%write expects an object and an output port as argument.");
    ra->type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* obj = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* p = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    if (p->type == schemerlicht_object_type_port && schemerlicht_vector_at(&p->value.v, 0, schemerlicht_object)->type != schemerlicht_object_type_true)
      {
      ra->type = schemerlicht_object_type_void;
      schemerlicht_string str = schemerlicht_object_to_string(ctxt, obj, 0);
      int fileid = cast(int, schemerlicht_vector_at(&p->value.v, 2, schemerlicht_object)->value.fx);
      if (fileid == SCHEMERLICHT_STRING_PORT_ID)
        {
        schemerlicht_fixnum required_length = schemerlicht_vector_at(&p->value.v, 4, schemerlicht_object)->value.fx + str.string_length;
        schemerlicht_fixnum available_length = schemerlicht_vector_at(&p->value.v, 5, schemerlicht_object)->value.fx;
        if (required_length > available_length)
          {
          schemerlicht_object* s = schemerlicht_vector_at(&p->value.v, 3, schemerlicht_object);
          schemerlicht_string new_string;
          schemerlicht_memsize size_init = 256;
          if (required_length - available_length > size_init)
            size_init = required_length - available_length;
          schemerlicht_string_init_with_size(ctxt, &new_string, size_init + s->value.s.string_length, 0);
          s->value.s = new_string; // old string is on the heap and will be cleaned up by gc
          schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
          schemerlicht_set_object(heap_obj, s);
          ++ctxt->heap_pos;
          schemerlicht_vector_at(&p->value.v, 5, schemerlicht_object)->value.fx = new_string.string_length;
          }
        }
      schemerlicht_fixnum current_pos = schemerlicht_vector_at(&p->value.v, 4, schemerlicht_object)->value.fx;
      schemerlicht_fixnum buffer_length = schemerlicht_vector_at(&p->value.v, 5, schemerlicht_object)->value.fx;
      schemerlicht_object* buffer = schemerlicht_vector_at(&p->value.v, 3, schemerlicht_object);
      char* chars_to_write = str.string_ptr;
      char* chars_to_write_end = str.string_ptr + str.string_length;

      while (chars_to_write < chars_to_write_end)
        {
        intptr_t dist = chars_to_write_end - chars_to_write;
        schemerlicht_fixnum buffer_size_remaining = buffer_length - current_pos;
        if (buffer_size_remaining < dist)
          {
          memcpy(buffer->value.s.string_ptr + current_pos, chars_to_write, buffer_size_remaining);
          schemerlicht_write(fileid, buffer->value.s.string_ptr, buffer_length);
          current_pos = 0;
          chars_to_write += buffer_size_remaining;
          }
        else
          {
          memcpy(buffer->value.s.string_ptr + current_pos, chars_to_write, dist);
          current_pos += dist;
          chars_to_write += dist;
          schemerlicht_vector_at(&p->value.v, 4, schemerlicht_object)->value.fx = current_pos;
          }
        }
      schemerlicht_string_destroy(ctxt, &str);
      }
    else
      {
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "%write expects an object and an output port as argument.");
      ra->type = schemerlicht_object_type_undefined;
      }
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_display(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_DISPLAY);
  if (b < 2)
    {
    schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "%display expects an object and an output port as argument.");
    ra->type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* obj = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* p = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    if (p->type == schemerlicht_object_type_port && schemerlicht_vector_at(&p->value.v, 0, schemerlicht_object)->type != schemerlicht_object_type_true)
      {
      ra->type = schemerlicht_object_type_void;
      schemerlicht_string str = schemerlicht_object_to_string(ctxt, obj, 1);
      int fileid = cast(int, schemerlicht_vector_at(&p->value.v, 2, schemerlicht_object)->value.fx);
      if (fileid == SCHEMERLICHT_STRING_PORT_ID)
        {
        schemerlicht_fixnum required_length = schemerlicht_vector_at(&p->value.v, 4, schemerlicht_object)->value.fx + str.string_length;
        schemerlicht_fixnum available_length = schemerlicht_vector_at(&p->value.v, 5, schemerlicht_object)->value.fx;
        if (required_length > available_length)
          {
          schemerlicht_object* s = schemerlicht_vector_at(&p->value.v, 3, schemerlicht_object);
          schemerlicht_string new_string;
          schemerlicht_memsize size_init = 256;
          if (required_length - available_length > size_init)
            size_init = required_length - available_length;
          schemerlicht_string_init_with_size(ctxt, &new_string, size_init + s->value.s.string_length, 0);
          s->value.s = new_string; // old string is on the heap and will be cleaned up by gc
          schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
          schemerlicht_set_object(heap_obj, s);
          ++ctxt->heap_pos;
          schemerlicht_vector_at(&p->value.v, 5, schemerlicht_object)->value.fx = new_string.string_length;
          }
        }
      schemerlicht_fixnum current_pos = schemerlicht_vector_at(&p->value.v, 4, schemerlicht_object)->value.fx;
      schemerlicht_fixnum buffer_length = schemerlicht_vector_at(&p->value.v, 5, schemerlicht_object)->value.fx;
      schemerlicht_object* buffer = schemerlicht_vector_at(&p->value.v, 3, schemerlicht_object);
      char* chars_to_write = str.string_ptr;
      char* chars_to_write_end = str.string_ptr + str.string_length;

      while (chars_to_write < chars_to_write_end)
        {
        intptr_t dist = chars_to_write_end - chars_to_write;
        schemerlicht_fixnum buffer_size_remaining = buffer_length - current_pos;
        if (buffer_size_remaining < dist)
          {
          memcpy(buffer->value.s.string_ptr + current_pos, chars_to_write, buffer_size_remaining);
          schemerlicht_write(fileid, buffer->value.s.string_ptr, buffer_length);
          current_pos = 0;
          chars_to_write += buffer_size_remaining;
          }
        else
          {
          memcpy(buffer->value.s.string_ptr + current_pos, chars_to_write, dist);
          current_pos += dist;
          chars_to_write += dist;
          schemerlicht_vector_at(&p->value.v, 4, schemerlicht_object)->value.fx = current_pos;
          }
        }
      schemerlicht_string_destroy(ctxt, &str);
      }
    else
      {
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "%display expects an object and an output port as argument.");
      ra->type = schemerlicht_object_type_undefined;
      }
    }
  }

////////////////////////////////////////////////////

static int _port_get_char(char* ch, void* p, int read)
  {
  schemerlicht_object* prt = cast(schemerlicht_object*, p);
  schemerlicht_byte b;
  int result = get_char(&b, prt, read);
  *ch = cast(char, b);
  return result;
  }

static void _port_next_char(void* p)
  {
  schemerlicht_object* prt = cast(schemerlicht_object*, p);
  schemerlicht_assert(prt->type == schemerlicht_object_type_port);
  schemerlicht_assert(schemerlicht_vector_begin(&prt->value.v, schemerlicht_object)->type == schemerlicht_object_type_true);
  schemerlicht_vector_at(&prt->value.v, 4, schemerlicht_object)->value.fx += 1;
  }

static schemerlicht_fixnum _port_get_position(void* p)
  {
  schemerlicht_object* prt = cast(schemerlicht_object*, p);
  schemerlicht_fixnum position = schemerlicht_vector_at(&prt->value.v, 4, schemerlicht_object)->value.fx;
  int fileid = cast(int, schemerlicht_vector_at(&prt->value.v, 2, schemerlicht_object)->value.fx);
  position += cast(schemerlicht_fixnum, schemerlicht_tell(fileid));
  return position;
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_read(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_READ);
  if (b < 1)
    {
    schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "%read expects an input port as argument.");
    ra->type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* p = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (p->type == schemerlicht_object_type_port && schemerlicht_vector_at(&p->value.v, 0, schemerlicht_object)->type == schemerlicht_object_type_true)
      {
      schemerlicht_string buff;
      schemerlicht_string_init(ctxt, &buff, "");
      schemerlicht_cell cell = schemerlicht_read_datum(ctxt, NULL, &buff, cast(void*, p), &_port_get_char, &_port_next_char, &_port_get_position);
#if 0
      schemerlicht_dump_cell_to_string(ctxt, &cell, &buff);
      printf("%s\n", buff.string_ptr);
#endif
      if (cell.type == schemerlicht_ct_invalid_cell)
        ra->type = schemerlicht_object_type_eof;
      else
        {
        schemerlicht_object obj = schemerlicht_cell_to_object(ctxt, &cell);
        schemerlicht_set_object(ra, &obj);
        }
      schemerlicht_string_destroy(ctxt, &buff);
      schemerlicht_destroy_cell(ctxt, &cell);
      }
    else
      {
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_RUNERROR, -1, -1, "%read expects an input port as argument.");
      ra->type = schemerlicht_object_type_undefined;
      }
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_load(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_LOAD);
  if (b == 0)
    {
    ra->type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* fn = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (fn->type == schemerlicht_object_type_string)
      {
      FILE* f = fopen(fn->value.s.string_ptr, "r");
      if (f)
        {
        schemerlicht_stream str;
        schemerlicht_stream_init(ctxt, &str, 256);
        char buffer[256];
        size_t bytes_read = fread(buffer, 1, 256, f);
        while (bytes_read)
          {
          schemerlicht_stream_write(ctxt, &str, buffer, bytes_read, 0);
          bytes_read = fread(buffer, 1, 256, f);
          }
        fclose(f);
        schemerlicht_stream_rewind(&str);
        schemerlicht_vector tokens = tokenize(ctxt, &str);
        schemerlicht_stream_close(ctxt, &str);
        //make new stack so that old stack is not overwritten.
        //new stack needs to be created before we run any macros.
        schemerlicht_vector stack_store = ctxt->stack;
        schemerlicht_vector new_stack;
        schemerlicht_vector_init_with_size(ctxt, &new_stack, schemerlicht_maxstack, schemerlicht_object);
        ctxt->stack = new_stack;

        schemerlicht_program prog = make_program(ctxt, &tokens);
        schemerlicht_preprocess(ctxt, &prog);
        if (prog.expressions.vector_size > 0)
          {
          schemerlicht_expression* expr = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression);
          schemerlicht_function* func = schemerlicht_compile_expression(ctxt, expr);
          destroy_tokens_vector(ctxt, &tokens);
          schemerlicht_program_destroy(ctxt, &prog);
          schemerlicht_object* res = schemerlicht_run(ctxt, func);
          schemerlicht_set_object(ra, res);
          //ra->type = schemerlicht_object_type_undefined;          
          schemerlicht_vector_push_back(ctxt, &ctxt->lambdas, func, schemerlicht_function*);
          }
        else
          {
          destroy_tokens_vector(ctxt, &tokens);
          schemerlicht_program_destroy(ctxt, &prog);
          ra->type = schemerlicht_object_type_undefined;
          }
        //restore old stack
        schemerlicht_vector_destroy(ctxt, &new_stack);
        ctxt->stack = stack_store;
        }
      else
        ra->type = schemerlicht_object_type_undefined;
      }
    else
      {
      ra->type = schemerlicht_object_type_undefined;
      }
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_eval(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_EVAL);
  if (b == 0)
    {
    ra->type = schemerlicht_object_type_undefined;
    }
  else
    {
    schemerlicht_object* q = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_string s = schemerlicht_object_to_string(ctxt, q, 0);
    schemerlicht_context* eval_ctxt = ctxt;
    if (b > 1)
      {
      schemerlicht_object* e = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
      if (e->type == schemerlicht_object_type_environment)
        {
        eval_ctxt = cast(schemerlicht_context*, e->value.ptr);
        }
      }
    schemerlicht_stream str;
    schemerlicht_memsize len = s.string_length;
    schemerlicht_stream_init(eval_ctxt, &str, len);
    schemerlicht_stream_write(eval_ctxt, &str, s.string_ptr, len, 0);
    schemerlicht_stream_rewind(&str);
    schemerlicht_vector tokens = tokenize(eval_ctxt, &str);
    schemerlicht_stream_close(eval_ctxt, &str);
    schemerlicht_program prog = make_program(eval_ctxt, &tokens);
    schemerlicht_preprocess(eval_ctxt, &prog);
    schemerlicht_expression* expr = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression);
    schemerlicht_function* func = schemerlicht_compile_expression(eval_ctxt, expr);
    schemerlicht_object* res = schemerlicht_run(eval_ctxt, func);
    schemerlicht_object res_copy = schemerlicht_object_deep_copy(ctxt, res);
    schemerlicht_function_free(eval_ctxt, func);
    destroy_tokens_vector(eval_ctxt, &tokens);
    schemerlicht_program_destroy(eval_ctxt, &prog);
    schemerlicht_string_destroy(ctxt, &s);
    schemerlicht_set_object(ra, &res_copy);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_putenv(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_PUTENV);
  if (b < 2)
    {
    ra->type = schemerlicht_object_type_false;
    }
  else
    {
    schemerlicht_object* s1 = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    schemerlicht_object* s2 = schemerlicht_vector_at(&ctxt->stack, a + 2 + c, schemerlicht_object);
    if (s1->type == schemerlicht_object_type_string && s2->type == schemerlicht_object_type_string)
      {
      int res = schemerlicht_putenv(s1->value.s.string_ptr, s2->value.s.string_ptr);
      if (res == 0)
        ra->type = schemerlicht_object_type_true;
      else
        ra->type = schemerlicht_object_type_false;
      }
    else
      {
      ra->type = schemerlicht_object_type_false;
      }
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_getenv(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_GETENV);
  if (b == 0)
    {
    ra->type = schemerlicht_object_type_false;
    }
  else
    {
    schemerlicht_object* str = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (str->type == schemerlicht_object_type_string)
      {
      const char* env = schemerlicht_getenv(str->value.s.string_ptr);
      if (env)
        {
        schemerlicht_object s = make_schemerlicht_object_string(ctxt, env);
        schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
        schemerlicht_set_object(heap_obj, &s);
        ++ctxt->heap_pos;
        schemerlicht_set_object(ra, &s);
        }
      else
        {
        ra->type = schemerlicht_object_type_false;
        }
      }
    else
      {
      ra->type = schemerlicht_object_type_false;
      }
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_file_exists(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FILE_EXISTS);
  if (b == 0)
    {
    ra->type = schemerlicht_object_type_false;
    }
  else
    {
    schemerlicht_object* str = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (str->type == schemerlicht_object_type_string)
      {
      int exists = schemerlicht_file_exists(str->value.s.string_ptr);
      if (exists)
        {
        ra->type = schemerlicht_object_type_true;
        }
      else
        {
        ra->type = schemerlicht_object_type_false;
        }
      }
    else
      {
      ra->type = schemerlicht_object_type_false;
      }
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_interaction_environment(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_INTERACTION_ENVIRONMENT);
  ra->type = schemerlicht_object_type_environment;
  ra->value.ptr = cast(void*, ctxt);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_is_environment(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_ENVIRONMENT);
  if (b > 0)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_environment)
      ra->type = schemerlicht_object_type_true;
    else
      ra->type = schemerlicht_object_type_false;
    }
  else
    {
    ra->type = schemerlicht_object_type_false;
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_null_environment(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_NULL_ENVIRONMENT);
  schemerlicht_memsize heapsize = ctxt->raw_heap.vector_size;
  if (b > 0)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_fixnum)
      {
      heapsize = cast(schemerlicht_memsize, v->value.fx);
      }
    else if (v->type == schemerlicht_object_type_flonum)
      {
      heapsize = cast(schemerlicht_memsize, v->value.fl);
      }
    }
  ra->type = schemerlicht_object_type_environment;
  schemerlicht_context* new_ctxt = schemerlicht_context_init(ctxt, heapsize);
  schemerlicht_vector_push_back(ctxt, &ctxt->environments, new_ctxt, schemerlicht_context*);
  ra->value.ptr = cast(void*, new_ctxt);
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_scheme_environment(schemerlicht_context* ctxt, int a, int b, int c)
  {
  // R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_primitive || ra->type == schemerlicht_object_type_primitive_object);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_SCHEME_ENVIRONMENT);
  schemerlicht_memsize heapsize = ctxt->raw_heap.vector_size;
  if (b > 0)
    {
    schemerlicht_object* v = schemerlicht_vector_at(&ctxt->stack, a + 1 + c, schemerlicht_object);
    if (v->type == schemerlicht_object_type_fixnum)
      {
      heapsize = cast(schemerlicht_memsize, v->value.fx);
      }
    else if (v->type == schemerlicht_object_type_flonum)
      {
      heapsize = cast(schemerlicht_memsize, v->value.fl);
      }
    }
  ra->type = schemerlicht_object_type_environment;
  schemerlicht_context* new_ctxt = schemerlicht_context_init(ctxt, heapsize);
  schemerlicht_compile_callcc(new_ctxt);
  schemerlicht_compile_r5rs(new_ctxt);
  schemerlicht_compile_input_output(new_ctxt);
  schemerlicht_compile_modules(new_ctxt, ctxt->module_path.string_ptr);
  schemerlicht_vector_push_back(ctxt, &ctxt->environments, new_ctxt, schemerlicht_context*);
  ra->value.ptr = cast(void*, new_ctxt);
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
    case SCHEMERLICHT_CHARCIEQUAL:
      schemerlicht_primitive_charciequal(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CHARCILESS:
      schemerlicht_primitive_charciless(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CHARCILEQ:
      schemerlicht_primitive_charcileq(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CHARCIGREATER:
      schemerlicht_primitive_charcigreater(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CHARCIGEQ:
      schemerlicht_primitive_charcigeq(ctxt, a, b, c);
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
    case SCHEMERLICHT_RECLAIM_GARBAGE:
      schemerlicht_primitive_reclaim_garbage(ctxt, a, b, c);
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
    case SCHEMERLICHT_NUMBER_STRING:
      schemerlicht_primitive_number_string(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_NUMBER:
      schemerlicht_primitive_string_number(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_NAN:
      schemerlicht_primitive_is_nan(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_INF:
      schemerlicht_primitive_is_inf(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_FINITE:
      schemerlicht_primitive_is_finite(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_LIST_REF:
      schemerlicht_primitive_list_ref(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_LIST_TAIL:
      schemerlicht_primitive_list_tail(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_REVERSE:
      schemerlicht_primitive_reverse(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CHAR_UPCASE:
      schemerlicht_primitive_char_upcase(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CHAR_DOWNCASE:
      schemerlicht_primitive_char_downcase(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CHAR_IS_UPPER:
      schemerlicht_primitive_char_is_upper(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CHAR_IS_LOWER:
      schemerlicht_primitive_char_is_lower(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CHAR_IS_ALPHA:
      schemerlicht_primitive_char_is_alpha(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CHAR_IS_NUMERIC:
      schemerlicht_primitive_char_is_numeric(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CHAR_IS_WHITESPACE:
      schemerlicht_primitive_char_is_whitespace(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_STRING_LIST:
      schemerlicht_primitive_string_list(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_LIST_STRING:
      schemerlicht_primitive_list_string(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_VECTOR_LIST:
      schemerlicht_primitive_vector_list(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_LIST_VECTOR:
      schemerlicht_primitive_list_vector(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_MAKE_PROMISE:
      schemerlicht_primitive_make_promise(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_PROMISE:
      schemerlicht_primitive_is_promise(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_SLOT_REF:
      schemerlicht_primitive_slot_ref(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_SLOT_SET:
      schemerlicht_primitive_slot_set(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_MAKE_PORT:
      schemerlicht_primitive_make_port(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_PORT:
      schemerlicht_primitive_is_port(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_WRITE_CHAR:
      schemerlicht_primitive_write_char(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_READ_CHAR:
      schemerlicht_primitive_read_char(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_PEEK_CHAR:
      schemerlicht_primitive_peek_char(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_INPUT_PORT:
      schemerlicht_primitive_is_input_port(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_OUTPUT_PORT:
      schemerlicht_primitive_is_output_port(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_OPEN_INPUT_FILE:
      schemerlicht_primitive_open_input_file(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_OPEN_OUTPUT_FILE:
      schemerlicht_primitive_open_output_file(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CLOSE_INPUT_PORT:
      schemerlicht_primitive_close_input_port(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_CLOSE_OUTPUT_PORT:
      schemerlicht_primitive_close_output_port(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FLUSH_OUTPUT_PORT:
      schemerlicht_primitive_flush_output_port(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_EOF:
      schemerlicht_primitive_is_eof(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_CHAR_READY:
      schemerlicht_primitive_is_char_ready(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_WRITE:
      schemerlicht_primitive_write(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_DISPLAY:
      schemerlicht_primitive_display(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_READ:
      schemerlicht_primitive_read(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_LOAD:
      schemerlicht_primitive_load(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_EVAL:
      schemerlicht_primitive_eval(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_GETENV:
      schemerlicht_primitive_getenv(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_PUTENV:
      schemerlicht_primitive_putenv(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_FILE_EXISTS:
      schemerlicht_primitive_file_exists(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_INTERACTION_ENVIRONMENT:
      schemerlicht_primitive_interaction_environment(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_IS_ENVIRONMENT:
      schemerlicht_primitive_is_environment(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_NULL_ENVIRONMENT:
      schemerlicht_primitive_null_environment(ctxt, a, b, c);
      break;
    case SCHEMERLICHT_SCHEME_ENVIRONMENT:
      schemerlicht_primitive_scheme_environment(ctxt, a, b, c);
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
  map_insert(ctxt, m, "char-ci=?", SCHEMERLICHT_CHARCIEQUAL);
  map_insert(ctxt, m, "char-ci<?", SCHEMERLICHT_CHARCILESS);
  map_insert(ctxt, m, "char-ci<=?", SCHEMERLICHT_CHARCILEQ);
  map_insert(ctxt, m, "char-ci>?", SCHEMERLICHT_CHARCIGREATER);
  map_insert(ctxt, m, "char-ci>=?", SCHEMERLICHT_CHARCIGEQ);
  map_insert(ctxt, m, "fixnum->char", SCHEMERLICHT_FIXNUM_CHAR);
  map_insert(ctxt, m, "integer->char", SCHEMERLICHT_FIXNUM_CHAR);
  map_insert(ctxt, m, "char->fixnum", SCHEMERLICHT_CHAR_FIXNUM);
  map_insert(ctxt, m, "char->integer", SCHEMERLICHT_CHAR_FIXNUM);
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
  map_insert(ctxt, m, "reclaim-garbage", SCHEMERLICHT_RECLAIM_GARBAGE);
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
  map_insert(ctxt, m, "exact->inexact", SCHEMERLICHT_FIXNUM_FLONUM);
  map_insert(ctxt, m, "inexact->exact", SCHEMERLICHT_FLONUM_FIXNUM);
  map_insert(ctxt, m, "number->string", SCHEMERLICHT_NUMBER_STRING);
  map_insert(ctxt, m, "string->number", SCHEMERLICHT_STRING_NUMBER);
  map_insert(ctxt, m, "nan?", SCHEMERLICHT_IS_NAN);
  map_insert(ctxt, m, "inf?", SCHEMERLICHT_IS_INF);
  map_insert(ctxt, m, "finite?", SCHEMERLICHT_IS_FINITE);
  map_insert(ctxt, m, "list-ref", SCHEMERLICHT_LIST_REF);
  map_insert(ctxt, m, "list-tail", SCHEMERLICHT_LIST_TAIL);
  map_insert(ctxt, m, "reverse", SCHEMERLICHT_REVERSE);
  map_insert(ctxt, m, "char-upcase", SCHEMERLICHT_CHAR_UPCASE);
  map_insert(ctxt, m, "char-downcase", SCHEMERLICHT_CHAR_DOWNCASE);
  map_insert(ctxt, m, "char-upper-case?", SCHEMERLICHT_CHAR_IS_UPPER);
  map_insert(ctxt, m, "char-lower-case?", SCHEMERLICHT_CHAR_IS_LOWER);
  map_insert(ctxt, m, "char-alphabetic?", SCHEMERLICHT_CHAR_IS_ALPHA);
  map_insert(ctxt, m, "char-numeric?", SCHEMERLICHT_CHAR_IS_NUMERIC);
  map_insert(ctxt, m, "char-whitespace?", SCHEMERLICHT_CHAR_IS_WHITESPACE);
  map_insert(ctxt, m, "string->list", SCHEMERLICHT_STRING_LIST);
  map_insert(ctxt, m, "list->string", SCHEMERLICHT_LIST_STRING);
  map_insert(ctxt, m, "vector->list", SCHEMERLICHT_VECTOR_LIST);
  map_insert(ctxt, m, "list->vector", SCHEMERLICHT_LIST_VECTOR);
  map_insert(ctxt, m, "%make-promise", SCHEMERLICHT_MAKE_PROMISE);
  map_insert(ctxt, m, "promise?", SCHEMERLICHT_IS_PROMISE);
  map_insert(ctxt, m, "%slot-ref", SCHEMERLICHT_SLOT_REF);
  map_insert(ctxt, m, "%slot-set!", SCHEMERLICHT_SLOT_SET);
  map_insert(ctxt, m, "%make-port", SCHEMERLICHT_MAKE_PORT);
  map_insert(ctxt, m, "port?", SCHEMERLICHT_IS_PORT);
  map_insert(ctxt, m, "%write-char", SCHEMERLICHT_WRITE_CHAR);
  map_insert(ctxt, m, "%read-char", SCHEMERLICHT_READ_CHAR);
  map_insert(ctxt, m, "%peek-char", SCHEMERLICHT_PEEK_CHAR);
  map_insert(ctxt, m, "input-port?", SCHEMERLICHT_IS_INPUT_PORT);
  map_insert(ctxt, m, "output-port?", SCHEMERLICHT_IS_OUTPUT_PORT);
  map_insert(ctxt, m, "open-input-file", SCHEMERLICHT_OPEN_INPUT_FILE);
  map_insert(ctxt, m, "open-output-file", SCHEMERLICHT_OPEN_OUTPUT_FILE);
  map_insert(ctxt, m, "close-input-port", SCHEMERLICHT_CLOSE_INPUT_PORT);
  map_insert(ctxt, m, "close-output-port", SCHEMERLICHT_CLOSE_OUTPUT_PORT);
  map_insert(ctxt, m, "%flush-output-port", SCHEMERLICHT_FLUSH_OUTPUT_PORT);
  map_insert(ctxt, m, "eof-object?", SCHEMERLICHT_IS_EOF);
  map_insert(ctxt, m, "%char-ready?", SCHEMERLICHT_IS_CHAR_READY);
  map_insert(ctxt, m, "%write", SCHEMERLICHT_WRITE);
  map_insert(ctxt, m, "%display", SCHEMERLICHT_DISPLAY);
  map_insert(ctxt, m, "%read", SCHEMERLICHT_READ);
  map_insert(ctxt, m, "load", SCHEMERLICHT_LOAD);
  map_insert(ctxt, m, "eval", SCHEMERLICHT_EVAL);
  map_insert(ctxt, m, "getenv", SCHEMERLICHT_GETENV);
  map_insert(ctxt, m, "putenv", SCHEMERLICHT_PUTENV);
  map_insert(ctxt, m, "file-exists?", SCHEMERLICHT_FILE_EXISTS);
  map_insert(ctxt, m, "interaction-environment", SCHEMERLICHT_INTERACTION_ENVIRONMENT);
  map_insert(ctxt, m, "environment?", SCHEMERLICHT_IS_ENVIRONMENT);
  map_insert(ctxt, m, "null-environment", SCHEMERLICHT_NULL_ENVIRONMENT);
  map_insert(ctxt, m, "scheme-environment", SCHEMERLICHT_SCHEME_ENVIRONMENT);
  return m;
  }
