#include "primitives.h"

#include "file_utils.h"

#include "asm_aux.h"
//#include "c_prim_decl.h"
#include "context.h"
#include "context_defs.h"
#include "globals.h"
#include "inlines.h"
#include "types.h"
#include "syscalls.h"

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>

COMPILER_BEGIN

using namespace VM;

namespace
  {
  std::vector<vmcode::operand> compute_argument_registers()
    {
    std::vector<vmcode::operand> usable_registers;
    usable_registers.push_back(vmcode::RCX);
    usable_registers.push_back(vmcode::RDX);
    usable_registers.push_back(vmcode::RSI);
    usable_registers.push_back(vmcode::RDI);
    usable_registers.push_back(vmcode::R8);
    usable_registers.push_back(vmcode::R9);
    usable_registers.push_back(vmcode::R12);
    usable_registers.push_back(vmcode::R14);
    return usable_registers;
    }
  }

const std::vector<vmcode::operand>& get_argument_registers()
  {
  static std::vector<vmcode::operand> reg = compute_argument_registers();
  return reg;
  }

void compile_is_eq(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::CMP, vmcode::RCX, vmcode::RDX);
  // if comparison (last cmp call) succeeded, then sete puts 1 in al, otherwise 0
  code.add(vmcode::SETE, vmcode::RAX);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_eq_contract_violation);
    }
  }

void compile_is_eqv(vmcode& code, const compiler_options& ops)
  {
  /*
  //From Bones:
  //`eqv?' performs /structural/ comparison, which means it compares
  //  the contents of its two arguments, in case they are of equal
  //  type. That means it will will return `#t' if both arguments have
  //  the same type and identical contents, even if the arguments are
  //  not numbers or characters.

  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::R11, vmcode::RDX);
  std::string lab_eq = label_to_string(label++);
  std::string lab = label_to_string(label++);
  code.add(vmcode::JMP, lab);
  compile_structurally_equal(code, ops, lab_eq);
  code.add(vmcode::LABEL, lab);
  code.add(vmcode::CALL, lab_eq);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_eqv_contract_violation);
    }
    */

  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::CMP, vmcode::RCX, vmcode::RDX);
  // if comparison (last cmp call) succeeded, then sete puts 1 in al, otherwise 0
  code.add(vmcode::SETE, vmcode::RAX);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_eqv_contract_violation);
    }
  }

void compile_is_eqv_structurally(vmcode& code, const compiler_options& ops)
  {
  //From Bones:
  //`eqv?' performs /structural/ comparison, which means it compares
  //  the contents of its two arguments, in case they are of equal
  //  type. That means it will will return `#t' if both arguments have
  //  the same type and identical contents, even if the arguments are
  //  not numbers or characters.

  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::R11, vmcode::RDX);
  std::string lab_eq = label_to_string(label++);
  std::string lab = label_to_string(label++);
  code.add(vmcode::JMP, lab);
  compile_structurally_equal(code, ops, lab_eq);
  code.add(vmcode::LABEL, lab);
  code.add(vmcode::CALL, lab_eq);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_eqvstruct_contract_violation);
    }
  }

void compile_is_equal(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::R11, vmcode::RDX);
  std::string lab_eq = label_to_string(label++);
  std::string lab = label_to_string(label++);
  code.add(vmcode::JMP, lab);
  compile_recursively_equal(code, ops, lab_eq);
  code.add(vmcode::LABEL, lab);
  code.add(vmcode::CALL, lab_eq);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_isequal_contract_violation);
    }
  }

void compile_closure(vmcode& code, const compiler_options& ops)
  {
  if (ops.safe_primitives)
    {
    code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);
    code.add(vmcode::INC, vmcode::RAX);
    check_heap(code, re_closure_heap_overflow);
    }
  auto done = label_to_string(label++);
  auto done2 = label_to_string(label++);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, (uint64_t)closure_tag << (uint64_t)block_shift);
  code.add(vmcode::OR, vmcode::RAX, vmcode::R15);
  code.add(vmcode::MOV, vmcode::R15, ALLOC);
  code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::RCX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(2), vmcode::RDX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(3), vmcode::RSI);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 3);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(4), vmcode::RDI);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 4);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(5), vmcode::R8);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 5);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(6), vmcode::R9);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 6);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(7), vmcode::R12);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 7);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(8), vmcode::R14);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 8);
  code.add(vmcode::JE, done);

  code.add(vmcode::SUB, vmcode::R11, vmcode::NUMBER, 8);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(9));
  code.add(vmcode::MOV, vmcode::RDX, LOCAL);

  auto lab = label_to_string(label++);
  code.add(vmcode::LABEL, lab);

  code.add(vmcode::MOV, vmcode::RCX, vmcode::MEM_RDX);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RCX);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::JE, done2);
  code.add(vmcode::JMP, lab);

  code.add(vmcode::LABEL, done);
  code.add(vmcode::INC, vmcode::R11);
  code.add(vmcode::SHL, vmcode::R11, vmcode::NUMBER, 3);
  code.add(vmcode::ADD, ALLOC, vmcode::R11);
  code.add(vmcode::LABEL, done2);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_closure_ref(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // here check whether it is a closure
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_closure(code, vmcode::RCX, vmcode::R11, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::SAR, vmcode::RDX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    {
    // optionally, check whether rdx is in bounds.
    std::string in_bounds = label_to_string(label++);
    std::string not_in_bounds = label_to_string(label++);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
    code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, block_size_mask);
    code.add(vmcode::AND, vmcode::RAX, vmcode::R15); // get size of closure
    code.add(vmcode::CMP, vmcode::RDX, vmcode::RAX);
    code.add(vmcode::JGE, not_in_bounds);
    code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, 0);
    code.add(vmcode::JGE, in_bounds);
    error_label(code, not_in_bounds, re_closure_ref_out_of_bounds);
    code.add(vmcode::LABEL, in_bounds);
    }

  code.add(vmcode::INC, vmcode::RDX); // increase 1 for header
  code.add(vmcode::SHL, vmcode::RDX, vmcode::NUMBER, 3);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_closure_ref_contract_violation);
    }
  }

void compile_string_copy(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1); // rcx should be positive
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
    code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, block_size_mask);
    code.add(vmcode::AND, vmcode::RAX, vmcode::R11);
    code.add(vmcode::INC, vmcode::RAX);
    check_heap(code, re_string_copy_heap_overflow);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, block_size_mask);
  code.add(vmcode::AND, vmcode::RAX, vmcode::R11);
  code.add(vmcode::MOV, vmcode::R11, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, (uint64_t)string_tag << (uint64_t)block_shift);
  code.add(vmcode::OR, vmcode::RAX, vmcode::R15);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R15, ALLOC);
  code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));

  auto repeat = label_to_string(label++);
  auto done = label_to_string(label++);

  code.add(vmcode::LABEL, repeat);
  code.add(vmcode::TEST, vmcode::R11, vmcode::R11);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::JMP, repeat);
  code.add(vmcode::LABEL, done);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_string_copy_contract_violation);
    }
  }

void compile_symbol_to_string(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1); // rcx should be positive
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_symbol(code, vmcode::RCX, vmcode::R11, error);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
    code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, block_size_mask);
    code.add(vmcode::AND, vmcode::RAX, vmcode::R11);
    code.add(vmcode::INC, vmcode::RAX);
    check_heap(code, re_symbol_to_string_heap_overflow);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, block_size_mask);
  code.add(vmcode::AND, vmcode::RAX, vmcode::R11);
  code.add(vmcode::MOV, vmcode::R11, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, (uint64_t)string_tag << (uint64_t)block_shift);
  code.add(vmcode::OR, vmcode::RAX, vmcode::R15);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R15, ALLOC);
  code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));

  auto repeat = label_to_string(label++);
  auto done = label_to_string(label++);

  code.add(vmcode::LABEL, repeat);
  code.add(vmcode::TEST, vmcode::R11, vmcode::R11);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::JMP, repeat);
  code.add(vmcode::LABEL, done);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_symbol_to_string_contract_violation);
    }
  }

void compile_string_hash(vmcode& code, const compiler_options& ops)
  {

  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xfffffffffffffff8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error);
    }

  code.add(vmcode::PUSH, vmcode::RBX);
  code.add(vmcode::PUSH, vmcode::RDX);
  code.add(vmcode::MOV, vmcode::R11, vmcode::MEM_RCX);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, block_size_mask);
  code.add(vmcode::AND, vmcode::R11, vmcode::RAX);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));

  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, 3498574187);
  code.add(vmcode::MUL, vmcode::R15);
  code.add(vmcode::MOV, vmcode::RBX, vmcode::RAX);

  auto repeat = label_to_string(label++);
  auto done = label_to_string(label++);

  code.add(vmcode::LABEL, repeat);
  code.add(vmcode::TEST, vmcode::R11, vmcode::R11);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
  code.add(vmcode::MUL, vmcode::R15);
  code.add(vmcode::XOR, vmcode::RBX, vmcode::RAX);
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::JMP, repeat);
  code.add(vmcode::LABEL, done);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::SHR, vmcode::RBX, vmcode::NUMBER, 8);
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::SHR, vmcode::RBX, vmcode::NUMBER, 8);
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::SHR, vmcode::RBX, vmcode::NUMBER, 8);
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::SHR, vmcode::RBX, vmcode::NUMBER, 8);
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::SHR, vmcode::RBX, vmcode::NUMBER, 8);
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::SHR, vmcode::RBX, vmcode::NUMBER, 8);
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::SHR, vmcode::RBX, vmcode::NUMBER, 8);
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 255);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::POP, vmcode::RDX);
  code.add(vmcode::POP, vmcode::RBX);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_string_hash_contract_violation);
    }
  }

void compile_string_append1(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    jump_if_arg_is_not_block(code, vmcode::RDX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xfffffffffffffff8);
  code.add(vmcode::AND, vmcode::RDX, vmcode::NUMBER, 0xfffffffffffffff8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error);
    jump_if_arg_does_not_point_to_string(code, vmcode::RDX, vmcode::R11, error);
    }

  code.add(vmcode::PUSH, vmcode::RBX);
  code.add(vmcode::PUSH, vmcode::RCX);
  string_length(code, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::RBX, vmcode::R15);
  code.add(vmcode::POP, vmcode::RCX);
  code.add(vmcode::PUSH, vmcode::RDX);
  string_length(code, vmcode::RDX);
  code.add(vmcode::POP, vmcode::RDX);

  if (ops.safe_primitives) // TO CHECK: string length should be divided by 8 here I thing before heap check
    {
    code.add(vmcode::MOV, vmcode::RAX, vmcode::RBX);
    code.add(vmcode::POP, vmcode::RBX);
    code.add(vmcode::PUSH, vmcode::RCX);
    code.add(vmcode::MOV, vmcode::RCX, vmcode::RAX);
    code.add(vmcode::ADD, vmcode::RAX, vmcode::R15);
    code.add(vmcode::MOV, vmcode::R11, vmcode::R15);
    //code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 3); // added as action on comment // TO CHECK: string length should be divided by 8 here I thing before heap check
    //code.add(vmcode::INC, vmcode::RAX);                     // not tested yet  ==> removed again: made compiler.scm fail!!
    check_heap(code, re_string_append_heap_overflow);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::RBX);
    code.add(vmcode::MOV, vmcode::RBX, vmcode::RCX);
    code.add(vmcode::POP, vmcode::RCX);
    code.add(vmcode::PUSH, vmcode::RAX);
    code.add(vmcode::MOV, vmcode::R15, vmcode::R11);
    }

  /*
  status: rcx points to string 1
          rdx points to string 2
          rbx contains string 1 length
          r15 contains string 2 length
  */

  code.add(vmcode::PUSH, vmcode::R15); // save string 2 length
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::ADD, vmcode::RAX, vmcode::R15);
  code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::INC, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R11, vmcode::RAX); // save block length in r11
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, (uint64_t)string_tag << (uint64_t)block_shift);
  code.add(vmcode::OR, vmcode::RAX, vmcode::R15);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R15, ALLOC);
  code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));

  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::SHL, vmcode::R11, vmcode::NUMBER, 3);
  code.add(vmcode::ADD, ALLOC, vmcode::R11);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::NUMBER, 0); // ending 0 character
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));

  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1)); // skip header string 1
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1)); // skip header string 2

  auto repeat = label_to_string(label++);
  auto done = label_to_string(label++);
  auto repeat2 = label_to_string(label++);
  auto done2 = label_to_string(label++);
  code.add(vmcode::MOV, vmcode::R11, vmcode::R15); // get start of string block
  code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, 0xfffffffffffffff8); // remove block tag
  code.add(vmcode::ADD, vmcode::R11, vmcode::NUMBER, CELLS(1)); // skip header

  code.add(vmcode::LABEL, repeat);
  code.add(vmcode::TEST, vmcode::RBX, vmcode::RBX);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, vmcode::RAX, vmcode::MEM_RCX);
  code.add(vmcode::MOV8, vmcode::MEM_R11, vmcode::RAX);
  code.add(vmcode::DEC, vmcode::RBX);
  code.add(vmcode::INC, vmcode::RCX);
  code.add(vmcode::INC, vmcode::R11);
  code.add(vmcode::JMP, repeat);
  code.add(vmcode::LABEL, done);

  code.add(vmcode::POP, vmcode::RBX); // get length of string 2 in rbx
  code.add(vmcode::LABEL, repeat2);
  code.add(vmcode::TEST, vmcode::RBX, vmcode::RBX);
  code.add(vmcode::JE, done2);
  code.add(vmcode::MOV8, vmcode::RAX, vmcode::MEM_RDX);
  code.add(vmcode::MOV8, vmcode::MEM_R11, vmcode::RAX);
  code.add(vmcode::DEC, vmcode::RBX);
  code.add(vmcode::INC, vmcode::RDX);
  code.add(vmcode::INC, vmcode::R11);
  code.add(vmcode::JMP, repeat2);
  code.add(vmcode::LABEL, done2);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  code.add(vmcode::POP, vmcode::RBX);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_string_append_contract_violation);
    }
  }

void compile_substring(vmcode& code, const compiler_options& ops)
  {
  std::string error, not_in_bounds;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    not_in_bounds = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 3);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RSI, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, 0);
    code.add(vmcode::JL, not_in_bounds);
    code.add(vmcode::CMP, vmcode::RSI, vmcode::NUMBER, 0);
    code.add(vmcode::JL, not_in_bounds);
    code.add(vmcode::CMP, vmcode::RDX, vmcode::RSI);
    code.add(vmcode::JG, not_in_bounds);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::RSI);
    code.add(vmcode::SUB, vmcode::RAX, vmcode::RDX);
    code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 1);
    check_heap(code, re_substring_heap_overflow);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xfffffffffffffff8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
    code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, block_size_mask);
    code.add(vmcode::AND, vmcode::RAX, vmcode::R11);
    code.add(vmcode::MOV, vmcode::R11, vmcode::RSI);
    code.add(vmcode::SHR, vmcode::R11, vmcode::NUMBER, 4);
    code.add(vmcode::INC, vmcode::R11);
    code.add(vmcode::CMP, vmcode::RAX, vmcode::R11);
    code.add(vmcode::JL, not_in_bounds);
    }

  code.add(vmcode::MOV, vmcode::RAX, vmcode::RSI);
  code.add(vmcode::SUB, vmcode::RAX, vmcode::RDX);
  code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 4);
  code.add(vmcode::INC, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R11, vmcode::RAX);
  code.add(vmcode::SHL, vmcode::R11, vmcode::NUMBER, 3);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, (uint64_t)string_tag << (uint64_t)block_shift);
  code.add(vmcode::OR, vmcode::RAX, vmcode::R15);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R15, ALLOC);
  code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::RAX, ALLOC);
  code.add(vmcode::ADD, ALLOC, vmcode::R11);

  code.add(vmcode::SHR, vmcode::RDX, vmcode::NUMBER, 1);
  code.add(vmcode::SHR, vmcode::RSI, vmcode::NUMBER, 1);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, vmcode::RCX, vmcode::RDX);

  code.add(vmcode::PUSH, vmcode::RBX);
  auto repeat = label_to_string(label++);
  auto done = label_to_string(label++);
  code.add(vmcode::LABEL, repeat);
  code.add(vmcode::CMP, vmcode::RDX, vmcode::RSI);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, vmcode::RBX, vmcode::MEM_RCX);
  code.add(vmcode::MOV8, vmcode::MEM_RAX, vmcode::RBX);
  code.add(vmcode::INC, vmcode::RDX);
  code.add(vmcode::INC, vmcode::RCX);
  code.add(vmcode::INC, vmcode::RAX);
  code.add(vmcode::JMP, repeat);
  code.add(vmcode::LABEL, done);

  auto repeat2 = label_to_string(label++);
  auto done2 = label_to_string(label++);
  code.add(vmcode::XOR, vmcode::RBX, vmcode::RBX); // make remaining bytes of string equal to 0
  code.add(vmcode::LABEL, repeat2);
  code.add(vmcode::MOV8, vmcode::MEM_RAX, vmcode::RBX);
  code.add(vmcode::INC, vmcode::RAX);
  code.add(vmcode::CMP, vmcode::RAX, ALLOC);
  code.add(vmcode::JE, done2);
  code.add(vmcode::JMP, repeat2);
  code.add(vmcode::LABEL, done2);

  code.add(vmcode::POP, vmcode::RBX);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_substring_contract_violation);
    error_label(code, not_in_bounds, re_substring_out_of_bounds);
    }
  }

void compile_allocate_symbol(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
    code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, block_size_mask);
    code.add(vmcode::AND, vmcode::RAX, vmcode::R11);
    code.add(vmcode::INC, vmcode::RAX);
    check_heap(code, re_allocate_symbol_heap_overflow);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, block_size_mask);
  code.add(vmcode::AND, vmcode::RAX, vmcode::R11);
  code.add(vmcode::MOV, vmcode::R11, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, (uint64_t)symbol_tag << (uint64_t)block_shift);
  code.add(vmcode::OR, vmcode::RAX, vmcode::R15);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R15, ALLOC);
  code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));

  auto repeat = label_to_string(label++);
  auto done = label_to_string(label++);

  code.add(vmcode::LABEL, repeat);
  code.add(vmcode::TEST, vmcode::R11, vmcode::R11);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::JMP, repeat);
  code.add(vmcode::LABEL, done);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_allocate_symbol_contract_violation);
    }
  }

void compile_make_string(vmcode& code, const compiler_options& ops)
  {
  auto fill_with_value = label_to_string(label++);
  auto fill_random = label_to_string(label++);
  auto fill_remainder = label_to_string(label++);
  auto done1 = label_to_string(label++);
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 0); // rcx should be positive
    code.add(vmcode::JL, error);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
    code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 4);
    code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, 2);
    check_heap(code, re_make_string_heap_overflow);
    }
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
  code.add(vmcode::JE, fill_with_value);
  if (ops.safe_primitives)
    {
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }

  code.add(vmcode::SHR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::INC, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, (uint64_t)string_tag << (uint64_t)block_shift);
  code.add(vmcode::OR, vmcode::RAX, vmcode::R15);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R15, ALLOC);
  code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));

  code.add(vmcode::LABEL, fill_random);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 8);
  code.add(vmcode::JL, fill_remainder);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::NUMBER, 0xffffffffffffffff);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::SUB, vmcode::RCX, vmcode::NUMBER, 8);
  code.add(vmcode::JMP, fill_random);
  code.add(vmcode::LABEL, fill_remainder);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::NUMBER, 0);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, done1); // assembly can jump short, but vm cannot
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 0x00000000000000ff);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, done1);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 0x000000000000ffff);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 2);
  code.add(vmcode::JE, done1);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 0x0000000000ffffff);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 3);
  code.add(vmcode::JE, done1);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 0x00000000ffffffff);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 4);
  code.add(vmcode::JE, done1);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 0x000000ffffffffff);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 5);
  code.add(vmcode::JE, done1);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 0x0000ffffffffffff);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 6);
  code.add(vmcode::JE, done1);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 0x00ffffffffffffff);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::LABEL, done1);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  code.add(vmcode::JMP, CONTINUE);
  /*
  code.add(vmcode::SHR, vmcode::RCX, vmcode::NUMBER, 4);
  code.add(vmcode::INC, vmcode::RCX);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, (uint64_t)string_tag << (uint64_t)block_shift);
  code.add(vmcode::OR, vmcode::RAX, vmcode::R15);
  code.add(vmcode::MOV, vmcode::R15, ALLOC);
  code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::LABEL, fill_zero);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::NUMBER, 0);
  code.add(vmcode::DEC, vmcode::RCX);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 0);
  code.add(vmcode::JNE, fill_zero);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  code.add(vmcode::JMP, CONTINUE);
  */


  code.add(vmcode::LABEL, fill_with_value);
  code.add(vmcode::SHR, vmcode::RDX, vmcode::NUMBER, 8); // get character value in dl
  code.add(vmcode::SHR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::INC, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, (uint64_t)string_tag << (uint64_t)block_shift);
  code.add(vmcode::OR, vmcode::RAX, vmcode::R15);
  code.add(vmcode::MOV, vmcode::R15, ALLOC);
  code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));

  auto fill_rest = label_to_string(label++);
  auto fill_full = label_to_string(label++);
  auto done = label_to_string(label++);
  code.add(vmcode::LABEL, fill_full);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::JL, fill_rest);

  code.add(vmcode::MOV8, MEM_ALLOC, vmcode::RDX);
  code.add(vmcode::MOV8, MEM_ALLOC, 1, vmcode::RDX);
  code.add(vmcode::MOV8, MEM_ALLOC, 2, vmcode::RDX);
  code.add(vmcode::MOV8, MEM_ALLOC, 3, vmcode::RDX);
  code.add(vmcode::MOV8, MEM_ALLOC, 4, vmcode::RDX);
  code.add(vmcode::MOV8, MEM_ALLOC, 5, vmcode::RDX);
  code.add(vmcode::MOV8, MEM_ALLOC, 6, vmcode::RDX);
  code.add(vmcode::MOV8, MEM_ALLOC, 7, vmcode::RDX);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::SUB, vmcode::RCX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::JMP, fill_full);

  code.add(vmcode::LABEL, fill_rest);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::NUMBER, 0);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, MEM_ALLOC, vmcode::RDX);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, MEM_ALLOC, 1, vmcode::RDX);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 2);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, MEM_ALLOC, 2, vmcode::RDX);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 3);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, MEM_ALLOC, 3, vmcode::RDX);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 4);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, MEM_ALLOC, 4, vmcode::RDX);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 5);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, MEM_ALLOC, 5, vmcode::RDX);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 6);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, MEM_ALLOC, 6, vmcode::RDX);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 7);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, MEM_ALLOC, 7, vmcode::RDX);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));

  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_make_string_contract_violation);
    }
  }

void compile_fixnum_to_char(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }

  code.add(vmcode::SHL, vmcode::RCX, vmcode::NUMBER, 7);
  code.add(vmcode::OR, vmcode::RCX, vmcode::NUMBER, char_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_fixnum_to_char_contract_violation);
    }
  }

