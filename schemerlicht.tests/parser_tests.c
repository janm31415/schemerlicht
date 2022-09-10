#include "parser_tests.h"
#include "test_assert.h"

#include "schemerlicht/stoken.h"
#include "schemerlicht/scontext.h"
#include "schemerlicht/sstream.h"
#include "schemerlicht/sparser.h"

#include "token_tests.h"

static void parse_fixnum_1()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "5");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(ctxt, &prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_literal, expr->type);
  TEST_EQ_INT(schemerlicht_type_fixnum, expr->expr.lit.type);
  TEST_EQ_INT(5, expr->expr.lit.lit.fx.value);
  TEST_EQ_INT(1, expr->expr.lit.lit.fx.line_nr);
  TEST_EQ_INT(1, expr->expr.lit.lit.fx.column_nr);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void parse_fixnum_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "-5555");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(ctxt, &prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_literal, expr->type);
  TEST_EQ_INT(schemerlicht_type_fixnum, expr->expr.lit.type);
  TEST_EQ_INT(-5555, expr->expr.lit.lit.fx.value);
  TEST_EQ_INT(1, expr->expr.lit.lit.fx.line_nr);
  TEST_EQ_INT(1, expr->expr.lit.lit.fx.column_nr);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void parse_flonum()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "3.14");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(ctxt, &prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_literal, expr->type);
  TEST_EQ_INT(schemerlicht_type_flonum, expr->expr.lit.type);
  TEST_EQ_DOUBLE(3.14, expr->expr.lit.lit.fl.value);
  TEST_EQ_INT(1, expr->expr.lit.lit.fl.line_nr);
  TEST_EQ_INT(1, expr->expr.lit.lit.fl.column_nr);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void parse_nil()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "()");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(ctxt, &prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_literal, expr->type);
  TEST_EQ_INT(schemerlicht_type_nil, expr->expr.lit.type);  
  TEST_EQ_INT(1, expr->expr.lit.lit.nil.line_nr);
  TEST_EQ_INT(1, expr->expr.lit.lit.nil.column_nr);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void parse_true()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "#t");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(ctxt, &prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_literal, expr->type);
  TEST_EQ_INT(schemerlicht_type_true, expr->expr.lit.type);
  TEST_EQ_INT(1, expr->expr.lit.lit.t.line_nr);
  TEST_EQ_INT(1, expr->expr.lit.lit.t.column_nr);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void parse_false()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "#f");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(ctxt, &prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_literal, expr->type);
  TEST_EQ_INT(schemerlicht_type_false, expr->expr.lit.type);
  TEST_EQ_INT(1, expr->expr.lit.lit.f.line_nr);
  TEST_EQ_INT(1, expr->expr.lit.lit.f.column_nr);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void parse_string()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "\"test\"");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(ctxt, &prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_literal, expr->type);
  TEST_EQ_INT(schemerlicht_type_string, expr->expr.lit.type);
  TEST_EQ_INT(1, expr->expr.lit.lit.str.line_nr);
  TEST_EQ_INT(1, expr->expr.lit.lit.str.column_nr);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void parse_character()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "#\\45");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(ctxt, &prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_literal, expr->type);
  TEST_EQ_INT(schemerlicht_type_character, expr->expr.lit.type);
  TEST_EQ_INT(1, expr->expr.lit.lit.ch.line_nr);
  TEST_EQ_INT(1, expr->expr.lit.lit.ch.column_nr);
  TEST_EQ_INT(45, expr->expr.lit.lit.ch.value);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void parse_character_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "#\\space");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(ctxt, &prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_literal, expr->type);
  TEST_EQ_INT(schemerlicht_type_character, expr->expr.lit.type);
  TEST_EQ_INT(1, expr->expr.lit.lit.ch.line_nr);
  TEST_EQ_INT(1, expr->expr.lit.lit.ch.column_nr);
  TEST_EQ_INT(32, expr->expr.lit.lit.ch.value);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void parse_begin()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(begin 1 2 3 4 5)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(ctxt, &prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_begin, expr->type);  
  TEST_EQ_INT(5, expr->expr.beg.arguments.vector_size);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

void run_all_parser_tests()
  {
  parse_fixnum_1();
  parse_fixnum_2();
  parse_flonum(); 
  parse_nil();
  parse_true(); 
  parse_false();
  parse_string();
  parse_character();
  parse_character_2();
  parse_begin();
  }