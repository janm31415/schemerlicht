#ifndef SCHEMERLICHT_VM_H
#define SCHEMERLICHT_VM_H

#include "schemerlicht.h"
#include "limits.h"
#include "object.h"
#include "func.h"

/*===========================================================================
* 
* Based on the LUA virtual machine
* 
* We assume that instructions are unsigned numbers.
* All instructions have an opcode in the first 6 bits.
* Instructions can have the following fields:
* 'A' : 8 bits
* 'B' : 9 bits
* 'C' : 9 bits
* 'Bx' : 18 bits ('B' and 'C' together)
* 'sBx' : signed Bx
*
* A signed argument is represented in excess K; that is, the number
* value is the unsigned value minus K. K is exactly the maximum value
* for that argument (so that -max is represented by 0, and +max is
* represented by 2*max), which is half the maximum for the corresponding
* unsigned argument.
*==========================================================================*/

enum schemerlicht_opcode_mode { schemerlicht_iABC, schemerlicht_iABx, schemerlicht_iAsBx };  /* basic instruction format */

/*
** size and position of opcode arguments.
*/
#define SCHEMERLICHT_SIZE_C 9
#define SCHEMERLICHT_SIZE_B 9
#define SCHEMERLICHT_SIZE_Bx (SCHEMERLICHT_SIZE_C + SCHEMERLICHT_SIZE_B)
#define SCHEMERLICHT_SIZE_A 8

#define SCHEMERLICHT_SIZE_OPCODE 6

#define SCHEMERLICHT_POS_C SCHEMERLICHT_SIZE_OPCODE
#define SCHEMERLICHT_POS_B (SCHEMERLICHT_POS_C + SCHEMERLICHT_SIZE_C)
#define SCHEMERLICHT_POS_Bx SCHEMERLICHT_POS_C
#define SCHEMERLICHT_POS_A (SCHEMERLICHT_POS_B + SCHEMERLICHT_SIZE_B)

/*
** limits for opcode arguments.
** we use (signed) int to manipulate most arguments,
** so they must fit in schemerlicht_mem_bits-1 bits (-1 for sign)
*/
#if SCHEMERLICHT_SIZE_Bx < schemerlicht_mem_bits-1
#define SCHEMERLICHT_MAXARG_Bx   ((1<<SCHEMERLICHT_SIZE_Bx)-1)
#define SCHEMERLICHT_MAXARG_sBx  (SCHEMERLICHT_MAXARG_Bx>>1)         /* 'sBx' is signed */
#else
#define SCHEMERLICHT_MAXARG_Bx        MAX_INT
#define SCHEMERLICHT_MAXARG_sBx        MAX_INT
#endif


#define SCHEMERLICHT_MAXARG_A ((1<<SCHEMERLICHT_SIZE_A)-1)
#define SCHEMERLICHT_MAXARG_B ((1<<SCHEMERLICHT_SIZE_B)-1)
#define SCHEMERLICHT_MAXARG_C ((1<<SCHEMERLICHT_SIZE_C)-1)

/* creates a mask with `n' 1 bits at position `p' */
#define SCHEMERLICHT_MASK1(n,p)	((~((~(schemerlicht_instruction)0)<<n))<<p)

/* creates a mask with `n' 0 bits at position `p' */
#define SCHEMERLICHT_MASK0(n,p)	(~SCHEMERLICHT_MASK1(n,p))

/*
** R(x) - register
** Kst(x) - constant (in constant table)
** RK(x) == if x < schemerlicht_maxstack then R(x) else Kst(x-schemerlicht_maxstack)
*/

typedef enum 
  {
  SCHEMERLICHT_OPCODE_MOVE,       /*  A B      R(A) := R(B)         */
  SCHEMERLICHT_OPCODE_LOADK,      /*  A Bx	   R(A) := Kst(Bx)      */
  SCHEMERLICHT_OPCODE_SETFIXNUM,  /*  A sBx    R(A).fx := sBx          Note: slight speedup measured*/
  SCHEMERLICHT_OPCODE_SETCHAR,    /*  A B      R(A).ch := B         */  
  SCHEMERLICHT_OPCODE_SETPRIM,    /*  A B      R(A).fx := B, R(A)->type := prim */
  SCHEMERLICHT_OPCODE_SETPRIMOBJ, /*  A B      R(A).fx := B, R(A)->type := prim object */
  SCHEMERLICHT_OPCODE_SETTYPE,    /*  A B      type of R(A) := B    */
  SCHEMERLICHT_OPCODE_MOVETOP,    /*  A B      R(0)..R(B) := R(A)..R(A+B), R(A+B+1)->type := blocking   Note: no speedup measured*/
  SCHEMERLICHT_OPCODE_CALL,       /*  A B C	   R(A) := R(A)(R(A+1+C), ... ,R(A+B+C)) */
  SCHEMERLICHT_OPCODE_EQTYPE,     /*  A B      if (type of R(A) == B) then pc++, else perform the following JMP instruction on the next line*/  
  SCHEMERLICHT_OPCODE_JMP,        /*  sBx      PC += sBx					*/
  SCHEMERLICHT_OPCODE_RETURN,     /*  A B	     return R(A), ... ,R(A+B-1) */
  SCHEMERLICHT_OPCODE_LOADGLOBAL, /*  A Bx     R(A) := Global(Bx) */
  SCHEMERLICHT_OPCODE_STOREGLOBAL,/*  A Bx     Global(Bx) := R(A) */
  SCHEMERLICHT_OPCODE_LIST_STACK, /*  A B      R(A) := (list R(A) R(A+1) ... R(A+x)) with x first occurence where R(A+x+1).type == schemerlicht_object_type_blocked */
  SCHEMERLICHT_OPCODE_CALL_FOREIGN/*  A B      R(A) := R(A)(R(A+1), ..., R(A+B) with R(A) a fixnum indicating the index in ctxt->externals*/
  } schemerlicht_opcode;

