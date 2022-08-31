#pragma once

#include "namespace.h"

COMPILER_BEGIN

#define CONTEXT VM::vmcode::R10

#define ALLOC VM::vmcode::RBP
#define MEM_ALLOC VM::vmcode::MEM_RBP
//#define BYTE_MEM_ALLOC VM::vmcode::BYTE_MEM_RBP

#define ALLOC_SAVED VM::vmcode::MEM_R10, 0
#define LIMIT VM::vmcode::MEM_R10, 8
#define FROM_SPACE VM::vmcode::MEM_R10, 16
#define FROM_SPACE_END VM::vmcode::MEM_R10, 24
#define TO_SPACE VM::vmcode::MEM_R10, 32
#define TO_SPACE_END VM::vmcode::MEM_R10, 40
#define FROMSPACE_RESERVE VM::vmcode::MEM_R10, 56
#define TEMP_FLONUM VM::vmcode::MEM_R10, 80
#define GC_SAVE VM::vmcode::MEM_R10, 88
#define RSP_SAVE VM::vmcode::MEM_R10, 112
#define NUMBER_OF_LOCALS VM::vmcode::MEM_R10, 72

#define GLOBALS VM::vmcode::MEM_R10, 96
#define GLOBALS_END VM::vmcode::MEM_R10, 104
#define LOCAL VM::vmcode::MEM_R10, 64

#define BUFFER VM::vmcode::MEM_R10, 120
#define DCVT VM::vmcode::MEM_R10, 128
#define OCVT VM::vmcode::MEM_R10, 136
#define XCVT VM::vmcode::MEM_R10, 144
#define GCVT VM::vmcode::MEM_R10, 152

#define STACK_TOP VM::vmcode::MEM_R10, 160
#define STACK VM::vmcode::MEM_R10, 168
#define STACK_SAVE VM::vmcode::MEM_R10, 176

#define ERROR VM::vmcode::MEM_R10, 184

#define RSP_STORE VM::vmcode::MEM_R10, 192

#define LAST_GLOBAL_VARIABLE_USED VM::vmcode::MEM_R10, 208


#define STACK_REGISTER VM::vmcode::R13
#define STACK_REGISTER_MEM VM::vmcode::MEM_R13

#define CONTINUE VM::vmcode::RBX

#define CELLS(n) (n)*8

#define CALLING_CONVENTION_INT_PAR_1 VM::vmcode::RCX
#define CALLING_CONVENTION_INT_PAR_2 VM::vmcode::RDX
#define CALLING_CONVENTION_INT_PAR_3 VM::vmcode::R8
#define CALLING_CONVENTION_INT_PAR_4 VM::vmcode::R9

#define CALLING_CONVENTION_REAL_PAR_1 VM::vmcode::XMM0
#define CALLING_CONVENTION_REAL_PAR_2 VM::vmcode::XMM1
#define CALLING_CONVENTION_REAL_PAR_3 VM::vmcode::XMM2
#define CALLING_CONVENTION_REAL_PAR_4 VM::vmcode::XMM3

#define virtual_machine_rsp_offset 8  // we need at least an offset of 8 because the virtual byte machine puts 0xffffffffffffffff as return value on the top of the stack
#define rsp_offset 0

COMPILER_END
