#pragma once

#include "vm/vmcode.h"
#include "namespace.h"

#include <vector>

#include "compiler_options.h"

COMPILER_BEGIN

const std::vector<VM::vmcode::operand>& get_argument_registers();

void compile_halt(VM::vmcode& code, const compiler_options& options);
void compile_add1(VM::vmcode& code, const compiler_options& options);
void compile_sub1(VM::vmcode& code, const compiler_options& options);

void compile_file_exists(VM::vmcode& code, const compiler_options& options);
void compile_getenv(VM::vmcode& code, const compiler_options& options);
void compile_current_seconds(VM::vmcode& code, const compiler_options& options);
void compile_current_milliseconds(VM::vmcode& code, const compiler_options& options);
void compile_putenv(VM::vmcode& code, const compiler_options& options);
void compile_eval(VM::vmcode& code, const compiler_options& options);
void compile_load(VM::vmcode& code, const compiler_options& options);
void compile_error(VM::vmcode& code, const compiler_options& options);
void compile_is_eq(VM::vmcode& code, const compiler_options& options);
void compile_is_eqv(VM::vmcode& code, const compiler_options& options);
void compile_is_eqv_structurally(VM::vmcode& code, const compiler_options& options);
void compile_is_equal(VM::vmcode& code, const compiler_options& options);
void compile_add(VM::vmcode& code, const compiler_options& options);
void compile_sub(VM::vmcode& code, const compiler_options& options);
void compile_mul(VM::vmcode& code, const compiler_options& options);
void compile_div(VM::vmcode& code, const compiler_options& options);
void compile_max(VM::vmcode& code, const compiler_options& options);
void compile_min(VM::vmcode& code, const compiler_options& options);
void compile_equal(VM::vmcode& code, const compiler_options& options);
void compile_not_equal(VM::vmcode& code, const compiler_options& options);
void compile_less(VM::vmcode& code, const compiler_options& options);
void compile_leq(VM::vmcode& code, const compiler_options& options);
void compile_geq(VM::vmcode& code, const compiler_options& options);
void compile_greater(VM::vmcode& code, const compiler_options& options);
void compile_bitwise_and(VM::vmcode& code, const compiler_options& options);
void compile_bitwise_not(VM::vmcode& code, const compiler_options& options);
void compile_bitwise_or(VM::vmcode& code, const compiler_options& options);
void compile_bitwise_xor(VM::vmcode& code, const compiler_options& options);

