#include "compiler_tests.h"
#include "test_assert.h"
#include "token_tests.h"
#include "schemerlicht/parser.h"
#include "schemerlicht/context.h"
#include "schemerlicht/token.h"
#include "schemerlicht/compiler.h"
#include "schemerlicht/vm.h"
#include "schemerlicht/error.h"
#include "schemerlicht/simplifyconv.h"

static void test_compile_fixnum_aux(schemerlicht_fixnum expected_value, const char* script)
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_function func = schemerlicht_compile_expression(ctxt, schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression));
  schemerlicht_object* res = schemerlicht_run(ctxt, &func);

  TEST_EQ_INT(schemerlicht_object_type_fixnum, res->type);
  TEST_EQ_INT(expected_value, res->value.fx);

  schemerlicht_function_destroy(ctxt, &func);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_compile_fixnum()
  {
  test_compile_fixnum_aux(5, "5");
  test_compile_fixnum_aux(45, "(45)");
  test_compile_fixnum_aux(-1, "(-1)");
  test_compile_fixnum_aux(100, "100");
  test_compile_fixnum_aux(4611686018427387903, "4611686018427387903");
  test_compile_fixnum_aux(-4611686018427387904, "-4611686018427387904");
  test_compile_fixnum_aux(9223372036854775807, "9223372036854775807");
  test_compile_fixnum_aux(-9223372036854775807, "-9223372036854775807");
  test_compile_fixnum_aux(-9223372036854775808, "-9223372036854775808");
  test_compile_fixnum_aux(-1, "-1");
  test_compile_fixnum_aux(-123, "-123");
  }

static void test_compile_flonum_aux(schemerlicht_flonum expected_value, const char* script)
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_function func = schemerlicht_compile_expression(ctxt, schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression));
  schemerlicht_object* res = schemerlicht_run(ctxt, &func);

  TEST_EQ_INT(schemerlicht_object_type_flonum, res->type);
  TEST_EQ_DOUBLE(expected_value, res->value.fl);

  schemerlicht_function_destroy(ctxt, &func);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_compile_flonum()
  {
  test_compile_flonum_aux(5.2, "5.2");
  test_compile_flonum_aux(3.14, "3.14");
  test_compile_flonum_aux(3.14, "(3.14)");
  test_compile_flonum_aux(3.14159265359, "(3.14159265359)");
  }

static void test_compile_aux(const char* expected_value, const char* script)
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_function func = schemerlicht_compile_expression(ctxt, schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression));
  schemerlicht_object* res = schemerlicht_run(ctxt, &func);
  schemerlicht_string s = schemerlicht_object_to_string(ctxt, res);

  TEST_EQ_STRING(expected_value, s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);
  schemerlicht_function_destroy(ctxt, &func);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_compile_bool()
  {
  test_compile_aux("#t", "#t");
  test_compile_aux("#t", "(#t)");
  test_compile_aux("#f", "#f");
  test_compile_aux("#f", "(#f)");
  }

static void test_compile_nil()
  {
  test_compile_aux("()", "()");
  test_compile_aux("()", "(())");
  test_compile_aux("()", "((()))");
  test_compile_aux("()", "(((())))");
  }