void compile_char_to_fixnum(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::MOV, vmcode::R11, vmcode::RCX);
    code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, char_mask);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, char_tag);
    code.add(vmcode::JNE, error);
    }

  code.add(vmcode::SHR, vmcode::RCX, vmcode::NUMBER, 7);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_char_to_fixnum_contract_violation);
    }
  }

void compile_fx_less(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string t = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::CMP, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::JL, t);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, t);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_fx_less_contract_violation);
    }
  }

void compile_fx_greater(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string t = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::CMP, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::JG, t);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, t);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_fx_greater_contract_violation);
    }
  }

void compile_fx_leq(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string t = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::CMP, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::JLE, t);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, t);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_fx_leq_contract_violation);
    }
  }

void compile_fx_geq(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string t = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::CMP, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::JGE, t);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, t);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_fx_geq_contract_violation);
    }
  }

void compile_fx_add1(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 2);
  code.add(vmcode::ADD, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_fx_add1_contract_violation);
    }
  }

void compile_fx_sub1(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::SUB, vmcode::RAX, vmcode::NUMBER, 2);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_fx_sub1_contract_violation);
    }
  }

void compile_fx_is_zero(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 0);
  code.add(vmcode::SETE, vmcode::RAX);
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_fx_is_zero_contract_violation);
    }
  }

void compile_fx_add(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::ADD, vmcode::RAX, vmcode::RDX);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_fx_add_contract_violation);
    }
  }

void compile_fx_sub(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::SUB, vmcode::RAX, vmcode::RDX);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_fx_sub_contract_violation);
    }
  }

void compile_fx_mul(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::IMUL, vmcode::RDX);
  code.add(vmcode::SAR, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_fx_mul_contract_violation);
    }
  }

void compile_fx_div(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string division_by_zero;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    division_by_zero = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::RDX);
    code.add(vmcode::JE, division_by_zero);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::XOR, vmcode::RDX, vmcode::RDX);
  //code.add(vmcode::CQO);
  code.add(vmcode::IDIV, vmcode::RCX);
  code.add(vmcode::SAL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_fx_div_contract_violation);
    error_label(code, division_by_zero, re_division_by_zero);
    }
  }

void compile_fx_equal(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::CMP, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::SETE, vmcode::RAX);
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_fx_equal_contract_violation);
    }
  }

void compile_char_less(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string t = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::MOV, vmcode::R11, vmcode::RCX);
    code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, char_mask);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, char_tag);
    code.add(vmcode::JNE, error);
    code.add(vmcode::MOV, vmcode::R11, vmcode::RDX);
    code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, char_mask);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, char_tag);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::CMP, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::JL, t);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, t);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_char_less_contract_violation);
    }
  }

void compile_char_greater(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string t = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::MOV, vmcode::R11, vmcode::RCX);
    code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, char_mask);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, char_tag);
    code.add(vmcode::JNE, error);
    code.add(vmcode::MOV, vmcode::R11, vmcode::RDX);
    code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, char_mask);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, char_tag);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::CMP, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::JG, t);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, t);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_char_greater_contract_violation);
    }
  }

void compile_char_leq(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string t = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::MOV, vmcode::R11, vmcode::RCX);
    code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, char_mask);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, char_tag);
    code.add(vmcode::JNE, error);
    code.add(vmcode::MOV, vmcode::R11, vmcode::RDX);
    code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, char_mask);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, char_tag);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::CMP, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::JLE, t);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, t);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_char_leq_contract_violation);
    }
  }

void compile_char_geq(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string t = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::MOV, vmcode::R11, vmcode::RCX);
    code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, char_mask);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, char_tag);
    code.add(vmcode::JNE, error);
    code.add(vmcode::MOV, vmcode::R11, vmcode::RDX);
    code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, char_mask);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, char_tag);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::CMP, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::JGE, t);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, t);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_char_geq_contract_violation);
    }
  }

void compile_char_equal(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::MOV, vmcode::R11, vmcode::RCX);
    code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, char_mask);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, char_tag);
    code.add(vmcode::JNE, error);
    code.add(vmcode::MOV, vmcode::R11, vmcode::RDX);
    code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, char_mask);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, char_tag);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::CMP, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::SETE, vmcode::RAX);
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_char_equal_contract_violation);
    }
  }

void compile_string(vmcode& code, const compiler_options& ops)
  {
  if (ops.safe_primitives)
    {
    code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);
    code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 3);
    code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, 2);
    check_heap(code, re_string_heap_overflow);
    }
  auto done = label_to_string(label++);
  auto repeat = label_to_string(label++);
  auto remainder = label_to_string(label++);
  auto remainder_loop = label_to_string(label++);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);
  code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::INC, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, (uint64_t)string_tag << (uint64_t)block_shift);
  code.add(vmcode::OR, vmcode::RAX, vmcode::R15);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R15, ALLOC);
  code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::NUMBER, 0);

  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 0);
  code.add(vmcode::JE, done);
  code.add(vmcode::SHR, vmcode::RCX, vmcode::NUMBER, 8);
  code.add(vmcode::MOV8, MEM_ALLOC, vmcode::RCX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
  code.add(vmcode::JE, done);
  code.add(vmcode::SHR, vmcode::RDX, vmcode::NUMBER, 8);
  code.add(vmcode::MOV8, MEM_ALLOC, 1, vmcode::RDX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
  code.add(vmcode::JE, done);
  code.add(vmcode::SHR, vmcode::RSI, vmcode::NUMBER, 8);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RSI);
  code.add(vmcode::MOV8, MEM_ALLOC, 2, vmcode::RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 3);
  code.add(vmcode::JE, done);
  code.add(vmcode::SHR, vmcode::RDI, vmcode::NUMBER, 8);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RDI);
  code.add(vmcode::MOV8, MEM_ALLOC, 3, vmcode::RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 4);
  code.add(vmcode::JE, done);
  code.add(vmcode::SHR, vmcode::R8, vmcode::NUMBER, 8);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R8);
  code.add(vmcode::MOV8, MEM_ALLOC, 4, vmcode::RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 5);
  code.add(vmcode::JE, done);
  code.add(vmcode::SHR, vmcode::R9, vmcode::NUMBER, 8);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R9);
  code.add(vmcode::MOV8, MEM_ALLOC, 5, vmcode::RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 6);
  code.add(vmcode::JE, done);
  code.add(vmcode::SHR, vmcode::R12, vmcode::NUMBER, 8);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R12);
  code.add(vmcode::MOV8, MEM_ALLOC, 6, vmcode::RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 7);
  code.add(vmcode::JE, done);
  code.add(vmcode::SHR, vmcode::R14, vmcode::NUMBER, 8);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R14);
  code.add(vmcode::MOV8, MEM_ALLOC, 7, vmcode::RAX);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::NUMBER, 0);
  code.add(vmcode::SUB, vmcode::R11, vmcode::NUMBER, 8);
  code.add(vmcode::MOV, vmcode::RDX, LOCAL);
  code.add(vmcode::LABEL, repeat);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 8);
  code.add(vmcode::JL, remainder);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RDX);
  code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV8, MEM_ALLOC, vmcode::RAX);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RDX);
  code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV8, MEM_ALLOC, 1, vmcode::RAX);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RDX);
  code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV8, MEM_ALLOC, 2, vmcode::RAX);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RDX);
  code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV8, MEM_ALLOC, 3, vmcode::RAX);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RDX);
  code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV8, MEM_ALLOC, 4, vmcode::RAX);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RDX);
  code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV8, MEM_ALLOC, 5, vmcode::RAX);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RDX);
  code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV8, MEM_ALLOC, 6, vmcode::RAX);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RDX);
  code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV8, MEM_ALLOC, 7, vmcode::RAX);

  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::NUMBER, 0);
  code.add(vmcode::SUB, vmcode::R11, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::JMP, repeat);

  code.add(vmcode::LABEL, remainder);
  code.add(vmcode::MOV, vmcode::RCX, ALLOC);
  code.add(vmcode::LABEL, remainder_loop);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 0);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RDX);
  code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV8, vmcode::MEM_RCX, vmcode::RAX);
  code.add(vmcode::INC, vmcode::RCX);
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::JMP, remainder_loop);

  code.add(vmcode::LABEL, done);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  code.add(vmcode::JMP, CONTINUE);

  /*
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, (uint64_t)string_tag << (uint64_t)block_shift);
  code.add(vmcode::OR, vmcode::RAX, vmcode::R15);
  code.add(vmcode::MOV, vmcode::R15, ALLOC);
  code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::RCX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(2), vmcode::RDX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(3), vmcode::RSI);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 3);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(4), vmcode::RDI);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 4);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(5), vmcode::R8);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 5);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(6), vmcode::R9);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 6);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(7), vmcode::R12);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 7);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(8), vmcode::R14);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 8);
  code.add(vmcode::JE, done);


  code.add(vmcode::JMP, CONTINUE);
  */
  }

void compile_string_length(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  auto done = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error);
    }

  string_length(code, vmcode::RCX);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_string_length_contract_violation);
    }
  }

void compile_string_ref(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error); // assembly can jump short, but vm cannot
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // here check whether it is a string
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::SAR, vmcode::RDX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    {
    // optionally, check whether rdx is in bounds.
    std::string in_bounds = label_to_string(label++);
    std::string not_in_bounds = label_to_string(label++);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
    code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, block_size_mask);
    code.add(vmcode::AND, vmcode::RAX, vmcode::R15);
    code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
    code.add(vmcode::CMP, vmcode::RDX, vmcode::RAX);
    code.add(vmcode::JGE, not_in_bounds);
    code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, 0);
    code.add(vmcode::JGE, in_bounds);
    error_label(code, not_in_bounds, re_string_ref_out_of_bounds);
    code.add(vmcode::LABEL, in_bounds);
    }

  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, 8); // increase 8 for header
  code.add(vmcode::ADD, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 255);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, char_tag);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_string_ref_contract_violation);
    }
  }

void compile_string_set(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 3);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error); // assembly can jump short, but vm cannot
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // here check whether it is a string and a char
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::MOV, vmcode::R11, vmcode::RSI);
    code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, char_mask);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, char_tag);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::SAR, vmcode::RDX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    {
    // optionally, check whether rdx is in bounds.
    std::string in_bounds = label_to_string(label++);
    std::string not_in_bounds = label_to_string(label++);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
    code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, block_size_mask);
    code.add(vmcode::AND, vmcode::RAX, vmcode::R15);
    code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
    code.add(vmcode::CMP, vmcode::RDX, vmcode::RAX);
    code.add(vmcode::JGE, not_in_bounds);
    code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, 0);
    code.add(vmcode::JGE, in_bounds);
    error_label(code, not_in_bounds, re_string_set_out_of_bounds);
    code.add(vmcode::LABEL, in_bounds);
    }

  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, 8); // increase 8 for header
  code.add(vmcode::ADD, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RSI);
  code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::MOV8, vmcode::MEM_RCX, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RSI);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_string_set_contract_violation);
    }
  }

void compile_vector_length(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_vector(code, vmcode::RCX, vmcode::R11, error);
    }
  //code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, block_size_mask);
  code.add(vmcode::AND, vmcode::RAX, vmcode::R11);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_vector_length_contract_violation);
    }
  }

void compile_string_fill(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  auto length_done = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::PUSH, vmcode::RCX);
  string_length(code, vmcode::RCX);
  // r15 contains the length, rcx is pointer to string, rdx is character
  code.add(vmcode::POP, vmcode::RCX);

  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::SHR, vmcode::RDX, vmcode::NUMBER, 8); // get character value in dl


  auto fill_rest = label_to_string(label++);
  auto fill_full = label_to_string(label++);
  auto done = label_to_string(label++);
  code.add(vmcode::LABEL, fill_full);
  code.add(vmcode::CMP, vmcode::R15, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::JL, fill_rest);

  code.add(vmcode::MOV8, vmcode::MEM_RCX, vmcode::RDX);
  code.add(vmcode::MOV8, vmcode::MEM_RCX, 1, vmcode::RDX);
  code.add(vmcode::MOV8, vmcode::MEM_RCX, 2, vmcode::RDX);
  code.add(vmcode::MOV8, vmcode::MEM_RCX, 3, vmcode::RDX);
  code.add(vmcode::MOV8, vmcode::MEM_RCX, 4, vmcode::RDX);
  code.add(vmcode::MOV8, vmcode::MEM_RCX, 5, vmcode::RDX);
  code.add(vmcode::MOV8, vmcode::MEM_RCX, 6, vmcode::RDX);
  code.add(vmcode::MOV8, vmcode::MEM_RCX, 7, vmcode::RDX);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::SUB, vmcode::R15, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::JMP, fill_full);

  code.add(vmcode::LABEL, fill_rest);
  code.add(vmcode::MOV, vmcode::MEM_RCX, vmcode::NUMBER, 0);
  code.add(vmcode::CMP, vmcode::R15, vmcode::NUMBER, 0);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, vmcode::MEM_RCX, vmcode::RDX);
  code.add(vmcode::CMP, vmcode::R15, vmcode::NUMBER, 1);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, vmcode::MEM_RCX, 1, vmcode::RDX);
  code.add(vmcode::CMP, vmcode::R15, vmcode::NUMBER, 2);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, vmcode::MEM_RCX, 2, vmcode::RDX);
  code.add(vmcode::CMP, vmcode::R15, vmcode::NUMBER, 3);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, vmcode::MEM_RCX, 3, vmcode::RDX);
  code.add(vmcode::CMP, vmcode::R15, vmcode::NUMBER, 4);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, vmcode::MEM_RCX, 4, vmcode::RDX);
  code.add(vmcode::CMP, vmcode::R15, vmcode::NUMBER, 5);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, vmcode::MEM_RCX, 5, vmcode::RDX);
  code.add(vmcode::CMP, vmcode::R15, vmcode::NUMBER, 6);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, vmcode::MEM_RCX, 6, vmcode::RDX);
  code.add(vmcode::CMP, vmcode::R15, vmcode::NUMBER, 7);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, vmcode::MEM_RCX, 7, vmcode::RDX);
  code.add(vmcode::LABEL, done);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, skiwi_quiet_undefined);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_string_fill_contract_violation);
    }
  }

void compile_vector_fill(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_vector(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, block_size_mask);
  code.add(vmcode::AND, vmcode::RAX, vmcode::R11);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));

  auto repeat = label_to_string(label++);
  auto done = label_to_string(label++);

  code.add(vmcode::LABEL, repeat);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::RAX);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, vmcode::MEM_RCX, vmcode::RDX);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::DEC, vmcode::RAX);
  code.add(vmcode::JMP, repeat);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, skiwi_quiet_undefined);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_vector_fill_contract_violation);
    }
  }

void compile_make_vector(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 0);
    code.add(vmcode::JE, error);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 3);
    code.add(vmcode::JGE, error);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
    code.add(vmcode::SAR, vmcode::RAX, vmcode::NUMBER, 1);
    code.add(vmcode::INC, vmcode::RAX);
    check_heap(code, re_make_vector_heap_overflow);
    }
  auto done = label_to_string(label++);
  auto fill = label_to_string(label++);
  auto fill_undefined = label_to_string(label++);
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, (uint64_t)vector_tag << (uint64_t)block_shift);
  code.add(vmcode::OR, vmcode::RAX, vmcode::R15);
  code.add(vmcode::MOV, vmcode::R15, ALLOC);
  code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
  code.add(vmcode::JE, fill);
  code.add(vmcode::LABEL, fill_undefined);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, done);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::NUMBER, skiwi_undefined);
  code.add(vmcode::DEC, vmcode::RCX);
  code.add(vmcode::JMP, fill_undefined);
  code.add(vmcode::LABEL, fill);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, done);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RDX);
  code.add(vmcode::DEC, vmcode::RCX);
  code.add(vmcode::JMP, fill);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_make_vector_contract_violation);
    }
  }

void compile_vector(vmcode& code, const compiler_options& ops)
  {
  auto done = label_to_string(label++);
  auto done2 = label_to_string(label++);
  if (ops.safe_primitives)
    {
    code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);
    code.add(vmcode::INC, vmcode::RAX);
    check_heap(code, re_vector_heap_overflow);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, (uint64_t)vector_tag << (uint64_t)block_shift);
  code.add(vmcode::OR, vmcode::RAX, vmcode::R15);
  code.add(vmcode::MOV, vmcode::R15, ALLOC);
  code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 0);
  code.add(vmcode::JE, done); // assembly can jump short, but vm cannot
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::RCX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(2), vmcode::RDX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(3), vmcode::RSI);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 3);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(4), vmcode::RDI);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 4);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(5), vmcode::R8);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 5);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(6), vmcode::R9);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 6);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(7), vmcode::R12);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 7);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(8), vmcode::R14);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 8);
  code.add(vmcode::JE, done);

  code.add(vmcode::SUB, vmcode::R11, vmcode::NUMBER, 8);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(9));
  code.add(vmcode::MOV, vmcode::RDX, LOCAL);

  auto lab = label_to_string(label++);
  code.add(vmcode::LABEL, lab);

  code.add(vmcode::MOV, vmcode::RCX, vmcode::MEM_RDX);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RCX);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::JE, done2);
  code.add(vmcode::JMP, lab);

  code.add(vmcode::LABEL, done);
  code.add(vmcode::INC, vmcode::R11);
  code.add(vmcode::SHL, vmcode::R11, vmcode::NUMBER, 3);
  code.add(vmcode::ADD, ALLOC, vmcode::R11);
  code.add(vmcode::LABEL, done2);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_slot_ref(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::SAR, vmcode::RDX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    {
    // optionally, check whether rdx is in bounds.
    std::string in_bounds = label_to_string(label++);
    std::string not_in_bounds = label_to_string(label++);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
    code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, block_size_mask);
    code.add(vmcode::AND, vmcode::RAX, vmcode::R15); // get size of vector
    code.add(vmcode::CMP, vmcode::RDX, vmcode::RAX);
    code.add(vmcode::JGE, not_in_bounds);
    code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, 0);
    code.add(vmcode::JGE, in_bounds);
    error_label(code, not_in_bounds, re_slot_ref_out_of_bounds);
    code.add(vmcode::LABEL, in_bounds);
    }

  code.add(vmcode::INC, vmcode::RDX); // increase 1 for header
  code.add(vmcode::SHL, vmcode::RDX, vmcode::NUMBER, 3);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_slot_ref_contract_violation);
    }
  }

void compile_slot_set(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 3);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::SAR, vmcode::RDX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    {
    // optionally, check whether rdx is in bounds.
    std::string in_bounds = label_to_string(label++);
    std::string not_in_bounds = label_to_string(label++);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
    code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, block_size_mask);
    code.add(vmcode::AND, vmcode::RAX, vmcode::R15); // get size of block
    code.add(vmcode::CMP, vmcode::RDX, vmcode::RAX);
    code.add(vmcode::JGE, not_in_bounds);
    code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, 0);
    code.add(vmcode::JGE, in_bounds);
    error_label(code, not_in_bounds, re_slot_set_out_of_bounds);
    code.add(vmcode::LABEL, in_bounds);
    }

  code.add(vmcode::INC, vmcode::RDX); // increase 1 for header
  code.add(vmcode::SHL, vmcode::RDX, vmcode::NUMBER, 3);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RSI);
  code.add(vmcode::MOV, vmcode::MEM_RCX, vmcode::RAX);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_slot_set_contract_violation);
    }
  }

void compile_vector_ref(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // here check whether it is a vector
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_vector(code, vmcode::RCX, vmcode::R11, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::SAR, vmcode::RDX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    {
    // optionally, check whether rdx is in bounds.
    std::string in_bounds = label_to_string(label++);
    std::string not_in_bounds = label_to_string(label++);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
    code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, block_size_mask);
    code.add(vmcode::AND, vmcode::RAX, vmcode::R15); // get size of vector
    code.add(vmcode::CMP, vmcode::RDX, vmcode::RAX);
    code.add(vmcode::JGE, not_in_bounds);
    code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, 0);
    code.add(vmcode::JGE, in_bounds);
    error_label(code, not_in_bounds, re_vector_ref_out_of_bounds);
    code.add(vmcode::LABEL, in_bounds);
    }

  code.add(vmcode::INC, vmcode::RDX); // increase 1 for header
  code.add(vmcode::SHL, vmcode::RDX, vmcode::NUMBER, 3);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_vector_ref_contract_violation);
    }
  }

void compile_vector_set(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 3);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // here check whether it is a vector
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_vector(code, vmcode::RCX, vmcode::R11, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::SAR, vmcode::RDX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    {
    // optionally, check whether rdx is in bounds.
    std::string in_bounds = label_to_string(label++);
    std::string not_in_bounds = label_to_string(label++);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
    code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, block_size_mask);
    code.add(vmcode::AND, vmcode::RAX, vmcode::R15); // get size of vector
    code.add(vmcode::CMP, vmcode::RDX, vmcode::RAX);
    code.add(vmcode::JGE, not_in_bounds);
    code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, 0);
    code.add(vmcode::JGE, in_bounds);
    error_label(code, not_in_bounds, re_vector_set_out_of_bounds);
    code.add(vmcode::LABEL, in_bounds);
    }

  code.add(vmcode::INC, vmcode::RDX); // increase 1 for header
  code.add(vmcode::SHL, vmcode::RDX, vmcode::NUMBER, 3);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RSI);
  code.add(vmcode::MOV, vmcode::MEM_RCX, vmcode::RAX);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_vector_set_contract_violation);
    }
  }

void compile_halt(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_add1(vmcode& code, const compiler_options& ops)
  {
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 0);
  std::string lab_arg_ok = label_to_string(label++);
  std::string lab_is_fixnum = label_to_string(label++);
  std::string error;
  code.add(vmcode::JNE, lab_arg_ok);
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RAX); // no arguments, return error here
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_arg_ok);
  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, lab_is_fixnum);

  // here check whether it is a block
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }

  // get address of block
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);

  // here check whether it is a flonum
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);
    }

  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RCX, CELLS(1));
  double d = 1.0;
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, *(reinterpret_cast<uint64_t*>(&d)));
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::RAX);
  code.add(vmcode::ADDSD, vmcode::XMM0, vmcode::XMM1);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(vmcode::MOV, vmcode::R11, ALLOC);
  code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::XMM0);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);

  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_is_fixnum);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 2);
  code.add(vmcode::ADD, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_add1_contract_violation);
    }
  }


void compile_sub1(vmcode& code, const compiler_options& ops)
  {
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 0);
  std::string lab_arg_ok = label_to_string(label++);
  std::string lab_is_fixnum = label_to_string(label++);
  std::string error;
  code.add(vmcode::JNE, lab_arg_ok);
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RAX); // no arguments, return error here
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_arg_ok);
  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, lab_is_fixnum);

  // here check whether it is a block
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }

  // get address of block
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);

  // here check whether it is a flonum
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);
    }

  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RCX);
  double d = 1.0;
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, *(reinterpret_cast<uint64_t*>(&d)));
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::RAX);
  code.add(vmcode::SUBSD, vmcode::XMM0, vmcode::XMM1);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(vmcode::MOV, vmcode::R11, ALLOC);
  code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::XMM0);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);


  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_is_fixnum);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, -2);
  code.add(vmcode::ADD, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_sub1_contract_violation);
    }
  }

void compile_bitwise_and_2(vmcode& code, const compiler_options& ops)
  {
  code.add(vmcode::LABEL, "L_bitwise_and_2");
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::TEST, vmcode::RAX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::RET);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_bitwise_and_contract_violation);
    }
  }

void compile_bitwise_or_2(vmcode& code, const compiler_options& ops)
  {
  code.add(vmcode::LABEL, "L_bitwise_or_2");
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::TEST, vmcode::RAX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::OR, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::RET);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_bitwise_or_contract_violation);
    }
  }

void compile_bitwise_xor_2(vmcode& code, const compiler_options& ops)
  {
  code.add(vmcode::LABEL, "L_bitwise_xor_2");
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::TEST, vmcode::RAX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::RET);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_bitwise_xor_contract_violation);
    }
  }

void compile_max_2(vmcode& code, const compiler_options& ops)
  {
  code.add(vmcode::LABEL, "L_max_2");
  auto l1 = label_to_string(label++);
  auto l2 = label_to_string(label++);
  auto l3 = label_to_string(label++);
  auto done = label_to_string(label++);
  std::string error;
  if (ops.safe_primitives)
    error = label_to_string(label++);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l1);
  else
    code.add(vmcode::JE, l1); // rax is fixnum
  //rax is flonum
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  else
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  // rax and rbx are flonum
  // here check whether they are a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // check whether they contain flonums
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::UCOMISD, vmcode::XMM0, vmcode::XMM1);
  if (ops.safe_primitives)
    code.add(vmcode::JA, done);
  else
    code.add(vmcode::JA, done);
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::XMM1);
  if (ops.safe_primitives)
    code.add(vmcode::JMP, done);
  else
    code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l1);
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, l2); // rax and rbx are fixnum
  // rax is fixnum and rbx is flonum
  fix2int(code, vmcode::RAX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RAX);
  // here check whether RBX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::UCOMISD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::JA, done);
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l2);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::RBX);
  auto done_fx = label_to_string(label++);
  code.add(vmcode::JG, done_fx);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::LABEL, done_fx);
  code.add(vmcode::RET);
  code.add(vmcode::JMP, done);

  code.add(vmcode::LABEL, l3);
  // rax is flonum and rbx is fixnum
  // here check whether RAX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  fix2int(code, vmcode::RBX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM1, vmcode::RBX);
  int2fix(code, vmcode::RBX);
  code.add(vmcode::UCOMISD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::JA, done);
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::XMM1);
  //code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::MOV, vmcode::RAX, TEMP_FLONUM);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(1), vmcode::XMM0);
  code.add(vmcode::MOV, vmcode::RAX, TEMP_FLONUM);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, block_tag);
  code.add(vmcode::RET);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_max_contract_violation);
    }
  }

void compile_min_2(vmcode& code, const compiler_options& ops)
  {
  code.add(vmcode::LABEL, "L_min_2");
  auto l1 = label_to_string(label++);
  auto l2 = label_to_string(label++);
  auto l3 = label_to_string(label++);
  auto done = label_to_string(label++);
  std::string error;
  if (ops.safe_primitives)
    error = label_to_string(label++);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l1);
  else
    code.add(vmcode::JE, l1); // rax is fixnum
  //rax is flonum
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  else
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  // rax and rbx are flonum
  // here check whether they are a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // check whether they contain flonums
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::UCOMISD, vmcode::XMM0, vmcode::XMM1);
  if (ops.safe_primitives)
    code.add(vmcode::JB, done);
  else
    code.add(vmcode::JB, done);
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::XMM1);
  if (ops.safe_primitives)
    code.add(vmcode::JMP, done);
  else
    code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l1);
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, l2); // rax and rbx are fixnum
  // rax is fixnum and rbx is flonum
  fix2int(code, vmcode::RAX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RAX);
  // here check whether RBX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::UCOMISD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::JB, done);
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l2);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::RBX);
  auto done_fx = label_to_string(label++);
  code.add(vmcode::JL, done_fx);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::LABEL, done_fx);
  code.add(vmcode::RET);
  code.add(vmcode::JMP, done);

  code.add(vmcode::LABEL, l3);
  // rax is flonum and rbx is fixnum
  // here check whether RAX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  fix2int(code, vmcode::RBX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM1, vmcode::RBX);
  int2fix(code, vmcode::RBX);
  code.add(vmcode::UCOMISD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::JB, done);
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::XMM1);
  //code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::MOV, vmcode::RAX, TEMP_FLONUM);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(1), vmcode::XMM0);
  code.add(vmcode::MOV, vmcode::RAX, TEMP_FLONUM);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, block_tag);
  code.add(vmcode::RET);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_min_contract_violation);
    }
  }


void compile_add_2(vmcode& code, const compiler_options& ops)
  {
  code.add(vmcode::LABEL, "L_add_2");
  auto l1 = label_to_string(label++);
  auto l2 = label_to_string(label++);
  auto l3 = label_to_string(label++);
  auto done = label_to_string(label++);
  std::string error;
  if (ops.safe_primitives)
    error = label_to_string(label++);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l1);
  else
    code.add(vmcode::JE, l1); // rax is fixnum
  //rax is flonum
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  else
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  // rax and rbx are flonum
  // here check whether they are a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // check whether they contain flonums
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::ADDSD, vmcode::XMM0, vmcode::XMM1);
  if (ops.safe_primitives)
    code.add(vmcode::JMP, done);
  else
    code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l1);
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, l2); // rax and rbx are fixnum
  // rax is fixnum and rbx is flonum
  fix2int(code, vmcode::RAX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RAX);
  // here check whether RBX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::ADDSD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l2);
  code.add(vmcode::ADD, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::RET);

  code.add(vmcode::LABEL, l3);
  // rax is flonum and rbx is fixnum
  // here check whether RAX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  fix2int(code, vmcode::RBX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM1, vmcode::RBX);
  code.add(vmcode::ADDSD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::MOV, vmcode::RAX, TEMP_FLONUM);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(1), vmcode::XMM0);
  code.add(vmcode::MOV, vmcode::RAX, TEMP_FLONUM);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, block_tag);
  code.add(vmcode::RET);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_add_contract_violation);
    }
  }

void compile_subtract_2(vmcode& code, const compiler_options& ops)
  {
  code.add(vmcode::LABEL, "L_subtract_2");
  auto l1 = label_to_string(label++);
  auto l2 = label_to_string(label++);
  auto l3 = label_to_string(label++);
  auto done = label_to_string(label++);
  std::string error;
  if (ops.safe_primitives)
    error = label_to_string(label++);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l1);
  else
    code.add(vmcode::JE, l1); // rax is fixnum
  //rax is flonum
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  else
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  // rax and rbx are flonum
  // here check whether they are a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // check whether they contain flonums
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::SUBSD, vmcode::XMM0, vmcode::XMM1);
  if (ops.safe_primitives)
    code.add(vmcode::JMP, done);
  else
    code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l1);
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, l2); // rax and rbx are fixnum
  // rax is fixnum and rbx is flonum
  fix2int(code, vmcode::RAX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RAX);
  // here check whether RBX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::SUBSD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l2);
  code.add(vmcode::SUB, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::RET);

  code.add(vmcode::LABEL, l3);
  // rax is flonum and rbx is fixnum
  // here check whether RAX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  fix2int(code, vmcode::RBX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM1, vmcode::RBX);
  code.add(vmcode::SUBSD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::MOV, vmcode::RAX, TEMP_FLONUM);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(1), vmcode::XMM0);
  code.add(vmcode::MOV, vmcode::RAX, TEMP_FLONUM);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, block_tag);
  code.add(vmcode::RET);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_sub_contract_violation);
    }
  }

void compile_multiply_2(vmcode& code, const compiler_options& ops)
  {
  code.add(vmcode::LABEL, "L_multiply_2");
  auto l1 = label_to_string(label++);
  auto l2 = label_to_string(label++);
  auto l3 = label_to_string(label++);
  auto done = label_to_string(label++);
  std::string error;
  if (ops.safe_primitives)
    error = label_to_string(label++);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l1);
  else
    code.add(vmcode::JE, l1); // rax is fixnum
  //rax is flonum
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  else
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  // rax and rbx are flonum
  // here check whether they are a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // check whether they contain flonums
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::MULSD, vmcode::XMM0, vmcode::XMM1);
  if (ops.safe_primitives)
    code.add(vmcode::JMP, done);
  else
    code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l1);
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, l2); // rax and rbx are fixnum
  // rax is fixnum and rbx is flonum
  fix2int(code, vmcode::RAX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RAX);
  // here check whether RBX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::MULSD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l2);
  code.add(vmcode::IMUL, vmcode::RBX);
  code.add(vmcode::SAR, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::RET);

  code.add(vmcode::LABEL, l3);
  // rax is flonum and rbx is fixnum
  // here check whether RAX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  fix2int(code, vmcode::RBX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM1, vmcode::RBX);
  code.add(vmcode::MULSD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::MOV, vmcode::RAX, TEMP_FLONUM);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(1), vmcode::XMM0);
  code.add(vmcode::MOV, vmcode::RAX, TEMP_FLONUM);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, block_tag);
  code.add(vmcode::RET);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_mul_contract_violation);
    }
  }


void compile_divide_2(vmcode& code, const compiler_options& ops)
  {
  code.add(vmcode::LABEL, "L_divide_2");
  auto l1 = label_to_string(label++);
  auto l2 = label_to_string(label++);
  auto l3 = label_to_string(label++);
  auto done = label_to_string(label++);
  std::string error;
  if (ops.safe_primitives)
    error = label_to_string(label++);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l1);
  else
    code.add(vmcode::JE, l1); // rax is fixnum
  //rax is flonum
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  else
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  // rax and rbx are flonum
  // here check whether they are a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // check whether they contain flonums
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::DIVSD, vmcode::XMM0, vmcode::XMM1);
  if (ops.safe_primitives)
    code.add(vmcode::JMP, done);
  else
    code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l1);
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, l2); // rax and rbx are fixnum
  // rax is fixnum and rbx is flonum
  fix2int(code, vmcode::RAX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RAX);
  // here check whether RBX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::DIVSD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l2);
  fix2int(code, vmcode::RAX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RAX);
  fix2int(code, vmcode::RBX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM1, vmcode::RBX);
  code.add(vmcode::DIVSD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l3);
  // rax is flonum and rbx is fixnum
  // here check whether RAX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  fix2int(code, vmcode::RBX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM1, vmcode::RBX);
  code.add(vmcode::DIVSD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::MOV, vmcode::RAX, TEMP_FLONUM);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(1), vmcode::XMM0);
  code.add(vmcode::MOV, vmcode::RAX, TEMP_FLONUM);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, block_tag);
  code.add(vmcode::RET);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_div_contract_violation);
    }
  }

void compile_pairwise_compare(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::LABEL, "L_pairwise_compare");
  code.add(vmcode::PUSH, vmcode::RBX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
  std::string lab_arg_ok = label_to_string(label++);
  std::string lab_arg_no = label_to_string(label++);
  std::string lab_arg_yes = label_to_string(label++);
  code.add(vmcode::JG, lab_arg_ok);

  code.add(vmcode::LABEL, lab_arg_no);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f); // less than 2 arguments, return false
  code.add(vmcode::POP, vmcode::RBX);
  code.add(vmcode::JMP, CONTINUE);

  code.add(vmcode::LABEL, lab_arg_ok);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX); // 1st arg
  code.add(vmcode::MOV, vmcode::RBX, vmcode::RDX); // 2nd arg
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JE, lab_arg_no);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
  code.add(vmcode::JE, lab_arg_yes);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::MOV, vmcode::RBX, vmcode::RSI); // 3th arg
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JE, lab_arg_no);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 3);
  code.add(vmcode::JE, lab_arg_yes);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::MOV, vmcode::RBX, vmcode::RDI); // 4th arg
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JE, lab_arg_no);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 4);
  code.add(vmcode::JE, lab_arg_yes); // JES works for assembly but not for vm

  code.add(vmcode::MOV, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::MOV, vmcode::RBX, vmcode::R8); // 5th arg
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JE, lab_arg_no);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 5);
  code.add(vmcode::JE, lab_arg_yes);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::MOV, vmcode::RBX, vmcode::R9); // 6th arg
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JE, lab_arg_no);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 6);
  code.add(vmcode::JE, lab_arg_yes);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::MOV, vmcode::RBX, vmcode::R12); // 7th arg
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JE, lab_arg_no);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 7);
  code.add(vmcode::JE, lab_arg_yes);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::MOV, vmcode::RBX, vmcode::R14); // 8th arg
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JE, lab_arg_no);
  code.add(vmcode::SUB, vmcode::R11, vmcode::NUMBER, 8);
  code.add(vmcode::TEST, vmcode::R11, vmcode::R11);
  code.add(vmcode::JE, lab_arg_yes);
  code.add(vmcode::MOV, vmcode::RDX, LOCAL);
  auto lab3 = label_to_string(label++);
  code.add(vmcode::LABEL, lab3);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::MOV, vmcode::RBX, vmcode::MEM_RDX);
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JE, lab_arg_no);
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::JE, lab_arg_yes);
  code.add(vmcode::JMP, lab3);

  code.add(vmcode::LABEL, lab_arg_yes);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t); // less than 2 arguments, return false
  code.add(vmcode::POP, vmcode::RBX);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_fold_binary(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::LABEL, "L_fold_binary_operation");
  code.add(vmcode::PUSH, vmcode::RBX);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX); // 1st arg
  code.add(vmcode::MOV, vmcode::RBX, vmcode::RDX); // 2nd arg
  code.add(vmcode::CALL, vmcode::R15);

  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
  auto lab1 = label_to_string(label++);
  auto lab2 = label_to_string(label++);
  auto lab3 = label_to_string(label++);
  auto done = label_to_string(label++);
  code.add(vmcode::JNE, lab1);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, lab2);
  code.add(vmcode::MOV, vmcode::R11, TEMP_FLONUM);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_R11);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_R11, CELLS(1));
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::RAX);
  code.add(vmcode::MOV, vmcode::RAX, ALLOC);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, block_tag);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::LABEL, lab2);
  code.add(vmcode::POP, vmcode::RBX);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab1);
  code.add(vmcode::MOV, vmcode::RBX, vmcode::RSI); //3rd arg
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 3);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, vmcode::RBX, vmcode::RDI); //4th arg
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 4);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, vmcode::RBX, vmcode::R8); //5th arg
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 5);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, vmcode::RBX, vmcode::R9); //6th arg
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 6);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, vmcode::RBX, vmcode::R12); //7th arg
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 7);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, vmcode::RBX, vmcode::R14); //8th arg
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::SUB, vmcode::R11, vmcode::NUMBER, 8);
  code.add(vmcode::MOV, vmcode::RDX, LOCAL);
  code.add(vmcode::TEST, vmcode::R11, vmcode::R11);
  code.add(vmcode::JE, done);
  code.add(vmcode::LABEL, lab3);
  code.add(vmcode::MOV, vmcode::RBX, vmcode::MEM_RDX);
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::JE, done);
  code.add(vmcode::JMP, lab3);
  }

void compile_bitwise_and(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
  std::string lab_arg_ok = label_to_string(label++);
  std::string lab_one_arg = label_to_string(label++);
  code.add(vmcode::JG, lab_arg_ok);
  code.add(vmcode::JE, lab_one_arg);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, (uint64_t)re_bitwise_and_contract_violation);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, error_tag);
  code.add(vmcode::JMP, ERROR);
  code.add(vmcode::LABEL, lab_one_arg); // only 1 argument
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 0xffffffffffffffff);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_arg_ok);
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_bitwise_and_2");
  code.add(vmcode::JMP, "L_fold_binary_operation");
  }

void compile_bitwise_not(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::XOR, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFFF);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFFE);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_bitwise_not_contract_violation);
    }
  }

void compile_arithmetic_shift(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  auto shift_right = label_to_string(label++);
  code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, 0);
  code.add(vmcode::JL, shift_right);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::SHR, vmcode::RDX, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::SAL, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, shift_right);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::SAR, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::NEG, vmcode::RDX);
  code.add(vmcode::SHR, vmcode::RDX, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::SAR, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::SAL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_arithmetic_shift_contract_violation);
    }
  }

void compile_bitwise_or(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
  std::string lab_arg_ok = label_to_string(label++);
  std::string lab_one_arg = label_to_string(label++);
  code.add(vmcode::JG, lab_arg_ok);
  code.add(vmcode::JE, lab_one_arg);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, (uint64_t)re_bitwise_or_contract_violation);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, error_tag);
  code.add(vmcode::JMP, ERROR);
  code.add(vmcode::LABEL, lab_one_arg); // only 1 argument
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RAX);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_arg_ok);
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_bitwise_or_2");
  code.add(vmcode::JMP, "L_fold_binary_operation");
  }

void compile_bitwise_xor(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
  std::string lab_arg_ok = label_to_string(label++);
  std::string lab_one_arg = label_to_string(label++);
  code.add(vmcode::JG, lab_arg_ok);
  code.add(vmcode::JE, lab_one_arg);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, (uint64_t)re_bitwise_xor_contract_violation);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, error_tag);
  code.add(vmcode::JMP, ERROR);
  code.add(vmcode::LABEL, lab_one_arg); // only 1 argument
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RAX);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_arg_ok);
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_bitwise_xor_2");
  code.add(vmcode::JMP, "L_fold_binary_operation");
  }

void compile_max(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
  std::string lab_arg_ok = label_to_string(label++);
  std::string lab_one_arg = label_to_string(label++);
  code.add(vmcode::JG, lab_arg_ok);
  code.add(vmcode::JE, lab_one_arg);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, (uint64_t)re_max_contract_violation);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, error_tag);
  code.add(vmcode::JMP, ERROR);
  code.add(vmcode::LABEL, lab_one_arg); // only 1 argument
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_arg_ok);
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_max_2");
  code.add(vmcode::JMP, "L_fold_binary_operation");
  }

void compile_min(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
  std::string lab_arg_ok = label_to_string(label++);
  std::string lab_one_arg = label_to_string(label++);
  code.add(vmcode::JG, lab_arg_ok);
  code.add(vmcode::JE, lab_one_arg);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, (uint64_t)re_min_contract_violation);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, error_tag);
  code.add(vmcode::JMP, ERROR);
  code.add(vmcode::LABEL, lab_one_arg); // only 1 argument
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_arg_ok);
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_min_2");
  code.add(vmcode::JMP, "L_fold_binary_operation");
  }

void compile_add(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
  std::string lab_arg_ok = label_to_string(label++);
  std::string lab_one_arg = label_to_string(label++);
  code.add(vmcode::JG, lab_arg_ok);
  code.add(vmcode::JE, lab_one_arg);
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RAX); // no arguments, return error here
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_one_arg); // only 1 argument
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_arg_ok);
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_add_2");
  code.add(vmcode::JMP, "L_fold_binary_operation");
  }

void compile_sub(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    }
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
  std::string lab_arg_ok = label_to_string(label++);
  std::string lab_one_arg = label_to_string(label++);
  code.add(vmcode::JG, lab_arg_ok);
  code.add(vmcode::JE, lab_one_arg);
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RAX); // no arguments, return error here
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_one_arg); // only 1 argument
  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  std::string rcx_is_fixnum = label_to_string(label++);
  std::string done = label_to_string(label++);
  code.add(vmcode::JE, rcx_is_fixnum);
  if (ops.safe_primitives)
    {
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 0x8000000000000000);
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::RCX);
  code.add(vmcode::XORPD, vmcode::XMM0, vmcode::XMM1);
  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::XMM0);
  code.add(vmcode::MOV, vmcode::RAX, ALLOC);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, block_tag);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::JMP, CONTINUE);

  code.add(vmcode::LABEL, rcx_is_fixnum);
  code.add(vmcode::NEG, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_arg_ok);
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_subtract_2");
  code.add(vmcode::JMP, "L_fold_binary_operation");
  if (ops.safe_primitives)
    {
    error_label(code, error, re_sub_contract_violation);
    }
  }

void compile_mul(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
  std::string lab_arg_ok = label_to_string(label++);
  std::string lab_one_arg = label_to_string(label++);
  code.add(vmcode::JG, lab_arg_ok);
  code.add(vmcode::JE, lab_one_arg);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 2); // no arguments, return error here
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_one_arg); // only 1 argument
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_arg_ok);
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_multiply_2");
  code.add(vmcode::JMP, "L_fold_binary_operation");
  }

void compile_div(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    }
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
  std::string lab_arg_ok = label_to_string(label++);
  std::string lab_one_arg = label_to_string(label++);
  code.add(vmcode::JG, lab_arg_ok);
  code.add(vmcode::JE, lab_one_arg);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 2); // no arguments, return error here
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_one_arg); // only 1 argument
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  double d = 1.0;
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, *(reinterpret_cast<uint64_t*>(&d)));
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::RAX);
  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  std::string rcx_is_fixnum = label_to_string(label++);
  std::string done = label_to_string(label++);
  code.add(vmcode::JE, rcx_is_fixnum);
  if (ops.safe_primitives)
    {
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, rcx_is_fixnum);
  fix2int(code, vmcode::RCX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM1, vmcode::RCX);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::DIVSD, vmcode::XMM0, vmcode::XMM1);
  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::XMM0);
  code.add(vmcode::MOV, vmcode::RAX, ALLOC);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, block_tag);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_arg_ok);
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_divide_2");
  code.add(vmcode::JMP, "L_fold_binary_operation");
  if (ops.safe_primitives)
    {
    error_label(code, error, re_div_contract_violation);
    }
  }

void compile_equal(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_equal_2");
  code.add(vmcode::JMP, "L_pairwise_compare");
  }

void compile_not_equal(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_not_equal_2");
  code.add(vmcode::JMP, "L_pairwise_compare");
  }

void compile_less(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_less_2");
  code.add(vmcode::JMP, "L_pairwise_compare");
  }

void compile_leq(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_leq_2");
  code.add(vmcode::JMP, "L_pairwise_compare");
  }

void compile_geq(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_geq_2");
  code.add(vmcode::JMP, "L_pairwise_compare");
  }

void compile_greater(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_greater_2");
  code.add(vmcode::JMP, "L_pairwise_compare");
  }

void compile_equal_2(vmcode& code, const compiler_options& ops)
  {
  code.add(vmcode::LABEL, "L_equal_2");
  auto l1 = label_to_string(label++);
  auto l2 = label_to_string(label++);
  auto l3 = label_to_string(label++);
  auto done = label_to_string(label++);
  std::string error;
  if (ops.safe_primitives)
    error = label_to_string(label++);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l1);
  else
    code.add(vmcode::JE, l1); // rax is fixnum
  //rax is flonum
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  else
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  // rax and rbx are flonum
  // here check whether they are a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // check whether they contain flonums
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::CMPEQPD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM0);
  code.add(vmcode::OR, vmcode::RBX, vmcode::NUMBER, block_tag);
  if (ops.safe_primitives)
    code.add(vmcode::JMP, done);
  else
    code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l1);
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, l2); // rax and rbx are fixnum
  // rax is fixnum and rbx is flonum
  fix2int(code, vmcode::RAX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RAX);
  // here check whether RBX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::CMPEQPD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM0);
  code.add(vmcode::OR, vmcode::RBX, vmcode::NUMBER, block_tag);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l2);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::SETE, vmcode::RAX);
  code.add(vmcode::JMP, done);

  code.add(vmcode::LABEL, l3);
  // rax is flonum and rbx is fixnum
  // here check whether RAX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  fix2int(code, vmcode::RBX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM1, vmcode::RBX);
  int2fix(code, vmcode::RBX);
  code.add(vmcode::CMPEQPD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM0);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::RET);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_equal_contract_violation);
    }
  }

