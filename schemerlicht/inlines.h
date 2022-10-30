#ifndef SCHEMERLICHT_INLINES_H
#define SCHEMERLICHT_INLINES_H

#define inline_functions(aa, bb, cc) \
  if ((bb) == 2) \
    { \
    schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, (aa) + (cc) + 1, schemerlicht_object); \
    schemerlicht_object* arg2 = schemerlicht_vector_at(&ctxt->stack, (aa) + (cc) + 2, schemerlicht_object); \
    if (arg1->type == schemerlicht_object_type_fixnum && arg2->type == schemerlicht_object_type_fixnum) \
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
          target->value.fx = arg1->value.fx / arg2->value.fx; \
          target->type = schemerlicht_object_type_fixnum; \
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
    else if (arg1->type == schemerlicht_object_type_flonum && arg2->type == schemerlicht_object_type_flonum) \
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
    else \
      { \
      schemerlicht_call_primitive(ctxt, function_id, aa, bb, cc); \
      } \
    } \
  else \
    { \
    schemerlicht_call_primitive(ctxt, function_id, aa, bb, cc); \
    }

#endif //SCHEMERLICHT_INLINES_H