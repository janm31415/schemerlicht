#ifndef SCHEMERLICHT_PRIMITIVES_H
#define SCHEMERLICHT_PRIMITIVES_H

#include "schemerlicht.h"
#include "map.h"

typedef enum
  {
  SCHEMERLICHT_ADD1,
  SCHEMERLICHT_SUB1,
  SCHEMERLICHT_ADD,
  SCHEMERLICHT_SUB,
  SCHEMERLICHT_MUL,
  SCHEMERLICHT_DIV,
  SCHEMERLICHT_EQUAL,
  SCHEMERLICHT_NOT_EQUAL,
  SCHEMERLICHT_LESS,
  SCHEMERLICHT_LEQ,
  SCHEMERLICHT_GREATER,
  SCHEMERLICHT_GEQ,
  SCHEMERLICHT_FXADD1,
  SCHEMERLICHT_FXSUB1,
  SCHEMERLICHT_FXADD,
  SCHEMERLICHT_FXSUB,
  SCHEMERLICHT_FXMUL,
  SCHEMERLICHT_FXDIV,
  SCHEMERLICHT_FXEQUAL,
  SCHEMERLICHT_FXLESS,
  SCHEMERLICHT_FXLEQ,
  SCHEMERLICHT_FXGREATER,
  SCHEMERLICHT_FXGEQ,
  SCHEMERLICHT_FXZERO,
  SCHEMERLICHT_FLADD1,
  SCHEMERLICHT_FLSUB1,
  SCHEMERLICHT_FLADD,
  SCHEMERLICHT_FLSUB,
  SCHEMERLICHT_FLMUL,
  SCHEMERLICHT_FLDIV,
  SCHEMERLICHT_FLEQUAL,
  SCHEMERLICHT_FLLESS,
  SCHEMERLICHT_FLLEQ,
  SCHEMERLICHT_FLGREATER,
  SCHEMERLICHT_FLGEQ,
  SCHEMERLICHT_FLZERO,
  SCHEMERLICHT_CHAREQUAL,
  SCHEMERLICHT_CHARLESS,
  SCHEMERLICHT_CHARLEQ,
  SCHEMERLICHT_CHARGREATER,
  SCHEMERLICHT_CHARGEQ,
  SCHEMERLICHT_CHARCIEQUAL,
  SCHEMERLICHT_CHARCILESS,
  SCHEMERLICHT_CHARCILEQ,
  SCHEMERLICHT_CHARCIGREATER,
  SCHEMERLICHT_CHARCIGEQ,
  SCHEMERLICHT_IS_FIXNUM,
  SCHEMERLICHT_IS_FLONUM,
  SCHEMERLICHT_IS_NULL,
  SCHEMERLICHT_IS_ZERO,
  SCHEMERLICHT_IS_BOOLEAN,
  SCHEMERLICHT_IS_CHAR,
  SCHEMERLICHT_NOT,
  SCHEMERLICHT_FIXNUM_CHAR,
  SCHEMERLICHT_CHAR_FIXNUM,
  SCHEMERLICHT_FIXNUM_FLONUM,
  SCHEMERLICHT_FLONUM_FIXNUM,
  SCHEMERLICHT_BITWISE_AND,
  SCHEMERLICHT_BITWISE_OR,
  SCHEMERLICHT_BITWISE_NOT,
  SCHEMERLICHT_BITWISE_XOR,
  SCHEMERLICHT_ARITHMETIC_SHIFT,
  SCHEMERLICHT_VECTOR,
  SCHEMERLICHT_VECTORREF,
  SCHEMERLICHT_VECTORSET,
  SCHEMERLICHT_IS_VECTOR,
  SCHEMERLICHT_IS_PAIR,
  SCHEMERLICHT_CONS,
  SCHEMERLICHT_CAR,
  SCHEMERLICHT_CDR,
  SCHEMERLICHT_HALT,
  SCHEMERLICHT_CLOSURE,
  SCHEMERLICHT_CLOSUREREF,
  SCHEMERLICHT_IS_CLOSURE,
  SCHEMERLICHT_LIST,
  SCHEMERLICHT_MAKE_VECTOR,
  SCHEMERLICHT_MAKE_STRING,
  SCHEMERLICHT_VECTOR_LENGTH,
  SCHEMERLICHT_STRING_LENGTH,
  SCHEMERLICHT_STRING_REF,
  SCHEMERLICHT_STRING_SET,
  SCHEMERLICHT_STRING_HASH,
  SCHEMERLICHT_IS_STRING,
  SCHEMERLICHT_EQ,
  SCHEMERLICHT_EQV,
  SCHEMERLICHT_IS_EQUAL,
  SCHEMERLICHT_STRING,
  SCHEMERLICHT_STRING_TO_SYMBOL,
  SCHEMERLICHT_SYMBOL_TO_STRING,
  SCHEMERLICHT_LENGTH,
  SCHEMERLICHT_SET_CAR,
  SCHEMERLICHT_SET_CDR,
  SCHEMERLICHT_IS_SYMBOL,
  SCHEMERLICHT_IS_PROCEDURE,
  SCHEMERLICHT_RECLAIM,
  SCHEMERLICHT_RECLAIM_GARBAGE,
  SCHEMERLICHT_MEMV,
  SCHEMERLICHT_MEMQ,
  SCHEMERLICHT_MEMBER,
  SCHEMERLICHT_ASSV,
  SCHEMERLICHT_ASSQ,
  SCHEMERLICHT_ASSOC,
  SCHEMERLICHT_STRING_APPEND,
  SCHEMERLICHT_STRING_COPY,
  SCHEMERLICHT_STRING_FILL,
  SCHEMERLICHT_VECTOR_FILL,
  SCHEMERLICHT_CURRENT_SECONDS,
  SCHEMERLICHT_CURRENT_MILLISECONDS,
  SCHEMERLICHT_IS_LIST,
  SCHEMERLICHT_STRING_EQUAL,
  SCHEMERLICHT_STRING_LESS,
  SCHEMERLICHT_STRING_GREATER,
  SCHEMERLICHT_STRING_LEQ,
  SCHEMERLICHT_STRING_GEQ,
  SCHEMERLICHT_STRING_CI_EQUAL,
  SCHEMERLICHT_STRING_CI_LESS,
  SCHEMERLICHT_STRING_CI_GREATER,
  SCHEMERLICHT_STRING_CI_LEQ,
  SCHEMERLICHT_STRING_CI_GEQ,
  SCHEMERLICHT_SUBSTRING,
  SCHEMERLICHT_MAX,
  SCHEMERLICHT_MIN,
  SCHEMERLICHT_APPLY,
  SCHEMERLICHT_APPEND,
  SCHEMERLICHT_IS_EXACT,
  SCHEMERLICHT_IS_INEXACT,
  SCHEMERLICHT_IS_NUMBER,
  SCHEMERLICHT_IS_RATIONAL,
  SCHEMERLICHT_IS_POSITIVE,
  SCHEMERLICHT_IS_NEGATIVE,
  SCHEMERLICHT_IS_ODD,
  SCHEMERLICHT_IS_EVEN,
  SCHEMERLICHT_IS_COMPLEX,
  SCHEMERLICHT_IS_REAL,
  SCHEMERLICHT_IS_INTEGER,
  SCHEMERLICHT_ABS,
  SCHEMERLICHT_QUOTIENT,
  SCHEMERLICHT_REMAINDER,
  SCHEMERLICHT_MODULO,
  SCHEMERLICHT_GCD,
  SCHEMERLICHT_LCM,
  SCHEMERLICHT_FLOOR,
  SCHEMERLICHT_CEILING,
  SCHEMERLICHT_TRUNCATE,
  SCHEMERLICHT_ROUND,
  SCHEMERLICHT_EXP,
  SCHEMERLICHT_EXPT,
  SCHEMERLICHT_LOG,
  SCHEMERLICHT_SIN,
  SCHEMERLICHT_COS,
  SCHEMERLICHT_TAN,
  SCHEMERLICHT_ASIN,
  SCHEMERLICHT_ACOS,
  SCHEMERLICHT_ATAN,
  SCHEMERLICHT_SQRT,
  SCHEMERLICHT_NUMBER_STRING,
  SCHEMERLICHT_STRING_NUMBER,
  SCHEMERLICHT_IS_NAN,
  SCHEMERLICHT_IS_INF,
  SCHEMERLICHT_IS_FINITE,
  SCHEMERLICHT_LIST_REF,
  SCHEMERLICHT_LIST_TAIL,
  SCHEMERLICHT_REVERSE,
  SCHEMERLICHT_CHAR_UPCASE,
  SCHEMERLICHT_CHAR_DOWNCASE,
  SCHEMERLICHT_CHAR_IS_UPPER,
  SCHEMERLICHT_CHAR_IS_LOWER,
  SCHEMERLICHT_CHAR_IS_ALPHA,
  SCHEMERLICHT_CHAR_IS_NUMERIC,
  SCHEMERLICHT_CHAR_IS_WHITESPACE,
  SCHEMERLICHT_STRING_LIST,
  SCHEMERLICHT_LIST_STRING,
  SCHEMERLICHT_VECTOR_LIST,
  SCHEMERLICHT_LIST_VECTOR,
  SCHEMERLICHT_MAKE_PROMISE,
  SCHEMERLICHT_IS_PROMISE,
  SCHEMERLICHT_SLOT_REF,
  SCHEMERLICHT_SLOT_SET,
  SCHEMERLICHT_MAKE_PORT,
  SCHEMERLICHT_IS_PORT,
  SCHEMERLICHT_WRITE_CHAR,
  SCHEMERLICHT_READ_CHAR,
  SCHEMERLICHT_PEEK_CHAR,
  SCHEMERLICHT_IS_INPUT_PORT,
  SCHEMERLICHT_IS_OUTPUT_PORT,
  SCHEMERLICHT_OPEN_INPUT_FILE,
  SCHEMERLICHT_OPEN_OUTPUT_FILE,
  SCHEMERLICHT_CLOSE_INPUT_PORT,
  SCHEMERLICHT_CLOSE_OUTPUT_PORT,
  SCHEMERLICHT_FLUSH_OUTPUT_PORT,
  SCHEMERLICHT_IS_EOF,
  SCHEMERLICHT_IS_CHAR_READY,
  SCHEMERLICHT_WRITE,
  SCHEMERLICHT_DISPLAY,
  SCHEMERLICHT_READ,
  SCHEMERLICHT_LOAD,
  SCHEMERLICHT_EVAL,
  SCHEMERLICHT_GETENV,
  SCHEMERLICHT_PUTENV,
  SCHEMERLICHT_FILE_EXISTS,
  SCHEMERLICHT_INTERACTION_ENVIRONMENT,
  SCHEMERLICHT_IS_ENVIRONMENT,
  SCHEMERLICHT_NULL_ENVIRONMENT,
  SCHEMERLICHT_SCHEME_ENVIRONMENT
  } schemerlicht_primitives;

