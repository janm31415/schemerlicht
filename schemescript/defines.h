#pragma once

#define variable_tag 1
#define virtual_machine_rsp_offset 8  // we need at least an offset of 8 because the virtual byte machine puts 0xffffffffffffffff as return value on the top of the stack 
#define rsp_offset 0

#define FIRST_TEMP_INTEGER_REG VM::vmcode::RAX
#define SECOND_TEMP_INTEGER_REG VM::vmcode::RBX
#define THIRD_TEMP_INTEGER_REG VM::vmcode::R11

#define STACK_BACKUP_REGISTER VM::vmcode::R10
#define STACK_MEM_BACKUP_REGISTER VM::vmcode::MEM_R10

#define FIRST_TEMP_REAL_REG VM::vmcode::XMM4
#define SECOND_TEMP_REAL_REG VM::vmcode::XMM5
#define THIRD_TEMP_REAL_REG VM::vmcode::XMM6

#define GLOBAL_VARIABLE_REG VM::vmcode::RBP
#define GLOBAL_VARIABLE_MEM_REG VM::vmcode::MEM_RBP

//#define RESERVED_REAL_REG VM::vmcode::XMM7
//#define RESERVED_REAL_REG_2 VM::vmcode::XMM8

#define CALLING_CONVENTION_INT_PAR_1 VM::vmcode::RCX
#define CALLING_CONVENTION_INT_PAR_2 VM::vmcode::RDX
#define CALLING_CONVENTION_INT_PAR_3 VM::vmcode::R8
#define CALLING_CONVENTION_INT_PAR_4 VM::vmcode::R9

#define REGISTER_FOR_INT_VARIABLE_1 VM::vmcode::R12
#define REGISTER_FOR_INT_VARIABLE_2 VM::vmcode::R13
#define REGISTER_FOR_INT_VARIABLE_3 VM::vmcode::R14
#define REGISTER_FOR_INT_VARIABLE_4 VM::vmcode::R15
#define REGISTER_FOR_INT_VARIABLE_5 VM::vmcode::RSI
#define REGISTER_FOR_INT_VARIABLE_6 VM::vmcode::RDI

#define CALLING_CONVENTION_REAL_PAR_1 VM::vmcode::XMM0
#define CALLING_CONVENTION_REAL_PAR_2 VM::vmcode::XMM1
#define CALLING_CONVENTION_REAL_PAR_3 VM::vmcode::XMM2
#define CALLING_CONVENTION_REAL_PAR_4 VM::vmcode::XMM3

#define REGISTER_FOR_REAL_VARIABLE_1 VM::vmcode::XMM7
#define REGISTER_FOR_REAL_VARIABLE_2 VM::vmcode::XMM8
#define REGISTER_FOR_REAL_VARIABLE_3 VM::vmcode::XMM9
#define REGISTER_FOR_REAL_VARIABLE_4 VM::vmcode::XMM10
#define REGISTER_FOR_REAL_VARIABLE_5 VM::vmcode::XMM11
#define REGISTER_FOR_REAL_VARIABLE_6 VM::vmcode::XMM12
#define REGISTER_FOR_REAL_VARIABLE_7 VM::vmcode::XMM13
#define REGISTER_FOR_REAL_VARIABLE_8 VM::vmcode::XMM14
#define REGISTER_FOR_REAL_VARIABLE_9 VM::vmcode::XMM15

enum storage_type
  {
  constant,
  external
  };

enum address_type
  {
  register_address,
  memory_address
  };

enum value_type
  {
  real,
  integer,
  real_array,
  integer_array,
  pointer_to_real,
  pointer_to_integer
  };