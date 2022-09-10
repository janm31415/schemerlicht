#include "dump_tests.h"
#include "schemerlicht/sdump.h"
#include "schemerlicht/scontext.h"
#include "schemerlicht/stoken.h"
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

void run_all_dump_tests()
  {
  dump_fixnum();
  dump_begin();
  dump_primcall();
  dump_if();
  dump_variable();
  dump_set();
  dump_lambda();
  }