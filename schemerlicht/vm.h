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
#define SCHEMERLICHT_MAXARG_sBx  (MAXARG_Bx>>1)         /* 'sBx' is signed */
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
  SCHEMERLICHT_OPCODE_MOVE,       /*	A B	     R(A) := R(B)					*/
  SCHEMERLICHT_OPCODE_LOADK       /*	A Bx	   R(A) := Kst(Bx)					*/
  } schemerlicht_opcode;

#define SCHEMERLICHT_NUM_OPCODES (cast(int, SCHEMERLICHT_OPCODE_LOADK+1))

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

/*
** masks for instruction properties
*/
enum schemerlicht_opcode_mode_mask {
  schemerlicht_opcode_mode_B_is_register = 2,           /* B is a register */
  schemerlicht_opcode_mode_B_is_register_or_constant,		/* B is a register/constant */
  schemerlicht_opcode_mode_C_is_register_or_constant,   /* C is a register/constant */
  schemerlicht_opcode_mode_sets_register_A,  /* instruction sets register A */
  schemerlicht_opcode_mode_Bx_is_a_constant, /* Bx is a constant */
  schemerlicht_opcode_mode_operator_is_test	 /* operator is a test */
  };

extern const schemerlicht_byte schemerlicht_opcode_modes[SCHEMERLICHT_NUM_OPCODES];

#define schemerlicht_get_opcode_mode(m)     (cast(enum schemerlicht_opcode_mode, schemerlicht_opcode_modes[m] & 3))
#define schemerlicht_test_opcode_mode(m, b) (schemerlicht_opcode_modes[m] & (1 << (b)))

schemerlicht_object* schemerlicht_run(schemerlicht_context* ctxt, schemerlicht_function* fun);

#endif //SCHEMERLICHT_VM_H