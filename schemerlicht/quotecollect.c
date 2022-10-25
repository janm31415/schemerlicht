#include "quotecollect.h"
#include "context.h"
#include "map.h"
#include "visitor.h"
#include "reader.h"
#include "string.h"
#include "stringvec.h"

#include <string.h>

typedef struct schemerlicht_quote_collection_visitor
  {
  schemerlicht_vector collected_quotes;
  schemerlicht_visitor* visitor;
  } schemerlicht_quote_collection_visitor;

static void visit_quote(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_quote_collection_visitor* vis = (schemerlicht_quote_collection_visitor*)(v->impl);
  schemerlicht_string s;
  schemerlicht_string* ptr_s = &s;
  schemerlicht_string_init(ctxt, ptr_s, "");
  schemerlicht_dump_cell_to_string(ctxt, &e->expr.quote.arg, ptr_s);
  schemerlicht_string* sit = schemerlicht_vector_begin(&vis->collected_quotes, schemerlicht_string);
  schemerlicht_string* sit_end = schemerlicht_vector_end(&vis->collected_quotes, schemerlicht_string);
  sit = schemerlicht_lower_bound(sit, sit_end, ptr_s);
  if (sit == sit_end || strcmp(sit->string_ptr, s.string_ptr) != 0)
    {
    schemerlicht_string* s_end = ptr_s + 1;
    schemerlicht_vector_insert(ctxt, &vis->collected_quotes, &sit, &ptr_s, &s_end, schemerlicht_string);
    }
  else
    {
    schemerlicht_string_destroy(ctxt, ptr_s);
    }
  }

static schemerlicht_quote_collection_visitor* schemerlicht_quote_collection_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_quote_collection_visitor* v = schemerlicht_new(ctxt, schemerlicht_quote_collection_visitor);
  schemerlicht_vector_init(ctxt, &v->collected_quotes, schemerlicht_string);
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

schemerlicht_vector schemerlicht_quote_collection(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_vector collected_quotes;
  schemerlicht_quote_collection_visitor* v = schemerlicht_quote_collection_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, program);
  collected_quotes = v->collected_quotes;
  schemerlicht_quote_collection_visitor_free(ctxt, v);
  return collected_quotes;
  }

void schemerlicht_quote_collection_destroy(schemerlicht_context* ctxt, schemerlicht_vector* collected_quotes)
  {
  schemerlicht_string* it = schemerlicht_vector_begin(collected_quotes, schemerlicht_string);
  schemerlicht_string* it_end = schemerlicht_vector_end(collected_quotes, schemerlicht_string);
  for (; it != it_end; ++it)
    {
    schemerlicht_string_destroy(ctxt, it);
    }
  schemerlicht_vector_destroy(ctxt, collected_quotes);
  }