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
#include "schemerlicht/begconv.h"
#include "schemerlicht/dump.h"
#include "schemerlicht/globdef.h"
#include "schemerlicht/defconv.h"
#include "schemerlicht/cps.h"
#include "schemerlicht/closure.h"
#include "schemerlicht/assignablevarconv.h"
#include "schemerlicht/lambdatolet.h"
#include "schemerlicht/freevaranalysis.h"
#include "schemerlicht/quotecollect.h"
#include "schemerlicht/quoteconv.h"
#include "schemerlicht/quasiquote.h"
#include "schemerlicht/environment.h"
#include "schemerlicht/gc.h"
#include "schemerlicht/alpha.h"
#include "schemerlicht/callcc.h"
#include "schemerlicht/foreign.h"
#include "schemerlicht/r5rs.h"
#include "schemerlicht/macro.h"
#include "schemerlicht/preprocess.h"
#include "schemerlicht/stackreduce.h"

#include <time.h>

static int print_gc_time = 0;
static int full_preprocessor = 0;

static void test_compile_fixnum_aux(schemerlicht_fixnum expected_value, const char* script)
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);
  schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program);

  TEST_EQ_INT(schemerlicht_object_type_fixnum, res->type);
  TEST_EQ_INT(expected_value, res->value.fx);

  schemerlicht_compiled_program_destroy(ctxt, &compiled_program);
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
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);
  schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program);

  TEST_EQ_INT(schemerlicht_object_type_flonum, res->type);
  TEST_EQ_DOUBLE(expected_value, res->value.fl);

  schemerlicht_compiled_program_destroy(ctxt, &compiled_program);
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

static void test_compile_aux_heap(const char* expected_value, const char* script, int heap_size)
  {
  schemerlicht_context* ctxt = schemerlicht_open(heap_size);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);

  if (full_preprocessor)
    {
    schemerlicht_preprocess(ctxt, &prog);
    }
  else
    {
    schemerlicht_expand_macros(ctxt, &prog);
    schemerlicht_quasiquote_conversion(ctxt, &prog);
    schemerlicht_define_conversion(ctxt, &prog);
    schemerlicht_single_begin_conversion(ctxt, &prog);
    schemerlicht_simplify_to_core_forms(ctxt, &prog);
    schemerlicht_alpha_conversion(ctxt, &prog);
    schemerlicht_vector quotes = schemerlicht_quote_collection(ctxt, &prog);
    schemerlicht_quote_conversion(ctxt, &prog, &quotes);
    schemerlicht_quote_collection_destroy(ctxt, &quotes);
    schemerlicht_stack_reduce_conversion(ctxt, &prog, 200);
    schemerlicht_global_define_environment_allocation(ctxt, &prog);
    schemerlicht_continuation_passing_style(ctxt, &prog);
    schemerlicht_lambda_to_let_conversion(ctxt, &prog);
    schemerlicht_assignable_variable_conversion(ctxt, &prog);
    schemerlicht_free_variable_analysis(ctxt, &prog);
    schemerlicht_closure_conversion(ctxt, &prog);
    }
#if 0
  schemerlicht_string dumped = schemerlicht_dump(ctxt, &prog);
  printf("%s\n", dumped.string_ptr);
  schemerlicht_string_destroy(ctxt, &dumped);
#endif  
  schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);
  schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program);
  schemerlicht_string s = schemerlicht_object_to_string(ctxt, res, 0);

  schemerlicht_print_any_error(ctxt);

#ifdef SCHEMERLICHT_DEBUG
  if (print_gc_time)
    printf("Time spent in GC: %lldms\n", ctxt->time_spent_gc * 1000 / CLOCKS_PER_SEC);
#endif

  TEST_EQ_STRING(expected_value, s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);
  //schemerlicht_function_free(ctxt, func);
  schemerlicht_compiled_program_destroy(ctxt, &compiled_program);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_compile_aux(const char* expected_value, const char* script)
  {
  test_compile_aux_heap(expected_value, script, 256);
  }

static void test_compile_aux_w_dump(const char* expected_value, const char* script)
  {
  schemerlicht_context* ctxt = schemerlicht_open(256 * 256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  if (full_preprocessor)
    {
    schemerlicht_preprocess(ctxt, &prog);
    }
  else
    {
    schemerlicht_expand_macros(ctxt, &prog);
    schemerlicht_quasiquote_conversion(ctxt, &prog);
    schemerlicht_define_conversion(ctxt, &prog);
    schemerlicht_single_begin_conversion(ctxt, &prog);
    schemerlicht_simplify_to_core_forms(ctxt, &prog);
    schemerlicht_alpha_conversion(ctxt, &prog);
    schemerlicht_vector quotes = schemerlicht_quote_collection(ctxt, &prog);
    schemerlicht_quote_conversion(ctxt, &prog, &quotes);
    schemerlicht_quote_collection_destroy(ctxt, &quotes);
    schemerlicht_global_define_environment_allocation(ctxt, &prog);
    schemerlicht_continuation_passing_style(ctxt, &prog);
    schemerlicht_lambda_to_let_conversion(ctxt, &prog);
    schemerlicht_assignable_variable_conversion(ctxt, &prog);
    schemerlicht_free_variable_analysis(ctxt, &prog);
    schemerlicht_closure_conversion(ctxt, &prog);
    }
#if 1
  schemerlicht_string dumped = schemerlicht_dump(ctxt, &prog, 0);
  printf("%s\n", dumped.string_ptr);
  schemerlicht_string_destroy(ctxt, &dumped);
#endif

  schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);
  printf("Compilation done\n");
  //schemerlicht_string assembly = schemerlicht_fun_to_string(ctxt, func);
  //printf("%s\n", assembly.string_ptr);
  //schemerlicht_string_destroy(ctxt, &assembly);

  schemerlicht_string debuginfo;
  schemerlicht_string_init(ctxt, &debuginfo, "");
  schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program);
  //printf("%s\n", debuginfo.string_ptr);
  schemerlicht_string_destroy(ctxt, &debuginfo);

  schemerlicht_string stackstring;
  schemerlicht_string_init(ctxt, &stackstring, "");
  schemerlicht_show_stack(ctxt, &stackstring, 0, 9);
  printf("%s\n", stackstring.string_ptr);
  schemerlicht_string_destroy(ctxt, &stackstring);

  schemerlicht_print_any_error(ctxt);

  schemerlicht_string s = schemerlicht_object_to_string(ctxt, res, 0);

  TEST_EQ_STRING(expected_value, s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);
  schemerlicht_compiled_program_destroy(ctxt, &compiled_program);
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
  test_compile_aux("#\\backspace", "#\\008");
  test_compile_aux("#\\tab", "#\\009");
  test_compile_aux("#\\newline", "#\\010");
  test_compile_aux("#\\vtab", "#\\011");
  test_compile_aux("#\\page", "#\\012");
  test_compile_aux("#\\return", "#\\013");
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
  test_compile_aux("#\\backspace", "#\\backspace");
  test_compile_aux("#\\tab", "#\\tab");
  test_compile_aux("#\\newline", "#\\newline");
  test_compile_aux("#\\newline", "#\\linefeed");
  test_compile_aux("#\\vtab", "#\\vtab");
  test_compile_aux("#\\page", "#\\page");
  test_compile_aux("#\\return", "#\\return");
  test_compile_aux("#\\space", "#\\space");
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
  test_compile_aux("#undefined", "(/ 4 2 0)");
  test_compile_aux("#undefined", "(/ 4 0)");
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
  test_compile_aux("#f", "(>= 1 3 2)");
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
  test_compile_aux("14", "(if (char? 12) 13 14)");
  test_compile_aux("13", "(if (char? #\\a) 13 14)");
  test_compile_aux("13", "(add1 (if (sub1 1) (sub1 13) 14))");
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
  test_compile_aux("10", "(let* ([x (+ 1 2)] [y(+ 3 4)])(+ x y))");
  test_compile_aux("4", "(let* ([x (+ 1 2)] [y(+ 3 4)]) (- y x))");
  test_compile_aux("18", "(let* ([x (let* ([y (+ 1 2)]) (* y y))])(+ x x))");
  test_compile_aux("7", "(let* ([x (+ 1 2)] [x(+ 3 4)]) x)");
  test_compile_aux("7", "(let* ([x (+ 1 2)] [x(+ x 4)]) x)");
  test_compile_aux("3", "(let* ([t (let* ([t (let* ([t (let* ([t (+ 1 2)]) t)]) t)]) t)]) t)");
  test_compile_aux("192", "(let* ([x 12] [x(+ x x)] [x(+ x x)] [x(+ x x)])  (+ x x))");
  }

static void test_compile_error_aux(const char* script, const char* first_error_message)
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_function* func = schemerlicht_compile_expression(ctxt, schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression));
  int contains_error = ctxt->number_of_compile_errors > 0 ? 1 : 0;
  TEST_EQ_INT(1, contains_error);
  if (contains_error)
    {
    schemerlicht_error_report* it = schemerlicht_vector_begin(&ctxt->compile_error_reports, schemerlicht_error_report);
    TEST_EQ_STRING(first_error_message, it->message.string_ptr);
    }

  schemerlicht_function_free(ctxt, func);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_compile_errors()
  {
  //test_compile_error_aux("(let ([x 5]) y)", "compile error (1,14): variable unknown: y"); // used to be an error, but now unlinked variables are allowed as they might be resolved later on by a load operation
  }

static void test_define()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(define x 5) x");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);  
  schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);  
  int contains_error = ctxt->number_of_compile_errors > 0 ? 1 : 0;
  TEST_EQ_INT(0, contains_error);
  schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program);
  schemerlicht_string s = schemerlicht_object_to_string(ctxt, res, 0);
  TEST_EQ_STRING("5", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);
  schemerlicht_compiled_program_destroy(ctxt, &compiled_program);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  tokens = schemerlicht_script2tokens(ctxt, "x");
  prog = make_program(ctxt, &tokens);
  compiled_program = schemerlicht_compile_program(ctxt, &prog);
  res = schemerlicht_run_program(ctxt, &compiled_program);
  s = schemerlicht_object_to_string(ctxt, res, 0);
  TEST_EQ_STRING("5", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);
  schemerlicht_compiled_program_destroy(ctxt, &compiled_program);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_fixnum_char_flonum_conversions()
  {
  test_compile_aux("#\\019", "(fixnum->char 19)");
  test_compile_aux("#\\a", "(fixnum->char 97)");
  test_compile_aux("#\\A", "(fixnum->char 65)");
  test_compile_aux("#\\z", "(fixnum->char 122)");
  test_compile_aux("#\\Z", "(fixnum->char 90)");
  test_compile_aux("#\\0", "(fixnum->char 48)");
  test_compile_aux("#\\9", "(fixnum->char 57)");
  test_compile_aux("19", "(char->fixnum #\\019)");
  test_compile_aux("97", "(char->fixnum #\\a)");
  test_compile_aux("65", "(char->fixnum #\\A)");
  test_compile_aux("122", "(char->fixnum #\\z)");
  test_compile_aux("90", "(char->fixnum #\\Z)");
  test_compile_aux("48", "(char->fixnum #\\0)");
  test_compile_aux("57", "(char->fixnum #\\9)");
  test_compile_aux("12", "(char->fixnum (fixnum->char 12))");
  test_compile_aux("#\\x", "(fixnum->char (char->fixnum #\\x))");
  test_compile_aux("3", "(flonum->fixnum 3.143)");
  test_compile_aux("-3", "(flonum->fixnum -3.143)");
  test_compile_aux("3.000000", "(fixnum->flonum 3)");
  test_compile_aux("#t", "(flonum? (fixnum->flonum 3))");
  }

static void test_bitwise_ops()
  {
  test_compile_aux("6", "(bitwise-not -7)");
  test_compile_aux("6", "(bitwise-not (bitwise-or (bitwise-not 7) 1))");
  test_compile_aux("2", "(bitwise-not (bitwise-or (bitwise-not 7) (bitwise-not 2)))");
  test_compile_aux("12", "(bitwise-and (bitwise-not (bitwise-not 12)) (bitwise-not (bitwise-not 12)))");
  test_compile_aux("19", "(bitwise-or 3 16)");
  test_compile_aux("7", "(bitwise-or 3 5)");
  test_compile_aux("7", "(bitwise-or 3 7)");
  test_compile_aux("6", "(bitwise-not (bitwise-or (bitwise-not 7) 1))");
  test_compile_aux("6", "(bitwise-not (bitwise-or 1 (bitwise-not 7)))");
  test_compile_aux("3", "(bitwise-and 3 7)");
  test_compile_aux("1", "(bitwise-and 3 5)");
  test_compile_aux("0", "(bitwise-and 2346 (bitwise-not 2346))");
  test_compile_aux("0", "(bitwise-and (bitwise-not 2346) 2346)");
  test_compile_aux("2376", "(bitwise-and 2376 2376)");
  }

static void test_vector()
  {
  test_compile_aux("#(1 2)", "(vector 1 2)");
  test_compile_aux("#(1 2 3 4 5 6 7 8)", "(vector 1 2 3 4 5 6 7 8)");
  test_compile_aux("#(1 2 3 4 5 6 7 8 9)", "(vector 1 2 3 4 5 6 7 8 9)");
  test_compile_aux("#(1 2 3 4 5 6 7 8 9 10)", "(vector 1 2 3 4 5 6 7 8 9 10)");
  test_compile_aux("#(1 2 3 4 5 6 7 8 9 10 11 12 13 14 15)", "(vector 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15)");
  test_compile_aux("2", "(let ([x (vector 1 2)]) (vector-ref x 1))");
  test_compile_aux("2", "(let ([x (vector 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15)]) (vector-ref x 1))");
  test_compile_aux("10", "(let ([x (vector 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15)]) (vector-ref x 9))");
  test_compile_aux("1", "(let ([x (vector 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15)]) (vector-ref x 0))");
  test_compile_aux("15", "(let ([x (vector 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15)]) (vector-ref x 14))");

  test_compile_aux("#undefined", "(let ([x (vector 1 2)] [y 3]) (vector-ref y 1))");
  test_compile_aux("#undefined", "(let ([x (vector 1 2)] [y 3]) (vector-ref x #t))");
  test_compile_aux("#undefined", "(let ([x (vector 1 2)] [y 3]) (vector-ref x 2))");
  test_compile_aux("#undefined", "(let ([x (vector 1 2)] [y 3]) (vector-ref x -1))");
  test_compile_aux("100", "(let ([x (vector 1 2)]) (begin (vector-set! x 1 100) (vector-ref x 1)))");
  test_compile_aux("#(1 100)", "(let ([x (vector 1 2)]) (begin (vector-set! x 1 100) x))");
  test_compile_aux("#(3.140000 2)", "(let ([x (vector 1 2)]) (begin (vector-set! x 0 3.14) x))");
  test_compile_aux("#(1 2)", "(let ([x (vector 1 2)]) (begin (vector-set! x 0.0 3.14) x))");
  test_compile_aux("#(1 2)", "(let ([x (vector 1 2)] [y 3]) (begin (vector-set! y 0.0 3.14) x))");
  test_compile_aux("#(1 2)", "(let ([x (vector 1 2)]) (begin (vector-set! x -1 3.14) x))");
  test_compile_aux("#(1 2)", "(let ([x (vector 1 2)]) (begin (vector-set! x 100 3.14) x))");
  test_compile_aux("#t", "(vector? (vector 0)) ");
  test_compile_aux("#f", "(vector? 1287) ");
  test_compile_aux("#f", "(vector? ()) ");
  test_compile_aux("#f", "(vector? #t) ");
  test_compile_aux("#f", "(vector? #f) ");
  }

static void test_pair()
  {
  test_compile_aux("#t", "(pair? (cons 1 2))");
  test_compile_aux("#f", "(pair? 12)");
  test_compile_aux("#f", "(pair? #t)");
  test_compile_aux("#f", "(pair? #f)");
  test_compile_aux("#f", "(pair? ())");
  test_compile_aux("#f", "(fixnum? (cons 12 43))");
  test_compile_aux("#f", "(boolean? (cons 12 43))");
  test_compile_aux("#f", "(null? (cons 12 43))");
  test_compile_aux("#f", "(not (cons 12 43))");
  test_compile_aux("32", "(if (cons 12 43) 32 43)");
  test_compile_aux("1", "(car(cons 1 23))");
  test_compile_aux("123", "(cdr(cons 43 123))");
  test_compile_aux("#t", "(let([x(cons 1 2)] [y(cons 3 4)]) (pair? x))");
  test_compile_aux("#t", "(pair? (cons(cons 12 3) #f))");
  test_compile_aux("#t", "(pair? (cons(cons 12 3) (cons #t #f)))");
  test_compile_aux("12", "(car(car(cons(cons 12 3) (cons #t #f))))");
  test_compile_aux("3", "(cdr(car(cons(cons 12 3) (cons #t #f))))");
  test_compile_aux("#t", "(car(cdr(cons(cons 12 3) (cons #t #f))))");
  test_compile_aux("#f", "(cdr(cdr(cons(cons 12 3) (cons #t #f))))");
  test_compile_aux("#t", "(pair? (cons(* 1 1) 1))");
  test_compile_aux("((1 . 4) 3 . 2)", "(let ([t0 (cons 1 2)] [t1 (cons 3 4)]) (let([a0(car t0)][a1(car t1)][d0(cdr t0)][d1(cdr t1)])(let([t0(cons a0 d1)][t1(cons a1 d0)]) (cons t0 t1))))");
  test_compile_aux("(1 . 2)", "(let ([t (cons 1 2)])(let([t t])(let([t t])(let([t t]) t))))");
  test_compile_aux("(1 . 2)", "(let ([t (let ([t (let ([t (let ([t (cons 1 2)]) t)]) t)]) t)]) t)");
  test_compile_aux("((((()) ()) (()) ()) ((()) ()) (()) ())", "(let ([x ()])(let([x(cons x x)])(let([x(cons x x)])(let([x(cons x x)])(cons x x)))))");
  test_compile_aux("((#t #t . #t) ((#f . #f) . #f))", "(cons (let ([x #t]) (let ([y (cons x x)]) (cons x y))) (cons(let([x #f]) (let([y(cons x x)]) (cons y x))) ())) ");
  test_compile_aux("(1 2 3 4 5 6 7 8 9 10)", "(letrec ([f (lambda (i lst) (if (= i 0) lst (f (sub1 i) (cons i lst))))])(f 10 ())) ");
  test_compile_aux("(2 . #undefined)", "(cons 2)");
  test_compile_aux("(2 . 3)", "(cons 2 3 4)");
  test_compile_aux("#undefined", "(car 2)");
  test_compile_aux("#undefined", "(cdr 2 3 4)");
  }

static void test_begin()
  {
  test_compile_aux("12", "(begin 12)");
  test_compile_aux("122", "(begin 13 122)");
  test_compile_aux("#t", "(begin 123 2343 #t)");
  test_compile_aux("(1 . 2)", "(let ([t (begin 12 (cons 1 2))]) (begin t t))");
  test_compile_aux("(1 . 2)", "(let ([t (begin 13 (cons 1 2))])(begin (cons 1 t)t ))");
  test_compile_aux("(1 . 2)", "(let ([t (cons 1 2)])(if (pair? t)(begin t) 12))");
  test_compile_aux("(1 . 2)", "(let ([t (begin 13 (cons 1 2))])(cons 1 t)t)");
  }

static void test_letrec()
  {
  test_compile_aux("12", "(letrec () 12)");
  test_compile_aux("10", "(letrec () (let ([x 5]) (+ x x)))");
  test_compile_aux("7", "(letrec ([f (lambda () 5)]) 7)");
  test_compile_aux("12", "(letrec ([f (lambda () 5)]) (let ([x 12]) x))");
  test_compile_aux("5", "(let ([f (lambda () 5)]) (f))");
  test_compile_aux("5", "(letrec ([f (lambda () 5)]) (f))");
  test_compile_aux("5", "(letrec ([f (lambda () 5)]) (let ([x (f)]) x))");
  test_compile_aux("11", "(letrec ([f (lambda () 5)]) (+ (f) 6))");
  test_compile_aux("11", "(letrec ([f (lambda () 5)]) (+ 6 (f)))");
  test_compile_aux("15", "(letrec ([f (lambda () 5)]) (- 20 (f)))");
  test_compile_aux("10", "(let ([f (lambda () 5)]) (+ (f) (f)))");
  test_compile_aux("12", "(letrec ([f (lambda () (+ 5 7))])(f))");
  test_compile_aux("25", "(letrec ([f (lambda (x) (+ x 12))]) (f 13))");
  test_compile_aux("12", "(letrec ([f (lambda (x) (+ x 12))]) (f 0))");
  test_compile_aux("24", "(letrec ([f (lambda (x) (+ x 12))]) (f (f 0)))");
  test_compile_aux("36", "(letrec ([f (lambda (x) (+ x 12))]) (f (f (f 0))))");
  test_compile_aux("41", "(letrec ([f (lambda (x y) (+ x y))] [g (lambda(x) (+ x 12))])(f 16 (f (g 0) (+ 1 (g 0)))))");
  test_compile_aux("24", "(letrec ([f (lambda (x) (g x x))][g(lambda(x y) (+ x y))])(f 12))");
  test_compile_aux("34", "(letrec ([f (lambda (x) (+ x 12))]) (f (f 10)))");
  test_compile_aux("36", "(letrec ([f (lambda (x) (+ x 12))]) (f (f (f 0))))");
  test_compile_aux("25", "(let ([f (lambda () 12)][g(lambda() 13)])(+ (f)(g)))");
  test_compile_aux("120", "(letrec ([f (lambda (x) (if (zero? x) 1 (* x(f(sub1 x)))))]) (f 5))");
  test_compile_aux("120", "(letrec ([f (lambda (x acc) (if (zero? x) acc (f(sub1 x) (* acc x))))]) (f 5 1))");
  test_compile_aux_heap("200", "(letrec ([f (lambda (x) (if (zero? x) 0 (+ 1 (f(sub1 x)))))]) (f 200))", 1000);
  test_compile_aux_heap("500", "(letrec ([f (lambda (x) (if (zero? x) 0 (+ 1 (f(sub1 x)))))]) (f 500))", 2000); // [JanM] add test later again when GC is in place
  }

static void test_lambdas()
  {
  test_compile_aux("<lambda>", "(lambda (x) (+ x x))");
  test_compile_aux("3", "((lambda(x) x) 3)");
  test_compile_aux("10", "((lambda(x y) (+ x y)) 3 7)");
  test_compile_aux("8", "(let ([x 5]) ((lambda (y) (+ 3 y)) x ))");
  test_compile_aux("5", "( (lambda () (+ 3 2)) () )");
  test_compile_aux("<lambda>", "(let ([f (lambda () 5)]) f)");
  test_compile_aux("8", "(let ([f (lambda (n) (+ n 5))]) (f 3))");
  test_compile_aux("5", "(let ([f (lambda () 5)]) (f))");
  test_compile_aux("8", "(let ([x 5]) ((lambda (y) (+ x y)) 3) )");
  }

static void test_tailcall()
  {
  test_compile_aux("#f", "(letrec ([e (lambda (x) (if (zero? x) #t (o (sub1 x))))][o(lambda(x) (if (zero? x) #f(e(sub1 x))))])(e 1))");
  test_compile_aux("0", "(letrec ([countdown (lambda (n) (if (zero? n) n (countdown(sub1 n))))])(countdown 5050))");
  test_compile_aux("50005000", "(letrec ([sum (lambda (n ac)(if (zero? n) ac (sum(sub1 n) (+ n ac))))]) (sum 10000 0))");
  test_compile_aux("#t", "(letrec ([e (lambda (x) (if (zero? x) #t (o (sub1 x))))][o(lambda(x) (if (zero? x) #f(e(sub1 x))))])(e 500))");
  }

static void test_closures()
  {
  test_compile_aux("12", "(let ([n 12])(let([f(lambda() n)])(f)))");
  test_compile_aux("112", "(let ([n 12])(let([f(lambda(m) (+ n m))])(f 100)))");
  test_compile_aux("120", "(let ([f (lambda (f n m)(if (zero? n)  m (f(sub1 n) (* n m))))]) (let([g(lambda(g n m) (f(lambda(n m) (g g n m)) n m))])  (g g 5 1)))");
  test_compile_aux("120", "(let ([f (lambda (f n) (if (zero? n) 1 (* n(f(sub1 n)))))]) (let([g(lambda(g n) (f(lambda(n) (g g n)) n))]) (g g 5)))");
  }

