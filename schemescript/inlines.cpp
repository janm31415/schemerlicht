#include "inlines.h"
#include "context.h"
#include "context_defs.h"
#include "asm_aux.h"
#include "globals.h"

COMPILER_BEGIN

void save_to_local(VM::vmcode& code, uint64_t pos)
  {
  code.add(VM::vmcode::MOV, VM::vmcode::R15, LOCAL);
  code.add(VM::vmcode::MOV, VM::vmcode::MEM_R15, CELLS(pos), VM::vmcode::RAX);
  }

void save_to_local(VM::vmcode& code, uint64_t pos, VM::vmcode::operand source, VM::vmcode::operand free_reg)
  {
  code.add(VM::vmcode::MOV, free_reg, LOCAL);
  code.add(VM::vmcode::MOV, get_mem_operand(free_reg), CELLS(pos), source);
  }

void load_local(VM::vmcode& code, uint64_t pos)
  {
  code.add(VM::vmcode::MOV, VM::vmcode::R15, LOCAL);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::MEM_R15, CELLS(pos));
  }

void load_local(VM::vmcode& code, uint64_t pos, VM::vmcode::operand target, VM::vmcode::operand free_reg)
  {
  code.add(VM::vmcode::MOV, free_reg, LOCAL);
  code.add(VM::vmcode::MOV, target, get_mem_operand(free_reg), CELLS(pos));
  }

void fix2int(VM::vmcode& code, VM::vmcode::operand reg)
  {
  code.add(VM::vmcode::SAR, reg, VM::vmcode::NUMBER, 1);
  }

void int2fix(VM::vmcode& code, VM::vmcode::operand reg)
  {
  code.add(VM::vmcode::SHL, reg, VM::vmcode::NUMBER, 1);
  }

