#include "vm.h"
#include "context.h"
#include "error.h"
#include "map.h"
#include "primitives.h"
#include "gc.h"
#include "inlines.h"

#include <stdio.h>
#include <string.h>

static void make_variable_arity_list(schemerlicht_context* ctxt, int a, int b)
  {
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
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
    schemerlicht_object* last_arg = schemerlicht_vector_at(&ctxt->stack, a + b - 1, schemerlicht_object);
    schemerlicht_set_object(v0, last_arg);
    schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
    schemerlicht_set_object(heap_obj, &obj1);
    ++ctxt->heap_pos;
    for (int j = b - 2; j >= 0; --j)
      {
      schemerlicht_object obj2 = make_schemerlicht_object_pair(ctxt);
      v0 = schemerlicht_vector_at(&obj2.value.v, 0, schemerlicht_object);
      v1 = schemerlicht_vector_at(&obj2.value.v, 1, schemerlicht_object);
      schemerlicht_set_object(v1, heap_obj);
      schemerlicht_object* arg = schemerlicht_vector_at(&ctxt->stack, a + j, schemerlicht_object);
      schemerlicht_set_object(v0, arg);
      heap_obj = &ctxt->heap[ctxt->heap_pos];
      schemerlicht_set_object(heap_obj, &obj2);
      ++ctxt->heap_pos;
      }
    schemerlicht_set_object(ra, heap_obj);
    }
  }