void compile_not_equal_2(vmcode& code, const compiler_options& ops)
  {
  code.add(vmcode::LABEL, "L_not_equal_2");
  auto l1 = label_to_string(label++);
  auto l2 = label_to_string(label++);
  auto l3 = label_to_string(label++);
  auto done = label_to_string(label++);
  std::string error;
  if (ops.safe_primitives)
    error = label_to_string(label++);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l1);
  else
    code.add(vmcode::JE, l1); // rax is fixnum
  //rax is flonum
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  else
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  // rax and rbx are flonum
  // here check whether they are a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // check whether they contain flonums
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::CMPEQPD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM0);
  code.add(vmcode::OR, vmcode::RBX, vmcode::NUMBER, block_tag);
  if (ops.safe_primitives)
    code.add(vmcode::JMP, done);
  else
    code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l1);
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, l2); // rax and rbx are fixnum
  // rax is fixnum and rbx is flonum
  fix2int(code, vmcode::RAX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RAX);
  // here check whether RBX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::CMPEQPD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM0);
  code.add(vmcode::OR, vmcode::RBX, vmcode::NUMBER, block_tag);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l2);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::SETE, vmcode::RAX);
  code.add(vmcode::JMP, done);

  code.add(vmcode::LABEL, l3);
  // rax is flonum and rbx is fixnum
  // here check whether RAX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  fix2int(code, vmcode::RBX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM1, vmcode::RBX);
  int2fix(code, vmcode::RBX);
  code.add(vmcode::CMPEQPD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM0);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::XOR, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::RET);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_not_equal_contract_violation);
    }
  }

void compile_less_2(vmcode& code, const compiler_options& ops)
  {
  code.add(vmcode::LABEL, "L_less_2");
  auto l1 = label_to_string(label++);
  auto l2 = label_to_string(label++);
  auto l3 = label_to_string(label++);
  auto done = label_to_string(label++);
  std::string error;
  if (ops.safe_primitives)
    error = label_to_string(label++);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l1);
  else
    code.add(vmcode::JE, l1); // rax is fixnum
  //rax is flonum
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  else
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  // rax and rbx are flonum
  // here check whether they are a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // check whether they contain flonums
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::CMPLTPD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM0);
  code.add(vmcode::OR, vmcode::RBX, vmcode::NUMBER, block_tag);
  if (ops.safe_primitives)
    code.add(vmcode::JMP, done);
  else
    code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l1);
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, l2); // rax and rbx are fixnum
  // rax is fixnum and rbx is flonum
  fix2int(code, vmcode::RAX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RAX);
  // here check whether RBX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::CMPLTPD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM0);
  code.add(vmcode::OR, vmcode::RBX, vmcode::NUMBER, block_tag);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l2);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::SETL, vmcode::RAX);
  code.add(vmcode::JMP, done);

  code.add(vmcode::LABEL, l3);
  // rax is flonum and rbx is fixnum
  // here check whether RAX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  fix2int(code, vmcode::RBX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM1, vmcode::RBX);
  int2fix(code, vmcode::RBX);
  code.add(vmcode::CMPLTPD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM0);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::RET);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_less_contract_violation);
    }
  }

void compile_leq_2(vmcode& code, const compiler_options& ops)
  {
  code.add(vmcode::LABEL, "L_leq_2");
  auto l1 = label_to_string(label++);
  auto l2 = label_to_string(label++);
  auto l3 = label_to_string(label++);
  auto done = label_to_string(label++);
  std::string error;
  if (ops.safe_primitives)
    error = label_to_string(label++);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l1);
  else
    code.add(vmcode::JE, l1); // rax is fixnum
  //rax is flonum
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  else
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  // rax and rbx are flonum
  // here check whether they are a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // check whether they contain flonums
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::CMPLEPD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM0);
  code.add(vmcode::OR, vmcode::RBX, vmcode::NUMBER, block_tag);
  if (ops.safe_primitives)
    code.add(vmcode::JMP, done);
  else
    code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l1);
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, l2); // rax and rbx are fixnum
  // rax is fixnum and rbx is flonum
  fix2int(code, vmcode::RAX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RAX);
  // here check whether RBX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::CMPLEPD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM0);
  code.add(vmcode::OR, vmcode::RBX, vmcode::NUMBER, block_tag);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l2);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::SETLE, vmcode::RAX);
  code.add(vmcode::JMP, done);

  code.add(vmcode::LABEL, l3);
  // rax is flonum and rbx is fixnum
  // here check whether RAX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  fix2int(code, vmcode::RBX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM1, vmcode::RBX);
  int2fix(code, vmcode::RBX);
  code.add(vmcode::CMPLEPD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM0);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::RET);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_leq_contract_violation);
    }
  }

void compile_geq_2(vmcode& code, const compiler_options& ops)
  {
  code.add(vmcode::LABEL, "L_geq_2");
  auto l1 = label_to_string(label++);
  auto l2 = label_to_string(label++);
  auto l3 = label_to_string(label++);
  auto done = label_to_string(label++);
  std::string error;
  if (ops.safe_primitives)
    error = label_to_string(label++);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l1);
  else
    code.add(vmcode::JE, l1); // rax is fixnum
  //rax is flonum
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  else
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  // rax and rbx are flonum
  // here check whether they are a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // check whether they contain flonums
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::CMPLEPD, vmcode::XMM1, vmcode::XMM0);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM1);
  code.add(vmcode::OR, vmcode::RBX, vmcode::NUMBER, block_tag);
  if (ops.safe_primitives)
    code.add(vmcode::JMP, done);
  else
    code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l1);
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, l2); // rax and rbx are fixnum
  // rax is fixnum and rbx is flonum
  fix2int(code, vmcode::RAX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RAX);
  // here check whether RBX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::CMPLEPD, vmcode::XMM1, vmcode::XMM0);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM1);
  code.add(vmcode::OR, vmcode::RBX, vmcode::NUMBER, block_tag);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l2);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::SETGE, vmcode::RAX);
  code.add(vmcode::JMP, done);

  code.add(vmcode::LABEL, l3);
  // rax is flonum and rbx is fixnum
  // here check whether RAX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  fix2int(code, vmcode::RBX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM1, vmcode::RBX);
  int2fix(code, vmcode::RBX);
  code.add(vmcode::CMPLEPD, vmcode::XMM1, vmcode::XMM0);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM1);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::RET);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_geq_contract_violation);
    }
  }

void compile_greater_2(vmcode& code, const compiler_options& ops)
  {
  code.add(vmcode::LABEL, "L_greater_2");
  auto l1 = label_to_string(label++);
  auto l2 = label_to_string(label++);
  auto l3 = label_to_string(label++);
  auto done = label_to_string(label++);
  std::string error;
  if (ops.safe_primitives)
    error = label_to_string(label++);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l1);
  else
    code.add(vmcode::JE, l1); // rax is fixnum
  //rax is flonum
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  else
    code.add(vmcode::JE, l3); // rax is flonum and rbx is fixnum
  // rax and rbx are flonum
  // here check whether they are a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // check whether they contain flonums
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::CMPLTPD, vmcode::XMM1, vmcode::XMM0);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM1);
  code.add(vmcode::OR, vmcode::RBX, vmcode::NUMBER, block_tag);
  if (ops.safe_primitives)
    code.add(vmcode::JMP, done);
  else
    code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l1);
  code.add(vmcode::TEST, vmcode::RBX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, l2); // rax and rbx are fixnum
  // rax is fixnum and rbx is flonum
  fix2int(code, vmcode::RAX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RAX);
  // here check whether RBX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RBX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RBX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RBX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RBX, CELLS(1));
  code.add(vmcode::CMPLTPD, vmcode::XMM1, vmcode::XMM0);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM1);
  code.add(vmcode::OR, vmcode::RBX, vmcode::NUMBER, block_tag);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l2);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::SETG, vmcode::RAX);

  code.add(vmcode::JMP, done);

  code.add(vmcode::LABEL, l3);
  // rax is flonum and rbx is fixnum
  // here check whether RAX is a block
  if (ops.safe_primitives)
    {
    code.add(vmcode::PUSH, vmcode::R11);
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RAX, vmcode::R11, error);
    code.add(vmcode::POP, vmcode::R11);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RAX, CELLS(1));
  fix2int(code, vmcode::RBX);
  code.add(vmcode::CVTSI2SD, vmcode::XMM1, vmcode::RBX);
  int2fix(code, vmcode::RBX);
  code.add(vmcode::CMPLTPD, vmcode::XMM1, vmcode::XMM0);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM1);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::RET);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_greater_contract_violation);
    }
  }

void compile_not(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, bool_f);
  // if comparison (last cmp call) succeeded, then sete puts 1 in al, otherwise 0
  code.add(vmcode::SETE, vmcode::RAX);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_is_fixnum(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::SETE, vmcode::RAX);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_is_flonum(vmcode& code, const compiler_options&)
  {
  auto lab_false = label_to_string(label++);
  jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::RAX, lab_false);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::RCX, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_is_vector(vmcode& code, const compiler_options&)
  {
  auto lab_false = label_to_string(label++);
  jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::RAX, lab_false);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_vector(code, vmcode::RCX, vmcode::RCX, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_is_pair(vmcode& code, const compiler_options&)
  {
  auto lab_false = label_to_string(label++);
  jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::RAX, lab_false);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_pair(code, vmcode::RCX, vmcode::RCX, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_is_string(vmcode& code, const compiler_options&)
  {
  auto lab_false = label_to_string(label++);
  jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::RAX, lab_false);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::RCX, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_is_symbol(vmcode& code, const compiler_options&)
  {
  auto lab_false = label_to_string(label++);
  jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::RAX, lab_false);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_symbol(code, vmcode::RCX, vmcode::RCX, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_is_promise(vmcode& code, const compiler_options&)
  {
  auto lab_false = label_to_string(label++);
  jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::RAX, lab_false);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_promise(code, vmcode::RCX, vmcode::RCX, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_is_closure(vmcode& code, const compiler_options&)
  {
  auto lab_false = label_to_string(label++);
  jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::RAX, lab_false);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_closure(code, vmcode::RCX, vmcode::RCX, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_is_procedure(vmcode& code, const compiler_options&)
  {
  auto lab_false = label_to_string(label++);
  auto check_closure = label_to_string(label++);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, procedure_mask);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::NUMBER, procedure_tag);
  code.add(vmcode::JNE, check_closure);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, check_closure);
  jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::RAX, lab_false);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_closure(code, vmcode::RCX, vmcode::RCX, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_is_nil(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, nil);
  code.add(vmcode::SETE, vmcode::RAX);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_is_eof_object(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, eof_tag);
  code.add(vmcode::SETE, vmcode::RAX);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_is_char(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::CMP8, vmcode::RCX, vmcode::NUMBER, char_tag);
  // if comparison (last cmp call) succeeded, then sete puts 1 in al, otherwise 0
  code.add(vmcode::SETE, vmcode::RAX);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_is_boolean(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::AND8, vmcode::RCX, vmcode::NUMBER, 247);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, bool_f);
  code.add(vmcode::SETE, vmcode::RAX);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_is_zero(vmcode& code, const compiler_options& ops)
  {
  auto l1 = label_to_string(label++);
  auto l2 = label_to_string(label++);
  auto l3 = label_to_string(label++);
  auto done = label_to_string(label++);
  std::string error;
  if (ops.safe_primitives)
    error = label_to_string(label++);
  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, l1); // rcx is fixnum
  //rcx is flonum  
  //here check whether rcx is a block
  if (ops.safe_primitives)
    {
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // check whether it contains a flonum
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::XMM1, vmcode::RAX);
  code.add(vmcode::CMPEQPD, vmcode::XMM0, vmcode::XMM1);
  code.add(vmcode::MOVMSKPD, vmcode::RAX, vmcode::XMM0);
  code.add(vmcode::JMP, done);
  code.add(vmcode::LABEL, l1);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 0);
  code.add(vmcode::SETE, vmcode::RAX);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_is_zero_contract_violation);
    }
  }

void compile_length(vmcode& code, const compiler_options& ops)
  {
  auto lab_start = label_to_string(label++);
  auto lab_finish = label_to_string(label++);
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::XOR, vmcode::R15, vmcode::R15);
  code.add(vmcode::LABEL, lab_start);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, nil);
  code.add(vmcode::JE, lab_finish);
  if (ops.safe_primitives)
    {
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_pair(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::INC, vmcode::R15);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX, CELLS(2));
  code.add(vmcode::MOV, vmcode::RCX, vmcode::RAX);
  code.add(vmcode::JMP, lab_start);
  code.add(vmcode::LABEL, lab_finish);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_length_contract_violation);
    }
  }

void compile_list(vmcode& code, const compiler_options& ops)
  {
  std::string not_empty = label_to_string(label++);
  std::string done = label_to_string(label++);
  std::string repeat = label_to_string(label++);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 0);
  code.add(vmcode::JNE, not_empty);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, nil);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, not_empty);

  if (ops.safe_primitives)
    {
    code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);
    code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 1);
    code.add(vmcode::ADD, vmcode::RAX, vmcode::R11); // shl 1 and add for * 3
    check_heap(code, re_list_heap_overflow);
    }

  code.add(vmcode::MOV, vmcode::R15, ALLOC);
  code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);

  uint64_t header = make_block_header(2, T_PAIR);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::RCX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
  code.add(vmcode::JE, done);

  code.add(vmcode::MOV, vmcode::RCX, ALLOC);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(3));
  code.add(vmcode::OR, vmcode::RCX, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(2), vmcode::RCX);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(3));
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::RDX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
  code.add(vmcode::JE, done);

  code.add(vmcode::MOV, vmcode::RCX, ALLOC);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(3));
  code.add(vmcode::OR, vmcode::RCX, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(2), vmcode::RCX);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(3));
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::RSI);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 3);
  code.add(vmcode::JE, done);

  code.add(vmcode::MOV, vmcode::RCX, ALLOC);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(3));
  code.add(vmcode::OR, vmcode::RCX, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(2), vmcode::RCX);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(3));
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::RDI);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 4);
  code.add(vmcode::JE, done);

  code.add(vmcode::MOV, vmcode::RCX, ALLOC);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(3));
  code.add(vmcode::OR, vmcode::RCX, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(2), vmcode::RCX);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(3));
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::R8);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 5);
  code.add(vmcode::JE, done);

  code.add(vmcode::MOV, vmcode::RCX, ALLOC);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(3));
  code.add(vmcode::OR, vmcode::RCX, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(2), vmcode::RCX);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(3));
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::R9);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 6);
  code.add(vmcode::JE, done);

  code.add(vmcode::MOV, vmcode::RCX, ALLOC);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(3));
  code.add(vmcode::OR, vmcode::RCX, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(2), vmcode::RCX);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(3));
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::R12);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 7);
  code.add(vmcode::JE, done);

  code.add(vmcode::MOV, vmcode::RCX, ALLOC);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(3));
  code.add(vmcode::OR, vmcode::RCX, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(2), vmcode::RCX);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(3));
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::R14);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 8);
  code.add(vmcode::JE, done);

  code.add(vmcode::SUB, vmcode::R11, vmcode::NUMBER, 8);
  code.add(vmcode::MOV, vmcode::RDX, LOCAL);
  code.add(vmcode::LABEL, repeat);
  code.add(vmcode::MOV, vmcode::RSI, vmcode::MEM_RDX);

  code.add(vmcode::MOV, vmcode::RCX, ALLOC);
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(3));
  code.add(vmcode::OR, vmcode::RCX, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(2), vmcode::RCX);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(3));
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::RSI);
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::JE, done);
  code.add(vmcode::JMP, repeat);

  code.add(vmcode::LABEL, done);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, nil);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(2), vmcode::RAX);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(3));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  code.add(vmcode::JMP, CONTINUE);


  }

void compile_cons(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    error = label_to_string(label++);
  uint64_t header = make_block_header(2, T_PAIR);
  if (ops.safe_primitives)
    {
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    if (ops.safe_cons)
      {
      code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 3);
      check_heap(code, re_cons_heap_overflow);
      }
    }
  code.add(vmcode::MOV, vmcode::R11, ALLOC);
  code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::RCX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(2), vmcode::RDX);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(3));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_cons_contract_violation);
    }
  }

void compile_set_car(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    error = label_to_string(label++);
  if (ops.safe_primitives)
    {
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // check whether it contains a pair
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_pair(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::MOV, vmcode::MEM_RCX, CELLS(1), vmcode::RDX);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_set_car_contract_violation);
    }
  }

void compile_set_cdr(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    error = label_to_string(label++);
  if (ops.safe_primitives)
    {
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // check whether it contains a pair
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_pair(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::MOV, vmcode::MEM_RCX, CELLS(2), vmcode::RDX);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_set_cdr_contract_violation);
    }
  }

void compile_car(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    error = label_to_string(label++);
  if (ops.safe_primitives)
    {
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // check whether it contains a pair
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_pair(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_car_contract_violation);
    }
  }

void compile_cdr(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    error = label_to_string(label++);
  if (ops.safe_primitives)
    {
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  // check whether it contains a pair
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_pair(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX, CELLS(2));
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_cdr_contract_violation);
    }
  }

void compile_mark(vmcode& code, const compiler_options&)
  {
  /*mark should not change rax or r11. clobbers rbx, rdx, r8, r9*/
  code.add(vmcode::LABEL, "L_mark");
  auto done = label_to_string(label++);
  auto not_marked_yet = label_to_string(label++);


  code.add(vmcode::MOV, vmcode::R15, vmcode::MEM_RAX);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::R15);
  code.add(vmcode::AND, vmcode::RDX, vmcode::NUMBER, block_mask);
  code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, block_tag);
  code.add(vmcode::JNE, done);
  code.add(vmcode::AND, vmcode::R15, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::CMP, vmcode::R15, FROM_SPACE);
  code.add(vmcode::JL, done);
  code.add(vmcode::CMP, vmcode::R15, FROM_SPACE_END);
  code.add(vmcode::JGE, done);
  code.add(vmcode::MOV, vmcode::RBX, vmcode::MEM_R15); // rbx contains header
  code.add(vmcode::MOV, vmcode::R9, vmcode::NUMBER, block_mask_bit);
  code.add(vmcode::TEST, vmcode::RBX, vmcode::R9);
  code.add(vmcode::JE, not_marked_yet);
  // this block was already marked. The header is replaced by the new address, marked
  // with block_mask_bit, so what we need to do now is remove the mark, and update
  // [rax] with the new address
  code.add(vmcode::MOV, vmcode::RDX, vmcode::NUMBER, ~block_mask_bit);
  code.add(vmcode::AND, vmcode::RBX, vmcode::RDX);
  code.add(vmcode::MOV, vmcode::MEM_RAX, vmcode::RBX);
  code.add(vmcode::RET);
  code.add(vmcode::LABEL, not_marked_yet);

  code.add(vmcode::MOV, vmcode::R8, vmcode::RBX);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::NUMBER, block_size_mask);
  code.add(vmcode::AND, vmcode::R8, vmcode::RDX); // get size of block in r8

  code.add(vmcode::MOV, vmcode::MEM_RDI, vmcode::RBX); // write unmarked header in tospace
  code.add(vmcode::MOV, vmcode::RDX, vmcode::RDI);
  code.add(vmcode::OR, vmcode::RDX, vmcode::NUMBER, block_tag); // mark as block
  code.add(vmcode::MOV, vmcode::MEM_RAX, vmcode::RDX); // let original pointer point to the object in the new location
  code.add(vmcode::OR, vmcode::RDX, vmcode::R9); // mark address of new position.
  code.add(vmcode::MOV, vmcode::MEM_R15, vmcode::RDX); // update old position of header with marked new address

  code.add(vmcode::ADD, vmcode::RDI, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, vmcode::R15, vmcode::NUMBER, CELLS(1));
  // copy the remaining entries in the block
  auto copy = label_to_string(label++);
  code.add(vmcode::LABEL, copy);
  code.add(vmcode::TEST, vmcode::R8, vmcode::R8);
  code.add(vmcode::JE, done);
  code.add(vmcode::DEC, vmcode::R8);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::MEM_R15);
  code.add(vmcode::MOV, vmcode::MEM_RDI, vmcode::RDX); // =>crash
  code.add(vmcode::ADD, vmcode::RDI, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, vmcode::R15, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::JMP, copy);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::RET);
  }

void compile_reclaim(vmcode& code, const compiler_options& ops)
  {
  auto reclaim = label_to_string(label++);
  code.add(vmcode::CMP, ALLOC, LIMIT);
  code.add(vmcode::JG, reclaim);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, reclaim);
  compile_reclaim_garbage(code, ops);
  }