void inline_is_fixnum(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::TEST, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::SETE, VM::vmcode::RAX);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_is_flonum(VM::vmcode& code, const compiler_options&)
  {
  auto lab_false = label_to_string(label++);
  auto done = label_to_string(label++);
  jump_if_arg_is_not_block(code, VM::vmcode::RAX, VM::vmcode::RBX, lab_false);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_flonum(code, VM::vmcode::RAX, VM::vmcode::RAX, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_t);
  code.add(VM::vmcode::JMP, done);
  code.add(VM::vmcode::LABEL, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  code.add(VM::vmcode::LABEL, done);
  }

void inline_is_pair(VM::vmcode& code, const compiler_options&)
  {
  auto lab_false = label_to_string(label++);
  auto done = label_to_string(label++);
  jump_if_arg_is_not_block(code, VM::vmcode::RAX, VM::vmcode::RBX, lab_false);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_pair(code, VM::vmcode::RAX, VM::vmcode::RAX, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_t);
  code.add(VM::vmcode::JMP, done);
  code.add(VM::vmcode::LABEL, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  code.add(VM::vmcode::LABEL, done);
  }

void inline_is_vector(VM::vmcode& code, const compiler_options&)
  {
  auto lab_false = label_to_string(label++);
  auto done = label_to_string(label++);
  jump_if_arg_is_not_block(code, VM::vmcode::RAX, VM::vmcode::RBX, lab_false);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_vector(code, VM::vmcode::RAX, VM::vmcode::RAX, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_t);
  code.add(VM::vmcode::JMP, done);
  code.add(VM::vmcode::LABEL, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  code.add(VM::vmcode::LABEL, done);
  }

void inline_is_port(VM::vmcode& code, const compiler_options&)
  {
  auto lab_false = label_to_string(label++);
  auto done = label_to_string(label++);
  jump_if_arg_is_not_block(code, VM::vmcode::RAX, VM::vmcode::RBX, lab_false);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_port(code, VM::vmcode::RAX, VM::vmcode::RAX, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_t);
  code.add(VM::vmcode::JMP, done);
  code.add(VM::vmcode::LABEL, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  code.add(VM::vmcode::LABEL, done);
  }

void inline_is_input_port(VM::vmcode& code, const compiler_options&)
  {
  auto lab_false = label_to_string(label++);
  auto done = label_to_string(label++);
  jump_if_arg_is_not_block(code, VM::vmcode::RAX, VM::vmcode::RBX, lab_false);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_port(code, VM::vmcode::RAX, VM::vmcode::RBX, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::JMP, done);
  code.add(VM::vmcode::LABEL, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  code.add(VM::vmcode::LABEL, done);
  }

void inline_is_output_port(VM::vmcode& code, const compiler_options&)
  {
  auto lab_false = label_to_string(label++);
  auto done = label_to_string(label++);
  jump_if_arg_is_not_block(code, VM::vmcode::RAX, VM::vmcode::RBX, lab_false);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_port(code, VM::vmcode::RAX, VM::vmcode::RBX, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::XOR, VM::vmcode::RAX, VM::vmcode::NUMBER, 8); // toggle 4-th bit to switch from #t to #f or vice versa
  code.add(VM::vmcode::JMP, done);
  code.add(VM::vmcode::LABEL, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  code.add(VM::vmcode::LABEL, done);
  }

void inline_is_string(VM::vmcode& code, const compiler_options&)
  {
  auto lab_false = label_to_string(label++);
  auto done = label_to_string(label++);
  jump_if_arg_is_not_block(code, VM::vmcode::RAX, VM::vmcode::RBX, lab_false);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_string(code, VM::vmcode::RAX, VM::vmcode::RAX, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_t);
  code.add(VM::vmcode::JMP, done);
  code.add(VM::vmcode::LABEL, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  code.add(VM::vmcode::LABEL, done);
  }

void inline_is_symbol(VM::vmcode& code, const compiler_options&)
  {
  auto lab_false = label_to_string(label++);
  auto done = label_to_string(label++);
  jump_if_arg_is_not_block(code, VM::vmcode::RAX, VM::vmcode::RBX, lab_false);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_symbol(code, VM::vmcode::RAX, VM::vmcode::RAX, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_t);
  code.add(VM::vmcode::JMP, done);
  code.add(VM::vmcode::LABEL, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  code.add(VM::vmcode::LABEL, done);
  }

void inline_is_promise(VM::vmcode& code, const compiler_options&)
  {
  auto lab_false = label_to_string(label++);
  auto done = label_to_string(label++);
  jump_if_arg_is_not_block(code, VM::vmcode::RAX, VM::vmcode::RBX, lab_false);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_promise(code, VM::vmcode::RAX, VM::vmcode::RAX, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_t);
  code.add(VM::vmcode::JMP, done);
  code.add(VM::vmcode::LABEL, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  code.add(VM::vmcode::LABEL, done);
  }

void inline_is_closure(VM::vmcode& code, const compiler_options&)
  {
  auto lab_false = label_to_string(label++);
  auto done = label_to_string(label++);
  jump_if_arg_is_not_block(code, VM::vmcode::RAX, VM::vmcode::RBX, lab_false);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_closure(code, VM::vmcode::RAX, VM::vmcode::RAX, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_t);
  code.add(VM::vmcode::JMP, done);
  code.add(VM::vmcode::LABEL, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  code.add(VM::vmcode::LABEL, done);
  }

void inline_is_nil(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::CMP, VM::vmcode::RAX, VM::vmcode::NUMBER, nil);
  code.add(VM::vmcode::SETE, VM::vmcode::RAX);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_is_eof_object(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::CMP, VM::vmcode::RAX, VM::vmcode::NUMBER, eof_tag);
  code.add(VM::vmcode::SETE, VM::vmcode::RAX);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_is_procedure(VM::vmcode& code, const compiler_options&)
  {
  auto done = label_to_string(label++);
  auto check_closure = label_to_string(label++);
  auto lab_false = label_to_string(label++);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, VM::vmcode::RAX);
  code.add(VM::vmcode::AND, VM::vmcode::RBX, VM::vmcode::NUMBER, procedure_mask);
  code.add(VM::vmcode::CMP, VM::vmcode::RBX, VM::vmcode::NUMBER, procedure_tag);
  code.add(VM::vmcode::JNE, check_closure);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_t);
  code.add(VM::vmcode::JMP, done);
  code.add(VM::vmcode::LABEL, check_closure);
  jump_if_arg_is_not_block(code, VM::vmcode::RAX, VM::vmcode::RBX, lab_false);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  jump_if_arg_does_not_point_to_closure(code, VM::vmcode::RAX, VM::vmcode::RAX, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_t);
  code.add(VM::vmcode::JMP, done);
  code.add(VM::vmcode::LABEL, lab_false);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  code.add(VM::vmcode::LABEL, done);
  }

void inline_is_boolean(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 247);
  code.add(VM::vmcode::CMP, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  code.add(VM::vmcode::SETE, VM::vmcode::RAX);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_is_char(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::CMP8, VM::vmcode::RAX, VM::vmcode::NUMBER, char_tag);
  code.add(VM::vmcode::SETE, VM::vmcode::RAX);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_fx_min(VM::vmcode& code, const compiler_options&)
  {
  auto done = label_to_string(label++);
  code.add(VM::vmcode::CMP, VM::vmcode::RAX, VM::vmcode::RBX);
  code.add(VM::vmcode::JL, done);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  code.add(VM::vmcode::LABEL, done);
  }

void inline_fl_min(VM::vmcode& code, const compiler_options&)
  {
  auto done = label_to_string(label++);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::AND, VM::vmcode::RBX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::MOV, VM::vmcode::XMM1, VM::vmcode::MEM_RBX, CELLS(1));
  code.add(VM::vmcode::UCOMISD, VM::vmcode::XMM0, VM::vmcode::XMM1);
  code.add(VM::vmcode::JB, done);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::XMM1);
  code.add(VM::vmcode::LABEL, done);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::MEM_RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  }

void inline_fx_max(VM::vmcode& code, const compiler_options&)
  {
  auto done = label_to_string(label++);
  code.add(VM::vmcode::CMP, VM::vmcode::RAX, VM::vmcode::RBX);
  code.add(VM::vmcode::JG, done);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  code.add(VM::vmcode::LABEL, done);
  }

void inline_fl_max(VM::vmcode& code, const compiler_options&)
  {
  auto done = label_to_string(label++);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::AND, VM::vmcode::RBX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::MOV, VM::vmcode::XMM1, VM::vmcode::MEM_RBX, CELLS(1));
  code.add(VM::vmcode::UCOMISD, VM::vmcode::XMM0, VM::vmcode::XMM1);
  code.add(VM::vmcode::JA, done);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::XMM1);
  code.add(VM::vmcode::LABEL, done);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::MEM_RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  }

void inline_fx_add1(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::ADD, VM::vmcode::RAX, VM::vmcode::NUMBER, 2);
  }

void inline_fl_add1(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::MEM_RAX, CELLS(1));
  double d = 1.0;
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, VM::vmcode::NUMBER, *(reinterpret_cast<uint64_t*>(&d)));
  code.add(VM::vmcode::MOV, VM::vmcode::XMM1, VM::vmcode::RBX);
  code.add(VM::vmcode::ADDSD, VM::vmcode::XMM0, VM::vmcode::XMM1);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::MEM_RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  }

void inline_fx_sub1(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::SUB, VM::vmcode::RAX, VM::vmcode::NUMBER, 2);
  }

void inline_fl_sub1(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::MEM_RAX, CELLS(1));
  double d = 1.0;
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, VM::vmcode::NUMBER, *(reinterpret_cast<uint64_t*>(&d)));
  code.add(VM::vmcode::MOV, VM::vmcode::XMM1, VM::vmcode::RBX);
  code.add(VM::vmcode::SUBSD, VM::vmcode::XMM0, VM::vmcode::XMM1);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::MEM_RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  }

