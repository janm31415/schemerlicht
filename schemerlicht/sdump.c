#include "sdump.h"
#include "smemory.h"

schemerlicht_dump_visitor* schemerlicht_dump_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_dump_visitor* v = schemerlicht_new(ctxt, schemerlicht_dump_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  schemerlicht_string_init(ctxt, &(v->s), "");
  return v;
  }

void schemerlicht_dump_visitor_free(schemerlicht_context* ctxt, schemerlicht_dump_visitor* v)
  {
  if (v)
    {
    schemerlicht_string_destroy(ctxt, &(v->s));
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }