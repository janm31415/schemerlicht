#include "sdump.h"
#include "smemory.h"

#include <string.h>
#include <stdio.h>

static void visit_fixnum(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  char buffer[80];
  memset(buffer, 0, 80 * sizeof(char));
  sprintf(buffer, "%lld", e->expr.lit.lit.fx.value);
  schemerlicht_string_append_cstr(ctxt, &(d->s), buffer);
  schemerlicht_string_push_back(ctxt, &(d->s), ' ');
  }

static void visit_flonum(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  char buffer[80];
  memset(buffer, 0, 80 * sizeof(char));
  sprintf(buffer, "%f", e->expr.lit.lit.fl.value);
  schemerlicht_string_append_cstr(ctxt, &(d->s), buffer);
  schemerlicht_string_push_back(ctxt, &(d->s), ' ');
  }

static int previsit_begin(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(e);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), "( begin ");
  return 1;
  }

static void postvisit_begin(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(e);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), ") ");
  }

static int previsit_primcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_parsed_primitive_call* p = &(e->expr.prim);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  if (p->as_object)
    {
    schemerlicht_string_append(ctxt, &(d->s), &(p->name));
    schemerlicht_string_push_back(ctxt, &(d->s), ' ');
    return 0;
    }
  schemerlicht_string_append_cstr(ctxt, &(d->s), "( ");
  schemerlicht_string_append(ctxt, &(d->s), &(p->name));
  schemerlicht_string_push_back(ctxt, &(d->s), ' ');
  return 1;
  }

static void postvisit_primcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_parsed_primitive_call* p = &(e->expr.prim);
  if (!p->as_object)
    {
    schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
    schemerlicht_string_append_cstr(ctxt, &(d->s), ") ");
    }
  }

schemerlicht_dump_visitor* schemerlicht_dump_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_dump_visitor* v = schemerlicht_new(ctxt, schemerlicht_dump_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  schemerlicht_string_init(ctxt, &(v->s), "");
  v->visitor->visit_fixnum = visit_fixnum;
  v->visitor->visit_flonum = visit_flonum;
  v->visitor->previsit_begin = previsit_begin;
  v->visitor->postvisit_begin = postvisit_begin;
  v->visitor->previsit_primcall = previsit_primcall;
  v->visitor->postvisit_primcall = postvisit_primcall;
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