static void test_compile_char()
  {
  test_compile_aux("#\\*", "#\\*");
  test_compile_aux("#\\a", "#\\97");

  test_compile_aux("#\\*", "(#\\*)");
  test_compile_aux("#\\!", "(#\\!)");
  test_compile_aux("#\\@", "(#\\@)");
  test_compile_aux("#\\#", "(#\\#)");
  test_compile_aux("#\\$", "(#\\$)");
  test_compile_aux("#\\%", "(#\\%)");
  test_compile_aux("#\\^", "(#\\^)");
  test_compile_aux("#\\&", "(#\\&)");
  test_compile_aux("#\\(", "(#\\()");
  test_compile_aux("#\\)", "(#\\))");
  test_compile_aux("#\\\"", "(#\\\")");
  test_compile_aux("#\\+", "(#\\+)");
  test_compile_aux("#\\-", "(#\\-)");
  test_compile_aux("#\\/", "(#\\/)");
  test_compile_aux("#\\'", "(#\\')");
  test_compile_aux("#\\;", "(#\\;)");
  test_compile_aux("#\\:", "(#\\:)");
  test_compile_aux("#\\<", "(#\\<)");
  test_compile_aux("#\\>", "(#\\>)");
  test_compile_aux("#\\.", "(#\\.)");
  test_compile_aux("#\\=", "(#\\=)");
  test_compile_aux("#\\?", "(#\\?)");

  test_compile_aux("#\\a", "#\\a");
  test_compile_aux("#\\b", "#\\b");
  test_compile_aux("#\\c", "#\\c");
  test_compile_aux("#\\d", "#\\d");
  test_compile_aux("#\\e", "#\\e");
  test_compile_aux("#\\f", "#\\f");
  test_compile_aux("#\\g", "#\\g");
  test_compile_aux("#\\h", "#\\h");
  test_compile_aux("#\\i", "#\\i");
  test_compile_aux("#\\j", "#\\j");
  test_compile_aux("#\\k", "#\\k");
  test_compile_aux("#\\l", "#\\l");
  test_compile_aux("#\\m", "#\\m");
  test_compile_aux("#\\n", "#\\n");
  test_compile_aux("#\\o", "#\\o");
  test_compile_aux("#\\p", "#\\p");
  test_compile_aux("#\\q", "#\\q");
  test_compile_aux("#\\r", "#\\r");
  test_compile_aux("#\\s", "#\\s");
  test_compile_aux("#\\t", "#\\t");
  test_compile_aux("#\\u", "#\\u");
  test_compile_aux("#\\v", "#\\v");
  test_compile_aux("#\\w", "#\\w");
  test_compile_aux("#\\x", "#\\x");
  test_compile_aux("#\\y", "#\\y");
  test_compile_aux("#\\z", "#\\z");

  test_compile_aux("#\\A", "#\\A");
  test_compile_aux("#\\B", "#\\B");
  test_compile_aux("#\\C", "#\\C");
  test_compile_aux("#\\D", "#\\D");
  test_compile_aux("#\\E", "#\\E");
  test_compile_aux("#\\F", "#\\F");
  test_compile_aux("#\\G", "#\\G");
  test_compile_aux("#\\H", "#\\H");
  test_compile_aux("#\\I", "#\\I");
  test_compile_aux("#\\J", "#\\J");
  test_compile_aux("#\\K", "#\\K");
  test_compile_aux("#\\L", "#\\L");
  test_compile_aux("#\\M", "#\\M");
  test_compile_aux("#\\N", "#\\N");
  test_compile_aux("#\\O", "#\\O");
  test_compile_aux("#\\P", "#\\P");
  test_compile_aux("#\\Q", "#\\Q");
  test_compile_aux("#\\R", "#\\R");
  test_compile_aux("#\\S", "#\\S");
  test_compile_aux("#\\T", "#\\T");
  test_compile_aux("#\\U", "#\\U");
  test_compile_aux("#\\V", "#\\V");
  test_compile_aux("#\\W", "#\\W");
  test_compile_aux("#\\X", "#\\X");
  test_compile_aux("#\\Y", "#\\Y");
  test_compile_aux("#\\Z", "#\\Z");

  test_compile_aux("#\\000", "#\\000");
  test_compile_aux("#\\001", "#\\001");
  test_compile_aux("#\\002", "#\\002");
  test_compile_aux("#\\003", "#\\003");
  test_compile_aux("#\\004", "#\\004");
  test_compile_aux("#\\005", "#\\005");
  test_compile_aux("#\\006", "#\\006");
  test_compile_aux("#\\007", "#\\007");
  test_compile_aux("#\\008", "#\\008");
  test_compile_aux("#\\009", "#\\009");
  test_compile_aux("#\\010", "#\\010");
  test_compile_aux("#\\011", "#\\011");
  test_compile_aux("#\\012", "#\\012");
  test_compile_aux("#\\013", "#\\013");
  test_compile_aux("#\\014", "#\\014");
  test_compile_aux("#\\015", "#\\015");
  test_compile_aux("#\\016", "#\\016");
  test_compile_aux("#\\017", "#\\017");
  test_compile_aux("#\\018", "#\\018");
  test_compile_aux("#\\019", "#\\019");
  test_compile_aux("#\\020", "#\\020");
  test_compile_aux("#\\021", "#\\021");
  test_compile_aux("#\\022", "#\\022");
  test_compile_aux("#\\023", "#\\023");

  test_compile_aux("#\\000", "#\\nul");
  test_compile_aux("#\\000", "#\\null");
  test_compile_aux("#\\008", "#\\backspace");
  test_compile_aux("#\\009", "#\\tab");
  test_compile_aux("#\\010", "#\\newline");
  test_compile_aux("#\\010", "#\\linefeed");
  test_compile_aux("#\\011", "#\\vtab");
  test_compile_aux("#\\012", "#\\page");
  test_compile_aux("#\\013", "#\\return");
  test_compile_aux("#\\ ", "#\\space");
  test_compile_aux("#\\127", "#\\rubout");
  }

static void test_compile_string()
  {
  test_compile_aux("\"jan\"", "\"jan\"");
  test_compile_aux("\"janneman\"", "\"janneman\"");
  }

static void test_single_argument_primitives()
  {
  test_compile_aux("1", "(add1 0)");
  test_compile_aux("0", "(add1)");
  test_compile_aux("0", "(add1 -1)");
  test_compile_aux("6", "(add1 5)");
  test_compile_aux("-999", "(add1 -1000)");
  test_compile_aux("536870911", "(add1 536870910)");
  test_compile_aux("-536870911", "(add1 -536870912)");
  test_compile_aux("2", "(add1 (add1 0))");
  test_compile_aux("18", "(add1 (add1 (add1 (add1 (add1 (add1 12))))))");
  test_compile_aux("1.500000", "(add1 0.5)");
  test_compile_aux("0.400000", "(add1 -0.6)");
  test_compile_aux("#undefined", "(add1 \"blah\")");
  test_compile_aux("8", "(add1 7 80 900)");
  test_compile_aux("#\\b", "(add1 #\\a)");

  test_compile_aux("0", "(sub1)");
  test_compile_aux("0", "(sub1 1)");
  test_compile_aux("-1", "(sub1 0)");
  test_compile_aux("-2", "(sub1 -1)");
  test_compile_aux("4", "(sub1 5)");
  test_compile_aux("-1001", "(sub1 -1000)");

  test_compile_aux("0.500000", "(sub1 1.5)");
  test_compile_aux("-0.500000", "(sub1 0.5)");
  test_compile_aux("-1.600000", "(sub1 -0.6)");
  }