void compile_reclaim_garbage(vmcode& code, const compiler_options&)
  {
  auto no_heap = label_to_string(label++);

  code.add(vmcode::CMP, ALLOC, FROM_SPACE_END);
  code.add(vmcode::JG, no_heap);

  code.add(vmcode::PUSH, vmcode::RBX);

  // run over all registers (in GC_SAVE) and locals. R11 contains the number of items.  
  code.add(vmcode::MOV, vmcode::R11, NUMBER_OF_LOCALS);
  code.add(vmcode::ADD, vmcode::R11, vmcode::NUMBER, 8); // add the 8 free registers

  code.add(vmcode::MOV, vmcode::RAX, GC_SAVE);
  code.add(vmcode::MOV, vmcode::MEM_RAX, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(1), vmcode::RDX);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(2), vmcode::RSI);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(3), vmcode::RDI);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(4), vmcode::R8);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(5), vmcode::R9);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(6), vmcode::R12);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(7), vmcode::R14);


  code.add(vmcode::MOV, vmcode::RSI, TO_SPACE); // rsi = alloc-ptr
  code.add(vmcode::MOV, vmcode::RDI, vmcode::RSI);

  /*
  All local variables are now in memory in sequential order, as gc_save and locals follow each other (see context creation).
  We will now mark them with the block_mask_bit.
  */
  auto mark_local_rep = label_to_string(label++);
  auto mark_local_done = label_to_string(label++);
  code.add(vmcode::LABEL, mark_local_rep);
  code.add(vmcode::TEST, vmcode::R11, vmcode::R11);
  code.add(vmcode::JE, mark_local_done);
  code.add(vmcode::CALL, "L_mark");
  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::JMP, mark_local_rep);
  code.add(vmcode::LABEL, mark_local_done);

  /*
  Next we mark the globals with the block_mask_bit.
  */
  code.add(vmcode::MOV, vmcode::RAX, GLOBALS);
  code.add(vmcode::MOV, vmcode::R11, GLOBALS_END);
  code.add(vmcode::SUB, vmcode::R11, GLOBALS);
  code.add(vmcode::SHR, vmcode::R11, vmcode::NUMBER, 3);
  auto mark_global_rep = label_to_string(label++);
  auto mark_global_done = label_to_string(label++);
  //auto skip_mark = label_to_string(label++);
  code.add(vmcode::LABEL, mark_global_rep);
  code.add(vmcode::TEST, vmcode::R11, vmcode::R11);
  code.add(vmcode::JE, mark_global_done);
  code.add(vmcode::CMP, vmcode::MEM_RAX, vmcode::NUMBER, unalloc_tag);
  code.add(vmcode::JE, mark_global_done);
  //code.add(vmcode::JE, skip_mark);
  code.add(vmcode::CALL, "L_mark");
  //code.add(vmcode::LABEL, skip_mark);
  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::JMP, mark_global_rep);
  code.add(vmcode::LABEL, mark_global_done);



  auto cmp_rsi_rdi_loop = label_to_string(label++);
  /*
  Next we update the items saved on the stack.
  */

  code.add(vmcode::MOV, vmcode::RAX, STACK);
  code.add(vmcode::MOV, vmcode::R11, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::RAX, STACK_TOP);
  code.add(vmcode::SUB, vmcode::R11, vmcode::RAX);
  code.add(vmcode::SHR, vmcode::R11, vmcode::NUMBER, 3);
  //code.add(vmcode::SUB, vmcode::R11, vmcode::NUMBER, 2); // r11 and rbx are pushed on the stack at the top of this method
  auto mark_rsp_rep = label_to_string(label++);
  code.add(vmcode::LABEL, mark_rsp_rep);
  code.add(vmcode::TEST, vmcode::R11, vmcode::R11);
  code.add(vmcode::JE, cmp_rsi_rdi_loop);
  code.add(vmcode::CALL, "L_mark");
  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::JMP, mark_rsp_rep);



  code.add(vmcode::LABEL, cmp_rsi_rdi_loop);
  auto rsi_equals_rdi = label_to_string(label++);
  auto simple_block = label_to_string(label++);
  auto complicated_block = label_to_string(label++);
  auto closure_block = label_to_string(label++);
  code.add(vmcode::CMP, vmcode::RSI, vmcode::RDI);
  code.add(vmcode::JE, rsi_equals_rdi);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RSI); // get header
  code.add(vmcode::MOV, vmcode::RCX, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::NUMBER, block_size_mask);
  code.add(vmcode::AND, vmcode::RCX, vmcode::RDX); // get size of block in rcx
  code.add(vmcode::MOV, vmcode::RDX, vmcode::RAX);
  code.add(vmcode::SHR, vmcode::RDX, vmcode::NUMBER, block_shift);
  code.add(vmcode::AND, vmcode::RDX, vmcode::NUMBER, block_header_mask); // rdx contains the type
  code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, closure_tag);
  code.add(vmcode::JE, closure_block);
  code.add(vmcode::JG, complicated_block);
  /*
  code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, string_tag);
  code.add(vmcode::JE, simple_block);
  code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, symbol_tag);
  code.add(vmcode::JE, simple_block);
  code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, flonum_tag);
  code.add(vmcode::JE, simple_block);
  code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, closure_tag);
  code.add(vmcode::JE, closure_block);
  code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, pair_tag);
  code.add(vmcode::JE, complicated_block);
  code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, vector_tag);
  code.add(vmcode::JE, complicated_block);
  */
  code.add(vmcode::LABEL, simple_block);
  code.add(vmcode::INC, vmcode::RCX);
  code.add(vmcode::SHL, vmcode::RCX, vmcode::NUMBER, 3);
  code.add(vmcode::ADD, vmcode::RSI, vmcode::RCX);
  code.add(vmcode::JMP, cmp_rsi_rdi_loop);
  code.add(vmcode::LABEL, closure_block);
  code.add(vmcode::ADD, vmcode::RSI, vmcode::NUMBER, CELLS(1)); // skip label address of closure
  code.add(vmcode::DEC, vmcode::RCX);
  // now treat closure as complicated block
  code.add(vmcode::LABEL, complicated_block);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RSI);
  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  auto complicated_block_loop = label_to_string(label++);
  auto complicated_block_done = label_to_string(label++);
  code.add(vmcode::LABEL, complicated_block_loop);
  code.add(vmcode::TEST, vmcode::RCX, vmcode::RCX);
  code.add(vmcode::JE, complicated_block_done);
  code.add(vmcode::CALL, "L_mark");
  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::DEC, vmcode::RCX);
  code.add(vmcode::JMP, complicated_block_loop);
  code.add(vmcode::LABEL, complicated_block_done);
  code.add(vmcode::MOV, vmcode::RSI, vmcode::RAX);
  code.add(vmcode::JMP, cmp_rsi_rdi_loop);
  code.add(vmcode::LABEL, rsi_equals_rdi);

  // swap spaces
  code.add(vmcode::MOV, vmcode::RDX, FROM_SPACE);
  code.add(vmcode::MOV, vmcode::RAX, TO_SPACE);
  code.add(vmcode::MOV, FROM_SPACE, vmcode::RAX);
  code.add(vmcode::MOV, TO_SPACE, vmcode::RDX);
  code.add(vmcode::MOV, vmcode::RDX, FROM_SPACE_END);
  code.add(vmcode::MOV, vmcode::RAX, TO_SPACE_END);
  code.add(vmcode::MOV, FROM_SPACE_END, vmcode::RAX);
  code.add(vmcode::MOV, TO_SPACE_END, vmcode::RDX);

  code.add(vmcode::MOV, ALLOC, vmcode::RSI);
  code.add(vmcode::MOV, vmcode::RAX, FROM_SPACE_END);
  code.add(vmcode::MOV, vmcode::R11, FROMSPACE_RESERVE);
  code.add(vmcode::SHL, vmcode::R11, vmcode::NUMBER, 3);
  code.add(vmcode::SUB, vmcode::RAX, vmcode::R11);
  code.add(vmcode::MOV, LIMIT, vmcode::RAX);

  code.add(vmcode::MOV, vmcode::RAX, GC_SAVE);
  code.add(vmcode::MOV, vmcode::RCX, vmcode::MEM_RAX);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::MEM_RAX, CELLS(1));
  code.add(vmcode::MOV, vmcode::RSI, vmcode::MEM_RAX, CELLS(2));
  code.add(vmcode::MOV, vmcode::RDI, vmcode::MEM_RAX, CELLS(3));
  code.add(vmcode::MOV, vmcode::R8, vmcode::MEM_RAX, CELLS(4));
  code.add(vmcode::MOV, vmcode::R9, vmcode::MEM_RAX, CELLS(5));
  code.add(vmcode::MOV, vmcode::R12, vmcode::MEM_RAX, CELLS(6));
  code.add(vmcode::MOV, vmcode::R14, vmcode::MEM_RAX, CELLS(7));
  /*
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, unalloc_tag);
  code.add(vmcode::MOV, vmcode::R15, TO_SPACE);
  code.add(vmcode::MOV, vmcode::R11, TO_SPACE_END);
  code.add(vmcode::SUB, vmcode::R11, TO_SPACE);
  code.add(vmcode::SHR, vmcode::R11, vmcode::NUMBER, 3);
  auto make_zero_loop = label_to_string(label++);
  auto make_zero_done = label_to_string(label++);
  code.add(vmcode::LABEL, make_zero_loop);
  code.add(vmcode::TEST, vmcode::R11, vmcode::R11);
  code.add(vmcode::JE, make_zero_done);
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::MOV, vmcode::MEM_R15, vmcode::RAX);
  code.add(vmcode::ADD, vmcode::R15, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::JMP, make_zero_loop);
  code.add(vmcode::LABEL, make_zero_done);
  */
  code.add(vmcode::POP, vmcode::RBX); // continue value

  code.add(vmcode::CMP, ALLOC, FROM_SPACE_END);
  code.add(vmcode::JG, no_heap);

  code.add(vmcode::JMP, CONTINUE);

  error_label(code, no_heap, re_heap_full);
  }

void compile_structurally_equal(vmcode& code, const compiler_options&, const std::string& label_name)
  {
  /*
  compares two objects structurally:
  rax and r11 are the input arguments
  rax contains a boolean on output
  r15 is clobbered
  */
  code.add(vmcode::LABEL, label_name);
  auto not_equal = label_to_string(label++);
  auto fail = label_to_string(label++);
  auto cleanup = label_to_string(label++);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::R11);
  code.add(vmcode::JNE, not_equal);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::RET);
  code.add(vmcode::LABEL, fail);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::RET);
  code.add(vmcode::LABEL, not_equal);
  jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R15, fail);
  jump_if_arg_is_not_block(code, vmcode::R11, vmcode::R15, fail);
  //get addresses of the blocks
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  //compare headers
  code.add(vmcode::MOV, vmcode::R15, vmcode::MEM_RAX);
  code.add(vmcode::CMP, vmcode::MEM_R11, vmcode::R15);
  code.add(vmcode::JNE, fail);
  //compare contents
  code.add(vmcode::PUSH, vmcode::RCX);
  code.add(vmcode::PUSH, vmcode::RSI);
  //get length in rcx
  code.add(vmcode::MOV, vmcode::RCX, vmcode::R15);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, block_size_mask);
  code.add(vmcode::AND, vmcode::RCX, vmcode::R15);
  //get address again in r15, r11 still contains the other address
  code.add(vmcode::MOV, vmcode::R15, vmcode::RAX);
  //init to true
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);

  auto repeat = label_to_string(label++);
  auto fail2 = label_to_string(label++);
  code.add(vmcode::LABEL, repeat);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, cleanup);
  //get next items
  code.add(vmcode::ADD, vmcode::R15, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, vmcode::R11, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::RSI, vmcode::MEM_R15);
  code.add(vmcode::CMP, vmcode::RSI, vmcode::MEM_R11);
  code.add(vmcode::JNE, fail2);
  code.add(vmcode::DEC, vmcode::RCX);
  code.add(vmcode::JMP, repeat);
  code.add(vmcode::LABEL, fail2);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::LABEL, cleanup);
  code.add(vmcode::POP, vmcode::RSI);
  code.add(vmcode::POP, vmcode::RCX);
  code.add(vmcode::RET);
  }

void compile_recursively_equal(vmcode& code, const compiler_options&, const std::string& label_name)
  {
  /*
  compares two objects structurally:
  rax and r11 are the input arguments
  rax contains a boolean on output
  r15 is clobbered
  */
  code.add(vmcode::LABEL, label_name);
  auto not_equal = label_to_string(label++);
  auto fail = label_to_string(label++);
  auto cleanup = label_to_string(label++);
  auto simple_type = label_to_string(label++);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::R11);
  code.add(vmcode::JNE, not_equal);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::RET);
  code.add(vmcode::LABEL, fail);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::RET);
  code.add(vmcode::LABEL, not_equal);
  jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R15, fail);
  jump_if_arg_is_not_block(code, vmcode::R11, vmcode::R15, fail);
  //get addresses of the blocks
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  //compare headers
  code.add(vmcode::MOV, vmcode::R15, vmcode::MEM_RAX);
  code.add(vmcode::CMP, vmcode::MEM_R11, vmcode::R15);
  code.add(vmcode::JNE, fail);
  code.add(vmcode::PUSH, vmcode::R15);
  code.add(vmcode::SHR, vmcode::R15, vmcode::NUMBER, block_shift);
  code.add(vmcode::AND, vmcode::R15, vmcode::NUMBER, block_header_mask); // r15 now contains the type of block
  code.add(vmcode::CMP, vmcode::R15, vmcode::NUMBER, closure_tag); // if smaller than 'closure_tag', then simple type
  code.add(vmcode::POP, vmcode::R15);
  code.add(vmcode::JL, simple_type);
  //compare contents
  code.add(vmcode::PUSH, vmcode::RCX);
  //code.add(vmcode::PUSH, vmcode::RSI);
  //get length in rcx
  code.add(vmcode::MOV, vmcode::RCX, vmcode::R15);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, block_size_mask);
  code.add(vmcode::AND, vmcode::RCX, vmcode::R15);
  //get address again in r15, r11 still contains the other address
  code.add(vmcode::MOV, vmcode::R15, vmcode::RAX);
  //init to true
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);

  auto repeat = label_to_string(label++);
  code.add(vmcode::LABEL, repeat);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, cleanup);
  //get next items
  code.add(vmcode::ADD, vmcode::R15, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, vmcode::R11, vmcode::NUMBER, CELLS(1));

  code.add(vmcode::PUSH, vmcode::R11);
  code.add(vmcode::PUSH, vmcode::R15);
  code.add(vmcode::PUSH, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_R11);
  code.add(vmcode::MOV, vmcode::R11, vmcode::MEM_R15);
  code.add(vmcode::CALL, label_name);
  code.add(vmcode::POP, vmcode::RCX);
  code.add(vmcode::POP, vmcode::R15);
  code.add(vmcode::POP, vmcode::R11);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JE, cleanup);

  code.add(vmcode::DEC, vmcode::RCX);
  code.add(vmcode::JMP, repeat);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::LABEL, cleanup);
  //code.add(vmcode::POP, vmcode::RSI);
  code.add(vmcode::POP, vmcode::RCX);
  code.add(vmcode::RET);

  code.add(vmcode::LABEL, simple_type);
  code.add(vmcode::PUSH, vmcode::RCX);
  code.add(vmcode::PUSH, vmcode::RSI);
  //get length in rcx
  code.add(vmcode::MOV, vmcode::RCX, vmcode::R15);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, block_size_mask);
  code.add(vmcode::AND, vmcode::RCX, vmcode::R15);
  //get address again in r15, r11 still contains the other address
  code.add(vmcode::MOV, vmcode::R15, vmcode::RAX);
  //init to true
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);

  auto repeat2 = label_to_string(label++);
  auto fail2 = label_to_string(label++);
  auto cleanup2 = label_to_string(label++);
  code.add(vmcode::LABEL, repeat2);
  code.add(vmcode::CMP, vmcode::RCX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, cleanup2);
  //get next items
  code.add(vmcode::ADD, vmcode::R15, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, vmcode::R11, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::RSI, vmcode::MEM_R15);
  code.add(vmcode::CMP, vmcode::RSI, vmcode::MEM_R11);
  code.add(vmcode::JNE, fail2);
  code.add(vmcode::DEC, vmcode::RCX);
  code.add(vmcode::JMP, repeat2);
  code.add(vmcode::LABEL, fail2);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::LABEL, cleanup2);
  code.add(vmcode::POP, vmcode::RSI);
  code.add(vmcode::POP, vmcode::RCX);
  code.add(vmcode::RET);
  }

void compile_member_cmp_eq(vmcode& code, const compiler_options&)
  {
  // this method gets its arguments in rax and r15, and sets the Z flag
  code.add(vmcode::LABEL, "L_compile_member_cmp_eq");
  code.add(vmcode::CMP, vmcode::RAX, vmcode::R15);
  code.add(vmcode::RET);
  }

void compile_member_cmp_equal(vmcode& code, const compiler_options&)
  {
  // this method gets its arguments in rax and r15, and sets the Z flag
  code.add(vmcode::LABEL, "L_compile_member_cmp_equal");
  code.add(vmcode::PUSH, vmcode::R11);
  code.add(vmcode::PUSH, vmcode::R15);
  code.add(vmcode::PUSH, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R11, vmcode::R15);
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_recursively_equal");
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, bool_t);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::R11);
  code.add(vmcode::POP, vmcode::RAX);
  code.add(vmcode::POP, vmcode::R15);
  code.add(vmcode::POP, vmcode::R11);
  code.add(vmcode::RET);
  }

void compile_member_cmp_eqv(vmcode& code, const compiler_options&)
  {
  // this method gets its arguments in rax and r15, and sets the Z flag
  code.add(vmcode::LABEL, "L_compile_member_cmp_eqv");
  code.add(vmcode::PUSH, vmcode::R11);
  code.add(vmcode::PUSH, vmcode::R15);
  code.add(vmcode::PUSH, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R11, vmcode::R15);
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_structurally_equal");
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, bool_t);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::R11);
  code.add(vmcode::POP, vmcode::RAX);
  code.add(vmcode::POP, vmcode::R15);
  code.add(vmcode::POP, vmcode::R11);
  code.add(vmcode::RET);
  }


void compile_assoc_cmp_eq(vmcode& code, const compiler_options&)
  {
  // this method gets its arguments in rax and r15, and sets the Z flag
  code.add(vmcode::LABEL, "L_compile_assoc_cmp_eq");
  code.add(vmcode::CMP, vmcode::RAX, vmcode::MEM_R15, CELLS(1));
  code.add(vmcode::RET);
  }

void compile_assoc_cmp_equal(vmcode& code, const compiler_options&)
  {
  // this method gets its arguments in rax and r15, and sets the Z flag
  code.add(vmcode::LABEL, "L_compile_assoc_cmp_equal");
  code.add(vmcode::PUSH, vmcode::R11);
  code.add(vmcode::PUSH, vmcode::R15);
  code.add(vmcode::PUSH, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R11, vmcode::MEM_R15, CELLS(1));
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_recursively_equal");
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, bool_t);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::R11);
  code.add(vmcode::POP, vmcode::RAX);
  code.add(vmcode::POP, vmcode::R15);
  code.add(vmcode::POP, vmcode::R11);
  code.add(vmcode::RET);
  }

void compile_assoc_cmp_eqv(vmcode& code, const compiler_options&)
  {
  // this method gets its arguments in rax and r15, and sets the Z flag
  code.add(vmcode::LABEL, "L_compile_assoc_cmp_eqv");
  code.add(vmcode::PUSH, vmcode::R11);
  code.add(vmcode::PUSH, vmcode::R15);
  code.add(vmcode::PUSH, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R11, vmcode::MEM_R15, CELLS(1));
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_structurally_equal");
  code.add(vmcode::CALL, vmcode::R15);
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, bool_t);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::R11);
  code.add(vmcode::POP, vmcode::RAX);
  code.add(vmcode::POP, vmcode::R15);
  code.add(vmcode::POP, vmcode::R11);
  code.add(vmcode::RET);
  }

namespace
  {
  void _compile_member(vmcode& code, const compiler_options& ops, const std::string& label_call, runtime_error re)
    {
    std::string error;
    if (ops.safe_primitives)
      {
      error = label_to_string(label++);
      code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
      code.add(vmcode::JNE, error);
      }
    std::string fail = label_to_string(label++);
    std::string success = label_to_string(label++);
    std::string repeat = label_to_string(label++);
    code.add(vmcode::LABEL, repeat);
    code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, nil);
    code.add(vmcode::JE, fail);

    if (ops.safe_primitives)
      {
      jump_if_arg_is_not_block(code, vmcode::RDX, vmcode::R11, error);
      }
    code.add(vmcode::AND, vmcode::RDX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
    if (ops.safe_primitives)
      {
      jump_if_arg_does_not_point_to_pair(code, vmcode::RDX, vmcode::R11, error);
      }
    code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
    code.add(vmcode::MOV, vmcode::R11, vmcode::LABELADDRESS, label_call);

    // get car
    code.add(vmcode::MOV, vmcode::R15, vmcode::MEM_RDX, CELLS(1));
    code.add(vmcode::CALL, vmcode::R11);
    code.add(vmcode::JE, success);
    code.add(vmcode::MOV, vmcode::RDX, vmcode::MEM_RDX, CELLS(2));
    code.add(vmcode::JMP, repeat);

    code.add(vmcode::LABEL, fail);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
    code.add(vmcode::JMP, CONTINUE);

    code.add(vmcode::LABEL, success);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::RDX);
    code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, block_tag); // add tag again
    code.add(vmcode::JMP, CONTINUE);

    if (ops.safe_primitives)
      {
      error_label(code, error, re);
      }
    }

  void _compile_assoc(vmcode& code, const compiler_options& ops, const std::string& label_call, runtime_error re)
    {
    std::string error;
    if (ops.safe_primitives)
      {
      error = label_to_string(label++);
      code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
      code.add(vmcode::JNE, error);
      }
    std::string fail = label_to_string(label++);
    std::string success = label_to_string(label++);
    std::string repeat = label_to_string(label++);
    std::string skip_nil = label_to_string(label++);
    code.add(vmcode::LABEL, repeat);
    code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, nil);
    code.add(vmcode::JE, fail);

    if (ops.safe_primitives)
      {
      jump_if_arg_is_not_block(code, vmcode::RDX, vmcode::R11, error);
      }
    code.add(vmcode::AND, vmcode::RDX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
    if (ops.safe_primitives)
      {
      jump_if_arg_does_not_point_to_pair(code, vmcode::RDX, vmcode::R11, error);
      }
    code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
    // get car
    code.add(vmcode::MOV, vmcode::R15, vmcode::MEM_RDX, CELLS(1));
    // test for nil
    code.add(vmcode::CMP, vmcode::R15, vmcode::NUMBER, nil);
    code.add(vmcode::JE, skip_nil);

    if (ops.safe_primitives)
      {
      jump_if_arg_is_not_block(code, vmcode::R15, vmcode::R11, error);
      }
    code.add(vmcode::AND, vmcode::R15, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
    if (ops.safe_primitives)
      {
      jump_if_arg_does_not_point_to_pair(code, vmcode::R15, vmcode::R11, error);
      }

    code.add(vmcode::MOV, vmcode::R11, vmcode::LABELADDRESS, label_call);
    code.add(vmcode::CALL, vmcode::R11);
    code.add(vmcode::JE, success);
    code.add(vmcode::LABEL, skip_nil);
    code.add(vmcode::MOV, vmcode::RDX, vmcode::MEM_RDX, CELLS(2));
    code.add(vmcode::JMP, repeat);

    code.add(vmcode::LABEL, fail);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
    code.add(vmcode::JMP, CONTINUE);

    code.add(vmcode::LABEL, success);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
    code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, block_tag); // add tag again
    code.add(vmcode::JMP, CONTINUE);

    if (ops.safe_primitives)
      {
      error_label(code, error, re);
      }
    }
  }

void compile_error(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, (uint64_t)re_silent);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, error_tag);
  code.add(vmcode::JMP, ERROR);
  }

void compile_memv(vmcode& code, const compiler_options& ops)
  {
  _compile_member(code, ops, "L_compile_member_cmp_eqv", re_memv_contract_violation);
  }

void compile_memq(vmcode& code, const compiler_options& ops)
  {
  _compile_member(code, ops, "L_compile_member_cmp_eq", re_memq_contract_violation);
  }

void compile_member(vmcode& code, const compiler_options& ops)
  {
  _compile_member(code, ops, "L_compile_member_cmp_equal", re_member_contract_violation);
  }

void compile_assv(vmcode& code, const compiler_options& ops)
  {
  _compile_assoc(code, ops, "L_compile_assoc_cmp_eqv", re_assv_contract_violation);
  }

void compile_assq(vmcode& code, const compiler_options& ops)
  {
  _compile_assoc(code, ops, "L_compile_assoc_cmp_eq", re_assq_contract_violation);
  }

