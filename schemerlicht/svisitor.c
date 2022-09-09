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
  v->visit_nil = NULL;
  v->visit_string = NULL;
  v->visit_symbol = NULL;
  v->visit_true = NULL;
  v->visit_false = NULL;
  v->visit_nop = NULL;
  v->visit_character = NULL;
  v->visit_variable = NULL;
  v->visit_quote = NULL;
  v->previsit_primcall = NULL;
  v->postvisit_primcall = NULL;
  v->previsit_funcall = NULL;
  v->postvisit_funcall = NULL;
  v->previsit_foreigncall = NULL;
  v->postvisit_foreigncall = NULL;
  v->previsit_begin = NULL;
  v->postvisit_begin = NULL;
  v->previsit_if = NULL;
  v->postvisit_if = NULL;
  v->previsit_set = NULL;
  v->postvisit_set = NULL;
  v->previsit_lambda = NULL;
  v->postvisit_lambda = NULL;
  v->previsit_let = NULL;
  v->visit_let_bindings = NULL;
  v->postvisit_let = NULL;

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
        case schemerlicht_type_variable:
          schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_VARIABLE), schemerlicht_visitor_entry);
          break;
        case schemerlicht_type_nop:
          schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_NOP), schemerlicht_visitor_entry);
          break;
        case schemerlicht_type_primitive_call:
          schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_PRIMCALL_PRE), schemerlicht_visitor_entry);
          break;
        case schemerlicht_type_foreign_call:
          schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_FOREIGNCALL_PRE), schemerlicht_visitor_entry);
          break;
        case schemerlicht_type_if:
          schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_IF_PRE), schemerlicht_visitor_entry);
          break;
        case schemerlicht_type_cond:
          schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_COND_PRE), schemerlicht_visitor_entry);
          break;
        case schemerlicht_type_do:
          schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_DO_PRE), schemerlicht_visitor_entry);
          break;
        case schemerlicht_type_case:
          schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_CASE_PRE), schemerlicht_visitor_entry);
          break;
        case schemerlicht_type_quote:
          schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_QUOTE), schemerlicht_visitor_entry);
          break;
        case schemerlicht_type_let:
          schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_LET_PRE), schemerlicht_visitor_entry);
          break;
        case schemerlicht_type_lambda:
          schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_LAMBDA_PRE), schemerlicht_visitor_entry);
          break;
        case schemerlicht_type_funcall:
          schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_FUNCALL_PRE), schemerlicht_visitor_entry);
          break;
        case schemerlicht_type_begin:
          schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_BEGIN_PRE), schemerlicht_visitor_entry);
          break;
        case schemerlicht_type_set:
          schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_SET_PRE), schemerlicht_visitor_entry);
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
    case SCHEMERLICHT_VISITOR_BEGIN_PRE:
    {
    if (vis->previsit_begin != NULL && vis->previsit_begin(ctxt, vis, e->expr))
      {
      schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_BEGIN_POST), schemerlicht_visitor_entry);
      schemerlicht_vector* arg = &(e->expr->expr.beg.arguments);
      schemerlicht_expression* expr_it = schemerlicht_vector_begin(arg, schemerlicht_expression);
      schemerlicht_expression* expr_it_end = schemerlicht_vector_end(arg, schemerlicht_expression);
      schemerlicht_expression* expr_rit = expr_it_end - 1;
      schemerlicht_expression* expr_rit_end = expr_it - 1;
      for (; expr_rit != expr_rit_end; --expr_rit)
        schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(expr_rit, SCHEMERLICHT_VISITOR_EXPRESSION_PRE), schemerlicht_visitor_entry);
      }
    break;
    }
    case SCHEMERLICHT_VISITOR_BEGIN_POST:
    {
    if (vis->postvisit_begin)
      vis->postvisit_begin(ctxt, vis, e->expr);
    break;
    }
    case SCHEMERLICHT_VISITOR_LITERAL:
    {
    schemerlicht_literal* lit = &(e->expr->expr.lit);
    switch (lit->type)
      {
      case schemerlicht_type_fixnum:
        if (vis->visit_fixnum != NULL)
          vis->visit_fixnum(ctxt, vis, e->expr);
        break;
      case schemerlicht_type_flonum:
        if (vis->visit_flonum != NULL)
          vis->visit_flonum(ctxt, vis, e->expr);
        break;
      case schemerlicht_type_character:
        if (vis->visit_character != NULL)
          vis->visit_character(ctxt, vis, e->expr);
        break;
      case schemerlicht_type_false:
        if (vis->visit_false != NULL)
          vis->visit_false(ctxt, vis, e->expr);
        break;
      case schemerlicht_type_true:
        if (vis->visit_true != NULL)
          vis->visit_true(ctxt, vis, e->expr);
        break;
      case schemerlicht_type_string:
        if (vis->visit_string != NULL)
          vis->visit_string(ctxt, vis, e->expr);
        break;
      case schemerlicht_type_symbol:
        if (vis->visit_symbol != NULL)
          vis->visit_symbol(ctxt, vis, e->expr);
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