static void test_set()
  {
  test_compile_aux("13", "(let ([x 12])(set! x 13) x)");
  test_compile_aux("13", "(let([x 12]) (set! x(add1 x)) x) ");
  test_compile_aux("12", "(let([x 12]) (let([x #f]) (set! x 14)) x) ");
  test_compile_aux("12", "(let([x 12]) (let([y(let([x #f]) (set! x 14))])  x)) ");
  test_compile_aux("10", "(let([f #f])(let([g(lambda() f)]) (set! f 10) (g))) ");
  test_compile_aux("13", "(let([f(lambda(x) (set! x (add1 x)) x)]) (f 12)) ");
  test_compile_aux("(10 . 11)", "(let([x 10]) (let([f(lambda(x) (set! x(add1 x)) x)]) (cons x(f x)))) ");
  test_compile_aux("17", "(let([t #f]) (let([locative (cons (lambda() t) (lambda(n) (set! t n)))]) ((cdr locative) 17) ((car locative))))");
  test_compile_aux("17", "(let([locative (let([t #f]) (cons (lambda() t)  (lambda(n) (set! t n))))]) ((cdr locative) 17) ((car locative))) ");
  test_compile_aux("(1 . 0)", "(let([make-counter (lambda() (let([counter -1]) (lambda()  (set! counter(add1 counter)) counter)))]) (let([c0(make-counter)] [c1(make-counter)])(c0) (cons(c0) (c1)))) ");
  test_compile_aux("120", "(let([fact #f]) (set! fact(lambda(n) (if (zero? n) 1 (* n(fact(sub1 n)))))) (fact 5)) ");
  test_compile_aux("120", "(let([fact #f]) ((begin (set! fact(lambda(n) (if (zero? n)  1 (* n(fact(sub1 n)))))) fact) 5)) ");
  }

static void test_letrec2()
  {
  test_compile_aux("12", "(letrec() 12) ");
  test_compile_aux("12", "(letrec([f 12]) f) ");
  test_compile_aux("25", "(letrec([f 12][g 13]) (+ f g)) ");
  test_compile_aux("120", "(letrec([fact (lambda(n) (if (zero? n)  1  (* n(fact(sub1 n)))))]) (fact 5)) ");
  test_compile_aux("12", "(letrec([f 12][g(lambda() f)]) (g)) ");
  test_compile_aux("130", "(letrec([f 12][g(lambda(n) (set! f n))])(g 130) f) ");
  test_compile_aux("12", "(letrec([f (lambda(g) (set! f g) (f))]) (f(lambda() 12))) ");
  test_compile_aux("100", "(letrec([f (cons (lambda() f) (lambda(x) (set! f x)))]) (let([g(car f)]) ((cdr f) 100) (g))) ");
  test_compile_aux("1", "(let([v (vector 1 2 3)])(vector-ref v 0))");
  test_compile_aux("#(5 2 3)", "(let([v (vector 1 2 3)])(vector-set! v 0 5) v)");
  test_compile_aux("48", "(letrec([f(letrec([g(lambda(x) (* x 2))]) (lambda(n) (g(* n 2))))]) (f 12)) ");
  test_compile_aux("120", "(letrec([f (lambda(f n) (if (zero? n)  1 (* n(f f(sub1 n)))))]) (f f 5)) "); // << This fails if no alpha conversion
  test_compile_aux("120", "(letrec([f1 (lambda(f n) (if (zero? n)  1 (* n(f f(sub1 n)))))]) (f1 f1 5)) "); // manually alpha converted
  test_compile_aux("120", "(let([f(lambda(f) (lambda(n) (if (zero? n)  1 (* n(f(sub1 n))))))]) (letrec([fix (lambda(f) (f(lambda(n) ((fix f) n))))])((fix f) 5))) ");
  }

static void test_inner_define()
  {
  test_compile_aux("7", "(let()(define x 3)(define y 4)(+ x y))");
  test_compile_aux("45", "(let([x 5])(define foo(lambda(y) (bar x y))) (define bar(lambda(a b) (+(* a b) a))) (foo(+ x 3)))");
  }

static void test_global_define()
  {
  test_compile_aux("3", "(define x 3) x");
  test_compile_aux("4", "(define x 3) (set! x 4) x");
  test_compile_aux("4", "(begin (define x 3) (set! x 4) x)");
  }

static void test_list()
  {
  test_compile_aux("()", "(list)");
  test_compile_aux("(3)", "(list 3)");
  test_compile_aux("(3 6)", "(list 3 6)");
  test_compile_aux("(1 2 3)", "(list 1 2 3)");
  test_compile_aux("(1 2 3 4)", "(list 1 2 3 4)");
  test_compile_aux("(1 2 3 4 5)", "(list 1 2 3 4 5)");
  test_compile_aux("(1 2 3 4 5 6)", "(list 1 2 3 4 5 6)");
  test_compile_aux("(1 2 3 4 5 6 7)", "(list 1 2 3 4 5 6 7)");
  test_compile_aux("(1 2 3 4 5 6 7 8)", "(list 1 2 3 4 5 6 7 8)");
  test_compile_aux("(1 2 3 4 5 6 7 8 9)", "(list 1 2 3 4 5 6 7 8 9)");
  test_compile_aux("(1 2 3 4 5 6 7 8 9 10)", "(list 1 2 3 4 5 6 7 8 9 10)");
  test_compile_aux("(1 2 3 4 5 6 7 8 9 10 11)", "(list 1 2 3 4 5 6 7 8 9 10 11)");
  test_compile_aux("(1 2 3 4 5 6 7 8 9 10 11 12)", "(list 1 2 3 4 5 6 7 8 9 10 11 12)");
  }

static void test_scheme()
  {
  test_compile_aux("4", "(+ 2 2)");
  test_compile_aux("210", "(+ (* 2 100) (* 1 10))");
  test_compile_aux("2", "(if (> 6 5) (+ 1 1) (+ 2 2))");
  test_compile_aux("4", "(if (< 6 5) (+ 1 1) (+ 2 2))");
  test_compile_aux("3", "(define x 3)");
  test_compile_aux("3", "(define x 3) x");
  test_compile_aux("6", "(define x 3) (+ x x)");
  test_compile_aux("3", "(begin (define x 1) (set! x (+ x 1)) (+ x 1))");
  test_compile_aux("10", "((lambda (x) (+ x x)) 5)");
  test_compile_aux("<lambda>", "(define twice (lambda (x) (* 2 x)))");
  test_compile_aux("10", "(define twice (lambda (x) (* 2 x))) (twice 5)");
  test_compile_aux("<lambda>", "(define cube (lambda (x) (* x x x)))");
  test_compile_aux("125", "(define cube (lambda (x) (* x x x))) (cube 5)");
  test_compile_aux("<lambda>", "(define compose (lambda (f g) (lambda (x) (f (g x)))))");
  test_compile_aux("<lambda>", "(define compose (lambda (f g) (lambda (x) (f (g x))))) (define repeat (lambda (f) (compose f f)))");
  test_compile_aux("20", "(define twice (lambda (x) (* 2 x))) (define compose (lambda (f g) (lambda (x) (f (g x))))) (define repeat (lambda (f) (compose f f))) ((repeat twice) 5)");
  test_compile_aux("80", "(define twice (lambda (x) (* 2 x))) (define compose (lambda (f g) (lambda (x) (f (g x))))) (define repeat (lambda (f) (compose f f))) ((repeat (repeat twice)) 5)");

  test_compile_aux("120", "(let ([x 5]) (define fact (lambda (n) (if (<= n 1) 1 (* n (fact (- n 1)))))) (fact x))");
  test_compile_aux("<lambda>", "(define fact (lambda (n) (if (<= n 1) 1 (* n (fact (- n 1))))))");
  test_compile_aux("6", "(define fact (lambda (n) (if (<= n 1) 1 (* n (fact (- n 1)))))) (fact 3)");
  test_compile_aux("479001600", "(define fact (lambda (n) (if (<= n 1) 1 (* n (fact (- n 1)))))) (fact 12)");
  test_compile_aux("<lambda>", "(define make_list (lambda (n) (list n)))");
  test_compile_aux("(10)", "(define twice (lambda (x) (* 2 x))) (define make_list (lambda (n) (list n))) (define compose (lambda (f g) (lambda (x) (f (g x)))))  ((compose make_list twice) 5)");

  test_compile_aux("<lambda>", "(define abs (lambda (n) (if (> n 0) n (- 0 n))))");
  test_compile_aux("(3 0 3)", "(define abs (lambda (n) (if (> n 0) n (- 0 n)))) (list (abs -3) (abs 0) (abs 3))");
  test_compile_aux("<lambda>", "(define make_cons (lambda (m n) (cons m n)))");
  test_compile_aux("(1 . 2)", "(define make_cons (lambda (m n) (cons m n))) (make_cons 1 2)");

  test_compile_aux("<lambda>", "(define combine (lambda (f) (lambda (x y) (if (null? x) (quote ()) (f (list (car x) (car y)) ((combine f) (cdr x) (cdr y)))))))");
  test_compile_aux("<closure>", "(define make_cons (lambda (m n) (cons m n)))(define combine (lambda (f) (lambda (x y) (if (null? x) (quote ()) (f (list (car x) (car y)) ((combine f) (cdr x) (cdr y))))))) (define zip (combine make_cons))");
  test_compile_aux("((1 5) (2 6) (3 7) (4 8))", "(define make_cons (lambda (m n) (cons m n)))(define combine (lambda (f) (lambda (x y) (if (null? x) (quote ()) (f (list (car x) (car y)) ((combine f) (cdr x) (cdr y))))))) (define zip (combine make_cons))(zip (list 1 2 3 4) (list 5 6 7 8))");
  }

static void test_fibonacci()
  {
  test_compile_aux("34", "(define fib (lambda (n) (cond [(< n 2) 1]  [else (+ (fib (- n 2)) (fib(- n 1)))]))) (fib 8)");
  test_compile_aux("55", "(define fib (lambda (n) (cond [(< n 2) 1]  [else (+ (fib (- n 2)) (fib(- n 1)))]))) (fib 9)");
  test_compile_aux("89", "(define fib (lambda (n) (cond [(< n 2) 1]  [else (+ (fib (- n 2)) (fib(- n 1)))]))) (fib 10)");
  //test_compile_aux_heap("165580141", "(define fib (lambda (n) (cond [(< n 2) 1]  [else (+ (fib (- n 2)) (fib(- n 1)))]))) (fib 40)", 256*256*256); // without gc heap overflow
  }

static void test_vectors()
  {
  test_compile_aux("#t", "(vector? (make-vector 0)) ");
  test_compile_aux("12", "(vector-length(make-vector 12)) ");
  test_compile_aux("#f", "(vector? (cons 1 2)) ");
  test_compile_aux("#f", "(vector? 1287) ");
  test_compile_aux("#f", "(vector? ()) ");
  test_compile_aux("#f", "(vector? #t) ");
  test_compile_aux("#f", "(vector? #f) ");
  test_compile_aux("#f", "(pair? (make-vector 12)) ");
  test_compile_aux("#f", "(null? (make-vector 12)) ");
  test_compile_aux("#f", "(boolean? (make-vector 12)) ");
  test_compile_aux("#()", "(make-vector 0) ");
  test_compile_aux("#(3.140000 3.140000 3.140000 3.140000 3.140000)", "(make-vector 5 3.14) ");
  test_compile_aux("12", "(vector-ref (make-vector 5 12) 0)");
  test_compile_aux("12", "(vector-ref (make-vector 5 12) 1)");
  test_compile_aux("12", "(vector-ref (make-vector 5 12) 2)");
  test_compile_aux("12", "(vector-ref (make-vector 5 12) 3)");
  test_compile_aux("12", "(vector-ref (make-vector 5 12) 4)");

  test_compile_aux("#(5 3.140000 (3 . 2))", "(vector 5 3.14 (cons 3 2)) ");

  test_compile_aux("#(#t #f)", "(let([v(make-vector 2)]) (vector-set! v 0 #t)(vector-set! v 1 #f) v) ");

  test_compile_aux("(#(100 200) . #(300 400))", "(let([v0(make-vector 2)])(let([v1(make-vector 2)]) (vector-set! v0 0 100) (vector-set! v0 1 200)  (vector-set! v1 0 300)  (vector-set! v1 1 400) (cons v0 v1))) ");
  test_compile_aux("(#(100 200 150) . #(300 400 350))", "(let([v0(make-vector 3)]) (let([v1(make-vector 3)]) (vector-set! v0 0 100)  (vector-set! v0 1 200)  (vector-set! v0 2 150) (vector-set! v1 0 300)  (vector-set! v1 1 400)  (vector-set! v1 2 350)  (cons v0 v1))) ");
  test_compile_aux("(#(100 200) . #(300 400))", "(let([n 2])  (let([v0(make-vector n)]) (let([v1(make-vector n)])  (vector-set! v0 0 100) (vector-set! v0 1 200)  (vector-set! v1 0 300) (vector-set! v1 1 400)  (cons v0 v1)))) ");
  test_compile_aux("(#(100 200 150) . #(300 400 350))", "(let([n 3]) (let([v0(make-vector n)]) (let([v1(make-vector(vector-length v0))]) (vector-set! v0(- (vector-length v0) 3) 100) (vector-set! v0(- (vector-length v1) 2) 200) (vector-set! v0(- (vector-length v0) 1) 150) (vector-set! v1(- (vector-length v1) 3) 300)  (vector-set! v1(- (vector-length v0) 2) 400)  (vector-set! v1(- (vector-length v1) 1) 350)  (cons v0 v1)))) ");
  test_compile_aux("1", "(let([n 1]) (vector-set! (make-vector n) (sub1 n) (* n n)) n) ");
  test_compile_aux("1", "(let([n 1])(let([v(make-vector 1)]) (vector-set! v(sub1 n) n) (vector-ref  v(sub1 n)))) ");
  test_compile_aux("(#(2) . #(13))", "(let([v0(make-vector 1)]) (vector-set! v0 0 1) (let([v1(make-vector 1)]) (vector-set! v1 0 13)  (vector-set! (if (vector? v0) v0 v1) (sub1(vector-length(if (vector? v0) v0 v1)))  (add1(vector-ref  (if (vector? v0) v0 v1)  (sub1(vector-length(if (vector? v0) v0 v1))))))  (cons v0 v1))) ");
  test_compile_aux("100", "(letrec([f (lambda(v i) (if (>= i 0) (begin (vector-set! v i i) (f v (sub1 i))) v))])(let([ v (make-vector 100) ]) (vector-length (f v (sub1 100)))))");

  test_compile_aux("#t", "(let([v(make-vector 2)]) (vector-set! v 0 v) (vector-set! v 1 v) (eq? (vector-ref  v 0) (vector-ref  v 1))) ");
  test_compile_aux("((1 . 2) . #t)", "(let([v(make-vector 1)][y(cons 1 2)]) (vector-set! v 0 y) (cons y(eq? y(vector-ref  v 0)))) ");
  test_compile_aux("(3 . 3)", "(letrec([f (lambda(v i) (if (>= i 0) (f v (sub1 i)) v))])(let([ v (cons 3 3) ]) (f v 3)))");

  test_compile_aux("#(#undefined #undefined #undefined #undefined #undefined #undefined #undefined #undefined #undefined #undefined)", "(let ([a (make-vector 10)] [b (make-vector 10)] [c (make-vector 10)]) a)");
  test_compile_aux("#(#t #t #t)", "(let ([v (vector 1 2 3)]) (vector-fill! v #t) v)");
  }

static void test_strings()
  {
  test_compile_aux("#t", "(string? (make-string 3)) ");
  test_compile_aux("#f", "(string? (make-vector 3)) ");

  test_compile_aux("\"aaaaa\"", "(make-string 5 #\\a)");
  test_compile_aux("\"bbbbbb\"", "(make-string 6 #\\b)");
  test_compile_aux("\"ccccccc\"", "(make-string 7 #\\c)");
  test_compile_aux("\"dddddddd\"", "(make-string 8 #\\d)");
  test_compile_aux("\"eeeeeeeee\"", "(make-string 9 #\\e)");
  test_compile_aux("\"ffffffffff\"", "(make-string 10 #\\f)");
  test_compile_aux("\"ggggggggggg\"", "(make-string 11 #\\g)");
  test_compile_aux("\"hhhhhhhhhhhh\"", "(make-string 12 #\\h)");
  test_compile_aux("\"iiiiiiiiiiiii\"", "(make-string 13 #\\i)");
  test_compile_aux("\"jjjjjjjjjjjjjj\"", "(make-string 14 #\\j)");
  test_compile_aux("\"kkkkkkkkkkkkkkk\"", "(make-string 15 #\\k)");
  test_compile_aux("\"llllllllllllllll\"", "(make-string 16 #\\l)");
  test_compile_aux("\"mmmmmmmmmmmmmmmmm\"", "(make-string 17 #\\m)");
  test_compile_aux("\"jan\"", "(string #\\j #\\a #\\n)");
  test_compile_aux("\"janneman\"", "(string #\\j #\\a #\\n #\\n #\\e #\\m #\\a #\\n)");
  test_compile_aux("\"jannemanneke\"", "(string #\\j #\\a #\\n #\\n #\\e #\\m #\\a #\\n #\\n #\\e #\\k #\\e)");
  test_compile_aux("\"jannemannekejannemanneke\"", "(string #\\j #\\a #\\n #\\n #\\e #\\m #\\a #\\n #\\n #\\e #\\k #\\e #\\j #\\a #\\n #\\n #\\e #\\m #\\a #\\n #\\n #\\e #\\k #\\e)");

  test_compile_aux("0", "(string-length (make-string 0 #\\a))");
  test_compile_aux("1", "(string-length (make-string 1 #\\a))");
  test_compile_aux("2", "(string-length (make-string 2 #\\a))");
  test_compile_aux("3", "(string-length (make-string 3 #\\a))");
  test_compile_aux("4", "(string-length (make-string 4 #\\a))");
  test_compile_aux("5", "(string-length (make-string 5 #\\a))");
  test_compile_aux("6", "(string-length (make-string 6 #\\a))");
  test_compile_aux("7", "(string-length (make-string 7 #\\a))");
  test_compile_aux("8", "(string-length (make-string 8 #\\a))");
  test_compile_aux("9", "(string-length (make-string 9 #\\a))");
  test_compile_aux("10", "(string-length (make-string 10 #\\a))");

  test_compile_aux("0", "(string-length (make-string 0))");
  test_compile_aux("1", "(string-length (make-string 1))");
  test_compile_aux("2", "(string-length (make-string 2))");
  test_compile_aux("3", "(string-length (make-string 3))");
  test_compile_aux("4", "(string-length (make-string 4))");
  test_compile_aux("5", "(string-length (make-string 5))");
  test_compile_aux("6", "(string-length (make-string 6))");
  test_compile_aux("7", "(string-length (make-string 7))");
  test_compile_aux("8", "(string-length (make-string 8))");
  test_compile_aux("9", "(string-length (make-string 9))");
  test_compile_aux("10", "(string-length (make-string 10))");

  test_compile_aux("#\\j", "(string-ref (string #\\j #\\a #\\n #\\n #\\e #\\m #\\a #\\n) 0)");
  test_compile_aux("#\\a", "(string-ref (string #\\j #\\a #\\n #\\n #\\e #\\m #\\a #\\n) 1)");
  test_compile_aux("#\\n", "(string-ref (string #\\j #\\a #\\n #\\n #\\e #\\m #\\a #\\n) 2)");
  test_compile_aux("#undefined", "(string-ref (string #\\j #\\a #\\n #\\n #\\e #\\m #\\a #\\n) 9)");

  test_compile_aux("#\\j", "(string-ref (string #\\j #\\a #\\n) 0)");
  test_compile_aux("#\\a", "(string-ref (string #\\j #\\a #\\n) 1)");
  test_compile_aux("#\\n", "(string-ref (string #\\j #\\a #\\n) 2)");
  test_compile_aux("#undefined", "(string-ref (string #\\j #\\a #\\n) 4)");

  test_compile_aux("\"abcde\"", "(let([s (make-string 5 #\\a)]) (string-set! s 1 #\\b)  (string-set! s 2 #\\c) (string-set! s 3 #\\d) (string-set! s 4 #\\e) s)");
  test_compile_aux("#\\c", "(let([s (make-string 5 #\\a)]) (string-set! s 1 #\\b)  (string-set! s 2 #\\c) (string-set! s 3 #\\d) (string-set! s 4 #\\e) (string-ref s 2))");
  test_compile_aux("\"\"", "(make-string 0)");
  test_compile_aux("#\\a", "(let([s(make-string 1)])(string-set! s 0 #\\a) (string-ref s 0))");
  test_compile_aux("(#\\a . #\\b)", "(let([s(make-string 2)]) (string-set! s 0 #\\a) (string-set! s 1 #\\b) (cons(string-ref s 0) (string-ref s 1)))");
  test_compile_aux("#\\a", "(let([i 0])(let([s(make-string 1)])(string-set! s i #\\a) (string-ref s i)))");
  test_compile_aux("(#\\a . #\\b)", "(let([i 0][j 1])(let([s(make-string 2)])(string-set! s i #\\a)  (string-set! s j #\\b)  (cons(string-ref s i) (string-ref s j))))");
  test_compile_aux("#\\a", "(let([i 0][c #\\a])(let([s(make-string 1)])(string-set! s i c) (string-ref s i)))");
  test_compile_aux("\"jan\"", "(string #\\j #\\a #\\n)");
  test_compile_aux("\"janneman\"", "(string #\\j #\\a #\\n #\\n #\\e #\\m #\\a #\\n)");

  test_compile_aux("12", "(string-length(make-string 12)) ");
  test_compile_aux("#f", "(string? (make-vector 12)) ");
  test_compile_aux("#f", "(string? (cons 1 2)) ");
  test_compile_aux("#f", "(string? 1287) ");
  test_compile_aux("#f", "(string? ()) ");
  test_compile_aux("#f", "(string? #t) ");
  test_compile_aux("#f", "(string? #f) ");
  test_compile_aux("#f", "(pair? (make-string 12)) ");
  test_compile_aux("#f", "(null? (make-string 12)) ");
  test_compile_aux("#f", "(boolean? (make-string 12)) ");
  test_compile_aux("#f", "(vector? (make-string 12)) ");
  test_compile_aux("\"\"", "(make-string 0)");

  test_compile_aux("\"tf\"", "(let([v(make-string 2)])(string-set! v 0 #\\t)(string-set! v 1 #\\f) v)");
  test_compile_aux("#t", "(let([v(make-string 2)]) (string-set! v 0 #\\x) (string-set! v 1 #\\x) (char=? (string-ref v 0) (string-ref v 1))) ");
  test_compile_aux("(\"abc\" . \"def\")", "(let([v0(make-string 3)]) (let([v1(make-string 3)]) (string-set! v0 0 #\\a) (string-set! v0 1 #\\b)(string-set! v0 2 #\\c) (string-set! v1 0 #\\d) (string-set! v1 1 #\\e) (string-set! v1 2 #\\f) (cons v0 v1))) ");
  test_compile_aux("(\"ab\" . \"cd\")", "(let([n 2])(let([v0(make-string n)])(let([v1(make-string n)])(string-set! v0 0 #\\a)(string-set! v0 1 #\\b)(string-set! v1 0 #\\c)(string-set! v1 1 #\\d)(cons v0 v1)))) ");
  test_compile_aux("(\"abc\" . \"ZYX\")", "(let([n 3])(let([v0(make-string n)])(let([v1(make-string(string-length v0))])(string-set! v0(- (string-length v0) 3) #\\a)(string-set! v0(- (string-length v1) 2) #\\b)(string-set! v0(- (string-length v0) 1) #\\c)(string-set! v1(- (string-length v1) 3) #\\Z)(string-set! v1(- (string-length v0) 2) #\\Y)(string-set! v1(- (string-length v1) 1) #\\X)(cons v0 v1)))) ");
  test_compile_aux("1", "(let([n 1])(string-set! (make-string n) (sub1 n) (fixnum->char 34)) n) ");
  test_compile_aux("1", "(let([n 1]) (let([v(make-string 1)]) (string-set! v(sub1 n) (fixnum->char n)) (char->fixnum(string-ref v(sub1 n))))) ");
  test_compile_aux("(\"b\" . \"A\")", "(let([v0(make-string 1)]) (string-set! v0 0 #\\a) (let([v1(make-string 1)]) (string-set! v1 0 #\\A) (string-set! (if (string? v0) v0 v1) (sub1(string-length(if (string? v0) v0 v1))) (fixnum->char (add1 (char->fixnum (string-ref (if (string? v0) v0 v1)(sub1(string-length(if (string? v0) v0 v1)))))))) (cons v0 v1))) ");
  test_compile_aux("\"\\\"\"", "(let([s(make-string 1)]) (string-set! s 0 #\\\") s) ");
  test_compile_aux("\"\\\\\"", "(let([s(make-string 1)]) (string-set! s 0 #\\\\) s) ");
  }