void compile_assoc(vmcode& code, const compiler_options& ops)
  {
  _compile_assoc(code, ops, "L_compile_assoc_cmp_equal", re_assoc_contract_violation);
  }

void compile_apply_fake_cps_identity(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::LABEL_ALIGNED, "L_identity");
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RDX);
  code.add(vmcode::JMP, "L_return_from_identity");
  }

void compile_apply(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string proc = label_to_string(label++);
  std::string clos = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JL, error);
    }

  //we move all registers in GC_SAVE, then all args are linear in memory (GC_SAVE is followed by LOCALS)
  code.add(vmcode::MOV, vmcode::RAX, GC_SAVE);
  code.add(vmcode::MOV, vmcode::MEM_RAX, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(1), vmcode::RDX);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(2), vmcode::RSI);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(3), vmcode::RDI);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(4), vmcode::R8);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(5), vmcode::R9);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(6), vmcode::R12);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(7), vmcode::R14);

  code.add(vmcode::MOV, vmcode::RDX, vmcode::R11); // save r11 in rdx

  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1)); // mem_rax now points to rdx = 2nd arg
  code.add(vmcode::SUB, vmcode::R11, vmcode::NUMBER, 2);

  //now we look for the last argument, which should be a list
  std::string repeat = label_to_string(label++);
  std::string done = label_to_string(label++);
  code.add(vmcode::LABEL, repeat);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 0);
  code.add(vmcode::JE, done); // last arg found, is in mem_rax
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::JMP, repeat);
  code.add(vmcode::LABEL, done);

  std::string unravel = label_to_string(label++);
  std::string unraveled = label_to_string(label++);

  code.add(vmcode::MOV, vmcode::R11, vmcode::RDX); // get length again
  code.add(vmcode::SUB, vmcode::R11, vmcode::NUMBER, 2);
  // mem_rax now contains a list, let's check
  code.add(vmcode::MOV, vmcode::R15, vmcode::MEM_RAX);
  code.add(vmcode::CMP, vmcode::R15, vmcode::NUMBER, nil); // last element is nil, so that's fine
  code.add(vmcode::JE, unraveled);
  if (ops.safe_primitives)
    jump_if_arg_is_not_block(code, vmcode::R15, vmcode::RDX, error);
  code.add(vmcode::AND, vmcode::R15, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    jump_if_arg_does_not_point_to_pair(code, vmcode::R15, vmcode::RDX, error);

  code.add(vmcode::LABEL, unravel);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::MEM_R15, CELLS(1)); // get car of pair in rdx
  code.add(vmcode::MOV, vmcode::MEM_RAX, vmcode::RDX);
  code.add(vmcode::INC, vmcode::R11);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::MEM_R15, CELLS(2)); // get cdr of pair in rdx
  code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, nil); // last element is nil, so all elements are unpacked
  code.add(vmcode::JE, unraveled);
  if (ops.safe_primitives)
    jump_if_arg_is_not_block(code, vmcode::RDX, vmcode::R15, error);
  code.add(vmcode::AND, vmcode::RDX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    jump_if_arg_does_not_point_to_pair(code, vmcode::RDX, vmcode::R15, error);
  code.add(vmcode::MOV, vmcode::R15, vmcode::RDX);
  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::JMP, unravel);
  code.add(vmcode::LABEL, unraveled);


  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, procedure_mask);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::NUMBER, procedure_tag);
  code.add(vmcode::JE, proc);
  if (ops.safe_primitives)
    {
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R15, error);
    code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
    jump_if_arg_does_not_point_to_closure(code, vmcode::RCX, vmcode::R15, error);
    code.add(vmcode::OR, vmcode::RCX, vmcode::NUMBER, block_tag); // add block tag again
    }
  // if we are here, then the apply is on a closure in rcx

  //first we make a fake cps object
  // todo: move this to the context. We don't need to create one each apply. We could reserve memory for it in the context
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, (uint64_t)closure_tag << (uint64_t)block_shift);
  code.add(vmcode::OR, vmcode::RAX, vmcode::R15);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::R15, vmcode::LABELADDRESS, "L_identity");
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::R15);
  code.add(vmcode::MOV, vmcode::R15, ALLOC);
  code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));

  //rcx is ok, it contains the closure
  code.add(vmcode::MOV, vmcode::RDX, vmcode::R15); // the cps object in rdx

  auto all_args_set_clos = label_to_string(label++);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 0);
  code.add(vmcode::JE, all_args_set_clos);
  code.add(vmcode::MOV, vmcode::RAX, GC_SAVE);
  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::RSI, vmcode::MEM_RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
  code.add(vmcode::JE, all_args_set_clos);

  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::RDI, vmcode::MEM_RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
  code.add(vmcode::JE, all_args_set_clos);

  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::R8, vmcode::MEM_RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 3);
  code.add(vmcode::JE, all_args_set_clos);

  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::R9, vmcode::MEM_RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 4);
  code.add(vmcode::JE, all_args_set_clos);

  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::R12, vmcode::MEM_RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 5);
  code.add(vmcode::JE, all_args_set_clos);

  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::R14, vmcode::MEM_RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 6);
  code.add(vmcode::JE, all_args_set_clos);

  code.add(vmcode::PUSH, vmcode::R11);
  code.add(vmcode::SUB, vmcode::R11, vmcode::NUMBER, 6);
  code.add(vmcode::MOV, vmcode::R15, LOCAL);
  /*
  We have to go from the end to the front, as we'll otherwise overwrite data we still need, as GC_SAVE and LOCAL are next to each other in memory
  */
  code.add(vmcode::SHL, vmcode::R11, vmcode::NUMBER, 3);
  code.add(vmcode::ADD, vmcode::R15, vmcode::R11);
  code.add(vmcode::ADD, vmcode::RAX, vmcode::R11);
  code.add(vmcode::SHR, vmcode::R11, vmcode::NUMBER, 3);
  code.add(vmcode::PUSH, vmcode::RCX);

  auto rep_arg_set_clos = label_to_string(label++);
  auto rep_arg_set_clos_done = label_to_string(label++);
  code.add(vmcode::LABEL, rep_arg_set_clos);
  code.add(vmcode::SUB, vmcode::R15, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::RCX, vmcode::MEM_RAX);
  code.add(vmcode::MOV, vmcode::MEM_R15, vmcode::RCX);
  code.add(vmcode::SUB, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 0);
  code.add(vmcode::JE, rep_arg_set_clos_done);
  code.add(vmcode::JMP, rep_arg_set_clos);
  code.add(vmcode::LABEL, rep_arg_set_clos_done);
  code.add(vmcode::POP, vmcode::RCX);
  code.add(vmcode::POP, vmcode::R11);
  code.add(vmcode::LABEL, all_args_set_clos);
  code.add(vmcode::ADD, vmcode::R11, vmcode::NUMBER, 2);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::MOV, vmcode::R15, vmcode::MEM_RAX, CELLS(1));

  code.add(vmcode::PUSH, CONTINUE);
  code.add(vmcode::JMP, vmcode::R15); // call closure
  code.add(vmcode::LABEL, "L_return_from_identity");

  code.add(vmcode::POP, CONTINUE);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, proc);
  // if we are here, then the apply is on a procedure in rcx

  auto all_args_set = label_to_string(label++);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 0);
  code.add(vmcode::JE, all_args_set);
  code.add(vmcode::MOV, vmcode::RAX, GC_SAVE);
  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::RCX, vmcode::MEM_RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
  code.add(vmcode::JE, all_args_set);
  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::RDX, vmcode::MEM_RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
  code.add(vmcode::JE, all_args_set);

  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::RSI, vmcode::MEM_RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 3);
  code.add(vmcode::JE, all_args_set);

  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::RDI, vmcode::MEM_RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 4);
  code.add(vmcode::JE, all_args_set);

  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::R8, vmcode::MEM_RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 5);
  code.add(vmcode::JE, all_args_set);

  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::R9, vmcode::MEM_RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 6);
  code.add(vmcode::JE, all_args_set);

  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::R12, vmcode::MEM_RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 7);
  code.add(vmcode::JE, all_args_set);

  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::R14, vmcode::MEM_RAX);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 8);
  code.add(vmcode::JE, all_args_set);

  code.add(vmcode::PUSH, vmcode::R11);
  code.add(vmcode::SUB, vmcode::R11, vmcode::NUMBER, 8);
  code.add(vmcode::MOV, vmcode::R15, LOCAL);
  code.add(vmcode::PUSH, vmcode::RCX);
  /*
  We have to go from the front to the end (as opposed to the case for closures above), as we'll otherwise overwrite data we still need, as GC_SAVE and LOCAL are next to each other in memory
  */
  auto rep_arg_set = label_to_string(label++);
  auto rep_arg_set_done = label_to_string(label++);
  code.add(vmcode::LABEL, rep_arg_set);
  code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::RCX, vmcode::MEM_RAX);
  code.add(vmcode::MOV, vmcode::MEM_R15, vmcode::RCX);
  code.add(vmcode::ADD, vmcode::R15, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 0);
  code.add(vmcode::JE, rep_arg_set_done);
  code.add(vmcode::JMP, rep_arg_set);
  code.add(vmcode::LABEL, rep_arg_set_done);
  code.add(vmcode::POP, vmcode::RCX);
  code.add(vmcode::POP, vmcode::R11);
  code.add(vmcode::LABEL, all_args_set);
  code.add(vmcode::MOV, vmcode::RAX, GC_SAVE);
  code.add(vmcode::MOV, vmcode::R15, vmcode::MEM_RAX);
  code.add(vmcode::AND, vmcode::R15, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8); // get procedure address
  code.add(vmcode::JMP, vmcode::R15); // call primitive

  if (ops.safe_primitives)
    {
    error_label(code, error, re_apply_contract_violation);
    }
  }

void compile_quotient(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string division_by_zero;
  std::string l1 = label_to_string(label++);
  std::string l2 = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    division_by_zero = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JL, error);
    }
  jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, l1);
  //rcx is a block
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::CVTTSD2SI, vmcode::RCX, vmcode::XMM0);
  code.add(vmcode::SHL, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::LABEL, l1);

  jump_if_arg_is_not_block(code, vmcode::RDX, vmcode::R11, l2);
  //rdx is a block
  code.add(vmcode::AND, vmcode::RDX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RDX, vmcode::R11, error);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RDX, CELLS(1));
  code.add(vmcode::CVTTSD2SI, vmcode::RDX, vmcode::XMM0);
  code.add(vmcode::SHL, vmcode::RDX, vmcode::NUMBER, 1);
  code.add(vmcode::LABEL, l2);

  if (ops.safe_primitives)
    {
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::RDX);
    code.add(vmcode::JE, division_by_zero);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::XOR, vmcode::RDX, vmcode::RDX);
  //code.add(vmcode::CQO);
  code.add(vmcode::IDIV, vmcode::RCX);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_quotient_contract_violation);
    error_label(code, division_by_zero, re_division_by_zero);
    }
  }

void compile_remainder(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string division_by_zero;
  std::string l1 = label_to_string(label++);
  std::string l2 = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    division_by_zero = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JL, error);
    }
  jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, l1);
  //rcx is a block
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::CVTTSD2SI, vmcode::RCX, vmcode::XMM0);
  code.add(vmcode::SHL, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::LABEL, l1);

  jump_if_arg_is_not_block(code, vmcode::RDX, vmcode::R11, l2);
  //rdx is a block
  code.add(vmcode::AND, vmcode::RDX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RDX, vmcode::R11, error);
    }
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RDX, CELLS(1));
  code.add(vmcode::CVTTSD2SI, vmcode::RDX, vmcode::XMM0);
  code.add(vmcode::SHL, vmcode::RDX, vmcode::NUMBER, 1);
  code.add(vmcode::LABEL, l2);

  if (ops.safe_primitives)
    {
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::RDX);
    code.add(vmcode::JE, division_by_zero);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::XOR, vmcode::RDX, vmcode::RDX);
  //code.add(vmcode::CQO);
  code.add(vmcode::IDIV, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RDX);
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, ~((uint64_t)1));
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_remainder_contract_violation);
    error_label(code, division_by_zero, re_division_by_zero);
    }
  }

void compile_make_port(vmcode& code, const compiler_options& ops)
  {
  /*
  rcx: boolean: input port?
  rdx: string:  filename
  rsi: fixnum:  filehandle
  rdi: string:  buffer
  r8:  fixnum:  index to buffer
  r9:  fixnum:  size of buffer
  extra storage: for input ports: contains the number of bytes actually read. is initialized equal to size of buffer.
  */
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 6);
    code.add(vmcode::JNE, error);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);
    code.add(vmcode::INC, vmcode::RAX);
    check_heap(code, re_make_port_heap_overflow);
    jump_if_arg_is_not_block(code, vmcode::RDX, vmcode::R11, error);
    code.add(vmcode::TEST, vmcode::RSI, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RDI, vmcode::R11, error);
    code.add(vmcode::TEST, vmcode::R8, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::R9, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::AND, vmcode::RDX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
    code.add(vmcode::AND, vmcode::RDI, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
    jump_if_arg_does_not_point_to_string(code, vmcode::RDX, vmcode::R11, error);
    jump_if_arg_does_not_point_to_string(code, vmcode::RDI, vmcode::R11, error);
    code.add(vmcode::OR, vmcode::RDX, vmcode::NUMBER, block_tag);
    code.add(vmcode::OR, vmcode::RDI, vmcode::NUMBER, block_tag);
    }

  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 7);
  code.add(vmcode::MOV, vmcode::R15, vmcode::NUMBER, (uint64_t)port_tag << (uint64_t)block_shift);
  code.add(vmcode::OR, vmcode::RAX, vmcode::R15);
  code.add(vmcode::MOV, vmcode::R15, ALLOC);
  code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::RCX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(2), vmcode::RDX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(3), vmcode::RSI);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(4), vmcode::RDI);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(5), vmcode::R8);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(6), vmcode::R9);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(7), vmcode::R9);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(8));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_make_port_contract_violation);
    }
  }

void compile_is_port(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  auto lab_false = label_to_string(label++);
  jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::RAX, lab_false);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_port(code, vmcode::RCX, vmcode::R11, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_is_port_contract_violation);
    }
  }

void compile_is_input_port(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  auto lab_false = label_to_string(label++);
  jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::RAX, lab_false);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_port(code, vmcode::RCX, vmcode::R11, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_is_input_port_contract_violation);
    }
  }

void compile_is_output_port(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  auto lab_false = label_to_string(label++);
  jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::RAX, lab_false);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_port(code, vmcode::RCX, vmcode::R11, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::XOR, vmcode::RAX, vmcode::NUMBER, 8); // toggle 4-th bit to switch from #t to #f or vice versa
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, lab_false);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_is_output_port_contract_violation);
    }
  }

void compile_peek_char(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_port(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::MOV, vmcode::R11, vmcode::MEM_RCX, CELLS(5));
  code.add(vmcode::MOV, vmcode::R15, vmcode::MEM_RCX, CELLS(7)); // get the number of bytes read

  // rcx contains port
  // r11 contains index
  // r15 contains number of bytes read

  auto no_buffer_read = label_to_string(label++);
  auto eof_lab = label_to_string(label++);
  code.add(vmcode::CMP, vmcode::R11, vmcode::R15);
  code.add(vmcode::JL, no_buffer_read);
  // start reading buffer
  code.add(vmcode::MOV, vmcode::R11, vmcode::MEM_RCX, CELLS(4)); // pointer to string
  code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);

#ifdef _WIN32
  code.add(vmcode::PUSH, vmcode::RCX);
  code.add(vmcode::PUSH, vmcode::RDX);
  code.add(vmcode::PUSH, vmcode::R8);

  code.add(vmcode::MOV, vmcode::R8, vmcode::MEM_RCX, CELLS(5)); // index
  code.add(vmcode::SAR, vmcode::R8, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::R11);
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::RCX, vmcode::MEM_RCX, CELLS(3)); // filehandle
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);

#else
  code.add(vmcode::PUSH, vmcode::RDI);
  code.add(vmcode::PUSH, vmcode::RSI);
  code.add(vmcode::PUSH, vmcode::RDX);

  code.add(vmcode::MOV, vmcode::RDI, vmcode::MEM_RCX, CELLS(3)); // filehandle
  code.add(vmcode::SAR, vmcode::RDI, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::MEM_RCX, CELLS(5)); // index
  code.add(vmcode::SAR, vmcode::RDX, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RSI, vmcode::R11);
  code.add(vmcode::ADD, vmcode::RSI, vmcode::NUMBER, CELLS(1));
#endif

  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&_skiwi_read);
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);
  
#ifdef _WIN32
  code.add(vmcode::POP, vmcode::R8);
  code.add(vmcode::POP, vmcode::RDX);
  code.add(vmcode::POP, vmcode::RCX);
#else
  code.add(vmcode::POP, vmcode::RDX);
  code.add(vmcode::POP, vmcode::RSI);
  code.add(vmcode::POP, vmcode::RDI);
#endif
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::MEM_RCX, CELLS(7), vmcode::RAX); // store the number of bytes read
  code.add(vmcode::CMP, vmcode::RAX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, eof_lab);
  code.add(vmcode::XOR, vmcode::R11, vmcode::R11);
  code.add(vmcode::MOV, vmcode::MEM_RCX, CELLS(5), vmcode::R11); //update index
  code.add(vmcode::LABEL, no_buffer_read);
  code.add(vmcode::MOV, vmcode::R15, vmcode::MEM_RCX, CELLS(4));
  code.add(vmcode::SAR, vmcode::R11, vmcode::NUMBER, 1);
  code.add(vmcode::AND, vmcode::R15, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8); // get address to string
  code.add(vmcode::ADD, vmcode::R11, vmcode::NUMBER, 8); // increase 8 for header
  code.add(vmcode::ADD, vmcode::R15, vmcode::R11);
  code.add(vmcode::MOV8, vmcode::RAX, vmcode::MEM_R15);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, char_tag);

  //code.add(vmcode::SUB, vmcode::R11, vmcode::NUMBER, 7);
  //code.add(vmcode::SHL, vmcode::R11, vmcode::NUMBER, 1);
  //code.add(vmcode::MOV, vmcode::MEM_RCX, CELLS(5), vmcode::R11); //update index

  code.add(vmcode::JMP, CONTINUE);

  code.add(vmcode::LABEL, eof_lab);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, eof_tag);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_peek_char_contract_violation);
    }
  }

void compile_read_char(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_port(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::MOV, vmcode::R11, vmcode::MEM_RCX, CELLS(5));
  code.add(vmcode::MOV, vmcode::R15, vmcode::MEM_RCX, CELLS(7)); // get the number of bytes read

  // rcx contains port
  // r11 contains index
  // r15 contains number of bytes read

  auto no_buffer_read = label_to_string(label++);
  auto eof_lab = label_to_string(label++);
  code.add(vmcode::CMP, vmcode::R11, vmcode::R15);
  code.add(vmcode::JL, no_buffer_read);
  // start reading buffer
  code.add(vmcode::MOV, vmcode::R11, vmcode::MEM_RCX, CELLS(4)); // pointer to string
  code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);

#ifdef _WIN32
  code.add(vmcode::PUSH, vmcode::RCX);
  code.add(vmcode::PUSH, vmcode::RDX);
  code.add(vmcode::PUSH, vmcode::R8);

  code.add(vmcode::MOV, vmcode::R8, vmcode::MEM_RCX, CELLS(5)); // index
  code.add(vmcode::SAR, vmcode::R8, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::R11);
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::MOV, vmcode::RCX, vmcode::MEM_RCX, CELLS(3)); // filehandle
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);

#else
  code.add(vmcode::PUSH, vmcode::RDI);
  code.add(vmcode::PUSH, vmcode::RSI);
  code.add(vmcode::PUSH, vmcode::RDX);

  code.add(vmcode::MOV, vmcode::RDI, vmcode::MEM_RCX, CELLS(3)); // filehandle
  code.add(vmcode::SAR, vmcode::RDI, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::MEM_RCX, CELLS(5)); // index
  code.add(vmcode::SAR, vmcode::RDX, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RSI, vmcode::R11);
  code.add(vmcode::ADD, vmcode::RSI, vmcode::NUMBER, CELLS(1));
#endif

  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&_skiwi_read);
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);

#ifdef _WIN32
  code.add(vmcode::POP, vmcode::R8);
  code.add(vmcode::POP, vmcode::RDX);
  code.add(vmcode::POP, vmcode::RCX);
#else
  code.add(vmcode::POP, vmcode::RDX);
  code.add(vmcode::POP, vmcode::RSI);
  code.add(vmcode::POP, vmcode::RDI);
#endif
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::MEM_RCX, CELLS(7), vmcode::RAX); // store the number of bytes read
  code.add(vmcode::CMP, vmcode::RAX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, eof_lab);
  code.add(vmcode::XOR, vmcode::R11, vmcode::R11);
  code.add(vmcode::LABEL, no_buffer_read);
  code.add(vmcode::MOV, vmcode::R15, vmcode::MEM_RCX, CELLS(4));
  code.add(vmcode::SAR, vmcode::R11, vmcode::NUMBER, 1);
  code.add(vmcode::AND, vmcode::R15, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8); // get address to string
  code.add(vmcode::ADD, vmcode::R11, vmcode::NUMBER, 8); // increase 8 for header
  code.add(vmcode::ADD, vmcode::R15, vmcode::R11);
  code.add(vmcode::MOV8, vmcode::RAX, vmcode::MEM_R15);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 8);
  code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, char_tag);

  code.add(vmcode::SUB, vmcode::R11, vmcode::NUMBER, 7);
  code.add(vmcode::SHL, vmcode::R11, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::MEM_RCX, CELLS(5), vmcode::R11); //update index

  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, eof_lab);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, eof_tag);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_read_char_contract_violation);
    }
  }

void compile_write_string(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    jump_if_arg_is_not_block(code, vmcode::RDX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::AND, vmcode::RDX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error);
    jump_if_arg_does_not_point_to_port(code, vmcode::RDX, vmcode::R11, error);
    }

  code.add(vmcode::PUSH, vmcode::RCX);

  string_length(code, vmcode::RCX);

  // r15 now contains the string length
  code.add(vmcode::POP, vmcode::RCX);
  code.add(vmcode::PUSH, vmcode::R15); // save string length
  code.add(vmcode::MOV, vmcode::R11, vmcode::R15);
  code.add(vmcode::SHL, vmcode::R11, vmcode::NUMBER, 1);
  code.add(vmcode::ADD, vmcode::R11, vmcode::MEM_RDX, CELLS(5)); // add the index to r11

  code.add(vmcode::MOV, vmcode::R15, vmcode::MEM_RDX, CELLS(6)); // get the size of the port buffer

  // rdx contains port
  // rcx contains input string
  // rax contains string
  // r11 contains index
  // r15 contains size

  auto noflush = label_to_string(label++);
  code.add(vmcode::CMP, vmcode::R11, vmcode::R15);
  /*
  two options:
  r11 is smaller than r15: this means the string fits in the buffer
  r11 is equal or larger than r15: the string does not fit in the buffer => we flush the buffer and also flush the string
  */
  code.add(vmcode::JL, noflush);
  // start flush
  code.add(vmcode::MOV, vmcode::R11, vmcode::MEM_RDX, CELLS(4)); // pointer to string
  code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);

