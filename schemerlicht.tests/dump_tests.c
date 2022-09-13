#include "dump_tests.h"
#include "schemerlicht/dump.h"
#include "schemerlicht/context.h"
#include "schemerlicht/token.h"
#include "test_assert.h"
#include "token_tests.h"

static void dump_fixnum()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "5");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);

  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);

  TEST_EQ_STRING("5 ", dumper->s.string_ptr);

  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void dump_begin()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(begin 5 3.14)");
  schemerlicht_program prog = make_program(ctxt, &tokens);
  
  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);

  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);
  
  TEST_EQ_STRING("( begin 5 3.140000 ) ", dumper->s.string_ptr);

  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void dump_primcall()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(+ 1 2)");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);

  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);

  TEST_EQ_STRING("( + 1 2 ) ", dumper->s.string_ptr);

  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }


static void dump_if()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(if (> 2 3) 1.5  8.9)");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);

  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);

  TEST_EQ_STRING("( if ( > 2 3 ) 1.500000 8.900000 ) ", dumper->s.string_ptr);

  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void dump_variable()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(define x 7)");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);

  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);

  TEST_EQ_STRING("( define x 7 ) ", dumper->s.string_ptr);

  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void dump_set()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(set! x (+ 1 2))");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);

  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);

  TEST_EQ_STRING("( set! x ( + 1 2 ) ) ", dumper->s.string_ptr);

  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void dump_lambda()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(lambda (x) (* x x))");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);

  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);

  TEST_EQ_STRING("( lambda ( x ) ( begin ( * x x ) ) ) ", dumper->s.string_ptr);

  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void dump_lambda_nested_begins()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(lambda (x) ( begin ( begin ( begin (+ 5 7 ) ) ) ) )");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);

  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);

  TEST_EQ_STRING("( lambda ( x ) ( begin ( + 5 7 ) ) ) ", dumper->s.string_ptr);

  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void dump_let()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(let ((x 2) (y 3)) (+ x y))");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);

  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);

  TEST_EQ_STRING("( let ( [ x 2 ] [ y 3 ] ) ( begin ( + x y ) ) ) ", dumper->s.string_ptr);

  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void dump_let_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(let ([f (lambda (x y) (fx+ x y))] [g (lambda (x) (fx+ x 12))]) (app f 16 (app f (app g 0) (fx+ 1 (app g 0)))))");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);

  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);

  TEST_EQ_STRING("( let ( [ f ( lambda ( x y ) ( begin ( fx+ x y ) ) ) ] [ g ( lambda ( x ) ( begin ( fx+ x 12 ) ) ) ] ) ( begin ( app f 16 ( app f ( app g 0 ) ( fx+ 1 ( app g 0 ) ) ) ) ) ) ", dumper->s.string_ptr);

  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void dump_let_3()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(let ([x 5]) x) ");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);

  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);

  TEST_EQ_STRING("( let ( [ x 5 ] ) ( begin x ) ) ", dumper->s.string_ptr);

  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void dump_let_4()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(let () 12)");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);

  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);

  TEST_EQ_STRING("( let ( ) ( begin 12 ) ) ", dumper->s.string_ptr);

  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void dump_foreign()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(foreign-call load-simulation addr \"13\")");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);

  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);

  TEST_EQ_STRING("( foreign-call load-simulation addr \"13\" ) ", dumper->s.string_ptr);

  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void dump_quote()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "'(a b c)");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);

  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);

  TEST_EQ_STRING("( quote (a b c) ) ", dumper->s.string_ptr);

  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void dump_quasiquote()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "`(a b c)");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);

  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);

  TEST_EQ_STRING("( quasiquote (a b c) ) ", dumper->s.string_ptr);

  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void dump_quasiquote_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "`(list ,(+ 1 2) 4)");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);

  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);

  TEST_EQ_STRING("( quasiquote (list (unquote (+ 1 2)) 4) ) ", dumper->s.string_ptr);

  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

static void dump_quasiquote_3()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "`(a ,(+ 1 2) ,@(map abs '(4 -5 6)) b)");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);

  schemerlicht_visit_program(ctxt, dumper->visitor, &prog);

  TEST_EQ_STRING("( quasiquote (a (unquote (+ 1 2)) (unquote-splicing (map abs (quote (4 -5 6)))) b) ) ", dumper->s.string_ptr);

  schemerlicht_dump_visitor_free(ctxt, dumper);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

void run_all_dump_tests()
  {
  dump_fixnum();
  dump_begin();
  dump_primcall();
  dump_if();
  dump_variable();
  dump_set();
  dump_lambda();
  dump_lambda_nested_begins();
  dump_let();
  dump_let_2();
  dump_let_3();
  dump_let_4();
  dump_foreign();
  dump_quote();
  dump_quasiquote();
  dump_quasiquote_2();
  dump_quasiquote_3();
  }