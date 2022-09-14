#include "copy_tests.h"
#include "schemerlicht/schemerlicht.h"
#include "schemerlicht/parser.h"
#include "schemerlicht/copy.h"
#include "schemerlicht/dump.h"
#include "schemerlicht/token.h"
#include "schemerlicht/context.h"
#include "test_assert.h"
#include "token_tests.h"

void test_copy(const char* script)
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, script);
  schemerlicht_program prog1 = make_program(ctxt, &tokens);
  schemerlicht_string s1 = dump(ctxt, &prog1);
  schemerlicht_program prog2 = schemerlicht_program_copy(ctxt, &prog1);
  schemerlicht_string s2 = dump(ctxt, &prog2);

  TEST_EQ_STRING(s1.string_ptr, s2.string_ptr);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog1);
  schemerlicht_program_destroy(ctxt, &prog2);
  schemerlicht_string_destroy(ctxt, &s1);
  schemerlicht_string_destroy(ctxt, &s2);
  schemerlicht_close(ctxt);
  }

void run_all_copy_tests()
  {
  test_copy("5");
  test_copy("3.14");
  test_copy("\"string\"");
  test_copy("symbol");
  test_copy("#\\a");
  test_copy("#t");
  test_copy("#f");
  test_copy("()");
  test_copy("(begin 1 2 3)");
  test_copy("(+ 1 2 3)");
  test_copy("(foreign-call load-simulation addr \"13\")");
  test_copy("(f 1 2 3)");
  test_copy("(if (> 1 2) 3 4)");
  test_copy("(set! x 5)");
  test_copy("(lambda (x) (* x x))");
  test_copy("(let ([x 3] [y 4]) ( + x y) )");
  test_copy("(quote #t)");
  test_copy("'(a b c 1 2 3)");
  test_copy("'(a b c #(1 2 3))");
  test_copy("'()");
  }