#include "conv_tests.h"
#include "schemerlicht/dump.h"
#include "schemerlicht/context.h"
#include "schemerlicht/token.h"
#include "schemerlicht/begconv.h"
#include "schemerlicht/defconv.h"
#include "schemerlicht/simplifyconv.h"
#include "schemerlicht/tailcall.h"
#include "schemerlicht/cps.h"
#include "schemerlicht/quasiquote.h"
#include "schemerlicht/lambdatolet.h"
#include "schemerlicht/assignablevarconv.h"
#include "schemerlicht/freevaranalysis.h"
#include "schemerlicht/closure.h"
#include "schemerlicht/quotecollect.h"
#include "schemerlicht/globdef.h"
#include "schemerlicht/quoteconv.h"
#include "schemerlicht/alpha.h"
#include "schemerlicht/constprop.h"
#include "schemerlicht/constfold.h"
#include "schemerlicht/preprocess.h"
#include "test_assert.h"
#include "token_tests.h"

static void test_convert_define()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(define x 5)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( set! x 5 ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_convert_define_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(let ([x 5]) (define foo (lambda (y) (bar x y))) (define bar (lambda (a b) (+ (* a b) a))) (foo (+ x 3)))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( let ( [ x 5 ] ) ( begin ( letrec ( [ foo ( lambda ( y ) ( begin ( bar x y ) ) ) ] [ bar ( lambda ( a b ) ( begin ( + ( * a b ) a ) ) ) ] ) ( begin ( foo ( + x 3 ) ) ) ) ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_convert_define_3()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(define y 5) (let ([z y]) ( + z 1) ) ");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( set! y 5 ) ( let ( [ z y ] ) ( begin ( + z 1 ) ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_convert_define_4()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(define ( foo x ) ( * x x )) ");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( set! foo ( lambda ( x ) ( begin ( * x x ) ) ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_convert_define_5()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(define (f . x) (apply + x)) (f 1 2 3 4 5)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( set! f ( lambda ( . x ) ( begin ( apply + x ) ) ) ) ( f 1 2 3 4 5 ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_convert_define_6()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(define ( + x ) ( * x x )) ");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( set! + ( lambda ( x ) ( begin ( * x x ) ) ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_convert_define_7()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(define (a)(let ((var 3))(define b 7.0)(define (fun x) (define (nested y) (+ x y)) (let ((z 3)) (nested z)))))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);  
  TEST_EQ_STRING("( set! a ( lambda ( ) ( begin ( let ( [ var 3 ] ) ( begin ( letrec ( [ b 7.000000 ] [ fun ( lambda ( x ) ( begin ( letrec ( [ nested ( lambda ( y ) ( begin ( + x y ) ) ) ] ) ( begin ( let ( [ z 3 ] ) ( begin ( nested z ) ) ) ) ) ) ) ] ) ( begin ) ) ) ) ) ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_convert_define_8()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(define x 34)(define (foo) (define x 5) x)(foo)x");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( set! x 34 ) ( set! foo ( lambda ( ) ( begin ( letrec ( [ x 5 ] ) ( begin x ) ) ) ) ) ( foo ) x ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_convert_define_9()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(define old-+ +) (define + (lambda (x y) (list y x))) (set! + old-+) (+ 6 3)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( set! old-+ + ) ( set! + ( lambda ( x y ) ( begin ( list y x ) ) ) ) ( set! + old-+ ) ( + 6 3 ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_single_begin_conv()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(define x 5) (define y 6)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_single_begin_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( begin ( define x 5 ) ( define y 6 ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_single_begin_conv_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(begin (define x 5)) (begin (begin (define y 6)))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_single_begin_conversion(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( begin ( define x 5 ) ( define y 6 ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_and()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(and)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("#t ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_and_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(and 3)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("3 ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_and_3()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(and 3 4)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( if 3 4 #f ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_or()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(or)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("#f ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_or_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(or 3)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("3 ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_or_3()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(or 3 4)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( let ( [ #%x 3 ] ) ( begin ( if #%x #%x 4 ) ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_letrec()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(letrec ([f (lambda () 5)]) (- 20 (f)))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( let ( [ f #undefined ] ) ( begin ( let ( [ #%t0 ( lambda ( ) ( begin 5 ) ) ] ) ( begin ( set! f #%t0 ) ) ) ( - 20 ( f ) ) ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_let_star()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(let* ([x (+ 1 2)] [y(+ 3 4)])(+ x y))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  TEST_EQ_STRING("( let ( [ x ( + 1 2 ) ] ) ( begin ( let ( [ y ( + 3 4 ) ] ) ( begin ( + x y ) ) ) ) ) ", dumper->s.string_ptr);
  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_named_let()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(define (number->list n) (let loop((n n) (acc '())) (if (< n 10) (cons n acc) (loop(quotient n 10) (cons(remainder n 10) acc)))))");
  schemerlicht_program prog = make_program(ctxt, &tokens);  

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  dumper->binding_left_bracket = '[';
  dumper->binding_right_bracket = ']';
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

static void simplify_to_core_conversion_case()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(case (+ 7 5) [(1 2 3) 'small] [(10 11 12) 'big])");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  
  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_string s = schemerlicht_dump(ctxt, &prog, 1);

  TEST_EQ_STRING("( let ( [ case-var ( + 7 5 ) ] ) ( begin ( if ( memv case-var ( quote (1 2 3) ) ) ( begin ( quote small ) ) ( if ( memv case-var ( quote (10 11 12) ) ) ( begin ( quote big ) ) ( begin #undefined ) ) ) ) ) ", s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_cond()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(cond(#f)(#f 12)(12 13))");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_string s = schemerlicht_dump(ctxt, &prog, 1);

  TEST_EQ_STRING("( let ( [ cond-var #f ] ) ( begin ( if cond-var cond-var ( let ( [ cond-var #f ] ) ( begin ( if cond-var ( begin 12 ) ( let ( [ cond-var 12 ] ) ( begin ( if cond-var ( begin 13 ) #undefined ) ) ) ) ) ) ) ) ) ", s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_cond_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(cond[(cons 1 2) => (lambda(x) (cdr x))])");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_string s = schemerlicht_dump(ctxt, &prog, 1);

  TEST_EQ_STRING("( let ( [ cond-var ( cons 1 2 ) ] ) ( begin ( if cond-var ( ( lambda ( x ) ( begin ( cdr x ) ) ) cond-var ) #undefined ) ) ) ", s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_do()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(do ([vec (make-vector 5)] [i 0 (+ i 1)])((= i 5) vec)(vector-set! vec i i))");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_string s = schemerlicht_dump(ctxt, &prog, 1);

  TEST_EQ_STRING("( let ( [ loop #undefined ] ) ( begin ( let ( [ #%t0 ( lambda ( vec i ) ( begin ( if ( = i 5 ) ( begin vec ) ( begin ( vector-set! vec i i ) ( loop vec ( + i 1 ) ) ) ) ) ) ] ) ( begin ( set! loop #%t0 ) ) ) ( loop ( make-vector 5 ) 0 ) ) ) ", s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_do_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(define (my-iota n) (do ((n n (- n 1)) (list '() (cons (- n 1) list))) ((zero? n) list)))");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_string s = schemerlicht_dump(ctxt, &prog, 1);

  TEST_EQ_STRING("( define ( my-iota n ) ( let ( [ loop #undefined ] ) ( begin ( let ( [ #%t0 ( lambda ( n list ) ( begin ( if ( zero? n ) ( begin list ) ( begin ( loop ( - n 1 ) ( cons ( - n 1 ) list ) ) ) ) ) ) ] ) ( begin ( set! loop #%t0 ) ) ) ( loop n ( quote () ) ) ) ) ) ", s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_when()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(when (< x 2) (+ 1 2) (* 3 4))");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_string s = schemerlicht_dump(ctxt, &prog, 1);

  TEST_EQ_STRING("( if ( < x 2 ) ( begin ( + 1 2 ) ( * 3 4 ) ) #undefined ) ", s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_unless()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(unless (< x 2) (+ 1 2) (* 3 4))");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_string s = schemerlicht_dump(ctxt, &prog, 1);

  TEST_EQ_STRING("( if ( not ( < x 2 ) ) ( begin ( + 1 2 ) ( * 3 4 ) ) #undefined ) ", s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void simplify_to_core_conversion_delay()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(set! p (delay (* 3 4)))");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_simplify_to_core_forms(ctxt, &prog);

  schemerlicht_string s = schemerlicht_dump(ctxt, &prog, 1);

  TEST_EQ_STRING("( set! p ( make-promise ( lambda ( ) ( begin ( * 3 4 ) ) ) ) ) ", s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void tail_call_analysis()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(define fact (lambda (x) (if (= x 0) 1 ( * x ( fact (- x 1))))))");
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
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(define fact (lambda (x) (define fact-tail (lambda (x accum) (if (= x 0) accum (fact-tail (- x 1) (* x accum))))) (fact-tail x 1) ) )");
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
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_continuation_passing_style(ctxt, &prog);
  schemerlicht_string res = schemerlicht_dump(ctxt, &prog, 1);
  TEST_EQ_STRING(expected, res.string_ptr);
  schemerlicht_string_destroy(ctxt, &res);
  schemerlicht_tail_call_analysis(ctxt, &prog);
  int only_tails = schemerlicht_program_only_has_tail_calls(ctxt, &prog);
  TEST_EQ_INT(1, only_tails);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_cps_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(define square (lambda (x) (* x x))) ( + (square 5) 1)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  schemerlicht_continuation_passing_style(ctxt, &prog);
  schemerlicht_string res = schemerlicht_dump(ctxt, &prog, 1);    
  TEST_EQ_STRING("( let ( [ #%k1 ( lambda ( #%k2 x ) ( begin ( #%k2 ( * x x ) ) ) ) ] ) ( begin ( let ( [ #%k0 ( set! square #%k1 ) ] ) ( begin ( halt #%k0 ) ) ) ) ) ( let ( [ #%k4 square ] ) ( begin ( #%k4 ( lambda ( #%k1 ) ( begin ( let ( [ #%k0 ( + #%k1 1 ) ] ) ( begin ( halt #%k0 ) ) ) ) ) 5 ) ) ) ", res.string_ptr);
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
  cps("(begin 1 2)", "( let ( [ #%k0 1 ] ) ( begin ( halt #%k0 ) ) ) ( let ( [ #%k0 2 ] ) ( begin ( halt #%k0 ) ) ) ");  
  cps("(+ 1 (f) 3)", "( let ( [ #%k5 f ] ) ( begin ( #%k5 ( lambda ( #%k2 ) ( begin ( let ( [ #%k0 ( + 1 #%k2 3 ) ] ) ( begin ( halt #%k0 ) ) ) ) ) ) ) ) ");  
  cps("(begin (begin 1 2 3))", "( begin 1 2 ( let ( [ #%k0 3 ] ) ( begin ( halt #%k0 ) ) ) ) ");
  cps("(begin (begin 1 2 (f)))", "( begin 1 2 ( let ( [ #%k1 f ] ) ( begin ( #%k1 ( lambda ( #%k0 ) ( begin ( halt #%k0 ) ) ) ) ) ) ) ");
  cps("(begin (begin 1 (f) 3))", "( begin 1 ( let ( [ #%k2 f ] ) ( begin ( #%k2 ( lambda ( #%k1 ) ( begin ( let ( [ #%k0 3 ] ) ( begin ( halt #%k0 ) ) ) ) ) ) ) ) ) ");
  cps("(begin (begin (f) 2 3))", "( let ( [ #%k2 f ] ) ( begin ( #%k2 ( lambda ( #%k1 ) ( begin 2 ( let ( [ #%k0 3 ] ) ( begin ( halt #%k0 ) ) ) ) ) ) ) ) ");    
  cps("(foreign-call load-simulation addr \"13\")", "( let ( [ #%k0 ( foreign-call load-simulation addr \"13\" ) ] ) ( begin ( halt #%k0 ) ) ) ");
  cps("(lambda ( a b ) ( + a b))", "( let ( [ #%k0 ( lambda ( #%k1 a b ) ( begin ( #%k1 ( + a b ) ) ) ) ] ) ( begin ( halt #%k0 ) ) ) ");
  cps("(f)", "( let ( [ #%k1 f ] ) ( begin ( #%k1 ( lambda ( #%k0 ) ( begin ( halt #%k0 ) ) ) ) ) ) ");
  cps("(let ([x 3] [y 4]) ( + x y) )", "( let ( [ x 3 ] [ y 4 ] ) ( begin ( let ( [ #%k0 ( + x y ) ] ) ( begin ( halt #%k0 ) ) ) ) ) ");
  cps("(let ([x (f)] [y 4]) ( + x y) )", "( let ( [ y 4 ] ) ( begin ( let ( [ #%k2 f ] ) ( begin ( #%k2 ( lambda ( x ) ( begin ( let ( [ #%k0 ( + x y ) ] ) ( begin ( halt #%k0 ) ) ) ) ) ) ) ) ) ) ");
  cps("(let ([x (f)] [y (g)]) ( + x y) )", "( let ( [ #%k5 f ] ) ( begin ( #%k5 ( lambda ( x ) ( begin ( let ( [ #%k2 g ] ) ( begin ( #%k2 ( lambda ( y ) ( begin ( let ( [ #%k0 ( + x y ) ] ) ( begin ( halt #%k0 ) ) ) ) ) ) ) ) ) ) ) ) ) ");
  cps("(let ([square (lambda (x) (* x x))]) (write (+ (square 10) 1)))", "( let ( [ square ( lambda ( #%k12 x ) ( begin ( #%k12 ( * x x ) ) ) ) ] ) ( begin ( let ( [ #%k1 write ] ) ( begin ( let ( [ #%k7 square ] ) ( begin ( #%k7 ( lambda ( #%k4 ) ( begin ( let ( [ #%k2 ( + #%k4 1 ) ] ) ( begin ( #%k1 ( lambda ( #%k0 ) ( begin ( halt #%k0 ) ) ) #%k2 ) ) ) ) ) 10 ) ) ) ) ) ) ) ");
  cps("(if #t 2 3)", "( if #t ( let ( [ #%k0 2 ] ) ( begin ( halt #%k0 ) ) ) ( let ( [ #%k0 3 ] ) ( begin ( halt #%k0 ) ) ) ) ");
  cps("(if (f) 2 3)", "( let ( [ #%k2 f ] ) ( begin ( #%k2 ( lambda ( #%k1 ) ( begin ( if #%k1 ( let ( [ #%k0 2 ] ) ( begin ( halt #%k0 ) ) ) ( let ( [ #%k0 3 ] ) ( begin ( halt #%k0 ) ) ) ) ) ) ) ) ) ");  
  cps("(let ([x (+ 1 2)]) (let([y(+ 3 4)])(+ x y)))", "( let ( [ x ( + 1 2 ) ] ) ( begin ( let ( [ y ( + 3 4 ) ] ) ( begin ( let ( [ #%k0 ( + x y ) ] ) ( begin ( halt #%k0 ) ) ) ) ) ) ) ");
  cps("(begin 13 122)", "( let ( [ #%k0 13 ] ) ( begin ( halt #%k0 ) ) ) ( let ( [ #%k0 122 ] ) ( begin ( halt #%k0 ) ) ) ");
  }

static void test_quasiquote(const char* script, const char* expected)
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_quasiquote_conversion(ctxt, &prog);
  schemerlicht_string res = schemerlicht_dump(ctxt, &prog, 1);
  TEST_EQ_STRING(expected, res.string_ptr);
  schemerlicht_string_destroy(ctxt, &res);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_quasiquote_conversion()
  {
  test_quasiquote("`(1 2)", "( quote (1 2) ) ");
  test_quasiquote("`let", "( quote let ) ");
  test_quasiquote("`( 1 ,(+ 2 3))", "( cons ( quote 1 ) ( cons ( + 2 3 ) ( quote () ) ) ) ");
  test_quasiquote("(quasiquote ( 1 ,(+ 2 3)))", "( cons ( quote 1 ) ( cons ( + 2 3 ) ( quote () ) ) ) ");
  test_quasiquote("`( ,(+ 2 3) 1)", "( cons ( + 2 3 ) ( quote (1) ) ) ");
  test_quasiquote("(quasiquote( (unquote (+ 2 3)) 1))", "( cons ( + 2 3 ) ( quote (1) ) ) ");
  test_quasiquote("`( 1 2 ,x )", "( cons ( quote 1 ) ( cons ( quote 2 ) ( cons x ( quote () ) ) ) ) ");
  test_quasiquote("(quasiquote ( 1 2 (unquote x) ))", "( cons ( quote 1 ) ( cons ( quote 2 ) ( cons x ( quote () ) ) ) ) ");
  test_quasiquote("`x", "( quote x ) ");
  test_quasiquote("(quasiquote x)", "( quote x ) ");
  test_quasiquote("`,x", "x ");
  test_quasiquote("(quasiquote (unquote x))", "x ");
  test_quasiquote("`( (,(+ 1 2) 3) 5 )", "( cons ( cons ( + 1 2 ) ( quote (3) ) ) ( quote (5) ) ) ");
  test_quasiquote("(quasiquote ( ((unquote (+ 1 2)) 3) 5 ) )", "( cons ( cons ( + 1 2 ) ( quote (3) ) ) ( quote (5) ) ) ");
  test_quasiquote("`( (5 4 ,(+ 1 2) 3) 5 )", "( cons ( cons ( quote 5 ) ( cons ( quote 4 ) ( cons ( + 1 2 ) ( quote (3) ) ) ) ) ( quote (5) ) ) ");
  test_quasiquote("(quasiquote ( (5 4 (unquote (+ 1 2)) 3) 5 ))", "( cons ( cons ( quote 5 ) ( cons ( quote 4 ) ( cons ( + 1 2 ) ( quote (3) ) ) ) ) ( quote (5) ) ) ");
  test_quasiquote("`(,@(cdr '(c)) )", "( cdr ( quote (c) ) ) ");
  test_quasiquote("(quasiquote ( (unquote-splicing (cdr '(c))) ))", "( cdr ( quote (c) ) ) ");
  test_quasiquote("`((foo ,(- 10 3)) ,@(cdr '(c)) . ,(car '(cons)))", "( cons ( cons ( quote foo ) ( cons ( - 10 3 ) ( quote () ) ) ) ( append ( cdr ( quote (c) ) ) ( car ( quote (cons) ) ) ) ) ");
  test_quasiquote("(quasiquote ((foo (unquote (- 10 3))) (unquote-splicing (cdr '(c))) . (unquote (car '(cons)))))", "( cons ( cons ( quote foo ) ( cons ( - 10 3 ) ( quote () ) ) ) ( append ( cdr ( quote (c) ) ) ( car ( quote (cons) ) ) ) ) ");
  test_quasiquote("(let ((name 'a)) `(list ,name ',name))", "( let ( [ name ( quote a ) ] ) ( begin ( cons ( quote list ) ( cons name ( cons ( cons ( quote quote ) ( cons name ( quote () ) ) ) ( quote () ) ) ) ) ) ) ");
  test_quasiquote("(let ((name (quote a))) (quasiquote (list (unquote name) (quote (unquote name)))))", "( let ( [ name ( quote a ) ] ) ( begin ( cons ( quote list ) ( cons name ( cons ( cons ( quote quote ) ( cons name ( quote () ) ) ) ( quote () ) ) ) ) ) ) ");
  test_quasiquote("`(a ,(+ 1 2) ,@(map abs '(4 -5 6)) b)", "( cons ( quote a ) ( cons ( + 1 2 ) ( append ( map abs ( quote (4 -5 6) ) ) ( quote (b) ) ) ) ) ");
  test_quasiquote("( cons ( quote a ) ( cons ( + 1 2 ) ( append ( map abs ( quote (4 -5 6) ) ) ( quote (b) ) ) ) ) ", "( cons ( quote a ) ( cons ( + 1 2 ) ( append ( map abs ( quote (4 -5 6) ) ) ( quote (b) ) ) ) ) ");
  test_quasiquote("`#(10 5 ,(sqrt 4) ,@(map sqrt '(16 9)) 8)", "( list->vector ( cons ( quote 10 ) ( cons ( quote 5 ) ( cons ( sqrt 4 ) ( append ( map sqrt ( quote (16 9) ) ) ( quote (8) ) ) ) ) ) ) ");
  test_quasiquote("(quasiquote #(10 5 (unquote (sqrt 4)) (unquote-splicing (map sqrt (quote (16 9)))) 8))", "( list->vector ( cons ( quote 10 ) ( cons ( quote 5 ) ( cons ( sqrt 4 ) ( append ( map sqrt ( quote (16 9) ) ) ( quote (8) ) ) ) ) ) ) ");
  test_quasiquote("`(1 2 #(10 ,(+ 2 6)) 5)", "( cons ( quote 1 ) ( cons ( quote 2 ) ( cons ( list->vector ( cons ( quote 10 ) ( cons ( + 2 6 ) ( quote () ) ) ) ) ( quote (5) ) ) ) ) ");
  test_quasiquote("(quasiquote (1 2 #(10 (unquote (+ 2 6))) 5))", "( cons ( quote 1 ) ( cons ( quote 2 ) ( cons ( list->vector ( cons ( quote 10 ) ( cons ( + 2 6 ) ( quote () ) ) ) ) ( quote (5) ) ) ) ) ");
  test_quasiquote("`( `(,(+ 1 2 ,(+ 3 4) ) ) )", "( cons ( cons ( quote quasiquote ) ( cons ( cons ( cons ( quote unquote ) ( cons ( cons ( quote + ) ( cons ( quote 1 ) ( cons ( quote 2 ) ( cons ( + 3 4 ) ( quote () ) ) ) ) ) ( quote () ) ) ) ( quote () ) ) ( quote () ) ) ) ( quote () ) ) ");
  test_quasiquote("`(,(+ 1 2 7) )", "( cons ( + 1 2 7 ) ( quote () ) ) ");
  test_quasiquote("`(a `(b ,(+ 1 2) ,(foo ,(+ 1 3) d) e) f)", "( cons ( quote a ) ( cons ( cons ( quote quasiquote ) ( cons ( cons ( quote b ) ( cons ( cons ( quote unquote ) ( cons ( quote (+ 1 2) ) ( quote () ) ) ) ( cons ( cons ( quote unquote ) ( cons ( cons ( quote foo ) ( cons ( + 1 3 ) ( quote (d) ) ) ) ( quote () ) ) ) ( quote (e) ) ) ) ) ( quote () ) ) ) ( quote (f) ) ) ) ");  
  test_quasiquote("(define(qt a) `(, (let((x `(, a))) x)))", "( define ( qt a ) ( cons ( let ( [ x ( cons a ( quote () ) ) ] ) ( begin x ) ) ( quote () ) ) ) ");
  }

static void test_lambda_to_let_conversion()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "((lambda (x y) (+ x y)) 1 2)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_lambda_to_let_conversion(ctxt, &prog);
  schemerlicht_string res = schemerlicht_dump(ctxt, &prog, 1);
  TEST_EQ_STRING("( let ( [ x 1 ] [ y 2 ] ) ( begin ( + x y ) ) ) ", res.string_ptr);
  schemerlicht_string_destroy(ctxt, &res);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_find_assignable_variables()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(let([x 12]) (let([y(let([x #f]) (set! x 14))])  x))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_find_assignable_variables(ctxt, &prog);
  schemerlicht_string res = schemerlicht_dump(ctxt, &prog, 1);
  schemerlicht_vector vars = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression)->expr.let.assignable_variables;
  TEST_EQ_INT(1, vars.vector_size);
  schemerlicht_string s = *schemerlicht_vector_at(&vars, 0, schemerlicht_string);
  TEST_EQ_STRING("x", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &res);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_assignable_variable_conversion()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(let([x 12]) (let([y(let([z #f]) (set! z 14))])  z))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_string res = schemerlicht_dump(ctxt, &prog, 1);
  TEST_EQ_STRING("( let ( [ x 12 ] ) ( begin ( let ( [ y ( let ( [ #%z #f ] ) ( begin ( let ( [ z ( vector #%z ) ] ) ( begin ( vector-set! z 0 14 ) ) ) ) ) ] ) ( begin ( vector-ref z 0 ) ) ) ) ) ", res.string_ptr);
  schemerlicht_string_destroy(ctxt, &res);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_assignable_variable_conversion_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(let ([f (lambda (c) (cons (lambda (v) (set! c v)) (lambda () c)))]) (let ([p (f 0)]) ( (car p) 12) ((cdr p))))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_string res = schemerlicht_dump(ctxt, &prog, 1);
  TEST_EQ_STRING("( let ( [ f ( lambda ( #%c ) ( begin ( let ( [ c ( vector #%c ) ] ) ( begin ( cons ( lambda ( v ) ( begin ( vector-set! c 0 v ) ) ) ( lambda ( ) ( begin ( vector-ref c 0 ) ) ) ) ) ) ) ) ] ) ( begin ( let ( [ p ( f 0 ) ] ) ( begin ( ( car p ) 12 ) ( ( cdr p ) ) ) ) ) ) ", res.string_ptr);
  schemerlicht_string_destroy(ctxt, &res);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_assignable_variable_conversion_3()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(let ([x 3]) (set! x 5))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_string res = schemerlicht_dump(ctxt, &prog, 1);
  TEST_EQ_STRING("( let ( [ #%x 3 ] ) ( begin ( let ( [ x ( vector #%x ) ] ) ( begin ( vector-set! x 0 5 ) ) ) ) ) ", res.string_ptr);
  schemerlicht_string_destroy(ctxt, &res);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_assignable_variable_conversion_4()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(let([x 5])(define foo(lambda(y) (bar x y))) (define bar(lambda(a b) (+(* a b) a))) (foo(+ x 3)))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);
  schemerlicht_lambda_to_let_conversion(ctxt, &prog);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_string res = schemerlicht_dump(ctxt, &prog, 1);
  TEST_EQ_STRING("( let ( [ x 5 ] ) ( begin ( let ( [ #%foo #undefined ] [ #%bar #undefined ] ) ( begin ( let ( [ foo ( vector #%foo ) ] [ bar ( vector #%bar ) ] ) ( begin ( let ( [ #%t0 ( lambda ( y ) ( begin ( ( vector-ref bar 0 ) x y ) ) ) ] [ #%t1 ( lambda ( a b ) ( begin ( + ( * a b ) a ) ) ) ] ) ( begin ( vector-set! foo 0 #%t0 ) ( vector-set! bar 0 #%t1 ) ) ) ( ( vector-ref foo 0 ) ( + x 3 ) ) ) ) ) ) ) ) ", res.string_ptr);
  schemerlicht_string_destroy(ctxt, &res);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_free_var_analysis()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(let ([x 5]) (lambda (y) (lambda () (+ x y))))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_free_variable_analysis(ctxt, &prog);
  schemerlicht_expression* expr = schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_let, expr->type);
  schemerlicht_parsed_let* l = &expr->expr.let;
  schemerlicht_expression* beg = schemerlicht_vector_at(&l->body, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_begin, beg->type);
  schemerlicht_expression* lam1 = schemerlicht_vector_at(&beg->expr.beg.arguments, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_lambda, lam1->type);
  schemerlicht_expression* beg2 = schemerlicht_vector_at(&lam1->expr.lambda.body, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_begin, beg2->type);
  schemerlicht_expression* lam2 = schemerlicht_vector_at(&beg2->expr.beg.arguments, 0, schemerlicht_expression);
  TEST_EQ_INT(schemerlicht_type_lambda, lam2->type);
  schemerlicht_parsed_lambda* lambda1 = &lam1->expr.lambda;
  schemerlicht_parsed_lambda* lambda2 = &lam2->expr.lambda;
  TEST_EQ_INT(1, lambda1->free_variables.vector_size);
  TEST_EQ_INT(2, lambda2->free_variables.vector_size);  
  schemerlicht_string* sit = schemerlicht_vector_begin(&lambda1->free_variables, schemerlicht_string);
  schemerlicht_string* sit_end = schemerlicht_vector_end(&lambda1->free_variables, schemerlicht_string);
  TEST_EQ_STRING("x", sit->string_ptr);
#if 0
  printf("lam1 free vars: ");
  for (; sit != sit_end; ++sit)
    {
    printf("%s  ", sit->string_ptr);
    }
  printf("\n");
#endif
  sit = schemerlicht_vector_begin(&lambda2->free_variables, schemerlicht_string);
  sit_end = schemerlicht_vector_end(&lambda2->free_variables, schemerlicht_string);
  TEST_EQ_STRING("x", sit->string_ptr);
  TEST_EQ_STRING("y", (sit+1)->string_ptr);
#if 0
  printf("lam2 free vars: ");
  for (; sit != sit_end; ++sit)
    {
    printf("%s  ", sit->string_ptr);
    }
  printf("\n");  
#endif
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_closure_conversion_1()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(let ([x 5]) (lambda (y) (lambda () (+ x y))))");
  schemerlicht_program prog = make_program(ctxt, &tokens);  
  schemerlicht_single_begin_conversion(ctxt, &prog);  
  schemerlicht_free_variable_analysis(ctxt, &prog);
  schemerlicht_closure_conversion(ctxt, &prog);
  schemerlicht_string res = schemerlicht_dump(ctxt, &prog, 1);
  TEST_EQ_STRING("( let ( [ x 5 ] ) ( begin ( closure ( lambda ( #%self1 y ) ( begin ( closure ( lambda ( #%self0 ) ( begin ( + ( closure-ref #%self0 1 ) ( closure-ref #%self0 2 ) ) ) ) ( closure-ref #%self1 1 ) y ) ) ) x ) ) ) ", res.string_ptr);
  schemerlicht_string_destroy(ctxt, &res);  
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_closure_conversion_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(lambda (x y z) (let ([ f (lambda (a b) (+ (* a x) (8 b y)))]) (- (f 1 2) (f 3 4))))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_free_variable_analysis(ctxt, &prog);
  schemerlicht_closure_conversion(ctxt, &prog);
  schemerlicht_string res = schemerlicht_dump(ctxt, &prog, 1);
  TEST_EQ_STRING("( lambda ( #%self1 x y z ) ( begin ( let ( [ f ( closure ( lambda ( #%self0 a b ) ( begin ( + ( * a ( closure-ref #%self0 1 ) ) ( 8 b ( closure-ref #%self0 2 ) ) ) ) ) x y ) ] ) ( begin ( - ( f 1 2 ) ( f 3 4 ) ) ) ) ) ) ", res.string_ptr);
  schemerlicht_string_destroy(ctxt, &res);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_call_cc()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "(define call/cc (lambda(k f) (f k (lambda(dummy-k result) (k result)))))");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);
  schemerlicht_lambda_to_let_conversion(ctxt, &prog);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_free_variable_analysis(ctxt, &prog);
  schemerlicht_closure_conversion(ctxt, &prog);
  schemerlicht_string res = schemerlicht_dump(ctxt, &prog, 1);
  TEST_EQ_STRING("( set! call/cc ( lambda ( #%self1 k f ) ( begin ( f k ( closure ( lambda ( #%self0 dummy-k result ) ( begin ( ( closure-ref #%self0 1 ) result ) ) ) k ) ) ) ) ) ", res.string_ptr);
  schemerlicht_string_destroy(ctxt, &res);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_quote_collect_1()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, "'r 'g 'b 'g");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_vector quotes = schemerlicht_quote_collection(ctxt, &prog);
  TEST_EQ_INT(3, quotes.vector_size);
  if (quotes.vector_size == 3)
    {
    schemerlicht_string* s0 = schemerlicht_vector_at(&quotes, 0, schemerlicht_string);
    schemerlicht_string* s1 = schemerlicht_vector_at(&quotes, 1, schemerlicht_string);
    schemerlicht_string* s2 = schemerlicht_vector_at(&quotes, 2, schemerlicht_string);
    TEST_EQ_STRING("b", s0->string_ptr);
    TEST_EQ_STRING("g", s1->string_ptr);
    TEST_EQ_STRING("r", s2->string_ptr);
    }
  schemerlicht_quote_collection_destroy(ctxt, &quotes);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_quote_conversion_aux(const char* script, const char* expected)
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_vector quotes = schemerlicht_quote_collection(ctxt, &prog);  
  schemerlicht_quote_conversion(ctxt, &prog, &quotes);
  schemerlicht_string res = schemerlicht_dump(ctxt, &prog, 1);
  TEST_EQ_STRING(expected, res.string_ptr);
  schemerlicht_string_destroy(ctxt, &res);
  schemerlicht_quote_collection_destroy(ctxt, &quotes);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_quote_conversion()
  {
  test_quote_conversion_aux("'0", "( begin ( set! #%q0 0 ) #%q0 ) ");
  test_quote_conversion_aux("(quote 0)", "( begin ( set! #%q0 0 ) #%q0 ) ");
  test_quote_conversion_aux("(begin (quote 0) (quote 0))", "( begin ( set! #%q0 0 ) #%q0 #%q0 ) ");
  test_quote_conversion_aux("'1", "( begin ( set! #%q0 1 ) #%q0 ) ");
  test_quote_conversion_aux("(quote -1)", "( begin ( set! #%q0 -1 ) #%q0 ) ");
  test_quote_conversion_aux("(quote 3.14)", "( begin ( set! #%q0 3.140000 ) #%q0 ) ");
  test_quote_conversion_aux("(quote \"Jan\")", "( begin ( set! #%q0 \"Jan\" ) #%q0 ) ");
  test_quote_conversion_aux("(quote #t)", "( begin ( set! #%q0 #t ) #%q0 ) ");
  test_quote_conversion_aux("(quote #f)", "( begin ( set! #%q0 #f ) #%q0 ) ");
  test_quote_conversion_aux("(quote #\\a)", "( begin ( set! #%q0 #\\97 ) #%q0 ) ");
  test_quote_conversion_aux("(quote ())", "( begin ( set! #%q0 () ) #%q0 ) ");
  test_quote_conversion_aux("(quote (1 2 3))", "( begin ( set! #%q0 ( cons 1 ( cons 2 ( cons 3 () ) ) ) ) #%q0 ) ");
  test_quote_conversion_aux("(quote (1 2 3 4 5 6 7 8))", "( begin ( set! #%q0 ( cons 1 ( cons 2 ( cons 3 ( cons 4 ( cons 5 ( cons 6 ( cons 7 ( cons 8 () ) ) ) ) ) ) ) ) ) #%q0 ) ");
  test_quote_conversion_aux("(quote #(1 2 3 4 5 6 7 8))", "( begin ( set! #%q0 ( vector 1 2 3 4 5 6 7 8 ) ) #%q0 ) ");
  test_quote_conversion_aux("(quote (a b c))", "( begin ( set! #%q0 ( cons ( string->symbol \"a\" ) ( cons ( string->symbol \"b\" ) ( cons ( string->symbol \"c\" ) () ) ) ) ) #%q0 ) ");
  test_quote_conversion_aux("'(foo foo foo foo foo foo foo foo foo foo foo)", "( begin ( set! #%q0 ( cons ( string->symbol \"foo\" ) ( cons ( string->symbol \"foo\" ) ( cons ( string->symbol \"foo\" ) ( cons ( string->symbol \"foo\" ) ( cons ( string->symbol \"foo\" ) ( cons ( string->symbol \"foo\" ) ( cons ( string->symbol \"foo\" ) ( cons ( string->symbol \"foo\" ) ( cons ( string->symbol \"foo\" ) ( cons ( string->symbol \"foo\" ) ( cons ( string->symbol \"foo\" ) () ) ) ) ) ) ) ) ) ) ) ) ) #%q0 ) ");
  }

static void test_alpha_conversion_aux(const char* script, const char* expected)
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);  
  schemerlicht_alpha_conversion(ctxt, &prog);
  schemerlicht_string res = schemerlicht_dump(ctxt, &prog, 1);
  TEST_EQ_STRING(expected, res.string_ptr);
  schemerlicht_string_destroy(ctxt, &res);  
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_alpha_conversion()
  {
  test_alpha_conversion_aux("(let ([x 5][y 6]) x)", "( let ( [ %x_0 5 ] [ %y_1 6 ] ) ( begin %x_0 ) ) ");
  test_alpha_conversion_aux("(define var (let ([x 5][y 6]) x))", "( define var ( let ( [ %x_0 5 ] [ %y_1 6 ] ) ( begin %x_0 ) ) ) ");
  }

static void test_alpha_naming()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_string original;
  schemerlicht_string_init(ctxt, &original, "myvar");
  schemerlicht_string alpha = schemerlicht_make_alpha_name(ctxt, &original, 123);
  schemerlicht_string original2 = schemerlicht_get_original_name_from_alpha(ctxt, &alpha);
  TEST_EQ_STRING("%myvar_123", alpha.string_ptr);
  TEST_EQ_STRING(original.string_ptr, original2.string_ptr);
  schemerlicht_string_destroy(ctxt, &original2);
  schemerlicht_string_destroy(ctxt, &original);
  schemerlicht_string_destroy(ctxt, &alpha);
  schemerlicht_close(ctxt);
  }

static void test_constant_propagation_aux(const char* script, const char* expected)
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);  
  schemerlicht_constant_propagation(ctxt, &prog);
  schemerlicht_string res = schemerlicht_dump(ctxt, &prog, 1);
  TEST_EQ_STRING(expected, res.string_ptr);
  schemerlicht_string_destroy(ctxt, &res);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_constant_propagation()
  {
  test_constant_propagation_aux("(let ((x 5)) (let ((y x)) (+ y y)))", "( + 5 5 ) ");
  }

static void test_constant_folding_aux(const char* script, const char* expected)
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  schemerlicht_constant_folding(ctxt, &prog);
  schemerlicht_string res = schemerlicht_dump(ctxt, &prog, 1);
  TEST_EQ_STRING(expected, res.string_ptr);
  schemerlicht_string_destroy(ctxt, &res);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_constant_folding()
  {
  test_constant_folding_aux("(if #t 1 2)", "1 ");
  test_constant_folding_aux("(if #f 1 2)", "2 ");
  test_constant_folding_aux("(if (and #f (f 2)) 123 (g 3))", "( g 3 ) ");

  test_constant_folding_aux("(if (if (flonum? x) #t #f) (a) (b))", "( if ( flonum? x ) ( a ) ( b ) ) ");

  test_constant_folding_aux("(if (> 3 2) 1 1)", "1 ");
  test_constant_folding_aux("(if (> 3 2) 2.5 2.5)", "2.500000 ");
  test_constant_folding_aux("(if (> 3 2) #\\a #\\a)", "#\\97 ");
  test_constant_folding_aux("(if (> 3 2) #t #t)", "#t ");
  test_constant_folding_aux("(if (> 3 2) #f #f)", "#f ");
  test_constant_folding_aux("(if (> 3 2) () ())", "() ");
  test_constant_folding_aux("(if (> 3 2) \"foo\" \"foo\")", "\"foo\" ");  


  test_constant_folding_aux("(+ 5 7)", "12 ");
  test_constant_folding_aux("(+ 5 7 9 10)", "31 ");
  test_constant_folding_aux("(+ 5.2 7.3)", "12.500000 ");
  test_constant_folding_aux("(- 5)", "-5 ");
  test_constant_folding_aux("(/ (* 3 (- (+ 23 9) 20.0) 1.5) 2)", "27.000000 ");
  }

static void test_full_conversion_aux(const char* script, const char* expected)
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_preprocess(ctxt, &prog);
  schemerlicht_string res = schemerlicht_dump(ctxt, &prog, 1);
  TEST_EQ_STRING(expected, res.string_ptr);
  schemerlicht_string_destroy(ctxt, &res);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void test_full_conversion()
  {
  test_full_conversion_aux("(define (my-iota n) (do ((n n (- n 1)) (list '() (cons (- n 1) list))) ((zero? n) list)))", "( let ( [ #%k0 ( set! #%q0 () ) ] ) ( begin ( halt #%k0 ) ) ) ( let ( [ #%k1 ( lambda ( #%self1 #%k2 %n_0 ) ( begin ( let ( [ #%%loop_1 #undefined ] ) ( begin ( let ( [ %loop_1 ( vector #%%loop_1 ) ] ) ( let ( [ %#%t0_4 ( closure ( lambda ( #%self0 #%k9 %n_2 %list_3 ) ( begin ( if ( zero? %n_2 ) ( begin ( #%k9 %list_3 ) ) ( begin ( let ( [ #%k10 ( vector-ref ( closure-ref #%self0 1 ) 0 ) ] ) ( begin ( #%k10 #%k9 ( - %n_2 1 ) ( cons ( - %n_2 1 ) %list_3 ) ) ) ) ) ) ) ) %loop_1 ) ] ) ( begin ( let ( [ #%k7 ( vector-set! %loop_1 0 %#%t0_4 ) ] ) ( begin ( let ( [ #%k3 ( vector-ref %loop_1 0 ) ] ) ( begin ( #%k3 #%k2 %n_0 #%q0 ) ) ) ) ) ) ) ) ) ) ) ) ] ) ( begin ( let ( [ #%k0 ( set! my-iota #%k1 ) ] ) ( begin ( halt #%k0 ) ) ) ) ) ");

  //test_full_conversion_aux("(define(filter-and-fix predicate-fn update-fn list) (let loop((list list)) (cond((null? list) '()) ((predicate-fn (car list)) (cons(update-fn(car list)) (loop(cdr list)))) (else (loop(cdr list))))))", "");
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
  test_convert_define_7();
  test_convert_define_8();
  test_convert_define_9();
  simplify_to_core_conversion_and();
  simplify_to_core_conversion_and_2();
  simplify_to_core_conversion_and_3();
  simplify_to_core_conversion_or();
  simplify_to_core_conversion_or_2();
  simplify_to_core_conversion_or_3();
  simplify_to_core_conversion_letrec();
  simplify_to_core_conversion_let_star();
  simplify_to_core_conversion_named_let();
  simplify_to_core_conversion_case();
  simplify_to_core_conversion_cond();
  simplify_to_core_conversion_cond_2();
  simplify_to_core_conversion_do();
  simplify_to_core_conversion_do_2();
  simplify_to_core_conversion_when();
  simplify_to_core_conversion_unless();
  simplify_to_core_conversion_delay();
  tail_call_analysis();
  tail_call_analysis_2();
  test_cps();
  test_cps_2();
  test_quasiquote_conversion();
  test_lambda_to_let_conversion();
  test_find_assignable_variables();
  test_assignable_variable_conversion();
  test_assignable_variable_conversion_2();
  test_assignable_variable_conversion_3();
  test_assignable_variable_conversion_4();
  test_free_var_analysis();
  test_closure_conversion_1();
  test_closure_conversion_2();
  test_quote_collect_1();
  test_quote_conversion();
  test_call_cc();
  test_alpha_conversion();
  test_alpha_naming();
  test_constant_propagation();
  test_constant_folding();
  test_full_conversion();
  }