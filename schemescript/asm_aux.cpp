#include "asm_aux.h"
#include "types.h"
#include "context.h"
#include "context_defs.h"
#include "globals.h"
#include <sstream>

COMPILER_BEGIN

using namespace VM;

vmcode::operand get_mem_operand(vmcode::operand reg)
  {
  switch (reg)
    {
    case vmcode::RAX: return vmcode::MEM_RAX;
    case vmcode::RBX: return vmcode::MEM_RBX;
    case vmcode::RCX: return vmcode::MEM_RCX;
    case vmcode::RDX: return vmcode::MEM_RDX;
    case vmcode::RSI: return vmcode::MEM_RSI;
    case vmcode::RDI: return vmcode::MEM_RDI;
    case vmcode::R8: return vmcode::MEM_R8;
    case vmcode::R9: return vmcode::MEM_R9;
    case vmcode::R10: return vmcode::MEM_R10;
    case vmcode::R11: return vmcode::MEM_R11;
    case vmcode::R12: return vmcode::MEM_R12;
    case vmcode::R13: return vmcode::MEM_R13;
    case vmcode::R14: return vmcode::MEM_R14;
    case vmcode::R15: return vmcode::MEM_R15;
    default: throw std::runtime_error("not implemented");
    }
  }


std::string label_to_string(uint64_t lab)
  {
  std::stringstream str;
  str << "L_" << lab;
  return str.str();
  }


void store_registers(vmcode& code)
  {
  code.add(vmcode::MOV, RSP_STORE, vmcode::RSP);
  }

void load_registers(vmcode& code)
  {
  code.add(vmcode::MOV, vmcode::RSP, RSP_STORE);
  }

int64_t int2fixnum(int64_t i)
  {
  return (i << fixnum_shift) | fixnum_tag;
  }

int64_t fixnum2int(int64_t i)
  {
  return i >> fixnum_shift;
  }

void error_label(vmcode& code, const std::string& label_name, runtime_error re)
  {
  code.add(vmcode::LABEL, label_name);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, (uint64_t)re);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, error_tag);
  code.add(vmcode::JMP, ERROR);
  }

namespace
  {

  void _jump_if_arg_is_not_block(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name)
    {
    code.add(vmcode::MOV, free_reg, arg);
    code.add(vmcode::AND, free_reg, vmcode::NUMBER, block_mask);
    code.add(vmcode::CMP, free_reg, vmcode::NUMBER, block_tag);
    code.add(vmcode::JNE, label_name);
    }

  void _jump_if_arg_does_not_point_to_type(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name, uint64_t typ)
    {
    code.add(vmcode::MOV, free_reg, get_mem_operand(arg));
    code.add(vmcode::SHR, free_reg, vmcode::NUMBER, block_shift);
    code.add(vmcode::AND, free_reg, vmcode::NUMBER, block_header_mask);
    code.add(vmcode::CMP, free_reg, vmcode::NUMBER, typ);
    code.add(vmcode::JNE, label_name);
    }

  void _jump_if_arg_does_not_point_to_flonum(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name)
    {
    _jump_if_arg_does_not_point_to_type(code, arg, free_reg, label_name, flonum_tag);
    }

  void _jump_if_arg_does_not_point_to_closure(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name)
    {
    _jump_if_arg_does_not_point_to_type(code, arg, free_reg, label_name, closure_tag);
    }

  void _jump_if_arg_does_not_point_to_vector(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name)
    {
    _jump_if_arg_does_not_point_to_type(code, arg, free_reg, label_name, vector_tag);
    }

  void _jump_if_arg_does_not_point_to_symbol(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name)
    {
    _jump_if_arg_does_not_point_to_type(code, arg, free_reg, label_name, symbol_tag);
    }

  void _jump_if_arg_does_not_point_to_promise(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name)
    {
    _jump_if_arg_does_not_point_to_type(code, arg, free_reg, label_name, promise_tag);
    }

  void _jump_if_arg_does_not_point_to_string(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name)
    {
    _jump_if_arg_does_not_point_to_type(code, arg, free_reg, label_name, string_tag);
    }

  void _jump_if_arg_does_not_point_to_pair(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name)
    {
    _jump_if_arg_does_not_point_to_type(code, arg, free_reg, label_name, pair_tag);
    }

  void _jump_if_arg_does_not_point_to_port(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name)
    {
    _jump_if_arg_does_not_point_to_type(code, arg, free_reg, label_name, port_tag);
    }
  }