static void test_add_fixnums()
  {
  test_compile_aux("0", "(+)");
  test_compile_aux("3", "(+ 1 2)");
  test_compile_aux("6", "(+ 1 2 3)");
  test_compile_aux("10", "(+ 1 2 3 4)");
  test_compile_aux("15", "(+ 1 2 3 4 5)");
  test_compile_aux("21", "(+ 1 2 3 4 5 6)");
  test_compile_aux("28", "(+ 1 2 3 4 5 6 7)");
  test_compile_aux("36", "(+ 1 2 3 4 5 6 7 8)");
  test_compile_aux("45", "(+ 1 2 3 4 5 6 7 8 9)");
  test_compile_aux("55", "(+ 1 2 3 4 5 6 7 8 9 10)");
  test_compile_aux("66", "(+ 1 2 3 4 5 6 7 8 9 10 11)");
  }

static void test_add_flonums()
  {
  test_compile_aux("3.000000", "(+ 1.0 2.0)");
  test_compile_aux("6.000000", "(+ 1.0 2.0 3.0)");
  test_compile_aux("10.000000", "(+ 1.0 2.0 3.0 4.0)");
  test_compile_aux("15.000000", "(+ 1.0 2.0 3.0 4.0 5.0)");
  test_compile_aux("21.000000", "(+ 1.0 2.0 3.0 4.0 5.0 6.0)");
  test_compile_aux("28.000000", "(+ 1.0 2.0 3.0 4.0 5.0 6.0 7.0)");
  test_compile_aux("36.000000", "(+ 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0)");
  test_compile_aux("45.000000", "(+ 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0)");
  test_compile_aux("55.000000", "(+ 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0)");
  test_compile_aux("66.000000", "(+ 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0 11.0)");
  }

static void test_add_chars()
  {
  test_compile_aux("#\\%", "(+ #\\005 #\\032)");
  }

static void test_add_mixed()
  {
  test_compile_aux("42", "(+ #\\005 #\\032 5)");
  test_compile_aux("3.000000", "(+ 1 2.0)");
  test_compile_aux("3.000000", "(+ 1.0 2)");
  test_compile_aux("45.140000", "(+ #\\005 #\\032 5 3.14)");
  test_compile_aux("#undefined", "(+ #\\005 #\\032 5 3.14 \"jan\")");
  }

static void test_sub()
  {
  test_compile_aux("0", "(-)");
  test_compile_aux("-1", "(- 1)");
  test_compile_aux("-1", "(- 1 2)");
  test_compile_aux("-3", "(- 1 2 3 -1)");
  test_compile_aux("0.100000", "(- 0.5 0.4)");
  test_compile_aux("-0.000000", "(- 0.5 0.4 0.1)");
  test_compile_aux("5.800000", "(- 7 0.5 0.4 0.1 0.2)");
  test_compile_aux("#\\001", "(- #\\008 #\\003 #\\004)");
  test_compile_aux("#undefined", "(- #\\008 #\\003 #\\004 \"sdf\")");
  }

static void test_mul()
  {
  test_compile_aux("1", "(*)");
  test_compile_aux("7", "(* 7)");
  test_compile_aux("7.000000", "(* 7.0)");
  test_compile_aux("#\\007", "(* #\\007)");
  test_compile_aux("2", "(* 1 2)");
  test_compile_aux("6", "(* 1 2 3)");
  test_compile_aux("-6", "(* 1 2 3 -1)");
  test_compile_aux("1.250000", "(* 0.5 2.5)");
  test_compile_aux("0.125000", "(* 0.5 2.5 0.1)");
  test_compile_aux("#\\232", "(* #\\200 #\\005)");
  }

static void test_div()
  {
  test_compile_aux("1", "(/)");
  test_compile_aux("0", "(/ 8)");
  test_compile_aux("0.125000", "(/ 8.0)");
  test_compile_aux("2", "(/ 4 2)");
  test_compile_aux("2", "(/ 8 2 2)");
  test_compile_aux("-1", "(/ 16 4 -4)");
  test_compile_aux("-1", "(/ 16 -4 4)");
  test_compile_aux("1.250000", "(/ 2.5 2)");
  test_compile_aux("-12.500000", "(/ 2.5 2 -0.1)");
  test_compile_aux("#\\004", "(/ #\\008 #\\002)");
  }

static void test_combination_of_math_ops()
  {
  test_compile_aux("27.000000", "(/ (* 3 (- (+ 23 9) 20.0) 1.5) 2)");
  }