static schemerlicht_string instruction_to_string(schemerlicht_context* ctxt, schemerlicht_instruction i)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "");
  char buffer[256];
  switch (SCHEMERLICHT_GET_OPCODE(i))
    {
    case SCHEMERLICHT_OPCODE_MOVE:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_B(i);
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
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_Bx(i);
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
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_Bx(i);
    schemerlicht_string_append_cstr(ctxt, &s, "STOREGLOBAL GLOBAL(");
    sprintf(buffer, "%d", b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := R(");
    sprintf(buffer, "%d", a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")");
    break;
    }
    case SCHEMERLICHT_OPCODE_MOVETOP:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_B(i);
    schemerlicht_string_append_cstr(ctxt, &s, "MOVETOP R(0)..R(");
    sprintf(buffer, "%d", b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := R(");
    sprintf(buffer, "%d", a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")..R(");
    sprintf(buffer, "%d", a + b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")   R(");
    sprintf(buffer, "%d", b + 1);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")->type = blocking");
    break;
    }
    case SCHEMERLICHT_OPCODE_LOADK:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_Bx(i);
    schemerlicht_string_append_cstr(ctxt, &s, "LOADK R(");
    sprintf(buffer, "%d", a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := K(");
    sprintf(buffer, "%d", b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")");
    break;
    }
    case SCHEMERLICHT_OPCODE_SETPRIM:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_B(i);
    schemerlicht_string_append_cstr(ctxt, &s, "SETPRIM R(");
    sprintf(buffer, "%d", a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := ");
    sprintf(buffer, "%d", b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    break;
    }
    case SCHEMERLICHT_OPCODE_SETPRIMOBJ:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_B(i);
    schemerlicht_string_append_cstr(ctxt, &s, "SETPRIMOBJ R(");
    sprintf(buffer, "%d", a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := ");
    sprintf(buffer, "%d", b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    break;
    }
    case SCHEMERLICHT_OPCODE_SETFIXNUM:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_sBx(i);
    schemerlicht_string_append_cstr(ctxt, &s, "SETFIXNUM R(");
    sprintf(buffer, "%d", a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := ");
    sprintf(buffer, "%d", b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    break;
    }
    case SCHEMERLICHT_OPCODE_SETCHAR:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_B(i);
    schemerlicht_string_append_cstr(ctxt, &s, "SETCHAR R(");
    sprintf(buffer, "%d", a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := ");
    sprintf(buffer, "%d", b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    break;
    }
    case SCHEMERLICHT_OPCODE_SETTYPE:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_B(i);
    schemerlicht_string_append_cstr(ctxt, &s, "SETTYPE R(");
    sprintf(buffer, "%d", a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ").type := ");
    sprintf(buffer, "%d", b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    break;
    }
    case SCHEMERLICHT_OPCODE_LIST_STACK:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    schemerlicht_string_append_cstr(ctxt, &s, "LISTSTACK R(");
    sprintf(buffer, "%d", a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := (list R(A) R(A+1) ... R(A+x)), list until R(A+x+1).type == schemerlicht_object_type_blocked");
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
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_B(i);
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
  if (obj->type == schemerlicht_object_type_primitive_object)
    {
    char buffer[256];
    uint64_t fx = cast(uint64_t, obj->value.fx);
    sprintf(buffer, "%lld", fx);
    schemerlicht_string_append_cstr(ctxt, &res, ": (as object) ");
    schemerlicht_string_append_cstr(ctxt, &res, buffer);
    }
  return res;
  }

schemerlicht_object* schemerlicht_run_debug(schemerlicht_context* ctxt, schemerlicht_string* s, schemerlicht_function* fun)
  {
  schemerlicht_assert(fun != NULL);
  schemerlicht_instruction* pc = schemerlicht_vector_begin(&(fun)->code, schemerlicht_instruction);
  schemerlicht_instruction* pc_end = schemerlicht_vector_end(&(fun)->code, schemerlicht_instruction);
  while (pc < pc_end)
    {
    const schemerlicht_instruction i = *pc++;
    schemerlicht_string tmp = instruction_to_string(ctxt, i);
    printf("%s\n", tmp.string_ptr);
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
      case SCHEMERLICHT_OPCODE_MOVETOP:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int b = SCHEMERLICHT_GETARG_B(i);
#if 1
      for (int j = 0; j <= b; ++j)
        {
        schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, j, schemerlicht_object);
        schemerlicht_object* source = schemerlicht_vector_at(&ctxt->stack, a + j, schemerlicht_object);
        schemerlicht_set_object(target, source);
        }
      schemerlicht_object* blocking = schemerlicht_vector_at(&ctxt->stack, b + 1, schemerlicht_object);
      blocking->type = schemerlicht_object_type_blocking;
#else
      schemerlicht_object* st = cast(schemerlicht_object*, ctxt->stack.vector_ptr);
      memcpy(st, st + a, (b + 1) * sizeof(schemerlicht_object));
      schemerlicht_object* blocking = schemerlicht_vector_at(&ctxt->stack, b + 1, schemerlicht_object);
      blocking->type = schemerlicht_object_type_blocking;
#endif
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
      printf("  constants size: %d\n", (fun)->constants.vector_size);
      schemerlicht_object* k = schemerlicht_vector_at(&(fun)->constants, bx, schemerlicht_object);
      schemerlicht_string obj_str = debug_object_to_string(ctxt, k);
      printf("  k -> %s\n", obj_str.string_ptr);
      schemerlicht_string_append_cstr(ctxt, s, "   k -> ");
      schemerlicht_string_append(ctxt, s, &obj_str);
      schemerlicht_string_destroy(ctxt, &obj_str);
      schemerlicht_string_append_cstr(ctxt, s, "\n");
      schemerlicht_set_object(target, k);
      break;
      }
      case SCHEMERLICHT_OPCODE_SETFIXNUM:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      const int b = SCHEMERLICHT_GETARG_sBx(i);
      target->type = schemerlicht_object_type_fixnum;
      target->value.fx = b;
      break;
      }
      case SCHEMERLICHT_OPCODE_SETPRIM:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      const int b = SCHEMERLICHT_GETARG_B(i);
      target->type = schemerlicht_object_type_primitive;
      target->value.fx = b;
      break;
      }
      case SCHEMERLICHT_OPCODE_SETPRIMOBJ:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      const int b = SCHEMERLICHT_GETARG_B(i);
      target->type = schemerlicht_object_type_primitive_object;
      target->value.fx = b;
      break;
      }
      case SCHEMERLICHT_OPCODE_SETCHAR:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      const int b = SCHEMERLICHT_GETARG_B(i);
      target->type = schemerlicht_object_type_char;
      target->value.ch = cast(schemerlicht_byte, b);
      break;
      }
      case SCHEMERLICHT_OPCODE_SETTYPE:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      int b = SCHEMERLICHT_GETARG_B(i);
      target->type = b;
      break;
      }
      case SCHEMERLICHT_OPCODE_CALL:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int b = SCHEMERLICHT_GETARG_B(i);
      const int c = SCHEMERLICHT_GETARG_C(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      if (target->type == schemerlicht_object_type_primitive)
        {
        schemerlicht_fixnum function_id = target->value.fx;
        //schemerlicht_call_primitive(ctxt, function_id, a, b, c);
        inline_functions(a, b, c);
        schemerlicht_string_append_cstr(ctxt, s, "   primitive call\n");
        printf("  primitive call\n");
        }
      else if (target->type == schemerlicht_object_type_primitive_object)
        {
        //schemerlicht_string stackstr = schemerlicht_show_stack(ctxt, 0, 9);
        //printf("%s\n", stackstr.string_ptr);
        //schemerlicht_string_destroy(ctxt, &stackstr);
        schemerlicht_fixnum function_id = target->value.fx;
        //schemerlicht_call_primitive(ctxt, function_id, a, b - 1, c + 1); // skip the closure argument
        inline_functions(a, b - 1, c + 1);
        schemerlicht_string_append_cstr(ctxt, s, "   primitive object call\n");
        printf("  primitive object call\n");
        //stackstr = schemerlicht_show_stack(ctxt, 0, 9);
        //printf("%s\n", stackstr.string_ptr);
        //schemerlicht_string_destroy(ctxt, &stackstr);

        schemerlicht_object continuation = *schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
        schemerlicht_assert(continuation.type == schemerlicht_object_type_closure);
        schemerlicht_object result_of_prim_call = *schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
        schemerlicht_object* r0 = schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
        schemerlicht_object* r1 = schemerlicht_vector_at(&ctxt->stack, 1, schemerlicht_object);
        schemerlicht_set_object(r0, &continuation);
        schemerlicht_set_object(r1, &result_of_prim_call);
        //stackstr = schemerlicht_show_stack(ctxt, 0, 9);
        //printf("%s\n", stackstr.string_ptr);
        //schemerlicht_string_destroy(ctxt, &stackstr);
        schemerlicht_object* lambda_obj = schemerlicht_vector_at(&continuation.value.v, 0, schemerlicht_object);
        schemerlicht_assert(lambda_obj->type == schemerlicht_object_type_lambda);
        schemerlicht_function* lambda = cast(schemerlicht_function*, lambda_obj->value.ptr);
        pc = schemerlicht_vector_begin(&lambda->code, schemerlicht_instruction);
        pc_end = schemerlicht_vector_end(&lambda->code, schemerlicht_instruction);
        fun = lambda;
        schemerlicht_check_garbage_collection(ctxt);
        }
      else if (target->type == schemerlicht_object_type_closure)
        {
        schemerlicht_object* lambda_obj = schemerlicht_vector_at(&target->value.v, 0, schemerlicht_object);
        schemerlicht_assert(lambda_obj->type == schemerlicht_object_type_lambda);
        schemerlicht_function* lambda = cast(schemerlicht_function*, lambda_obj->value.ptr);
        schemerlicht_assert(lambda != NULL);
        //at R(0) we expect the closure (i.e. target) but this already so by compile_funcall
        //schemerlicht_set_object(schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object), target);
        pc = schemerlicht_vector_begin(&lambda->code, schemerlicht_instruction);
        pc_end = schemerlicht_vector_end(&lambda->code, schemerlicht_instruction);
        fun = lambda;
        printf("now starting GC\n");
        schemerlicht_check_garbage_collection(ctxt);
        }
      else
        {        
        printf("Calling an unknown object\n");
        schemerlicht_string stackstr = schemerlicht_show_stack(ctxt, 0, 9);
        printf("%s\n", stackstr.string_ptr);
        schemerlicht_string_destroy(ctxt, &stackstr);

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
      case SCHEMERLICHT_OPCODE_LIST_STACK:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      int x = a;
      schemerlicht_object* rx = schemerlicht_vector_at(&ctxt->stack, x, schemerlicht_object);
      while (rx->type != schemerlicht_object_type_blocking)
        {
        ++x;
        rx = schemerlicht_vector_at(&ctxt->stack, x, schemerlicht_object);
        }
      make_variable_arity_list(ctxt, a, x - a);
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
      const int b = SCHEMERLICHT_GETARG_B(i);
      if (a != 0)
        {
#if 1
        for (int j = 0; j < b; ++j)
          {
          schemerlicht_object* retj = schemerlicht_vector_at(&ctxt->stack, j, schemerlicht_object);
          schemerlicht_object* srcj = schemerlicht_vector_at(&ctxt->stack, a + j, schemerlicht_object);
          schemerlicht_set_object(retj, srcj);
          }
#else
        schemerlicht_object* st = cast(schemerlicht_object*, ctxt->stack.vector_ptr);
        memcpy(st, st + a, b * sizeof(schemerlicht_object));
#endif
        }
      schemerlicht_object* stack_to_block = schemerlicht_vector_at(&ctxt->stack, b, schemerlicht_object);
      stack_to_block->type = schemerlicht_object_type_blocking;
      pc = pc_end;
      schemerlicht_check_garbage_collection(ctxt);
      break;
      }
      default:
        schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
      }
    }
  return schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
  }

