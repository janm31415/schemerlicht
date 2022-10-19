#include "vm.h"
#include "context.h"
#include "error.h"
#include "map.h"
#include "primitives.h"


#define opmode(t,b,bk,ck,sa,k,m) (((t)<<schemerlicht_opcode_mode_operator_is_test) | \
   ((b)<<schemerlicht_opcode_mode_B_is_register) | ((bk)<<schemerlicht_opcode_mode_B_is_register_or_constant) | ((ck)<<schemerlicht_opcode_mode_C_is_register_or_constant) | \
   ((sa)<<schemerlicht_opcode_mode_sets_register_A) | ((k)<<schemerlicht_opcode_mode_Bx_is_a_constant) | (m))


const schemerlicht_byte schemerlicht_opcode_modes[SCHEMERLICHT_NUM_OPCODES] = {
  /*       T  B Bk Ck sA  K  mode			   opcode    */
  opmode(0, 1, 0, 0, 1, 0, schemerlicht_iABC)		/* SCHEMERLICHT_OPCODE_MOVE */
 ,opmode(0, 0, 0, 0, 1, 1, schemerlicht_iABx)		/* SCHEMERLICHT_OPCODE_LOADK */
 ,opmode(0, 0, 0, 0, 0, 0, schemerlicht_iABC)		/* SCHEMERLICHT_OPCODE_CALL */
  };


schemerlicht_object* schemerlicht_run(schemerlicht_context* ctxt, schemerlicht_function* fun)
  {
  schemerlicht_instruction* pc = schemerlicht_vector_begin(&fun->code, schemerlicht_instruction);
  schemerlicht_instruction* pc_end = schemerlicht_vector_end(&fun->code, schemerlicht_instruction);
  while (pc < pc_end)
    {
    const schemerlicht_instruction i = *pc++;
    const int a = SCHEMERLICHT_GETARG_A(i);
    schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
    switch (SCHEMERLICHT_GET_OPCODE(i))
      {
      case SCHEMERLICHT_OPCODE_LOADK:
      {
      schemerlicht_object* k = schemerlicht_vector_at(&fun->constants, SCHEMERLICHT_GETARG_Bx(i), schemerlicht_object);
      schemerlicht_set_object(target, k);
      break;
      }
      case SCHEMERLICHT_OPCODE_CALL:
      {
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
      default:
        schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
      }
    }
  return schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
  }