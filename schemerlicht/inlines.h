#ifndef SCHEMERLICHT_INLINES_H
#define SCHEMERLICHT_INLINES_H

#include "primitives.h"

#define inline_functions2(aa, bb, cc) \
  if ((bb) == 2) \
    { \
    const schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, (aa) + (cc) + 1, schemerlicht_object); \
    switch (schemerlicht_object_get_type(arg1)) \
      { \
      case schemerlicht_object_type_fixnum: \
        { \
        const schemerlicht_object* arg2 = schemerlicht_vector_at(&ctxt->stack, (aa) + (cc) + 2, schemerlicht_object); \
        switch (schemerlicht_object_get_type(arg2)) \
          { \
          case schemerlicht_object_type_fixnum: \
            { \
            switch (function_id) \
              { \
              case SCHEMERLICHT_ADD: \
              case SCHEMERLICHT_FXADD: \
                { \
                target->value.fx = arg1->value.fx + arg2->value.fx; \
                target->type = schemerlicht_object_type_fixnum; \
                break; \
                } \
              case SCHEMERLICHT_SUB: \
              case SCHEMERLICHT_FXSUB: \
                { \
                target->value.fx = arg1->value.fx - arg2->value.fx; \
                target->type = schemerlicht_object_type_fixnum; \
                break; \
                } \
              case SCHEMERLICHT_MUL: \
              case SCHEMERLICHT_FXMUL: \
                { \
                target->value.fx = arg1->value.fx * arg2->value.fx; \
                target->type = schemerlicht_object_type_fixnum; \
                break; \
                } \
              case SCHEMERLICHT_DIV: \
              case SCHEMERLICHT_FXDIV: \
                { \
                if (arg2->value.fx == 0) \
                  target->type = schemerlicht_object_type_undefined; \
                else \
                  { \
                  target->value.fx = arg1->value.fx / arg2->value.fx; \
                  target->type = schemerlicht_object_type_fixnum; \
                  } \
                break; \
                } \
              case SCHEMERLICHT_NOT_EQUAL: \
                { \
                target->type = (arg1->value.fx != arg2->value.fx) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
                break; \
                } \
              case SCHEMERLICHT_EQUAL: \
              case SCHEMERLICHT_FXEQUAL: \
                { \
                target->type = (arg1->value.fx == arg2->value.fx) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
                break; \
                } \
              case SCHEMERLICHT_LESS: \
              case SCHEMERLICHT_FXLESS: \
                { \
                target->type = (arg1->value.fx < arg2->value.fx) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
                break; \
                } \
              case SCHEMERLICHT_GREATER: \
              case SCHEMERLICHT_FXGREATER: \
                { \
                target->type = (arg1->value.fx > arg2->value.fx) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
                break; \
                } \
              case SCHEMERLICHT_LEQ: \
              case SCHEMERLICHT_FXLEQ: \
                { \
                target->type = (arg1->value.fx <= arg2->value.fx) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
                break; \
                } \
              case SCHEMERLICHT_GEQ: \
              case SCHEMERLICHT_FXGEQ: \
                { \
                target->type = (arg1->value.fx >= arg2->value.fx) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
                break; \
                } \
              default: \
                schemerlicht_call_primitive(ctxt, function_id, aa, bb, cc); \
                break; \
              } \
            break; \
            } \
          default: \
            schemerlicht_call_primitive(ctxt, function_id, aa, bb, cc); \
            break; \
          } \
        break; \
        } \
      case schemerlicht_object_type_flonum: \
        { \
        const schemerlicht_object* arg2 = schemerlicht_vector_at(&ctxt->stack, (aa) + (cc) + 2, schemerlicht_object); \
        switch (schemerlicht_object_get_type(arg2)) \
          { \
          case schemerlicht_object_type_flonum: \
            { \
            switch (function_id) \
              { \
              case SCHEMERLICHT_ADD: \
              case SCHEMERLICHT_FLADD: \
                { \
                target->value.fl = arg1->value.fl + arg2->value.fl; \
                target->type = schemerlicht_object_type_flonum; \
                break; \
                } \
              case SCHEMERLICHT_SUB: \
              case SCHEMERLICHT_FLSUB: \
                { \
                target->value.fl = arg1->value.fl - arg2->value.fl; \
                target->type = schemerlicht_object_type_flonum; \
                break; \
                } \
              case SCHEMERLICHT_MUL: \
              case SCHEMERLICHT_FLMUL: \
                { \
                target->value.fl = arg1->value.fl * arg2->value.fl; \
                target->type = schemerlicht_object_type_flonum; \
                break; \
                } \
              case SCHEMERLICHT_DIV: \
              case SCHEMERLICHT_FLDIV: \
                { \
                target->value.fl = arg1->value.fl / arg2->value.fl; \
                target->type = schemerlicht_object_type_flonum; \
                break; \
                } \
              case SCHEMERLICHT_NOT_EQUAL: \
                { \
                target->type = (arg1->value.fl != arg2->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
                break; \
                } \
              case SCHEMERLICHT_EQUAL: \
              case SCHEMERLICHT_FLEQUAL: \
                { \
                target->type = (arg1->value.fl == arg2->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
                break; \
                } \
              case SCHEMERLICHT_LESS: \
              case SCHEMERLICHT_FLLESS: \
                { \
                target->type = (arg1->value.fl < arg2->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
                break; \
                } \
              case SCHEMERLICHT_GREATER: \
              case SCHEMERLICHT_FLGREATER: \
                { \
                target->type = (arg1->value.fl > arg2->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
                break; \
                } \
              case SCHEMERLICHT_LEQ: \
              case SCHEMERLICHT_FLLEQ: \
                { \
                target->type = (arg1->value.fl <= arg2->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
                break; \
                } \
              case SCHEMERLICHT_GEQ: \
              case SCHEMERLICHT_FLGEQ: \
                { \
                target->type = (arg1->value.fl >= arg2->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
                break; \
                } \
              default: \
                schemerlicht_call_primitive(ctxt, function_id, aa, bb, cc); \
                break; \
              } \
            break; \
            } \
          default: \
            schemerlicht_call_primitive(ctxt, function_id, aa, bb, cc); \
            break; \
          } \
          break; \
        } \
      case schemerlicht_object_type_closure: \
        { \
        const schemerlicht_object* arg2 = schemerlicht_vector_at(&ctxt->stack, (aa) + (cc) + 2, schemerlicht_object); \
        switch (schemerlicht_object_get_type(arg2)) \
          { \
          case schemerlicht_object_type_fixnum: \
            { \
            switch (function_id) \
              { \
              case SCHEMERLICHT_CLOSUREREF: \
                { \
                schemerlicht_set_object(target, schemerlicht_vector_at(&arg1->value.v, arg2->value.fx, schemerlicht_object)); \
                break; \
                } \
              default: \
                schemerlicht_call_primitive(ctxt, function_id, aa, bb, cc); \
                break; \
              } \
            break; \
            } \
          default: \
            schemerlicht_call_primitive(ctxt, function_id, aa, bb, cc); \
            break; \
          } \
        break; \
        } \
      default: \
        schemerlicht_call_primitive(ctxt, function_id, aa, bb, cc); \
        break; \
      } \
    } \
  else if ((bb) == 1) \
    { \
    const schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, (aa) + (cc) + 1, schemerlicht_object); \
    switch (schemerlicht_object_get_type(arg1)) \
      { \
      case schemerlicht_object_type_pair: \
        { \
        switch (function_id) \
          { \
          case SCHEMERLICHT_CAR: \
            schemerlicht_set_object(target, schemerlicht_vector_at(&arg1->value.v, 0, schemerlicht_object)); \
            break; \
          case SCHEMERLICHT_CDR: \
            schemerlicht_set_object(target, schemerlicht_vector_at(&arg1->value.v, 1, schemerlicht_object)); \
            break; \
          default: \
            schemerlicht_call_primitive(ctxt, function_id, aa, bb, cc); \
            break; \
          } \
        break; \
        } \
      default: \
        schemerlicht_call_primitive(ctxt, function_id, aa, bb, cc); \
        break; \
      } \
    } \
  else \
    { \
    schemerlicht_call_primitive(ctxt, function_id, aa, bb, cc); \
    }
/*
#define inline_functions(aa, bb, cc) \
  if ((bb) == 2) \
    { \
    const schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, (aa) + (cc) + 1, schemerlicht_object); \
    const schemerlicht_object* arg2 = schemerlicht_vector_at(&ctxt->stack, (aa) + (cc) + 2, schemerlicht_object); \
    if (schemerlicht_object_get_type(arg1) == schemerlicht_object_type_fixnum && schemerlicht_object_get_type(arg2) == schemerlicht_object_type_fixnum) \
      { \
      switch (function_id) \
      { \
        case SCHEMERLICHT_ADD: \
        case SCHEMERLICHT_FXADD: \
          { \
          target->value.fx = arg1->value.fx + arg2->value.fx; \
          target->type = schemerlicht_object_type_fixnum; \
          break; \
          } \
        case SCHEMERLICHT_SUB: \
        case SCHEMERLICHT_FXSUB: \
          { \
          target->value.fx = arg1->value.fx - arg2->value.fx; \
          target->type = schemerlicht_object_type_fixnum; \
          break; \
          } \
        case SCHEMERLICHT_MUL: \
        case SCHEMERLICHT_FXMUL: \
          { \
          target->value.fx = arg1->value.fx * arg2->value.fx; \
          target->type = schemerlicht_object_type_fixnum; \
          break; \
          } \
        case SCHEMERLICHT_DIV: \
        case SCHEMERLICHT_FXDIV: \
          { \
          if (arg2->value.fx == 0) \
            target->type = schemerlicht_object_type_undefined; \
          else \
            { \
            target->value.fx = arg1->value.fx / arg2->value.fx; \
            target->type = schemerlicht_object_type_fixnum; \
            } \
          break; \
          } \
        case SCHEMERLICHT_NOT_EQUAL: \
          { \
          target->type = (arg1->value.fx != arg2->value.fx) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
          break; \
          } \
        case SCHEMERLICHT_EQUAL: \
        case SCHEMERLICHT_FXEQUAL: \
          { \
          target->type = (arg1->value.fx == arg2->value.fx) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
          break; \
          } \
        case SCHEMERLICHT_LESS: \
        case SCHEMERLICHT_FXLESS: \
          { \
          target->type = (arg1->value.fx < arg2->value.fx) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
          break; \
          } \
        case SCHEMERLICHT_GREATER: \
        case SCHEMERLICHT_FXGREATER: \
          { \
          target->type = (arg1->value.fx > arg2->value.fx) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
          break; \
          } \
        case SCHEMERLICHT_LEQ: \
        case SCHEMERLICHT_FXLEQ: \
          { \
          target->type = (arg1->value.fx <= arg2->value.fx) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
          break; \
          } \
        case SCHEMERLICHT_GEQ: \
        case SCHEMERLICHT_FXGEQ: \
          { \
          target->type = (arg1->value.fx >= arg2->value.fx) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
          break; \
          } \
        default: \
          { \
          schemerlicht_call_primitive(ctxt, function_id, aa, bb, cc); \
          break; \
          } \
      } \
      } \
    else if (schemerlicht_object_get_type(arg1) == schemerlicht_object_type_flonum && schemerlicht_object_get_type(arg2) == schemerlicht_object_type_flonum) \
      { \
      switch (function_id) \
      { \
        case SCHEMERLICHT_ADD: \
        case SCHEMERLICHT_FLADD: \
          { \
          target->value.fl = arg1->value.fl + arg2->value.fl; \
          target->type = schemerlicht_object_type_flonum; \
          break; \
          } \
        case SCHEMERLICHT_SUB: \
        case SCHEMERLICHT_FLSUB: \
          { \
          target->value.fl = arg1->value.fl - arg2->value.fl; \
          target->type = schemerlicht_object_type_flonum; \
          break; \
          } \
        case SCHEMERLICHT_MUL: \
        case SCHEMERLICHT_FLMUL: \
          { \
          target->value.fl = arg1->value.fl * arg2->value.fl; \
          target->type = schemerlicht_object_type_flonum; \
          break; \
          } \
        case SCHEMERLICHT_DIV: \
        case SCHEMERLICHT_FLDIV: \
          { \
          target->value.fl = arg1->value.fl / arg2->value.fl; \
          target->type = schemerlicht_object_type_flonum; \
          break; \
          } \
        case SCHEMERLICHT_NOT_EQUAL: \
          { \
          target->type = (arg1->value.fl != arg2->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
          break; \
          } \
        case SCHEMERLICHT_EQUAL: \
        case SCHEMERLICHT_FLEQUAL: \
          { \
          target->type = (arg1->value.fl == arg2->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
          break; \
          } \
        case SCHEMERLICHT_LESS: \
        case SCHEMERLICHT_FLLESS: \
          { \
          target->type = (arg1->value.fl < arg2->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
          break; \
          } \
        case SCHEMERLICHT_GREATER: \
        case SCHEMERLICHT_FLGREATER: \
          { \
          target->type = (arg1->value.fl > arg2->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
          break; \
          } \
        case SCHEMERLICHT_LEQ: \
        case SCHEMERLICHT_FLLEQ: \
          { \
          target->type = (arg1->value.fl <= arg2->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
          break; \
          } \
        case SCHEMERLICHT_GEQ: \
        case SCHEMERLICHT_FLGEQ: \
          { \
          target->type = (arg1->value.fl >= arg2->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false; \
          break; \
          } \
        default: \
          { \
          schemerlicht_call_primitive(ctxt, function_id, aa, bb, cc); \
          break; \
          } \
      } \
      } \
    else if (schemerlicht_object_get_type(arg1) == schemerlicht_object_type_closure && schemerlicht_object_get_type(arg2) == schemerlicht_object_type_fixnum) \
      { \
      switch (function_id) \
      { \
        case SCHEMERLICHT_CLOSUREREF: \
          { \
          schemerlicht_set_object(target, schemerlicht_vector_at(&arg1->value.v, arg2->value.fx, schemerlicht_object)); \
          break; \
          } \
        default: \
          { \
          schemerlicht_call_primitive(ctxt, function_id, aa, bb, cc); \
          break; \
          } \
      } \
      } \
    else \
      { \
      schemerlicht_call_primitive(ctxt, function_id, aa, bb, cc); \
      } \
    } \
  else \
    { \
    schemerlicht_call_primitive(ctxt, function_id, aa, bb, cc); \
    }
    */
#endif //SCHEMERLICHT_INLINES_H