void inline_fx_is_zero(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::TEST, VM::vmcode::RAX, VM::vmcode::RAX);
  code.add(VM::vmcode::SETE, VM::vmcode::RAX);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_fl_is_zero(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::XOR, VM::vmcode::RAX, VM::vmcode::RAX);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM1, VM::vmcode::RAX);
  code.add(VM::vmcode::CMPEQPD, VM::vmcode::XMM0, VM::vmcode::XMM1);
  code.add(VM::vmcode::MOVMSKPD, VM::vmcode::RAX, VM::vmcode::XMM0);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_fx_add(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::ADD, VM::vmcode::RAX, VM::vmcode::RBX);
  }

void inline_fl_add(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::AND, VM::vmcode::RBX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::MOV, VM::vmcode::XMM1, VM::vmcode::MEM_RBX, CELLS(1));
  code.add(VM::vmcode::ADDSD, VM::vmcode::XMM0, VM::vmcode::XMM1);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::MEM_RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  }

void inline_fx_sub(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::SUB, VM::vmcode::RAX, VM::vmcode::RBX);
  }

void inline_fl_sub(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::AND, VM::vmcode::RBX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::MOV, VM::vmcode::XMM1, VM::vmcode::MEM_RBX, CELLS(1));
  code.add(VM::vmcode::SUBSD, VM::vmcode::XMM0, VM::vmcode::XMM1);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::MEM_RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  }

void inline_fx_mul(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::MOV, VM::vmcode::R15, VM::vmcode::RDX);
  code.add(VM::vmcode::IMUL, VM::vmcode::RBX);
  code.add(VM::vmcode::SAR, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::MOV, VM::vmcode::RDX, VM::vmcode::R15);
  }

void inline_fl_mul(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::AND, VM::vmcode::RBX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::MOV, VM::vmcode::XMM1, VM::vmcode::MEM_RBX, CELLS(1));
  code.add(VM::vmcode::MULSD, VM::vmcode::XMM0, VM::vmcode::XMM1);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::MEM_RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  }

void inline_fx_div(VM::vmcode& code, const compiler_options& ops)
  {
  std::string done, div_by_zero;
  if (ops.safe_primitives)
    {
    div_by_zero = label_to_string(label++);
    done = label_to_string(label++);
    code.add(VM::vmcode::TEST, VM::vmcode::RBX, VM::vmcode::RBX);
    code.add(VM::vmcode::JE, div_by_zero);
    }
  code.add(VM::vmcode::MOV, VM::vmcode::R15, VM::vmcode::RDX);
  code.add(VM::vmcode::XOR, VM::vmcode::RDX, VM::vmcode::RDX);
  //code.add(VM::vmcode::CQO);
  code.add(VM::vmcode::IDIV, VM::vmcode::RBX);
  code.add(VM::vmcode::SAL, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::MOV, VM::vmcode::RDX, VM::vmcode::R15);
  if (ops.safe_primitives)
    {
    code.add(VM::vmcode::JMP, done);
    error_label(code, div_by_zero, re_division_by_zero);
    code.add(VM::vmcode::LABEL, done);
    }
  }