schemerlicht_object* schemerlicht_run(schemerlicht_context* ctxt, schemerlicht_function* fun)
  {
  schemerlicht_assert(fun != NULL);
  schemerlicht_instruction* pc = schemerlicht_vector_begin(&(fun)->code, schemerlicht_instruction);
  schemerlicht_instruction* pc_end = schemerlicht_vector_end(&(fun)->code, schemerlicht_instruction);
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
      case SCHEMERLICHT_OPCODE_MOVETOP:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int b = SCHEMERLICHT_GETARG_B(i);
#if 1
      for (int j = 0; j <= b; ++j)
        {
        schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, j, schemerlicht_object);
        schemerlicht_object* source = schemerlicht_vector_at(&ctxt->stack, a + j, schemerlicht_object);
        schemerlicht_set_object(target, source);
        }
      schemerlicht_object* blocking = schemerlicht_vector_at(&ctxt->stack, b + 1, schemerlicht_object);
      blocking->type = schemerlicht_object_type_blocking;
#else
      schemerlicht_object* st = cast(schemerlicht_object*, ctxt->stack.vector_ptr);
      memcpy(st, st + a, (b + 1) * sizeof(schemerlicht_object));
      schemerlicht_object* blocking = schemerlicht_vector_at(&ctxt->stack, b + 1, schemerlicht_object);
      blocking->type = schemerlicht_object_type_blocking;
