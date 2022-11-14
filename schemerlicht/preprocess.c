#include "preprocess.h"
#include "context.h"
#include "simplifyconv.h"
#include "begconv.h"
#include "dump.h"
#include "globdef.h"
#include "defconv.h"
#include "cps.h"
#include "closure.h"
#include "assignablevarconv.h"
#include "lambdatolet.h"
#include "freevaranalysis.h"
#include "quotecollect.h"
#include "quoteconv.h"
#include "quasiquote.h"
#include "alpha.h"
#include "vector.h"
#include "macro.h"
#include "constprop.h"
#include "constfold.h"

void schemerlicht_preprocess(schemerlicht_context* ctxt, schemerlicht_program* prog)
  {
  schemerlicht_expand_macros(ctxt, prog);
  schemerlicht_quasiquote_conversion(ctxt, prog);
  schemerlicht_define_conversion(ctxt, prog);
  schemerlicht_single_begin_conversion(ctxt, prog);
  schemerlicht_simplify_to_core_forms(ctxt, prog);
  schemerlicht_alpha_conversion(ctxt, prog);
  schemerlicht_vector quotes = schemerlicht_quote_collection(ctxt, prog);
  schemerlicht_quote_conversion(ctxt, prog, &quotes);
  schemerlicht_quote_collection_destroy(ctxt, &quotes);
  schemerlicht_global_define_environment_allocation(ctxt, prog);
  schemerlicht_continuation_passing_style(ctxt, prog);
  schemerlicht_lambda_to_let_conversion(ctxt, prog);
  schemerlicht_assignable_variable_conversion(ctxt, prog);
  schemerlicht_constant_propagation(ctxt, prog);
  schemerlicht_free_variable_analysis(ctxt, prog);
  schemerlicht_closure_conversion(ctxt, prog);
  schemerlicht_constant_folding(ctxt, prog);
  }

void schemerlicht_preprocess_internal_libs(schemerlicht_context* ctxt, schemerlicht_program* prog)
  {
  // internal libs do not need all preprocessing steps
  schemerlicht_define_conversion(ctxt, prog);
  schemerlicht_single_begin_conversion(ctxt, prog);
  schemerlicht_simplify_to_core_forms(ctxt, prog);
  schemerlicht_vector quotes = schemerlicht_quote_collection(ctxt, prog);
  schemerlicht_quote_conversion(ctxt, prog, &quotes);
  schemerlicht_quote_collection_destroy(ctxt, &quotes);
  schemerlicht_global_define_environment_allocation(ctxt, prog);
  schemerlicht_continuation_passing_style(ctxt, prog);
  schemerlicht_lambda_to_let_conversion(ctxt, prog);
  schemerlicht_assignable_variable_conversion(ctxt, prog);
  schemerlicht_free_variable_analysis(ctxt, prog);
  schemerlicht_closure_conversion(ctxt, prog);
  }