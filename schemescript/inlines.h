#pragma once

#include "vm/vmcode.h"
#include "namespace.h"

#include "compiler_options.h"

COMPILER_BEGIN

// destroys R15
void save_to_local(VM::vmcode& code, uint64_t pos);

// destroys R15
void load_local(VM::vmcode& code, uint64_t pos);

void save_to_local(VM::vmcode& code, uint64_t pos, VM::vmcode::operand source, VM::vmcode::operand free_reg);
void load_local(VM::vmcode& code, uint64_t pos, VM::vmcode::operand target, VM::vmcode::operand free_reg);

void fix2int(VM::vmcode& code, VM::vmcode::operand reg);
void int2fix(VM::vmcode& code, VM::vmcode::operand reg);

void inline_is_fixnum(VM::vmcode& code, const compiler_options& options);
void inline_is_flonum(VM::vmcode& code, const compiler_options& options);
void inline_is_pair(VM::vmcode& code, const compiler_options& options);
void inline_is_vector(VM::vmcode& code, const compiler_options& options);
void inline_is_string(VM::vmcode& code, const compiler_options& options);
void inline_is_symbol(VM::vmcode& code, const compiler_options& options);
void inline_is_promise(VM::vmcode& code, const compiler_options& options);
void inline_is_closure(VM::vmcode& code, const compiler_options& options);
void inline_is_nil(VM::vmcode& code, const compiler_options& options);
void inline_is_eof_object(VM::vmcode& code, const compiler_options& options);
void inline_is_procedure(VM::vmcode& code, const compiler_options& options);
void inline_is_boolean(VM::vmcode& code, const compiler_options& options);
void inline_is_char(VM::vmcode& code, const compiler_options& options);
void inline_is_port(VM::vmcode& code, const compiler_options& options);
void inline_is_input_port(VM::vmcode& code, const compiler_options& options);
void inline_is_output_port(VM::vmcode& code, const compiler_options& options);

void inline_eq(VM::vmcode& code, const compiler_options& options);
void inline_fx_add(VM::vmcode& code, const compiler_options& options);
void inline_fl_add(VM::vmcode& code, const compiler_options& options);
void inline_fx_sub(VM::vmcode& code, const compiler_options& options);
void inline_fl_sub(VM::vmcode& code, const compiler_options& options);
void inline_fx_mul(VM::vmcode& code, const compiler_options& options);
void inline_fl_mul(VM::vmcode& code, const compiler_options& options);
void inline_fx_div(VM::vmcode& code, const compiler_options& options);
void inline_fl_div(VM::vmcode& code, const compiler_options& options);

void inline_fx_add1(VM::vmcode& code, const compiler_options& options);
void inline_fl_add1(VM::vmcode& code, const compiler_options& options);
void inline_fx_sub1(VM::vmcode& code, const compiler_options& options);
void inline_fl_sub1(VM::vmcode& code, const compiler_options& options);
void inline_fx_is_zero(VM::vmcode& code, const compiler_options& options);
void inline_fl_is_zero(VM::vmcode& code, const compiler_options& options);
void inline_fx_min(VM::vmcode& code, const compiler_options& options);
void inline_fl_min(VM::vmcode& code, const compiler_options& options);
void inline_fx_max(VM::vmcode& code, const compiler_options& options);
void inline_fl_max(VM::vmcode& code, const compiler_options& options);

void inline_fx_less(VM::vmcode& code, const compiler_options& options);
void inline_fl_less(VM::vmcode& code, const compiler_options& options);
void inline_fx_leq(VM::vmcode& code, const compiler_options& options);
void inline_fl_leq(VM::vmcode& code, const compiler_options& options);
void inline_fx_greater(VM::vmcode& code, const compiler_options& options);
void inline_fl_greater(VM::vmcode& code, const compiler_options& options);
void inline_fx_geq(VM::vmcode& code, const compiler_options& options);
void inline_fl_geq(VM::vmcode& code, const compiler_options& options);
void inline_fx_equal(VM::vmcode& code, const compiler_options& options);
void inline_fl_equal(VM::vmcode& code, const compiler_options& options);
void inline_fx_not_equal(VM::vmcode& code, const compiler_options& options);
void inline_fl_not_equal(VM::vmcode& code, const compiler_options& options);

void inline_cons(VM::vmcode& code, const compiler_options& options);
void inline_car(VM::vmcode& code, const compiler_options& options);
void inline_cdr(VM::vmcode& code, const compiler_options& options);
void inline_set_car(VM::vmcode& code, const compiler_options& options);
void inline_set_cdr(VM::vmcode& code, const compiler_options& options);

void inline_not(VM::vmcode& code, const compiler_options& options);
void inline_memq(VM::vmcode& code, const compiler_options& options);
void inline_assq(VM::vmcode& code, const compiler_options& options);

void inline_ieee754_sign(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_exponent(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_mantissa(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_fxsin(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_fxcos(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_fxtan(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_fxasin(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_fxacos(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_fxatan1(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_fxlog(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_fxround(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_fxtruncate(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_fxsqrt(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_flsin(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_flcos(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_fltan(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_flasin(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_flacos(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_flatan1(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_fllog(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_flround(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_fltruncate(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_flsqrt(VM::vmcode& code, const compiler_options& options);
void inline_ieee754_pi(VM::vmcode& code, const compiler_options& options);

void inline_bitwise_and(VM::vmcode& code, const compiler_options& options);
void inline_bitwise_not(VM::vmcode& code, const compiler_options& options);
void inline_bitwise_or(VM::vmcode& code, const compiler_options& options);
void inline_bitwise_xor(VM::vmcode& code, const compiler_options& options);

void inline_char_to_fixnum(VM::vmcode& code, const compiler_options& options);
void inline_fixnum_to_char(VM::vmcode& code, const compiler_options& options);
void inline_vector_length(VM::vmcode& code, const compiler_options& options);
void inline_flonum_to_fixnum(VM::vmcode& code, const compiler_options& options);
void inline_fixnum_to_flonum(VM::vmcode& code, const compiler_options& options);

void inline_undefined(VM::vmcode& code, const compiler_options& options);
void inline_skiwi_quiet_undefined(VM::vmcode& code, const compiler_options& options);

void inline_arithmetic_shift(VM::vmcode& code, const compiler_options& options);
void inline_quotient(VM::vmcode& code, const compiler_options& options);
void inline_remainder(VM::vmcode& code, const compiler_options& options);

COMPILER_END
