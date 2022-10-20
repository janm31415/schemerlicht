#include "vm.h"
#include "context.h"
#include "error.h"
#include "map.h"
#include "primitives.h"

schemerlicht_object* schemerlicht_run(schemerlicht_context* ctxt, schemerlicht_function* fun)
  {
  schemerlicht_instruction* pc = schemerlicht_vector_begin(&fun->code, schemerlicht_instruction);
  schemerlicht_instruction* pc_end = schemerlicht_vector_end(&fun->code, schemerlicht_instruction);
  while (pc < pc_end)
    {
    const schemerlicht_instruction i = *pc++;    
    switch (SCHEMERLICHT_GET_OPCODE(i))
      {
      case SCHEMERLICHT_OPCODE_MOVE:
      {
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, SCHEMERLICHT_GETARG_A(i), schemerlicht_object);
      schemerlicht_object* source = schemerlicht_vector_at(&ctxt->stack, SCHEMERLICHT_GETARG_B(i), schemerlicht_object);
      schemerlicht_set_object(target, source);
      break;
      }
      case SCHEMERLICHT_OPCODE_LOADK:
      {
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, SCHEMERLICHT_GETARG_A(i), schemerlicht_object);
      schemerlicht_object* k = schemerlicht_vector_at(&fun->constants, SCHEMERLICHT_GETARG_Bx(i), schemerlicht_object);
      schemerlicht_set_object(target, k);
      break;
      }
      case SCHEMERLICHT_OPCODE_CALL:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      if (target->type == schemerlicht_object_type_fixnum)
        {
        schemerlicht_fixnum function_id = target->value.fx;
        const int b = SCHEMERLICHT_GETARG_B(i);
        const int c = SCHEMERLICHT_GETARG_C(i);
        schemerlicht_call_primitive(ctxt, function_id, a, b, c);
        }
      else
        {
        schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
        }
      break;
      }
      case SCHEMERLICHT_OPCODE_EQTYPE:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int b = SCHEMERLICHT_GETARG_B(i);
      schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      if (ra->type == b)
        {
        ++pc;
        }
      else
        {
        schemerlicht_instruction next_i = *pc++;
        schemerlicht_assert(SCHEMERLICHT_GET_OPCODE(next_i) == SCHEMERLICHT_OPCODE_JMP);
        int offset = SCHEMERLICHT_GETARG_sBx(next_i);
        pc += offset;
        }
      break;
      }
      case SCHEMERLICHT_OPCODE_JMP:
      {
      pc += SCHEMERLICHT_GETARG_sBx(i);
      break;
      }
      case SCHEMERLICHT_OPCODE_RETURN:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);      
      if (a != 0)
        {
        int b = SCHEMERLICHT_GETARG_B(i);
        for (int j = 0; j < b; ++j)
          {
          schemerlicht_object* retj = schemerlicht_vector_at(&ctxt->stack, j, schemerlicht_object);
          schemerlicht_object* srcj = schemerlicht_vector_at(&ctxt->stack, a+j, schemerlicht_object);
          schemerlicht_set_object(retj, srcj);
          }
        }
      pc = pc_end;
      break;
      }
      default:
        schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
      }
    }
  return schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
  }