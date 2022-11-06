#include "callcc.h"
#include "parser.h"
#include "context.h"
#include "token.h"
#include "compiler.h"
#include "vm.h"
#include "simplifyconv.h"
#include "begconv.h"
#include "globdef.h"
#include "defconv.h"
#include "alpha.h"
#include "lambdatolet.h"
#include "assignablevarconv.h"
#include "freevaranalysis.h"
#include "closure.h"
#include "dump.h"

#include <string.h>

static schemerlicht_vector script2tokens(schemerlicht_context* ctxt, const char* script)
  {
  schemerlicht_stream str;
  schemerlicht_stream_init(ctxt, &str, 10);
  schemerlicht_memsize len = cast(schemerlicht_memsize, strlen(script));
  schemerlicht_stream_write(ctxt, &str, script, len, 0);
  schemerlicht_stream_rewind(&str);
  schemerlicht_vector tokens = tokenize(ctxt, &str);
  schemerlicht_stream_close(ctxt, &str);
  return tokens;
  }

schemerlicht_function* schemerlicht_compile_callcc(schemerlicht_context* ctxt)
  {  
  char* script = "( define call/cc ( lambda ( %self1 %k_0 %f_1 ) ( begin ( %f_1 %k_0 ( closure ( lambda ( %self0 %dummy-k_2 %result_3 ) ( begin ( ( closure-ref %self0 1 ) %result_3 ) ) ) %k_0 ) ) ) ) )\n"
    "(define call-with-current-continuation call/cc)";
  schemerlicht_vector tokens = script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);  
  schemerlicht_function* callccfunc = schemerlicht_compile_expression(ctxt, schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression));
  schemerlicht_run(ctxt, callccfunc);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  return callccfunc;
  }