void inline_fl_div(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::AND, VM::vmcode::RBX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::MOV, VM::vmcode::XMM1, VM::vmcode::MEM_RBX, CELLS(1));
  code.add(VM::vmcode::DIVSD, VM::vmcode::XMM0, VM::vmcode::XMM1);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::MEM_RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  }

void inline_eq(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::CMP, VM::vmcode::RAX, VM::vmcode::RBX);
  code.add(VM::vmcode::SETE, VM::vmcode::RAX);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_fx_less(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::CMP, VM::vmcode::RAX, VM::vmcode::RBX);
  code.add(VM::vmcode::SETL, VM::vmcode::RAX);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_fl_less(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::AND, VM::vmcode::RBX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::MOV, VM::vmcode::XMM1, VM::vmcode::MEM_RBX, CELLS(1));
  code.add(VM::vmcode::CMPLTPD, VM::vmcode::XMM0, VM::vmcode::XMM1);
  code.add(VM::vmcode::MOVMSKPD, VM::vmcode::RAX, VM::vmcode::XMM0);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_fx_leq(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::CMP, VM::vmcode::RAX, VM::vmcode::RBX);
  code.add(VM::vmcode::SETLE, VM::vmcode::RAX);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_fl_leq(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::AND, VM::vmcode::RBX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::MOV, VM::vmcode::XMM1, VM::vmcode::MEM_RBX, CELLS(1));
  code.add(VM::vmcode::CMPLEPD, VM::vmcode::XMM0, VM::vmcode::XMM1);
  code.add(VM::vmcode::MOVMSKPD, VM::vmcode::RAX, VM::vmcode::XMM0);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_fx_greater(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::CMP, VM::vmcode::RAX, VM::vmcode::RBX);
  code.add(VM::vmcode::SETG, VM::vmcode::RAX);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_fl_greater(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::AND, VM::vmcode::RBX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::MOV, VM::vmcode::XMM1, VM::vmcode::MEM_RBX, CELLS(1));
  code.add(VM::vmcode::CMPLTPD, VM::vmcode::XMM1, VM::vmcode::XMM0);
  code.add(VM::vmcode::MOVMSKPD, VM::vmcode::RAX, VM::vmcode::XMM1);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_fx_equal(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::CMP, VM::vmcode::RAX, VM::vmcode::RBX);
  code.add(VM::vmcode::SETE, VM::vmcode::RAX);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_fl_equal(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::AND, VM::vmcode::RBX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::MOV, VM::vmcode::XMM1, VM::vmcode::MEM_RBX, CELLS(1));
  code.add(VM::vmcode::CMPEQPD, VM::vmcode::XMM0, VM::vmcode::XMM1);
  code.add(VM::vmcode::MOVMSKPD, VM::vmcode::RAX, VM::vmcode::XMM0);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_fx_not_equal(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::CMP, VM::vmcode::RAX, VM::vmcode::RBX);
  code.add(VM::vmcode::SETE, VM::vmcode::RAX);
  code.add(VM::vmcode::XOR, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_fl_not_equal(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::AND, VM::vmcode::RBX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::MOV, VM::vmcode::XMM1, VM::vmcode::MEM_RBX, CELLS(1));
  code.add(VM::vmcode::CMPEQPD, VM::vmcode::XMM0, VM::vmcode::XMM1);
  code.add(VM::vmcode::MOVMSKPD, VM::vmcode::RAX, VM::vmcode::XMM0);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::XOR, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_fx_geq(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::CMP, VM::vmcode::RAX, VM::vmcode::RBX);
  code.add(VM::vmcode::SETGE, VM::vmcode::RAX);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_fl_geq(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::AND, VM::vmcode::RBX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::MOV, VM::vmcode::XMM1, VM::vmcode::MEM_RBX, CELLS(1));
  code.add(VM::vmcode::CMPLEPD, VM::vmcode::XMM1, VM::vmcode::XMM0);
  code.add(VM::vmcode::MOVMSKPD, VM::vmcode::RAX, VM::vmcode::XMM1);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_cons(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(2), VM::vmcode::RBX);
  uint64_t header = make_block_header(2, T_PAIR);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(3));
  }

void inline_car(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::MEM_RAX, CELLS(1));
  }

void inline_cdr(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::MEM_RAX, CELLS(2));
  }

void inline_set_car(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::MEM_RAX, CELLS(1), VM::vmcode::RBX);
  }

void inline_set_cdr(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::MEM_RAX, CELLS(2), VM::vmcode::RBX);
  }

void inline_not(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::CMP, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  code.add(VM::vmcode::SETE, VM::vmcode::RAX);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 3);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  }