#ifdef _WIN32
  code.add(vmcode::PUSH, vmcode::RCX);
  code.add(vmcode::PUSH, vmcode::RDX);
  code.add(vmcode::PUSH, vmcode::R8);

  code.add(vmcode::MOV, vmcode::RCX, vmcode::MEM_RDX, CELLS(3)); // filehandle
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::R8, vmcode::MEM_RDX, CELLS(5)); // index
  code.add(vmcode::SAR, vmcode::R8, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::R11);
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
#else
  code.add(vmcode::PUSH, vmcode::RDI);
  code.add(vmcode::PUSH, vmcode::RSI);
  code.add(vmcode::PUSH, vmcode::RDX);

  code.add(vmcode::MOV, vmcode::RDI, vmcode::MEM_RDX, CELLS(3)); // filehandle
  code.add(vmcode::SAR, vmcode::RDI, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::MEM_RDX, CELLS(5)); // index
  code.add(vmcode::SAR, vmcode::RDX, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RSI, vmcode::R11);
  code.add(vmcode::ADD, vmcode::RSI, vmcode::NUMBER, CELLS(1));
#endif

  
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&_skiwi_write);
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);

#ifdef _WIN32
  code.add(vmcode::POP, vmcode::R8);
  code.add(vmcode::POP, vmcode::RDX);
  code.add(vmcode::POP, vmcode::RCX);
#else
  code.add(vmcode::POP, vmcode::RDX);
  code.add(vmcode::POP, vmcode::RSI);
  code.add(vmcode::POP, vmcode::RDI);
#endif

  code.add(vmcode::XOR, vmcode::R11, vmcode::R11);
  code.add(vmcode::MOV, vmcode::MEM_RDX, CELLS(5), vmcode::R11);

  // now flush the input string
  code.add(vmcode::POP, vmcode::R15); // get the string length
#ifdef _WIN32
  code.add(vmcode::PUSH, vmcode::RCX);
  code.add(vmcode::PUSH, vmcode::RDX);
  code.add(vmcode::PUSH, vmcode::R8);
  code.add(vmcode::MOV, vmcode::R8, vmcode::R15); // string length

  code.add(vmcode::MOV, vmcode::R15, vmcode::RCX); // pointer to string
  code.add(vmcode::MOV, vmcode::RCX, vmcode::MEM_RDX, CELLS(3)); // filehandle
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::R15);
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));



#else
  code.add(vmcode::PUSH, vmcode::RDI);
  code.add(vmcode::PUSH, vmcode::RSI);
  code.add(vmcode::PUSH, vmcode::RDX);

  code.add(vmcode::MOV, vmcode::RDI, vmcode::MEM_RDX, CELLS(3)); // filehandle
  code.add(vmcode::SAR, vmcode::RDI, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::R15); // string length
  code.add(vmcode::MOV, vmcode::RSI, vmcode::RCX);
  code.add(vmcode::ADD, vmcode::RSI, vmcode::NUMBER, CELLS(1));
#endif

  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&_skiwi_write);
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);

#ifdef _WIN32
  code.add(vmcode::POP, vmcode::R8);
  code.add(vmcode::POP, vmcode::RDX);
  code.add(vmcode::POP, vmcode::RCX);
#else
  code.add(vmcode::POP, vmcode::RDX);
  code.add(vmcode::POP, vmcode::RSI);
  code.add(vmcode::POP, vmcode::RDI);
#endif

  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, skiwi_quiet_undefined);
  code.add(vmcode::JMP, CONTINUE); // done

  code.add(vmcode::LABEL, noflush);

  code.add(vmcode::MOV, vmcode::R15, vmcode::MEM_RDX, CELLS(5)); //get old index
  code.add(vmcode::SAR, vmcode::R15, vmcode::NUMBER, 1);
  // r11 contains the final index, we can already save it
  code.add(vmcode::MOV, vmcode::MEM_RDX, CELLS(5), vmcode::R11); //update index

  code.add(vmcode::POP, vmcode::R11); //string-length

  // so right now: r11 == string length and r15 == index in buffer  
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RDX, CELLS(4));
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8); // get address to string
  code.add(vmcode::ADD, vmcode::R15, vmcode::NUMBER, 8); // increase 8 for header
  code.add(vmcode::ADD, vmcode::RAX, vmcode::R15); // rax now points to position where we can add to the string
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, 8); // increase 8 for header

  auto repeat = label_to_string(label++);
  auto done = label_to_string(label++);

  code.add(vmcode::LABEL, repeat);
  code.add(vmcode::TEST, vmcode::R11, vmcode::R11);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::MEM_RCX);
  code.add(vmcode::MOV8, vmcode::MEM_RAX, vmcode::RDX);
  code.add(vmcode::INC, vmcode::RCX);
  code.add(vmcode::INC, vmcode::RAX);
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::JMP, repeat);
  code.add(vmcode::LABEL, done);

  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, skiwi_quiet_undefined);

  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_write_string_contract_violation);
    }
  }

void compile_write_char(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::CMP8, vmcode::RCX, vmcode::NUMBER, char_tag);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RDX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RDX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_port(code, vmcode::RDX, vmcode::R11, error);
    }
  code.add(vmcode::MOV, vmcode::R11, vmcode::MEM_RDX, CELLS(5));
  code.add(vmcode::MOV, vmcode::R15, vmcode::MEM_RDX, CELLS(6));

  // rdx contains port
  // rcx contains character
  // rax contains string
  // r11 contains index
  // r15 contains size

  auto noflush = label_to_string(label++);
  code.add(vmcode::CMP, vmcode::R11, vmcode::R15);
  code.add(vmcode::JL, noflush);
  // start flush
  code.add(vmcode::MOV, vmcode::R11, vmcode::MEM_RDX, CELLS(4)); // pointer to string
  code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);

#ifdef _WIN32
  code.add(vmcode::PUSH, vmcode::RCX);
  code.add(vmcode::PUSH, vmcode::RDX);
  code.add(vmcode::PUSH, vmcode::R8);

  code.add(vmcode::MOV, vmcode::RCX, vmcode::MEM_RDX, CELLS(3)); // filehandle
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::R8, vmcode::MEM_RDX, CELLS(5)); // index
  code.add(vmcode::SAR, vmcode::R8, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::R11);
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
#else
  code.add(vmcode::PUSH, vmcode::RDI);
  code.add(vmcode::PUSH, vmcode::RSI);
  code.add(vmcode::PUSH, vmcode::RDX);

  code.add(vmcode::MOV, vmcode::RDI, vmcode::MEM_RDX, CELLS(3)); // filehandle
  code.add(vmcode::SAR, vmcode::RDI, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::MEM_RDX, CELLS(5)); // index
  code.add(vmcode::SAR, vmcode::RDX, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RSI, vmcode::R11);
  code.add(vmcode::ADD, vmcode::RSI, vmcode::NUMBER, CELLS(1));
#endif

  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&_skiwi_write);
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);

#ifdef _WIN32
  code.add(vmcode::POP, vmcode::R8);
  code.add(vmcode::POP, vmcode::RDX);
  code.add(vmcode::POP, vmcode::RCX);
#else
  code.add(vmcode::POP, vmcode::RDX);
  code.add(vmcode::POP, vmcode::RSI);
  code.add(vmcode::POP, vmcode::RDI);
#endif

  code.add(vmcode::XOR, vmcode::R11, vmcode::R11);
  code.add(vmcode::LABEL, noflush);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RDX, CELLS(4));
  code.add(vmcode::SAR, vmcode::R11, vmcode::NUMBER, 1);
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8); // get address to string
  code.add(vmcode::ADD, vmcode::R11, vmcode::NUMBER, 8); // increase 8 for header
  code.add(vmcode::ADD, vmcode::RAX, vmcode::R11);
  code.add(vmcode::SHR, vmcode::RCX, vmcode::NUMBER, 8); // get char
  code.add(vmcode::MOV8, vmcode::MEM_RAX, vmcode::RCX); // write char

  code.add(vmcode::SUB, vmcode::R11, vmcode::NUMBER, 7);
  code.add(vmcode::SHL, vmcode::R11, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::MEM_RDX, CELLS(5), vmcode::R11); //update index

  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, skiwi_quiet_undefined);

  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_write_char_contract_violation);
    }
  }

void compile_flush_output_port(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_port(code, vmcode::RCX, vmcode::R11, error);
    }
  /*
  Windows:
  rcx: filehandle
  rdx: buffer
  r8: buffersize

  Linux:
  rdi: filehandle
  rsi: buffer
  rdx: buffersize
  */
  auto skip = label_to_string(label++);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::R11, vmcode::MEM_RAX, CELLS(3)); // check filehandle valid?
  code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 0);
  code.add(vmcode::JL, skip);
  code.add(vmcode::MOV, vmcode::R11, vmcode::MEM_RAX, CELLS(4)); // pointer to string
  code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
#ifdef _WIN32
  code.add(vmcode::PUSH, vmcode::RDX);
  code.add(vmcode::PUSH, vmcode::R8);

  code.add(vmcode::MOV, vmcode::RCX, vmcode::MEM_RAX, CELLS(3)); // filehandle
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::R8, vmcode::MEM_RAX, CELLS(5)); // index
  code.add(vmcode::SAR, vmcode::R8, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(5), vmcode::NUMBER, 0); // set index to zero
  code.add(vmcode::MOV, vmcode::RDX, vmcode::R11);
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
#else
  code.add(vmcode::PUSH, vmcode::RDI);
  code.add(vmcode::PUSH, vmcode::RSI);
  code.add(vmcode::PUSH, vmcode::RDX);

  code.add(vmcode::MOV, vmcode::RDI, vmcode::MEM_RAX, CELLS(3)); // filehandle
  code.add(vmcode::SAR, vmcode::RDI, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::MEM_RAX, CELLS(5)); // index
  code.add(vmcode::SAR, vmcode::RDX, vmcode::NUMBER, 1);
  code.add(vmcode::MOV, vmcode::MEM_RAX, CELLS(5), vmcode::NUMBER, 0);  // set index to zero
  code.add(vmcode::MOV, vmcode::RSI, vmcode::R11);
  code.add(vmcode::ADD, vmcode::RSI, vmcode::NUMBER, CELLS(1));
#endif

  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&_skiwi_write);
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);

#ifdef _WIN32
  code.add(vmcode::POP, vmcode::R8);
  code.add(vmcode::POP, vmcode::RDX);
#else
  code.add(vmcode::POP, vmcode::RDX);
  code.add(vmcode::POP, vmcode::RSI);
  code.add(vmcode::POP, vmcode::RDI);
#endif

  code.add(vmcode::LABEL, skip);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, skiwi_quiet_undefined);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_flush_output_port_contract_violation);
    }
  }

void compile_close_file(vmcode& code, const compiler_options& ops)
  {
  // rcx : filehandle  
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }

  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1); // rcx now contains the filehandle

  /*
  Windows:
  rcx
  rdx
  r8

  Linux:
  rdi
  rsi
  rdx
  */
#ifndef WIN32
  code.add(vmcode::PUSH, vmcode::RDI);
  code.add(vmcode::MOV, vmcode::RDI, vmcode::RCX);
#endif

  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&_skiwi_close);
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);

#ifndef WIN32
  code.add(vmcode::POP, vmcode::RDI);
#endif
  code.add(vmcode::SAL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_close_file_contract_violation);
    }
  }

void compile_open_file(vmcode& code, const compiler_options& ops)
  {
  // rcx : filename
  // rdx : boolean: true = input file, false = output file
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error);
    }

  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1)); // rcx now points to string representing filename

#ifdef _WIN32
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, _O_CREAT | O_WRONLY | O_TRUNC | O_BINARY);
#else
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, O_CREAT | O_WRONLY | O_TRUNC);
#endif
  code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, bool_t);
  auto is_output = label_to_string(label++);
  code.add(vmcode::JNE, is_output);
#ifdef _WIN32
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, O_RDONLY | O_BINARY);
#else
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, O_RDONLY);
#endif
  code.add(vmcode::LABEL, is_output);
  /*
  Windows:
  rcx
  rdx
  r8

  Linux:
  rdi
  rsi
  rdx
  */
#ifdef _WIN32
  code.add(vmcode::PUSH, vmcode::R8);

  code.add(vmcode::MOV, vmcode::R8, vmcode::NUMBER, _S_IREAD | _S_IWRITE);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::R11);

#elif defined(unix)
  code.add(vmcode::PUSH, vmcode::RDI);
  code.add(vmcode::PUSH, vmcode::RSI);

  code.add(vmcode::MOV, vmcode::RDX, vmcode::NUMBER, __S_IREAD | __S_IWRITE);
  code.add(vmcode::MOV, vmcode::RDI, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::RSI, vmcode::R11);
#elif defined(__APPLE__)
  code.add(vmcode::PUSH, vmcode::RDI);
  code.add(vmcode::PUSH, vmcode::RSI);

  code.add(vmcode::MOV, vmcode::RDX, vmcode::NUMBER, S_IREAD | S_IWRITE);
  code.add(vmcode::MOV, vmcode::RDI, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::RSI, vmcode::R11);
#endif

#ifdef _WIN32
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&_open);
#else
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&open);
#endif  
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);

#ifdef _WIN32
  code.add(vmcode::POP, vmcode::R8);
#else
  code.add(vmcode::POP, vmcode::RSI);
  code.add(vmcode::POP, vmcode::RDI);
#endif
  code.add(vmcode::SAL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_open_file_contract_violation);
    }
  }

void compile_str2num(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error);
    }

  copy_string_to_buffer(code);

  std::string empty_string = label_to_string(label++);
  std::string rax_is_integer = label_to_string(label++);
  // first check whether the string has zero length
  code.add(vmcode::MOV, vmcode::RCX, BUFFER);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::RAX);
  code.add(vmcode::JE, empty_string);
  code.add(vmcode::SAR, vmcode::RDX, vmcode::NUMBER, 1);
  /*
  Windows:
  rcx
  rdx
  r8

  Linux:
  rdi
  rsi
  rdx
  */
#ifdef _WIN32
  code.add(vmcode::PUSH, vmcode::R8);

  code.add(vmcode::PUSH, vmcode::RAX); // endptr for strtoll

  code.add(vmcode::MOV, vmcode::R8, vmcode::RDX);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::RSP);
  code.add(vmcode::MOV, vmcode::RCX, BUFFER);

#else
  code.add(vmcode::PUSH, vmcode::RDI);
  code.add(vmcode::PUSH, vmcode::RSI);
  code.add(vmcode::PUSH, vmcode::RAX); // endptr for strtoll

  code.add(vmcode::MOV, vmcode::RSI, vmcode::RSP);
  code.add(vmcode::MOV, vmcode::RDI, BUFFER);
#endif

  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&strtoll);
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);
  // check endptr
  code.add(vmcode::POP, vmcode::R11);

#ifdef _WIN32
  code.add(vmcode::POP, vmcode::R8);
#else
  code.add(vmcode::POP, vmcode::RSI);
  code.add(vmcode::POP, vmcode::RDI);
#endif

  code.add(vmcode::MOV, vmcode::RCX, vmcode::MEM_R11);
  code.add(vmcode::TEST8, vmcode::RCX, vmcode::RCX);
  code.add(vmcode::JE, rax_is_integer);

  // now try as a float
#ifdef _WIN32
  code.add(vmcode::PUSH, vmcode::RAX); // endptr for strtoll

  code.add(vmcode::MOV, vmcode::RDX, vmcode::RSP);
  code.add(vmcode::MOV, vmcode::RCX, BUFFER);

#else
  code.add(vmcode::PUSH, vmcode::RDI);
  code.add(vmcode::PUSH, vmcode::RSI);
  code.add(vmcode::PUSH, vmcode::RAX); // endptr for strtoll

  code.add(vmcode::MOV, vmcode::RSI, vmcode::RSP);
  code.add(vmcode::MOV, vmcode::RDI, BUFFER);
#endif

  
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&strtod);
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);

  // check endptr
  code.add(vmcode::POP, vmcode::R11);

#ifndef WIN32
  code.add(vmcode::POP, vmcode::RSI);
  code.add(vmcode::POP, vmcode::RDI);
#endif

  code.add(vmcode::MOV, vmcode::RCX, vmcode::MEM_R11);
  code.add(vmcode::TEST8, vmcode::RCX, vmcode::RCX);
  code.add(vmcode::JNE, empty_string);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(vmcode::MOV, vmcode::R11, ALLOC);
  code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::XMM0);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::RAX);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);

  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, rax_is_integer);
  code.add(vmcode::SAL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JMP, CONTINUE);

  code.add(vmcode::LABEL, empty_string);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_str2num_contract_violation);
    }
  }

void compile_num2str(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string flonum = label_to_string(label++);
  std::string cont = label_to_string(label++);
  std::string oct = label_to_string(label++);
  std::string hex = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::JNE, flonum);
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, 16);
  code.add(vmcode::JE, oct);
  code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, 32);
  code.add(vmcode::JE, hex);
  code.add(vmcode::MOV, vmcode::R15, DCVT);
  code.add(vmcode::JMP, cont);
  code.add(vmcode::LABEL, oct);
  code.add(vmcode::MOV, vmcode::R15, OCVT);
  code.add(vmcode::JMP, cont);
  code.add(vmcode::LABEL, hex);
  code.add(vmcode::MOV, vmcode::R15, XCVT);
  code.add(vmcode::JMP, cont);
  code.add(vmcode::LABEL, flonum);
  if (ops.safe_primitives)
    {
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::MOV, vmcode::RCX, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::MOV, vmcode::R15, GCVT);

  /*
  Windows:
  rcx
  rdx
  r8

  Linux:
  rdi
  rsi
  rdx
  */
#ifdef _WIN32
  code.add(vmcode::PUSH, vmcode::R8);

  code.add(vmcode::MOV, vmcode::R8, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::XMM2, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::R15);
  code.add(vmcode::MOV, vmcode::RCX, BUFFER);

#else
  code.add(vmcode::PUSH, vmcode::RDI);
  code.add(vmcode::PUSH, vmcode::RSI);

  code.add(vmcode::MOV, vmcode::XMM0, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::RSI, vmcode::R15);
  code.add(vmcode::MOV, vmcode::RDI, BUFFER);
#endif


  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&_skiwi_sprintf_floating);
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);

#ifdef _WIN32
  code.add(vmcode::POP, vmcode::R8);
#else
  code.add(vmcode::POP, vmcode::RSI);
  code.add(vmcode::POP, vmcode::RDI);
#endif

  allocate_buffer_as_string(code);

  code.add(vmcode::JMP, CONTINUE);


  code.add(vmcode::LABEL, cont); // rcx contains number, r15 contains format

  /*
  Windows:
  rcx
  rdx
  r8

  Linux:
  rdi
  rsi
  rdx
  */
#ifdef _WIN32
  code.add(vmcode::PUSH, vmcode::R8);

  code.add(vmcode::MOV, vmcode::R8, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::R15);
  code.add(vmcode::MOV, vmcode::RCX, BUFFER);

#else
  code.add(vmcode::PUSH, vmcode::RDI);
  code.add(vmcode::PUSH, vmcode::RSI);

  code.add(vmcode::MOV, vmcode::RDX, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::RSI, vmcode::R15);
  code.add(vmcode::MOV, vmcode::RDI, BUFFER);
#endif

  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&_skiwi_sprintf);
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);
  

#ifdef _WIN32
  code.add(vmcode::POP, vmcode::R8);
#else
  code.add(vmcode::POP, vmcode::RSI);
  code.add(vmcode::POP, vmcode::RDI);
#endif

  allocate_buffer_as_string(code);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_num2str_contract_violation);
    }
  }

void compile_ieee754_sign(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 63);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_ieee754_sign_contract_violation);
    }
  }

void compile_ieee754_exponent(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);
    }
  //  ($inline "mov rax, [rax + CELLS(1)]; sar rax, 51; and rax, 0xfff; or rax, 1" x))
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::SAR, vmcode::RAX, vmcode::NUMBER, 52); // mantissa is 52
  code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0x7ff); // remove sign bit
  code.add(vmcode::SAL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_ieee754_exponent_contract_violation);
    }
  }

void compile_ieee754_mantissa(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);
    }
  // ($inline "mov rax, [rax + CELLS(1)]; mov r11, 0x000fffffffffffff; and rax, r11; INT2FIX rax" x))
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, 0x000fffffffffffff);
  code.add(vmcode::AND, vmcode::RAX, vmcode::R11);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_ieee754_mantissa_contract_violation);
    }
  }

void compile_ieee754_sin(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string is_fixnum = label_to_string(label++);
  std::string save_flonum = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, is_fixnum);
  if (ops.safe_primitives)
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);

  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::JMP, save_flonum);
  code.add(vmcode::LABEL, is_fixnum);
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RCX);
  code.add(vmcode::LABEL, save_flonum);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(vmcode::MOV, vmcode::R11, ALLOC);
  code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(VM::vmcode::CSIN, VM::vmcode::XMM0, VM::vmcode::XMM0);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_ieee754_sin_contract_violation);
    }
  }

void compile_ieee754_cos(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string is_fixnum = label_to_string(label++);
  std::string save_flonum = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, is_fixnum);
  if (ops.safe_primitives)
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);

  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::JMP, save_flonum);
  code.add(vmcode::LABEL, is_fixnum);
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RCX);
  code.add(vmcode::LABEL, save_flonum);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(vmcode::MOV, vmcode::R11, ALLOC);
  code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(VM::vmcode::CCOS, VM::vmcode::XMM0, VM::vmcode::XMM0);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_ieee754_cos_contract_violation);
    }
  }

void compile_ieee754_tan(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string is_fixnum = label_to_string(label++);
  std::string save_flonum = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, is_fixnum);
  if (ops.safe_primitives)
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);

  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::JMP, save_flonum);
  code.add(vmcode::LABEL, is_fixnum);
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RCX);
  code.add(vmcode::LABEL, save_flonum);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(vmcode::MOV, vmcode::R11, ALLOC);
  code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(VM::vmcode::CTAN, VM::vmcode::XMM0, VM::vmcode::XMM0);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_ieee754_tan_contract_violation);
    }
  }

void compile_ieee754_asin(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string is_fixnum = label_to_string(label++);
  std::string save_flonum = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, is_fixnum);
  if (ops.safe_primitives)
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);

  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::JMP, save_flonum);
  code.add(vmcode::LABEL, is_fixnum);
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RCX);
  code.add(vmcode::LABEL, save_flonum);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(vmcode::MOV, vmcode::R11, ALLOC);
  code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(VM::vmcode::CASIN, VM::vmcode::XMM0, VM::vmcode::XMM0);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_ieee754_asin_contract_violation);
    }
  }

void compile_ieee754_acos(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string is_fixnum = label_to_string(label++);
  std::string save_flonum = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, is_fixnum);
  if (ops.safe_primitives)
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);

  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::JMP, save_flonum);
  code.add(vmcode::LABEL, is_fixnum);
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RCX);
  code.add(vmcode::LABEL, save_flonum);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(vmcode::MOV, vmcode::R11, ALLOC);
  code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(VM::vmcode::CACOS, VM::vmcode::XMM0, VM::vmcode::XMM0);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_ieee754_acos_contract_violation);
    }
  }