void jump_if_arg_is_not_block(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name)
  {
  _jump_if_arg_is_not_block(code, arg, free_reg, label_name);
  }

void jump_if_arg_does_not_point_to_flonum(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name)
  {
  _jump_if_arg_does_not_point_to_flonum(code, arg, free_reg, label_name);
  }

void jump_if_arg_does_not_point_to_closure(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name)
  {
  _jump_if_arg_does_not_point_to_closure(code, arg, free_reg, label_name);
  }

void jump_if_arg_does_not_point_to_vector(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name)
  {
  _jump_if_arg_does_not_point_to_vector(code, arg, free_reg, label_name);
  }

void jump_if_arg_does_not_point_to_symbol(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name)
  {
  _jump_if_arg_does_not_point_to_symbol(code, arg, free_reg, label_name);
  }

void jump_if_arg_does_not_point_to_promise(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name)
  {
  _jump_if_arg_does_not_point_to_promise(code, arg, free_reg, label_name);
  }

void jump_if_arg_does_not_point_to_string(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name)
  {
  _jump_if_arg_does_not_point_to_string(code, arg, free_reg, label_name);
  }

void jump_if_arg_does_not_point_to_pair(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name)
  {
  _jump_if_arg_does_not_point_to_pair(code, arg, free_reg, label_name);
  }

void jump_if_arg_does_not_point_to_port(vmcode& code, vmcode::operand arg, vmcode::operand free_reg, const std::string& label_name)
  {
  _jump_if_arg_does_not_point_to_port(code, arg, free_reg, label_name);
  }

void check_heap(vmcode& code, runtime_error re)
  {
  // clobbers rax and r15
  auto heap_ok = label_to_string(label++);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::ADD, vmcode::RAX, ALLOC);
  code.add(vmcode::MOV, vmcode::R15, FROM_SPACE_END);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::R15);
  code.add(vmcode::JL, heap_ok);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, (uint64_t)re);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, error_tag);
  code.add(vmcode::JMP, ERROR);
  code.add(vmcode::LABEL, heap_ok);
  }

void copy_string_to_buffer(vmcode& code)
  {
  // assumptions:
  // input: buffer is in BUFFER
  //        string is in rcx
  // output: result is in rax
  // clobbers r11 and r15


  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, block_size_mask);
  code.add(vmcode::AND, vmcode::RAX, vmcode::R11);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::R11, BUFFER);
  std::string fill = label_to_string(label++);
  std::string done = label_to_string(label++);
  code.add(vmcode::LABEL, fill);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::RAX);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, vmcode::R15, vmcode::MEM_RCX);
  code.add(vmcode::MOV, vmcode::MEM_R11, vmcode::R15);
  code.add(vmcode::ADD, vmcode::R11, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::DEC, vmcode::RAX);
  code.add(vmcode::JMP, fill);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::MOV, vmcode::MEM_R11, vmcode::NUMBER, 0);
  }

void allocate_buffer_as_string(vmcode& code)
  {
  // assumptions:
  // input: buffer is in BUFFER
  //        size of string is in rax
  // output: result is in rax
  // clobbers r11 and r15
  // does not check the heap
  code.add(vmcode::PUSH, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::R11, BUFFER);
  code.add(vmcode::ADD, vmcode::R11, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::MEM_R11, vmcode::NUMBER, 0); // make closing zeros at end of string in buffer
  code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::INC, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::RCX, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, (uint64_t)string_tag << (uint64_t)block_shift);
  code.add(vmcode::OR, vmcode::RAX, vmcode::R15);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R15, ALLOC);
  code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));

  std::string fill = label_to_string(label++);
  std::string done = label_to_string(label++);

  code.add(vmcode::MOV, vmcode::R11, BUFFER);
  

  code.add(vmcode::LABEL, fill);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_R11);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::ADD, vmcode::R11, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::DEC, vmcode::RCX);
  code.add(vmcode::TEST, vmcode::RCX, vmcode::RCX);
  code.add(vmcode::JE, done);
  code.add(vmcode::JMP, fill);
  code.add(vmcode::LABEL, done);

  code.add(vmcode::POP, vmcode::RCX);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  }