static void test_quotes()
  {
  test_compile_aux("(1 2 3)", "(quote (1 2 3))");
  test_compile_aux("3", "(if #f (quote ()) 3)");
  test_compile_aux("()", "(quote ())");
  test_compile_aux("1", "(quote 1)");
  test_compile_aux("1", "(quote 1)");
  test_compile_aux("1.300000", "(quote 1.3)");
  test_compile_aux("\"Jan\"", "(quote \"Jan\")");
  test_compile_aux("#t", "(quote #t)");
  test_compile_aux("#f", "(quote #f)");
  test_compile_aux("a", "(quote a)");
  test_compile_aux("a", "(begin (quote a) (quote ()) (quote a))");
  test_compile_aux("a", "(begin (quote a) (quote ()) (quote a))");
  test_compile_aux("(1 2)", "(quote (1 2))");
  test_compile_aux("#(1 2 3.140000 #t)", "(quote #(1 2 3.14 #t))");

  test_compile_aux("42", "quote 42 ");
  test_compile_aux("(1 . 2)", "quote (1 . 2) ");
  test_compile_aux("(1 2 3)", "quote (1 2 3) ");
  test_compile_aux("(1 2 3)", "quote (1 2 3) ");
  test_compile_aux("(1 2 3)", "(let([x quote (1 2 3)]) x) ");
  test_compile_aux("(1 2 3)", "(let([f(lambda() quote (1 2 3))]) (f)) ");
  test_compile_aux("#t", "(let([f(lambda() quote (1 2 3))]) (eq? (f)(f))) ");
  test_compile_aux("(1 2 3)", "(let([f(lambda() (lambda()  quote (1 2 3)))]) ((f))) ");
  test_compile_aux("(#(1 2 3) . 1)", "(let([x quote #(1 2 3)]) (cons x(vector-ref x 0))) ");
  test_compile_aux("\"Hello World\"", "\"Hello World\" ");
  test_compile_aux("(\"Hello\" \"World\")", "quote (\"Hello\" \"World\") ");

  test_compile_aux("42", "'42 ");
  test_compile_aux("(1 . 2)", "'(1 . 2) ");
  test_compile_aux("(1 2 3)", "'(1 2 3) ");
  test_compile_aux("(1 2 3)", "(let([x '(1 2 3)]) x) ");
  test_compile_aux("(1 2 3)", "(let([f(lambda() '(1 2 3))]) (f)) ");
  test_compile_aux("#t", "(let([f(lambda() '(1 2 3))]) (eq? (f)(f))) ");
  test_compile_aux("(1 2 3)", "(let([f(lambda() (lambda()  '(1 2 3)))]) ((f))) ");
  test_compile_aux("(#(1 2 3) . 1)", "(let([x '#(1 2 3)]) (cons x(vector-ref x 0))) ");
  test_compile_aux("\"Hello World\"", "\"Hello World\" ");
  test_compile_aux("(\"Hello\" \"World\")", "'(\"Hello\" \"World\") ");

  test_compile_aux("#\\a", "(quote #\\a)");
  test_compile_aux("(a b c)", "(quote (a b c))");
  test_compile_aux("#f", "(symbol? (quote (a b c)))");
  test_compile_aux("#t", "(symbol? (quote a ))");
  }

static void test_length()
  {
  test_compile_aux("0", "(length ())");
  test_compile_aux("1", "(length (list 8))");
  test_compile_aux("2", "(length (list 8 9))");
  test_compile_aux("3", "(length (list 8 9 10))");
  test_compile_aux("8", "(length (list 8 9 10 11 12 13 14 15))");
  test_compile_aux("#undefined", "(length 5)");
  }

static void test_set_car_cdr()
  {
  test_compile_aux("(1)", "(let ([x (cons 1 2)])(begin(set-cdr! x ()) x))");
  test_compile_aux("(1)", "(let ([x (cons 1 2)]) (set-cdr! x ()) x)");
  test_compile_aux("(12 14 . 15)", "(let ([x (cons 12 13)] [y (cons 14 15)])  (set-cdr! x y) x)");
  test_compile_aux("(14 12 . 13)", "(let ([x (cons 12 13)] [y (cons 14 15)]) (set-cdr! y x) y)");
  test_compile_aux("(12 . 13)", "(let ([x (cons 12 13)] [y (cons 14 15)])(set-cdr! y x)x)");
  test_compile_aux("(14 . 15)", "(let ([x (cons 12 13)] [y (cons 14 15)]) (set-cdr! x y) y)");
  test_compile_aux("(#t . #f)", "(let ([x (let ([x (cons 1 2)]) (set-car! x #t) (set-cdr! x #f) x)]) (cons x x) x)");
  test_compile_aux("(#t . #t)", "(let ([x (cons 1 2)]) (set-cdr! x x)  (set-car! (cdr x) x) (cons(eq? x(car x)) (eq? x(cdr x))))");
  test_compile_aux("#f", "(let ([x #f])(if (pair? x) (set-car! x 12) #f)  x)");
  }

static void test_when_unless()
  {
  test_compile_aux("(3 . 2)", "(let([x(cons 1 2)]) (when(pair? x) (set-car! x(+ (car x) (cdr x)))) x) ");
  test_compile_aux("(5 . 2)", "(let([x(cons 1 2)]) (when(pair? x) (set-car! x(+ (car x) (cdr x))) (set-car! x(+ (car x) (cdr x)))) x) ");
  test_compile_aux("(3 . 2)", "(let([x(cons 1 2)]) (unless(fixnum? x) (set-car! x(+ (car x) (cdr x)))) x) ");
  test_compile_aux("(5 . 2)", "(let([x(cons 1 2)]) (unless(fixnum? x) (set-car! x(+ (car x) (cdr x))) (set-car! x(+ (car x) (cdr x)))) x) ");
  }

static void test_symbol()
  {
  test_compile_aux("#t", "(symbol? 'foo) ");
  test_compile_aux("#f", "(symbol? '()) ");
  test_compile_aux("#f", "(symbol? "") ");
  test_compile_aux("#f", "(symbol? '(1 2)) ");
  test_compile_aux("#f", "(symbol? '#()) ");
  test_compile_aux("#f", "(symbol? (lambda(x) x)) ");
  test_compile_aux("#t", "(symbol? 'foo) ");
  test_compile_aux("#f", "(string? 'foo) ");
  test_compile_aux("#f", "(pair? 'foo) ");
  test_compile_aux("#f", "(vector? 'foo) ");
  test_compile_aux("#f", "(null? 'foo) ");
  test_compile_aux("#f", "(boolean? 'foo) ");
  test_compile_aux("#f", "(procedure? 'foo) ");
  test_compile_aux("#f", "(eq? 'foo 'bar) ");
  test_compile_aux("#t", "(eq? 'foo 'foo) ");
  test_compile_aux("foo", "'foo ");
  test_compile_aux("(foo bar baz)", "'(foo bar baz) ");
  test_compile_aux("(foo foo foo foo foo foo foo foo foo foo foo)", "'(foo foo foo foo foo foo foo foo foo foo foo)  ");
  }

static void test_primitive_objects()
  {
  test_compile_aux("3", "(define op (lambda (a b c) (a b c)))   (op + 1 2)");
  test_compile_aux("7", "(define op (lambda (a b c) (a b c)))  (define three (lambda () 3)) (define four (lambda () 4)) (op + (three) (four))");
  test_compile_aux("7", "(define op (lambda (a b c) (a (b) (c))))  (define three (lambda () 3)) (define four (lambda () 4)) (op + three four)");
  test_compile_aux("8", "(define fib (lambda (n) (cond [(< n 2) 1]  [else (+ (fib (- n 2)) (fib(- n 1)))]))) (define op (lambda (a b c) (a b c)))  (op + (fib 3) (fib 4))");
  test_compile_aux("<lambda>", "(define twice (lambda (x) (* 2 x)))");
  test_compile_aux("10", "(define twice (lambda (x) (* 2 x))) (twice 5)");
  test_compile_aux("<lambda>", "(define compose (lambda (f g) (lambda (x) (f (g x)))))");
  test_compile_aux("(10)", "(define twice (lambda (x) (* 2 x))) (define compose (lambda (f g) (lambda (x) (f (g x))))) ((compose list twice) 5)");
  test_compile_aux("<lambda>", "(define abs (lambda (n) ((if (> n 0) + -) 0 n)))");
  test_compile_aux("(3 0 3)", "(define abs (lambda (n) ((if (> n 0) + -) 0 n))) (list (abs -3) (abs 0) (abs 3))");
  }

static void test_applying_thunks()
  {
  test_compile_aux("12", "(let([f(lambda() 12)]) (f)) ");
  test_compile_aux("25", "(let([f(lambda() (+ 12 13))]) (f)) ");
  test_compile_aux("26", "(let([f(lambda() 13)]) (+ (f)(f))) ");
  test_compile_aux("50", "(let([f(lambda()(let([g(lambda() (+ 2 3))])(* (g)(g))))])(+ (f)(f))) ");
  test_compile_aux("50", "(let([f(lambda()(let([f(lambda() (+ 2 3))])(* (f)(f))))]) (+ (f)(f))) ");
  test_compile_aux("14", "(let([f(if (boolean? (lambda() 12))(lambda() 13)(lambda() 14))])(f)) ");
  }

static void test_parameter_passing()
  {
  test_compile_aux("12", "(let([f(lambda(x) x)]) (f 12)) ");
  test_compile_aux("25", "(let([f(lambda(x y) (+ x y))]) (f 12 13)) ");
  test_compile_aux("1100", "(let([f(lambda(x)(let([g(lambda(x y) (+ x y))])(g x 100)))])(f 1000)) ");
  test_compile_aux("26", "(let([f(lambda(g) (g 2 13))])(f(lambda(n m) (* n m)))) ");
  test_compile_aux("10100", "(let([f(lambda(g) (+ (g 10) (g 100)))])(f(lambda(x) (* x x)))) ");
  test_compile_aux("120", "(let([f(lambda(f n m)(if (zero? n) m (f f(sub1 n) (* n m))))]) (f f 5 1)) ");
  test_compile_aux("120", "(let([f(lambda(f n)(if (zero? n) 1 (* n(f f(sub1 n)))))]) (f f 5)) ");
  }

static void test_cond()
  {
  test_compile_aux("2", "(cond[1 2][else 3]) ");
  test_compile_aux("1", "(cond[1][else 13]) ");
  test_compile_aux("#f", "(cond[#f #t][#t #f]) ");
  test_compile_aux("17", "(cond[else 17]) ");
  test_compile_aux("13", "(cond[#f][#f 12][12 13]) ");
  test_compile_aux("1287", "(let([b #t])(cond [else 1287])) ");
  test_compile_aux("2", "(cond[(cons 1 2) => (lambda(x) (cdr x))]) ");
  test_compile_aux("yes", "(if (> 3 2) 'yes 'no)");
  test_compile_aux("no", "(if (> 2 3) 'yes 'no)");
  test_compile_aux("1", "(if (> 3 2) (- 3 2)(+ 3 2))");
  test_compile_aux("greater", "(cond [(> 3 2) 'greater] [(< 3 2) 'less])");
  test_compile_aux("less", "(cond [(> 2 3) 'greater] [(< 2 3) 'less])");
  test_compile_aux("equal", "(cond [(> 3 3) 'greater] [(< 3 3) 'less] [else 'equal])");
  }

static void test_newton()
  {
  test_compile_aux("14.142136", "(define abs (lambda (n) ( (if (> n 0) + -) 0 n) ))  (define newton lambda(guess function derivative epsilon) (define guess2 (- guess (/ (function guess) (derivative guess)))) (if (< (abs(- guess guess2)) epsilon) guess2 (newton guess2 function derivative epsilon)))(define square-root lambda(a) (newton 1 (lambda(x1) (-(* x1 x1) a)) (lambda(x2) (* 2 x2)) 1e-8))(square-root 200.)");
  test_compile_aux("14.142136", "(define abs(lambda(n) (if (> n 0) n(- 0 n))))(define newton lambda(guess function derivative epsilon) (define guess2 (- guess (/ (function guess) (derivative guess)))) (if (< (abs(- guess guess2)) epsilon) guess2 (newton guess2 function derivative epsilon)))(define square-root lambda(a) (newton 1 (lambda(x) (-(* x x) a)) (lambda(x) (* 2 x)) 1e-8))(square-root 200.)");
  test_compile_aux("5", "(define abs1 (lambda (n) ((if (> n 0) + -) 0 n) )) (define abs2 (lambda (n) (abs1 (- (abs1 n))))) (abs2 -5)");
  test_compile_aux("-500", "(define abs1 (lambda (n) ((if (> n 0) + -) 0 n) ))  (- (abs1 -500))");
  test_compile_aux("-500", "(define appl (lambda (op n) (op n) ))  (- (appl - -500))");
  }

static void test_compile_aux_callcc(const char* expected_value, const char* script)
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_compile_callcc(ctxt);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_quasiquote_conversion(ctxt, &prog);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  schemerlicht_alpha_conversion(ctxt, &prog);
  schemerlicht_vector quotes = schemerlicht_quote_collection(ctxt, &prog);
  schemerlicht_quote_conversion(ctxt, &prog, &quotes);
  schemerlicht_quote_collection_destroy(ctxt, &quotes);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);
  schemerlicht_continuation_passing_style(ctxt, &prog);
  schemerlicht_lambda_to_let_conversion(ctxt, &prog);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_free_variable_analysis(ctxt, &prog);
  schemerlicht_closure_conversion(ctxt, &prog);
#if 0
  schemerlicht_string dumped = schemerlicht_dump(ctxt, &prog);
  printf("%s\n", dumped.string_ptr);
  schemerlicht_string_destroy(ctxt, &dumped);
#endif
  schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);
  schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program);
  schemerlicht_string s = schemerlicht_object_to_string(ctxt, res, 0);

#ifdef SCHEMERLICHT_DEBUG
  if (print_gc_time)
    printf("Time spent in GC: %lldms\n", ctxt->time_spent_gc * 1000 / CLOCKS_PER_SEC);
#endif

  TEST_EQ_STRING(expected_value, s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);
  schemerlicht_compiled_program_destroy(ctxt, &compiled_program);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_compile_cc_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_compile_callcc(ctxt);

#if 0
  schemerlicht_object callcc;
  callcc.type = schemerlicht_object_type_lambda;
  callcc.value.ptr = cast(void*, callccfunc);

  schemerlicht_string callcc_name;
  schemerlicht_string_init(ctxt, &callcc_name, "call/cc");
  schemerlicht_environment_entry entry;
  entry.type = SCHEMERLICHT_ENV_TYPE_GLOBAL;
  entry.position = ctxt->globals.vector_size;
  schemerlicht_vector_push_back(ctxt, &ctxt->globals, callcc, schemerlicht_object);

  schemerlicht_environment_add(ctxt, &callcc_name, entry);
#endif

  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(call/cc (lambda(throw) (+ 5 (* 10 (throw 1)))))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  schemerlicht_alpha_conversion(ctxt, &prog);
  schemerlicht_vector quotes = schemerlicht_quote_collection(ctxt, &prog);
  schemerlicht_quote_conversion(ctxt, &prog, &quotes);
  schemerlicht_quote_collection_destroy(ctxt, &quotes);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);
  schemerlicht_continuation_passing_style(ctxt, &prog);
  schemerlicht_lambda_to_let_conversion(ctxt, &prog);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_free_variable_analysis(ctxt, &prog);
  schemerlicht_closure_conversion(ctxt, &prog);
#if 0
  schemerlicht_string dumped = schemerlicht_dump(ctxt, &prog);
  printf("%s\n", dumped.string_ptr);
  schemerlicht_string_destroy(ctxt, &dumped);
#endif

  schemerlicht_vector compiled_program1 = schemerlicht_compile_program(ctxt, &prog);
  schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program1);

  schemerlicht_string s = schemerlicht_object_to_string(ctxt, res, 0);
  TEST_EQ_STRING("1", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);

  schemerlicht_compiled_program_destroy(ctxt, &compiled_program1);
  schemerlicht_close(ctxt);


  test_compile_aux_callcc("1", "(call/cc (lambda(throw) (+ 5 (* 10 (throw 1)))))");
  test_compile_aux_callcc("15", "(call/cc (lambda(throw) (+ 5 (* 10 1))))");
  test_compile_aux_callcc("35", "(call/cc (lambda(throw) (+ 5 (* 10 (call/cc(lambda(escape) (* 100 (escape 3))))))))");
  test_compile_aux_callcc("3", "(call/cc(lambda(throw) (+ 5 (* 10 (call/cc(lambda(escape) (* 100 (throw 3))))))))");
  test_compile_aux_callcc("1005", "(call/cc(lambda(throw) (+ 5 (* 10 (call/cc(lambda(escape) (* 100 1)))))))");
  }

static void test_compile_cc()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(define call/cc (lambda(k f) (f k (lambda(dummy-k result) (k result)))))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  schemerlicht_alpha_conversion(ctxt, &prog);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);
  //schemerlicht_continuation_passing_style(ctxt, &prog);
  schemerlicht_lambda_to_let_conversion(ctxt, &prog);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_free_variable_analysis(ctxt, &prog);
  schemerlicht_closure_conversion(ctxt, &prog);
  schemerlicht_vector callcc = schemerlicht_compile_program(ctxt, &prog);
  schemerlicht_run_program(ctxt, &callcc);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);

  tokens = schemerlicht_script2tokens(ctxt, "(call/cc (lambda(throw) (+ 5 (* 10 (throw 1)))))");
  prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  schemerlicht_alpha_conversion(ctxt, &prog);
  schemerlicht_vector quotes = schemerlicht_quote_collection(ctxt, &prog);
  schemerlicht_quote_conversion(ctxt, &prog, &quotes);
  schemerlicht_quote_collection_destroy(ctxt, &quotes);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);
  schemerlicht_continuation_passing_style(ctxt, &prog);
  schemerlicht_lambda_to_let_conversion(ctxt, &prog);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_free_variable_analysis(ctxt, &prog);
  schemerlicht_closure_conversion(ctxt, &prog);

  schemerlicht_vector func1 = schemerlicht_compile_program(ctxt, &prog);
  schemerlicht_object* res = schemerlicht_run_program(ctxt, &func1);
  schemerlicht_string s = schemerlicht_object_to_string(ctxt, res, 0);
  TEST_EQ_STRING("1", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);

  tokens = schemerlicht_script2tokens(ctxt, "(call/cc (lambda(throw) (+ 5 (* 10 1))))");
  prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  schemerlicht_alpha_conversion(ctxt, &prog);
  quotes = schemerlicht_quote_collection(ctxt, &prog);
  schemerlicht_quote_conversion(ctxt, &prog, &quotes);
  schemerlicht_quote_collection_destroy(ctxt, &quotes);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);
  schemerlicht_continuation_passing_style(ctxt, &prog);
  schemerlicht_lambda_to_let_conversion(ctxt, &prog);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_free_variable_analysis(ctxt, &prog);
  schemerlicht_closure_conversion(ctxt, &prog);

  schemerlicht_vector func2 = schemerlicht_compile_program(ctxt, &prog);
  res = schemerlicht_run_program(ctxt, &func2);
  s = schemerlicht_object_to_string(ctxt, res, 0);
  TEST_EQ_STRING("15", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);

  tokens = schemerlicht_script2tokens(ctxt, "(call/cc (lambda(throw) (+ 5 (* 10 (call/cc(lambda(escape) (* 100 (escape 3))))))))");
  prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  schemerlicht_alpha_conversion(ctxt, &prog);
  quotes = schemerlicht_quote_collection(ctxt, &prog);
  schemerlicht_quote_conversion(ctxt, &prog, &quotes);
  schemerlicht_quote_collection_destroy(ctxt, &quotes);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);
  schemerlicht_continuation_passing_style(ctxt, &prog);
  schemerlicht_lambda_to_let_conversion(ctxt, &prog);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_free_variable_analysis(ctxt, &prog);
  schemerlicht_closure_conversion(ctxt, &prog);

  schemerlicht_vector func3 = schemerlicht_compile_program(ctxt, &prog);
  res = schemerlicht_run_program(ctxt, &func3);
  s = schemerlicht_object_to_string(ctxt, res, 0);
  TEST_EQ_STRING("35", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);

  tokens = schemerlicht_script2tokens(ctxt, "(call/cc(lambda(throw) (+ 5 (* 10 (call/cc(lambda(escape) (* 100 (throw 3))))))))");
  prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  schemerlicht_alpha_conversion(ctxt, &prog);
  quotes = schemerlicht_quote_collection(ctxt, &prog);
  schemerlicht_quote_conversion(ctxt, &prog, &quotes);
  schemerlicht_quote_collection_destroy(ctxt, &quotes);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);
  schemerlicht_continuation_passing_style(ctxt, &prog);
  schemerlicht_lambda_to_let_conversion(ctxt, &prog);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_free_variable_analysis(ctxt, &prog);
  schemerlicht_closure_conversion(ctxt, &prog);

  schemerlicht_vector func4 = schemerlicht_compile_program(ctxt, &prog);
  res = schemerlicht_run_program(ctxt, &func4);
  s = schemerlicht_object_to_string(ctxt, res, 0);
  TEST_EQ_STRING("3", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);

  tokens = schemerlicht_script2tokens(ctxt, "(call/cc(lambda(throw) (+ 5 (* 10 (call/cc(lambda(escape) (* 100 1)))))))");
  prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  schemerlicht_alpha_conversion(ctxt, &prog);
  quotes = schemerlicht_quote_collection(ctxt, &prog);
  schemerlicht_quote_conversion(ctxt, &prog, &quotes);
  schemerlicht_quote_collection_destroy(ctxt, &quotes);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);
  schemerlicht_continuation_passing_style(ctxt, &prog);
  schemerlicht_lambda_to_let_conversion(ctxt, &prog);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_free_variable_analysis(ctxt, &prog);
  schemerlicht_closure_conversion(ctxt, &prog);

  schemerlicht_vector func5 = schemerlicht_compile_program(ctxt, &prog);
  res = schemerlicht_run_program(ctxt, &func5);
  s = schemerlicht_object_to_string(ctxt, res, 0);
  TEST_EQ_STRING("1005", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);

  schemerlicht_compiled_program_destroy(ctxt, &func1);
  schemerlicht_compiled_program_destroy(ctxt, &func2);
  schemerlicht_compiled_program_destroy(ctxt, &func3);
  schemerlicht_compiled_program_destroy(ctxt, &func4);
  schemerlicht_compiled_program_destroy(ctxt, &func5);
  schemerlicht_compiled_program_destroy(ctxt, &callcc);
  schemerlicht_close(ctxt);
  }

static void test_ack_performance()
  {
  print_gc_time = 1;
  //currently less than 2s with large heap (size 256*256*256) on my laptop
  int c0 = clock();
  test_compile_aux_heap("4093", "(define (ack m n) (cond((= m 0) (+ n 1)) ((= n 0) (ack(- m 1) 1)) (else (ack(- m 1) (ack m(- n 1)))))) (ack 3 9)", 256 * 256);
  //test_compile_aux_heap("32765", "(define (ack m n) (cond((= m 0) (+ n 1)) ((= n 0) (ack(- m 1) 1)) (else (ack(- m 1) (ack m(- n 1)))))) (ack 3 12)", 256 * 256 * 16);
  int c1 = clock();
  printf("Ack time: %lldms\n", (int64_t)(c1 - c0) * (int64_t)1000 / (int64_t)CLOCKS_PER_SEC);
  print_gc_time = 0;
  }

static void test_fib_performance()
  {
  print_gc_time = 1;
  int c0 = clock();
  //test_compile_aux_heap("165580141", "(define fib (lambda (n) (cond [(fx<? n 2) 1]  [else (fx+ (fib (fx- n 2)) (fib(fx- n 1)))]))) (fib 40)", 256 * 256);
  //test_compile_aux_heap("165580141", "(define fib (lambda (n) (cond [(< n 2) 1]  [else (+ (fib (- n 2)) (fib(- n 1)))]))) (fib 40)", 256 * 256);
  //test_compile_aux_heap("102334155", "(define(fib n)(if (< n 2) n (+(fib(- n 1))(fib(- n 2))))) (fib 40)", 256 * 256);
  //test_compile_aux_heap("1346269", "(define(fib n)(if (< n 2) n (+(fib(- n 1))(fib(- n 2))))) (fib 30)",  256*256);
  test_compile_aux_heap("9227465", "(define(fib n)(if (< n 2) n (+(fib(- n 1))(fib(- n 2))))) (fib 35)", 256 * 256);
  int c1 = clock();
  printf("Fib time: %lldms\n", (int64_t)(c1 - c0) * (int64_t)1000 / (int64_t)CLOCKS_PER_SEC);
  //test_compile_aux_w_dump("55", "(define(fib n)(if (< n 2) n (+(fib(- n 1))(fib(- n 2))))) (fib 10)");
  print_gc_time = 0;
  }

