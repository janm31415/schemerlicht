#include "compiler_tests.h"
#include "test_assert.h"
#include "token_tests.h"
#include "schemerlicht/parser.h"
#include "schemerlicht/context.h"
#include "schemerlicht/token.h"
#include "schemerlicht/compiler.h"

static void test_compile_fixnum()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "5");
  schemerlicht_program prog = make_program(ctxt, &tokens);

  schemerlicht_function func = schemerlicht_compile_expression(ctxt, schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression));

  schemerlicht_function_destroy(ctxt, &func);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_close(ctxt);
  }

void run_all_compiler_tests()
  {
  test_compile_fixnum();
  }