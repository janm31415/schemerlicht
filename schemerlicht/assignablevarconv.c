#include "assignablevarconv.h"
#include "visitor.h"
#include "context.h"
#include "vector.h"
#include "map.h"

typedef struct schemerlicht_find_assignable_variables_visitor
  {
  schemerlicht_visitor* visitor;
  schemerlicht_map* assignable_variables;
  } schemerlicht_find_assignable_variables_visitor;

static schemerlicht_find_assignable_variables_visitor* schemerlicht_find_assignable_variables_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_find_assignable_variables_visitor* v = schemerlicht_new(ctxt, schemerlicht_find_assignable_variables_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
 // v->visitor->postvisit_funcall = postvisit_funcall;
  v->assignable_variables = schemerlicht_map_new(ctxt, 0, 8);
  return v;
  }

static void schemerlicht_find_assignable_variables_visitor_free(schemerlicht_context* ctxt, schemerlicht_find_assignable_variables_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_map_free(ctxt, v->assignable_variables);
    schemerlicht_delete(ctxt, v);
    }
  }

void schemerlicht_assignable_variable_conversion(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_find_assignable_variables_visitor* v = schemerlicht_find_assignable_variables_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_find_assignable_variables_visitor_free(ctxt, v);
  }