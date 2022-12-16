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

#if 0
#include "object.h"
#include "func.h"
#include "syscalls.h"
#include "reader.h"
#include "preprocess.h"
#include "compiler.h"
#include "callcc.h"
#include "r5rs.h"
#include "inputoutput.h"
#include "modules.h"
#include "dump.h"
#include <time.h>
#include <stdlib.h>

static char to_lower(char ch)
  {
  if (ch >= 'A' && ch <= 'Z')
    ch += ('a' - 'A');
  return ch;
  }

static schemerlicht_fixnum gcd(schemerlicht_fixnum a, schemerlicht_fixnum b)
  {
  a = a < 0 ? -a : a;
  b = b < 0 ? -b : b;
  if (b == 0)
    return a;
  while ((a % b) > 0)
    {
    schemerlicht_fixnum R = a % b;
    a = b;
    b = R;
    }
  return b;
  }
static schemerlicht_fixnum lcm(schemerlicht_fixnum a, schemerlicht_fixnum b)
  {
  a = a < 0 ? -a : a;
  b = b < 0 ? -b : b;
  return a * b / gcd(a, b);
  }


static schemerlicht_fixnum ipow(schemerlicht_fixnum base, schemerlicht_fixnum exp)
  {
  if (exp < 0)
    return 0;
  schemerlicht_fixnum result = 1;
  for (;;)
    {
    if (exp & 1)
      result *= base;
    exp >>= 1;
    if (!exp)
      break;
    base *= base;
    }
  return result;
  }

