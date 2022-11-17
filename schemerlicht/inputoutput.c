#include "inputoutput.h"

#include "parser.h"
#include "context.h"
#include "token.h"
#include "compiler.h"
#include "vm.h"
#include "preprocess.h"

void schemerlicht_compile_input_output(schemerlicht_context* ctxt)
  {
  char* script = "(define (call-with-output-file fname proc)\n"
  "  (let((in(open-output-file fname)))\n"
  "    (call-with-values (lambda() (proc in))\n"
  "      (lambda results\n"
  "      (close-output-port in)\n"
  "        (apply values results)))))\n"
  "  (define(call-with-input-file fname proc)\n"
  "          (let((in(open-input-file fname)))\n"
  "            (call-with-values(lambda() (proc in))\n"
  "              (lambda results\n"
  "              (close-input-port in)\n"
  "                (apply values results)))))\n"  
  "  (define(with-input-from-file fname thunk)\n"
  "    (call-with-input-file fname\n"
  "    (lambda(in)\n"
  "      (let((old standard-input-port))\n"
  "        (dynamic-wind\n"
  "        (lambda() (set! standard-input-port in))\n"
  "          thunk\n"
  "          (lambda() (set! standard-input-port old)))))))\n"  
  "  (define(with-output-to-file fname thunk)\n"
  "    (call-with-output-file fname\n"
  "    (lambda(in)\n"
  "      (let((old standard-output-port))\n"
  "        (dynamic-wind\n"
  "        (lambda() (set! standard-output-port in))\n"
  "          thunk\n"
  "          (lambda() (set! standard-output-port old)))))))";

  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_preprocess_internal_libs(ctxt, &prog);
  schemerlicht_function* inputoutput = schemerlicht_compile_expression(ctxt, schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression));
  schemerlicht_run(ctxt, inputoutput);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_vector_push_back(ctxt, &ctxt->lambdas, inputoutput, schemerlicht_function*);
  }