#endif
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
      schemerlicht_object* k = schemerlicht_vector_at(&(fun)->constants, bx, schemerlicht_object);
      schemerlicht_set_object(target, k);
      break;
      }
      case SCHEMERLICHT_OPCODE_SETFIXNUM:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      const int b = SCHEMERLICHT_GETARG_sBx(i);
      target->type = schemerlicht_object_type_fixnum;
      target->value.fx = b;
      break;
      }
      case SCHEMERLICHT_OPCODE_SETPRIM:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      const int b = SCHEMERLICHT_GETARG_B(i);
      target->type = schemerlicht_object_type_primitive;
      target->value.fx = b;
      break;
      }
      case SCHEMERLICHT_OPCODE_SETPRIMOBJ:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      const int b = SCHEMERLICHT_GETARG_B(i);
      target->type = schemerlicht_object_type_primitive_object;
      target->value.fx = b;
      break;
      }
      case SCHEMERLICHT_OPCODE_SETCHAR:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      const int b = SCHEMERLICHT_GETARG_B(i);
      target->type = schemerlicht_object_type_char;
      target->value.ch = cast(schemerlicht_byte, b);
      break;
      }
      case SCHEMERLICHT_OPCODE_SETTYPE:
      {
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, SCHEMERLICHT_GETARG_A(i), schemerlicht_object);
      const int b = SCHEMERLICHT_GETARG_B(i);
      target->type = b;
      break;
      }
      case SCHEMERLICHT_OPCODE_CALL:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int b = SCHEMERLICHT_GETARG_B(i);
      const int c = SCHEMERLICHT_GETARG_C(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      if (target->type == schemerlicht_object_type_primitive)
        {
        schemerlicht_fixnum function_id = target->value.fx;
        inline_functions(a, b, c);
        }
      else if (target->type == schemerlicht_object_type_primitive_object)
        {
        schemerlicht_fixnum function_id = target->value.fx;
        //const int b = SCHEMERLICHT_GETARG_B(i);
        //const int c = SCHEMERLICHT_GETARG_C(i);
        inline_functions(a, b - 1, c + 1);
        //schemerlicht_call_primitive(ctxt, function_id, a, b - 1, c + 1); // skip the closure argument        
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
        fun = lambda;
        schemerlicht_check_garbage_collection(ctxt);
        }
      else if (target->type == schemerlicht_object_type_closure)
        {
        schemerlicht_assert(a == 0); // closures restart the stack
        schemerlicht_object* lambda_obj = schemerlicht_vector_at(&target->value.v, 0, schemerlicht_object);
        schemerlicht_assert(lambda_obj->type == schemerlicht_object_type_lambda);
        schemerlicht_function* lambda = cast(schemerlicht_function*, lambda_obj->value.ptr);
        //at R(0) we expect the closure (i.e. target) but this already so by compile_funcall
        //schemerlicht_set_object(schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object), target); 
        pc = schemerlicht_vector_begin(&lambda->code, schemerlicht_instruction);
        pc_end = schemerlicht_vector_end(&lambda->code, schemerlicht_instruction);
        fun = lambda;
        schemerlicht_check_garbage_collection(ctxt);
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
      case SCHEMERLICHT_OPCODE_LIST_STACK:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      int x = a;
      schemerlicht_object* rx = schemerlicht_vector_at(&ctxt->stack, x, schemerlicht_object);
      while (rx->type != schemerlicht_object_type_blocking)
        {
        ++x;
        rx = schemerlicht_vector_at(&ctxt->stack, x, schemerlicht_object);
        }
      make_variable_arity_list(ctxt, a, x - a);
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
      const int b = SCHEMERLICHT_GETARG_B(i);
      if (a != 0)
        {
#if 1
        for (int j = 0; j < b; ++j)
          {
          schemerlicht_object* retj = schemerlicht_vector_at(&ctxt->stack, j, schemerlicht_object);
          schemerlicht_object* srcj = schemerlicht_vector_at(&ctxt->stack, a + j, schemerlicht_object);
          schemerlicht_set_object(retj, srcj);
          }
#else
        schemerlicht_object* st = cast(schemerlicht_object*, ctxt->stack.vector_ptr);
        memcpy(st, st + a, b * sizeof(schemerlicht_object));
#endif
        }
      schemerlicht_object* stack_to_block = schemerlicht_vector_at(&ctxt->stack, b, schemerlicht_object);
      stack_to_block->type = schemerlicht_object_type_blocking;
      pc = pc_end;
      schemerlicht_check_garbage_collection(ctxt);
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
    schemerlicht_string tmp = instruction_to_string(ctxt, i);
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

schemerlicht_string schemerlicht_show_stack(schemerlicht_context* ctxt, int stack_start, int stack_end)
  {
  char buffer[256];
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "SCHEMERLICHT STACK:\n");
  for (int i = stack_start; i <= stack_end; ++i)
    {
    sprintf(buffer, "%d", i);
    schemerlicht_string_append_cstr(ctxt, &s, "  ");
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ": ");
    schemerlicht_object* stack_item = schemerlicht_vector_at(&ctxt->stack, i, schemerlicht_object);
    schemerlicht_string tmp = schemerlicht_object_to_string(ctxt, stack_item);
    schemerlicht_string_append(ctxt, &s, &tmp);
    schemerlicht_string_destroy(ctxt, &tmp);
    schemerlicht_string_push_back(ctxt, &s, '\n');
    }
  return s;
  }