void inline_memq(VM::vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    }
  std::string fail = label_to_string(label++);
  std::string success = label_to_string(label++);
  std::string repeat = label_to_string(label++);
  std::string done = label_to_string(label++);
  code.add(VM::vmcode::LABEL, repeat);
  code.add(VM::vmcode::CMP, VM::vmcode::RBX, VM::vmcode::NUMBER, nil);
  code.add(VM::vmcode::JE, fail);

  if (ops.safe_primitives)
    {
    jump_if_arg_is_not_block(code, VM::vmcode::RBX, VM::vmcode::R15, error);
    }
  code.add(VM::vmcode::AND, VM::vmcode::RBX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_pair(code, VM::vmcode::RBX, VM::vmcode::R15, error);
    }
  // get car
  code.add(VM::vmcode::MOV, VM::vmcode::R15, VM::vmcode::MEM_RBX, CELLS(1));
  code.add(VM::vmcode::CMP, VM::vmcode::RAX, VM::vmcode::R15);
  code.add(VM::vmcode::JE, success);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, VM::vmcode::MEM_RBX, CELLS(2));
  code.add(VM::vmcode::JMP, repeat);

  code.add(VM::vmcode::LABEL, fail);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  code.add(VM::vmcode::JMP, done);

  code.add(VM::vmcode::LABEL, success);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, block_tag); // add tag again  

  if (ops.safe_primitives)
    {
    code.add(VM::vmcode::JMP, done);
    error_label(code, error, re_memq_contract_violation);
    }

  code.add(VM::vmcode::LABEL, done);
  }

void inline_assq(VM::vmcode& code, const compiler_options& ops)
  {
  std::string error;
  if (ops.safe_primitives)
    {
    error = label_to_string(label++);
    }
  std::string fail = label_to_string(label++);
  std::string success = label_to_string(label++);
  std::string repeat = label_to_string(label++);
  std::string skip_nil = label_to_string(label++);
  std::string done = label_to_string(label++);
  code.add(VM::vmcode::LABEL, repeat);
  code.add(VM::vmcode::CMP, VM::vmcode::RBX, VM::vmcode::NUMBER, nil);
  code.add(VM::vmcode::JE, fail);

  if (ops.safe_primitives)
    {
    jump_if_arg_is_not_block(code, VM::vmcode::RBX, VM::vmcode::R15, error);
    }
  code.add(VM::vmcode::AND, VM::vmcode::RBX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_pair(code, VM::vmcode::RBX, VM::vmcode::R15, error);
    }
  // get car
  code.add(VM::vmcode::MOV, VM::vmcode::R15, VM::vmcode::MEM_RBX, CELLS(1));
  // test for nil
  code.add(VM::vmcode::CMP, VM::vmcode::R15, VM::vmcode::NUMBER, nil);
  code.add(VM::vmcode::JE, skip_nil);

  if (ops.safe_primitives)
    {
    code.add(VM::vmcode::AND, VM::vmcode::R15, VM::vmcode::NUMBER, block_mask);
    code.add(VM::vmcode::CMP, VM::vmcode::R15, VM::vmcode::NUMBER, block_tag);
    code.add(VM::vmcode::JNE, error);
    code.add(VM::vmcode::MOV, VM::vmcode::R15, VM::vmcode::MEM_RBX, CELLS(1));
    }
  code.add(VM::vmcode::AND, VM::vmcode::R15, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  if (ops.safe_primitives)
    {
    jump_if_arg_does_not_point_to_pair(code, VM::vmcode::R15, VM::vmcode::R15, error);
    code.add(VM::vmcode::MOV, VM::vmcode::R15, VM::vmcode::MEM_RBX, CELLS(1));
    code.add(VM::vmcode::AND, VM::vmcode::R15, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
    }

  code.add(VM::vmcode::CMP, VM::vmcode::RAX, VM::vmcode::MEM_R15, CELLS(1));
  code.add(VM::vmcode::JE, success);
  code.add(VM::vmcode::LABEL, skip_nil);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, VM::vmcode::MEM_RBX, CELLS(2));
  code.add(VM::vmcode::JMP, repeat);

  code.add(VM::vmcode::LABEL, fail);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, bool_f);
  code.add(VM::vmcode::JMP, done);

  code.add(VM::vmcode::LABEL, success);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::R15);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, block_tag); // add tag again  

  if (ops.safe_primitives)
    {
    code.add(VM::vmcode::JMP, done);
    error_label(code, error, re_assq_contract_violation);
    }

  code.add(VM::vmcode::LABEL, done);

  }

void inline_ieee754_sign(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::SHR, VM::vmcode::RAX, VM::vmcode::NUMBER, 63);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  }

void inline_ieee754_exponent(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::SAR, VM::vmcode::RAX, VM::vmcode::NUMBER, 52); // mantissa is 52
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0x7ff); // remove sign bit
  code.add(VM::vmcode::SAL, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  }

void inline_ieee754_mantissa(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, VM::vmcode::NUMBER, 0x000fffffffffffff);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::RBX);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  }

void inline_ieee754_fxsin(VM::vmcode& code, const compiler_options&)
  {
  assert(0);
  /*
  code.add(VM::vmcode::SAR, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::PUSH, VM::vmcode::RAX);
  code.add(VM::vmcode::FILD, VM::vmcode::MEM_RSP);
  code.add(VM::vmcode::POP, VM::vmcode::RAX);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::FSIN);
  code.add(VM::vmcode::FSTP, MEM_ALLOC, CELLS(1));
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  */
  }