void raw_string_length(vmcode& code, vmcode::operand string_reg)
  {
  /*
  input is a const char* or a char*, not a scheme string
  clobbers rax, r11, r15, and string_reg
  result is in r15
  string_reg points to the string
  */
  std::string stringlengthdone = label_to_string(label++);
  std::string repeat = label_to_string(label++);

  code.add(vmcode::XOR, vmcode::R15, vmcode::R15);
  code.add(vmcode::LABEL, repeat);
  code.add(vmcode::MOV8, vmcode::RAX, get_mem_operand(string_reg));
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, stringlengthdone);
  code.add(vmcode::INC, vmcode::R15);
  code.add(vmcode::INC, string_reg);
  code.add(vmcode::JMP, repeat);

  code.add(vmcode::LABEL, stringlengthdone);
  // r15 now contains the string length
  }

void string_length(vmcode& code, vmcode::operand string_reg)
  {
  /*
  input points to a scheme string (thus not char* or const char*, see string_length_raw)
  clobbers rax, r11, r15, and string_reg
  result is in r15
  string_reg points to the string
  */
  std::string stringlengthdone = label_to_string(label++);

  code.add(vmcode::MOV, vmcode::R15, get_mem_operand(string_reg));
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, block_size_mask);
  code.add(vmcode::AND, vmcode::R15, vmcode::R11);
  code.add(vmcode::SHL, vmcode::R15, vmcode::NUMBER, 3);
  code.add(vmcode::ADD, string_reg, vmcode::R15);
  code.add(vmcode::SUB, vmcode::R15, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV8, vmcode::RAX, get_mem_operand(string_reg));
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, stringlengthdone);
  code.add(vmcode::INC, vmcode::R15);
  code.add(vmcode::INC, string_reg);

  code.add(vmcode::MOV8, vmcode::RAX, get_mem_operand(string_reg));
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, stringlengthdone);
  code.add(vmcode::INC, vmcode::R15);
  code.add(vmcode::INC, string_reg);

  code.add(vmcode::MOV8, vmcode::RAX, get_mem_operand(string_reg));
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, stringlengthdone);
  code.add(vmcode::INC, vmcode::R15);
  code.add(vmcode::INC, string_reg);

  code.add(vmcode::MOV8, vmcode::RAX, get_mem_operand(string_reg));
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, stringlengthdone);
  code.add(vmcode::INC, vmcode::R15);
  code.add(vmcode::INC, string_reg);

  code.add(vmcode::MOV8, vmcode::RAX, get_mem_operand(string_reg));
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, stringlengthdone);
  code.add(vmcode::INC, vmcode::R15);
  code.add(vmcode::INC, string_reg);

  code.add(vmcode::MOV8, vmcode::RAX, get_mem_operand(string_reg));
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, stringlengthdone);
  code.add(vmcode::INC, vmcode::R15);
  code.add(vmcode::INC, string_reg);

  code.add(vmcode::MOV8, vmcode::RAX, get_mem_operand(string_reg));
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, stringlengthdone);
  code.add(vmcode::INC, vmcode::R15);
  code.add(vmcode::INC, string_reg);

  code.add(vmcode::LABEL, stringlengthdone);
  // r15 now contains the string length
  }

namespace
  {
  void _break()
    {
    printf("break here\n");
    }
  }

void break_point(vmcode& code)
  {
  code.add(vmcode::PUSH, vmcode::R15);
  code.add(vmcode::PUSH, vmcode::R11);
  code.add(vmcode::PUSH, vmcode::RAX);

  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&_break);
  code.add(vmcode::CALL, vmcode::R11);

  code.add(vmcode::POP, vmcode::RAX);
  code.add(vmcode::POP, vmcode::R11);
  code.add(vmcode::POP, vmcode::R15);
  }

void push(vmcode& code, vmcode::operand reg)
  {
  code.add(vmcode::MOV, STACK_REGISTER_MEM, reg);
  code.add(vmcode::ADD, STACK_REGISTER, vmcode::NUMBER, 8);
  }

void pop(vmcode& code, vmcode::operand reg)
  {
  code.add(vmcode::SUB, STACK_REGISTER, vmcode::NUMBER, 8);
  code.add(vmcode::MOV, reg, STACK_REGISTER_MEM);
  }

COMPILER_END
