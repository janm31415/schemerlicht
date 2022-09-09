#include "svisitor.h"
#include "smemory.h"

static schemerlicht_visitory_entry make_entry(schemerlicht_expression* e, enum schemerlicht_visitor_entry_type t)
  {
  schemerlicht_visitory_entry entry;
  entry.expr = e;
  entry.type = t;
  return entry;
  }

static void destroy(schemerlicht_context* ctxt, schemerlicht_visitor* v)
  {
  if (v != NULL)
    {
    schemerlicht_vector_destroy(ctxt, &(v->v));
    schemerlicht_delete(ctxt, v);    
    }
  }

schemerlicht_visitor* schemerlicht_visitor_new(schemerlicht_context* ctxt, void* impl)
  {
  schemerlicht_visitor* v = schemerlicht_new(ctxt, schemerlicht_visitor);
  v->impl = impl;
  v->previsit_program = NULL;
  v->postvisit_program = NULL;
  v->previsit_expression = NULL;
  v->postvisit_expression = NULL;
  v->visit_fixnum = NULL;
  v->visit_flonum = NULL;
  v->destroy = destroy;
  schemerlicht_vector_init(ctxt, &(v->v), schemerlicht_visitory_entry);
  return v;
  }

void visit(schemerlicht_context* ctxt, schemerlicht_visitory_entry entry, schemerlicht_visitor* vis)
  {
  UNUSED(ctxt);
  UNUSED(entry);
  UNUSED(vis);
  }