static void test_equality()
  {
  test_compile_aux("#f", "(= 12 13)");
  test_compile_aux("#t", "(= 12 12)");
  test_compile_aux("#f", "(= 12.1 13.1)");
  test_compile_aux("#t", "(= 12.1 12.1)");
  test_compile_aux("#f", "(= 12 13.1)");
  test_compile_aux("#t", "(= 12 12.0)");
  test_compile_aux("#f", "(= 12.0 13)");
  test_compile_aux("#t", "(= 12.0 12)");
  test_compile_aux("#t", "(= 12 12)");
  test_compile_aux("#f", "(= 13 12)");
  test_compile_aux("#f", "(= 16 (+ 13 1)) ");
  test_compile_aux("#t", "(= 16 (+ 13 3))");
  test_compile_aux("#f", "(= 16 (+ 13 13))");
  test_compile_aux("#f", "(= (+ 13 1) 16) ");
  test_compile_aux("#t", "(= (+ 13 3) 16) ");
  test_compile_aux("#f", "(= (+ 13 13) 16)");
  test_compile_aux("#f", "(= 12.0 13)");
  test_compile_aux("#t", "(= 12.0 12)");
  test_compile_aux("#f", "(= 13.0 12)");
  test_compile_aux("#f", "(= 16.0 (+ 13 1)) ");
  test_compile_aux("#t", "(= 16.0 (+ 13 3))");
  test_compile_aux("#f", "(= 16.0 (+ 13 13))");
  test_compile_aux("#f", "(= (+ 13.0 1) 16) ");
  test_compile_aux("#t", "(= (+ 13.0 3) 16.0) ");
  test_compile_aux("#f", "(= (+ 13.0 13.0) 16.0)");
  test_compile_aux("#t", "(= 12 12 12)");
  test_compile_aux("#t", "(= 12 12 12 12)");
  test_compile_aux("#t", "(= 12 12 12 12 12)");
  test_compile_aux("#t", "(= 12 12 12 12 12 12)");
  test_compile_aux("#t", "(= 12 12 12 12 12 12 12)");
  test_compile_aux("#t", "(= 12 12 12 12 12 12 12 12)");
  test_compile_aux("#t", "(= 12 12 12 12 12 12 12 12 12)");
  test_compile_aux("#t", "(= 12 12 12 12 12 12 12 12 12 12)");
  test_compile_aux("#f", "(= 13 12 12)");
  test_compile_aux("#f", "(= 13 12 12 12)");
  test_compile_aux("#f", "(= 13 12 12 12 12)");
  test_compile_aux("#f", "(= 13 12 12 12 12 12)");
  test_compile_aux("#f", "(= 13 12 12 12 12 12 12)");
  test_compile_aux("#f", "(= 13 12 12 12 12 12 12 12)");
  test_compile_aux("#f", "(= 13 12 12 12 12 12 12 12 12)");
  test_compile_aux("#f", "(= 13 12 12 12 12 12 12 12 12 12)");
  test_compile_aux("#f", "(= 12 12 13)");
  test_compile_aux("#f", "(= 12 12 12 13)");
  test_compile_aux("#f", "(= 12 12 12 12 13)");
  test_compile_aux("#f", "(= 12 12 12 12 12 13)");
  test_compile_aux("#f", "(= 12 12 12 12 12 12 13)");
  test_compile_aux("#f", "(= 12 12 12 12 12 12 12 13)");
  test_compile_aux("#f", "(= 12 12 12 12 12 12 12 12 13)");
  test_compile_aux("#f", "(= 12 12 12 12 12 12 12 12 12 13)");
  }

static void test_inequality()
  {
  test_compile_aux("#t", "(!= 12 13)");
  test_compile_aux("#f", "(!= 12 12)");
  test_compile_aux("#t", "(!= 12.1 13.1)");
  test_compile_aux("#f", "(!= 12.1 12.1)");
  test_compile_aux("#t", "(!= 12 13.1)");
  test_compile_aux("#f", "(!= 12 12.0)");
  test_compile_aux("#t", "(!= 12.0 13)");
  test_compile_aux("#f", "(!= 12.0 12)");
  test_compile_aux("#f", "(!= 12 12)");
  test_compile_aux("#t", "(!= 13 12)");
  test_compile_aux("#t", "(!= 16 (+ 13 1)) ");
  test_compile_aux("#f", "(!= 16 (+ 13 3))");
  test_compile_aux("#t", "(!= 16 (+ 13 13))");
  test_compile_aux("#t", "(!= (+ 13 1) 16) ");
  test_compile_aux("#f", "(!= (+ 13 3) 16) ");
  test_compile_aux("#t", "(!= (+ 13 13) 16)");
  test_compile_aux("#t", "(!= 12.0 13)");
  test_compile_aux("#f", "(!= 12.0 12)");
  test_compile_aux("#t", "(!= 13.0 12)");
  test_compile_aux("#t", "(!= 16.0 (+ 13 1)) ");
  test_compile_aux("#f", "(!= 16.0 (+ 13 3))");
  test_compile_aux("#t", "(!= 16.0 (+ 13 13))");
  test_compile_aux("#t", "(!= (+ 13.0 1) 16) ");
  test_compile_aux("#f", "(!= (+ 13.0 3) 16.0) ");
  test_compile_aux("#t", "(!= (+ 13.0 13.0) 16.0)");
  test_compile_aux("#t", "(!= 12 13 14 15 16 17 18 19 20 21 22 23)");
  test_compile_aux("#t", "(!= 12 13 14 15 16 17 18 19 20 21 22 12)");
  }

