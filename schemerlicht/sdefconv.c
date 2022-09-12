#include "sdefconv.h"
#include "scontext.h"
#include "svisitor.h"
#include "sbegconv.h"


void schemerlicht_define_conversion(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_remove_nested_begin_expressions(ctxt, program);
  }