#define SCHEMERLICHT_NUM_OPCODES (cast(int, SCHEMERLICHT_OPCODE_RETURN+1))

#define SCHEMERLICHT_GET_OPCODE(i)	(cast(schemerlicht_opcode, (i)&SCHEMERLICHT_MASK1(SCHEMERLICHT_SIZE_OPCODE,0)))
#define SCHEMERLICHT_SET_OPCODE(i,o)	((i) = (((i)&SCHEMERLICHT_MASK0(SCHEMERLICHT_SIZE_OPCODE,0)) | cast(schemerlicht_instruction, o)))

#define SCHEMERLICHT_GETARG_A(i)	(cast(int, (i)>>SCHEMERLICHT_POS_A))
#define SCHEMERLICHT_SETARG_A(i,u)	((i) = (((i)&SCHEMERLICHT_MASK0(SCHEMERLICHT_SIZE_A,SCHEMERLICHT_POS_A)) | \
		((cast(schemerlicht_instruction, u)<<SCHEMERLICHT_POS_A)&SCHEMERLICHT_MASK1(SCHEMERLICHT_SIZE_A,SCHEMERLICHT_POS_A))))

#define SCHEMERLICHT_GETARG_B(i)	(cast(int, ((i)>>SCHEMERLICHT_POS_B) & SCHEMERLICHT_MASK1(SCHEMERLICHT_SIZE_B,0)))
#define SCHEMERLICHT_SETARG_B(i,b)	((i) = (((i)&SCHEMERLICHT_MASK0(SCHEMERLICHT_SIZE_B,SCHEMERLICHT_POS_B)) | \
		((cast(schemerlicht_instruction, b)<<SCHEMERLICHT_POS_B)&SCHEMERLICHT_MASK1(SCHEMERLICHT_SIZE_B,SCHEMERLICHT_POS_B))))

#define SCHEMERLICHT_GETARG_C(i)	(cast(int, ((i)>>SCHEMERLICHT_POS_C) & SCHEMERLICHT_MASK1(SCHEMERLICHT_SIZE_C,0)))
#define SCHEMERLICHT_SETARG_C(i,b)	((i) = (((i)&SCHEMERLICHT_MASK0(SCHEMERLICHT_SIZE_C,SCHEMERLICHT_POS_C)) | \
		((cast(schemerlicht_instruction, b)<<SCHEMERLICHT_POS_C)&SCHEMERLICHT_MASK1(SCHEMERLICHT_SIZE_C,SCHEMERLICHT_POS_C))))

#define SCHEMERLICHT_GETARG_Bx(i)	(cast(int, ((i)>>SCHEMERLICHT_POS_Bx) & SCHEMERLICHT_MASK1(SCHEMERLICHT_SIZE_Bx,0)))
#define SCHEMERLICHT_SETARG_Bx(i,b)	((i) = (((i)&SCHEMERLICHT_MASK0(SCHEMERLICHT_SIZE_Bx,SCHEMERLICHT_POS_Bx)) | \
		((cast(schemerlicht_instruction, b)<<SCHEMERLICHT_POS_Bx)&SCHEMERLICHT_MASK1(SCHEMERLICHT_SIZE_Bx,SCHEMERLICHT_POS_Bx))))

#define SCHEMERLICHT_GETARG_sBx(i)	(SCHEMERLICHT_GETARG_Bx(i)-SCHEMERLICHT_MAXARG_sBx)
#define SCHEMERLICHT_SETARG_sBx(i,b)	SCHEMERLICHT_SETARG_Bx((i),cast(unsigned int, (b)+SCHEMERLICHT_MAXARG_sBx))


schemerlicht_object* schemerlicht_run_debug(schemerlicht_context* ctxt, schemerlicht_string* s, const schemerlicht_function* fun);
schemerlicht_object* schemerlicht_run(schemerlicht_context* ctxt, const schemerlicht_function* fun);

schemerlicht_string schemerlicht_fun_to_string(schemerlicht_context* ctxt, schemerlicht_function* fun);

schemerlicht_string schemerlicht_show_stack(schemerlicht_context* ctxt, int stack_start, int stack_end);

#endif //SCHEMERLICHT_VM_H
