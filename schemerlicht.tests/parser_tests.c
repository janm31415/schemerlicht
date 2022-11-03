#include "parser_tests.h"
#include "test_assert.h"

#include "schemerlicht/token.h"
#include "schemerlicht/context.h"
#include "schemerlicht/stream.h"
#include "schemerlicht/parser.h"
#include "schemerlicht/error.h"

#include "token_tests.h"

static void parse_fixnum_1()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = script2tokens(ctxt, "5");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression);
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
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = script2tokens(ctxt, "-5555");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression);
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
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = script2tokens(ctxt, "3.14");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression);
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
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = script2tokens(ctxt, "()");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression);
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
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = script2tokens(ctxt, "#t");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression);
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
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = script2tokens(ctxt, "#f");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression);
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
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = script2tokens(ctxt, "\"test\"");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression);
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
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = script2tokens(ctxt, "#\\45");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression);
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
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = script2tokens(ctxt, "#\\space");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression);
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
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = script2tokens(ctxt, "(begin 1 2 3 4 5)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_begin, expr->type);
  TEST_EQ_INT(5, expr->expr.beg.arguments.vector_size);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void parse_primcall()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = script2tokens(ctxt, "(+ 1 2)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_primitive_call, expr->type);
  TEST_EQ_INT(2, expr->expr.prim.arguments.vector_size);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void parse_case()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = script2tokens(ctxt, "(case (+ 7 5) [(1 2 3) 'small] [(10 11 12) 'big])");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_case, expr->type);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void parse_cond()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = script2tokens(ctxt, "(cond(#f)(#f 12)(12 13))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_cond, expr->type);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void parse_cond_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = script2tokens(ctxt, "(cond((cons 1 2) => (lambda(x) (cdr x))))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_cond, expr->type);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void parse_do()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = script2tokens(ctxt, "(do ([vec (make-vector 5)] [i 0 (+ i 1)])((= i 5) vec)(vector-set! vec i i))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  TEST_EQ_INT(1, prog.expressions.vector_size);
  schemerlicht_expression* expr = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_do, expr->type);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_error_jump()
  {
  volatile int branch_side_1 = 0;
  volatile int branch_side_2 = 0;
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_longjmp my_long_jump;
  int val = setjmp(my_long_jump.jmp);
  if (val != 0)
    {
    branch_side_1 = 1;
    }
  else
    {
    branch_side_2 = 1;
    ctxt->error_jmp = &my_long_jump;    
    schemerlicht_throw(ctxt, 2);
    }
  TEST_EQ_INT(1, branch_side_1);
  TEST_EQ_INT(1, branch_side_2);
  TEST_EQ_INT(2, my_long_jump.status);
  schemerlicht_close(ctxt);
  }

static void parse_error_aux(const char* script, const char* first_error_message)
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  int contains_error = ctxt->number_of_syntax_errors > 0 ? 1 : 0;  
  TEST_EQ_INT(1, contains_error);
  if (contains_error)
    {
    schemerlicht_error_report* it = schemerlicht_vector_begin(&ctxt->syntax_error_reports, schemerlicht_error_report);
    TEST_EQ_STRING(first_error_message, it->message.string_ptr);
    }
  schemerlicht_program_destroy(ctxt, &prog);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_close(ctxt);
  }

static void parse_errors()
  {
  parse_error_aux("(", "syntax error (1,1): no tokens");
  parse_error_aux("(begin 5 6 7 8 9 10 11 12", "syntax error (1,24): expected keyword: ) expected");
  parse_error_aux("(begin", "syntax error (1,2): no tokens");
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
  parse_primcall();
  parse_case();
  parse_cond();
  parse_cond_2();
  parse_do();
  test_error_jump();
  parse_errors();
  }