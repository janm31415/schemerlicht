#include "module_tests.h"
#include "schemerlicht/schemerlicht.h"
#include "schemerlicht/context.h"
#include "schemerlicht/parser.h"
#include "schemerlicht/preprocess.h"
#include "schemerlicht/token.h"
#include "schemerlicht/callcc.h"
#include "schemerlicht/r5rs.h"
#include "schemerlicht/inputoutput.h"
#include "schemerlicht/modules.h"
#include "schemerlicht/modulesconf.h"
#include "schemerlicht/compiler.h"
#include "schemerlicht/func.h"
#include "schemerlicht/error.h"
#include "schemerlicht/vm.h"
#include "schemerlicht/dump.h"
#include "test_assert.h"

#include <time.h>

static void test_compile_aux_w_dump(schemerlicht_context* ctxt, const char* expected_value, const char* script)
  {
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_preprocess(ctxt, &prog);
#if 1
  schemerlicht_string dumped = schemerlicht_dump(ctxt, &prog);
  printf("%s\n", dumped.string_ptr);
  schemerlicht_string_destroy(ctxt, &dumped);
#endif
  schemerlicht_function* func = schemerlicht_compile_expression(ctxt, schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression));
  schemerlicht_string debug_str;
  schemerlicht_string_init(ctxt, &debug_str, "");
  schemerlicht_object* res = schemerlicht_run_debug(ctxt, &debug_str, func);
  printf("%s\n", debug_str.string_ptr);
  schemerlicht_string_destroy(ctxt, &debug_str);
  schemerlicht_string s = schemerlicht_object_to_string(ctxt, res, 0);

  if (ctxt->number_of_compile_errors > 0)
    {
    schemerlicht_error_report* it = schemerlicht_vector_back(&ctxt->compile_error_reports, schemerlicht_error_report);
    printf("%s\n", it->message.string_ptr);
    }
  if (ctxt->number_of_syntax_errors > 0)
    {
    schemerlicht_error_report* it = schemerlicht_vector_back(&ctxt->syntax_error_reports, schemerlicht_error_report);
    printf("%s\n", it->message.string_ptr);
    }
  if (ctxt->number_of_runtime_errors > 0)
    {
    schemerlicht_error_report* it = schemerlicht_vector_back(&ctxt->runtime_error_reports, schemerlicht_error_report);
    printf("%s\n", it->message.string_ptr);
    }

  TEST_EQ_STRING(expected_value, s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);

  schemerlicht_vector_push_back(ctxt, &ctxt->lambdas, func, schemerlicht_function*);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  }

static void test_compile_aux(schemerlicht_context* ctxt, const char* expected_value, const char* script)
  {
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_preprocess(ctxt, &prog);
#if 0
  schemerlicht_string dumped = schemerlicht_dump(ctxt, &prog);
  printf("%s\n", dumped.string_ptr);
  schemerlicht_string_destroy(ctxt, &dumped);
#endif
  schemerlicht_function* func = schemerlicht_compile_expression(ctxt, schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression));
  schemerlicht_object* res = schemerlicht_run(ctxt, func);
  schemerlicht_string s = schemerlicht_object_to_string(ctxt, res, 0);

  if (ctxt->number_of_compile_errors > 0)
    {
    schemerlicht_error_report* it = schemerlicht_vector_begin(&ctxt->compile_error_reports, schemerlicht_error_report);
    schemerlicht_error_report* it_end = schemerlicht_vector_end(&ctxt->compile_error_reports, schemerlicht_error_report);
    for (; it != it_end; ++it)
      printf("%s\n", it->message.string_ptr);
    }
  if (ctxt->number_of_syntax_errors > 0)
    {
    schemerlicht_error_report* it = schemerlicht_vector_begin(&ctxt->syntax_error_reports, schemerlicht_error_report);
    schemerlicht_error_report* it_end = schemerlicht_vector_end(&ctxt->syntax_error_reports, schemerlicht_error_report);
    for (; it != it_end; ++it)
      printf("%s\n", it->message.string_ptr);
    }
  if (ctxt->number_of_runtime_errors > 0)
    {
    schemerlicht_error_report* it = schemerlicht_vector_begin(&ctxt->runtime_error_reports, schemerlicht_error_report);
    schemerlicht_error_report* it_end = schemerlicht_vector_end(&ctxt->runtime_error_reports, schemerlicht_error_report);
    for (; it != it_end; ++it)
      printf("%s\n", it->message.string_ptr);
    }

  TEST_EQ_STRING(expected_value, s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);

  schemerlicht_vector_push_back(ctxt, &ctxt->lambdas, func, schemerlicht_function*);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  }