static void test_lambda_variable_arity_not_using_rest_arg()
  {
  test_compile_aux("12", "(let([f(lambda args 12)]) (f)) ");
  test_compile_aux("12", "(let([f(lambda args 12)]) (f 10)) ");
  test_compile_aux("12", "(let([f(lambda args 12)]) (f 10 20)) ");
  test_compile_aux("12", "(let([f(lambda args 12)]) (f 10 20 30)) ");
  test_compile_aux("12", "(let([f(lambda args 12)]) (f 10 20 30 40)) ");
  test_compile_aux("12", "(let([f(lambda args 12)]) (f 10 20 30 40 50)) ");
  test_compile_aux("12", "(let([f(lambda args 12)]) (f 10 20 30 40 50 60 70 80 90)) ");
  test_compile_aux("12", "(let([f(lambda (a0 . args) 12)]) (f 10)) ");
  test_compile_aux("10", "(let([f(lambda (a0 . args) a0)]) (f 10)) ");
  test_compile_aux("12", "(let([f(lambda (a0 . args) 12)]) (f 10 20)) ");
  test_compile_aux("10", "(let([f(lambda (a0 . args) a0)]) (f 10 20)) ");
  test_compile_aux("12", "(let([f(lambda (a0 . args) 12)]) (f 10 20 30)) ");
  test_compile_aux("10", "(let([f(lambda (a0 . args) a0)]) (f 10 20 30)) ");
  test_compile_aux("12", "(let([f(lambda (a0 . args) 12)]) (f 10 20 30 40)) ");
  test_compile_aux("10", "(let([f(lambda (a0 . args) a0)]) (f 10 20 30 40)) ");
  test_compile_aux("#(10 20)", "(let([f(lambda(a0 a1 . args) (vector a0 a1))]) (f 10 20 30 40 50 60 70 80 90 100)) ");
  test_compile_aux("#(10 20 30)", "(let([f(lambda(a0 a1 a2 . args) (vector a0 a1 a2))]) (f 10 20 30 40 50 60 70 80 90 100)) ");
  test_compile_aux("#(10 20 30 40)", "(let([f(lambda(a0 a1 a2 a3 . args) (vector a0 a1 a2 a3))]) (f 10 20 30 40 50 60 70 80 90 100)) ");
  test_compile_aux("#(10 20 30 40 50)", "(let([f(lambda(a0 a1 a2 a3 a4 . args) (vector a0 a1 a2 a3 a4))]) (f 10 20 30 40 50 60 70 80 90 100)) ");
  test_compile_aux("#(10 20 30 40 50 60)", "(let([f(lambda(a0 a1 a2 a3 a4 a5 . args) (vector a0 a1 a2 a3 a4 a5))]) (f 10 20 30 40 50 60 70 80 90 100)) ");
  }

static void test_lambda_variable_arity_while_using_rest_arg()
  {
  test_compile_aux("()", "(let([f(lambda args args)]) (f)) ");
  test_compile_aux("(10)", "(let([f(lambda args args)]) (f 10)) ");
  test_compile_aux("(10 20)", "(let([f(lambda args args)]) (f 10 20)) ");
  test_compile_aux("(10 20 30)", "(let([f(lambda args args)]) (f 10 20 30)) ");
  test_compile_aux("(10 20 30 40)", "(let([f(lambda args args)]) (f 10 20 30 40)) ");
  test_compile_aux("(10 20 30 40 50)", "(let([f(lambda args args)]) (f 10 20 30 40 50)) ");
  test_compile_aux("(10 20 30 40 50 60)", "(let([f(lambda args args)]) (f 10 20 30 40 50 60)) ");
  test_compile_aux("(10 20 30 40 50 60 70)", "(let([f(lambda args args)]) (f 10 20 30 40 50 60 70)) ");
  test_compile_aux("#(10 ())", "(let([f(lambda(a0 . args) (vector a0 args))]) (f 10)) ");
  test_compile_aux("#(10 (20))", "(let([f(lambda(a0 . args) (vector a0 args))]) (f 10 20)) ");
  test_compile_aux("#(10 (20 30))", "(let([f(lambda(a0 . args) (vector a0 args))]) (f 10 20 30)) ");
  test_compile_aux("#(10 (20 30 40))", "(let([f(lambda(a0 . args) (vector a0 args))]) (f 10 20 30 40)) ");
  test_compile_aux("#(10 20 (30 40 50 60 70 80 90))", "(let([f(lambda(a0 a1 . args) (vector a0 a1 args))]) (f 10 20 30 40 50 60 70 80 90)) ");
  test_compile_aux("#(10 20 30 (40 50 60 70 80 90))", "(let([f(lambda(a0 a1 a2 . args) (vector a0 a1 a2 args))]) (f 10 20 30 40 50 60 70 80 90)) ");
  test_compile_aux("#(10 20 30 40 (50 60 70 80 90))", "(let([f(lambda(a0 a1 a2 a3 . args) (vector a0 a1 a2 a3 args))]) (f 10 20 30 40 50 60 70 80 90)) ");
  test_compile_aux("#(10 20 30 40 50 (60 70 80 90))", "(let([f(lambda(a0 a1 a2 a3 a4 . args) (vector a0 a1 a2 a3 a4 args))]) (f 10 20 30 40 50 60 70 80 90)) ");
  test_compile_aux("#(10 20 30 40 50 60 (70 80 90))", "(let([f(lambda(a0 a1 a2 a3 a4 a5 . args)(vector a0 a1 a2 a3 a4 a5 args))]) (f 10 20 30 40 50 60 70 80 90)) ");
  test_compile_aux("#(10 20 30 40 50 60 70 (80 90))", "(let([f(lambda(a0 a1 a2 a3 a4 a5 a6 . args)(vector a0 a1 a2 a3 a4 a5 a6 args))]) (f 10 20 30 40 50 60 70 80 90)) ");
  test_compile_aux("#(10 20 30 40 50 60 70 80 (90))", "(let([f(lambda(a0 a1 a2 a3 a4 a5 a6 a7 . args)(vector a0 a1 a2 a3 a4 a5 a6 a7 args))]) (f 10 20 30 40 50 60 70 80 90)) ");
  test_compile_aux("#(\"a\" (\"b\" \"c\" \"d\"))", "(let([f(lambda(a0 . args) (vector a0 args))]) (f \"a\" \"b\" \"c\" \"d\")) ");
  }

static void test_lambda_long_list()
  {
  test_compile_aux("#(10 20 30 40 50 60 70 80 (90))", "(let([f(lambda(a0 a1 a2 a3 a4 a5 a6 a7 a8)(vector a0 a1 a2 a3 a4 a5 a6 a7 (list a8)))]) (f 10 20 30 40 50 60 70 80 90)) ");
  test_compile_aux("#(10 20 30 40 50 60 70 80 90)", "((lambda(a0 a1 a2 a3 a4 a5 a6 a7 a8)(vector a0 a1 a2 a3 a4 a5 a6 a7 a8)) 10 20 30 40 50 60 70 80 90) ");
  test_compile_aux("450", "((lambda(a0 a1 a2 a3 a4 a5 a6 a7 a8) (+ a0 a1 a2 a3 a4 a5 a6 a7 a8)) 10 20 30 40 50 60 70 80 90) ");
  }

static void test_lambda_variable_arity_while_using_rest_arg_and_closure()
  {
  test_compile_aux("112", "(let ([n 12])(let([f(lambda(m . args) (+ n m))])(f 100 5 2 8)))");
  test_compile_aux("112", "(let ([n 12])(let([f(lambda(m . args) (+ n m))])(f 100 5)))");
  test_compile_aux("112", "(let ([n 12])(let([f(lambda(m . args) (+ n m))])(f 100 ())))");
  test_compile_aux("112", "(let ([n 12])(let([f(lambda(m . args) (+ n m))])(f 100)))");
  test_compile_aux("115", "(let ([n 12][m 100])(let([f (lambda(a b . args) (+ a b n m))])(f 1 2 5)))");
  test_compile_aux("115", "(let ([n 12][m 100])(let([f (lambda(a b . args) (+ a b n m))])(f 1 2)))");
  test_compile_aux("3", "(let([f (lambda(a b . args) (+ a b))])(f 1 2 3 4))");
  test_compile_aux("6", "(let([f (lambda(a b . args) (+ a b (car args)))])(f 1 2 3 4))");
  test_compile_aux("10", "(let([f (lambda(a b . args) (+ a b (car args) (car (cdr args))))])(f 1 2 3 4))");
  test_compile_aux("322", "(let ([n 12][m 100][z 200])(let([f (lambda(a b . args) (+ a b z n m (car args) (car (cdr args))))])(f 1 2 3 4)))");
  test_compile_aux("315", "(let ([n 12][m 100][z 200])(let([f (lambda(a b . args) (+ a b z n m))])(f 1 2 3 4)))");
  }

static void test_garbage_collection()
  {
  schemerlicht_context* ctxt = schemerlicht_open(200);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(vector 1 2 3)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  schemerlicht_alpha_conversion(ctxt, &prog);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);
  schemerlicht_continuation_passing_style(ctxt, &prog);
  schemerlicht_lambda_to_let_conversion(ctxt, &prog);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_free_variable_analysis(ctxt, &prog);
  schemerlicht_closure_conversion(ctxt, &prog);  
  schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);
  TEST_EQ_INT(0, ctxt->heap_pos);
  TEST_EQ_INT(0, schemerlicht_need_to_perform_gc(ctxt));

  for (int i = 0; i < 80; ++i)
    {
    schemerlicht_run_program(ctxt, &compiled_program);
    //schemerlicht_string stackstring = schemerlicht_show_stack(ctxt, 0, 2);
    //printf("%s\n", stackstring.string_ptr);
    //schemerlicht_string_destroy(ctxt, &stackstring);
    TEST_EQ_INT(i + 1, ctxt->heap_pos);
    TEST_EQ_INT(0, schemerlicht_need_to_perform_gc(ctxt));
    }
  schemerlicht_run_program(ctxt, &compiled_program);
  TEST_EQ_INT(1, ctxt->heap_pos);
  TEST_EQ_INT(0, schemerlicht_need_to_perform_gc(ctxt));
  schemerlicht_run_program(ctxt, &compiled_program);
  TEST_EQ_INT(2, ctxt->heap_pos);
  TEST_EQ_INT(0, schemerlicht_need_to_perform_gc(ctxt));

  schemerlicht_compiled_program_destroy(ctxt, &compiled_program);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_is_equal()
  {
  test_compile_aux("#t", "(equal? 'yes 'yes)");
  test_compile_aux("#f", "(equal? 'yes 'no)");
  test_compile_aux("#t", "(equal? (* 6 7) 42)");
  test_compile_aux("#f", "(equal? 2 2.0)");
  test_compile_aux("#t", "(let ([v (cons 1 2)]) (equal? v v))");
  test_compile_aux("#t", "(equal? (cons 1 2) (cons 1 2))");
  test_compile_aux("#t", "(equal? (cons (cons 3 4) 2) (cons (cons 3 4) 2))");
  test_compile_aux("#t", "(equal? (fixnum->char 955) (fixnum->char 955))");
  test_compile_aux("#t", "(equal? (make-string 3 #\\z) (make-string 3 #\\z))");
  test_compile_aux("#t", "(equal? #t #t)");
  test_compile_aux("#t", "(eqv? 'yes 'yes)");
  test_compile_aux("#f", "(eqv? 'yes 'no)");
  test_compile_aux("#t", "(eqv? (* 6 7) 42)");
  test_compile_aux("#f", "(eqv? 2 2.0)");
  test_compile_aux("#t", "(let ([v (cons 1 2)]) (eqv? v v))");

  test_compile_aux("#f", "(eqv? (cons 1 2) (cons 1 2))");
  test_compile_aux("#f", "(eqv? (cons (cons 3 4) 2) (cons (cons 3 4) 2))");
  test_compile_aux("#t", "(eqv? (fixnum->char 955) (fixnum->char 955))");
  test_compile_aux("#f", "(eqv? (make-string 3 #\\z) (make-string 3 #\\z))");
  test_compile_aux("#t", "(eqv? #t #t)");

  test_compile_aux("#t", "(eq? 'yes 'yes)");
  test_compile_aux("#f", "(eq? 'yes 'no)");
  test_compile_aux("#t", "(eq? (* 6 7) 42)");
  test_compile_aux("#f", "(eq? 2 2.0)");
  test_compile_aux("#t", "(let ([v (cons 1 2)]) (eq? v v))");
  test_compile_aux("#f", "(eq? (cons 1 2) (cons 1 2))");
  test_compile_aux("#f", "(eq? (cons (cons 3 4) 2) (cons (cons 3 4) 2))");
  test_compile_aux("#t", "(eq? (fixnum->char 955) (fixnum->char 955))");
  test_compile_aux("#f", "(eq? (make-string 3 #\\z) (make-string 3 #\\z))");
  test_compile_aux("#t", "(eq? #t #t)");
  }

static void test_memv_memq_member()
  {
  test_compile_aux("(2 3 4)", "(memv 2 (list 1 2 3 4))");
  test_compile_aux("#f", "(memv 9 (list 1 2 3 4))");
  test_compile_aux("(101 102)", "(memv 101 '(100 101 102))");
  test_compile_aux("#f", "(memv 101 '((b a) (y x)))");
  test_compile_aux("(a c)", "(memv 'a '(b a c))");
  test_compile_aux("#f", "(memv '(y x) '((b a) (y x)))");

  test_compile_aux("(2 3 4)", "(memq 2 (list 1 2 3 4))");
  test_compile_aux("#f", "(memq 9 (list 1 2 3 4))");
  test_compile_aux("(101 102)", "(memq 101 '(100 101 102))");
  test_compile_aux("#f", "(memq 101 '((b a) (y x)))");
  test_compile_aux("(a c)", "(memq 'a '(b a c))");
  test_compile_aux("#f", "(memq '(y x) '((b a) (y x)))");

  test_compile_aux("(2 3 4)", "(member 2 (list 1 2 3 4))");
  test_compile_aux("#f", "(member 9 (list 1 2 3 4))");
  test_compile_aux("(101 102)", "(member 101 '(100 101 102))");
  test_compile_aux("#f", "(member 101 '((b a) (y x)))");
  test_compile_aux("(a c)", "(member 'a '(b a c))");
  test_compile_aux("((y x))", "(member '(y x) '((b a) (y x)))");
  }

static void test_case()
  {
  test_compile_aux("big", "(case (+ 7 5) [(1 2 3) 'small] [(10 11 12) 'big])");
  test_compile_aux("75", "(case (+ 7 3) [(10 11 12) 75])");
  test_compile_aux("small", "(case (- 7 5) [(1 2 3) 'small] [(10 11 12) 'big])");
  test_compile_aux("composite", "(case (* 2 3) [(2 3 5 7) 'prime][(1 4 6 8 9) 'composite])");
  test_compile_aux("#undefined", "(case (car '(c d)) [(a) 'a] [(b) 'b])");
  test_compile_aux("consonant", "(case (car '(c d)) [(a e i o u) 'vowel][(w y) 'semivowel][else 'consonant])");
  test_compile_aux("vowel", "(case (car '(e d)) [(a e i o u) 'vowel][(w y) 'semivowel][else 'consonant])");
  //test_compile_aux("backwards", "(case (list 'y 'x) [((a b) (x y)) 'forwards]  [((b a) (y x)) 'backwards])"); // this only works if case is rewritten with member instead of memv
  test_compile_aux("5", "(let ([x 3]) (case x [else 5]))");
  test_compile_aux("5", "(let ([x #\\a]) (case x [(#\\newline) 7] [else 5]))");
  test_compile_aux("5", "(let ([x #\\a]) (case x [(#\\newline) (char->fixnum x)] [else 5]))");
  test_compile_aux("10", "(let ([x #\\newline]) (case x [(#\\newline) (char->fixnum x)] [else 5]))");
  test_compile_aux("#t", "(let ([x #\\newline]) (eqv? x #\\newline))");
  test_compile_aux("(#\\newline)", "(let ([x #\\newline]) (memv x '(#\\newline)))");
  }

static void test_named_let()
  {
  test_compile_aux("((6 1 3) (-5 -2))", "(let loop ([numbers '(3 -2 1 6 -5)][nonneg '()][neg '()])(cond[(null? numbers) (list nonneg neg)][(>= (car numbers) 0) (loop(cdr numbers)(cons(car numbers) nonneg)neg)][(< (car numbers) 0)(loop(cdr numbers) nonneg (cons(car numbers) neg))]))");
  }

static void test_list_ops()
  {
  test_compile_aux("((a 1) (b 2) (c 3))", "(define e (list (list 'a 1) (list 'b 2) (list 'c 3)))");
  test_compile_aux("(a 1)", "(define e (list (list 'a 1) (list 'b 2) (list 'c 3))) (assq 'a e)");
  test_compile_aux("(b 2)", "(define e (list (list 'a 1) (list 'b 2) (list 'c 3))) (assq 'b e)");
  test_compile_aux("(c 3)", "(define e (list (list 'a 1) (list 'b 2) (list 'c 3))) (assq 'c e)");
  test_compile_aux("#f", "(define e (list (list 'a 1) (list 'b 2) (list 'c 3))) (assq 'd e)");
  test_compile_aux("((a))", "(assoc (list 'a) '(((a)) ((b)) ((c))))");
  test_compile_aux("(5 7)", "(assv 5 '((2 3) (5 7) (11 13)))");
  test_compile_aux("((a 1) (b 2) (c 3))", "(define e (list (list 'a 1) (list 'b 2) (list 'c 3))) (set! e '((a 1 ) ( b 2) (c 3)) ) e");
  test_compile_aux("(a 1)", "(define e '((a 1 ) ( b 2) (c 3)) )(assv 'a e)");
  test_compile_aux("#f", "(define e '((a 1 ) ( b 2) (c 3)) )(assv 'd e)");
  test_compile_aux("(a 1)", "(define e '((a 1 ) ( b 2) (c 3)) )(assq 'a e)");
  test_compile_aux("(a b)", "(define e '((a 1 ) ( b 2) (c 3)) )(memq 'a '(a b))");
  }

static void test_symbol_ops()
  {
  test_compile_aux("#t", "(symbol? 'foo)");
  test_compile_aux("#t", "(symbol? (car '(a b)))");
  test_compile_aux("#f", "(symbol? \"bar\")");
  test_compile_aux("#t", "(symbol? 'nil)");
  test_compile_aux("#f", "(symbol? '())");
  test_compile_aux("#f", "(symbol? #f)");
  test_compile_aux("abcdefghabcdefghabcdefghabcdefgh1", "(quote abcdefghabcdefghabcdefghabcdefgh1)");
  test_compile_aux("abcdefghabcdefghabcdefghabcdefgh2", "(quote abcdefghabcdefghabcdefghabcdefgh2)");
  test_compile_aux("\"sym\"", "(symbol->string 'sym)");
  test_compile_aux("\"thisisaverylongsymbol\"", "(symbol->string 'thisisaverylongsymbol)");
  test_compile_aux("((a 1) (b 2) (c 3))", "(define e '((a 1 ) ( b 2) (c 3)) )");
  test_compile_aux("(a 1)", "(define e '((a 1 ) ( b 2) (c 3)) ) (assv 'a e)");
  test_compile_aux("#f", "(define e '((a 1 ) ( b 2) (c 3)) ) (assv 'd e)");
  test_compile_aux("(a 1)", "(define e '((a 1 ) ( b 2) (c 3)) )(assq 'a e)");
  test_compile_aux("(a b)", "(define e '((a 1 ) ( b 2) (c 3)) )(memq 'a '(a b))");
  }

static void test_string_ops()
  {
  test_compile_aux("\"sym\"", "(string-copy \"sym\")");
  test_compile_aux("\"thisisaverylongstring\"", "(string-copy \"thisisaverylongstring\")");
  test_compile_aux("#undefined", "(string-copy '())");
  test_compile_aux("#f", "(let ([s \"str\"])  (eq? s (string-copy s)))");
  test_compile_aux("#f", "(let ([s \"str\"])  (eqv? s (string-copy s)))");
  test_compile_aux("#t", "(let ([s \"str\"])  (equal? s (string-copy s)))");

  test_compile_aux("\"abcdef\"", "(string-append \"abc\" \"def\")");
  test_compile_aux("\"abcddefg\"", "(string-append \"abcd\" \"defg\")");
  test_compile_aux("\"abcdefghijklmn\"", "(string-append \"abcdefg\" \"hijklmn\")");
  test_compile_aux("\"abcdefg1hijklmn2\"", "(string-append \"abcdefg1\" \"hijklmn2\")");
  test_compile_aux("\"\"", "(string-append)");
  test_compile_aux("\"Jan\"", "(string-append \"Jan\")");
  test_compile_aux("\"abcdef\"", "(string-append \"abc\" \"def\")");
  test_compile_aux("\"abcddefg\"", "(string-append \"abcd\" \"defg\")");
  test_compile_aux("\"abcdefghijklmn\"", "(string-append \"abcdefg\" \"hijklmn\")");
  test_compile_aux("\"abcdefg1hijklmn2\"", "(string-append \"abcdefg1\" \"hijklmn2\")");
  test_compile_aux("\"abcdefghijklm\"", "(string-append \"abc\" \"def\" \"ghijklm\")");

  test_compile_aux("193485963", "(string-hash \"abc\")");
  test_compile_aux("193450027", "(string-hash \"ABC\")");
  test_compile_aux("177670", "(string-hash \"a\")");
  test_compile_aux("5863207", "(string-hash \"aa\")");
  test_compile_aux("193485928", "(string-hash \"aaa\")");
  test_compile_aux("177638", "(string-hash \"A\")");
  test_compile_aux("5862119", "(string-hash \"AA\")");
  test_compile_aux("193449992", "(string-hash \"AAA\")");
  test_compile_aux("177674", "(string-hash (symbol->string 'e))");
  test_compile_aux("177687", "(string-hash (symbol->string 'r))");
  test_compile_aux("5863207", "(string-hash \"aa\")");
  test_compile_aux("5862119", "(string-hash \"AA\")");
  test_compile_aux("177684", "(string-hash (symbol->string 'o))");
  test_compile_aux("193450027", "(string-hash \"ABC\")");
  test_compile_aux("177638", "(string-hash \"A\")");
  test_compile_aux("177691", "(string-hash (symbol->string 'v))");
  test_compile_aux("193485963", "(string-hash \"abc\")");
  test_compile_aux("177673", "(string-hash (symbol->string 'd))");
  test_compile_aux("177675", "(string-hash (symbol->string 'f))");
  test_compile_aux("177677", "(string-hash (symbol->string 'h))");
  test_compile_aux("177688", "(string-hash (symbol->string 's))");
  test_compile_aux("177689", "(string-hash (symbol->string 't))");
  test_compile_aux("177682", "(string-hash (symbol->string 'm))");
  test_compile_aux("177678", "(string-hash (symbol->string 'i))");
  test_compile_aux("177681", "(string-hash (symbol->string 'l))");
  test_compile_aux("5863210", "(string-hash (symbol->string 'ad))");
  test_compile_aux("177690", "(string-hash (symbol->string 'u))");
  test_compile_aux("177695", "(string-hash (symbol->string 'z))");
  test_compile_aux("5863214", "(string-hash (symbol->string 'ah))");
  test_compile_aux("177694", "(string-hash (symbol->string 'y))");
  test_compile_aux("177679", "(string-hash (symbol->string 'j))");
  test_compile_aux("177680", "(string-hash (symbol->string 'k))");
  test_compile_aux("177676", "(string-hash (symbol->string 'g))");
  test_compile_aux("177693", "(string-hash (symbol->string 'x))");
  test_compile_aux("177692", "(string-hash (symbol->string 'w))");
  test_compile_aux("177685", "(string-hash (symbol->string 'p))");
  test_compile_aux("5863212", "(string-hash (symbol->string 'af))");
  test_compile_aux("177686", "(string-hash (symbol->string 'q))");
  test_compile_aux("177670", "(string-hash \"a\")");
  test_compile_aux("193449992", "(string-hash \"AAA\")");
  test_compile_aux("193485928", "(string-hash \"aaa\")");
  test_compile_aux("177683", "(string-hash (symbol->string 'n))");
  test_compile_aux("6952521827504", "(string-hash (symbol->string 'Martin))");

  test_compile_aux("\"zzzzzzzzzzzzzzzzzzzz\"", "(let ([s \"thisismylongerstring\"]) (string-fill! s #\\z) s)");

  test_compile_aux("#f", "(string=? \"abc\" \"def\")");
  test_compile_aux("#t", "(string=? \"abc\" \"abc\")");
  test_compile_aux("#t", "(string<? \"abc\" \"def\")");
  test_compile_aux("#f", "(string<? \"def\" \"abc\")");
  test_compile_aux("#t", "(string<? \"ababc\" \"abdef\")");
  test_compile_aux("#f", "(string<? \"abdef\" \"ababc\")");
  test_compile_aux("#t", "(string<? \"abc\" \"abcd\")");
  test_compile_aux("#f", "(string<? \"abcd\" \"abc\")");
  test_compile_aux("#f", "(string<? \"abc\" \"abc\")");
  test_compile_aux("#f", "(string>? \"abc\" \"def\")");
  test_compile_aux("#t", "(string>? \"def\" \"abc\")");
  test_compile_aux("#f", "(string>? \"ababc\" \"abdef\")");
  test_compile_aux("#t", "(string>? \"abdef\" \"ababc\")");
  test_compile_aux("#f", "(string>? \"abc\" \"abcd\")");
  test_compile_aux("#t", "(string>? \"abcd\" \"abc\")");
  test_compile_aux("#f", "(string>? \"abc\" \"abc\")");
  test_compile_aux("#t", "(string<=? \"abc\" \"def\")");
  test_compile_aux("#f", "(string<=? \"def\" \"abc\")");
  test_compile_aux("#t", "(string<=? \"ababc\" \"abdef\")");
  test_compile_aux("#f", "(string<=? \"abdef\" \"ababc\")");
  test_compile_aux("#t", "(string<=? \"abc\" \"abcd\")");
  test_compile_aux("#f", "(string<=? \"abcd\" \"abc\")");
  test_compile_aux("#t", "(string<=? \"abc\" \"abc\")");
  test_compile_aux("#f", "(string>=? \"abc\" \"def\")");
  test_compile_aux("#t", "(string>=? \"def\" \"abc\")");
  test_compile_aux("#f", "(string>=? \"ababc\" \"abdef\")");
  test_compile_aux("#t", "(string>=? \"abdef\" \"ababc\")");
  test_compile_aux("#f", "(string>=? \"abc\" \"abcd\")");
  test_compile_aux("#t", "(string>=? \"abcd\" \"abc\")");
  test_compile_aux("#t", "(string>=? \"abc\" \"abc\")");

  test_compile_aux("#f", "(string-ci=? \"abc\" \"def\")");
  test_compile_aux("#t", "(string-ci=? \"abc\" \"abc\")");
  test_compile_aux("#t", "(string-ci<? \"abc\" \"def\")");
  test_compile_aux("#f", "(string-ci<? \"def\" \"abc\")");
  test_compile_aux("#t", "(string-ci<? \"ababc\" \"abdef\")");
  test_compile_aux("#f", "(string-ci<? \"abdef\" \"ababc\")");
  test_compile_aux("#t", "(string-ci<? \"abc\" \"abcd\")");
  test_compile_aux("#f", "(string-ci<? \"abcd\" \"abc\")");
  test_compile_aux("#f", "(string-ci<? \"abc\" \"abc\")");
  test_compile_aux("#f", "(string-ci>? \"abc\" \"def\")");
  test_compile_aux("#t", "(string-ci>? \"def\" \"abc\")");
  test_compile_aux("#f", "(string-ci>? \"ababc\" \"abdef\")");
  test_compile_aux("#t", "(string-ci>? \"abdef\" \"ababc\")");
  test_compile_aux("#f", "(string-ci>? \"abc\" \"abcd\")");
  test_compile_aux("#t", "(string-ci>? \"abcd\" \"abc\")");
  test_compile_aux("#f", "(string-ci>? \"abc\" \"abc\")");
  test_compile_aux("#t", "(string-ci<=? \"abc\" \"def\")");
  test_compile_aux("#f", "(string-ci<=? \"def\" \"abc\")");
  test_compile_aux("#t", "(string-ci<=? \"ababc\" \"abdef\")");
  test_compile_aux("#f", "(string-ci<=? \"abdef\" \"ababc\")");
  test_compile_aux("#t", "(string-ci<=? \"abc\" \"abcd\")");
  test_compile_aux("#f", "(string-ci<=? \"abcd\" \"abc\")");
  test_compile_aux("#t", "(string-ci<=? \"abc\" \"abc\")");
  test_compile_aux("#f", "(string-ci>=? \"abc\" \"def\")");
  test_compile_aux("#t", "(string-ci>=? \"def\" \"abc\")");
  test_compile_aux("#f", "(string-ci>=? \"ababc\" \"abdef\")");
  test_compile_aux("#t", "(string-ci>=? \"abdef\" \"ababc\")");
  test_compile_aux("#f", "(string-ci>=? \"abc\" \"abcd\")");
  test_compile_aux("#t", "(string-ci>=? \"abcd\" \"abc\")");
  test_compile_aux("#t", "(string-ci>=? \"abc\" \"abc\")");

  test_compile_aux("#f", "(string-ci=? \"aBc\" \"dEF\")");
  test_compile_aux("#t", "(string-ci=? \"aBc\" \"ABC\")");
  test_compile_aux("#t", "(string-ci<? \"aBc\" \"DEF\")");
  test_compile_aux("#f", "(string-ci<? \"dEf\" \"ABc\")");
  test_compile_aux("#t", "(string-ci<? \"aBAbc\" \"ABDEf\")");
  test_compile_aux("#f", "(string-ci<? \"aBDef\" \"abAbc\")");
  test_compile_aux("#t", "(string-ci<? \"aBC\" \"aBCd\")");
  test_compile_aux("#f", "(string-ci<? \"ABcd\" \"ABc\")");
  test_compile_aux("#f", "(string-ci<? \"aBC\" \"aBC\")");
  test_compile_aux("#f", "(string-ci>? \"ABC\" \"deF\")");
  test_compile_aux("#t", "(string-ci>? \"dEf\" \"abC\")");
  test_compile_aux("#f", "(string-ci>? \"aBAbC\" \"abdEF\")");
  test_compile_aux("#t", "(string-ci>? \"abDeF\" \"aBAbc\")");
  test_compile_aux("#f", "(string-ci>? \"ABc\" \"abCD\")");
  test_compile_aux("#t", "(string-ci>? \"abCD\" \"Abc\")");
  test_compile_aux("#f", "(string-ci>? \"abC\" \"aBc\")");
  test_compile_aux("#t", "(string-ci<=? \"abC\" \"DeF\")");
  test_compile_aux("#f", "(string-ci<=? \"deF\" \"AbC\")");
  test_compile_aux("#t", "(string-ci<=? \"aBABc\" \"abdEF\")");
  test_compile_aux("#f", "(string-ci<=? \"aBDEf\" \"ABAbc\")");
  test_compile_aux("#t", "(string-ci<=? \"aBC\" \"aBCd\")");
  test_compile_aux("#f", "(string-ci<=? \"abCd\" \"Abc\")");
  test_compile_aux("#t", "(string-ci<=? \"aBc\" \"aBc\")");
  test_compile_aux("#f", "(string-ci>=? \"aBC\" \"dEf\")");
  test_compile_aux("#t", "(string-ci>=? \"DEf\" \"abC\")");
  test_compile_aux("#f", "(string-ci>=? \"ABabc\" \"abDEF\")");
  test_compile_aux("#t", "(string-ci>=? \"ABdef\" \"aBABc\")");
  test_compile_aux("#f", "(string-ci>=? \"abC\" \"aBcd\")");
  test_compile_aux("#t", "(string-ci>=? \"abCD\" \"aBc\")");
  test_compile_aux("#t", "(string-ci>=? \"aBC\" \"ABc\")");

  test_compile_aux("\"cdefgh\"", "(substring \"abcdefghijklm\" 2 8)");
  test_compile_aux("\"c\"", "(substring \"abcdefghijklm\" 2 3)");
  test_compile_aux("\"cdefghijklm\"", "(substring \"abcdefghijklm\" 2 13)");
  test_compile_aux("\"cdefghijklm\"", "(substring \"abcdefghijklm\" 2 14)");
  test_compile_aux("\"cdefghijklm\"", "(substring \"abcdefghijklm\" 2 15)");
  test_compile_aux("\"cdefghijklm\"", "(substring \"abcdefghijklm\" 2 16)");
  test_compile_aux("\"cdefghijklm\"", "(substring \"abcdefghijklm\" 2 35)");
  }