static void test_less()
  {
  test_compile_aux("#f", "(< 4 2)");
  test_compile_aux("#t", "(< 2 4)");
  test_compile_aux("#f", "(< 4 2 3)");
  test_compile_aux("#t", "(< 2 4 5)");
  test_compile_aux("#f", "(< 2 4 3)");
  test_compile_aux("#f", "(< 4.1 2)");
  test_compile_aux("#t", "(< 2.1 4)");
  test_compile_aux("#f", "(< 4.1 2 3)");
  test_compile_aux("#t", "(< 2.1 4 5)");
  test_compile_aux("#f", "(< 2.1 4 3)");
  test_compile_aux("#t", "(< 12 13)");
  test_compile_aux("#f", "(< 12 12)");
  test_compile_aux("#f", "(< 13 12)");
  test_compile_aux("#f", "(< 16 (+ 13 1)) ");
  test_compile_aux("#f", "(< 16 (+ 13 3))");
  test_compile_aux("#t", "(< 16 (+ 13 13))");
  test_compile_aux("#t", "(< (+ 13 1) 16) ");
  test_compile_aux("#f", "(< (+ 13 3) 16) ");
  test_compile_aux("#f", "(< (+ 13 13) 16)");
  test_compile_aux("#t", "(< 12.0 13)");
  test_compile_aux("#f", "(< 12.0 12)");
  test_compile_aux("#f", "(< 13.0 12)");
  test_compile_aux("#f", "(< 16.0 (+ 13 1.0)) ");
  test_compile_aux("#f", "(< 16.0 (+ 13.0 3.0))");
  test_compile_aux("#t", "(< 16.0 (+ 13.0 13.0))");
  test_compile_aux("#t", "(< (+ 13.0 1) 16.0) ");
  test_compile_aux("#f", "(< (+ 13.0 3.000000001) 16.0)");
  test_compile_aux("#f", "(< (+ 13 13.0) 16)");
  }

static void test_leq()
  {
  test_compile_aux("#t", "(<= 12 13)");
  test_compile_aux("#t", "(<= 12 12)");
  test_compile_aux("#f", "(<= 13 12)");
  test_compile_aux("#f", "(<= 16 (+ 13 1)) ");
  test_compile_aux("#t", "(<= 16 (+ 13 3))");
  test_compile_aux("#t", "(<= 16 (+ 13 13))");
  test_compile_aux("#t", "(<= (+ 13 1) 16) ");
  test_compile_aux("#t", "(<= (+ 13 3) 16) ");
  test_compile_aux("#f", "(<= (+ 13 13) 16)");
  test_compile_aux("#t", "(<= 12.0 13.0)");
  test_compile_aux("#t", "(<= 12.0 12.0)");
  test_compile_aux("#f", "(<= 13.0 12)");
  test_compile_aux("#f", "(<= 16 (+ 13.0 1)) ");
  test_compile_aux("#t", "(<= 16 (+ 13 3.0))");
  test_compile_aux("#t", "(<= 16.0 (+ 13.0 13.0))");
  test_compile_aux("#t", "(<= (+ 13.0 1) 16) ");
  test_compile_aux("#t", "(<= (+ 13 3.0) 16.0) ");
  test_compile_aux("#f", "(<= (+ 13.0 13) 16.0)");
  }

static void test_greater()
  {
  test_compile_aux("#f", "(> 12 13)");
  test_compile_aux("#f", "(> 12 12)");
  test_compile_aux("#t", "(> 13 12)");
  test_compile_aux("#t", "(> 16 (+ 13 1)) ");
  test_compile_aux("#f", "(> 16 (+ 13 3))");
  test_compile_aux("#f", "(> 16 (+ 13 13))");
  test_compile_aux("#f", "(> (+ 13 1) 16) ");
  test_compile_aux("#f", "(> (+ 13 3) 16) ");
  test_compile_aux("#t", "(> (+ 13 13) 16)");
  test_compile_aux("#f", "(> 12.0 13)");
  test_compile_aux("#f", "(> 12.0 12)");
  test_compile_aux("#t", "(> 13.0 12)");
  test_compile_aux("#t", "(> 16.0 (+ 13 1)) ");
  test_compile_aux("#f", "(> 16.0 (+ 13 3))");
  test_compile_aux("#f", "(> 16.0 (+ 13 13))");
  test_compile_aux("#f", "(> (+ 13.0 1) 16) ");
  test_compile_aux("#f", "(> (+ 13.0 3) 16) ");
  test_compile_aux("#t", "(> (+ 13.0 13) 16)");
  }

static void test_geq()
  {
  test_compile_aux("#f", "(>= 12 13)");
  test_compile_aux("#t", "(>= 12 12)");
  test_compile_aux("#t", "(>= 13 12)");
  test_compile_aux("#t", "(>= 16 (+ 13 1)) ");
  test_compile_aux("#t", "(>= 16 (+ 13 3))");
  test_compile_aux("#f", "(>= 16 (+ 13 13))");
  test_compile_aux("#f", "(>= (+ 13 1) 16) ");
  test_compile_aux("#t", "(>= (+ 13 3) 16) ");
  test_compile_aux("#t", "(>= (+ 13 13) 16)");
  test_compile_aux("#f", "(>= 12.0 13)");
  test_compile_aux("#t", "(>= 12.0 12)");
  test_compile_aux("#t", "(>= 13.0 12)");
  test_compile_aux("#t", "(>= 16.0 (+ 13 1)) ");
  test_compile_aux("#t", "(>= 16.0 (+ 13 3))");
  test_compile_aux("#f", "(>= 16.0 (+ 13 13))");
  test_compile_aux("#f", "(>= (+ 13.0 1) 16) ");
  test_compile_aux("#t", "(>= (+ 13.0 3) 16) ");
  test_compile_aux("#t", "(>= (+ 13.0 13) 16)");
  }