void inline_ieee754_fxcos(VM::vmcode& code, const compiler_options&)
  {
  assert(0);
  /*
  code.add(VM::vmcode::SAR, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::PUSH, VM::vmcode::RAX);
  code.add(VM::vmcode::FILD, VM::vmcode::MEM_RSP);
  code.add(VM::vmcode::POP, VM::vmcode::RAX);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::FCOS);
  code.add(VM::vmcode::FSTP, MEM_ALLOC, CELLS(1));
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  */
  }

void inline_ieee754_fxtan(VM::vmcode& code, const compiler_options&)
  {
  assert(0);
  /*
  code.add(VM::vmcode::SAR, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::PUSH, VM::vmcode::RAX);
  code.add(VM::vmcode::FILD, VM::vmcode::MEM_RSP);
  code.add(VM::vmcode::POP, VM::vmcode::RAX);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::FPTAN);
  code.add(VM::vmcode::FSTP, VM::vmcode::ST0);
  code.add(VM::vmcode::FSTP, MEM_ALLOC, CELLS(1));
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  */
  }

void inline_ieee754_fxasin(VM::vmcode& code, const compiler_options&)
  {
  assert(0);
  /*
  code.add(VM::vmcode::SAR, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::PUSH, VM::vmcode::RAX);
  code.add(VM::vmcode::FILD, VM::vmcode::MEM_RSP);
  code.add(VM::vmcode::POP, VM::vmcode::RAX);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);

  code.add(VM::vmcode::FLD, VM::vmcode::ST0);
  code.add(VM::vmcode::FMUL, VM::vmcode::ST0, VM::vmcode::ST0);
  code.add(VM::vmcode::FLD1);
  code.add(VM::vmcode::FSUBRP);
  code.add(VM::vmcode::FSQRT);
  code.add(VM::vmcode::FPATAN);
  code.add(VM::vmcode::FSTP, MEM_ALLOC, CELLS(1));
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  */
  }

void inline_ieee754_fxacos(VM::vmcode& code, const compiler_options&)
  {
  assert(0);
  /*
  code.add(VM::vmcode::SAR, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::PUSH, VM::vmcode::RAX);
  code.add(VM::vmcode::FILD, VM::vmcode::MEM_RSP);
  code.add(VM::vmcode::POP, VM::vmcode::RAX);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::FLD, VM::vmcode::ST0);
  code.add(VM::vmcode::FMUL, VM::vmcode::ST0, VM::vmcode::ST0);
  code.add(VM::vmcode::FLD1);
  code.add(VM::vmcode::FSUBRP);
  code.add(VM::vmcode::FSQRT);
  code.add(VM::vmcode::FXCH, VM::vmcode::ST1);
  code.add(VM::vmcode::FPATAN);
  code.add(VM::vmcode::FSTP, MEM_ALLOC, CELLS(1));
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  */
  }

void inline_ieee754_fxatan1(VM::vmcode& code, const compiler_options&)
  {
  assert(0);
  /*
  code.add(VM::vmcode::SAR, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::PUSH, VM::vmcode::RAX);
  code.add(VM::vmcode::FILD, VM::vmcode::MEM_RSP);
  code.add(VM::vmcode::POP, VM::vmcode::RAX);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::FLD1);
  code.add(VM::vmcode::FPATAN);
  code.add(VM::vmcode::FSTP, MEM_ALLOC, CELLS(1));
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  */
  }

void inline_ieee754_fxlog(VM::vmcode& code, const compiler_options&)
  {
  assert(0);
  /*
  code.add(VM::vmcode::FLDLN2);

  code.add(VM::vmcode::SAR, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::PUSH, VM::vmcode::RAX);
  code.add(VM::vmcode::FILD, VM::vmcode::MEM_RSP);
  code.add(VM::vmcode::POP, VM::vmcode::RAX);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);

  code.add(VM::vmcode::FYL2X);
  code.add(VM::vmcode::FSTP, MEM_ALLOC, CELLS(1));
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  */
  }

void inline_ieee754_fxround(VM::vmcode& code, const compiler_options&)
  {
  assert(0);
  /*
  code.add(VM::vmcode::SAR, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::PUSH, VM::vmcode::RAX);
  code.add(VM::vmcode::FILD, VM::vmcode::MEM_RSP);
  code.add(VM::vmcode::POP, VM::vmcode::RAX);

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::FRNDINT);
  code.add(VM::vmcode::FSTP, MEM_ALLOC, CELLS(1));
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  */
  }

void inline_ieee754_fxtruncate(VM::vmcode&, const compiler_options&)
  {
  //do nothing
  }

void inline_ieee754_fxsqrt(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::SAR, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::CVTSI2SD, VM::vmcode::XMM0, VM::vmcode::RAX);
  code.add(VM::vmcode::SQRTPD, VM::vmcode::XMM0, VM::vmcode::XMM0);
  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  }