static int get_char(schemerlicht_byte* ch, schemerlicht_object* p, int read)
  {
  schemerlicht_assert(schemerlicht_object_get_type(p) == schemerlicht_object_type_port);
  schemerlicht_assert(schemerlicht_object_get_type(schemerlicht_vector_begin(&p->value.v, schemerlicht_object)) == schemerlicht_object_type_true);
  int fileid = cast(int, schemerlicht_vector_at(&p->value.v, 2, schemerlicht_object)->value.fx);
  schemerlicht_fixnum current_pos = schemerlicht_vector_at(&p->value.v, 4, schemerlicht_object)->value.fx;
  schemerlicht_fixnum buffer_length = schemerlicht_vector_at(&p->value.v, 5, schemerlicht_object)->value.fx;
  schemerlicht_fixnum bytes_read = schemerlicht_vector_at(&p->value.v, 6, schemerlicht_object)->value.fx;
  schemerlicht_object* buffer = schemerlicht_vector_at(&p->value.v, 3, schemerlicht_object);
  if (current_pos >= bytes_read)
    { // read buffer
    int bytes_last_read = schemerlicht_read(fileid, buffer->value.s.string_ptr, cast(schemerlicht_memsize, buffer_length));
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
  schemerlicht_assert(schemerlicht_object_get_type(prt) == schemerlicht_object_type_port);
  schemerlicht_assert(schemerlicht_object_get_type(schemerlicht_vector_begin(&prt->value.v, schemerlicht_object)) == schemerlicht_object_type_true);
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
#endif

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

static schemerlicht_string instruction_to_string(schemerlicht_context* ctxt, schemerlicht_instruction instruc)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "");
  char buffer[256];
  switch (SCHEMERLICHT_GET_OPCODE(instruc))
    {
    case SCHEMERLICHT_OPCODE_MOVE:
    {
    const int a = SCHEMERLICHT_GETARG_A(instruc);
    const int b = SCHEMERLICHT_GETARG_B(instruc);
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
    const int a = SCHEMERLICHT_GETARG_A(instruc);
    const int b = SCHEMERLICHT_GETARG_Bx(instruc);
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
    const int a = SCHEMERLICHT_GETARG_A(instruc);
    const int b = SCHEMERLICHT_GETARG_Bx(instruc);
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
    const int a = SCHEMERLICHT_GETARG_A(instruc);
    const int b = SCHEMERLICHT_GETARG_B(instruc);
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
    const int a = SCHEMERLICHT_GETARG_A(instruc);
    const int b = SCHEMERLICHT_GETARG_Bx(instruc);
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
    const int a = SCHEMERLICHT_GETARG_A(instruc);
    const int b = SCHEMERLICHT_GETARG_B(instruc);
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
    const int a = SCHEMERLICHT_GETARG_A(instruc);
    const int b = SCHEMERLICHT_GETARG_B(instruc);
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
    const int a = SCHEMERLICHT_GETARG_A(instruc);
    const int b = SCHEMERLICHT_GETARG_sBx(instruc);
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
    const int a = SCHEMERLICHT_GETARG_A(instruc);
    const int b = SCHEMERLICHT_GETARG_B(instruc);
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
    const int a = SCHEMERLICHT_GETARG_A(instruc);
    const int b = SCHEMERLICHT_GETARG_B(instruc);
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
    const int a = SCHEMERLICHT_GETARG_A(instruc);
    schemerlicht_string_append_cstr(ctxt, &s, "LISTSTACK R(");
    schemerlicht_int_to_char(buffer, a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ") := (list R(A) R(A+1) ... R(A+x)), list until R(A+x+1).type == schemerlicht_object_type_blocked");
    break;
    }
    case SCHEMERLICHT_OPCODE_CALL:
    {
    const int a = SCHEMERLICHT_GETARG_A(instruc);
    schemerlicht_string_append_cstr(ctxt, &s, "CALL R(");
    schemerlicht_int_to_char(buffer, a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")");
    break;
    }
    case SCHEMERLICHT_OPCODE_EQTYPE:
    {
    const int a = SCHEMERLICHT_GETARG_A(instruc);
    const int b = SCHEMERLICHT_GETARG_B(instruc);
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
    int b = SCHEMERLICHT_GETARG_sBx(instruc);
    schemerlicht_string_append_cstr(ctxt, &s, "JUMP ");
    schemerlicht_int_to_char(buffer, b);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    break;
    }
    case SCHEMERLICHT_OPCODE_RETURN:
    {
    const int a = SCHEMERLICHT_GETARG_A(instruc);
    schemerlicht_string_append_cstr(ctxt, &s, "RETURN R(");
    schemerlicht_int_to_char(buffer, a);
    schemerlicht_string_append_cstr(ctxt, &s, buffer);
    schemerlicht_string_append_cstr(ctxt, &s, ")");
    break;
    }
    case SCHEMERLICHT_OPCODE_CALL_FOREIGN:
    {
    const int a = SCHEMERLICHT_GETARG_A(instruc);
    //const int b = SCHEMERLICHT_GETARG_B(instruc);
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

#include "primdispatch.h"

schemerlicht_object* schemerlicht_run(schemerlicht_context* ctxt, schemerlicht_function* fun)
  {
  schemerlicht_assert(fun != NULL);
  const schemerlicht_instruction* pc = schemerlicht_vector_begin(&(fun)->code, schemerlicht_instruction);  
  while (1)
    {
    schemerlicht_assert(pc < schemerlicht_vector_end(&(fun)->code, schemerlicht_instruction));
#ifdef SCHEMERLICHT_CHECK_HEAP_OVERFLOW
    if (ctxt->heap_pos >= ctxt->raw_heap.vector_size / 2)
      {
      schemerlicht_runtime_error_cstr(ctxt, SCHEMERLICHT_ERROR_MEMORY, -1, -1, "Heap is full!");
      break;
      }
#endif
    const schemerlicht_instruction instruc = *pc++;
    const int opcode = SCHEMERLICHT_GET_OPCODE(instruc);
    schemerlicht_assert((opcode == SCHEMERLICHT_OPCODE_JMP) || (SCHEMERLICHT_GETARG_A(instruc) < schemerlicht_maxstack));
    switch (opcode)
      {
      case SCHEMERLICHT_OPCODE_MOVE:
      {
      const int a = SCHEMERLICHT_GETARG_A(instruc);
      const int b = SCHEMERLICHT_GETARG_B(instruc);
#if 0
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      const schemerlicht_object* source = schemerlicht_vector_at(&ctxt->stack, b, schemerlicht_object);
      schemerlicht_set_object(target, source);
#else
      memcpy(((schemerlicht_object*)ctxt->stack.vector_ptr)+a, ((schemerlicht_object*)ctxt->stack.vector_ptr) + b, sizeof(schemerlicht_object));
#endif
      continue;
      }
      case SCHEMERLICHT_OPCODE_MOVETOP:
      {
      const int a = SCHEMERLICHT_GETARG_A(instruc);
      const int b = SCHEMERLICHT_GETARG_B(instruc);
      for (int j = 0; j <= b; ++j)
        {
#if 0
        schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, j, schemerlicht_object);
        const schemerlicht_object* source = schemerlicht_vector_at(&ctxt->stack, a + j, schemerlicht_object);
        schemerlicht_set_object(target, source);
#else
        memcpy(((schemerlicht_object*)ctxt->stack.vector_ptr) + j, ((schemerlicht_object*)ctxt->stack.vector_ptr) + a + j, sizeof(schemerlicht_object));
#endif
        }
      schemerlicht_object* blocking = schemerlicht_vector_at(&ctxt->stack, b + 1, schemerlicht_object);
      blocking->type = schemerlicht_object_type_blocking;
      continue;
      }
      case SCHEMERLICHT_OPCODE_LOADGLOBAL:
      {
      const int a = SCHEMERLICHT_GETARG_A(instruc);
      const int bx = SCHEMERLICHT_GETARG_Bx(instruc);
#if 0
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      const schemerlicht_object* global = schemerlicht_vector_at(&ctxt->globals, bx, schemerlicht_object);
      schemerlicht_set_object(target, global);
#else
      memcpy(((schemerlicht_object*)ctxt->stack.vector_ptr) + a, ((schemerlicht_object*)ctxt->globals.vector_ptr) + bx, sizeof(schemerlicht_object));
#endif
      continue;
      }
      case SCHEMERLICHT_OPCODE_STOREGLOBAL:
      {
      const int a = SCHEMERLICHT_GETARG_A(instruc);
      const int bx = SCHEMERLICHT_GETARG_Bx(instruc);
#if 0
      const schemerlicht_object* source = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      schemerlicht_object* global = schemerlicht_vector_at(&ctxt->globals, bx, schemerlicht_object);
      schemerlicht_set_object(global, source);
#else
      memcpy(((schemerlicht_object*)ctxt->globals.vector_ptr) + bx, ((schemerlicht_object*)ctxt->stack.vector_ptr) + a, sizeof(schemerlicht_object));
#endif
      continue;
      }
      case SCHEMERLICHT_OPCODE_LOADK:
      {
      const int a = SCHEMERLICHT_GETARG_A(instruc);
      const int bx = SCHEMERLICHT_GETARG_Bx(instruc);
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
      continue;
      }
      case SCHEMERLICHT_OPCODE_SETFIXNUM:
      {
      const int a = SCHEMERLICHT_GETARG_A(instruc);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      const int b = SCHEMERLICHT_GETARG_sBx(instruc);
      target->type = schemerlicht_object_type_fixnum;
      target->value.fx = b;
      continue;
      }
      case SCHEMERLICHT_OPCODE_SETPRIM:
      {
      const int a = SCHEMERLICHT_GETARG_A(instruc);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      const int b = SCHEMERLICHT_GETARG_B(instruc);
      target->type = schemerlicht_object_type_primitive;
      target->value.fx = b;
      continue;
      }
      case SCHEMERLICHT_OPCODE_SETPRIMOBJ:
      {
      const int a = SCHEMERLICHT_GETARG_A(instruc);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      const int b = SCHEMERLICHT_GETARG_B(instruc);
      target->type = schemerlicht_object_type_primitive_object;
      target->value.fx = b;
      continue;
      }
      case SCHEMERLICHT_OPCODE_SETCHAR:
      {
      const int a = SCHEMERLICHT_GETARG_A(instruc);
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      const int b = SCHEMERLICHT_GETARG_B(instruc);
      target->type = schemerlicht_object_type_char;
      target->value.ch = cast(schemerlicht_byte, b);
      continue;
      }
      case SCHEMERLICHT_OPCODE_SETTYPE:
      {
      schemerlicht_object* target = schemerlicht_vector_at(&ctxt->stack, SCHEMERLICHT_GETARG_A(instruc), schemerlicht_object);
      const int b = SCHEMERLICHT_GETARG_B(instruc);
      target->type = b;
      continue;
      }
      case SCHEMERLICHT_OPCODE_CALL:
      {
      const int a = SCHEMERLICHT_GETARG_A(instruc);
      const int b = SCHEMERLICHT_GETARG_B(instruc);
      const int c = SCHEMERLICHT_GETARG_C(instruc);
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
        //schemerlicht_call_primitive_dispatch(ctxt, function_id, a, b, c);
        (*prim_fun_dispatch_table[function_id])(ctxt, a, b, c);
#else
        //schemerlicht_call_primitive_dispatch(ctxt, function_id, a, b, c);
        (*prim_fun_dispatch_table[function_id])(ctxt, a, b, c);
#endif
        continue;
        }
        case schemerlicht_object_type_primitive_object:
        {
        const schemerlicht_fixnum function_id = target->value.fx;
        //schemerlicht_call_primitive_dispatch(ctxt, function_id, a, b - 1, c + 1);
        (*prim_fun_dispatch_table[function_id])(ctxt, a, b-1, c+1);
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
          fun = lambda;
          }
        else
          {
          schemerlicht_assert(schemerlicht_object_get_type(&continuation) == schemerlicht_object_type_lambda);
          schemerlicht_function* lambda = cast(schemerlicht_function*, continuation.value.ptr);
          pc = schemerlicht_vector_begin(&lambda->code, schemerlicht_instruction);          
          fun = lambda;
          }
        schemerlicht_check_garbage_collection(ctxt);
        continue;
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
        fun = lambda;
        schemerlicht_check_garbage_collection(ctxt);
        continue;
        }
        case schemerlicht_object_type_lambda:
        {
        schemerlicht_assert(a == 0); // closures restart the stack
        schemerlicht_function* lambda = cast(schemerlicht_function*, target->value.ptr);
        pc = schemerlicht_vector_begin(&lambda->code, schemerlicht_instruction);        
        fun = lambda;
        schemerlicht_check_garbage_collection(ctxt);
        continue;
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
        return schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
        }
        }
      break;
      }
      case SCHEMERLICHT_OPCODE_EQTYPE:
      {
      const int a = SCHEMERLICHT_GETARG_A(instruc);
      const int b = SCHEMERLICHT_GETARG_B(instruc);
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
      continue;
      }
      case SCHEMERLICHT_OPCODE_LIST_STACK:
      {
      const int a = SCHEMERLICHT_GETARG_A(instruc);
      int x = a;
      schemerlicht_object* rx = schemerlicht_vector_at(&ctxt->stack, x, schemerlicht_object);
      while (schemerlicht_object_get_type(rx) != schemerlicht_object_type_blocking)
        {
        ++x;
        rx = schemerlicht_vector_at(&ctxt->stack, x, schemerlicht_object);
        }
      make_variable_arity_list(ctxt, a, x - a);
      continue;
      }
      case SCHEMERLICHT_OPCODE_CALL_FOREIGN:
      {
      const int a = SCHEMERLICHT_GETARG_A(instruc);
      const int b = SCHEMERLICHT_GETARG_B(instruc);
      schemerlicht_object* ra = schemerlicht_vector_at(&ctxt->stack, a, schemerlicht_object);
      schemerlicht_assert(schemerlicht_object_get_type(ra) == schemerlicht_object_type_fixnum);
      const schemerlicht_memsize position = cast(schemerlicht_memsize, ra->value.fx);
      schemerlicht_assert(position < ctxt->externals.vector_size);
      schemerlicht_external_function* ext = schemerlicht_vector_at(&ctxt->externals, position, schemerlicht_external_function);
      //schemerlicht_assert(ext->arguments.vector_size == cast(schemerlicht_memsize, b));
      schemerlicht_object result = schemerlicht_call_external(ctxt, ext, a + 1, b);
      schemerlicht_set_object(ra, &result);
      continue;
      }
      case SCHEMERLICHT_OPCODE_JMP:
      {
      const int sbx = SCHEMERLICHT_GETARG_sBx(instruc);
      pc += sbx;
      continue;
      }
      case SCHEMERLICHT_OPCODE_RETURN:
      {
      const int a = SCHEMERLICHT_GETARG_A(instruc);
      const int b = SCHEMERLICHT_GETARG_B(instruc);
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
      schemerlicht_check_garbage_collection(ctxt);
      return schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
      }
      //default:
      //  schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
      }
    }
  //return schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
  }

schemerlicht_string schemerlicht_fun_to_string(schemerlicht_context* ctxt, schemerlicht_function* fun)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "");
  schemerlicht_instruction* it = schemerlicht_vector_begin(&fun->code, schemerlicht_instruction);
  schemerlicht_instruction* it_end = schemerlicht_vector_end(&fun->code, schemerlicht_instruction);
  for (; it != it_end; ++it)
    {
    const schemerlicht_instruction instruc = *it;
    schemerlicht_string tmp = instruction_to_string(ctxt, instruc);
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