void schemerlicht_primitive_add1(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_sub1(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_add(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_sub(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_mul(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_div(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_equal(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_not_equal(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_less(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_leq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_greater(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_geq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_fixnum(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_flonum(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_null(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_zero(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_boolean(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_char(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_not(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxadd1(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxsub1(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxadd(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxsub(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxmul(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxdiv(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxequal(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxless(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxleq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxgreater(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxgeq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fxzero(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fladd1(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flsub1(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fladd(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flsub(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flmul(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fldiv(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flequal(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flless(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flleq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flgreater(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flgeq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flzero(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_charequal(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_charless(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_charleq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_chargreater(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_chargeq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_charciequal(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_charciless(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_charcileq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_charcigreater(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_charcigeq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fixnum_char(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_fixnum_flonum(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_char_fixnum(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flonum_fixnum(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_bitwise_and(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_bitwise_or(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_bitwise_not(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_bitwise_xor(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_arithmetic_shift(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_vector(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_vector_ref(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_vector_set(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_vector(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_pair(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_cons(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_cdr(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_car(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_halt(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_closure(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_closure_ref(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_closure(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_list(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_make_vector(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_make_string(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_vector_length(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_length(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_ref(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_set(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_hash(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_string(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_eq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_eqv(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_equal(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_to_symbol(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_symbol_to_string(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_length(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_set_car(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_set_cdr(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_symbol(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_procedure(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_reclaim(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_reclaim_garbage(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_memv(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_memq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_member(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_assv(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_assq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_assoc(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_append(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_copy(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_fill(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_vector_fill(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_current_seconds(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_current_milliseconds(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_list(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_equal(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_less(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_greater(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_leq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_geq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_ci_equal(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_ci_less(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_ci_greater(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_ci_leq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_ci_geq(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_substring(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_max(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_min(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_apply(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_append(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_exact(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_inexact(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_number(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_rational(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_positive(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_negative(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_odd(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_even(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_complex(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_real(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_integer(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_abs(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_quotient(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_remainder(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_modulo(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_gcd(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_lcm(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_floor(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_ceiling(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_truncate(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_round(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_exp(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_expt(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_log(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_sin(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_cos(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_tan(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_asin(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_acos(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_atan(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_sqrt(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_number_string(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_number(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_nan(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_inf(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_finite(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_list_ref(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_list_tail(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_reverse(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_char_upcase(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_char_downcase(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_char_is_upper(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_char_is_lower(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_char_is_alpha(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_char_is_numeric(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_char_is_whitespace(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_string_list(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_list_string(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_vector_list(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_list_vector(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_make_promise(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_promise(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_slot_ref(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_slot_set(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_make_port(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_port(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_write_char(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_read_char(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_peek_char(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_input_port(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_output_port(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_open_input_file(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_open_output_file(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_close_input_port(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_close_output_port(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_flush_output_port(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_eof(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_char_ready(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_write(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_display(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_read(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_load(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_eval(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_putenv(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_getenv(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_file_exists(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_interaction_environment(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_is_environment(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_null_environment(schemerlicht_context* ctxt, int a, int b, int c);
void schemerlicht_primitive_scheme_environment(schemerlicht_context* ctxt, int a, int b, int c);

void schemerlicht_call_primitive_inlined(schemerlicht_context* ctxt, schemerlicht_fixnum prim_id, int a, int b, int c);
void schemerlicht_call_primitive(schemerlicht_context* ctxt, schemerlicht_fixnum prim_id, int a, int b, int c);

schemerlicht_map* generate_primitives_map(schemerlicht_context* ctxt);

#endif //SCHEMERLICHT_PRIMITIVES_H