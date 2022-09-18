#include "quotecollect.h"
#include "context.h"
#include "map.h"
#include "visitor.h"
#include "reader.h"
#include "string.h"

typedef struct schemerlicht_quote_collection_visitor
  {
  schemerlicht_visitor* visitor;
  } schemerlicht_quote_collection_visitor;

static void visit_quote(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "");
  schemerlicht_dump_cell_to_string(ctxt, &e->expr.quote.arg, &s);

  }

static schemerlicht_quote_collection_visitor* schemerlicht_quote_collection_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_quote_collection_visitor* v = schemerlicht_new(ctxt, schemerlicht_quote_collection_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->visit_quote = visit_quote;
  return v;
  }

static void schemerlicht_quote_collection_visitor_free(schemerlicht_context* ctxt, schemerlicht_quote_collection_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }

void schemerlicht_quote_collection(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_quote_collection_visitor* v = schemerlicht_quote_collection_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_quote_collection_visitor_free(ctxt, v);
  }