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
#include "preprocess.h"

void schemerlicht_compile_callcc(schemerlicht_context* ctxt)
  {  
  char* script = "( define %call/cc ( lambda ( %self1 %k_0 %f_1 ) ( begin ( %f_1 %k_0 ( closure ( lambda ( %self0 %dummy-k_2 %result_3 ) ( begin ( ( closure-ref %self0 1 ) %result_3 ) ) ) %k_0 ) ) ) ) )\n";  
  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);  
  schemerlicht_vector callcc = schemerlicht_compile_program(ctxt, &prog);
  schemerlicht_run_program(ctxt, &callcc);
  schemerlicht_assert(schemerlicht_vector_begin(&ctxt->stack, schemerlicht_object)->type == schemerlicht_object_type_lambda);
  schemerlicht_function* call_cc_fun = cast(schemerlicht_function*, schemerlicht_vector_begin(&ctxt->stack, schemerlicht_object)->value.ptr);
  ctxt->callcc_fun = call_cc_fun;
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_compiled_program_register(ctxt, &callcc);
  char* dynamic_wind_script = "(define %dynamic-winds '())\n"
  "  (define dynamic-wind (lambda (before thunk after)\n"
  "    (before)\n"
  "    (set! %dynamic-winds (cons (cons before after) %dynamic-winds))\n"
  "    (let((ans(thunk)))\n"
  "      (set! %dynamic-winds (cdr %dynamic-winds))\n"
  "      (after)\n"
  "      ans)))\n"
  "  (define %dynamic-unwind #f)\n"
  "  (define call-with-current-continuation\n"
  "  (let((oldcc %call/cc))\n"
  "    (lambda(proc)\n"
  "      (let((winds %dynamic-winds))\n"
  "        (oldcc\n"
  "        (lambda(cont)\n"
  "          (proc(lambda(c2)\n"
  "            (%dynamic-unwind winds(fx- (length %dynamic-winds) (length winds)))\n"
  "            (cont c2)))))))))\n"
  "  (set! %dynamic-unwind (lambda(to delta)\n"
  "    (cond((eq? %dynamic-winds to))\n"
  "      ((fx<? delta 0)\n"
  "        (%dynamic-unwind (cdr to) (fx+ delta 1))\n"
  "        ((car (car to)))\n"
  "        (set! %dynamic-winds to))\n"
  "      (else\n"
  "        (let((after(cdr(car %dynamic-winds))))\n"
  "          (set! %dynamic-winds(cdr %dynamic-winds))\n"
  "          (after)\n"
  "          (%dynamic-unwind to(fx- delta 1)))))))\n"
  "  (define call/cc call-with-current-continuation)";
  tokens = schemerlicht_script2tokens(ctxt, dynamic_wind_script);
  prog = make_program(ctxt, &tokens);
  schemerlicht_preprocess_internal_libs(ctxt, &prog);
  schemerlicht_vector dynamic_wind = schemerlicht_compile_program(ctxt, &prog);
  schemerlicht_run_program(ctxt, &dynamic_wind);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_compiled_program_register(ctxt, &dynamic_wind);
  }