static void test_current_seconds()
  {
  test_compile_aux("#t", "(>= (current-seconds) 0)");
  test_compile_aux("#t", "(>= (current-milliseconds) 0)");
  }

static void test_is_list()
  {
  test_compile_aux("#t", "(list? '(1 2 3))");
  test_compile_aux("#t", "(list? '())");
  test_compile_aux("#f", "(list? (cons 1 2))");
  test_compile_aux("#t", "(list? (cons 1 (cons 2 (cons 3 ()))))");
  test_compile_aux("#f", "(list? (cons 1 (cons 2 (cons 3 4))))");
  test_compile_aux("#t", "(list? (list 1 2 3 4 5))");
  test_compile_aux("#f", "(list? (vector 1 2 3 4 5))");
  test_compile_aux("#f", "(list? \"a string\")");
  }

static void test_min_max()
  {
  test_compile_aux("1", "(max 1)");
  test_compile_aux("2", "(min 2)");
  test_compile_aux("1.500000", "(max 1.5)");
  test_compile_aux("2.200000", "(min 2.2)");
  test_compile_aux("2.000000", "(max 1.0 2.0)");
  test_compile_aux("7.200000", "(max 1.0 2.0 -0.4 7.2 2.3 7.1 -11.0)");
  test_compile_aux("2.000000", "(max 1 2.0)");
  test_compile_aux("2.000000", "(max 1.0 2)");
  test_compile_aux("2", "(max 1 2)");
  test_compile_aux("7", "(max 1 2 7 -3 -11 6 3)");
  test_compile_aux("1.000000", "(min 1.0 2.0)");
  test_compile_aux("-11.000000", "(min 1.0 2.0 -0.4 7.2 2.3 7.1 -11.0)");
  test_compile_aux("1.000000", "(min 1 2.0)");
  test_compile_aux("1.000000", "(min 1.0 2)");
  test_compile_aux("1", "(min 1 2)");
  test_compile_aux("-11", "(min 1 2 7 -3 -11 6 3)");
  test_compile_aux("1024", "(arithmetic-shift 1 10)");
  test_compile_aux("31", "(arithmetic-shift 255 -3)");
  }

static void test_override()
  {
  test_compile_aux("(3 6)", "(define + (lambda (x y) (list y x))) (+ 6 3)");
  test_compile_aux("(8 3)", "(define + (lambda (x y) (list y x))) (define add3 (lambda (x) (+ 3 x))) (add3 8)");
  test_compile_aux("9", "(define old-+ +) (define + (lambda (x y) (list y x))) (set! + old-+) (+ 6 3)");
  }

static void test_apply()
  {
  test_compile_aux("0", "(apply + ())");
  test_compile_aux("7", "(apply + (list 3 4))");
  test_compile_aux("10", "(apply + (list 1 2 3 4))");
  test_compile_aux("10", "(apply + 1 2 (list 3 4))");
  test_compile_aux("36", "(apply + (list 1 2 3 4 5 6 7 8))");
  test_compile_aux("45", "(apply + (list 1 2 3 4 5 6 7 8 9))");
  test_compile_aux("55", "(apply + (list 1 2 3 4 5 6 7 8 9 10))");
  test_compile_aux("55", "(apply + 1 2 3 4 5 6 7 8 9 (list 10))");
  test_compile_aux("(#(1 2 3 4 5 6 7 8))", "(cons(apply vector '(1 2 3 4 5 6 7 8)) '())");
  test_compile_aux("(#(1 2 3 4 5 6 7 8))", "(cons(apply vector 1 '(2 3 4 5 6 7 8)) '())");
  test_compile_aux("(#(1 2 3 4 5 6 7 8))", "(cons(apply vector 1 2 '(3 4 5 6 7 8)) '())");
  test_compile_aux("(#(1 2 3 4 5 6 7 8))", "(cons(apply vector 1 2 3 '(4 5 6 7 8)) '())");
  test_compile_aux("(#(1 2 3 4 5 6 7 8))", "(cons(apply vector 1 2 3 4 '(5 6 7 8)) '())");
  test_compile_aux("(#(1 2 3 4 5 6 7 8))", "(cons(apply vector 1 2 3 4 5 '(6 7 8)) '())");
  test_compile_aux("(#(1 2 3 4 5 6 7 8))", "(cons(apply vector 1 2 3 4 5 6 '(7 8)) '())");
  test_compile_aux("(#(1 2 3 4 5 6 7 8))", "(cons(apply vector 1 2 3 4 5 6 7 '(8)) '())");
  test_compile_aux("(#(1 2 3 4 5 6 7 8))", "(cons(apply vector 1 2 3 4 5 6 7 8 ()) '())");

  test_compile_aux("13", "(let([f(lambda() 12)])( + (apply f '()) 1))");
  test_compile_aux("26", "(let([f(lambda(x) ( + x 12))]) ( + (apply f 13 '()) 1))");
  test_compile_aux("26", "(let([f(lambda(x) ( + x 12))]) ( + (apply f(cons 13 '())) 1))");
  test_compile_aux("27", "(let([f(lambda(x y z) ( + x(* y z)))])( + (apply f 12 '(7 2)) 1))");
  test_compile_aux("12", "(let([f(lambda() 12)])(apply f '()))");
  test_compile_aux("25", "(let([f(lambda(x) ( + x 12))])(apply f 13 '()))");
  test_compile_aux("25", "(let([f(lambda(x) ( + x 12))])(apply f(cons 13 '())))");
  test_compile_aux("26", "(let([f(lambda(x y z) ( + x(* y z)))])(apply f 12 '(7 2)))");
  test_compile_aux("#(1 2 3 4 5 6 7 8)", "(apply vector '(1 2 3 4 5 6 7 8))");
  test_compile_aux("#(1 2 3 4 5 6 7 8)", "(apply vector 1 '(2 3 4 5 6 7 8))");
  test_compile_aux("#(1 2 3 4 5 6 7 8)", "(apply vector 1 2 '(3 4 5 6 7 8)) ");
  test_compile_aux("#(1 2 3 4 5 6 7 8)", "(apply vector 1 2 3 '(4 5 6 7 8)) ");
  test_compile_aux("#(1 2 3 4 5 6 7 8)", "(apply vector 1 2 3 4 '(5 6 7 8)) ");
  test_compile_aux("#(1 2 3 4 5 6 7 8)", "(apply vector 1 2 3 4 5 '(6 7 8)) ");
  test_compile_aux("#(1 2 3 4 5 6 7 8)", "(apply vector 1 2 3 4 5 6 '(7 8)) ");
  test_compile_aux("#(1 2 3 4 5 6 7 8)", "(apply vector 1 2 3 4 5 6 7 '(8)) ");
  test_compile_aux("#(1 2 3 4 5 6 7 8)", "(apply vector 1 2 3 4 5 6 7 8 ())");
  test_compile_aux("1800", "(define compose (lambda(f g)(lambda args(f(apply g args))))) (define twice (lambda (x) (* 2 x))) ((compose twice *) 12 75)");
  }

static schemerlicht_fixnum seventeen(schemerlicht_context* ctxt)
  {
  return 17;
  }

static void test_foreign_1()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_external_function ext = schemerlicht_external_function_init(ctxt, "seventeen", cast(SCHEMERLICHT_FUNPTR(), &seventeen), schemerlicht_foreign_fixnum);
  schemerlicht_register_external_function(ctxt, &ext);

  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(foreign-call seventeen)");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_quasiquote_conversion(ctxt, &prog);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  schemerlicht_alpha_conversion(ctxt, &prog);
  schemerlicht_vector quotes = schemerlicht_quote_collection(ctxt, &prog);
  schemerlicht_quote_conversion(ctxt, &prog, &quotes);
  schemerlicht_quote_collection_destroy(ctxt, &quotes);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);
  schemerlicht_continuation_passing_style(ctxt, &prog);
  schemerlicht_lambda_to_let_conversion(ctxt, &prog);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_free_variable_analysis(ctxt, &prog);
  schemerlicht_closure_conversion(ctxt, &prog);
#if 0
  schemerlicht_string dumped = schemerlicht_dump(ctxt, &prog);
  printf("%s\n", dumped.string_ptr);
  schemerlicht_string_destroy(ctxt, &dumped);
#endif
  schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);  
  schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program);
  schemerlicht_string s = schemerlicht_object_to_string(ctxt, res, 0);

  TEST_EQ_STRING("17", s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);
  schemerlicht_compiled_program_destroy(ctxt, &compiled_program);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_foreign_aux(const char* expected, const char* script, const char* name, SCHEMERLICHT_FUNPTR(address), schemerlicht_foreign_return_type ret_type)
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_external_function ext = schemerlicht_external_function_init(ctxt, name, address, ret_type);
  schemerlicht_register_external_function(ctxt, &ext);

  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_quasiquote_conversion(ctxt, &prog);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  schemerlicht_alpha_conversion(ctxt, &prog);
  schemerlicht_vector quotes = schemerlicht_quote_collection(ctxt, &prog);
  schemerlicht_quote_conversion(ctxt, &prog, &quotes);
  schemerlicht_quote_collection_destroy(ctxt, &quotes);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);
  schemerlicht_continuation_passing_style(ctxt, &prog);
  schemerlicht_lambda_to_let_conversion(ctxt, &prog);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_free_variable_analysis(ctxt, &prog);
  schemerlicht_closure_conversion(ctxt, &prog);
#if 0
  schemerlicht_string dumped = schemerlicht_dump(ctxt, &prog);
  printf("%s\n", dumped.string_ptr);
  schemerlicht_string_destroy(ctxt, &dumped);
#endif
  schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);
  schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program);
  schemerlicht_string s = schemerlicht_object_to_string(ctxt, res, 0);

  TEST_EQ_STRING(expected, s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);
  schemerlicht_compiled_program_destroy(ctxt, &compiled_program);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static schemerlicht_flonum mathpi(schemerlicht_context* ctxt)
  {
  return 3.1415926535897;
  }

static void dosomething(schemerlicht_context* ctxt)
  {
  }

static const char* printHelloWorld(schemerlicht_context* ctxt)
  {
  return "Hello World!";
  }

static schemerlicht_object createCustomObject(schemerlicht_context* ctxt)
  {
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_true;
  return obj;
  }

static schemerlicht_fixnum addone(schemerlicht_context* ctxt, schemerlicht_object* obj)
  {
  return obj->value.fx + 1;
  }

static schemerlicht_flonum addonef(schemerlicht_context* ctxt, schemerlicht_object* obj)
  {
  return obj->value.fl + 1.0;
  }

static schemerlicht_fixnum getStringLength(schemerlicht_context* ctxt, schemerlicht_object* obj)
  {
  return cast(schemerlicht_fixnum, strlen(obj->value.s.string_ptr));
  }

static schemerlicht_flonum add_three(schemerlicht_context* ctxt, schemerlicht_object* a, schemerlicht_object* b, schemerlicht_object* c)
  {
  return a->value.fx + b->value.fx + c->value.fl;
  }

static void test_foreign()
  {
  test_foreign_aux("17", "(foreign-call seventeen)", "seventeen", cast(SCHEMERLICHT_FUNPTR(), &seventeen), schemerlicht_foreign_fixnum);
  test_foreign_aux("3.141593", "(foreign-call mathpi)", "mathpi", cast(SCHEMERLICHT_FUNPTR(), &mathpi), schemerlicht_foreign_flonum);
  test_foreign_aux("\"Hello World!\"", "(foreign-call printHelloWorld)", "printHelloWorld", cast(SCHEMERLICHT_FUNPTR(), &printHelloWorld), schemerlicht_foreign_char_pointer);
  test_foreign_aux("#t", "(foreign-call createCustomObject)", "createCustomObject", cast(SCHEMERLICHT_FUNPTR(), &createCustomObject), schemerlicht_foreign_object);
  test_foreign_aux("8", "(foreign-call addone 7)", "addone", cast(SCHEMERLICHT_FUNPTR(), &addone), schemerlicht_foreign_fixnum);
  test_foreign_aux("4.140000", "(foreign-call addonef 3.14)", "addonef", cast(SCHEMERLICHT_FUNPTR(), &addonef), schemerlicht_foreign_flonum);
  test_foreign_aux("5", "(foreign-call get-string-length \"abcde\")", "get-string-length", cast(SCHEMERLICHT_FUNPTR(), &getStringLength), schemerlicht_foreign_fixnum);
  test_foreign_aux("16.200000", "(foreign-call add_three 7 9 0.2)", "add_three", cast(SCHEMERLICHT_FUNPTR(), &add_three), schemerlicht_foreign_flonum);
  }

static void test_r5rs_funs()
  {
  
  test_compile_aux("(a b c . d)", "(append '(a b) '(c . d))"); 
  test_compile_aux("(1 2 3 4)", "(append (list 1 2) (list 3 4))");
  test_compile_aux("(1 2 3 4)", "(append '(1 2) '(3 4))");
  test_compile_aux("(1 2 3 4 5 6 7 8)", "(append '(1 2) '(3 4) '(5 6) '(7 8))");
  test_compile_aux("(1 2 3 4 5 6 . 35)", "(append '(1 2) '(3 4) '(5 6) 35)");
  test_compile_aux("(1 2 3 4 5 6 . cons)", "(append '(1 2) '(3 4) '(5 6) 'cons)");
  test_compile_aux("()", "(append '())");
  test_compile_aux("()", "(append '() '() '())");
  test_compile_aux("(1 2 3)", "(append '(1 2 3))");  
  
  test_compile_aux("#t", "(exact? 3)");
  test_compile_aux("#f", "(exact? 3.14)");
  test_compile_aux("#f", "(inexact? 3)");
  test_compile_aux("#t", "(inexact? 3.14)");
  test_compile_aux("#t", "(number? 3)");
  test_compile_aux("#t", "(number? 3.14)");
  test_compile_aux("#f", "(number? 'a)");
  test_compile_aux("#t", "(rational? 5)");
  test_compile_aux("#t", "(rational? 5.2)");
  test_compile_aux("#f", "(rational? (/ 5.0 0.0))");
  test_compile_aux("#f", "(rational? \"test\")");
  test_compile_aux("#t", "(positive? 5)");
  test_compile_aux("#f", "(negative? 5)");
  test_compile_aux("#t", "(positive? 5.2)");
  test_compile_aux("#f", "(negative? 5.2)");
  test_compile_aux("#f", "(positive? -5)");
  test_compile_aux("#t", "(negative? -5)");
  test_compile_aux("#f", "(positive? -5.2)");
  test_compile_aux("#t", "(negative? -5.2)");
  test_compile_aux("#f", "(even? -5)");
  test_compile_aux("#t", "(even? -6)");
  test_compile_aux("#t", "(odd? -5)");
  test_compile_aux("#f", "(odd? -6)");
  test_compile_aux("#t", "(complex? 3)");
  test_compile_aux("#t", "(complex? 3.14)");
  test_compile_aux("#f", "(complex? #\\a)");
  test_compile_aux("#t", "(real? 3)");
  test_compile_aux("#t", "(real? 3.14)");
  test_compile_aux("#f", "(real? #\\a)");
  test_compile_aux("#t", "(integer? 3)");
  test_compile_aux("#t", "(integer? 3.00)");
  test_compile_aux("#f", "(integer? 3.14)");
  test_compile_aux("#f", "(integer? #\\a)");

  test_compile_aux("3", "(abs 3)");
  test_compile_aux("3.200000", "(abs 3.2)");
  test_compile_aux("3", "(abs -3)");
  test_compile_aux("3.200000", "(abs -3.2)");
  test_compile_aux("0", "(abs 0)");
  test_compile_aux("0.000000", "(abs 0.0)");

  test_compile_aux("3", "(quotient 10 3)");
  test_compile_aux("-3", "(quotient -10 3)");
  test_compile_aux("3", "(quotient 10.0 3)");
  test_compile_aux("-3", "(quotient -10.0 3)");
  test_compile_aux("3", "(quotient 10.0 3.0)");
  test_compile_aux("-3", "(quotient -10.0 3.0)");
  test_compile_aux("3", "(quotient 10 3.0)");
  test_compile_aux("-3", "(quotient -10 3.0)");
  test_compile_aux("1", "(remainder 10 3)");
  test_compile_aux("-1", "(remainder -10 3)");
  test_compile_aux("1", "(remainder 10.0 3)");
  test_compile_aux("-1", "(remainder -10.0 3)");
  test_compile_aux("1", "(remainder 10.0 3.0)");
  test_compile_aux("-1", "(remainder -10.0 3.0)");
  test_compile_aux("1", "(remainder 10 3.0)");
  test_compile_aux("-1", "(remainder -10 3.0)");
  test_compile_aux("1", "(modulo 10 3)");
  test_compile_aux("2", "(modulo -10 3)");
  test_compile_aux("1", "(modulo 10.0 3)");
  test_compile_aux("-1", "(modulo -10.0 -3)");
  test_compile_aux("1", "(modulo 10.0 3.0)");
  test_compile_aux("2", "(modulo -10.0 3.0)");
  test_compile_aux("-2", "(modulo 10 -3.0)");
  test_compile_aux("2", "(modulo -10 3.0)");

  test_compile_aux("4", "(gcd 32 -36)");
  test_compile_aux("4", "(gcd 32.0 -36)");
  test_compile_aux("4", "(gcd 32 -36)");
  test_compile_aux("4", "(gcd 32.0 -36)");
  test_compile_aux("0", "(gcd)");
  test_compile_aux("2", "(gcd 2)");
  test_compile_aux("4", "(gcd 32.0 -36 16 -4)");
  test_compile_aux("2", "(gcd 32.0 -36 16 -4 6)");
  test_compile_aux("288", "(lcm 32 -36)");
  test_compile_aux("288", "(lcm 32.0 -36)");
  test_compile_aux("1", "(lcm)");
  test_compile_aux("5.000000", "(lcm 5.0)");

  test_compile_aux("3", "(ceiling 3)");
  test_compile_aux("3.000000", "(ceiling 3.0)");
  test_compile_aux("4.000000", "(ceiling 3.0000000001)");
  test_compile_aux("3.000000", "(ceiling 2.9)");
  test_compile_aux("-3", "(ceiling -3)");
  test_compile_aux("-3.000000", "(ceiling -3.1)");
  test_compile_aux("-2.000000", "(ceiling -2.9)");
  test_compile_aux("-2.000000", "(ceiling -2.5)");
  test_compile_aux("-2.000000", "(ceiling -2.1)");
  test_compile_aux("3", "(floor 3)");
  test_compile_aux("3.000000", "(floor 3.0)");
  test_compile_aux("3.000000", "(floor 3.0000000001)");
  test_compile_aux("2.000000", "(floor 2.9)");
  test_compile_aux("-3", "(floor -3)");
  test_compile_aux("-4.000000", "(floor -3.1)");
  test_compile_aux("-3.000000", "(floor -2.9)");
  test_compile_aux("-3.000000", "(floor -2.5)");
  test_compile_aux("-3.000000", "(floor -2.1)");
  test_compile_aux("8.000000", "(round 8.0)");
  test_compile_aux("8.000000", "(round 8.4)");
  test_compile_aux("8.000000", "(round 7.5)");
  test_compile_aux("8", "(round 8)");
  test_compile_aux("8.000000", "(truncate 8.0)");
  test_compile_aux("8.000000", "(truncate 8.4)");
  test_compile_aux("7.000000", "(truncate 7.5)");
  test_compile_aux("8", "(truncate 8)");

  test_compile_aux("3.000000", "(sqrt 9)");
  test_compile_aux("1.414214", "(sqrt 2.0)");
  test_compile_aux("0.841471", "(sin 1.0)");
  test_compile_aux("0.841471", "(sin 1)");
  test_compile_aux("0.909297", "(sin 2.0)");
  test_compile_aux("0.909297", "(sin 2)");
  test_compile_aux("0.540302", "(cos 1.0)");
  test_compile_aux("0.540302", "(cos 1)");
  test_compile_aux("1.557408", "(tan 1.0)");
  test_compile_aux("1.557408", "(tan 1)");
  test_compile_aux("1.570796", "(asin 1.0)");
  test_compile_aux("1.570796", "(asin 1)");
  test_compile_aux("1.047198", "(acos 0.5)");
  test_compile_aux("0.000000", "(acos 1)");
  test_compile_aux("0.785398", "(atan 1.0)");
  test_compile_aux("0.785398", "(atan 1)");
  test_compile_aux("2.079442", "(log 8.0)");
  test_compile_aux("2.079442", "(log 8)");

  test_compile_aux("8", "(expt 2 3)");
  test_compile_aux("1", "(expt 2 0)");
  test_compile_aux("1", "(expt 0 0)");
  test_compile_aux("1024", "(expt 2 10)");
  test_compile_aux("16", "(expt 4 2)");
  test_compile_aux("16.000000", "(expt 4.0 2.0)");
  test_compile_aux("2.000000", "(expt 4.0 0.5)");
  test_compile_aux("1.414214", "(expt 2.0 0.5)");
  test_compile_aux("343", "(expt 7 3)");
  test_compile_aux("907.492700", "(expt 7 3.5)");

  test_compile_aux("3", "(inexact->exact 3.14)");
  test_compile_aux("3", "(inexact->exact 3)");
  test_compile_aux("3.140000", "(exact->inexact 3.14)");
  test_compile_aux("3.000000", "(exact->inexact 3)");
  test_compile_aux("#t", "(flonum? (exact->inexact 3))");
  test_compile_aux("#t", "(inexact? (exact->inexact 3))");

  test_compile_aux("\"3.14159843513843606e+00\"", "(number->string 3.1415984351384361)");
  test_compile_aux("\"25\"", "(number->string 25)");
  test_compile_aux("\"31\"", "(number->string 25 8)");
  test_compile_aux("\"1a\"", "(number->string 26 16)");
  test_compile_aux("\"+nan.0\"", "(number->string (/ 0.0 0.0))");
  test_compile_aux("\"+inf.0\"", "(number->string (/ 1.0 0.0))");
  test_compile_aux("\"-inf.0\"", "(number->string (/ -1.0 0.0))");
  test_compile_aux("\"111001000\"", "(number->string 456 2)");

  test_compile_aux("#f", "(string->number \"\")");
  test_compile_aux("25", "(string->number \"25\")");
  test_compile_aux("21", "(string->number \"25\" 8)");
  test_compile_aux("255", "(string->number \"ff\" 16)");
  test_compile_aux("#f", "(string->number \"3.14asdf\")");
  test_compile_aux("3.140000", "(string->number \"3.14\")");

  test_compile_aux("#t", "(finite? 3.14)");
  test_compile_aux("#t", "(finite? 2)");
  test_compile_aux("#f", "(finite? (/ 1.0 0.0))");
  test_compile_aux("#f", "(finite? (/ 0.0 0.0))");
  test_compile_aux("#f", "(nan? 3.14)");
  test_compile_aux("#f", "(nan? 2)");
  test_compile_aux("#f", "(nan? (/ 1.0 0.0))");
  test_compile_aux("#t", "(nan? (/ 0.0 0.0))");
  test_compile_aux("#f", "(inf? 3.14)");
  test_compile_aux("#f", "(inf? 2)");
  test_compile_aux("#t", "(inf? (/ 1.0 0.0))");
  test_compile_aux("#f", "(inf? (/ 0.0 0.0))");
  test_compile_aux("#undefined", "(inf? (/ 0 0))");

  test_compile_aux("(5 4 3 2 1)", "(reverse (list 1 2 3 4 5))");
  test_compile_aux("()", "(reverse (list))");
  test_compile_aux("(4 5 6)", "(list-tail (list 1 2 3 4 5 6) 3)");
  test_compile_aux("(4 5 6)", "(list-tail '(1 2 3 4 5 6) 3)");
  test_compile_aux("(4 5 6)", "(list-tail '(1 2 3 4 5 6) 3)");
  test_compile_aux("4", "(list-ref '(1 2 3 4 5 6) 3)");
  test_compile_aux("4", "(list-ref (list 1 2 3 4 5 6) 3)");
  test_compile_aux("c", "(list-ref '(a b c d) 2)");
  test_compile_aux("c", "(list-ref '(a b c d) (inexact->exact (round 1.8)))");
  }