void inline_ieee754_flsin(VM::vmcode& code, const compiler_options&)
  {
  assert(0);
  /*
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::FLD, VM::vmcode::MEM_RAX, CELLS(1));

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::FSIN);
  code.add(VM::vmcode::FSTP, MEM_ALLOC, CELLS(1));
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  */
  }

void inline_ieee754_flcos(VM::vmcode& code, const compiler_options&)
  {
  assert(0);
  /*
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::FLD, VM::vmcode::MEM_RAX, CELLS(1));

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::FCOS);
  code.add(VM::vmcode::FSTP, MEM_ALLOC, CELLS(1));
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  */
  }

void inline_ieee754_fltan(VM::vmcode& code, const compiler_options&)
  {
  assert(0);
  /*
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::FLD, VM::vmcode::MEM_RAX, CELLS(1));

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::FPTAN);
  code.add(VM::vmcode::FSTP, VM::vmcode::ST0);
  code.add(VM::vmcode::FSTP, MEM_ALLOC, CELLS(1));
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  */
  }

void inline_ieee754_flasin(VM::vmcode& code, const compiler_options&)
  {
  assert(0);
  /*
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::FLD, VM::vmcode::MEM_RAX, CELLS(1));

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);

  code.add(VM::vmcode::FLD, VM::vmcode::ST0);
  code.add(VM::vmcode::FMUL, VM::vmcode::ST0, VM::vmcode::ST0);
  code.add(VM::vmcode::FLD1);
  code.add(VM::vmcode::FSUBRP);
  code.add(VM::vmcode::FSQRT);
  code.add(VM::vmcode::FPATAN);
  code.add(VM::vmcode::FSTP, MEM_ALLOC, CELLS(1));
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  */
  }

void inline_ieee754_flacos(VM::vmcode& code, const compiler_options&)
  {
  assert(0);
  /*
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::FLD, VM::vmcode::MEM_RAX, CELLS(1));

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::FLD, VM::vmcode::ST0);
  code.add(VM::vmcode::FMUL, VM::vmcode::ST0, VM::vmcode::ST0);
  code.add(VM::vmcode::FLD1);
  code.add(VM::vmcode::FSUBRP);
  code.add(VM::vmcode::FSQRT);
  code.add(VM::vmcode::FXCH, VM::vmcode::ST1);
  code.add(VM::vmcode::FPATAN);
  code.add(VM::vmcode::FSTP, MEM_ALLOC, CELLS(1));
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  */
  }

void inline_ieee754_flatan1(VM::vmcode& code, const compiler_options&)
  {
  assert(0);
  /*
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::FLD, VM::vmcode::MEM_RAX, CELLS(1));

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::FLD1);
  code.add(VM::vmcode::FPATAN);
  code.add(VM::vmcode::FSTP, MEM_ALLOC, CELLS(1));
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  */
  }

void inline_ieee754_fllog(VM::vmcode& code, const compiler_options&)
  {
  assert(0);
  /*
  code.add(VM::vmcode::FLDLN2);

  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::FLD, VM::vmcode::MEM_RAX, CELLS(1));

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);

  code.add(VM::vmcode::FYL2X);
  code.add(VM::vmcode::FSTP, MEM_ALLOC, CELLS(1));
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  */
  }

void inline_ieee754_flround(VM::vmcode& code, const compiler_options&)
  {
  assert(0);
  /*
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::FLD, VM::vmcode::MEM_RAX, CELLS(1));

  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::FRNDINT);
  code.add(VM::vmcode::FSTP, MEM_ALLOC, CELLS(1));
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  */
  }

void inline_ieee754_fltruncate(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::CVTTSD2SI, VM::vmcode::RAX, VM::vmcode::XMM0);
  code.add(VM::vmcode::SAL, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  }

void inline_ieee754_flsqrt(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::SQRTPD, VM::vmcode::XMM0, VM::vmcode::XMM0);
  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  }

void inline_ieee754_pi(VM::vmcode& code, const compiler_options&)
  {
  assert(0);
  /*
  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::FLDPI);
  code.add(VM::vmcode::FSTP, MEM_ALLOC, CELLS(1));
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  */
  }

void inline_bitwise_and(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::RBX);
  }

void inline_bitwise_not(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::XOR, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFFF);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFFE);
  }

void inline_bitwise_or(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::RBX);
  }

void inline_bitwise_xor(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::XOR, VM::vmcode::RAX, VM::vmcode::RBX);
  }

void inline_char_to_fixnum(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::SHR, VM::vmcode::RAX, VM::vmcode::NUMBER, 7);
  }

void inline_fixnum_to_char(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 7);
  code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, char_tag);
  }

void inline_vector_length(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::MEM_RAX);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, VM::vmcode::NUMBER, block_size_mask);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::RBX);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  }