static void test_compare_incorrect_argument()
  {
  test_compile_aux("#undefined", "(= 3 #t)");
  test_compile_aux("#undefined", "(!= 3 ())");
  test_compile_aux("#undefined", "(< 3 #t)");
  test_compile_aux("#undefined", "(<= 3 ())");
  test_compile_aux("#undefined", "(> 3 #t)");
  test_compile_aux("#undefined", "(>= 3 ())");
  }

static void test_arithmetic()
  {
  test_compile_aux("45", "(+ (+ (+ (+ (+ (+ (+ (+ 1 2) 3) 4) 5) 6) 7) 8) 9)");
  test_compile_aux("45", "(+ 1 (+ 2 (+ 3 (+ 4 (+ 5 (+ 6 (+ 7 (+ 8 9))))))))");
  test_compile_aux("-43", "(- (- (- (- (- (- (- (- 1 2) 3) 4) 5) 6) 7) 8) 9)");
  test_compile_aux("5", "(- 1 (- 2 (- 3 (- 4 (- 5 (- 6 (- 7 (- 8 9))))))))");
  test_compile_aux("5040", "(* (* (* (* (* 2 3) 4) 5) 6) 7)");
  test_compile_aux("5040", "(* 2 (* 3 (* 4 (* 5 (* 6 7)))))");
  }

static void test_is_fixnum()
  {
  test_compile_aux("#f", "(fixnum? #\\019)");
  test_compile_aux("#f", "(fixnum? #\\a)");
  test_compile_aux("#f", "(fixnum? #t)");
  test_compile_aux("#f", "(fixnum? #f)");
  test_compile_aux("#f", "(fixnum? 0.3)");
  test_compile_aux("#f", "(fixnum? 0.0)");
  test_compile_aux("#f", "(fixnum? ())");
  test_compile_aux("#t", "(fixnum? 0)");
  test_compile_aux("#t", "(fixnum? -1)");
  test_compile_aux("#t", "(fixnum? 1)");
  test_compile_aux("#t", "(fixnum? -1000)");
  test_compile_aux("#t", "(fixnum? 1000)");
  test_compile_aux("#t", "(fixnum? ((1000)))");
  }

static void test_not()
  {
  test_compile_aux("#f", "(not #t)");
  test_compile_aux("#t", "(not #f)");
  test_compile_aux("#t", "(not (not #t))");
  test_compile_aux("#f", "(not #\\a)");
  test_compile_aux("#f", "(not 0)");
  test_compile_aux("#f", "(not 15)");
  test_compile_aux("#f", "(not ())");
  test_compile_aux("#t", "(not (not 15))");
  test_compile_aux("#f", "(not (fixnum? 15))");
  test_compile_aux("#t", "(not (fixnum? #f))");
  }

static void test_is_null()
  {
  test_compile_aux("#t", "(null? ())");
  test_compile_aux("#f", "(null? #f)");
  test_compile_aux("#f", "(null? #t)");
  test_compile_aux("#f", "(null? (null? ()))");
  test_compile_aux("#f", "(null? #\\a)");
  test_compile_aux("#f", "(null? 0)");
  test_compile_aux("#f", "(null? -10)");
  test_compile_aux("#f", "(null? 10)");
  }

static void test_is_flonum()
  {
  test_compile_aux("#f", "(flonum? #\\019)");
  test_compile_aux("#f", "(flonum? #\\a)");
  test_compile_aux("#f", "(flonum? #t)");
  test_compile_aux("#f", "(flonum? #f)");
  test_compile_aux("#t", "(flonum? 0.3)");
  test_compile_aux("#t", "(flonum? 0.0)");
  test_compile_aux("#f", "(flonum? ())");
  test_compile_aux("#t", "(flonum? 0.0)");
  test_compile_aux("#t", "(flonum? -1.50)");
  test_compile_aux("#t", "(flonum? 1.02)");
  test_compile_aux("#f", "(flonum? -1000)");
  test_compile_aux("#f", "(flonum? 1000)");
  test_compile_aux("#t", "(flonum? ((1000.0)))");
  }

static void test_is_zero()
  {
  test_compile_aux("#t", "(zero? 0)");
  test_compile_aux("#f", "(zero? 1)");
  test_compile_aux("#f", "(zero? -1)");
  test_compile_aux("#f", "(zero? 64)");
  test_compile_aux("#f", "(zero? 960)");
  test_compile_aux("#f", "(zero? 53687091158)");
  test_compile_aux("#t", "(zero? 0.0)");
  test_compile_aux("#f", "(zero? 0.0001)");
  test_compile_aux("#f", "(zero? 1.1)");
  test_compile_aux("#f", "(zero? -1.1)");
  test_compile_aux("#f", "(zero? 64.1)");
  test_compile_aux("#f", "(zero? 960.1)");
  test_compile_aux("#f", "(zero? 53687091158.1)");
  test_compile_aux("#f", "(zero? #t)");
  }

static void test_is_boolean()
  {
  test_compile_aux("#t", "(boolean? #t)");
  test_compile_aux("#t", "(boolean? #f)");
  test_compile_aux("#f", "(boolean? 0)");
  test_compile_aux("#f", "(boolean? 1)");
  test_compile_aux("#f", "(boolean? -1)");
  test_compile_aux("#f", "(boolean? ())");
  test_compile_aux("#f", "(boolean? #\\a)");
  test_compile_aux("#t", "(boolean? (boolean? 0))");
  test_compile_aux("#t", "(boolean? (fixnum? (boolean? 0)))");
  }