static void test_chars()
  {
  test_compile_aux("#t", "(char=? #\\A #\\A)");
  test_compile_aux("#f", "(char=? #\\A #\\a)");
  test_compile_aux("#t", "(char<? #\\a #\\b)");
  test_compile_aux("#f", "(char<? #\\b #\\a)");
  test_compile_aux("#f", "(char<? #\\a #\\a)");
  test_compile_aux("#f", "(char>? #\\a #\\b)");
  test_compile_aux("#t", "(char>? #\\b #\\a)");
  test_compile_aux("#f", "(char>? #\\a #\\a)");
  test_compile_aux("#t", "(char<=? #\\a #\\b)");
  test_compile_aux("#f", "(char<=? #\\b #\\a)");
  test_compile_aux("#t", "(char<=? #\\a #\\a)");
  test_compile_aux("#f", "(char>=? #\\a #\\b)");
  test_compile_aux("#t", "(char>=? #\\b #\\a)");
  test_compile_aux("#t", "(char>=? #\\a #\\a)");
  test_compile_aux("#\\a", "(char-downcase #\\A)");
  test_compile_aux("#\\a", "(char-downcase #\\a)");
  test_compile_aux("#\\z", "(char-downcase #\\Z)");
  test_compile_aux("#\\z", "(char-downcase #\\z)");
  test_compile_aux("#\\A", "(char-upcase #\\A)");
  test_compile_aux("#\\A", "(char-upcase #\\a)");
  test_compile_aux("#\\Z", "(char-upcase #\\Z)");
  test_compile_aux("#\\Z", "(char-upcase #\\z)");
  test_compile_aux("#t", "(char-ci=? #\\A #\\A)");
  test_compile_aux("#t", "(char-ci=? #\\A #\\a)");
  test_compile_aux("#f", "(char-ci=? #\\b #\\a)");
  test_compile_aux("#f", "(char-ci=? #\\B #\\a)");
  test_compile_aux("#f", "(char-ci=? #\\b #\\A)");
  test_compile_aux("#f", "(char-ci=? #\\B #\\A)");
  test_compile_aux("#f", "(char-ci<? #\\A #\\A)");
  test_compile_aux("#f", "(char-ci<? #\\A #\\a)");
  test_compile_aux("#f", "(char-ci<? #\\b #\\a)");
  test_compile_aux("#f", "(char-ci<? #\\B #\\a)");
  test_compile_aux("#f", "(char-ci<? #\\b #\\A)");
  test_compile_aux("#f", "(char-ci<? #\\B #\\A)");
  test_compile_aux("#f", "(char-ci>? #\\A #\\A)");
  test_compile_aux("#f", "(char-ci>? #\\A #\\a)");
  test_compile_aux("#t", "(char-ci>? #\\b #\\a)");
  test_compile_aux("#t", "(char-ci>? #\\B #\\a)");
  test_compile_aux("#t", "(char-ci>? #\\b #\\A)");
  test_compile_aux("#t", "(char-ci>? #\\B #\\A)");
  test_compile_aux("#t", "(char-ci<=? #\\A #\\A)");
  test_compile_aux("#t", "(char-ci<=? #\\A #\\a)");
  test_compile_aux("#f", "(char-ci<=? #\\b #\\a)");
  test_compile_aux("#f", "(char-ci<=? #\\B #\\a)");
  test_compile_aux("#f", "(char-ci<=? #\\b #\\A)");
  test_compile_aux("#f", "(char-ci<=? #\\B #\\A)");
  test_compile_aux("#t", "(char-ci>=? #\\A #\\A)");
  test_compile_aux("#t", "(char-ci>=? #\\A #\\a)");
  test_compile_aux("#t", "(char-ci>=? #\\b #\\a)");
  test_compile_aux("#t", "(char-ci>=? #\\B #\\a)");
  test_compile_aux("#t", "(char-ci>=? #\\b #\\A)");
  test_compile_aux("#t", "(char-ci>=? #\\B #\\A)");
  test_compile_aux("#t", "(char-upper-case? #\\B)");
  test_compile_aux("#f", "(char-upper-case? #\\b)");
  test_compile_aux("#f", "(char-upper-case? #\\3)");
  test_compile_aux("#f", "(char-lower-case? #\\B)");
  test_compile_aux("#t", "(char-lower-case? #\\b)");
  test_compile_aux("#f", "(char-lower-case? #\\3)");
  test_compile_aux("#t", "(char-alphabetic? #\\B)");
  test_compile_aux("#t", "(char-alphabetic? #\\b)");
  test_compile_aux("#f", "(char-alphabetic? #\\3)");
  test_compile_aux("#f", "(char-numeric? #\\B)");
  test_compile_aux("#f", "(char-numeric? #\\b)");
  test_compile_aux("#t", "(char-numeric? #\\3)");
  test_compile_aux("#f", "(char-whitespace? #\\B)");
  test_compile_aux("#f", "(char-whitespace? #\\b)");
  test_compile_aux("#f", "(char-whitespace? #\\3)");
  test_compile_aux("#t", "(char-whitespace? #\\space)");
  test_compile_aux("#t", "(char-whitespace? #\\newline)");
  test_compile_aux("#t", "(char-whitespace? #\\tab)");
  }

static void test_list_conversions()
  {
  test_compile_aux("(#\\a #\\b #\\c)", "(string->list \"abc\")");
  test_compile_aux("\"abc\"", "(list->string (list #\\a #\\b #\\c))");
  test_compile_aux("\"\"", "(list->string '())");
  test_compile_aux("(\"abc\" 3 2.100000)", "(vector->list (vector \"abc\" 3 2.1))");
  test_compile_aux("#(#\\a #\\b #\\c)", "(list->vector (list #\\a #\\b #\\c))");
  test_compile_aux("#()", "(list->vector '())");
  test_compile_aux("(dah dah didah)", "(vector->list '#(dah dah didah))");
  test_compile_aux("#(dididit dah)", "(list->vector '(dididit dah))");
  }

static void test_compile_aux_r5rs(const char* expected_value, const char* script)
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_compile_callcc(ctxt);
  schemerlicht_compile_r5rs(ctxt);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_quasiquote_conversion(ctxt, &prog);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  schemerlicht_alpha_conversion(ctxt, &prog);
  schemerlicht_vector quotes = schemerlicht_quote_collection(ctxt, &prog);
  schemerlicht_quote_conversion(ctxt, &prog, &quotes);
  schemerlicht_quote_collection_destroy(ctxt, &quotes);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);
  schemerlicht_continuation_passing_style(ctxt, &prog);
  schemerlicht_lambda_to_let_conversion(ctxt, &prog);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_free_variable_analysis(ctxt, &prog);
  schemerlicht_closure_conversion(ctxt, &prog);
#if 0
  schemerlicht_string dumped = schemerlicht_dump(ctxt, &prog);
  printf("%s\n", dumped.string_ptr);
  schemerlicht_string_destroy(ctxt, &dumped);
#endif
  schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);
  schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program);
  schemerlicht_print_any_error(ctxt);
  schemerlicht_string s = schemerlicht_object_to_string(ctxt, res, 0);

#ifdef SCHEMERLICHT_DEBUG
  if (print_gc_time)
    printf("Time spent in GC: %lldms\n", ctxt->time_spent_gc * 1000 / CLOCKS_PER_SEC);
#endif

  TEST_EQ_STRING(expected_value, s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);
  schemerlicht_compiled_program_destroy(ctxt, &compiled_program);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_compile_aux_r5rs_heap(const char* expected_value, const char* script, schemerlicht_memsize heap)
  {
  schemerlicht_context* ctxt = schemerlicht_open(heap);
  schemerlicht_compile_callcc(ctxt);
  schemerlicht_compile_r5rs(ctxt);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_quasiquote_conversion(ctxt, &prog);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  schemerlicht_alpha_conversion(ctxt, &prog);
  schemerlicht_vector quotes = schemerlicht_quote_collection(ctxt, &prog);
  schemerlicht_quote_conversion(ctxt, &prog, &quotes);
  schemerlicht_quote_collection_destroy(ctxt, &quotes);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);
  schemerlicht_continuation_passing_style(ctxt, &prog);
  schemerlicht_lambda_to_let_conversion(ctxt, &prog);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_free_variable_analysis(ctxt, &prog);
  schemerlicht_closure_conversion(ctxt, &prog);
#if 0
  schemerlicht_string dumped = schemerlicht_dump(ctxt, &prog);
  printf("%s\n", dumped.string_ptr);
  schemerlicht_string_destroy(ctxt, &dumped);
#endif
  schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);  
#if 0
  schemerlicht_string debug_string;
  schemerlicht_string_init(ctxt, &debug_string, "");
  schemerlicht_object* res = schemerlicht_run_debug(ctxt, &debug_string, func);
  schemerlicht_string_destroy(ctxt, &debug_string);
#else
  schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program);
#endif
  schemerlicht_string s = schemerlicht_object_to_string(ctxt, res, 0);

#ifdef SCHEMERLICHT_DEBUG
  if (print_gc_time)
    printf("Time spent in GC: %lldms\n", ctxt->time_spent_gc * 1000 / CLOCKS_PER_SEC);
#endif

  TEST_EQ_STRING(expected_value, s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);
  schemerlicht_compiled_program_destroy(ctxt, &compiled_program);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_control_ops()
  {
  test_compile_aux_r5rs("(b e h)", "(map cadr '((a b) (d e) (g h)))");
  test_compile_aux_r5rs("(1 4 27 256 3125)", "(map (lambda (n) (expt n n)) '(1 2 3 4 5))");
  test_compile_aux_r5rs("(5 7 9)", "(map + '(1 2 3) '(4 5 6))");
  test_compile_aux_r5rs("(1 2)", "(let ([count 0]) (map (lambda (ignored) (set! count (+ count 1)) count) '(a b)))");

  test_compile_aux_r5rs("#(0 1 4 9 16)", "(let ([v (make-vector 5)]) (for-each (lambda (i) (vector-set! v i (* i i))) '(0 1 2 3 4)) v)");
  test_compile_aux_r5rs("3", "3 #;(let ([v (make-vector 5)]) (for-each (lambda (i) (vector-set! v i (* i i))) '(0 1 2 3 4)) v)");
  test_compile_aux_r5rs("#(0 1 4 9 16)", "#|\n"
    "Here is a multiline comment, similar\n"
    "to /* and */ in c/c++\n"
    "|#\n"
    "(let ([v (make-vector 5)])\n"
    "     (for-each #; here is comment\n"
    "        (lambda (i) (vector-set! v i (* i i))) ; here is also comment\n"
    "        '(0 1 2 3 4))\n"
    "     v) #; blabla\n");

  test_compile_aux("1", "(slot-ref (cons 1 2) 0)");
  test_compile_aux("2", "(slot-ref (cons 1 2) 1)");
  test_compile_aux_r5rs("<promise>", "(define p (make-promise(lambda() (+ 5 6))))");
  test_compile_aux_r5rs("#t", "(define p (make-promise(lambda() (+ 5 6)))) (promise? p)");
  test_compile_aux_r5rs("#t", "(define p (make-promise(lambda() (+ 5 6)))) (closure? (slot-ref p 0))");
  test_compile_aux_r5rs("11", "(define p (make-promise(lambda() (+ 5 6)))) (force p)");
  test_compile_aux_r5rs("11", "(define p (make-promise(lambda() (+ 5 6)))) (force p) (force p)");
  test_compile_aux_r5rs("13", "(define p (make-promise(lambda() (+ 5 6)))) (force p) (force p) (force 13)");
  test_compile_aux_r5rs("<promise>", "(define p (delay (* 3 4)))");
  test_compile_aux_r5rs("12", "(define p (delay (* 3 4))) (force p)");
  }

static void test_quasiquote()
  {
  test_compile_aux("(list 3 4)", "`(list ,(+ 1 2) 4)");
  test_compile_aux("(list a (quote a))", "(let ((name 'a)) `(list ,name ',name))");
  test_compile_aux_r5rs("(a 3 4 5 6 b)", "`(a ,(+ 1 2) ,@(map abs '(4 -5 6)) b)");
  test_compile_aux("((foo 7) . cons)", "`((foo ,(- 10 3)) ,@(cdr '(c)) . ,(car '(cons)))");
  test_compile_aux_r5rs("#(10 5 2.000000 4.000000 3.000000 8)", "`#(10 5 ,(sqrt 4) ,@(map sqrt '(16 9)) 8)");
  test_compile_aux("(a (quasiquote (b c)))", "`(a `(b c))");
  test_compile_aux("(a (quasiquote (b c (quote d))))", "`(a `(b c 'd))");
  test_compile_aux("(a (quasiquote (b c (quasiquote d))))", "`(a `(b c `d))");
  test_compile_aux("(a (quasiquote (b c (quasiquote (quasiquote d)))))", "`(a `(b c ``d))");
  test_compile_aux("(a (quasiquote (b c (quasiquote (quote d)))))", "`(a `(b c `'d))");
  test_compile_aux("(quote (quote (quote d)))", "`'''d");
  test_compile_aux("(a (quasiquote (b (unquote (+ 1 2)))))", "`(a `(b ,(+ 1 2)))");
  test_compile_aux("(a (quasiquote (b (unquote (+ 1 2)) (unquote (foo 4 d)) e)) f)", "`(a `(b ,(+ 1 2) ,(foo ,(+ 1 3) d) e) f)");
  test_compile_aux("(a (quasiquote (b (unquote x) (unquote (quote y)) d)) e)", "(let ((name1 'x) (name2 'y)) `(a `(b ,,name1 ,',name2 d) e))");
  }

static void test_port()
  {
  test_compile_aux("#t", "(define default-port (make-port #f \"stdout\" 1 (make-string 1024) 0 1024)) (port? default-port)");
  test_compile_aux("#f", "(port? (vector #f \"stdout\" 1 (make-string 1024) 0 1024))");
  test_compile_aux("#f", "(output-port? (vector #f \"stdout\" 1 (make-string 1024) 0 1024))");
  test_compile_aux("#f", "(input-port? (vector #f \"stdout\" 1 (make-string 1024) 0 1024))");
  test_compile_aux("#t", "(define default-port (make-port #f \"stdout\" 1 (make-string 1024) 0 1024)) (output-port? default-port)");
  test_compile_aux("#f", "(define default-port (make-port #f \"stdout\" 1 (make-string 1024) 0 1024)) (input-port? default-port)");
  test_compile_aux("#t", "(define default-port (make-port #t \"stdin\" 1 (make-string 1024) 0 1024)) (input-port? default-port)");
  test_compile_aux("#f", "(define default-port (make-port #t \"stdin\" 1 (make-string 1024) 0 1024)) (output-port? default-port)");
  test_compile_aux("#<void>", "(define default-port (make-port #f \"stdout\" 1 (make-string 1024) 0 1024)) (%write-char #\\f default-port) (%write-char #\\o default-port) (%write-char #\\o default-port) (%write-char #\\013 default-port) (%flush-output-port default-port)");
  test_compile_aux("<port>: \"out.txt\"", "(define my_file (open-output-file \"out.txt\"))");
  test_compile_aux("#<void>", "(define my_file (open-output-file \"out.txt\")) (close-output-port my_file)");
  test_compile_aux("#\\f", "(define my_file (open-output-file \"out.txt\"))"
    "(%write-char #\\f my_file) (%write-char #\\o my_file) (%write-char #\\o my_file)"
    "(%flush-output-port my_file) (close-output-port my_file)"
    "(define read_file (open-input-file \"out.txt\"))"
    "(%read-char read_file)");
  test_compile_aux("#\\o", "(define my_file (open-output-file \"out.txt\"))"
    "(%write-char #\\f my_file) (%write-char #\\o my_file) (%write-char #\\o my_file)"
    "(%flush-output-port my_file) (close-output-port my_file)"
    "(define read_file (open-input-file \"out.txt\"))"
    "(%read-char read_file) (%read-char read_file)");
  test_compile_aux("#\\o", "(define my_file (open-output-file \"out.txt\"))"
    "(%write-char #\\f my_file) (%write-char #\\o my_file) (%write-char #\\o my_file)"
    "(%flush-output-port my_file) (close-output-port my_file)"
    "(define read_file (open-input-file \"out.txt\"))"
    "(%read-char read_file) (%read-char read_file) (%read-char read_file)");
  test_compile_aux("#eof", "(define my_file (open-output-file \"out.txt\"))"
    "(%write-char #\\f my_file) (%write-char #\\o my_file) (%write-char #\\o my_file)"
    "(%flush-output-port my_file) (close-output-port my_file)"
    "(define read_file (open-input-file \"out.txt\"))"
    "(%read-char read_file) (%read-char read_file) (%read-char read_file) (%read-char read_file)");
  test_compile_aux("#\\f", "(define my_file (open-output-file \"out.txt\"))"
    "(%write-char #\\f my_file) (%write-char #\\o my_file) (%write-char #\\o my_file)"
    "(%flush-output-port my_file) (close-output-port my_file)"
    "(define read_file (open-input-file \"out.txt\"))"
    "(%peek-char read_file)(%peek-char read_file)(%peek-char read_file)(%peek-char read_file)(%peek-char read_file)(%peek-char read_file)");
  test_compile_aux("#f", "(eof-object? 3)");
  test_compile_aux("#t", "(define my_file (open-output-file \"out.txt\"))"
    "(%write-char #\\f my_file) (%write-char #\\o my_file) (%write-char #\\o my_file)"
    "(%flush-output-port my_file) (close-output-port my_file)"
    "(define read_file (open-input-file \"out.txt\"))"
    "(%read-char read_file) (%read-char read_file) (%read-char read_file) (eof-object? (%read-char read_file))");
  test_compile_aux("#f", "(define read_file (open-input-file \"out.txt\")) (%char-ready? read_file)");
  test_compile_aux("#t", "(define read_file (open-input-file \"out.txt\")) (%read-char read_file) (%char-ready? read_file)");
  test_compile_aux("#t", "(define read_file (open-input-file \"out.txt\")) (%peek-char read_file) (%char-ready? read_file)");
  test_compile_aux("#f", "(define read_file (open-input-file \"out.txt\")) (%read-char read_file) (%read-char read_file) (%read-char read_file) (%char-ready? read_file)");
  test_compile_aux("#t", "(define read_file (open-input-file \"out.txt\")) (%read-char read_file) (%read-char read_file) (%read-char read_file) (%read-char read_file) (%char-ready? read_file)");

  test_compile_aux("#<void>", "(define wf (open-output-file \"out2.txt\"))"
    "(%write (+ 7 9) wf)"
    "(%flush-output-port wf)"
    "(close-output-port wf)"
  );
  test_compile_aux("#<void>", "(define wf (open-output-file \"out2.txt\"))"
    "(slot-set! wf 5 1)"
    "(%write (+ 7 9) wf)"
    "(%flush-output-port wf)"
    "(close-output-port wf)"
  );
  test_compile_aux("16", "(define rf (open-input-file \"out2.txt\"))"
    "(define res (%read rf))"
    "(close-input-port rf)"
    "res");
  }

