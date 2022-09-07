#include "parser_tests.h"
#include "test_assert.h"

#include "schemerlicht/stoken.h"
#include "schemerlicht/scontext.h"
#include "schemerlicht/sstream.h"
#include "schemerlicht/sparser.h"

#include "token_tests.h"

void parse_fixnum_1()
  {
  schemerlicht_context ctxt;
  schemerlicht_vector tokens = script2tokens(&ctxt, "5");
  schemerlicht_program prog = make_program(&ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(&ctxt, &prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_literal, expr->type);
  TEST_EQ_INT(schemerlicht_type_fixnum, expr->expr.lit.type);
  TEST_EQ_INT(5, expr->expr.lit.lit.fx.value);
  TEST_EQ_INT(1, expr->expr.lit.lit.fx.line_nr);
  TEST_EQ_INT(1, expr->expr.lit.lit.fx.column_nr);
  destroy_tokens_vector(&ctxt, &tokens);
  schemerlicht_program_destroy(&ctxt, &prog);
  }

void parse_fixnum_2()
  {
  schemerlicht_context ctxt;
  schemerlicht_vector tokens = script2tokens(&ctxt, "-5555");
  schemerlicht_program prog = make_program(&ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(&ctxt, &prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_literal, expr->type);
  TEST_EQ_INT(schemerlicht_type_fixnum, expr->expr.lit.type);
  TEST_EQ_INT(-5555, expr->expr.lit.lit.fx.value);
  TEST_EQ_INT(1, expr->expr.lit.lit.fx.line_nr);
  TEST_EQ_INT(1, expr->expr.lit.lit.fx.column_nr);
  destroy_tokens_vector(&ctxt, &tokens);
  schemerlicht_program_destroy(&ctxt, &prog);
  }

void run_all_parser_tests()
  {
  parse_fixnum_1();
  parse_fixnum_2();
  }