static void test_is_char()
  {
  test_compile_aux("#t", "(char? #\\a)");
  test_compile_aux("#t", "(char? #\\Z)");
  test_compile_aux("#t", "(char? #\\000)");
  test_compile_aux("#f", "(char? #t)");
  test_compile_aux("#f", "(char? #f)");
  test_compile_aux("#f", "(char? (char? #t))");
  test_compile_aux("#f", "(char? 0)");
  test_compile_aux("#f", "(char? 10)");
  test_compile_aux("#f", "(char? 0.3)");
  }

static void test_fx_arithmetic()
  {
  test_compile_aux("#f", "(fx=? 12 13)");
  test_compile_aux("#f", "(fx=? 13 12)");
  test_compile_aux("#t", "(fx=? 12 12)");
  test_compile_aux("#t", "(fx<? 12 13)");
  test_compile_aux("#f", "(fx<? 13 12)");
  test_compile_aux("#f", "(fx<? 12 12)");
  test_compile_aux("#f", "(fx>? 12 13)");
  test_compile_aux("#t", "(fx>? 13 12)");
  test_compile_aux("#f", "(fx>? 12 12)");
  test_compile_aux("#t", "(fx<=? 12 13)");
  test_compile_aux("#f", "(fx<=? 13 12)");
  test_compile_aux("#t", "(fx<=? 12 12)");
  test_compile_aux("#f", "(fx>=? 12 13)");
  test_compile_aux("#t", "(fx>=? 13 12)");
  test_compile_aux("#t", "(fx>=? 12 12)");
  test_compile_aux("25", "(fx+ 12 13)");
  test_compile_aux("25", "(fx+ 13 12)");
  test_compile_aux("24", "(fx+ 12 12)");
  test_compile_aux("-1", "(fx- 12 13)");
  test_compile_aux("1", "(fx- 13 12)");
  test_compile_aux("0", "(fx- 12 12)");
  test_compile_aux("156", "(fx* 12 13)");
  test_compile_aux("156", "(fx* 13 12)");
  test_compile_aux("144", "(fx* 12 12)");
  test_compile_aux("-156", "(fx* 12 -13)");
  test_compile_aux("-156", "(fx* -13 12)");
  test_compile_aux("-144", "(fx* 12 -12)");
  test_compile_aux("6", "(fx/ 12 2)");
  test_compile_aux("1", "(fx/ 13 12)");
  test_compile_aux("2", "(fx/ 12 5)");
  test_compile_aux("-6", "(fx/ -12 2)");
  test_compile_aux("-1", "(fx/ 13 -12)");
  test_compile_aux("-2", "(fx/ -12 5)");
  }

static void test_if()
  {
  test_compile_aux("2", "(if (< 2 3) (2) (3))");
  test_compile_aux("3", "(if (< 3 2) (2) (3))");
  test_compile_aux("12", "(if #t 12 13)");
  test_compile_aux("13", "(if #f 12 13)");
  test_compile_aux("12", "(if 0 12 13)");
  test_compile_aux("43", "(if () 43 ())");
  test_compile_aux("13", "(if #t (if 12 13 4) 17)");
  test_compile_aux("4", "(if #f 12 (if #f 13 4))");
  test_compile_aux("2", "(if #\\X (if 1 2 3) (if 4 5 6))");
  test_compile_aux("#t", "(if (not (boolean? #t)) 15 (boolean? #f))");
  test_compile_aux("-23", "(if (if (char? #\\a) (boolean? #\\b) (fixnum? #\\c)) 119 -23)");
  test_compile_aux("6", "(if (if (if (not 1) (not 2) (not 3)) 4 5) 6 7)");
  test_compile_aux("7", "(if (not (if (if (not 1) (not 2) (not 3)) 4 5)) 6 7)");
  test_compile_aux("#f", "(not (if (not (if (if (not 1) (not 2) (not 3)) 4 5)) 6 7))");
  test_compile_aux("14", "(if (char? 12) 13 14) )");
  test_compile_aux("13", "(if (char? #\\a) 13 14) )");
  test_compile_aux("13", "(add1 (if (sub1 1) (sub1 13) 14)))");
  test_compile_aux("13", "(if (= 12 13) 12 13) ");
  test_compile_aux("13", "(if (= 12 12) 13 14) ");
  test_compile_aux("12", "(if (< 12 13) 12 13) ");
  test_compile_aux("14", "(if (< 12 12) 13 14) ");
  test_compile_aux("14", "(if (< 13 12) 13 14) ");
  test_compile_aux("12", "(if (<= 12 13) 12 13) ");
  test_compile_aux("12", "(if (<= 12 12) 12 13) ");
  test_compile_aux("14", "(if (<= 13 12) 13 14) ");
  test_compile_aux("13", "(if (> 12 13) 12 13) ");
  test_compile_aux("13", "(if (> 12 12) 12 13) ");
  test_compile_aux("13", "(if (> 13 12) 13 14) ");
  test_compile_aux("13", "(if (>= 12 13) 12 13) ");
  test_compile_aux("12", "(if (>= 12 12) 12 13) ");
  test_compile_aux("13", "(if (>= 13 12) 13 14) ");
  }