static void test_read()
  {
  test_compile_aux("21", "(define rf (open-input-file \"data/read_test.txt\"))"
    "(define res (%read rf))"
    "(close-input-port rf)"
    "res");
  test_compile_aux("22.330000", "(define rf (open-input-file \"data/read_test.txt\"))"
    "(%read rf)"
    "(define res (%read rf))"
    "(close-input-port rf)"
    "res");
  test_compile_aux("(16)", "(define rf (open-input-file \"data/read_test.txt\"))"
    "(%read rf)(%read rf)"
    "(define res (%read rf))"
    "(close-input-port rf)"
    "res");
  test_compile_aux("(3.140000)", "(define rf (open-input-file \"data/read_test.txt\"))"
    "(%read rf)(%read rf)(%read rf)"
    "(define res (%read rf))"
    "(close-input-port rf)"
    "res");
  test_compile_aux("#t", "(define rf (open-input-file \"data/read_test.txt\"))"
    "(%read rf)(%read rf)(%read rf)(%read rf)"
    "(define res (%read rf))"
    "(close-input-port rf)"
    "res");
  test_compile_aux("#f", "(define rf (open-input-file \"data/read_test.txt\"))"
    "(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)"
    "(define res (%read rf))"
    "(close-input-port rf)"
    "res");
  test_compile_aux("()", "(define rf (open-input-file \"data/read_test.txt\"))"
    "(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)"
    "(define res (%read rf))"
    "(close-input-port rf)"
    "res");
  test_compile_aux("alpha", "(define rf (open-input-file \"data/read_test.txt\"))"
    "(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)"
    "(define res (%read rf))"
    "(close-input-port rf)"
    "res");
  test_compile_aux("(1 2 3 4 5)", "(define rf (open-input-file \"data/read_test.txt\"))"
    "(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)"
    "(define res (%read rf))"
    "(close-input-port rf)"
    "res");
  test_compile_aux("(((1 2) 3 4) 78)", "(define rf (open-input-file \"data/read_test.txt\"))"
    "(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)"
    "(define res (%read rf))"
    "(close-input-port rf)"
    "res");
  test_compile_aux("#(1 2 3)", "(define rf (open-input-file \"data/read_test.txt\"))"
    "(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)"
    "(define res (%read rf))"
    "(close-input-port rf)"
    "res");
  test_compile_aux("\"foo\"", "(define rf (open-input-file \"data/read_test.txt\"))"
    "(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)"
    "(define res (%read rf))"
    "(close-input-port rf)"
    "res");
  test_compile_aux("(quote bar)", "(define rf (open-input-file \"data/read_test.txt\"))"
    "(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)"
    "(define res (%read rf))"
    "(close-input-port rf)"
    "res");
  test_compile_aux("#eof", "(define rf (open-input-file \"data/read_test.txt\"))"
    "(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)(%read rf)"
    "(define res (%read rf))"
    "(close-input-port rf)"
    "res");
  }

static void test_write()
  {  
  test_compile_aux("\"50#\\\\space3.141590\\\"FOO\\\"\n\\\"FOO\\\\\\\"BAR\\\"\"", "(define open-input-string (lambda (s) (make-port #t \"input-string\" -2 s 0 (string-length s))))\n"
    "(define open-output-string (lambda() (make-port #f \"output-string\" -2 (make-string 256) 0 256)))\n"
    "(define get-output-string(lambda(s) (substring(slot-ref s 3) 0 (slot-ref s 4))))\n"
    "(define ostr (open-output-string))\n"
    "(%write 50 ostr) (%write #\\032 ostr)\n"
    "(%write 3.14159 ostr)\n"
    "(%write \"FOO\" ostr)\n"
    "(%write-char #\\newline ostr)\n"
    "(%write \"FOO\\\"BAR\" ostr)\n"
    "(get-output-string ostr)\n"
  );
  }

static void test_display()
  {
  test_compile_aux("\"50 3.141590\nFOOBAR\"", "(define open-input-string (lambda (s) (make-port #t \"input-string\" -2 s 0 (string-length s))))\n"
    "(define open-output-string (lambda() (make-port #f \"output-string\" -2 (make-string 256) 0 256)))\n"
    "(define get-output-string(lambda(s) (substring(slot-ref s 3) 0 (slot-ref s 4))))\n"
    "(define ostr (open-output-string))\n"
    "(%display 50 ostr) (%display #\\032 ostr)\n"
    "(%display 3.14159 ostr)\n"
    "(%write-char #\\newline ostr)\n"
    "(%display \"FOOBAR\" ostr)\n"
    "(get-output-string ostr)\n"
  );
  }

static void test_getenv()
  {
  test_compile_aux("#f", "(getenv \"SchemerlichtTest\")");
  test_compile_aux("#f", "(putenv \"SchemerlichtTest\")");
  test_compile_aux("#t", "(putenv \"SchemerlichtTest\" \"DummyValue\")");
  test_compile_aux("\"DummyValue\"", "(getenv \"SchemerlichtTest\")");
  test_compile_aux("#t", "(putenv \"SchemerlichtTest\" \"Dummy\")");
  test_compile_aux("\"Dummy\"", "(getenv \"SchemerlichtTest\")");

  test_compile_aux("#f", "(file-exists? \"ThisFileDoesProbablyNotExist\")");
  test_compile_aux("#t", "(file-exists? \"out.txt\")"); // this file was created in one of the previous tests
  }

static void test_eval()
  {
  test_compile_aux("<environment>", "(interaction-environment)");
  test_compile_aux("#t", "(environment? (interaction-environment))");
  test_compile_aux("#f", "(environment? \"Hello\")");
  test_compile_aux("#f", "(environment? 5)");
  test_compile_aux("#t", "(environment? (null-environment))");
  test_compile_aux("#t", "(environment? (scheme-environment))");
  test_compile_aux("21", "(eval '(* 7 3))");
  test_compile_aux("(7 . 3)", "(eval '(cons 7 3))");
  test_compile_aux("#(1 2 3 4 5 6 7)", "(eval '(vector 1 2 3 4 5 6 7))");
  test_compile_aux("#(1 2 3 4 5 6 7 8 9 10 11 12 13 14 15)", "(eval '(vector 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15))");
  test_compile_aux("#((1 2 3) #(3.000000 40.000000) 3 \"abc\" 5 6 7)", "(eval '(vector (list 1 2 3) (vector 3.0 40.0) 3 \"abc\" 5 6 7))");

  test_compile_aux("21", "(define e (null-environment)) (eval '(define x (* 7 3)) e)");
  test_compile_aux("#unassigned", "(define e (null-environment)) (eval '(define x (* 7 3)) e) x");
  test_compile_aux("#unassigned", "(define e (null-environment)) (eval '(define x (* 7 3)) e) (eval 'x)");
  test_compile_aux("21", "(define e (null-environment)) (eval '(define x (* 7 3)) e) (eval 'x e)");
  test_compile_aux("21", "(define e (interaction-environment)) (eval '(define x (* 7 3)) e) x");
  test_compile_aux("21", "(eval '(define x (* 7 3))) x");
  }

static void test_load()
  {
  test_compile_aux("((1 5) (2 6) (3 7) (4 8))", "(load \"data/load_test.scm\") (zip (list 1 2 3 4) (list 5 6 7 8))");
  }

static void test_macros()
  {
  test_compile_aux("8", "(define-macro (eight) '(+ 3 5))  (eight)");
  test_compile_aux("7", "(define-macro (sum x y) `(+ ,x ,y))  (sum 3 4)");
  test_compile_aux("10", "(define-macro (when2 test . args) `(if ,test (begin ,@args) #f))  (when2 (> 5 2) 7 8 9 10)");

  test_compile_aux("19", "(defmacro mac1 (a b) `(+ ,a (* ,b 3))) (mac1 4 5)");
  test_compile_aux("8", "(defmacro eight2 () `8) (eight2)");
  }

static void test_calcc_extended()
  {
  test_compile_aux_r5rs("1", "(values 1)");
  test_compile_aux_r5rs("((multiple . values) 1 2)", "(values 1 2)");
  test_compile_aux_r5rs("((multiple . values) 1 2 3)", "(values 1 2 3)");
  test_compile_aux_r5rs("3", "(call-with-values (lambda () (values 1 2)) +)");
  test_compile_aux_r5rs("5", "(call-with-values (lambda () (values 4 5)) (lambda (a b) b))");
  test_compile_aux_r5rs("-1", "(call-with-values * -)");
  test_compile_aux_r5rs("(2 1)", "(define lst '()) (define before (lambda () (set! lst (cons 1 lst)))) (define thunk (lambda () (set! lst (cons 2 lst))))  (define after (lambda () (set! lst (cons 3 lst)))) (dynamic-wind before thunk after)");
  test_compile_aux_r5rs("(3 2 1)", "(define lst '()) (define before (lambda () (set! lst (cons 1 lst)))) (define thunk (lambda () (set! lst (cons 2 lst))))  (define after (lambda () (set! lst (cons 3 lst)))) (dynamic-wind before thunk after) lst");
  const char* script = "(let ((path '())\n"
    "     (c #f))\n"
    " (let ((add (lambda (s)\n"
    "             (set! path (cons s path)))))\n"
    "   (dynamic-wind\n"
    "     (lambda () (add 'connect))\n"
    "     (lambda ()\n"
    "       (add (call-with-current-continuation\n"
    "             (lambda (c0)\n"
    "               (set! c c0)\n"
    "               'talk1))))\n"
    "     (lambda () (add 'disconnect)))\n"
    "   (if (< (length path) 4)\n"
    "       (c 'talk2)\n"
    "       (reverse path))))\n";

  test_compile_aux_r5rs("(connect talk1 disconnect connect talk2 disconnect)", script);
  }

static void test_error()
  {
  test_compile_aux_r5rs("12", "(error \"test_error\") (+ 5 7)");
  }

static void test_curry()
  {
  test_compile_aux_r5rs("<closure>", "(curry list 1 2)");
  test_compile_aux_r5rs("(1 2 3)", "((curry list 1 2) 3)");
  test_compile_aux_r5rs("<closure>", "(define pair_with_one (curry cons 1))");
  test_compile_aux_r5rs("(1 . 2)", "(define pair_with_one (curry cons 1))(pair_with_one 2)");
  test_compile_aux_r5rs("(1)", "(define pair_with_one (curry cons 1))(pair_with_one '())");
  test_compile_aux_r5rs("(1 2 . 3)", "(define pair_with_one (curry cons 1))(pair_with_one (cons 2 3))");
  test_compile_aux_r5rs("<lambda>", "(define lam (lambda (x y z) (list x y z)))");
  test_compile_aux_r5rs("(1 2 3)", "(define lam (lambda (x y z) (list x y z)))(lam 1 2 3)");
  test_compile_aux_r5rs("<closure>", "(define lam (lambda (x y z) (list x y z)))(define foo (curry lam 1))");
  test_compile_aux_r5rs("(1 2 3)", "(define lam (lambda (x y z) (list x y z)))(define foo (curry lam 1))(foo 2 3)");
  test_compile_aux_r5rs("<closure>", "(define lam (lambda (x y z) (list x y z)))(define foo (curry lam 1))(define bar (curry lam 1 2))");
  test_compile_aux_r5rs("(1 2 4)", "(define lam (lambda (x y z) (list x y z)))(define foo (curry lam 1))(define bar (curry lam 1 2))(bar 4)");
  }

static void test_curry_2()
  {
  test_compile_aux_heap("((1 2) (3 4))", "\n"
    "(define map(lambda(proc lst1 . lsts)\n"
    "   (if (null? lsts)\n"
    "      (let loop([lst lst1])\n"
    "        (if (null? lst)\n"
    "          '()\n"
    "          (cons (proc (car lst)) (loop(cdr lst)))\n"
    "          ))\n"
    "      (let loop ([lsts(cons lst1 lsts)])\n"
    "        (let ([hds(let loop2([lsts lsts])\n"
    "          (if (null? lsts)\n"
    "            '()\n"
    "            (let([x(car lsts)])\n"
    "              (and (not (null? x))\n"
    "                (let([r(loop2(cdr lsts))])\n"
    "                  (and r(cons(car x) r))\n"
    "                  )))))])\n"
    "          (if hds"
    "          (cons(apply proc hds)\n"
    "            (loop(let loop3([lsts lsts])\n"
    "              (if (null? lsts)\n"
    "                '()\n"
    "                (cons(cdr(car lsts)) (loop3(cdr lsts)))\n"
    "                ))))\n"
    "            '()\n"
    "            ))))))\n"
    "(define (curry func . args) (lambda x (apply func (append args x))))\n"
    "(define (csv-map proc csvlst) (map (curry map proc) csvlst))\n"
    //"(define (csv-map proc csvlst) (map (lambda (lst) (map proc lst)) csvlst))\n"
    "(define r '((\"1\" \"2\") (\"3\" \"4\")))\n"
    "(csv-map string->number r)", 256*256);
  }

static void test_long_apply()
  {
  //test_compile_aux("210", "(apply + (list 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20))");
  //test_compile_aux("410", "(define lst (list 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20)) (apply + 200 lst)");
  
  //test_compile_aux_heap("210", "(define (oper lst) (apply + lst))  ( oper (list 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20)) ( oper (list 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20))", 90);
  
  //test_compile_aux_r5rs("(1.000000 1.414214 1.732051 2.000000 2.236068 2.449490 2.645751 2.828427 3.000000 3.162278 3.316625 3.464102 3.605551 3.741657 3.872983 4.000000 4.123106 4.242641 4.358899 4.472136)", "(define lst (list 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20)) (map sqrt lst)");  
  //test_compile_aux("987", "(define fib (lambda (n) (cond [(< n 2) 1]  [else (+ (fib (- n 2)) (fib(- n 1)))]))) (fib 15)");
  test_compile_aux_r5rs_heap("(1 4 9 16 25 36 49 64 81 100 121 144 169 196 225 256 289 324 361 400)", "(define lst (list 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20)) (map * lst lst)", 200);  
  
  //test_compile_aux_r5rs("11.958261", "(define (rms nums) (sqrt (/ (apply + (map * nums nums))(length nums)))) (rms (list 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20))");
  //TEST_EQ("(1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20)", run("(define dm (list 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20))"));
  //TEST_EQ("(1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20)", run("dm"));
  //TEST_EQ("11.9791", run("(rms dm)"));
  //TEST_EQ("(1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20)", run("dm"));
  //TEST_EQ("(0 1 2 3 4)", run("(iota 5)"));
  //TEST_EQ("2000", run("(define dm (iota 2000)) (length dm)"));
  //TEST_EQ("1999000", run("(apply + dm)"));
  //TEST_EQ("2000", run("(length dm)"));
  }

static void test_jaffer_bug_1()
  {
  test_compile_aux("(3)", "(apply (lambda x x) (list 3))");  
  test_compile_aux("(3 4)", "(apply (lambda x x) (list 3 4))");
  test_compile_aux("(3 4 5 6)", "(apply (lambda x x) (list 3 4 5 6))");
  
  test_compile_aux_r5rs_heap("#t", 
  "(define test\n"
    "  (lambda (expect fun . args)\n"
    "    (write (cons fun args))\n"
    "    (display \"  ==> \")\n"
    "    ((lambda (res)\n"
    "      (write res)\n"
    "      (newline)\n"
    "      (cond ((not (equal? expect res))\n"
    "	     (display \" BUT EXPECTED \")\n"
    "	     (write expect)\n"
    "	     (newline)\n"
    "	     #f)\n"
    "	    (else #t)))\n"
    "     (if (procedure? fun) (apply fun args) (car args)))))\n"
    "(test '(3 4 5 6) (lambda x x) 3 4 5 6)", 2048);  
  }

static void test_jaffer_bug_2()
  {  
  test_compile_aux("7", "(define test (lambda (fun . args) (apply fun args))) (test apply + (list 3 4))");   
  test_compile_aux_r5rs("#t",
    "(define test\n"
    "  (lambda (expect fun . args)\n"
    "    (write (cons fun args))\n"
    "    (display \"  ==> \")\n"
    "    ((lambda (res)\n"
    "      (write res)\n"
    "      (newline)\n"
    "      (cond ((not (equal? expect res))\n"
    "	     (display \" BUT EXPECTED \")\n"
    "	     (write expect)\n"
    "	     (newline)\n"
    "	     #f)\n"
    "	    (else #t)))\n"
    "     (if (procedure? fun) (apply fun args) (car args)))))\n"
    "(test 7 apply + (list 3 4))");
  }

static void test_jaffer_bug_3()
  {
  test_compile_aux("7", "(define test (lambda (fun . args) (apply fun args))) (test apply (lambda (a b) (+ a b)) (list 3 4))");  
  }

static void test_nested_define()
  {  
  test_compile_aux("<lambda>", "(define (a)(let ((var 3))(define b 7.0)(define (fun x) (define (nested y) (+ x y)) (let ((z 3)) (nested z)))))");
  test_compile_aux("<lambda>", "(define (a)(let ((var 3))(define b 7.0)(define (fun x) (define (nested y) (+ x y)) (let ((z 3)) (nested z)))(fun 2)))");
  
  test_compile_aux("<lambda>", "(define(a)(let((var 3))(define b 7.0)(define(fun x) (define(nested y) (+ x y)) (let((z 3)) (nested z)))))\n");
    
  
  test_compile_aux("<closure>", 
    "(define(test-inexact-printing)\n"
    "  (let((f0.0 (string->number \"0.0\"))\n"
    "    (f0.5 (string->number \"0.5\"))\n"
    "    (f1.0 (string->number \"1.0\"))\n"
    "    (f2.0 (string->number \"2.0\")))    \n"
    "(define log2\n"
    "(let((l2(log 2)))\n"
    "  (lambda(x) (/ (log x) l2))))\n"
    "    (define (float-print-test x)\n"
    "      (define (testit number)	\n"
    "	(= number (string->number (number->string number))))\n"
    "      (let ((eps (float-precision x))\n"
    "	    (all-ok? #t))\n"
    "	(do ((j -100 (+ j 1)))\n"
    "	    ((or (not all-ok?) (> j 100)) all-ok?)\n"
    "	  (let* ((xx (+ x (* j eps)))\n"
    "		 (ok? (testit xx)))\n"
    "	    (cond ((not ok?)\n"
    "		   (display \"Number readback failure for \")\n"
    "		   (display `(+ ,x (* ,j ,eps))) (newline)\n"
    "		   (display xx) (newline)\n"
    "		   (display (string->number (number->string xx))) (newline)\n"
    "		   (set! all-ok? #f))\n"
    "		  )))))\n"
    "))\n"
  );
  }

static void test_define_vs_internal_define()
  {  
  test_compile_aux("5", 
    "(define x 34)\n"
    "(define (foo) (define x 5) x)\n"
    "(foo)\n"
    );    
  test_compile_aux("34",
    "(define x 34)\n"
    "(define (foo) (define x 5) x)\n"
    "(foo)\n"
    "x"
  );
  }

static void test_quasiquote_comparison()
  {
  test_compile_aux("((foo 7) . cons)", "`((foo ,(- 10 3)) ,@(cdr '(c)) . ,(car '(cons)))");
  test_compile_aux("((foo 7) . cons)", "'((foo 7) . cons)");
  test_compile_aux("#t", "(equal? '((foo 7) . cons) `((foo ,(- 10 3)) ,@(cdr '(c)) . ,(car '(cons))))");
  }

static void test_string_list()
  {
  test_compile_aux("(#\\1 #\\\\ #\\\")", "(string->list \"1\\\\\\\"\")");
  test_compile_aux("\"1\\\\\\\"\"", "(list->string '(#\\1 #\\\\ #\\\"))");
  test_compile_aux("\"1\\\\\\\"\"", "(define test (lambda (fun . args) (apply fun args))) ( test list->string '(#\\1 #\\\\ #\\\"))");
  test_compile_aux("\"1\\\\\\\"\"", "\"1\\\\\\\"\"");
  }

static void test_compare_quote()
  {
  test_compile_aux("#t", "(equal? '(#t #f a () 9739 -3 . #((test) \"te \" \\\" st\" \"\" test #() b c)) '(#t #f a () 9739 -3 . #((test) \"te \" \\\" st\" \"\" test #() b c)))");
  test_compile_aux("#t", "(eq? '(#t #f a () 9739 -3 . #((test) \"te \" \\\" st\" \"\" test #() b c)) '(#t #f a () 9739 -3 . #((test) \"te \" \\\" st\" \"\" test #() b c)))");
  test_compile_aux("#t", "(eqv? '(#t #f a () 9739 -3 . #((test) \"te \" \\\" st\" \"\" test #() b c)) '(#t #f a () 9739 -3 . #((test) \"te \" \\\" st\" \"\" test #() b c)))");

  test_compile_aux("#t", 
    "(define wf (open-output-file \"write_test_obj.txt\"))\n"
    "(define write-test-obj\n"
    "'(#t #f a () 9739 -3 . #((test) \"te \\\" \\\" st\" \"\" test #() b c)))\n"
    "(%write write-test-obj wf)\n"
    "(%flush-output-port wf)\n"
    "(close-output-port wf)\n"
    "(define rf (open-input-file \"write_test_obj.txt\"))\n"
    "(define line (%read rf))\n"
    "(close-input-port rf)\n"
    "(equal? write-test-obj line)\n"
  );
  }

static void test_multi_float_print()
  {
  test_compile_aux_r5rs("(1.000000 10.000000 100.000000 0.100000 0.010000 0.001000)", "(map string->number '(\"1.0\" \"10.0\" \"100.0\" \"0.1\" \"0.01\" \"0.001\" ))");
  test_compile_aux_r5rs_heap("#t", 
    "    (define (float-print-test x)\n"
    "      (define (testit number)	\n"
    "	(= number (string->number (number->string number))))\n"
    "      (let ((eps 0.000001)\n"
    "	    (all-ok? #t))\n"
    "	(do ((j -100 (+ j 1)))\n"
    "	    ((or (not all-ok?) (> j 100)) all-ok?)\n"
    "	  (let* ((xx (+ x (* j eps)))\n"
    "		 (ok? (testit xx)))\n"
    "	    (cond ((not ok?)\n"
    "		   (display \"Number readback failure for \")\n"
    "		   (display `(+ ,x (* ,j ,eps))) (newline)\n"
    "		   (display xx) (newline)\n"
    "		   (display (string->number (number->string xx))) (newline)\n"
    "		   (set! all-ok? #f))\n"
    "		  )))))\n"
    "(define (mult-float-print-test x)\n"
    "      (let ((res #t))\n"
    "	(for-each\n"
    "	 (lambda (mult)\n"
    "	   (or (float-print-test (* mult x)) (set! res #f)))\n"
    "	 (map string->number\n"
    "	      '(\"1.0\" \"10.0\" \"100.0\" \n"
    "	    	\"0.1\" \"0.01\" \"0.001\" )))\n"
    "	res))\n"
    "(mult-float-print-test 0.0)", 2048*16);
  }

static void test_float_rw_range()
  {
  test_compile_aux("\"1.e-323\"", "(let* ((x -323) (estr (string-append \"1.e\" (number->string x)))) estr )");
  test_compile_aux("0.000000", "(let* ((x -323) (estr (string-append \"1.e\" (number->string x))) (num (string->number estr))) num )");
  test_compile_aux("\"9.88131291682493088e-324\"", "(let* ((x -323) (estr (string-append \"1.e\" (number->string x))) (num (string->number estr)) (str (number->string num))) str )");
  test_compile_aux("#t", "(let* ((x -323) (estr (string-append \"1.e\" (number->string x))) (num (string->number estr)) (str (number->string num))) (= (string->number str) num) )");

  test_compile_aux("\"1.e19\"", "(let* ((x 19) (estr (string-append \"1.e\" (number->string x)))) estr )");
  test_compile_aux("10000000000000000000.000000", "(let* ((x 19) (estr (string-append \"1.e\" (number->string x))) (num (string->number estr))) num )");
  test_compile_aux("\"1.00000000000000000e+19\"", "(let* ((x 19) (estr (string-append \"1.e\" (number->string x))) (num (string->number estr)) (str (number->string num))) str )");
  test_compile_aux("#t", "(let* ((x 19) (estr (string-append \"1.e\" (number->string x))) (num (string->number estr)) (str (number->string num))) (= (string->number str) num) )");
  }

static void test_multiple_lines()
  {
  test_compile_aux("3", "1 2 3");
  }

static void test_car_bug()
  {
  test_compile_aux("(0 1 2 3 4)", "(define lst list) (define (my-iota n) (do ((n n (- n 1)) (lst '() (cons (- n 1) lst))) ((zero? n) lst)))\n"    
    "(my-iota 5)"
  );

  test_compile_aux("(0 1 2 3 4)", "(define (my-iota n) (do ((n n (- n 1)) (list '() (cons (- n 1) list))) ((zero? n) list)))\n"
    "(my-iota 5)"
  );
      
  test_compile_aux("100", "(define (my-iota n) (do ((n n (- n 1)) (list '() (cons (- n 1) list))) ((zero? n) list)))\n"
    "(length (my-iota 100)) (length (my-iota 100)) (length (my-iota 100)) (length (my-iota 100)) (length (my-iota 100)) (length (my-iota 100))"
  );
  }