static void test_srfi6(schemerlicht_context* ctxt)
  {
  test_compile_aux(ctxt, "<port>: \"input-string\"", "(import 'srfi-6) (define p (open-input-string \"21 34\"))");
  test_compile_aux(ctxt, "#t", "(input-port? p)");
  test_compile_aux(ctxt, "21", "(read p)");
  test_compile_aux(ctxt, "34", "(read p)");
  test_compile_aux(ctxt, "#t", "(eof-object? (peek-char p))");

  test_compile_aux(ctxt, "<port>: \"input-string\"", "(define p2 (open-input-string \"(a . (b . (c . ()))) 34\"))");
  test_compile_aux(ctxt, "#t", "(input-port? p2)");
  test_compile_aux(ctxt, "(a b c)", "(read p2)");
  test_compile_aux(ctxt, "34", "(read p2)");
  test_compile_aux(ctxt, "#t", "(eof-object? (peek-char p2))");

  const char* script =
    "(let ([q (open-output-string)]\n"
    "      [x '(a b c)])\n"
    "          (write (car x) q)\n"
    "          (write (cdr x) q)\n"
    "          (get-output-string q))\n";

  test_compile_aux(ctxt, "\"a(b c)\"", script);

  const char* script2 =
    "(let ([os (open-output-string)])\n"
    "          (write \"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\" os)\n"
    "          (get-output-string os))\n";

  test_compile_aux(ctxt, "\"\"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\"\"", script2);

  }

static void test_srfi28(schemerlicht_context* ctxt)
  {
  test_compile_aux(ctxt, "\"Hello, World!\"", "(import 'srfi-28) (format \"Hello, ~a\" \"World!\")");
  test_compile_aux(ctxt, "\"Error, list is too short: (one \"two\" 3)\n\"", "(format \"Error, list is too short: ~s~%\" '(one \"two\" 3))");
  }

static void test_csv(schemerlicht_context* ctxt)
  {
  test_compile_aux(ctxt, "((1 2 3 4) (5 6 7 8))", "(import 'csv) (define lst '((1 2 3 4) (5 6 7 8)))");
  test_compile_aux(ctxt, "#<void>", "(write-csv lst \"out.csv\")");
  test_compile_aux(ctxt, "((\"1\" \"2\" \"3\" \"4\") (\"5\" \"6\" \"7\" \"8\"))", "(define r (read-csv \"out.csv\")) r");
  test_compile_aux(ctxt, "((\"1\" \"2\" \"3\" \"4\") (\"5\" \"6\" \"7\" \"8\"))", "r");
  test_compile_aux(ctxt, "((1 2 3 4) (5 6 7 8))", "(map (lambda (lst) (map string->number lst)) r)");
  test_compile_aux(ctxt, "((1 2 3 4) (5 6 7 8))", "(csv-map string->number r)");
  test_compile_aux(ctxt, "((1 2 3 4) (5 6 7 8))", "(csv->numbers r)");
  test_compile_aux(ctxt, "((\"Jan\" #\\K Symb \"\"Quoted string\"\") (5 6 7 8))", "(set! lst '((\"Jan\" #\\K Symb \"\\\"Quoted string\\\"\") (5 6 7 8)))");
  test_compile_aux(ctxt, "#<void>", "(write-csv lst \"out.csv\")");
  test_compile_aux(ctxt, "((\"Jan\" \"K\" \"Symb\" \"\"Quoted string\"\") (\"5\" \"6\" \"7\" \"8\"))", "(define r (read-csv \"out.csv\"))");
  test_compile_aux(ctxt, "4", "(length (list-ref r 0))");
  test_compile_aux(ctxt, "4", "(length (list-ref r 1))");
  }

static void test_jaffer(schemerlicht_context* ctxt)
  {
  test_compile_aux(ctxt, "#t", "(import 'test-r4rs)");
  }

void run_all_module_tests()
  {
  int c0 = clock();
  schemerlicht_context* ctxt = schemerlicht_open(2048 * 16);
  schemerlicht_compile_callcc(ctxt);
  schemerlicht_compile_r5rs(ctxt);
  schemerlicht_compile_input_output(ctxt);
  schemerlicht_compile_modules(ctxt, SCHEMERLICHT_MODULES_PATH);
  int c1 = clock();
  printf("Startup time: %lldms\n", (int64_t)(c1 - c0) * (int64_t)1000 / (int64_t)CLOCKS_PER_SEC);

  //test_srfi6(ctxt);
  //test_srfi28(ctxt);
  //test_csv(ctxt);
  test_jaffer(ctxt);

  schemerlicht_close(ctxt);
  }