void compile_make_promise(VM::vmcode& code, const compiler_options& options);
void compile_compare_strings(VM::vmcode& code, const compiler_options& options);
void compile_compare_strings_ci(VM::vmcode& code, const compiler_options& options);
void compile_fx_equal(VM::vmcode& code, const compiler_options& options);
void compile_fx_less(VM::vmcode& code, const compiler_options& options);
void compile_fx_greater(VM::vmcode& code, const compiler_options& options);
void compile_fx_leq(VM::vmcode& code, const compiler_options& options);
void compile_fx_geq(VM::vmcode& code, const compiler_options& options);
void compile_fx_add(VM::vmcode& code, const compiler_options& options);
void compile_fx_sub(VM::vmcode& code, const compiler_options& options);
void compile_fx_mul(VM::vmcode& code, const compiler_options& options);
void compile_fx_div(VM::vmcode& code, const compiler_options& options);
void compile_fx_add1(VM::vmcode& code, const compiler_options& options);
void compile_fx_sub1(VM::vmcode& code, const compiler_options& options);
void compile_fx_is_zero(VM::vmcode& code, const compiler_options& options);
void compile_char_equal(VM::vmcode& code, const compiler_options& options);
void compile_char_less(VM::vmcode& code, const compiler_options& options);
void compile_char_greater(VM::vmcode& code, const compiler_options& options);
void compile_char_leq(VM::vmcode& code, const compiler_options& options);
void compile_char_geq(VM::vmcode& code, const compiler_options& options);
void compile_char_to_fixnum(VM::vmcode& code, const compiler_options& options);
void compile_fixnum_to_char(VM::vmcode& code, const compiler_options& options);
void compile_closure(VM::vmcode& code, const compiler_options& options);
void compile_closure_ref(VM::vmcode& code, const compiler_options& options);
void compile_make_vector(VM::vmcode& code, const compiler_options& options);
void compile_vector(VM::vmcode& code, const compiler_options& options);
void compile_vector_length(VM::vmcode& code, const compiler_options& options);
void compile_slot_ref(VM::vmcode& code, const compiler_options& options);
void compile_slot_set(VM::vmcode& code, const compiler_options& options);
void compile_vector_ref(VM::vmcode& code, const compiler_options& options);
void compile_vector_set(VM::vmcode& code, const compiler_options& options);
void compile_allocate_symbol(VM::vmcode& code, const compiler_options& options);
void compile_make_string(VM::vmcode& code, const compiler_options& options);
void compile_string(VM::vmcode& code, const compiler_options& options);
void compile_string_length(VM::vmcode& code, const compiler_options& options);
void compile_string_ref(VM::vmcode& code, const compiler_options& options);
void compile_string_set(VM::vmcode& code, const compiler_options& options);
void compile_list(VM::vmcode& code, const compiler_options& options);
void compile_length(VM::vmcode& code, const compiler_options& options);
void compile_set_car(VM::vmcode& code, const compiler_options& options);
void compile_set_cdr(VM::vmcode& code, const compiler_options& options);
void compile_make_port(VM::vmcode& code, const compiler_options& options);
void compile_is_port(VM::vmcode& code, const compiler_options& options);
void compile_is_input_port(VM::vmcode& code, const compiler_options& options);
void compile_is_output_port(VM::vmcode& code, const compiler_options& options);
void compile_read_char(VM::vmcode& code, const compiler_options& options);
void compile_peek_char(VM::vmcode& code, const compiler_options& options);
void compile_write_char(VM::vmcode& code, const compiler_options& options);
void compile_write_string(VM::vmcode& code, const compiler_options& options);
void compile_flush_output_port(VM::vmcode& code, const compiler_options& options);
void compile_open_file(VM::vmcode& code, const compiler_options& options);
void compile_close_file(VM::vmcode& code, const compiler_options& options);
void compile_is_eof_object(VM::vmcode& code, const compiler_options& options);
void compile_num2str(VM::vmcode& code, const compiler_options& options);
void compile_str2num(VM::vmcode& code, const compiler_options& options);
void compile_string_fill(VM::vmcode& code, const compiler_options& options);
void compile_vector_fill(VM::vmcode& code, const compiler_options& options);
void compile_substring(VM::vmcode& code, const compiler_options& options);
void compile_string_append1(VM::vmcode& code, const compiler_options& options);
void compile_string_hash(VM::vmcode& code, const compiler_options& options);

void compile_ieee754_sign(VM::vmcode& code, const compiler_options& options);
void compile_ieee754_exponent(VM::vmcode& code, const compiler_options& options);
void compile_ieee754_mantissa(VM::vmcode& code, const compiler_options& options);
void compile_ieee754_sin(VM::vmcode& code, const compiler_options& options);
void compile_ieee754_cos(VM::vmcode& code, const compiler_options& options);
void compile_ieee754_tan(VM::vmcode& code, const compiler_options& options);
void compile_ieee754_asin(VM::vmcode& code, const compiler_options& options);
void compile_ieee754_acos(VM::vmcode& code, const compiler_options& options);
void compile_ieee754_atan1(VM::vmcode& code, const compiler_options& options);
void compile_ieee754_log(VM::vmcode& code, const compiler_options& options);
void compile_ieee754_round(VM::vmcode& code, const compiler_options& options);
void compile_ieee754_truncate(VM::vmcode& code, const compiler_options& options);
void compile_ieee754_sqrt(VM::vmcode& code, const compiler_options& options);
void compile_ieee754_pi(VM::vmcode& code, const compiler_options& options);
void compile_fixnum_expt(VM::vmcode& code, const compiler_options& options);
void compile_flonum_expt(VM::vmcode& code, const compiler_options& options);