static void test_and()
  {
  test_compile_aux("#t", "(and #t)");
  test_compile_aux("#f", "(and #f)");
  test_compile_aux("#t", "(and #t #t)");
  test_compile_aux("#f", "(and #f #f)");
  test_compile_aux("#f", "(and #f #t)");
  test_compile_aux("#f", "(and #t #f)");
  test_compile_aux("#t", "(and #t #t #t)");
  test_compile_aux("#f", "(and #f #t #t)");
  test_compile_aux("#f", "(and #t #t #f)");
  test_compile_aux("#f", "(and #t #f #t)");
  test_compile_aux("#f", "(and #f #f #f)");
  }

static void test_or()
  {
  test_compile_aux("#t", "(or #t)");
  test_compile_aux("#f", "(or #f)");
  test_compile_aux("#t", "(or #t #t)");
  test_compile_aux("#f", "(or #f #f)");
  test_compile_aux("#t", "(or #f #t)");
  test_compile_aux("#t", "(or #t #f)");
  test_compile_aux("#t", "(or #t #t #t)");
  test_compile_aux("#t", "(or #f #t #t)");
  test_compile_aux("#t", "(or #t #t #f)");
  test_compile_aux("#t", "(or #t #f #t)");
  test_compile_aux("#f", "(or #f #f #f)");
  }

static void test_let()
  {
  test_compile_aux("5", "(let ([x 5]) x)");
  test_compile_aux("5", "(let ([x 5][y 6]) x)");
  test_compile_aux("6", "(let ([x 5][y 6]) y)");
  test_compile_aux("3", "(let ([x (+ 1 2)]) x)");
  test_compile_aux("10", "(let ([x (+ 1 2)]) (let([y(+ 3 4)])(+ x y))) ");
  test_compile_aux("4", "(let ([x (+ 1 2)])  (let([y(+ 3 4)])(- y x)))");
  test_compile_aux("4", "(let ([x (+ 1 2)] [y(+ 3 4)])  (- y x))");
  test_compile_aux("18", "(let ([x (let ([y (+ 1 2)]) (* y y))]) (+ x x))");
  test_compile_aux("7", "(let ([x (+ 1 2)]) (let([x(+ 3 4)]) x))");
  test_compile_aux("7", "(let ([x (+ 1 2)]) (let([x(+ x 4)]) x)) ");
  test_compile_aux("3", "(let ([t (let ([t (let ([t (let ([t (+ 1 2)]) t)]) t)]) t)]) t)");
  test_compile_aux("192", "(let ([x 12])  (let([x(+ x x)]) (let([x(+ x x)]) (let([x(+ x x)]) (+ x x)))))");
  test_compile_aux("45", "(let ([a 0] [b 1] [c 2] [d 3] [e 4] [f 5] [g 6] [h 7] [i 8] [j 9]) (+ a b c d e f g h i j) )");
  }

static void test_let_star()
  {
  test_compile_aux("5", "(let* ([x 5]) x)");
  test_compile_aux("3", "(let* ([x (+ 1 2)]) x)");
  test_compile_aux("10","(let* ([x (+ 1 2)] [y(+ 3 4)])(+ x y))");
  test_compile_aux("4", "(let* ([x (+ 1 2)] [y(+ 3 4)]) (- y x))");
  test_compile_aux("18","(let* ([x (let* ([y (+ 1 2)]) (* y y))])(+ x x))");
  test_compile_aux("7", "(let* ([x (+ 1 2)] [x(+ 3 4)]) x)");
  test_compile_aux("7", "(let* ([x (+ 1 2)] [x(+ x 4)]) x)");
  test_compile_aux("3", "(let* ([t (let* ([t (let* ([t (let* ([t (+ 1 2)]) t)]) t)]) t)]) t)");
  test_compile_aux("192", "(let* ([x 12] [x(+ x x)] [x(+ x x)] [x(+ x x)])  (+ x x))");
  }

static void test_compile_error_aux(const char* script, const char* first_error_message)
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_function func = schemerlicht_compile_expression(ctxt, schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression));
  int contains_error = ctxt->number_of_compile_errors > 0 ? 1 : 0;
  TEST_EQ_INT(1, contains_error);
  if (contains_error)
    {
    schemerlicht_error_report* it = schemerlicht_vector_begin(&ctxt->compile_error_reports, schemerlicht_error_report);
    TEST_EQ_STRING(first_error_message, it->message.string_ptr);
    }

  schemerlicht_function_destroy(ctxt, &func);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_compile_errors()
  {
  test_compile_error_aux("(let ([x 5]) y)", "compile error (1,14): variable unknown: y");
  }

void run_all_compiler_tests()
  {
  test_compile_fixnum();
  test_compile_flonum();
  test_compile_bool();
  test_compile_nil();
  test_compile_char();
  test_compile_string();
  test_single_argument_primitives();
  test_add_fixnums();
  test_add_flonums();
  test_add_chars();
  test_add_mixed();
  test_sub();
  test_mul();
  test_div();
  test_combination_of_math_ops();
  test_equality();
  test_inequality();
  test_less();
  test_leq();
  test_greater();
  test_geq();
  test_compare_incorrect_argument();
  test_arithmetic();
  test_is_fixnum();
  test_not();
  test_is_null();
  test_is_flonum();
  test_is_zero();
  test_is_boolean();
  test_is_char();
  test_fx_arithmetic();
  test_if();
  test_and();
  test_or();
  test_let();
  test_let_star();
  test_compile_errors();
  }