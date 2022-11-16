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
    printf("%s\n", it->message.string_ptr);
    }
  if (ctxt->number_of_syntax_errors > 0)
    {
    schemerlicht_error_report* it = schemerlicht_vector_begin(&ctxt->syntax_error_reports, schemerlicht_error_report);
    printf("%s\n", it->message.string_ptr);
    }
  if (ctxt->number_of_runtime_errors > 0)
    {
    schemerlicht_error_report* it = schemerlicht_vector_begin(&ctxt->runtime_error_reports, schemerlicht_error_report);
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
  //test_compile_aux(ctxt, "5", "(import 'srfi-6) 5");
  test_compile_aux(ctxt, "<port>: \"input-string\"", "(import 'srfi-6) (define p (open-input-string \"21 34\"))");
  //test_compile_aux(ctxt, "<port>: \"input-string\"", "(load \"C:/_Dev/schemerlicht/schemerlicht/scm/srfi/srfi6.scm\") (define p (open-input-string \"21 34\"))");
  }

void run_all_module_tests()
  {
  schemerlicht_context* ctxt = schemerlicht_open(2048);  
  schemerlicht_compile_callcc(ctxt);
  schemerlicht_compile_r5rs(ctxt);
  schemerlicht_compile_input_output(ctxt);  
  schemerlicht_compile_modules(ctxt, SCHEMERLICHT_MODULES_PATH);

  test_srfi6(ctxt);

  schemerlicht_close(ctxt);
  }