void compile_not(VM::vmcode& code, const compiler_options& options);
void compile_is_fixnum(VM::vmcode& code, const compiler_options& options);
void compile_is_flonum(VM::vmcode& code, const compiler_options& options);
void compile_is_nil(VM::vmcode& code, const compiler_options& options);
void compile_is_vector(VM::vmcode& code, const compiler_options& options);
void compile_is_pair(VM::vmcode& code, const compiler_options& options);
void compile_is_string(VM::vmcode& code, const compiler_options& options);
void compile_is_closure(VM::vmcode& code, const compiler_options& options);
void compile_is_procedure(VM::vmcode& code, const compiler_options&);
void compile_is_symbol(VM::vmcode& code, const compiler_options& options);
void compile_is_promise(VM::vmcode& code, const compiler_options& options);
void compile_is_zero(VM::vmcode& code, const compiler_options& options);
void compile_is_char(VM::vmcode& code, const compiler_options& options);
void compile_is_boolean(VM::vmcode& code, const compiler_options& options);
void compile_member(VM::vmcode& code, const compiler_options& options);
void compile_memv(VM::vmcode& code, const compiler_options& options);
void compile_memq(VM::vmcode& code, const compiler_options& ops);
void compile_assoc(VM::vmcode& code, const compiler_options& options);
void compile_assv(VM::vmcode& code, const compiler_options& options);
void compile_assq(VM::vmcode& code, const compiler_options& ops);
void compile_apply(VM::vmcode& code, const compiler_options& options);
void compile_cons(VM::vmcode& code, const compiler_options& options);
void compile_car(VM::vmcode& code, const compiler_options& options);
void compile_cdr(VM::vmcode& code, const compiler_options& options);
void compile_reclaim_garbage(VM::vmcode& code, const compiler_options& options);
void compile_reclaim(VM::vmcode& code, const compiler_options& options);
void compile_arithmetic_shift(VM::vmcode& code, const compiler_options& options);
void compile_quotient(VM::vmcode& code, const compiler_options& options);
void compile_remainder(VM::vmcode& code, const compiler_options& options);
void compile_fixnum_to_flonum(VM::vmcode& code, const compiler_options& options);
void compile_flonum_to_fixnum(VM::vmcode& code, const compiler_options& options);
void compile_string_copy(VM::vmcode& code, const compiler_options& options);
void compile_symbol_to_string(VM::vmcode& code, const compiler_options& options);
void compile_undefined(VM::vmcode& code, const compiler_options& options);
void compile_skiwi_quiet_undefined(VM::vmcode& code, const compiler_options& options);

void compile_member_cmp_eq(VM::vmcode& code, const compiler_options&);
void compile_member_cmp_eqv(VM::vmcode& code, const compiler_options&);
void compile_member_cmp_equal(VM::vmcode& code, const compiler_options&);
void compile_assoc_cmp_eq(VM::vmcode& code, const compiler_options&);
void compile_assoc_cmp_eqv(VM::vmcode& code, const compiler_options&);
void compile_assoc_cmp_equal(VM::vmcode& code, const compiler_options&);
void compile_structurally_equal(VM::vmcode& code, const compiler_options&, const std::string& label_name);
void compile_recursively_equal(VM::vmcode& code, const compiler_options&, const std::string& label_name);
void compile_mark(VM::vmcode& code, const compiler_options&);
void compile_not_equal_2(VM::vmcode& code, const compiler_options& ops);
void compile_equal_2(VM::vmcode& code, const compiler_options& options);
void compile_less_2(VM::vmcode& code, const compiler_options& options);
void compile_leq_2(VM::vmcode& code, const compiler_options& options);
void compile_geq_2(VM::vmcode& code, const compiler_options& options);
void compile_greater_2(VM::vmcode& code, const compiler_options& options);
void compile_bitwise_and_2(VM::vmcode& code, const compiler_options& options);
void compile_bitwise_or_2(VM::vmcode& code, const compiler_options& options);
void compile_bitwise_xor_2(VM::vmcode& code, const compiler_options& options);
void compile_add_2(VM::vmcode& code, const compiler_options& options);
void compile_subtract_2(VM::vmcode& code, const compiler_options& options);
void compile_multiply_2(VM::vmcode& code, const compiler_options& options);
void compile_divide_2(VM::vmcode& code, const compiler_options& options);
void compile_max_2(VM::vmcode& code, const compiler_options& options);
void compile_min_2(VM::vmcode& code, const compiler_options& options);
void compile_fold_binary(VM::vmcode& code, const compiler_options& options);
void compile_pairwise_compare(VM::vmcode& code, const compiler_options& options);
void compile_apply_fake_cps_identity(VM::vmcode& code, const compiler_options& options);

COMPILER_END
