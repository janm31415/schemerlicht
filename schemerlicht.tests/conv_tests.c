#include "conv_tests.h"
#include "schemerlicht/sdump.h"
#include "schemerlicht/scontext.h"
#include "schemerlicht/stoken.h"
#include "schemerlicht/sbegconv.h"
#include "schemerlicht/sdefconv.h"
#include "schemerlicht/ssimplifyconv.h"
#include "test_assert.h"
#include "token_tests.h"

static void test_convert_define()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(define x 5)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( set! x 5 ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_convert_define_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(let ([x 5]) (define foo (lambda (y) (bar x y))) (define bar (lambda (a b) (+ (* a b) a))) (foo (+ x 3)))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( let ( [ x 5 ] ) ( begin ( letrec ( [ foo ( lambda ( y ) ( begin ( bar x y ) ) ) ] [ bar ( lambda ( a b ) ( begin ( + ( * a b ) a ) ) ) ] ) ( begin ( foo ( + x 3 ) ) ) ) ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_convert_define_3()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(define y 5) (let ([z y]) ( + z 1) ) ");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( set! y 5 ) ( let ( [ z y ] ) ( begin ( + z 1 ) ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_convert_define_4()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(define ( foo x ) ( * x x )) ");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( set! foo ( lambda ( x ) ( begin ( * x x ) ) ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_convert_define_5()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(define (f . x) (apply + x)) (f 1 2 3 4 5)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( set! f ( lambda ( . x ) ( begin ( apply + x ) ) ) ) ( f 1 2 3 4 5 ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_convert_define_6()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(define ( + x ) ( * x x )) ");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( set! + ( lambda ( x ) ( begin ( * x x ) ) ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_single_begin_conv()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(define x 5) (define y 6)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_single_begin_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( begin ( define x 5 ) ( define y 6 ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_single_begin_conv_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(begin (define x 5)) (begin (begin (define y 6)))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_single_begin_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( begin ( define x 5 ) ( define y 6 ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_and()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(and)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("#t ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_and_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(and 3)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("3 ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_and_3()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(and 3 4)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( if 3 4 #f ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_or()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(or)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("#f ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_or_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(or 3)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("3 ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_or_3()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(or 3 4)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( let ( [ #%x 3 ] ) ( begin ( if #%x #%x 4 ) ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_letrec()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(letrec ([f (lambda () 5)]) (- 20 (f)))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( let ( [ f #undefined ] ) ( begin ( let ( [ #%t0 ( lambda ( ) ( begin 5 ) ) ] ) ( begin ( set! f #%t0 ) ) ) ( - 20 ( f ) ) ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

void run_all_conv_tests()
  {
  test_single_begin_conv();
  test_single_begin_conv_2();
  test_convert_define();
  test_convert_define_2();
  test_convert_define_3();
  test_convert_define_4();
  test_convert_define_5();
  test_convert_define_6();
  simplify_to_core_conversion_and();
  simplify_to_core_conversion_and_2();
  simplify_to_core_conversion_and_3();
  simplify_to_core_conversion_or();
  simplify_to_core_conversion_or_2();
  simplify_to_core_conversion_or_3();
  simplify_to_core_conversion_letrec();
  }