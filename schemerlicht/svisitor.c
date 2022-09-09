#include "svisitor.h"
#include "smemory.h"

static schemerlicht_visitor_entry make_entry(schemerlicht_expression* e, enum schemerlicht_visitor_entry_type t)
  {
  schemerlicht_visitor_entry entry;
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
  schemerlicht_vector_init(ctxt, &(v->v), schemerlicht_visitor_entry);
  return v;
  }

static void visit_entry(schemerlicht_context* ctxt, schemerlicht_visitor* vis, schemerlicht_visitor_entry* e)
  {
  switch (e->type)
    {
    case SCHEMERLICHT_VISITOR_EXPRESSION_PRE:
    {
    if (vis->previsit_expression != NULL && vis->previsit_expression(ctxt, vis, e->expr))
      {
      schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_EXPRESSION_POST), schemerlicht_visitor_entry);
      switch (e->expr->type)
        {
        case schemerlicht_type_literal:
          schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_LITERAL), schemerlicht_visitor_entry);
          break;
        default:
          break;
        }
      }
    break;
    }
    case SCHEMERLICHT_VISITOR_EXPRESSION_POST:
    {
    if (vis->postvisit_expression)
      vis->postvisit_expression(ctxt, vis, e->expr);
    break;
    }
    case SCHEMERLICHT_VISITOR_LITERAL:
    {
    schemerlicht_literal* lit = &(e->expr->expr.lit);
    switch (lit->type)
      {
      case schemerlicht_type_fixnum:
        if (vis->visit_fixnum != NULL)
          vis->visit_fixnum(ctxt, vis, &(lit->lit.fx));
        break;
      case schemerlicht_type_flonum:
        if (vis->visit_flonum != NULL)
          vis->visit_flonum(ctxt, vis, &(lit->lit.fl));
        break;
      }
    break;
    }
    }
  }

static void visit(schemerlicht_context* ctxt, schemerlicht_visitor* vis)
  {
  while (vis->v.vector_size > 0)
    {
    schemerlicht_visitor_entry* e = schemerlicht_vector_back(ctxt, &(vis->v), schemerlicht_visitor_entry);
    schemerlicht_vector_pop_back(&(vis->v));
    visit_entry(ctxt, vis, e);
    }
  }

void schemerlicht_visit_expression(schemerlicht_context* ctxt, schemerlicht_visitor* vis, schemerlicht_expression* expr)
  {
  schemerlicht_visitor_entry e = make_entry(expr, SCHEMERLICHT_VISITOR_EXPRESSION_PRE);
  schemerlicht_vector_push_back(ctxt, &(vis->v), e, schemerlicht_visitor_entry);
  visit(ctxt, vis);
  }

void schemerlicht_visit_program(schemerlicht_context* ctxt, schemerlicht_visitor* vis, schemerlicht_program* p)
  {
  if (vis->previsit_program != NULL && vis->previsit_program(ctxt, vis, p))
    {
    schemerlicht_expression* expr_it = schemerlicht_vector_begin(&p->expressions, schemerlicht_expression);
    schemerlicht_expression* expr_it_end = schemerlicht_vector_end(&p->expressions, schemerlicht_expression);
    schemerlicht_expression* expr_rit = expr_it_end - 1;
    schemerlicht_expression* expr_rit_end = expr_it - 1;
    for (; expr_rit != expr_rit_end; --expr_rit)
      {
      schemerlicht_visitor_entry e = make_entry(expr_rit, SCHEMERLICHT_VISITOR_EXPRESSION_PRE);
      schemerlicht_vector_push_back(ctxt, &(vis->v), e, schemerlicht_visitor_entry);
      }
    visit(ctxt, vis);
    if (vis->postvisit_program != NULL)
      vis->postvisit_program(ctxt, vis, p);
    }
  }