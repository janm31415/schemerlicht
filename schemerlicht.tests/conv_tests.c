#include "conv_tests.h"
#include "schemerlicht/dump.h"
#include "schemerlicht/context.h"
#include "schemerlicht/token.h"
#include "schemerlicht/begconv.h"
#include "schemerlicht/defconv.h"
#include "schemerlicht/simplifyconv.h"
#include "schemerlicht/tailcall.h"
#include "schemerlicht/cps.h"
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

static void simplify_to_core_conversion_let_star()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(let* ([x (+ 1 2)] [y(+ 3 4)])(+ x y))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( let ( [ x ( + 1 2 ) ] ) ( begin ( let ( [ y ( + 3 4 ) ] ) ( begin ( + x y ) ) ) ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_named_let()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(define (number->list n) (let loop((n n) (acc '())) (if (< n 10) (cons n acc) (loop(quotient n 10) (cons(remainder n 10) acc)))))");
  schemerlicht_program prog = make_program(ctxt, &tokens);  

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( define ( number->list n ) ( let loop ( [ n n ] [ acc ( quote () ) ] ) ( begin ( if ( < n 10 ) ( cons n acc ) ( loop ( quotient n 10 ) ( cons ( remainder n 10 ) acc ) ) ) ) ) ) ", dumper->s.string_ptr);

  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_string_clear(&dumper->s);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( define ( number->list n ) ( ( let ( [ loop #undefined ] ) ( begin ( let ( [ #%t0 ( lambda ( n acc ) ( begin ( if ( < n 10 ) ( cons n acc ) ( loop ( quotient n 10 ) ( cons ( remainder n 10 ) acc ) ) ) ) ) ] ) ( begin ( set! loop #%t0 ) ) ) loop ) ) n ( quote () ) ) ) ", dumper->s.string_ptr);

  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void tail_call_analysis()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(define fact (lambda (x) (if (= x 0) 1 ( * x ( fact (- x 1))))))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_tail_call_analysis(ctxt, &prog);
  int only_tails = schemerlicht_program_only_has_tail_calls(ctxt, &prog);
  TEST_EQ_INT(0, only_tails);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }
static void tail_call_analysis_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(define fact (lambda (x) (define fact-tail (lambda (x accum) (if (= x 0) accum (fact-tail (- x 1) (* x accum))))) (fact-tail x 1) ) )");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_tail_call_analysis(ctxt, &prog);
  int only_tails = schemerlicht_program_only_has_tail_calls(ctxt, &prog);
  TEST_EQ_INT(1, only_tails);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void cps(const char* script, const char* expected)
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_continuation_passing_style(ctxt, &prog);
  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING(expected, dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);
  schemerlicht_tail_call_analysis(ctxt, &prog);
  int only_tails = schemerlicht_program_only_has_tail_calls(ctxt, &prog);
  TEST_EQ_INT(1, only_tails);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_cps_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(define square (lambda (x) (* x x))) ( + (square 5) 1)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  schemerlicht_continuation_passing_style(ctxt, &prog);
  schemerlicht_string res = schemerlicht_dump(ctxt, &prog);  
  TEST_EQ_STRING("( begin ( let ( [ #%k1 ( lambda ( #%k2 x ) ( begin ( #%k2 ( * x x ) ) ) ) ] ) ( begin ( let ( [ #%k0 ( set! square #%k1 ) ] ) ( begin ( halt #%k0 ) ) ) ) ) ( let ( [ #%k4 square ] ) ( begin ( #%k4 ( lambda ( #%k1 ) ( begin ( let ( [ #%k0 ( + #%k1 1 ) ] ) ( begin ( halt #%k0 ) ) ) ) ) 5 ) ) ) ) ", res.string_ptr);
  schemerlicht_string_destroy(ctxt, &res);
  schemerlicht_tail_call_analysis(ctxt, &prog);
  int only_tails = schemerlicht_program_only_has_tail_calls(ctxt, &prog);
  TEST_EQ_INT(1, only_tails);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_cps()
  {
  cps("(15)", "( let ( [ #%k0 15 ] ) ( begin ( halt #%k0 ) ) ) ");
  cps("(+ 22 (- x 3) x)", "( let ( [ #%k0 ( + 22 ( - x 3 ) x ) ] ) ( begin ( halt #%k0 ) ) ) ");
  cps("(+ 22 (f x) 33 (g y))", "( let ( [ #%k10 f ] ) ( begin ( #%k10 ( lambda ( #%k2 ) ( begin ( let ( [ #%k6 g ] ) ( begin ( #%k6 ( lambda ( #%k4 ) ( begin ( let ( [ #%k0 ( + 22 #%k2 33 #%k4 ) ] ) ( begin ( halt #%k0 ) ) ) ) ) y ) ) ) ) ) x ) ) ) ");
  cps("(set! x 5)", "( let ( [ #%k0 ( set! x 5 ) ] ) ( begin ( halt #%k0 ) ) ) ");
  cps("(+ 1 2 3)", "( let ( [ #%k0 ( + 1 2 3 ) ] ) ( begin ( halt #%k0 ) ) ) ");
  cps("(f 3)", "( let ( [ #%k1 f ] ) ( begin ( #%k1 ( lambda ( #%k0 ) ( begin ( halt #%k0 ) ) ) 3 ) ) ) ");
  cps("(f 1 2 3)", "( let ( [ #%k1 f ] ) ( begin ( #%k1 ( lambda ( #%k0 ) ( begin ( halt #%k0 ) ) ) 1 2 3 ) ) ) ");
  cps("(f 1 (g 2) 3)", "( let ( [ #%k1 f ] ) ( begin ( let ( [ #%k6 g ] ) ( begin ( #%k6 ( lambda ( #%k3 ) ( begin ( #%k1 ( lambda ( #%k0 ) ( begin ( halt #%k0 ) ) ) 1 #%k3 3 ) ) ) 2 ) ) ) ) ) ");
  cps("(set! x (f))", "( let ( [ #%k2 f ] ) ( begin ( #%k2 ( lambda ( #%k1 ) ( begin ( let ( [ #%k0 ( set! x #%k1 ) ] ) ( begin ( halt #%k0 ) ) ) ) ) ) ) ) ");
  cps("(begin 1 2)", "( begin ( let ( [ #%k0 1 ] ) ( begin ( halt #%k0 ) ) ) ( let ( [ #%k0 2 ] ) ( begin ( halt #%k0 ) ) ) ) ");
  cps("(begin (a) (b))", "( begin ( let ( [ #%k1 a ] ) ( begin ( #%k1 ( lambda ( #%k0 ) ( begin ( halt #%k0 ) ) ) ) ) ) ( let ( [ #%k1 b ] ) ( begin ( #%k1 ( lambda ( #%k0 ) ( begin ( halt #%k0 ) ) ) ) ) ) ) ");
  cps("(begin (a) 2)", "( begin ( let ( [ #%k1 a ] ) ( begin ( #%k1 ( lambda ( #%k0 ) ( begin ( halt #%k0 ) ) ) ) ) ) ( let ( [ #%k0 2 ] ) ( begin ( halt #%k0 ) ) ) ) ");
  cps("(begin 4 3 (a) 2)", "( begin ( let ( [ #%k0 4 ] ) ( begin ( halt #%k0 ) ) ) ( let ( [ #%k0 3 ] ) ( begin ( halt #%k0 ) ) ) ( let ( [ #%k1 a ] ) ( begin ( #%k1 ( lambda ( #%k0 ) ( begin ( halt #%k0 ) ) ) ) ) ) ( let ( [ #%k0 2 ] ) ( begin ( halt #%k0 ) ) ) ) ");
  cps("(begin 1 2 (a))", "( begin ( let ( [ #%k0 1 ] ) ( begin ( halt #%k0 ) ) ) ( let ( [ #%k0 2 ] ) ( begin ( halt #%k0 ) ) ) ( let ( [ #%k1 a ] ) ( begin ( #%k1 ( lambda ( #%k0 ) ( begin ( halt #%k0 ) ) ) ) ) ) ) ");
  cps("(+ 1 (f) 3)", "( let ( [ #%k5 f ] ) ( begin ( #%k5 ( lambda ( #%k2 ) ( begin ( let ( [ #%k0 ( + 1 #%k2 3 ) ] ) ( begin ( halt #%k0 ) ) ) ) ) ) ) ) ");
  cps("(begin (begin 1 2 3))", "( begin ( begin 1 2 ( let ( [ #%k0 3 ] ) ( begin ( halt #%k0 ) ) ) ) ) ");
  cps("(begin (begin 1 2 (f)))", "( begin ( begin 1 2 ( let ( [ #%k1 f ] ) ( begin ( #%k1 ( lambda ( #%k0 ) ( begin ( halt #%k0 ) ) ) ) ) ) ) ) ");
  cps("(begin (begin 1 (f) 3))", "( begin ( begin 1 ( let ( [ #%k2 f ] ) ( begin ( #%k2 ( lambda ( #%k1 ) ( begin ( let ( [ #%k0 3 ] ) ( begin ( halt #%k0 ) ) ) ) ) ) ) ) ) ) ");
  cps("(begin (begin (f) 2 3))", "( begin ( let ( [ #%k2 f ] ) ( begin ( #%k2 ( lambda ( #%k1 ) ( begin 2 ( let ( [ #%k0 3 ] ) ( begin ( halt #%k0 ) ) ) ) ) ) ) ) ) ");
  cps("(foreign-call load-simulation addr \"13\")", "( let ( [ #%k0 ( foreign-call load-simulation addr \"13\" ) ] ) ( begin ( halt #%k0 ) ) ) ");
  cps("(lambda ( a b ) ( + a b))", "( let ( [ #%k0 ( lambda ( #%k1 a b ) ( begin ( #%k1 ( + a b ) ) ) ) ] ) ( begin ( halt #%k0 ) ) ) ");
  cps("(f)", "( let ( [ #%k1 f ] ) ( begin ( #%k1 ( lambda ( #%k0 ) ( begin ( halt #%k0 ) ) ) ) ) ) ");
  cps("(let ([x 3] [y 4]) ( + x y) )", "( let ( [ x 3 ] [ y 4 ] ) ( begin ( let ( [ #%k0 ( + x y ) ] ) ( begin ( halt #%k0 ) ) ) ) ) ");
  cps("(let ([x (f)] [y 4]) ( + x y) )", "( let ( [ y 4 ] ) ( begin ( let ( [ #%k2 f ] ) ( begin ( #%k2 ( lambda ( x ) ( begin ( let ( [ #%k0 ( + x y ) ] ) ( begin ( halt #%k0 ) ) ) ) ) ) ) ) ) ) ");
  cps("(let ([x (f)] [y (g)]) ( + x y) )", "( let ( [ #%k5 f ] ) ( begin ( #%k5 ( lambda ( x ) ( begin ( let ( [ #%k2 g ] ) ( begin ( #%k2 ( lambda ( y ) ( begin ( let ( [ #%k0 ( + x y ) ] ) ( begin ( halt #%k0 ) ) ) ) ) ) ) ) ) ) ) ) ) ");
  cps("(let ([square (lambda (x) (* x x))]) (write (+ (square 10) 1)))", "( let ( [ square ( lambda ( #%k12 x ) ( begin ( #%k12 ( * x x ) ) ) ) ] ) ( begin ( let ( [ #%k1 write ] ) ( begin ( let ( [ #%k7 square ] ) ( begin ( #%k7 ( lambda ( #%k4 ) ( begin ( let ( [ #%k2 ( + #%k4 1 ) ] ) ( begin ( #%k1 ( lambda ( #%k0 ) ( begin ( halt #%k0 ) ) ) #%k2 ) ) ) ) ) 10 ) ) ) ) ) ) ) ");
  cps("(if #t 2 3)", "( if #t ( let ( [ #%k0 2 ] ) ( begin ( halt #%k0 ) ) ) ( let ( [ #%k0 3 ] ) ( begin ( halt #%k0 ) ) ) ) ");
  cps("(if (f) 2 3)", "( let ( [ #%k2 f ] ) ( begin ( #%k2 ( lambda ( #%k1 ) ( begin ( if #%k1 ( let ( [ #%k0 2 ] ) ( begin ( halt #%k0 ) ) ) ( let ( [ #%k0 3 ] ) ( begin ( halt #%k0 ) ) ) ) ) ) ) ) ) ");
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
  simplify_to_core_conversion_let_star();
  simplify_to_core_conversion_named_let();
  tail_call_analysis();
  tail_call_analysis_2();
  test_cps();
  test_cps_2();
  }