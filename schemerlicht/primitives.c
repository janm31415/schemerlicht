#include "primitives.h"
#include "context.h"
#include "object.h"
#include "error.h"

void schemerlicht_primitive_add1(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);

  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
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
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);

  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXADD1);

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
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_fixnum;
      ret.value.fx = arg->value.fx + 1;
      }
    else
      ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fladd1(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);

  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLADD1);

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
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_flonum)
      {
      ret.type = schemerlicht_object_type_flonum;
      ret.value.fl = arg->value.fl + 1;
      }
    else
      ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_sub1(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);

  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
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
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);

  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXSUB1);

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
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_fixnum;
      ret.value.fx = arg->value.fx - 1;
      }
    else
      ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flsub1(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);

  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLSUB1);

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
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
    if (arg->type == schemerlicht_object_type_flonum)
      {
      ret.type = schemerlicht_object_type_flonum;
      ret.value.fl = arg->value.fl - 1;
      }
    else
      ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_add(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
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
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXADD);
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
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
      if (arg->type != schemerlicht_object_type_fixnum)
        {
        ret.type = schemerlicht_object_type_undefined;
        break;
        }
      ret.value.fx += arg->value.fx;
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fladd(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLADD);
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
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
      if (arg->type != schemerlicht_object_type_flonum)
        {
        ret.type = schemerlicht_object_type_undefined;
        break;
        }
      ret.value.fl += arg->value.fl;
      }
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_sub(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
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
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
      schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
          break;
        }
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
      schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
          break;
        }
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
      schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
      ret.value.ch = first_arg->value.ch;
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXSUB);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    ret.value.fx = 0;
    schemerlicht_set_object(ra, &ret);
    }
  else if (b == 1)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    ret.value.fx = first_arg->value.fx;
    for (int j = 1; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flsub(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLSUB);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_flonum;
    ret.value.fl = 0;
    schemerlicht_set_object(ra, &ret);
    }
  else if (b == 1)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_flonum)
      {
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    ret.value.fl = first_arg->value.fl;
    for (int j = 1; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_mul(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
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
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
      schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
          break;
        }
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
      schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
          break;
        }
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
            break;
          }
        }
      break;
      }
      case schemerlicht_object_type_char:
      {
      schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
      ret.value.ch = first_arg->value.ch;
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXMUL);
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
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    ret.value.fx = first_arg->value.fx;
    for (int j = 1; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flmul(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLMUL);
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
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_flonum)
      {
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    ret.value.fl = first_arg->value.fl;
    for (int j = 1; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_div(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
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
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
      schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
          break;
        }
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
      schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
          break;
        }
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
      schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
      ret.value.ch = first_arg->value.ch;
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXDIV);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_fixnum;
    ret.value.fx = 1;
    schemerlicht_set_object(ra, &ret);
    }
  else if (b == 1)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_fixnum)
      {
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    ret.value.fx = first_arg->value.fx;
    for (int j = 1; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_fldiv(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLDIV);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_flonum;
    ret.value.fl = 1;
    schemerlicht_set_object(ra, &ret);
    }
  else if (b == 1)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
    if (first_arg->type != schemerlicht_object_type_flonum)
      {
      ret.type = schemerlicht_object_type_undefined;
      schemerlicht_set_object(ra, &ret);
      return;
      }
    ret.value.fl = first_arg->value.fl;
    for (int j = 1; j < b; ++j)
      {
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_equal(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  //schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  //schemerlicht_assert(ra->value.fx == SCHEMERLICHT_EQUAL);
  if (b > 1)
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
    switch (first_arg->type)
      {
      case schemerlicht_object_type_fixnum:
      {
      for (int j = 1; j < b; ++j)
        {
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
        schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXEQUAL);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_true;
    schemerlicht_set_object(ra, &ret);
    return;
    }
  schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
  if (first_arg->type != schemerlicht_object_type_fixnum)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    return;
    }
  for (int j = 1; j < b; ++j)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flequal(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLEQUAL);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_true;
    schemerlicht_set_object(ra, &ret);
    return;
    }
  schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
  if (first_arg->type != schemerlicht_object_type_flonum)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    return;
    }
  for (int j = 1; j < b; ++j)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_charequal(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_CHAREQUAL);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_true;
    schemerlicht_set_object(ra, &ret);
    return;
    }
  schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
  if (first_arg->type != schemerlicht_object_type_char)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    return;
    }
  for (int j = 1; j < b; ++j)
    {
    schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  //schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  //schemerlicht_assert(ra->value.fx == SCHEMERLICHT_LESS);
  for (int j = 0; j < b - 1; ++j)
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  for (int j = 0; j < b - 1; ++j)
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flless(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  for (int j = 0; j < b - 1; ++j)
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_charless(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  for (int j = 0; j < b - 1; ++j)
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
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
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flgeq(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
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
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_chargeq(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  //schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  //schemerlicht_assert(ra->value.fx == SCHEMERLICHT_GREATER);
  for (int j = 0; j < b - 1; ++j)
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  for (int j = 0; j < b - 1; ++j)
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flgreater(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  for (int j = 0; j < b - 1; ++j)
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_chargreater(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  for (int j = 0; j < b - 1; ++j)
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1 + j, schemerlicht_object);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
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
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flleq(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
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
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_charleq(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_FIXNUM);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_FLONUM);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_NULL);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_ZERO);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_true;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FXZERO);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_true;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_false;
    if (first_arg->type == schemerlicht_object_type_fixnum && first_arg->value.fx == 0)
      ret.type = schemerlicht_object_type_true;
    else
      ret.type = schemerlicht_object_type_false;
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////

void schemerlicht_primitive_flzero(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_FLZERO);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_true;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_false;
    if (first_arg->type == schemerlicht_object_type_flonum && first_arg->value.fl == 0)
      ret.type = schemerlicht_object_type_true;
    else
      ret.type = schemerlicht_object_type_false;
    schemerlicht_set_object(ra, &ret);
    }
  }

////////////////////////////////////////////////////
void schemerlicht_primitive_is_boolean(schemerlicht_context* ctxt, int a, int b, int c)
  {
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_BOOLEAN);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_IS_CHAR);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_undefined;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
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
  UNUSED(c);
  // R(A), ... ,R(A+C-1) := R(A)(R(A+1), ... ,R(A+B)) */
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(ra->type == schemerlicht_object_type_fixnum);
  schemerlicht_assert(ra->value.fx == SCHEMERLICHT_NOT);
  if (b == 0)
    {
    schemerlicht_object ret;
    ret.type = schemerlicht_object_type_false;
    schemerlicht_set_object(ra, &ret);
    }
  else
    {
    schemerlicht_object* first_arg = schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
    schemerlicht_object ret;
    if (first_arg->type == schemerlicht_object_type_false)
      ret.type = schemerlicht_object_type_true;
    else
      ret.type = schemerlicht_object_type_false;
    schemerlicht_set_object(ra, &ret);
    }
  }
////////////////////////////////////////////////////

void schemerlicht_call_primitive(schemerlicht_context* ctxt, schemerlicht_fixnum prim_id, int a, int b, int c)
  {
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
    default:
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
      break;
    }
  }

static void map_insert(schemerlicht_context* ctxt, schemerlicht_map* m, const char* str, int value)
  {
  schemerlicht_object* obj = schemerlicht_map_insert_string(ctxt, m, str);
  obj->type = schemerlicht_object_type_fixnum;
  obj->value.fx = (schemerlicht_fixnum)value;
  }

schemerlicht_map* generate_primitives_map(schemerlicht_context* ctxt)
  {
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
  return m;
  }