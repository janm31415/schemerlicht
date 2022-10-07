#include "freevaranalysis.h"
#include "visitor.h"
#include "context.h"
#include "vector.h"
#include "stringvec.h"

typedef struct schemerlicht_free_variable_analysis_visitor
  {
  schemerlicht_visitor* visitor;
  schemerlicht_vector assignable_variables;
  } schemerlicht_free_variable_analysis_visitor;

static schemerlicht_free_variable_analysis_visitor* schemerlicht_free_variable_analysis_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_free_variable_analysis_visitor* v = schemerlicht_new(ctxt, schemerlicht_free_variable_analysis_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
 
  return v;
  }

static void schemerlicht_free_variable_analysis_visitor_free(schemerlicht_context* ctxt, schemerlicht_free_variable_analysis_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);    
    schemerlicht_delete(ctxt, v);
    }
  }

void schemerlicht_free_variable_analysis(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_free_variable_analysis_visitor* v = schemerlicht_free_variable_analysis_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_free_variable_analysis_visitor_free(ctxt, v);
  }