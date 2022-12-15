#include "vm.h"
#include "context.h"
#include "error.h"
#include "map.h"
#include "primitives.h"
#include "gc.h"
#include "foreign.h"
#include "environment.h"
#include "syscalls.h"
#include "limits.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

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
    schemerlicht_int_to_char(buffer, a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := R(");
    schemerlicht_int_to_char(buffer, b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")");
    break;
    }
    case SCHEMERLICHT_OPCODE_LOADGLOBAL:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_Bx(i);
    schemerlicht_string_append_cstr(ctxt, &s, "LOADGLOBAL R(");
    schemerlicht_int_to_char(buffer, a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := GLOBAL(");
    schemerlicht_int_to_char(buffer, b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")");
    break;
    }
    case SCHEMERLICHT_OPCODE_STOREGLOBAL:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_Bx(i);
    schemerlicht_string_append_cstr(ctxt, &s, "STOREGLOBAL GLOBAL(");
    schemerlicht_int_to_char(buffer, b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := R(");
    schemerlicht_int_to_char(buffer, a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")");
    break;
    }
    case SCHEMERLICHT_OPCODE_MOVETOP:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_B(i);
    schemerlicht_string_append_cstr(ctxt, &s, "MOVETOP R(0)..R(");
    schemerlicht_int_to_char(buffer, b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := R(");
    schemerlicht_int_to_char(buffer, a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")..R(");
    schemerlicht_int_to_char(buffer, a + b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")   R(");
    schemerlicht_int_to_char(buffer, b + 1);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")->type = blocking");
    break;
    }
    case SCHEMERLICHT_OPCODE_LOADK:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_Bx(i);
    schemerlicht_string_append_cstr(ctxt, &s, "LOADK R(");
    schemerlicht_int_to_char(buffer, a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := K(");
    schemerlicht_int_to_char(buffer, b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")");
    break;
    }
    case SCHEMERLICHT_OPCODE_SETPRIM:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_B(i);
    schemerlicht_string_append_cstr(ctxt, &s, "SETPRIM R(");
    schemerlicht_int_to_char(buffer, a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := ");
    schemerlicht_int_to_char(buffer, b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    break;
    }
    case SCHEMERLICHT_OPCODE_SETPRIMOBJ:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_B(i);
    schemerlicht_string_append_cstr(ctxt, &s, "SETPRIMOBJ R(");
    schemerlicht_int_to_char(buffer, a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := ");
    schemerlicht_int_to_char(buffer, b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    break;
    }
    case SCHEMERLICHT_OPCODE_SETFIXNUM:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_sBx(i);
    schemerlicht_string_append_cstr(ctxt, &s, "SETFIXNUM R(");
    schemerlicht_int_to_char(buffer, a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := ");
    schemerlicht_int_to_char(buffer, b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    break;
    }
    case SCHEMERLICHT_OPCODE_SETCHAR:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_B(i);
    schemerlicht_string_append_cstr(ctxt, &s, "SETCHAR R(");
    schemerlicht_int_to_char(buffer, a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := ");
    schemerlicht_int_to_char(buffer, b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    break;
    }
    case SCHEMERLICHT_OPCODE_SETTYPE:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_B(i);
    schemerlicht_string_append_cstr(ctxt, &s, "SETTYPE R(");
    schemerlicht_int_to_char(buffer, a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ").type := ");
    schemerlicht_int_to_char(buffer, b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    break;
    }
    case SCHEMERLICHT_OPCODE_LIST_STACK:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    schemerlicht_string_append_cstr(ctxt, &s, "LISTSTACK R(");
    schemerlicht_int_to_char(buffer, a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := (list R(A) R(A+1) ... R(A+x)), list until R(A+x+1).type == schemerlicht_object_type_blocked");
    break;
    }
    case SCHEMERLICHT_OPCODE_CALL:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    schemerlicht_string_append_cstr(ctxt, &s, "CALL R(");
    schemerlicht_int_to_char(buffer, a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")");
    break;
    }
    case SCHEMERLICHT_OPCODE_EQTYPE:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    const int b = SCHEMERLICHT_GETARG_B(i);
    schemerlicht_string_append_cstr(ctxt, &s, "EQTYPE if R(");
    schemerlicht_int_to_char(buffer, a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")->type == ");
    schemerlicht_int_to_char(buffer, b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, " then skip next line");
    break;
    }
    case SCHEMERLICHT_OPCODE_JMP:
    {
    int b = SCHEMERLICHT_GETARG_sBx(i);
    schemerlicht_string_append_cstr(ctxt, &s, "JUMP ");
    schemerlicht_int_to_char(buffer, b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    break;
    }
    case SCHEMERLICHT_OPCODE_RETURN:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    schemerlicht_string_append_cstr(ctxt, &s, "RETURN R(");
    schemerlicht_int_to_char(buffer, a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")");
    break;
    }
    case SCHEMERLICHT_OPCODE_CALL_FOREIGN:
    {
    const int a = SCHEMERLICHT_GETARG_A(i);
    //const int b = SCHEMERLICHT_GETARG_B(i);
    schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
    schemerlicht_assert(schemerlicht_object_get_type(ra) == schemerlicht_object_type_fixnum);
    const schemerlicht_memsize position = cast(schemerlicht_memsize, ra->value.fx);
    schemerlicht_assert(position < ctxt->externals.vector_size);
    schemerlicht_external_function* ext = schemerlicht_vector_at(&ctxt->externals, position, schemerlicht_external_function);
    schemerlicht_string_append_cstr(ctxt, &s, "CALL FOREIGN R(");
    schemerlicht_int_to_char(buffer, a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") = ");
    schemerlicht_string_append(ctxt, &s, &ext->name);
    break;
    }
    default:
      break;
    }
  return s;
  }

static schemerlicht_string debug_object_to_string(schemerlicht_context* ctxt, schemerlicht_object* obj)
  {
  schemerlicht_string res = schemerlicht_object_to_string(ctxt, obj, 0);
  if (schemerlicht_object_get_type(obj) == schemerlicht_object_type_lambda)
    {
    char buffer[256];
    uint64_t ptr = cast(uint64_t, obj->value.ptr);
    schemerlicht_fixnum_to_char(buffer, cast(schemerlicht_fixnum, ptr));
    schemerlicht_string_append_cstr(ctxt, &res, ": ");
    schemerlicht_string_append_cstr(ctxt, &res, buffer);
    }
  if (schemerlicht_object_get_type(obj) == schemerlicht_object_type_primitive)
    {
    char buffer[256];
    schemerlicht_fixnum_to_char(buffer, obj->value.fx);
    schemerlicht_string_append_cstr(ctxt, &res, ": ");
    schemerlicht_string_append_cstr(ctxt, &res, buffer);
    }
  if (schemerlicht_object_get_type(obj) == schemerlicht_object_type_primitive_object)
    {
    char buffer[256];
    schemerlicht_fixnum_to_char(buffer, obj->value.fx);
    schemerlicht_string_append_cstr(ctxt, &res, ": (as object) ");
    schemerlicht_string_append_cstr(ctxt, &res, buffer);
    }
  return res;
  }

#if SCHEMERLICHT_DISPATCH_TYPE == SCHEMERLICHT_DIRECT_CALL_THREADING || SCHEMERLICHT_DISPATCH_TYPE == SCHEMERLICHT_SWITCH_FUN_DISPATCHER

typedef void (*opcode_fun)(schemerlicht_context*, schemerlicht_instruction**, schemerlicht_instruction**, schemerlicht_function**);

static void opcode_move(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun);
static void opcode_loadk(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun);
static void opcode_setfixnum(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun);
static void opcode_setchar(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun);
static void opcode_setprim(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun);
static void opcode_setprimobj(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun);
static void opcode_settype(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun);
static void opcode_movetop(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun);
static void opcode_call(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun);
static void opcode_eqtype(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun);
static void opcode_jmp(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun);
static void opcode_return(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun);
static void opcode_loadglobal(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun);
static void opcode_storeglobal(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun);
static void opcode_liststack(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun);
static void opcode_foreigncall(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun);


static opcode_fun dispatch_table[] = { &opcode_move, &opcode_loadk, &opcode_setfixnum, &opcode_setchar, &opcode_setprim, &opcode_setprimobj,
&opcode_settype, &opcode_movetop, &opcode_call, &opcode_eqtype, &opcode_jmp, &opcode_return, &opcode_loadglobal, &opcode_storeglobal,
&opcode_liststack, &opcode_foreigncall };

static void opcode_move(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun)
  {
  UNUSED(fun);
  UNUSED(pc_end);
  const schemerlicht_instruction i = *(*pc)++;
  schemerlicht_assert(SCHEMERLICHT_GET_OPCODE(i) == SCHEMERLICHT_OPCODE_MOVE);
  const int a = SCHEMERLICHT_GETARG_A(i);
  const int b = SCHEMERLICHT_GETARG_B(i);
  schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  const schemerlicht_object* source = schemerlicht_vector_at(&ctxt->stack, b, schemerlicht_object);
  schemerlicht_set_object(target, source);
  }

static void opcode_movetop(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun)
  {
  UNUSED(fun);
  UNUSED(pc_end);
  const schemerlicht_instruction i = *(*pc)++;
  schemerlicht_assert(SCHEMERLICHT_GET_OPCODE(i) == SCHEMERLICHT_OPCODE_MOVETOP);
  const int a = SCHEMERLICHT_GETARG_A(i);
  const int b = SCHEMERLICHT_GETARG_B(i);
  for (int j = 0; j <= b; ++j)
    {
    schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, j, schemerlicht_object);
    const schemerlicht_object* source = schemerlicht_vector_at(&ctxt->stack, a + j, schemerlicht_object);
    schemerlicht_set_object(target, source);
    }
  schemerlicht_object* blocking = schemerlicht_vector_at(&ctxt->stack, b + 1, schemerlicht_object);
  blocking->type = schemerlicht_object_type_blocking;
  }

static void opcode_loadglobal(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun)
  {
  UNUSED(fun);
  UNUSED(pc_end);
  const schemerlicht_instruction i = *(*pc)++;
  schemerlicht_assert(SCHEMERLICHT_GET_OPCODE(i) == SCHEMERLICHT_OPCODE_LOADGLOBAL);
  const int a = SCHEMERLICHT_GETARG_A(i);
  const int bx = SCHEMERLICHT_GETARG_Bx(i);
  schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  const schemerlicht_object* global = schemerlicht_vector_at(&ctxt->globals, bx, schemerlicht_object);
  schemerlicht_set_object(target, global);
  }

static void opcode_storeglobal(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun)
  {
  UNUSED(fun);
  UNUSED(pc_end);
  const schemerlicht_instruction i = *(*pc)++;
  schemerlicht_assert(SCHEMERLICHT_GET_OPCODE(i) == SCHEMERLICHT_OPCODE_STOREGLOBAL);
  const int a = SCHEMERLICHT_GETARG_A(i);
  const int bx = SCHEMERLICHT_GETARG_Bx(i);
  const schemerlicht_object* source = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_object* global = schemerlicht_vector_at(&ctxt->globals, bx, schemerlicht_object);
  schemerlicht_set_object(global, source);
  }

static void opcode_loadk(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun)
  {
  UNUSED(fun);
  UNUSED(pc_end);
  const schemerlicht_instruction i = *(*pc)++;
  schemerlicht_assert(SCHEMERLICHT_GET_OPCODE(i) == SCHEMERLICHT_OPCODE_LOADK);
  const int a = SCHEMERLICHT_GETARG_A(i);
  const int bx = SCHEMERLICHT_GETARG_Bx(i);
  schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  const schemerlicht_object* k = schemerlicht_vector_at(&(*fun)->constants, bx, schemerlicht_object);
  if (schemerlicht_object_get_type(k) == schemerlicht_object_type_string)
    {
    target->type = schemerlicht_object_type_string;
    schemerlicht_string_copy(ctxt, &target->value.s, &k->value.s);
    schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
    schemerlicht_set_object(heap_obj, target);
    ++ctxt->heap_pos;
    }
  else
    {
    schemerlicht_set_object(target, k);
    }
  }

static void opcode_setfixnum(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun)
  {
  UNUSED(fun);
  UNUSED(pc_end);
  const schemerlicht_instruction i = *(*pc)++;
  schemerlicht_assert(SCHEMERLICHT_GET_OPCODE(i) == SCHEMERLICHT_OPCODE_SETFIXNUM);
  const int a = SCHEMERLICHT_GETARG_A(i);
  schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  const int b = SCHEMERLICHT_GETARG_sBx(i);
  target->type = schemerlicht_object_type_fixnum;
  target->value.fx = b;
  }

static void opcode_setprim(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun)
  {
  UNUSED(fun);
  UNUSED(pc_end);
  const schemerlicht_instruction i = *(*pc)++;
  schemerlicht_assert(SCHEMERLICHT_GET_OPCODE(i) == SCHEMERLICHT_OPCODE_SETPRIM);
  const int a = SCHEMERLICHT_GETARG_A(i);
  schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  const int b = SCHEMERLICHT_GETARG_B(i);
  target->type = schemerlicht_object_type_primitive;
  target->value.fx = b;
  }

static void opcode_setprimobj(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun)
  {
  UNUSED(fun);
  UNUSED(pc_end);
  const schemerlicht_instruction i = *(*pc)++;
  schemerlicht_assert(SCHEMERLICHT_GET_OPCODE(i) == SCHEMERLICHT_OPCODE_SETPRIMOBJ);
  const int a = SCHEMERLICHT_GETARG_A(i);
  schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  const int b = SCHEMERLICHT_GETARG_B(i);
  target->type = schemerlicht_object_type_primitive_object;
  target->value.fx = b;
  }

static void opcode_setchar(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun)
  {
  UNUSED(fun);
  UNUSED(pc_end);
  const schemerlicht_instruction i = *(*pc)++;
  schemerlicht_assert(SCHEMERLICHT_GET_OPCODE(i) == SCHEMERLICHT_OPCODE_SETCHAR);
  const int a = SCHEMERLICHT_GETARG_A(i);
  schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  const int b = SCHEMERLICHT_GETARG_B(i);
  target->type = schemerlicht_object_type_char;
  target->value.ch = cast(schemerlicht_byte, b);
  }

static void opcode_settype(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun)
  {
  UNUSED(fun);
  UNUSED(pc_end);
  const schemerlicht_instruction i = *(*pc)++;
  schemerlicht_assert(SCHEMERLICHT_GET_OPCODE(i) == SCHEMERLICHT_OPCODE_SETTYPE);
  schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, SCHEMERLICHT_GETARG_A(i), schemerlicht_object);
  const int b = SCHEMERLICHT_GETARG_B(i);
  target->type = b;
  }

static void opcode_call(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun)
  {
  const schemerlicht_instruction i = *(*pc)++;
  schemerlicht_assert(SCHEMERLICHT_GET_OPCODE(i) == SCHEMERLICHT_OPCODE_CALL);
  const int a = SCHEMERLICHT_GETARG_A(i);
  const int b = SCHEMERLICHT_GETARG_B(i);
  const int c = SCHEMERLICHT_GETARG_C(i);
  schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  if (schemerlicht_object_get_type(target) == schemerlicht_object_type_unassigned)
    {
    //this should be a global defined function, but at the time of calling, the function wasn't created yet.
    //let's see whether its global position has been updated.
    target = schemerlicht_vector_at(&ctxt->globals, target->value.fx, schemerlicht_object);
    }
  switch (schemerlicht_object_get_type(target))
    {
    case schemerlicht_object_type_primitive:
    {
    const schemerlicht_fixnum function_id = target->value.fx;
#ifdef SCHEMERLICHT_USE_INLINES    
#include "inlines.h"
#else
    schemerlicht_call_primitive(ctxt, function_id, a, b, c);
#endif
    break;
    }
    case schemerlicht_object_type_primitive_object:
    {
    const schemerlicht_fixnum function_id = target->value.fx;
#ifdef SCHEMERLICHT_USE_INLINES
    --b;
    ++c;
#include "inlines.h"
#else
    schemerlicht_call_primitive(ctxt, function_id, a, b - 1, c + 1);
#endif
    //schemerlicht_call_primitive(ctxt, function_id, a, b - 1, c + 1); // skip the closure argument        
    schemerlicht_object continuation = *schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
    schemerlicht_assert(schemerlicht_object_get_type(&continuation) == schemerlicht_object_type_closure || schemerlicht_object_get_type(&continuation) == schemerlicht_object_type_lambda);
    schemerlicht_object result_of_prim_call = *schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
    schemerlicht_object* r0 = schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
    schemerlicht_object* r1 = schemerlicht_vector_at(&ctxt->stack, 1, schemerlicht_object);
    schemerlicht_set_object(r0, &continuation);
    schemerlicht_set_object(r1, &result_of_prim_call);
    if (schemerlicht_object_get_type(&continuation) == schemerlicht_object_type_closure)
      {
      const schemerlicht_object* lambda_obj = schemerlicht_vector_at(&continuation.value.v, 0, schemerlicht_object);
      schemerlicht_assert(schemerlicht_object_get_type(lambda_obj) == schemerlicht_object_type_lambda);
      schemerlicht_function* lambda = cast(schemerlicht_function*, lambda_obj->value.ptr);
      *pc = schemerlicht_vector_begin(&lambda->code, schemerlicht_instruction);
      *pc_end = schemerlicht_vector_end(&lambda->code, schemerlicht_instruction);
      *fun = lambda;
      }
    else
      {
      schemerlicht_assert(schemerlicht_object_get_type(&continuation) == schemerlicht_object_type_lambda);
      schemerlicht_function* lambda = cast(schemerlicht_function*, continuation.value.ptr);
      *pc = schemerlicht_vector_begin(&lambda->code, schemerlicht_instruction);
      *pc_end = schemerlicht_vector_end(&lambda->code, schemerlicht_instruction);
      *fun = lambda;
      }
    schemerlicht_check_garbage_collection(ctxt);
    break;
    }
    case schemerlicht_object_type_closure:
    {
    schemerlicht_assert(a == 0); // closures restart the stack
    const schemerlicht_object* lambda_obj = schemerlicht_vector_at(&target->value.v, 0, schemerlicht_object);
    schemerlicht_assert(schemerlicht_object_get_type(lambda_obj) == schemerlicht_object_type_lambda);
    schemerlicht_function* lambda = cast(schemerlicht_function*, lambda_obj->value.ptr);
    //at R(0) we expect the closure (i.e. target) but this already so by compile_funcall
    //schemerlicht_set_object(schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object), target); 
    *pc = schemerlicht_vector_begin(&lambda->code, schemerlicht_instruction);
    *pc_end = schemerlicht_vector_end(&lambda->code, schemerlicht_instruction);
    *fun = lambda;
    schemerlicht_check_garbage_collection(ctxt);
    break;
    }
    case schemerlicht_object_type_lambda:
    {
    schemerlicht_assert(a == 0); // closures restart the stack
    schemerlicht_function* lambda = cast(schemerlicht_function*, target->value.ptr);
    *pc = schemerlicht_vector_begin(&lambda->code, schemerlicht_instruction);
    *pc_end = schemerlicht_vector_end(&lambda->code, schemerlicht_instruction);
    *fun = lambda;
    schemerlicht_check_garbage_collection(ctxt);
    break;
    }
    default:
    {
    schemerlicht_string error_message;
    schemerlicht_string_init(ctxt, &error_message, "attempt to call a non-procedure");
    if (schemerlicht_object_get_type(target) == schemerlicht_object_type_unassigned)
      {
      schemerlicht_object* key = schemerlicht_environment_find_key_given_position(ctxt, target->value.fx);
      if (key != NULL)
        {
        schemerlicht_string_append_cstr(ctxt, &error_message, ": ");
        schemerlicht_string_append(ctxt, &error_message, &key->value.s);
        }
      }
    else
      {
      schemerlicht_string tmp = schemerlicht_object_to_string(ctxt, target, 0);
      schemerlicht_string_append_cstr(ctxt, &error_message, ": ");
      schemerlicht_string_append(ctxt, &error_message, &tmp);
      schemerlicht_string_destroy(ctxt, &tmp);
      }
    schemerlicht_runtime_error(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, -1, -1, &error_message);
    *pc = *pc_end;
    }
    }
  }

static void opcode_eqtype(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun)
  {
  UNUSED(fun);
  UNUSED(pc_end);
  const schemerlicht_instruction i = *(*pc)++;
  const int a = SCHEMERLICHT_GETARG_A(i);
  const int b = SCHEMERLICHT_GETARG_B(i);
  const schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  if (schemerlicht_object_get_type(ra) == b)
    {
    ++(*pc);
    }
  else
    {
    const schemerlicht_instruction next_i = *(*pc)++;
    schemerlicht_assert(SCHEMERLICHT_GET_OPCODE(next_i) == SCHEMERLICHT_OPCODE_JMP);
    const int offset = SCHEMERLICHT_GETARG_sBx(next_i);
    *pc += offset;
    }
  }

static void opcode_liststack(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun)
  {
  UNUSED(fun);
  UNUSED(pc_end);
  const schemerlicht_instruction i = *(*pc)++;
  schemerlicht_assert(SCHEMERLICHT_GET_OPCODE(i) == SCHEMERLICHT_OPCODE_LIST_STACK);
  const int a = SCHEMERLICHT_GETARG_A(i);
  int x = a;
  schemerlicht_object* rx = schemerlicht_vector_at(&ctxt->stack, x, schemerlicht_object);
  while (schemerlicht_object_get_type(rx) != schemerlicht_object_type_blocking)
    {
    ++x;
    rx = schemerlicht_vector_at(&ctxt->stack, x, schemerlicht_object);
    }
  make_variable_arity_list(ctxt, a, x - a);
  }

static void opcode_foreigncall(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun)
  {
  UNUSED(fun);
  UNUSED(pc_end);
  const schemerlicht_instruction i = *(*pc)++;
  schemerlicht_assert(SCHEMERLICHT_GET_OPCODE(i) == SCHEMERLICHT_OPCODE_CALL_FOREIGN);
  const int a = SCHEMERLICHT_GETARG_A(i);
  const int b = SCHEMERLICHT_GETARG_B(i);
  schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
  schemerlicht_assert(schemerlicht_object_get_type(ra) == schemerlicht_object_type_fixnum);
  const schemerlicht_memsize position = cast(schemerlicht_memsize, ra->value.fx);
  schemerlicht_assert(position < ctxt->externals.vector_size);
  schemerlicht_external_function* ext = schemerlicht_vector_at(&ctxt->externals, position, schemerlicht_external_function);
  //schemerlicht_assert(ext->arguments.vector_size == cast(schemerlicht_memsize, b));
  schemerlicht_object result = schemerlicht_call_external(ctxt, ext, a + 1, b);
  schemerlicht_set_object(ra, &result);
  }

static void opcode_jmp(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun)
  {
  UNUSED(fun);
  UNUSED(pc_end);
  UNUSED(ctxt);
  const schemerlicht_instruction i = *(*pc)++;
  schemerlicht_assert(SCHEMERLICHT_GET_OPCODE(i) == SCHEMERLICHT_OPCODE_JMP);
  const int sbx = SCHEMERLICHT_GETARG_sBx(i);
  *pc += sbx;
  }

static void opcode_return(schemerlicht_context* ctxt, schemerlicht_instruction** pc, schemerlicht_instruction** pc_end, schemerlicht_function** fun)
  {
  UNUSED(fun);
  const schemerlicht_instruction i = *(*pc)++;
  schemerlicht_assert(SCHEMERLICHT_GET_OPCODE(i) == SCHEMERLICHT_OPCODE_RETURN);
  const int a = SCHEMERLICHT_GETARG_A(i);
  const int b = SCHEMERLICHT_GETARG_B(i);
  if (a != 0)
    {
    for (int j = 0; j < b; ++j)
      {
      schemerlicht_object* retj = schemerlicht_vector_at(&ctxt->stack, j, schemerlicht_object);
      const schemerlicht_object* srcj = schemerlicht_vector_at(&ctxt->stack, a + j, schemerlicht_object);
      schemerlicht_set_object(retj, srcj);
      }
    }
  schemerlicht_object* stack_to_block = schemerlicht_vector_at(&ctxt->stack, b, schemerlicht_object);
  stack_to_block->type = schemerlicht_object_type_blocking;
  *pc = *pc_end;
  schemerlicht_check_garbage_collection(ctxt);
  }

#endif

#if SCHEMERLICHT_DISPATCH_TYPE == SCHEMERLICHT_DIRECT_CALL_THREADING

schemerlicht_object* schemerlicht_run(schemerlicht_context* ctxt, schemerlicht_function* fun)
  {
  schemerlicht_assert(fun != NULL);
  schemerlicht_instruction* pc = schemerlicht_vector_begin(&(fun)->code, schemerlicht_instruction);
  schemerlicht_instruction* pc_end = schemerlicht_vector_end(&(fun)->code, schemerlicht_instruction);
  while (pc < pc_end)
    {
    const schemerlicht_instruction i = *pc;
    const int opcode = SCHEMERLICHT_GET_OPCODE(i);
    (*dispatch_table[opcode])(ctxt, &pc, &pc_end, &fun);
    }
  return schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
  }

#elif SCHEMERLICHT_DISPATCH_TYPE == SCHEMERLICHT_SWITCH_FUN_DISPATCHER

schemerlicht_object* schemerlicht_run(schemerlicht_context* ctxt, schemerlicht_function* fun)
  {
  schemerlicht_assert(fun != NULL);
  schemerlicht_instruction* pc = schemerlicht_vector_begin(&(fun)->code, schemerlicht_instruction);
  schemerlicht_instruction* pc_end = schemerlicht_vector_end(&(fun)->code, schemerlicht_instruction);
  while (pc < pc_end)
    {
#ifdef SCHEMERLICHT_CHECK_HEAP_OVERFLOW
    if (ctxt->heap_pos >= ctxt->raw_heap.vector_size / 2)
      {
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_MEMORY, -1, -1, "Heap is full!");
      break;
      }
#endif
    const schemerlicht_instruction i = *pc;
    const int opcode = SCHEMERLICHT_GET_OPCODE(i);
    schemerlicht_assert((opcode == SCHEMERLICHT_OPCODE_JMP) || (SCHEMERLICHT_GETARG_A(i) < schemerlicht_maxstack));
    switch (opcode)
      {
      case SCHEMERLICHT_OPCODE_MOVE:
      {
      opcode_move(ctxt, &pc, &pc_end, &fun);
      break;
      }
      case SCHEMERLICHT_OPCODE_MOVETOP:
      {
      opcode_movetop(ctxt, &pc, &pc_end, &fun);
      break;
      }
      case SCHEMERLICHT_OPCODE_LOADGLOBAL:
      {
      opcode_loadglobal(ctxt, &pc, &pc_end, &fun);
      break;
      }
      case SCHEMERLICHT_OPCODE_STOREGLOBAL:
      {
      opcode_storeglobal(ctxt, &pc, &pc_end, &fun);
      break;
      }
      case SCHEMERLICHT_OPCODE_LOADK:
      {
      opcode_loadk(ctxt, &pc, &pc_end, &fun);
      break;
      }
      case SCHEMERLICHT_OPCODE_SETFIXNUM:
      {
      opcode_setfixnum(ctxt, &pc, &pc_end, &fun);
      break;
      }
      case SCHEMERLICHT_OPCODE_SETPRIM:
      {
      opcode_setprim(ctxt, &pc, &pc_end, &fun);
      break;
      }
      case SCHEMERLICHT_OPCODE_SETPRIMOBJ:
      {
      opcode_setprimobj(ctxt, &pc, &pc_end, &fun);
      break;
      }
      case SCHEMERLICHT_OPCODE_SETCHAR:
      {
      opcode_setchar(ctxt, &pc, &pc_end, &fun);
      break;
      }
      case SCHEMERLICHT_OPCODE_SETTYPE:
      {
      opcode_settype(ctxt, &pc, &pc_end, &fun);
      break;
      }
      case SCHEMERLICHT_OPCODE_CALL:
      {
      opcode_call(ctxt, &pc, &pc_end, &fun);
      break;
      }
      case SCHEMERLICHT_OPCODE_EQTYPE:
      {
      opcode_eqtype(ctxt, &pc, &pc_end, &fun);
      break;
      }
      case SCHEMERLICHT_OPCODE_LIST_STACK:
      {
      opcode_liststack(ctxt, &pc, &pc_end, &fun);
      break;
      }
      case SCHEMERLICHT_OPCODE_CALL_FOREIGN:
      {
      opcode_foreigncall(ctxt, &pc, &pc_end, &fun);
      break;
      }
      case SCHEMERLICHT_OPCODE_JMP:
      {
      opcode_jmp(ctxt, &pc, &pc_end, &fun);
      break;
      }
      case SCHEMERLICHT_OPCODE_RETURN:
      {
      opcode_return(ctxt, &pc, &pc_end, &fun);
      break;
      }
      default:
        schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
      }
    }
  return schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
  }

#elif SCHEMERLICHT_DISPATCH_TYPE == SCHEMERLICHT_SWITCH_DISPATCHER

schemerlicht_object* schemerlicht_run(schemerlicht_context* ctxt, schemerlicht_function* fun)
  {
  schemerlicht_assert(fun != NULL);
  const schemerlicht_instruction* pc = schemerlicht_vector_begin(&(fun)->code, schemerlicht_instruction);
  const schemerlicht_instruction* pc_end = schemerlicht_vector_end(&(fun)->code, schemerlicht_instruction);
  while (pc < pc_end)
    {
#ifdef SCHEMERLICHT_CHECK_HEAP_OVERFLOW
    if (ctxt->heap_pos >= ctxt->raw_heap.vector_size / 2)
      {
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_MEMORY, -1, -1, "Heap is full!");
      break;
      }
#endif
    const schemerlicht_instruction i = *pc++;
    const int opcode = SCHEMERLICHT_GET_OPCODE(i);
    schemerlicht_assert((opcode == SCHEMERLICHT_OPCODE_JMP) || (SCHEMERLICHT_GETARG_A(i) < schemerlicht_maxstack));
    switch (opcode & 15)
      {
      case SCHEMERLICHT_OPCODE_MOVE:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int b = SCHEMERLICHT_GETARG_B(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      const schemerlicht_object* source = schemerlicht_vector_at(&ctxt->stack, b, schemerlicht_object);
      schemerlicht_set_object(target, source);
      break;
      }
      case SCHEMERLICHT_OPCODE_MOVETOP:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int b = SCHEMERLICHT_GETARG_B(i);
      for (int j = 0; j <= b; ++j)
        {
        schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, j, schemerlicht_object);
        const schemerlicht_object* source = schemerlicht_vector_at(&ctxt->stack, a + j, schemerlicht_object);
        schemerlicht_set_object(target, source);
        }
      schemerlicht_object* blocking = schemerlicht_vector_at(&ctxt->stack, b + 1, schemerlicht_object);
      blocking->type = schemerlicht_object_type_blocking;
      break;
      }
      case SCHEMERLICHT_OPCODE_LOADGLOBAL:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int bx = SCHEMERLICHT_GETARG_Bx(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      const schemerlicht_object* global = schemerlicht_vector_at(&ctxt->globals, bx, schemerlicht_object);
      schemerlicht_set_object(target, global);
      break;
      }
      case SCHEMERLICHT_OPCODE_STOREGLOBAL:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int bx = SCHEMERLICHT_GETARG_Bx(i);
      const schemerlicht_object* source = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      schemerlicht_object* global = schemerlicht_vector_at(&ctxt->globals, bx, schemerlicht_object);
      schemerlicht_set_object(global, source);
      break;
      }
      case SCHEMERLICHT_OPCODE_LOADK:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int bx = SCHEMERLICHT_GETARG_Bx(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      const schemerlicht_object* k = schemerlicht_vector_at(&(fun)->constants, bx, schemerlicht_object);
      if (schemerlicht_object_get_type(k) == schemerlicht_object_type_string)
        {
        target->type = schemerlicht_object_type_string;
        schemerlicht_string_copy(ctxt, &target->value.s, &k->value.s);
        schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
        schemerlicht_set_object(heap_obj, target);
        ++ctxt->heap_pos;
        }
      else
        {
        schemerlicht_set_object(target, k);
        }
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
      int b = SCHEMERLICHT_GETARG_B(i);
      int c = SCHEMERLICHT_GETARG_C(i);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      if (schemerlicht_object_get_type(target) == schemerlicht_object_type_unassigned)
        {
        //this should be a global defined function, but at the time of calling, the function wasn't created yet.
        //let's see whether its global position has been updated.
        target = schemerlicht_vector_at(&ctxt->globals, target->value.fx, schemerlicht_object);
        }
      switch (schemerlicht_object_get_type(target))
        {
        case schemerlicht_object_type_primitive:
        {
        const schemerlicht_fixnum function_id = target->value.fx;
#ifdef SCHEMERLICHT_USE_INLINES        
#include "inlines.h"
#else
        schemerlicht_call_primitive(ctxt, function_id, a, b, c);
#endif
        break;
        }
        case schemerlicht_object_type_primitive_object:
        {
        const schemerlicht_fixnum function_id = target->value.fx;
#ifdef SCHEMERLICHT_USE_INLINES
        --b;
        ++c;
#include "inlines.h"
#else
        schemerlicht_call_primitive(ctxt, function_id, a, b - 1, c + 1);
#endif
        //schemerlicht_call_primitive(ctxt, function_id, a, b - 1, c + 1); // skip the closure argument        
        schemerlicht_object continuation = *schemerlicht_vector_at(&ctxt->stack, a + 1, schemerlicht_object);
        schemerlicht_assert(schemerlicht_object_get_type(&continuation) == schemerlicht_object_type_closure || schemerlicht_object_get_type(&continuation) == schemerlicht_object_type_lambda);
        schemerlicht_object result_of_prim_call = *schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
        schemerlicht_object* r0 = schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
        schemerlicht_object* r1 = schemerlicht_vector_at(&ctxt->stack, 1, schemerlicht_object);
        schemerlicht_set_object(r0, &continuation);
        schemerlicht_set_object(r1, &result_of_prim_call);
        if (schemerlicht_object_get_type(&continuation) == schemerlicht_object_type_closure)
          {
          const schemerlicht_object* lambda_obj = schemerlicht_vector_at(&continuation.value.v, 0, schemerlicht_object);
          schemerlicht_assert(schemerlicht_object_get_type(lambda_obj) == schemerlicht_object_type_lambda);
          schemerlicht_function* lambda = cast(schemerlicht_function*, lambda_obj->value.ptr);
          pc = schemerlicht_vector_begin(&lambda->code, schemerlicht_instruction);
          pc_end = schemerlicht_vector_end(&lambda->code, schemerlicht_instruction);
          fun = lambda;
          }
        else
          {
          schemerlicht_assert(schemerlicht_object_get_type(&continuation) == schemerlicht_object_type_lambda);
          schemerlicht_function* lambda = cast(schemerlicht_function*, continuation.value.ptr);
          pc = schemerlicht_vector_begin(&lambda->code, schemerlicht_instruction);
          pc_end = schemerlicht_vector_end(&lambda->code, schemerlicht_instruction);
          fun = lambda;
          }
        schemerlicht_check_garbage_collection(ctxt);
        break;
        }
        case schemerlicht_object_type_closure:
        {
        schemerlicht_assert(a == 0); // closures restart the stack
        const schemerlicht_object* lambda_obj = schemerlicht_vector_at(&target->value.v, 0, schemerlicht_object);
        schemerlicht_assert(schemerlicht_object_get_type(lambda_obj) == schemerlicht_object_type_lambda);
        schemerlicht_function* lambda = cast(schemerlicht_function*, lambda_obj->value.ptr);
        //at R(0) we expect the closure (i.e. target) but this already so by compile_funcall
        //schemerlicht_set_object(schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object), target); 
        pc = schemerlicht_vector_begin(&lambda->code, schemerlicht_instruction);
        pc_end = schemerlicht_vector_end(&lambda->code, schemerlicht_instruction);
        fun = lambda;
        schemerlicht_check_garbage_collection(ctxt);
        break;
        }
        case schemerlicht_object_type_lambda:
        {
        schemerlicht_assert(a == 0); // closures restart the stack
        schemerlicht_function* lambda = cast(schemerlicht_function*, target->value.ptr);
        pc = schemerlicht_vector_begin(&lambda->code, schemerlicht_instruction);
        pc_end = schemerlicht_vector_end(&lambda->code, schemerlicht_instruction);
        fun = lambda;
        schemerlicht_check_garbage_collection(ctxt);
        break;
        }
        default:
        {
        schemerlicht_string error_message;
        schemerlicht_string_init(ctxt, &error_message, "attempt to call a non-procedure");
        if (schemerlicht_object_get_type(target) == schemerlicht_object_type_unassigned)
          {
          schemerlicht_object* key = schemerlicht_environment_find_key_given_position(ctxt, target->value.fx);
          if (key != NULL)
            {
            schemerlicht_string_append_cstr(ctxt, &error_message, ": ");
            schemerlicht_string_append(ctxt, &error_message, &key->value.s);
            }
          }
        else
          {
          schemerlicht_string tmp = schemerlicht_object_to_string(ctxt, target, 0);
          schemerlicht_string_append_cstr(ctxt, &error_message, ": ");
          schemerlicht_string_append(ctxt, &error_message, &tmp);
          schemerlicht_string_destroy(ctxt, &tmp);
          }
        schemerlicht_runtime_error(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, -1, -1, &error_message);
        pc = pc_end;
        }
        }
      break;
      }
      case SCHEMERLICHT_OPCODE_EQTYPE:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int b = SCHEMERLICHT_GETARG_B(i);
      const schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      if (schemerlicht_object_get_type(ra) == b)
        {
        ++pc;
        }
      else
        {
        const schemerlicht_instruction next_i = *pc++;
        schemerlicht_assert(SCHEMERLICHT_GET_OPCODE(next_i) == SCHEMERLICHT_OPCODE_JMP);
        const int offset = SCHEMERLICHT_GETARG_sBx(next_i);
        pc += offset;
        }
      break;
      }
      case SCHEMERLICHT_OPCODE_LIST_STACK:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      int x = a;
      schemerlicht_object* rx = schemerlicht_vector_at(&ctxt->stack, x, schemerlicht_object);
      while (schemerlicht_object_get_type(rx) != schemerlicht_object_type_blocking)
        {
        ++x;
        rx = schemerlicht_vector_at(&ctxt->stack, x, schemerlicht_object);
        }
      make_variable_arity_list(ctxt, a, x - a);
      break;
      }
      case SCHEMERLICHT_OPCODE_CALL_FOREIGN:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int b = SCHEMERLICHT_GETARG_B(i);
      schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      schemerlicht_assert(schemerlicht_object_get_type(ra) == schemerlicht_object_type_fixnum);
      const schemerlicht_memsize position = cast(schemerlicht_memsize, ra->value.fx);
      schemerlicht_assert(position < ctxt->externals.vector_size);
      schemerlicht_external_function* ext = schemerlicht_vector_at(&ctxt->externals, position, schemerlicht_external_function);
      //schemerlicht_assert(ext->arguments.vector_size == cast(schemerlicht_memsize, b));
      schemerlicht_object result = schemerlicht_call_external(ctxt, ext, a + 1, b);
      schemerlicht_set_object(ra, &result);
      break;
      }
      case SCHEMERLICHT_OPCODE_JMP:
      {
      const int sbx = SCHEMERLICHT_GETARG_sBx(i);
      pc += sbx;
      break;
      }
      case SCHEMERLICHT_OPCODE_RETURN:
      {
      const int a = SCHEMERLICHT_GETARG_A(i);
      const int b = SCHEMERLICHT_GETARG_B(i);
      if (a != 0)
        {
        for (int j = 0; j < b; ++j)
          {
          schemerlicht_object* retj = schemerlicht_vector_at(&ctxt->stack, j, schemerlicht_object);
          const schemerlicht_object* srcj = schemerlicht_vector_at(&ctxt->stack, a + j, schemerlicht_object);
          schemerlicht_set_object(retj, srcj);
          }
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
#endif


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

void schemerlicht_show_stack(schemerlicht_context* ctxt, schemerlicht_string* s, int stack_start, int stack_end)
  {
  char buffer[256];
  schemerlicht_string_append_cstr(ctxt, s, "SCHEMERLICHT STACK:\n");
  if (stack_end >= schemerlicht_maxstack)
    stack_end = schemerlicht_maxstack - 1;
  for (int i = stack_start; i <= stack_end; ++i)
    {
    schemerlicht_int_to_char(buffer, i);
    schemerlicht_string_append_cstr(ctxt, s, "  ");
    schemerlicht_string_append_cstr(ctxt, s, buffer);
    schemerlicht_string_append_cstr(ctxt, s, ": ");
    schemerlicht_object* stack_item = schemerlicht_vector_at(&ctxt->stack, i, schemerlicht_object);
    schemerlicht_string tmp = schemerlicht_object_to_string(ctxt, stack_item, 0);
    schemerlicht_string_append(ctxt, s, &tmp);
    schemerlicht_string_destroy(ctxt, &tmp);
#ifdef SCHEMERLICHT_DEBUG_LAMBDA_DEFINITION
    if (schemerlicht_object_get_type(stack_item) == schemerlicht_object_type_closure)
      {
      schemerlicht_object* lam = schemerlicht_vector_at(&stack_item->value.v, 0, schemerlicht_object);
      schemerlicht_function* fun = cast(schemerlicht_function*, lam->value.ptr);
      schemerlicht_string_append(ctxt, s, &fun->function_definition);
      }
#endif
    schemerlicht_string_push_back(ctxt, s, '\n');
    }  
  }

schemerlicht_object* schemerlicht_run_program(schemerlicht_context* ctxt, schemerlicht_vector* functions)
  {
  schemerlicht_function** it = schemerlicht_vector_begin(functions, schemerlicht_function*);
  schemerlicht_function** it_end = schemerlicht_vector_end(functions, schemerlicht_function*);
  for (; it != it_end; ++it)
    {
    schemerlicht_run(ctxt, *it);
    }
  return schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
  }
