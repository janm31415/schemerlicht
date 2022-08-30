#pragma once

#include "namespace.h"
#include "vm/vmcode.h"
#include <string>

#include "types.h"

COMPILER_BEGIN

VM::vmcode::operand get_mem_operand(VM::vmcode::operand reg);

VM::vmcode::operand get_byte_mem_operand(VM::vmcode::operand reg);


std::string label_to_string(uint64_t lab);

int64_t int2fixnum(int64_t i);
int64_t fixnum2int(int64_t i);

void break_point(VM::vmcode& code);

void error_label(VM::vmcode& code, const std::string& label_name, runtime_error re);

void jump_if_arg_is_not_block(VM::vmcode& code, VM::vmcode::operand arg, VM::vmcode::operand free_reg, const std::string& label_name);
void jump_if_arg_does_not_point_to_flonum(VM::vmcode& code, VM::vmcode::operand arg, VM::vmcode::operand free_reg, const std::string& label_name);
void jump_if_arg_does_not_point_to_closure(VM::vmcode& code, VM::vmcode::operand arg, VM::vmcode::operand free_reg, const std::string& label_name);
void jump_if_arg_does_not_point_to_vector(VM::vmcode& code, VM::vmcode::operand arg, VM::vmcode::operand free_reg, const std::string& label_name);
void jump_if_arg_does_not_point_to_symbol(VM::vmcode& code, VM::vmcode::operand arg, VM::vmcode::operand free_reg, const std::string& label_name);
void jump_if_arg_does_not_point_to_string(VM::vmcode& code, VM::vmcode::operand arg, VM::vmcode::operand free_reg, const std::string& label_name);
void jump_if_arg_does_not_point_to_pair(VM::vmcode& code, VM::vmcode::operand arg, VM::vmcode::operand free_reg, const std::string& label_name);
void jump_if_arg_does_not_point_to_port(VM::vmcode& code, VM::vmcode::operand arg, VM::vmcode::operand free_reg, const std::string& label_name);
void jump_if_arg_does_not_point_to_promise(VM::vmcode& code, VM::vmcode::operand arg, VM::vmcode::operand free_reg, const std::string& label_name);

/*assumes RAX contains the extra heap size requested*/
void check_heap(VM::vmcode& code, runtime_error re);


void save_before_foreign_call(VM::vmcode& code);
void restore_after_foreign_call(VM::vmcode& code);

void align_stack(VM::vmcode& code);
void restore_stack(VM::vmcode& code);

void copy_string_to_buffer(VM::vmcode& code);
void allocate_buffer_as_string(VM::vmcode& code);

void raw_string_length(VM::vmcode& code, VM::vmcode::operand string_reg);
void string_length(VM::vmcode& code, VM::vmcode::operand string_reg);

void push(VM::vmcode& code, VM::vmcode::operand reg);
void pop(VM::vmcode& code, VM::vmcode::operand reg);

COMPILER_END