static void test_long_symbol()
  {
  test_compile_aux_heap("(#\\space #\\space #\\space #\\_ #\\space #\\space #\\space #\\_ #\\space #\\space #\\space #\\_ #\\newline #\\space #\\_ #\\/ #\\space #\\\\ #\\_ #\\/ #\\space #\\\\ #\\_ #\\/ #\\. #\\\\ #\\space #\\newline #\\/ #\\space #\\\\ #\\space #\\space #\\space #\\\\ #\\_ #\\space #\\. #\\space #\\space #\\/ #\\. #\\\\ #\\newline #\\\\ #\\space #\\space #\\space #\\\\ #\\space #\\/ #\\. #\\space #\\_ #\\/ #\\. #\\\\ #\\space #\\/ #\\newline #\\/ #\\space #\\\\ #\\_ #\\/ #\\. #\\space #\\_ #\\/ #\\space #\\\\ #\\_ #\\space #\\. #\\\\ #\\newline #\\\\ #\\space #\\/ #\\space #\\\\ #\\space #\\/ #\\space #\\space #\\_ #\\/ #\\space #\\\\ #\\_ #\\/ #\\newline #\\/ #\\space #\\space #\\_ #\\/ #\\. #\\\\ #\\space #\\/ #\\space #\\\\ #\\space #\\/ #\\space #\\\\ #\\newline #\\\\ #\\space #\\/ #\\space #\\\\ #\\space #\\/ #\\space #\\space #\\_ #\\/ #\\space #\\space #\\space #\\/ #\\newline #\\/ #\\space #\\\\ #\\space #\\/ #\\. #\\\\ #\\space #\\/ #\\. #\\\\ #\\_ #\\/ #\\space #\\\\ #\\newline #\\\\ #\\_ #\\/ #\\space #\\\\ #\\space #\\/ #\\. #\\space #\\_ #\\space #\\. #\\\\ #\\space #\\/ #\\newline #\\/ #\\space #\\\\ #\\_ #\\space #\\. #\\space #\\_ #\\/ #\\space #\\\\ #\\space #\\space #\\space #\\\\ #\\newline #\\\\ #\\_ #\\space #\\space #\\\\ #\\_ #\\/ #\\space #\\space #\\_ #\\/ #\\. #\\\\ #\\space #\\/ #\\newline #\\/ #\\space #\\space #\\_ #\\/ #\\space #\\space #\\space #\\/ #\\space #\\\\ #\\space #\\/ #\\space #\\\\ #\\newline #\\\\ #\\_ #\\space #\\space #\\\\ #\\space #\\/ #\\space #\\\\ #\\_ #\\space #\\. #\\\\ #\\_ #\\/ #\\newline #\\/ #\\space #\\\\ #\\_ #\\space #\\space #\\\\ #\\_ #\\space #\\space #\\\\ #\\_ #\\space #\\. #\\\\ #\\newline #\\\\ #\\_ #\\space #\\space #\\\\ #\\_ #\\/ #\\space #\\space #\\_ #\\/ #\\. #\\\\ #\\space #\\/ #\\newline #\\/ #\\space #\\\\ #\\_ #\\space #\\space #\\\\ #\\space #\\/ #\\. #\\\\ #\\space #\\space #\\. #\\\\ #\\newline #\\\\ #\\space #\\/ #\\. #\\\\ #\\_ #\\space #\\. #\\space #\\space #\\/ #\\. #\\\\ #\\space #\\/ #\\newline #\\/ #\\space #\\space #\\space #\\space #\\. #\\space #\\_ #\\/ #\\. #\\\\ #\\space #\\/ #\\space #\\\\ #\\newline #\\\\ #\\space #\\/ #\\. #\\\\ #\\_ #\\/ #\\. #\\\\ #\\_ #\\space #\\. #\\\\ #\\space #\\/ #\\newline #\\/ #\\space #\\\\ #\\_ #\\space #\\. #\\space #\\space #\\/ #\\space #\\space #\\_ #\\/ #\\space #\\\\ #\\newline #\\\\ #\\_ #\\space #\\space #\\\\ #\\_ #\\/ #\\. #\\\\ #\\_ #\\space #\\space #\\\\ #\\_ #\\/ #\\newline #\\/ #\\space #\\space #\\_ #\\/ #\\space #\\\\ #\\space #\\/ #\\space #\\\\ #\\_ #\\space #\\space #\\\\ #\\newline #\\\\ #\\_ #\\/ #\\space #\\space #\\_ #\\/ #\\. #\\\\ #\\_ #\\space #\\space #\\\\ #\\_ #\\/ #\\newline #\\/ #\\space #\\\\ #\\space #\\/ #\\space #\\space #\\_ #\\space #\\. #\\space #\\_ #\\space #\\space #\\\\ #\\newline #\\\\ #\\space #\\/ #\\space #\\\\ #\\_ #\\/ #\\. #\\space #\\_ #\\space #\\space #\\\\ #\\_ #\\/ #\\newline #\\/ #\\space #\\space #\\_ #\\space #\\space #\\\\ #\\space #\\space #\\space #\\\\ #\\_ #\\/ #\\space #\\\\ #\\newline #\\\\ #\\_ #\\/ #\\. #\\\\ #\\_ #\\space #\\. #\\\\ #\\_ #\\/ #\\space #\\space #\\_ #\\/ #\\newline #\\/ #\\space #\\\\ #\\space #\\space #\\. #\\space #\\_ #\\/ #\\space #\\space #\\space #\\/ #\\space #\\\\ #\\newline #\\\\ #\\space #\\/ #\\. #\\\\ #\\_ #\\/ #\\space #\\\\ #\\_ #\\/ #\\. #\\\\ #\\space #\\/ #\\newline #\\/ #\\space #\\\\ #\\_ #\\space #\\. #\\space #\\_ #\\/ #\\. #\\space #\\space #\\space #\\space #\\\\ #\\newline #\\\\ #\\space #\\space #\\space #\\space #\\space #\\space #\\. #\\space #\\space #\\/ #\\. #\\\\ #\\_ #\\/ #\\newline #\\/ #\\space #\\\\ #\\_ #\\/ #\\space #\\\\ #\\_ #\\/ #\\space #\\\\ #\\_ #\\space #\\. #\\\\ #\\newline #\\\\ #\\_ #\\/ #\\space #\\space #\\space #\\/ #\\space #\\\\ #\\_ #\\/ #\\. #\\space #\\space #\\/ #\\newline #\\/ #\\space #\\space #\\space #\\/ #\\space #\\space #\\_ #\\space #\\space #\\\\ #\\space #\\/ #\\space #\\\\ #\\newline #\\\\ #\\_ #\\/ #\\space #\\\\ #\\_ #\\/ #\\space #\\\\ #\\_ #\\/ #\\. #\\\\ #\\_ #\\/ #\\newline #\\/ #\\space #\\\\ #\\_ #\\/ #\\space #\\space #\\_ #\\/ #\\space #\\\\ #\\_ #\\space #\\. #\\\\ #\\newline #\\\\ #\\space #\\space #\\space #\\space #\\_ #\\/ #\\. #\\space #\\space #\\/ #\\. #\\space #\\_ #\\/ #\\newline #\\/ #\\space #\\\\ #\\space #\\/ #\\. #\\space #\\space #\\/ #\\space #\\\\ #\\_ #\\space #\\. #\\\\ #\\newline #\\\\ #\\_ #\\/ #\\. #\\space #\\_ #\\/ #\\. #\\\\ #\\_ #\\/ #\\. #\\\\ #\\space #\\/ #\\newline #\\/ #\\space #\\space #\\_ #\\space #\\. #\\\\ #\\_ #\\space #\\. #\\space #\\_ #\\space #\\. #\\\\ #\\newline #\\\\ #\\_ #\\/ #\\space #\\\\ #\\space #\\/ #\\space #\\\\ #\\_ #\\/ #\\space #\\\\ #\\_ #\\/ #\\newline)"
    , "(define long-symbol '\n"
    "  (#\\  #\\  #\\  #\\_ #\\  #\\  #\\  #\\_ #\\  #\\  #\\  #\\_ #\\newline\n"
    "   #\\  #\\_ #\\/ #\\  #\\\\ #\\_ #\\/ #\\  #\\\\ #\\_ #\\/ #\\. #\\\\ #\\  #\\newline\n"
    "   #\\/ #\\  #\\\\ #\\  #\\  #\\  #\\\\ #\\_ #\\  #\\. #\\  #\\  #\\/ #\\. #\\\\ #\\newline\n"
    "   #\\\\ #\\  #\\  #\\  #\\\\ #\\  #\\/ #\\. #\\  #\\_ #\\/ #\\. #\\\\ #\\  #\\/ #\\newline\n"
    "   #\\/ #\\  #\\\\ #\\_ #\\/ #\\. #\\  #\\_ #\\/ #\\  #\\\\ #\\_ #\\  #\\. #\\\\ #\\newline\n"
    "   #\\\\ #\\  #\\/ #\\  #\\\\ #\\  #\\/ #\\  #\\  #\\_ #\\/ #\\  #\\\\ #\\_ #\\/ #\\newline\n"
    "   #\\/ #\\  #\\  #\\_ #\\/ #\\. #\\\\ #\\  #\\/ #\\  #\\\\ #\\  #\\/ #\\  #\\\\ #\\newline\n"
    "   #\\\\ #\\  #\\/ #\\  #\\\\ #\\  #\\/ #\\  #\\  #\\_ #\\/ #\\  #\\  #\\  #\\/ #\\newline\n"
    "   #\\/ #\\  #\\\\ #\\  #\\/ #\\. #\\\\ #\\  #\\/ #\\. #\\\\ #\\_ #\\/ #\\  #\\\\ #\\newline\n"
    "   #\\\\ #\\_ #\\/ #\\  #\\\\ #\\  #\\/ #\\. #\\  #\\_ #\\  #\\. #\\\\ #\\  #\\/ #\\newline\n"
    "   #\\/ #\\  #\\\\ #\\_ #\\  #\\. #\\  #\\_ #\\/ #\\  #\\\\ #\\  #\\  #\\  #\\\\ #\\newline\n"
    "   #\\\\ #\\_ #\\  #\\  #\\\\ #\\_ #\\/ #\\  #\\  #\\_ #\\/ #\\. #\\\\ #\\  #\\/ #\\newline\n"
    "   #\\/ #\\  #\\  #\\_ #\\/ #\\  #\\  #\\  #\\/ #\\  #\\\\ #\\  #\\/ #\\  #\\\\ #\\newline\n"
    "   #\\\\ #\\_ #\\  #\\  #\\\\ #\\  #\\/ #\\  #\\\\ #\\_ #\\  #\\. #\\\\ #\\_ #\\/ #\\newline\n"
    "   #\\/ #\\  #\\\\ #\\_ #\\  #\\  #\\\\ #\\_ #\\  #\\  #\\\\ #\\_ #\\  #\\. #\\\\ #\\newline\n"
    "   #\\\\ #\\_ #\\  #\\  #\\\\ #\\_ #\\/ #\\  #\\  #\\_ #\\/ #\\. #\\\\ #\\  #\\/ #\\newline\n"
    "   #\\/ #\\  #\\\\ #\\_ #\\  #\\  #\\\\ #\\  #\\/ #\\. #\\\\ #\\  #\\  #\\. #\\\\ #\\newline\n"
    "   #\\\\ #\\  #\\/ #\\. #\\\\ #\\_ #\\  #\\. #\\  #\\  #\\/ #\\. #\\\\ #\\  #\\/ #\\newline\n"
    "   #\\/ #\\  #\\  #\\  #\\  #\\. #\\  #\\_ #\\/ #\\. #\\\\ #\\  #\\/ #\\  #\\\\ #\\newline\n"
    "   #\\\\ #\\  #\\/ #\\. #\\\\ #\\_ #\\/ #\\. #\\\\ #\\_ #\\  #\\. #\\\\ #\\  #\\/ #\\newline\n"
    "   #\\/ #\\  #\\\\ #\\_ #\\  #\\. #\\  #\\  #\\/ #\\  #\\  #\\_ #\\/ #\\  #\\\\ #\\newline\n"
    "   #\\\\ #\\_ #\\  #\\  #\\\\ #\\_ #\\/ #\\. #\\\\ #\\_ #\\  #\\  #\\\\ #\\_ #\\/ #\\newline\n"
    "   #\\/ #\\  #\\  #\\_ #\\/ #\\  #\\\\ #\\  #\\/ #\\  #\\\\ #\\_ #\\  #\\  #\\\\ #\\newline\n"
    "   #\\\\ #\\_ #\\/ #\\  #\\  #\\_ #\\/ #\\. #\\\\ #\\_ #\\  #\\  #\\\\ #\\_ #\\/ #\\newline\n"
    "   #\\/ #\\  #\\\\ #\\  #\\/ #\\  #\\  #\\_ #\\  #\\. #\\  #\\_ #\\  #\\  #\\\\ #\\newline\n"
    "   #\\\\ #\\  #\\/ #\\  #\\\\ #\\_ #\\/ #\\. #\\  #\\_ #\\  #\\  #\\\\ #\\_ #\\/ #\\newline\n"
    "   #\\/ #\\  #\\  #\\_ #\\  #\\  #\\\\ #\\  #\\  #\\  #\\\\ #\\_ #\\/ #\\  #\\\\ #\\newline\n"
    "   #\\\\ #\\_ #\\/ #\\. #\\\\ #\\_ #\\  #\\. #\\\\ #\\_ #\\/ #\\  #\\  #\\_ #\\/ #\\newline\n"
    "   #\\/ #\\  #\\\\ #\\  #\\  #\\. #\\  #\\_ #\\/ #\\  #\\  #\\  #\\/ #\\  #\\\\ #\\newline\n"
    "   #\\\\ #\\  #\\/ #\\. #\\\\ #\\_ #\\/ #\\  #\\\\ #\\_ #\\/ #\\. #\\\\ #\\  #\\/ #\\newline\n"
    "   #\\/ #\\  #\\\\ #\\_ #\\  #\\. #\\  #\\_ #\\/ #\\. #\\  #\\  #\\  #\\  #\\\\ #\\newline\n"
    "   #\\\\ #\\  #\\  #\\  #\\  #\\  #\\  #\\. #\\  #\\  #\\/ #\\. #\\\\ #\\_ #\\/ #\\newline\n"
    "   #\\/ #\\  #\\\\ #\\_ #\\/ #\\  #\\\\ #\\_ #\\/ #\\  #\\\\ #\\_ #\\  #\\. #\\\\ #\\newline\n"
    "   #\\\\ #\\_ #\\/ #\\  #\\  #\\  #\\/ #\\  #\\\\ #\\_ #\\/ #\\. #\\  #\\  #\\/ #\\newline\n"
    "   #\\/ #\\  #\\  #\\  #\\/ #\\  #\\  #\\_ #\\  #\\  #\\\\ #\\  #\\/ #\\  #\\\\ #\\newline\n"
    "   #\\\\ #\\_ #\\/ #\\  #\\\\ #\\_ #\\/ #\\  #\\\\ #\\_ #\\/ #\\. #\\\\ #\\_ #\\/ #\\newline\n"
    "   #\\/ #\\  #\\\\ #\\_ #\\/ #\\  #\\  #\\_ #\\/ #\\  #\\\\ #\\_ #\\  #\\. #\\\\ #\\newline\n"
    "   #\\\\ #\\  #\\  #\\  #\\  #\\_ #\\/ #\\. #\\  #\\  #\\/ #\\. #\\  #\\_ #\\/ #\\newline\n"
    "   #\\/ #\\  #\\\\ #\\  #\\/ #\\. #\\  #\\  #\\/ #\\  #\\\\ #\\_ #\\  #\\. #\\\\ #\\newline\n"
    "   #\\\\ #\\_ #\\/ #\\. #\\  #\\_ #\\/ #\\. #\\\\ #\\_ #\\/ #\\. #\\\\ #\\  #\\/ #\\newline\n"
    "   #\\/ #\\  #\\  #\\_ #\\  #\\. #\\\\ #\\_ #\\  #\\. #\\  #\\_ #\\  #\\. #\\\\ #\\newline\n"
    "   #\\\\ #\\_ #\\/ #\\  #\\\\ #\\  #\\/ #\\  #\\\\ #\\_ #\\/ #\\  #\\\\ #\\_ #\\/ #\\newline))\n", 10000);
  }

static void test_unquote_bug()
  {
  test_compile_aux("(55)", "(define (qt a) `(,a)  ) (qt 55)");
  test_compile_aux("((55))", "(define(qt a) `(, (let((x `(, a))) x)))(qt 55)");
  }

static void test_hex()
  {
  test_compile_aux("255", "#xff");
  test_compile_aux("255", "#xFF");
  test_compile_aux("255", "#xFf");
  test_compile_aux("255", "#xfF");
  test_compile_aux("243", "#xf3");
  test_compile_aux("11229389", "#xab58cd");
  test_compile_aux("0", "#xg");
  }

static void test_bin()
  {
  test_compile_aux("18", "#b10010");
  test_compile_aux("1", "#b0000000001");
  test_compile_aux("127", "#b1111111");
  test_compile_aux("0", "#b333");
  }

static void test_octal()
  {
  test_compile_aux("4104", "#o10010");
  test_compile_aux("59", "#o73");
  test_compile_aux("0", "#o9");
  test_compile_aux("0", "#o8");
  }

static void test_build_function()
  {
  const char* script = "(lambda (a b) (+ a b))";
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_preprocess(ctxt, &prog);
   
#if 0
  schemerlicht_string dumped = schemerlicht_dump(ctxt, &prog);
  printf("%s\n", dumped.string_ptr);
  schemerlicht_string_destroy(ctxt, &dumped);
#endif  
  schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);
  schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program);
  schemerlicht_function* fun = cast(schemerlicht_function*, res->value.ptr);
  schemerlicht_string s = schemerlicht_object_to_string(ctxt, res, 0);
  schemerlicht_print_any_error(ctxt); 
  TEST_EQ_STRING("<lambda>", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);  
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  
  schemerlicht_object* st0 = schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
  schemerlicht_object* st1 = schemerlicht_vector_at(&ctxt->stack, 1, schemerlicht_object);
  schemerlicht_object* st2 = schemerlicht_vector_at(&ctxt->stack, 2, schemerlicht_object);
  schemerlicht_object* st3 = schemerlicht_vector_at(&ctxt->stack, 3, schemerlicht_object);
    
  *st0 = ctxt->empty_continuation;
  *st1 = ctxt->halt_continuation;
  st2->type = schemerlicht_object_type_fixnum;
  st2->value.fx = 20;
  st3->type = schemerlicht_object_type_fixnum;
  st3->value.fx = 25;
  res = schemerlicht_run(ctxt, fun);
  s = schemerlicht_object_to_string(ctxt, res, 0);
  schemerlicht_print_any_error(ctxt);
  TEST_EQ_STRING("45", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);

  *st0 = ctxt->empty_continuation;
  *st1 = ctxt->halt_continuation;
  st2->type = schemerlicht_object_type_fixnum;
  st2->value.fx = 123;
  st3->type = schemerlicht_object_type_fixnum;
  st3->value.fx = 456;
  res = schemerlicht_run(ctxt, fun);
  s = schemerlicht_object_to_string(ctxt, res, 0);
  schemerlicht_print_any_error(ctxt);
  TEST_EQ_STRING("579", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);

  schemerlicht_compiled_program_destroy(ctxt, &compiled_program);
  schemerlicht_close(ctxt);
  }

static void test_build_function_2()
  {
  const char* script = "(define my_global_var 180)";
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_preprocess(ctxt, &prog);
  schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);
  schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);

  const char* script2 = "(lambda (x) (+ x my_global_var))";
  tokens = schemerlicht_script2tokens(ctxt, script2);
  prog = make_program(ctxt, &tokens);
  schemerlicht_preprocess(ctxt, &prog);
  schemerlicht_vector compiled_program2 = schemerlicht_compile_program(ctxt, &prog);
  res = schemerlicht_run_program(ctxt, &compiled_program2);
  schemerlicht_function* fun = cast(schemerlicht_function*, res->value.ptr);
  schemerlicht_string s = schemerlicht_object_to_string(ctxt, res, 0);
  schemerlicht_print_any_error(ctxt);
  TEST_EQ_STRING("<lambda>", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);

  schemerlicht_object* st0 = schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
  schemerlicht_object* st1 = schemerlicht_vector_at(&ctxt->stack, 1, schemerlicht_object);
  schemerlicht_object* st2 = schemerlicht_vector_at(&ctxt->stack, 2, schemerlicht_object);

  *st0 = ctxt->empty_continuation;
  *st1 = ctxt->halt_continuation;
  st2->type = schemerlicht_object_type_fixnum;
  st2->value.fx = 20;
  res = schemerlicht_run(ctxt, fun);
  s = schemerlicht_object_to_string(ctxt, res, 0);
  schemerlicht_print_any_error(ctxt);
  TEST_EQ_STRING("200", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);

  *st0 = ctxt->empty_continuation;
  *st1 = ctxt->halt_continuation;
  st2->type = schemerlicht_object_type_fixnum;
  st2->value.fx = 123;
  res = schemerlicht_run(ctxt, fun);
  s = schemerlicht_object_to_string(ctxt, res, 0);
  schemerlicht_print_any_error(ctxt);
  TEST_EQ_STRING("303", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);

  schemerlicht_compiled_program_destroy(ctxt, &compiled_program);
  schemerlicht_compiled_program_destroy(ctxt, &compiled_program2);
  schemerlicht_close(ctxt);
  }
  
static void test_lookup()
  {
  const char* script = "(define sum_fun (lambda (a b) (+ a b)))";
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_preprocess(ctxt, &prog);
  schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);
  schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program);
  schemerlicht_string s = schemerlicht_object_to_string(ctxt, res, 0);
  schemerlicht_print_any_error(ctxt);
  TEST_EQ_STRING("<lambda>", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  
  schemerlicht_function* sum_fun = schemerlicht_lookup(ctxt, "sum_fun");
  TEST_EQ_INT(1, sum_fun == NULL ? 0 : 1);
    
  schemerlicht_object* st0 = schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
  schemerlicht_object* st1 = schemerlicht_vector_at(&ctxt->stack, 1, schemerlicht_object);
  schemerlicht_object* st2 = schemerlicht_vector_at(&ctxt->stack, 2, schemerlicht_object);
  schemerlicht_object* st3 = schemerlicht_vector_at(&ctxt->stack, 3, schemerlicht_object);
    
  *st0 = ctxt->empty_continuation;
  *st1 = ctxt->halt_continuation;
  st2->type = schemerlicht_object_type_fixnum;
  st2->value.fx = 20;
  st3->type = schemerlicht_object_type_fixnum;
  st3->value.fx = 25;
  res = schemerlicht_run(ctxt, sum_fun);
  s = schemerlicht_object_to_string(ctxt, res, 0);
  schemerlicht_print_any_error(ctxt);
  TEST_EQ_STRING("45", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);
    
  schemerlicht_close(ctxt);
  }
  
static void test_schemerlicht_call()
  {
  const char* script = "(define sum_fun (lambda (a b) (+ a b)))";
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_preprocess(ctxt, &prog);
  schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);
  schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program);
  schemerlicht_string s = schemerlicht_object_to_string(ctxt, res, 0);
  schemerlicht_print_any_error(ctxt);
  TEST_EQ_STRING("<lambda>", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  
  schemerlicht_function* sum_fun = schemerlicht_lookup(ctxt, "sum_fun");
  TEST_EQ_INT(1, sum_fun == NULL ? 0 : 1);
  
  schemerlicht_object arg1 = schemerlicht_make_flonum(3.25);
  schemerlicht_object arg2 = schemerlicht_make_flonum(1.75);
  
  res = schemerlicht_call_2(ctxt, sum_fun, &arg1, &arg2);
  s = schemerlicht_object_to_string(ctxt, res, 0);
  schemerlicht_print_any_error(ctxt);
  TEST_EQ_STRING("5.000000", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);
    
  schemerlicht_close(ctxt);
  }

void run_all_compiler_tests()
  { 
  for (int i = 0; i < 2; ++i)
    {
    full_preprocessor = i;
#if 1
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
    test_define();    
    test_fixnum_char_flonum_conversions();
    test_bitwise_ops();
    test_vector();
    test_pair();
    test_begin();        
    test_letrec();    
    test_lambdas();
    test_tailcall();
    test_closures();
    test_set();
    test_letrec2();    
    test_inner_define();
    test_global_define();
    test_list();    
    test_scheme();
    test_fibonacci();
    test_vectors();
    test_strings();
    test_quotes();    
    test_length();
    test_set_car_cdr();
    test_when_unless();
    test_symbol();
    test_primitive_objects();
    test_applying_thunks();
    test_parameter_passing();
    test_cond();
    test_newton();
    test_compile_cc();
    test_compile_cc_2();    
    test_ack_performance();
    test_fib_performance();    
    test_lambda_variable_arity_not_using_rest_arg();
    test_lambda_variable_arity_while_using_rest_arg();
    test_lambda_long_list();
    test_lambda_variable_arity_while_using_rest_arg_and_closure();
    test_garbage_collection();
    test_is_equal();
    test_memv_memq_member();
    test_case();
    test_named_let();
    test_list_ops();
    test_symbol_ops();    
    test_string_ops();
    test_current_seconds();
    test_is_list();
    test_min_max();
    test_override();
    test_apply();    
    test_foreign_1();
    test_foreign();    
    test_r5rs_funs();    
    test_chars();    
    test_list_conversions();
    test_control_ops();
    test_quasiquote();    
    test_port();
    test_read();
    test_write();
    test_display();    
    if (i == 0)
      test_getenv(); // only run once, as the environment will be modified
    test_eval();
    test_load();
    test_macros();
    test_calcc_extended();    
    test_error();
    test_curry();
    test_curry_2();
    test_long_apply();
    test_jaffer_bug_1();
    test_jaffer_bug_2();
    test_jaffer_bug_3();
    test_nested_define();
    test_define_vs_internal_define();
    test_quasiquote_comparison();
    test_string_list();
    test_compare_quote();
    test_multi_float_print();
    test_float_rw_range();
    test_multiple_lines();    
    test_car_bug();    
    test_long_symbol();
    test_unquote_bug();
    test_hex();
    test_bin();
    test_octal();
    test_build_function();
    test_build_function_2();
#endif
    test_lookup();
    test_schemerlicht_call();
    }
  }
