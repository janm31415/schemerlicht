#include "vm.h"
#include "context.h"
#include "error.h"
#include "map.h"
#include "primitives.h"

#include <stdio.h>

static schemerlicht_string instruction_to_string(schemerlicht_context* ctxt, schemerlicht_instruction i)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "");
  char buffer[256];
  switch (SCHEMERLICHT_GET_OPCODE(i))
    {
    case SCHEMERLICHT_OPCODE_MOVE:
    {
    int a = SCHEMERLICHT_GETARG_A(i);
    int b = SCHEMERLICHT_GETARG_B(i);
    schemerlicht_string_append_cstr(ctxt, &s, "MOVE R(");
    sprintf(buffer, "%d", a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := R(");
    sprintf(buffer, "%d", b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")");
    break;
    }
    case SCHEMERLICHT_OPCODE_LOADGLOBAL:
    {
    int a = SCHEMERLICHT_GETARG_A(i);
    int b = SCHEMERLICHT_GETARG_Bx(i);
    schemerlicht_string_append_cstr(ctxt, &s, "LOADGLOBAL R(");
    sprintf(buffer, "%d", a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := GLOBAL(");
    sprintf(buffer, "%d", b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")");
    break;
    }
    case SCHEMERLICHT_OPCODE_STOREGLOBAL:
    {
    int a = SCHEMERLICHT_GETARG_A(i);
    int b = SCHEMERLICHT_GETARG_Bx(i);
    schemerlicht_string_append_cstr(ctxt, &s, "STOREGLOBAL GLOBAL(");
    sprintf(buffer, "%d", b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := R(");
    sprintf(buffer, "%d", a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")");
    break;
    }
    case SCHEMERLICHT_OPCODE_LOADK:
    {
    int a = SCHEMERLICHT_GETARG_A(i);
    int b = SCHEMERLICHT_GETARG_Bx(i);
    schemerlicht_string_append_cstr(ctxt, &s, "LOADK R(");
    sprintf(buffer, "%d", a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := K(");
    sprintf(buffer, "%d", b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")");
    break;
    }
    case SCHEMERLICHT_OPCODE_CALL:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    schemerlicht_string_append_cstr(ctxt, &s, "CALL R(");
    sprintf(buffer, "%d", a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")");
    break;
    }
    case SCHEMERLICHT_OPCODE_EQTYPE:
    {
    int a = SCHEMERLICHT_GETARG_A(i);
    int b = SCHEMERLICHT_GETARG_B(i);
    schemerlicht_string_append_cstr(ctxt, &s, "EQTYPE if R(");
    sprintf(buffer, "%d", a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")->type == ");
    sprintf(buffer, "%d", b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, " then skip next line");
    break;
    }
    case SCHEMERLICHT_OPCODE_JMP:
    {
    int b = SCHEMERLICHT_GETARG_sBx(i);
    schemerlicht_string_append_cstr(ctxt, &s, "JUMP ");
    sprintf(buffer, "%d", b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    break;
    }
    case SCHEMERLICHT_OPCODE_RETURN:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    schemerlicht_string_append_cstr(ctxt, &s, "RETURN R(");
    sprintf(buffer, "%d", a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")");
    break;
    }
    default:
      break;
    }
  return s;
  }

static schemerlicht_string debug_object_to_string(schemerlicht_context* ctxt, schemerlicht_object* obj)
  {
  schemerlicht_string res = schemerlicht_object_to_string(ctxt, obj);
  if (obj->type == schemerlicht_object_type_lambda)
    {
    char buffer[256];
    uint64_t ptr = cast(uint64_t, obj->value.ptr);
    sprintf(buffer, "%lld", ptr);
    schemerlicht_string_append_cstr(ctxt, &res, ": ");
    schemerlicht_string_append_cstr(ctxt, &res, buffer);
    }
  if (obj->type == schemerlicht_object_type_primitive)
    {
    char buffer[256];
    uint64_t fx = cast(uint64_t, obj->value.fx);
    sprintf(buffer, "%lld", fx);
    schemerlicht_string_append_cstr(ctxt, &res, ": ");
    schemerlicht_string_append_cstr(ctxt, &res, buffer);
    }
  return res;
  }

schemerlicht_object* schemerlicht_run_debug(schemerlicht_context* ctxt, schemerlicht_string* s, schemerlicht_function** fun)
  {
  schemerlicht_instruction* pc = schemerlicht_vector_begin(&(*fun)->code, schemerlicht_instruction);
  schemerlicht_instruction* pc_end = schemerlicht_vector_end(&(*fun)->code, schemerlicht_instruction);
  while (pc < pc_end)
    {
    const schemerlicht_instruction i = *pc++;
    schemerlicht_string tmp = instruction_to_string(ctxt, i);
    schemerlicht_string_append(ctxt, s, &tmp);
    schemerlicht_string_destroy(ctxt, &tmp);
    schemerlicht_string_append_cstr(ctxt, s, "\n");
    switch (SCHEMERLICHT_GET_OPCODE(i))
      {
      case SCHEMERLICHT_OPCODE_MOVE:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int b = SCHEMERLICHT_GETARG_B(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      schemerlicht_object* source = schemerlicht_vector_at(&ctxt->stack, b, schemerlicht_object);
      schemerlicht_set_object(target, source);
      break;
      }
      case SCHEMERLICHT_OPCODE_LOADGLOBAL:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int bx = SCHEMERLICHT_GETARG_Bx(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      schemerlicht_object* global = schemerlicht_vector_at(&ctxt->globals, bx, schemerlicht_object);
      schemerlicht_set_object(target, global);
      break;
      }
      case SCHEMERLICHT_OPCODE_STOREGLOBAL:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int bx = SCHEMERLICHT_GETARG_Bx(i);
      schemerlicht_object* source = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      schemerlicht_object* global = schemerlicht_vector_at(&ctxt->globals, bx, schemerlicht_object);
      schemerlicht_set_object(global, source);
      break;
      }
      case SCHEMERLICHT_OPCODE_LOADK:
      {
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, SCHEMERLICHT_GETARG_A(i), schemerlicht_object);
      schemerlicht_object* k = schemerlicht_vector_at(&(*fun)->constants, SCHEMERLICHT_GETARG_Bx(i), schemerlicht_object);
      schemerlicht_string obj_str = debug_object_to_string(ctxt, k);
      schemerlicht_string_append_cstr(ctxt, s, "   k -> ");
      schemerlicht_string_append(ctxt, s, &obj_str);
      schemerlicht_string_destroy(ctxt, &obj_str);
      schemerlicht_string_append_cstr(ctxt, s, "\n");
      schemerlicht_set_object(target, k);
      break;
      }
      case SCHEMERLICHT_OPCODE_CALL:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      if (target->type == schemerlicht_object_type_primitive)
        {
        schemerlicht_fixnum function_id = target->value.fx;
        const int b = SCHEMERLICHT_GETARG_B(i);
        const int c = SCHEMERLICHT_GETARG_C(i);
        schemerlicht_call_primitive(ctxt, function_id, a, b, c);
        schemerlicht_string_append_cstr(ctxt, s, "   primitive call\n");
        }
      else if (target->type == schemerlicht_object_type_primitive_object)
        {        
        schemerlicht_fixnum function_id = target->value.fx;
        const int b = SCHEMERLICHT_GETARG_B(i);
        const int c = SCHEMERLICHT_GETARG_C(i);
        schemerlicht_call_primitive(ctxt, function_id, a, b - 1, c + 1); // skip the closure argument
        schemerlicht_string_append_cstr(ctxt, s, "   primitive object call\n");
        schemerlicht_object continuation = *schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
        schemerlicht_assert(continuation.type == schemerlicht_object_type_closure);
        schemerlicht_object result_of_prim_call = *schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
        schemerlicht_object* r0 = schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
        schemerlicht_object* r1 = schemerlicht_vector_at(&ctxt->stack, 1, schemerlicht_object);
        schemerlicht_set_object(r0, &continuation);
        schemerlicht_set_object(r1, &result_of_prim_call);
        schemerlicht_object* lambda_obj = schemerlicht_vector_at(&continuation.value.v, 0, schemerlicht_object);
        schemerlicht_assert(lambda_obj->type == schemerlicht_object_type_lambda);
        schemerlicht_function* lambda = cast(schemerlicht_function*, lambda_obj->value.ptr);
        pc = schemerlicht_vector_begin(&lambda->code, schemerlicht_instruction);
        pc_end = schemerlicht_vector_end(&lambda->code, schemerlicht_instruction);
        fun = &lambda;
        }
      else if (target->type == schemerlicht_object_type_closure)
        {
        schemerlicht_object* lambda_obj = schemerlicht_vector_at(&target->value.v, 0, schemerlicht_object);
        schemerlicht_assert(lambda_obj->type == schemerlicht_object_type_lambda);
        schemerlicht_function* lambda = cast(schemerlicht_function*, lambda_obj->value.ptr);
        //at R(0) we expect the closure (i.e. target) but this already so by compile_funcall
        //schemerlicht_set_object(schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object), target);
        pc = schemerlicht_vector_begin(&lambda->code, schemerlicht_instruction);
        pc_end = schemerlicht_vector_end(&lambda->code, schemerlicht_instruction);
        fun = &lambda;
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
      int sbx = SCHEMERLICHT_GETARG_sBx(i);
      pc += sbx;
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
          schemerlicht_object* srcj = schemerlicht_vector_at(&ctxt->stack, a + j, schemerlicht_object);
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

schemerlicht_object* schemerlicht_run(schemerlicht_context* ctxt, schemerlicht_function** fun)
  {
  schemerlicht_instruction* pc = schemerlicht_vector_begin(&(*fun)->code, schemerlicht_instruction);
  schemerlicht_instruction* pc_end = schemerlicht_vector_end(&(*fun)->code, schemerlicht_instruction);
  while (pc < pc_end)
    {
    const schemerlicht_instruction i = *pc++;
    switch (SCHEMERLICHT_GET_OPCODE(i))
      {
      case SCHEMERLICHT_OPCODE_MOVE:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int b = SCHEMERLICHT_GETARG_B(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      schemerlicht_object* source = schemerlicht_vector_at(&ctxt->stack, b, schemerlicht_object);
      schemerlicht_set_object(target, source);
      break;
      }
      case SCHEMERLICHT_OPCODE_LOADGLOBAL:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int bx = SCHEMERLICHT_GETARG_Bx(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      schemerlicht_object* global = schemerlicht_vector_at(&ctxt->globals, bx, schemerlicht_object);
      schemerlicht_set_object(target, global);
      break;
      }
      case SCHEMERLICHT_OPCODE_STOREGLOBAL:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int bx = SCHEMERLICHT_GETARG_Bx(i);
      schemerlicht_object* source = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      schemerlicht_object* global = schemerlicht_vector_at(&ctxt->globals, bx, schemerlicht_object);
      schemerlicht_set_object(global, source);
      break;
      }
      case SCHEMERLICHT_OPCODE_LOADK:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int bx = SCHEMERLICHT_GETARG_Bx(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      schemerlicht_object* k = schemerlicht_vector_at(&(*fun)->constants, bx, schemerlicht_object);
      schemerlicht_set_object(target, k);
      break;
      }
      case SCHEMERLICHT_OPCODE_CALL:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      if (target->type == schemerlicht_object_type_primitive)
        {
        schemerlicht_fixnum function_id = target->value.fx;
        const int b = SCHEMERLICHT_GETARG_B(i);
        const int c = SCHEMERLICHT_GETARG_C(i);
        schemerlicht_call_primitive(ctxt, function_id, a, b, c);
        }
      else if (target->type == schemerlicht_object_type_primitive_object)
        {
        schemerlicht_fixnum function_id = target->value.fx;
        const int b = SCHEMERLICHT_GETARG_B(i);
        const int c = SCHEMERLICHT_GETARG_C(i);
        schemerlicht_call_primitive(ctxt, function_id, a, b - 1, c + 1); // skip the closure argument
        }
      //else if (target->type == schemerlicht_object_type_lambda)
      //  {
      //  schemerlicht_function* lambda = cast(schemerlicht_function*, target->value.ptr);
      //  //schemerlicht_run(ctxt, lambda); // I think, because of continuation passing style, we could replace fun by lambda here.
      //  pc = schemerlicht_vector_begin(&lambda->code, schemerlicht_instruction);
      //  pc_end = schemerlicht_vector_end(&lambda->code, schemerlicht_instruction);
      //  fun = &lambda;      
      //  }
      else if (target->type == schemerlicht_object_type_closure)
        {
        schemerlicht_object* lambda_obj = schemerlicht_vector_at(&target->value.v, 0, schemerlicht_object);
        schemerlicht_assert(lambda_obj->type == schemerlicht_object_type_lambda);
        schemerlicht_function* lambda = cast(schemerlicht_function*, lambda_obj->value.ptr);
        //at R(0) we expect the closure (i.e. target) but this already so by compile_funcall
        //schemerlicht_set_object(schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object), target); 
        pc = schemerlicht_vector_begin(&lambda->code, schemerlicht_instruction);
        pc_end = schemerlicht_vector_end(&lambda->code, schemerlicht_instruction);
        fun = &lambda;
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
          schemerlicht_object* srcj = schemerlicht_vector_at(&ctxt->stack, a + j, schemerlicht_object);
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


schemerlicht_string schemerlicht_fun_to_string(schemerlicht_context* ctxt, schemerlicht_function* fun)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "");
  schemerlicht_instruction* it = schemerlicht_vector_begin(&fun->code, schemerlicht_instruction);
  schemerlicht_instruction* it_end = schemerlicht_vector_end(&fun->code, schemerlicht_instruction);
  for (; it != it_end; ++it)
    {
    const schemerlicht_instruction i = *it;
    schemerlicht_string tmp = instruction_to_string(ctxt, i, 0);
    schemerlicht_string_append(ctxt, &s, &tmp);
    schemerlicht_string_destroy(ctxt, &tmp);
    schemerlicht_string_append_cstr(ctxt, &s, "\n");
    }

  schemerlicht_function** fit = schemerlicht_vector_begin(&fun->lambdas, schemerlicht_function*);
  schemerlicht_function** fit_end = schemerlicht_vector_end(&fun->lambdas, schemerlicht_function*);
  for (; fit != fit_end; ++fit)
    {
    schemerlicht_string_append_cstr(ctxt, &s, "\nSUBFUNCTION:\n");
    schemerlicht_string subfunstr = schemerlicht_fun_to_string(ctxt, *fit);
    schemerlicht_string_append(ctxt, &s, &subfunstr);
    schemerlicht_string_destroy(ctxt, &subfunstr);
    }
  return s;
  }