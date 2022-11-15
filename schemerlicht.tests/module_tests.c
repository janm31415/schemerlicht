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

void run_all_module_tests()
  {
  schemerlicht_context* ctxt = schemerlicht_open(2048);
  schemerlicht_compile_callcc(ctxt);
  schemerlicht_compile_r5rs(ctxt);
  schemerlicht_compile_input_output(ctxt);
  
  //schemerlicht_compile_modules(ctxt, "");

  schemerlicht_close(ctxt);
  }