void compile_ieee754_atan1(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string is_fixnum = label_to_string(label++);
  std::string save_flonum = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, is_fixnum);
  if (ops.safe_primitives)
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);

  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::JMP, save_flonum);
  code.add(vmcode::LABEL, is_fixnum);
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RCX);
  code.add(vmcode::LABEL, save_flonum);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(vmcode::MOV, vmcode::R11, ALLOC);
  code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(VM::vmcode::CATAN, VM::vmcode::XMM0, VM::vmcode::XMM0);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_ieee754_atan1_contract_violation);
    }
  }

void compile_ieee754_log(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string is_fixnum = label_to_string(label++);
  std::string save_flonum = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, is_fixnum);
  if (ops.safe_primitives)
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);

  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::JMP, save_flonum);
  code.add(vmcode::LABEL, is_fixnum);
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RCX);
  code.add(vmcode::LABEL, save_flonum);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(vmcode::MOV, vmcode::R11, ALLOC);
  code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(VM::vmcode::CLOG, VM::vmcode::XMM0, VM::vmcode::XMM0);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_ieee754_log_contract_violation);
    }
  }

void compile_ieee754_round(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string is_fixnum = label_to_string(label++);
  std::string save_flonum = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, is_fixnum);
  if (ops.safe_primitives)
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);

  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::JMP, save_flonum);
  code.add(vmcode::LABEL, is_fixnum);
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RCX);
  code.add(vmcode::LABEL, save_flonum);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(vmcode::MOV, vmcode::R11, ALLOC);
  code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(VM::vmcode::CROUND, VM::vmcode::XMM0, VM::vmcode::XMM0);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_ieee754_round_contract_violation);
    }
  }

void compile_ieee754_truncate(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string is_fixnum = label_to_string(label++);
  std::string save_fixnum = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, is_fixnum);
  if (ops.safe_primitives)
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);

  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::JMP, save_fixnum);
  code.add(vmcode::LABEL, is_fixnum);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, save_fixnum);
  code.add(vmcode::CVTTSD2SI, vmcode::RAX, vmcode::XMM0);
  code.add(vmcode::SAL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_ieee754_round_contract_violation);
    }
  }

void compile_ieee754_sqrt(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  std::string is_fixnum = label_to_string(label++);
  std::string save_flonum = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, is_fixnum);
  if (ops.safe_primitives)
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);

  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::JMP, save_flonum);
  code.add(vmcode::LABEL, is_fixnum);
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RCX);
  code.add(vmcode::LABEL, save_flonum);
  code.add(vmcode::SQRTPD, vmcode::XMM0, vmcode::XMM0);
  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(vmcode::MOV, vmcode::R11, ALLOC);
  code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::XMM0);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_ieee754_sqrt_contract_violation);
    }
  }

void compile_ieee754_pi(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 0);
    code.add(vmcode::JNE, error);
    }
  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(vmcode::MOV, vmcode::R11, ALLOC);
  code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  double pi = 3.14159265358979323846264338327950288419716939937510;
  uint64_t value = *((uint64_t*)(&pi));
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::NUMBER, value);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_ieee754_pi_contract_violation);
    }
  }

void compile_fixnum_expt(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::SAR, vmcode::RDX, vmcode::NUMBER, 1);
  if (ops.safe_primitives)
    {
    code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, 0);
    code.add(vmcode::JL, error);
    }
  std::string done = label_to_string(label++);
  std::string rep = label_to_string(label++);
  std::string zero = label_to_string(label++);
  code.add(vmcode::CMP, vmcode::RDX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, zero);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::R15, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::RCX, vmcode::RDX);
  code.add(vmcode::DEC, vmcode::RCX);
  code.add(vmcode::JE, done);
  code.add(vmcode::LABEL, rep);
  code.add(vmcode::IMUL, vmcode::R15);
  code.add(vmcode::DEC, vmcode::RCX);
  code.add(vmcode::JE, done);
  code.add(vmcode::JMP, rep);
  code.add(vmcode::LABEL, done);
  code.add(vmcode::SAL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, zero);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 2);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_fixnum_expt_contract_violation);
    }
  }

void compile_flonum_expt(vmcode& code, const compiler_options& ops)
  {
  // only for positive exponents
  std::string error;
  std::string rcx_is_fixnum = label_to_string(label++);
  std::string rcx_stored = label_to_string(label++);
  std::string rdx_is_fixnum = label_to_string(label++);
  std::string rdx_stored = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, rdx_is_fixnum);
  if (ops.safe_primitives)
    jump_if_arg_is_not_block(code, vmcode::RDX, vmcode::R11, error);
  code.add(vmcode::AND, vmcode::RDX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RDX, vmcode::R11, error);

  code.add(vmcode::MOV, vmcode::XMM1, vmcode::MEM_RDX, CELLS(1));
  code.add(vmcode::JMP, rdx_stored);
  code.add(vmcode::LABEL, rdx_is_fixnum);
  code.add(vmcode::SAR, vmcode::RDX, vmcode::NUMBER, 1);
  code.add(vmcode::CVTSI2SD, vmcode::XMM1, vmcode::RDX);
  code.add(vmcode::LABEL, rdx_stored);

  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, rcx_is_fixnum);
  if (ops.safe_primitives)
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);

  code.add(vmcode::MOV, vmcode::XMM0, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::JMP, rcx_stored);
  code.add(vmcode::LABEL, rcx_is_fixnum);
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RCX);
  code.add(vmcode::LABEL, rcx_stored);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(vmcode::MOV, vmcode::R11, ALLOC);
  code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);

  code.add(vmcode::CPOW, vmcode::XMM0, vmcode::XMM1);

  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::XMM0);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_flonum_expt_contract_violation);
    }
  /*
  // only for positive exponents
  std::string error;
  std::string rcx_is_fixnum = label_to_string(label++);
  std::string rcx_stored = label_to_string(label++);
  std::string rdx_is_fixnum = label_to_string(label++);
  std::string rdx_stored = label_to_string(label++);
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::TEST, vmcode::RDX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, rdx_is_fixnum);
  if (ops.safe_primitives)
    jump_if_arg_is_not_block(code, vmcode::RDX, vmcode::R11, error);
  code.add(vmcode::AND, vmcode::RDX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RDX, vmcode::R11, error);

  code.add(vmcode::FLD, vmcode::MEM_RDX, CELLS(1));
  code.add(vmcode::JMP, rdx_stored);
  code.add(vmcode::LABEL, rdx_is_fixnum);
  code.add(vmcode::SAR, vmcode::RDX, vmcode::NUMBER, 1);
  code.add(vmcode::PUSH, vmcode::RDX);
  code.add(vmcode::FILD, vmcode::MEM_RSP);
  code.add(vmcode::POP, vmcode::RDX);
  code.add(vmcode::LABEL, rdx_stored);

  code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::JE, rcx_is_fixnum);
  if (ops.safe_primitives)
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);

  code.add(vmcode::FLD, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::JMP, rcx_stored);
  code.add(vmcode::LABEL, rcx_is_fixnum);
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::PUSH, vmcode::RCX);
  code.add(vmcode::FILD, vmcode::MEM_RSP);
  code.add(vmcode::POP, vmcode::RCX);
  code.add(vmcode::LABEL, rcx_stored);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(vmcode::MOV, vmcode::R11, ALLOC);
  code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);

  code.add(vmcode::FYL2X);
  code.add(vmcode::FLD, vmcode::ST0);
  code.add(vmcode::FISTPQ, MEM_ALLOC, CELLS(1));
  code.add(vmcode::FILD, MEM_ALLOC, CELLS(1));
  code.add(vmcode::FSUBP);
  code.add(vmcode::F2XM1);
  code.add(vmcode::FLD1);
  code.add(vmcode::FADDP);
  code.add(vmcode::FILD, MEM_ALLOC, CELLS(1));
  code.add(vmcode::FXCH);
  code.add(vmcode::FSCALE);
  code.add(vmcode::FSTP, MEM_ALLOC, CELLS(1));
  code.add(vmcode::FSTP, MEM_ALLOC, CELLS(2)); // popping to some unused memory location
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_flonum_expt_contract_violation);
    }
  */
  }

void compile_fixnum_to_flonum(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    code.add(vmcode::TEST, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    if (ops.safe_flonums)
      {
      code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 2);
      check_heap(code, re_fixnum_to_flonum_heap_overflow);
      }
    }
  code.add(vmcode::SAR, vmcode::RCX, vmcode::NUMBER, 1);
  code.add(vmcode::CVTSI2SD, vmcode::XMM0, vmcode::RCX);
  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(vmcode::MOV, vmcode::R11, ALLOC);
  code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::XMM0);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_fixnum_to_flonum_contract_violation);
    }
  }
void compile_flonum_to_fixnum(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_flonum(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX, CELLS(1));
  code.add(vmcode::MOV, vmcode::XMM0, vmcode::RAX);
  code.add(vmcode::CVTTSD2SI, vmcode::RAX, vmcode::XMM0);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_flonum_to_fixnum_contract_violation);
    }
  }

void compile_compare_strings(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 3);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    jump_if_arg_is_not_block(code, vmcode::RDX, vmcode::R11, error);
    code.add(vmcode::TEST, vmcode::RSI, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::AND, vmcode::RDX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error);
    jump_if_arg_does_not_point_to_string(code, vmcode::RDX, vmcode::R11, error);
    }
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::SAR, vmcode::RSI, vmcode::NUMBER, 1);
  auto repeat = label_to_string(label++);
  auto equal = label_to_string(label++);
  auto less = label_to_string(label++);
  auto greater = label_to_string(label++);
  code.add(vmcode::LABEL, repeat);
  code.add(vmcode::TEST, vmcode::RSI, vmcode::RSI);
  code.add(vmcode::JE, equal);
  code.add(vmcode::MOV8, vmcode::RAX, vmcode::MEM_RDX);
  code.add(vmcode::CMP8, vmcode::MEM_RCX, vmcode::RAX);
  code.add(vmcode::JL, less);
  code.add(vmcode::JG, greater);
  code.add(vmcode::INC, vmcode::RCX);
  code.add(vmcode::INC, vmcode::RDX);
  code.add(vmcode::DEC, vmcode::RSI);
  code.add(vmcode::JMP, repeat);
  code.add(vmcode::LABEL, equal);
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RAX);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, less);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 0xfffffffffffffffe);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, greater);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 2);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_compare_strings_contract_violation);
    }
  }


void compile_compare_strings_ci(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 3);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    jump_if_arg_is_not_block(code, vmcode::RDX, vmcode::R11, error);
    code.add(vmcode::TEST, vmcode::RSI, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::AND, vmcode::RDX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error); // assembly can jump_short, but vm cannot.
    jump_if_arg_does_not_point_to_string(code, vmcode::RDX, vmcode::R11, error); // assembly can jump_short, but vm cannot.
    }
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::SAR, vmcode::RSI, vmcode::NUMBER, 1);
  auto repeat = label_to_string(label++);
  auto equal = label_to_string(label++);
  auto less = label_to_string(label++);
  auto greater = label_to_string(label++);
  auto local_equal = label_to_string(label++);
  auto test_bl = label_to_string(label++);
  auto test_again = label_to_string(label++);
  /*
  A = 65
  Z = 90
  a = 97
  z = 122
  */
  code.add(vmcode::PUSH, vmcode::RBX);
  code.add(vmcode::LABEL, repeat);
  code.add(vmcode::TEST, vmcode::RSI, vmcode::RSI);
  code.add(vmcode::JE, equal);
  code.add(vmcode::MOV8, vmcode::RAX, vmcode::MEM_RCX);
  code.add(vmcode::MOV8, vmcode::RBX, vmcode::MEM_RDX);
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::JE, local_equal);
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, 65);
  code.add(vmcode::JL, test_bl);
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, 90);
  code.add(vmcode::JG, test_bl);
  code.add(vmcode::ADD8, vmcode::RAX, vmcode::NUMBER, 32);
  code.add(vmcode::LABEL, test_bl);
  code.add(vmcode::CMP8, vmcode::RBX, vmcode::NUMBER, 65);
  code.add(vmcode::JL, test_again);
  code.add(vmcode::CMP8, vmcode::RBX, vmcode::NUMBER, 90);
  code.add(vmcode::JG, test_again);
  code.add(vmcode::ADD8, vmcode::RBX, vmcode::NUMBER, 32);
  code.add(vmcode::LABEL, test_again);
  code.add(vmcode::CMP8, vmcode::RAX, vmcode::RBX);
  code.add(vmcode::JL, less);
  code.add(vmcode::JG, greater);
  code.add(vmcode::LABEL, local_equal);
  code.add(vmcode::INC, vmcode::RCX);
  code.add(vmcode::INC, vmcode::RDX);
  code.add(vmcode::DEC, vmcode::RSI);
  code.add(vmcode::JMP, repeat);
  code.add(vmcode::LABEL, equal);
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RAX);
  code.add(vmcode::POP, vmcode::RBX);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, less);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 0xfffffffffffffffe);
  code.add(vmcode::POP, vmcode::RBX);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, greater);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 2);
  code.add(vmcode::POP, vmcode::RBX);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_compare_strings_ci_contract_violation);
    }
  }

void compile_make_promise(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    if (ops.safe_promises)
      {
      code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 2);
      check_heap(code, re_make_promise_heap_overflow);
      }
    }
  if (ops.safe_primitives)
    {
    code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
    jump_if_arg_does_not_point_to_closure(code, vmcode::RCX, vmcode::R11, error);
    code.add(vmcode::OR, vmcode::RCX, vmcode::NUMBER, block_tag);
    }

  uint64_t header = make_block_header(1, T_PROMISE);
  code.add(vmcode::MOV, vmcode::R11, ALLOC);
  code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::RCX);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);

  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_make_promise_contract_violation);
    }
  }

void compile_undefined(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, skiwi_undefined);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_skiwi_quiet_undefined(vmcode& code, const compiler_options&)
  {
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, skiwi_quiet_undefined);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_load(vmcode& code, const compiler_options& ops)
  {
  /*
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
*/
  /*
Windows:
rcx
Linux:
rdi
*/
/*
#ifndef WIN32
  code.add(vmcode::PUSH, vmcode::RDI);
  code.add(vmcode::MOV, vmcode::RDI, vmcode::RCX);
#endif

  code.add(vmcode::MOV, ALLOC_SAVED, ALLOC); // this line is here so that our foreign calls can access free heap memory
  save_before_foreign_call(code);
  align_stack(code);
  code.add(vmcode::MOV, vmcode::R15, CONTEXT); // r15 should be saved by the callee but r10 not, so we save the context in r15
#ifdef _WIN32
  code.add(vmcode::SUB, vmcode::RSP, vmcode::NUMBER, 32);
#else
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RAX);
#endif  
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&c_prim_load);
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);
  code.add(vmcode::MOV, CONTEXT, vmcode::R15); // now we restore the context
  restore_stack(code);
  restore_after_foreign_call(code);
  code.add(vmcode::MOV, ALLOC, ALLOC_SAVED); // foreign calls should have updated free heap memory if they used some
#ifndef WIN32
  code.add(vmcode::POP, vmcode::RDI);
#endif
  // now check whether rax contains an error: if so we jump to ERROR
  code.add(vmcode::MOV, vmcode::RCX, vmcode::RAX);
  code.add(vmcode::AND8, vmcode::RCX, vmcode::NUMBER, error_mask);
  code.add(vmcode::CMP8, vmcode::RCX, vmcode::NUMBER, error_tag);
  std::string error_in_load = label_to_string(label++);
  code.add(vmcode::JE, error_in_load);
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_load_contract_violation);
    }
  code.add(vmcode::LABEL, error_in_load);
  code.add(vmcode::JMP, ERROR);
  */
  }


void compile_eval(vmcode& code, const compiler_options& ops)
  {
  /*
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
*/
  /*
Windows:
rcx
Linux:
rdi
*/
/*
#ifndef WIN32
  code.add(vmcode::PUSH, vmcode::RDI);
  code.add(vmcode::MOV, vmcode::RDI, vmcode::RCX);
#endif

  code.add(vmcode::MOV, ALLOC_SAVED, ALLOC); // this line is here so that our foreign calls can access free heap memory
  save_before_foreign_call(code);
  align_stack(code);
  code.add(vmcode::MOV, vmcode::R15, CONTEXT); // r15 should be saved by the callee but r10 not, so we save the context in r15
#ifdef _WIN32
  code.add(vmcode::SUB, vmcode::RSP, vmcode::NUMBER, 32);
#else
  code.add(vmcode::XOR, vmcode::RAX, vmcode::RAX);
#endif  
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&c_prim_eval);
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);
  code.add(vmcode::MOV, CONTEXT, vmcode::R15); // now we restore the context
  restore_stack(code);
  restore_after_foreign_call(code);
  code.add(vmcode::MOV, ALLOC, ALLOC_SAVED); // foreign calls should have updated free heap memory if they used some
#ifndef WIN32
  code.add(vmcode::POP, vmcode::RDI);
#endif
  code.add(vmcode::JMP, CONTINUE);
  if (ops.safe_primitives)
    {
    error_label(code, error, re_eval_contract_violation);
    }
  */
  }

void compile_current_seconds(VM::vmcode& code, const compiler_options& /*options*/)
  {
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&_skiwi_current_seconds);
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_current_milliseconds(VM::vmcode& code, const compiler_options& /*options*/)
  {
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&_skiwi_current_milliseconds);
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);
  code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 1);
  code.add(vmcode::JMP, CONTINUE);
  }

void compile_getenv(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));

  /*
Windows:
rcx
Linux:
rdi
*/
#ifndef WIN32
  code.add(vmcode::PUSH, vmcode::RDI);
  code.add(vmcode::MOV, vmcode::RDI, vmcode::RCX);
#endif

  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&_skiwi_getenv);
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);
#ifndef WIN32
  code.add(vmcode::POP, vmcode::RDI);
#endif
  std::string false_label = label_to_string(label++);
  code.add(vmcode::CMP, vmcode::RAX, vmcode::NUMBER, 0);
  code.add(vmcode::JE, false_label);
  code.add(vmcode::MOV, vmcode::RCX, vmcode::RAX); // rcx points to string now
  code.add(vmcode::PUSH, vmcode::RCX);
  raw_string_length(code, vmcode::RCX); //r15 contains string length
  code.add(vmcode::POP, vmcode::RCX);
  if (ops.safe_primitives)
    {
    code.add(vmcode::MOV, vmcode::R11, vmcode::R15); // save length in r11
    code.add(vmcode::MOV, vmcode::RAX, vmcode::R15); // put length in rax
    code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 3);
    code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, 2);
    check_heap(code, re_getenv_heap_overflow);
    code.add(vmcode::MOV, vmcode::R15, vmcode::R11); // length again in r15
    }

  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  code.add(vmcode::SHR, vmcode::R15, vmcode::NUMBER, 3);
  code.add(vmcode::INC, vmcode::R15);
  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)string_tag << (uint64_t)block_shift);
  code.add(vmcode::OR, vmcode::R15, vmcode::R11);
  code.add(vmcode::MOV, MEM_ALLOC, vmcode::R15);
  code.add(vmcode::MOV, vmcode::R15, ALLOC);
  code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
  code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));

  auto repeat = label_to_string(label++);
  auto done = label_to_string(label++);

  code.add(vmcode::MOV, vmcode::R11, vmcode::RAX);
  code.add(vmcode::LABEL, repeat);
  code.add(vmcode::TEST, vmcode::R11, vmcode::R11);
  code.add(vmcode::JE, done);
  code.add(vmcode::MOV8, vmcode::RAX, vmcode::MEM_RCX);
  code.add(vmcode::MOV8, MEM_ALLOC, vmcode::RAX);
  code.add(vmcode::DEC, vmcode::R11);
  code.add(vmcode::INC, ALLOC);
  code.add(vmcode::INC, vmcode::RCX);
  code.add(vmcode::JMP, repeat);
  code.add(vmcode::LABEL, done);

  code.add(vmcode::MOV8, MEM_ALLOC, vmcode::NUMBER, 0);
  code.add(vmcode::INC, ALLOC);

  auto repeat2 = label_to_string(label++);
  auto done2 = label_to_string(label++);
  code.add(vmcode::LABEL, repeat2);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 7);
  code.add(vmcode::AND, vmcode::RAX, ALLOC);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::RAX);
  code.add(vmcode::JE, done2);
  code.add(vmcode::MOV8, MEM_ALLOC, vmcode::NUMBER, 0);
  code.add(vmcode::INC, ALLOC);
  code.add(vmcode::JMP, repeat2);
  code.add(vmcode::LABEL, done2);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
  code.add(vmcode::JMP, CONTINUE);

  code.add(vmcode::LABEL, false_label);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_getenv_contract_violation);
    }
  }

void compile_putenv(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    jump_if_arg_is_not_block(code, vmcode::RDX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(vmcode::AND, vmcode::RDX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error);
    jump_if_arg_does_not_point_to_string(code, vmcode::RDX, vmcode::R11, error);
    }
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
  code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
  /*
Windows:
rcx:
rdx:

Linux:
rdi:
rsi:
*/

#ifndef WIN32
  code.add(vmcode::PUSH, vmcode::RDI);
  code.add(vmcode::PUSH, vmcode::RSI);
  code.add(vmcode::MOV, vmcode::RDI, vmcode::RCX);
  code.add(vmcode::MOV, vmcode::RSI, vmcode::RDX);
#endif

  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&_skiwi_putenv);
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);
  
#ifndef WIN32
  code.add(vmcode::POP, vmcode::RSI);
  code.add(vmcode::POP, vmcode::RDI);
#endif

  std::string success = label_to_string(label++);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::RAX);
  code.add(vmcode::JE, success);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, success);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_putenv_contract_violation);
    }
  }


void compile_file_exists(vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JNE, error);
    jump_if_arg_is_not_block(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::AND, vmcode::RCX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_string(code, vmcode::RCX, vmcode::R11, error);
    }
  code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
  /*
Windows:
rcx:
Linux:
rdi:
*/

#ifndef WIN32
  code.add(vmcode::PUSH, vmcode::RDI);
  code.add(vmcode::MOV, vmcode::RDI, vmcode::RCX);
#endif

  code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, (uint64_t)&_skiwi_file_exists);
  code.add(vmcode::CALLEXTERNAL, vmcode::R11);
  
#ifndef WIN32
  code.add(vmcode::POP, vmcode::RDI);
#endif

  std::string does_not_exists = label_to_string(label++);
  code.add(vmcode::TEST, vmcode::RAX, vmcode::RAX);
  code.add(vmcode::JE, does_not_exists);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_t);
  code.add(vmcode::JMP, CONTINUE);
  code.add(vmcode::LABEL, does_not_exists);
  code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, bool_f);
  code.add(vmcode::JMP, CONTINUE);

  if (ops.safe_primitives)
    {
    error_label(code, error, re_file_exists_contract_violation);
    }
  }

COMPILER_END
