#pragma once

#include "namespace.h"

COMPILER_BEGIN

#define CONTEXT VM::vmcode::R10
#define RBX_STORE VM::vmcode::MEM_R10, 0
#define RDI_STORE VM::vmcode::MEM_R10, 8
#define RSI_STORE VM::vmcode::MEM_R10, 16
#define RSP_STORE VM::vmcode::MEM_R10, 24
#define RBP_STORE VM::vmcode::MEM_R10, 32
#define R12_STORE VM::vmcode::MEM_R10, 40
#define R13_STORE VM::vmcode::MEM_R10, 48
#define R14_STORE VM::vmcode::MEM_R10, 56
#define R15_STORE VM::vmcode::MEM_R10, 64

#define ALLOC VM::vmcode::RBP
#define MEM_ALLOC VM::vmcode::MEM_RBP
#define BYTE_MEM_ALLOC VM::vmcode::BYTE_MEM_RBP

#define ALLOC_SAVED VM::vmcode::MEM_R10, 72
#define LIMIT VM::vmcode::MEM_R10, 80
#define FROM_SPACE VM::vmcode::MEM_R10, 88
#define FROM_SPACE_END VM::vmcode::MEM_R10, 96
#define TO_SPACE VM::vmcode::MEM_R10, 104
#define TO_SPACE_END VM::vmcode::MEM_R10, 112
#define FROMSPACE_RESERVE VM::vmcode::MEM_R10, 128
#define TEMP_FLONUM VM::vmcode::MEM_R10, 152
#define GC_SAVE VM::vmcode::MEM_R10, 160
#define RSP_SAVE VM::vmcode::MEM_R10, 184
#define NUMBER_OF_LOCALS VM::vmcode::MEM_R10, 144

#define GLOBALS VM::vmcode::MEM_R10, 168
#define GLOBALS_END VM::vmcode::MEM_R10, 176
#define LOCAL VM::vmcode::MEM_R10, 136

#define BUFFER VM::vmcode::MEM_R10, 192
#define DCVT VM::vmcode::MEM_R10, 200
#define OCVT VM::vmcode::MEM_R10, 208
#define XCVT VM::vmcode::MEM_R10, 216
#define GCVT VM::vmcode::MEM_R10, 224

#define STACK_TOP VM::vmcode::MEM_R10, 232
#define STACK VM::vmcode::MEM_R10, 240
#define STACK_SAVE VM::vmcode::MEM_R10, 248

#define ERROR VM::vmcode::MEM_R10, 256

#define LAST_GLOBAL_VARIABLE_USED VM::vmcode::MEM_R10, 272


#define STACK_REGISTER VM::vmcode::R13
#define STACK_REGISTER_MEM VM::vmcode::MEM_R13

#define CONTINUE VM::vmcode::RBX

#define CELLS(n) (n)*8

COMPILER_END