void inline_flonum_to_fixnum(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::MEM_RAX, CELLS(1));
  code.add(VM::vmcode::MOV, VM::vmcode::XMM0, VM::vmcode::RAX);
  code.add(VM::vmcode::CVTTSD2SI, VM::vmcode::RAX, VM::vmcode::XMM0);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  }

void inline_fixnum_to_flonum(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::SAR, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::CVTSI2SD, VM::vmcode::XMM0, VM::vmcode::RAX);
  uint64_t header = make_block_header(1, T_FLONUM);
  code.add(VM::vmcode::MOV, VM::vmcode::RBX, ALLOC);
  code.add(VM::vmcode::OR, VM::vmcode::RBX, VM::vmcode::NUMBER, block_tag);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, header);
  code.add(VM::vmcode::MOV, MEM_ALLOC, VM::vmcode::RAX);
  code.add(VM::vmcode::MOV, MEM_ALLOC, CELLS(1), VM::vmcode::XMM0);
  code.add(VM::vmcode::ADD, ALLOC, VM::vmcode::NUMBER, CELLS(2));
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RBX);
  }

void inline_undefined(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, skiwi_undefined);
  }

void inline_skiwi_quiet_undefined(VM::vmcode& code, const compiler_options&)
  {
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::NUMBER, skiwi_quiet_undefined);
  }

void inline_arithmetic_shift(VM::vmcode& code, const compiler_options&)
  {
  auto shift_right = label_to_string(label++);
  auto done = label_to_string(label++);
  code.add(VM::vmcode::MOV, VM::vmcode::R15, VM::vmcode::RCX);
  code.add(VM::vmcode::CMP, VM::vmcode::RBX, VM::vmcode::NUMBER, 0);
  code.add(VM::vmcode::JL, shift_right);
  code.add(VM::vmcode::SHR, VM::vmcode::RBX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::MOV, VM::vmcode::RCX, VM::vmcode::RBX);
  code.add(VM::vmcode::SAL, VM::vmcode::RAX, VM::vmcode::RCX);
  code.add(VM::vmcode::JMP, done);
  code.add(VM::vmcode::LABEL, shift_right);
  code.add(VM::vmcode::SAR, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::NEG, VM::vmcode::RBX);
  code.add(VM::vmcode::SHR, VM::vmcode::RBX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::MOV, VM::vmcode::RCX, VM::vmcode::RBX);
  code.add(VM::vmcode::SAR, VM::vmcode::RAX, VM::vmcode::RCX);
  code.add(VM::vmcode::SAL, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::LABEL, done);
  code.add(VM::vmcode::MOV, VM::vmcode::RCX, VM::vmcode::R15);
  }

void inline_quotient(VM::vmcode& code, const compiler_options& ops)
  {
  std::string done, div_by_zero;
  if (ops.safe_primitives)
    {
    div_by_zero = label_to_string(label++);
    done = label_to_string(label++);
    code.add(VM::vmcode::TEST, VM::vmcode::RBX, VM::vmcode::RBX);
    code.add(VM::vmcode::JE, div_by_zero);
    }
  code.add(VM::vmcode::MOV, VM::vmcode::R15, VM::vmcode::RDX);
  code.add(VM::vmcode::XOR, VM::vmcode::RDX, VM::vmcode::RDX);
  //code.add(VM::vmcode::CQO);
  code.add(VM::vmcode::IDIV, VM::vmcode::RBX);
  code.add(VM::vmcode::SHL, VM::vmcode::RAX, VM::vmcode::NUMBER, 1);
  code.add(VM::vmcode::MOV, VM::vmcode::RDX, VM::vmcode::R15);
  if (ops.safe_primitives)
    {
    code.add(VM::vmcode::JMP, done);
    error_label(code, div_by_zero, re_division_by_zero);
    code.add(VM::vmcode::LABEL, done);
    }
  }

void inline_remainder(VM::vmcode& code, const compiler_options& ops)
  {
  std::string done, div_by_zero;
  if (ops.safe_primitives)
    {
    div_by_zero = label_to_string(label++);
    done = label_to_string(label++);
    code.add(VM::vmcode::TEST, VM::vmcode::RBX, VM::vmcode::RBX);
    code.add(VM::vmcode::JE, div_by_zero);
    }
  code.add(VM::vmcode::MOV, VM::vmcode::R15, VM::vmcode::RDX);
  code.add(VM::vmcode::XOR, VM::vmcode::RDX, VM::vmcode::RDX);
  //code.add(VM::vmcode::CQO);
  code.add(VM::vmcode::IDIV, VM::vmcode::RBX);
  code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::RDX);
  code.add(VM::vmcode::AND, VM::vmcode::RAX, VM::vmcode::NUMBER, ~((uint64_t)1));
  code.add(VM::vmcode::MOV, VM::vmcode::RDX, VM::vmcode::R15);
  if (ops.safe_primitives)
    {
    code.add(VM::vmcode::JMP, done);
    error_label(code, div_by_zero, re_division_by_zero);
    code.add(VM::vmcode::LABEL, done);
    }
  }

COMPILER_END
