#include "svisitor.h"
#include "smemory.h"

static int previsit_program(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_program* p)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(p);
  return 1;
  }
static void postvisit_program(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_program* p)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(p);
  }
static int previsit_expression(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  return 1;
  }
static void postvisit_expression(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static int previsit_begin(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  return 1;
  }
static void postvisit_begin(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static void visit_fixnum(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static void visit_flonum(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static void visit_nil(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static void visit_string(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static void visit_symbol(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static void visit_true(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static void visit_false(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static void visit_nop(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static void visit_character(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static void visit_variable(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static void visit_quote(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static int previsit_primcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  return 1;
  }
static void postvisit_primcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static int previsit_funcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  return 1;
  }
static void postvisit_funcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static int previsit_foreigncall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  return 1;
  }
static void postvisit_foreigncall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static int previsit_if(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  return 1;
  }
static void postvisit_if(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static int previsit_set(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  return 1;
  }
static void postvisit_set(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static int previsit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  return 1;
  }
static void postvisit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static int previsit_let(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  return 1;
  }
static int previsit_let_binding(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_let_binding* b)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(b);
  return 1;
  }
static void postvisit_let_binding(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_let_binding* b)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(b);
  }
static void postvisit_let_bindings(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static void postvisit_let(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }

static schemerlicht_visitor_entry make_entry(schemerlicht_expression* e, enum schemerlicht_visitor_entry_type t)
  {
  schemerlicht_visitor_entry entry;
  entry.binding = NULL;
  entry.expr = e;
  entry.type = t;
  return entry;
  }

static schemerlicht_visitor_entry make_entry_binding(schemerlicht_let_binding* b, enum schemerlicht_visitor_entry_type t)
  {
  schemerlicht_visitor_entry entry;
  entry.binding = b;
  entry.expr = NULL;
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
  v->previsit_program = previsit_program;
  v->postvisit_program = postvisit_program;
  v->previsit_expression = previsit_expression;
  v->postvisit_expression = postvisit_expression;
  v->visit_fixnum = visit_fixnum;
  v->visit_flonum = visit_flonum;
  v->visit_nil = visit_nil;
  v->visit_string = visit_string;
  v->visit_symbol = visit_symbol;
  v->visit_true = visit_true;
  v->visit_false = visit_false;
  v->visit_nop = visit_nop;
  v->visit_character = visit_character;
  v->visit_variable = visit_variable;
  v->visit_quote = visit_quote;
  v->previsit_primcall = previsit_primcall;
  v->postvisit_primcall = postvisit_primcall;
  v->previsit_funcall = previsit_funcall;
  v->postvisit_funcall = postvisit_funcall;
  v->previsit_foreigncall = previsit_foreigncall;
  v->postvisit_foreigncall = postvisit_foreigncall;
  v->previsit_begin = previsit_begin;
  v->postvisit_begin = postvisit_begin;
  v->previsit_if = previsit_if;
  v->postvisit_if = postvisit_if;
  v->previsit_set = previsit_set;
  v->postvisit_set = postvisit_set;
  v->previsit_lambda = previsit_lambda;
  v->postvisit_lambda = postvisit_lambda;
  v->previsit_let = previsit_let;
  v->previsit_let_binding = previsit_let_binding;
  v->postvisit_let_binding = postvisit_let_binding;
  v->postvisit_let_bindings = postvisit_let_bindings;
  v->postvisit_let = postvisit_let;

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
    if (vis->previsit_expression(ctxt, vis, e->expr))
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
    vis->postvisit_expression(ctxt, vis, e->expr);
    break;
    }
    case SCHEMERLICHT_VISITOR_BEGIN_PRE:
    {
    if (vis->previsit_begin(ctxt, vis, e->expr))
      {
      schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_BEGIN_POST), schemerlicht_visitor_entry);
      schemerlicht_vector* arg = &(e->expr->expr.beg.arguments);
      schemerlicht_expression* expr_it = schemerlicht_vector_begin(arg, schemerlicht_expression);
      schemerlicht_expression* expr_it_end = schemerlicht_vector_end(arg, schemerlicht_expression);
      schemerlicht_expression* expr_rit = expr_it_end - 1;
      schemerlicht_expression* expr_rit_end = expr_it - 1;
      for (; expr_rit != expr_rit_end; --expr_rit) // IMPORTANT: brackets necessary, as schemerlicht_vector_push_back is a C macro
        {
        schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(expr_rit, SCHEMERLICHT_VISITOR_EXPRESSION_PRE), schemerlicht_visitor_entry);
        }
      }
    break;
    }
    case SCHEMERLICHT_VISITOR_BEGIN_POST:
    {
    vis->postvisit_begin(ctxt, vis, e->expr);
    break;
    }
    case SCHEMERLICHT_VISITOR_LITERAL:
    {
    schemerlicht_literal* lit = &(e->expr->expr.lit);
    switch (lit->type)
      {
      case schemerlicht_type_fixnum:
        vis->visit_fixnum(ctxt, vis, e->expr);
        break;
      case schemerlicht_type_flonum:
        vis->visit_flonum(ctxt, vis, e->expr);
        break;
      case schemerlicht_type_character:
        vis->visit_character(ctxt, vis, e->expr);
        break;
      case schemerlicht_type_false:
        if (vis->visit_false != NULL)
          vis->visit_false(ctxt, vis, e->expr);
        break;
      case schemerlicht_type_true:
        vis->visit_true(ctxt, vis, e->expr);
        break;
      case schemerlicht_type_string:
        vis->visit_string(ctxt, vis, e->expr);
        break;
      case schemerlicht_type_symbol:
        vis->visit_symbol(ctxt, vis, e->expr);
        break;
      }
    break;
    }
    case SCHEMERLICHT_VISITOR_NOP:
    {
    vis->visit_nop(ctxt, vis, e->expr);
    break;
    }
    case SCHEMERLICHT_VISITOR_VARIABLE:
    {
    vis->visit_variable(ctxt, vis, e->expr);
    break;
    }
    case SCHEMERLICHT_VISITOR_QUOTE:
    {
    vis->visit_quote(ctxt, vis, e->expr);
    break;
    }
    case SCHEMERLICHT_VISITOR_FUNCALL_PRE:
    {
    if (vis->previsit_funcall(ctxt, vis, e->expr))
      {
      schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_FUNCALL_POST), schemerlicht_visitor_entry);
      schemerlicht_vector* arg = &(e->expr->expr.funcall.arguments);
      schemerlicht_expression* expr_it = schemerlicht_vector_begin(arg, schemerlicht_expression);
      schemerlicht_expression* expr_it_end = schemerlicht_vector_end(arg, schemerlicht_expression);
      schemerlicht_expression* expr_rit = expr_it_end - 1;
      schemerlicht_expression* expr_rit_end = expr_it - 1;
      for (; expr_rit != expr_rit_end; --expr_rit) // IMPORTANT: brackets necessary, as schemerlicht_vector_push_back is a C macro
        {
        schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(expr_rit, SCHEMERLICHT_VISITOR_EXPRESSION_PRE), schemerlicht_visitor_entry);
        }
      arg = &(e->expr->expr.funcall.fun);
      expr_it = schemerlicht_vector_begin(arg, schemerlicht_expression);
      expr_it_end = schemerlicht_vector_end(arg, schemerlicht_expression);
      expr_rit = expr_it_end - 1;
      expr_rit_end = expr_it - 1;
      for (; expr_rit != expr_rit_end; --expr_rit) // IMPORTANT: brackets necessary, as schemerlicht_vector_push_back is a C macro
        {
        schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(expr_rit, SCHEMERLICHT_VISITOR_EXPRESSION_PRE), schemerlicht_visitor_entry);
        }
      }
    break;
    }
    case SCHEMERLICHT_VISITOR_FUNCALL_POST:
    {
    vis->postvisit_funcall(ctxt, vis, e->expr);
    break;
    }
    case SCHEMERLICHT_VISITOR_PRIMCALL_PRE:
    {
    if (vis->previsit_primcall(ctxt, vis, e->expr))
      {
      schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_PRIMCALL_POST), schemerlicht_visitor_entry);
      schemerlicht_vector* arg = &(e->expr->expr.prim.arguments);
      schemerlicht_expression* expr_it = schemerlicht_vector_begin(arg, schemerlicht_expression);
      schemerlicht_expression* expr_it_end = schemerlicht_vector_end(arg, schemerlicht_expression);
      schemerlicht_expression* expr_rit = expr_it_end - 1;
      schemerlicht_expression* expr_rit_end = expr_it - 1;
      for (; expr_rit != expr_rit_end; --expr_rit) // IMPORTANT: brackets necessary, as schemerlicht_vector_push_back is a C macro
        {
        schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(expr_rit, SCHEMERLICHT_VISITOR_EXPRESSION_PRE), schemerlicht_visitor_entry);
        }     
      }
    break;
    }
    case SCHEMERLICHT_VISITOR_PRIMCALL_POST:
    {
    vis->postvisit_primcall(ctxt, vis, e->expr);
    break;
    }
    case SCHEMERLICHT_VISITOR_FOREIGNCALL_PRE:
    {
    if (vis->previsit_foreigncall(ctxt, vis, e->expr))
      {
      schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_FOREIGNCALL_POST), schemerlicht_visitor_entry);
      schemerlicht_vector* arg = &(e->expr->expr.foreign.arguments);
      schemerlicht_expression* expr_it = schemerlicht_vector_begin(arg, schemerlicht_expression);
      schemerlicht_expression* expr_it_end = schemerlicht_vector_end(arg, schemerlicht_expression);
      schemerlicht_expression* expr_rit = expr_it_end - 1;
      schemerlicht_expression* expr_rit_end = expr_it - 1;
      for (; expr_rit != expr_rit_end; --expr_rit) // IMPORTANT: brackets necessary, as schemerlicht_vector_push_back is a C macro
        {
        schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(expr_rit, SCHEMERLICHT_VISITOR_EXPRESSION_PRE), schemerlicht_visitor_entry);
        }
      }
    break;
    }
    case SCHEMERLICHT_VISITOR_FOREIGNCALL_POST:
    {
    vis->postvisit_foreigncall(ctxt, vis, e->expr);
    break;
    }
    case SCHEMERLICHT_VISITOR_IF_PRE:
    {
    if (vis->previsit_if(ctxt, vis, e->expr))
      {
      schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_IF_POST), schemerlicht_visitor_entry);
      schemerlicht_vector* arg = &(e->expr->expr.i.arguments);
      schemerlicht_expression* expr_it = schemerlicht_vector_begin(arg, schemerlicht_expression);
      schemerlicht_expression* expr_it_end = schemerlicht_vector_end(arg, schemerlicht_expression);
      schemerlicht_expression* expr_rit = expr_it_end - 1;
      schemerlicht_expression* expr_rit_end = expr_it - 1;
      for (; expr_rit != expr_rit_end; --expr_rit) // IMPORTANT: brackets necessary, as schemerlicht_vector_push_back is a C macro
        {
        schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(expr_rit, SCHEMERLICHT_VISITOR_EXPRESSION_PRE), schemerlicht_visitor_entry);
        }
      }
    break;
    }
    case SCHEMERLICHT_VISITOR_IF_POST:
    {
    vis->postvisit_if(ctxt, vis, e->expr);
    break;
    }
    case SCHEMERLICHT_VISITOR_LAMBDA_PRE:
    {
    if (vis->previsit_lambda(ctxt, vis, e->expr))
      {
      schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_LAMBDA_POST), schemerlicht_visitor_entry);
      schemerlicht_vector* arg = &(e->expr->expr.lambda.body);
      schemerlicht_expression* expr_it = schemerlicht_vector_begin(arg, schemerlicht_expression);
      schemerlicht_expression* expr_it_end = schemerlicht_vector_end(arg, schemerlicht_expression);
      schemerlicht_expression* expr_rit = expr_it_end - 1;
      schemerlicht_expression* expr_rit_end = expr_it - 1;
      for (; expr_rit != expr_rit_end; --expr_rit) // IMPORTANT: brackets necessary, as schemerlicht_vector_push_back is a C macro
        {
        schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(expr_rit, SCHEMERLICHT_VISITOR_EXPRESSION_PRE), schemerlicht_visitor_entry);
        }
      }
    break;
    }
    case SCHEMERLICHT_VISITOR_LAMBDA_POST:
    {
    vis->postvisit_lambda(ctxt, vis, e->expr);
    break;
    }
    case SCHEMERLICHT_VISITOR_SET_PRE:
    {
    if (vis->previsit_set(ctxt, vis, e->expr))
      {
      schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_SET_POST), schemerlicht_visitor_entry);
      schemerlicht_vector* arg = &(e->expr->expr.set.value);
      schemerlicht_expression* expr_it = schemerlicht_vector_begin(arg, schemerlicht_expression);
      schemerlicht_expression* expr_it_end = schemerlicht_vector_end(arg, schemerlicht_expression);
      schemerlicht_expression* expr_rit = expr_it_end - 1;
      schemerlicht_expression* expr_rit_end = expr_it - 1;
      for (; expr_rit != expr_rit_end; --expr_rit) // IMPORTANT: brackets necessary, as schemerlicht_vector_push_back is a C macro
        {
        schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(expr_rit, SCHEMERLICHT_VISITOR_EXPRESSION_PRE), schemerlicht_visitor_entry);
        }
      }
    break;
    }
    case SCHEMERLICHT_VISITOR_SET_POST:
    {
    vis->postvisit_set(ctxt, vis, e->expr);
    break;
    }
    case SCHEMERLICHT_VISITOR_LET_PRE:
    {
    if (vis->previsit_let(ctxt, vis, e->expr))
      {
      schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_LET_BINDINGS_POST), schemerlicht_visitor_entry);
      schemerlicht_vector* arg = &(e->expr->expr.let.bindings);
      schemerlicht_let_binding* expr_it = schemerlicht_vector_begin(arg, schemerlicht_let_binding);
      schemerlicht_let_binding* expr_it_end = schemerlicht_vector_end(arg, schemerlicht_let_binding);
      schemerlicht_let_binding* expr_rit = expr_it_end - 1;
      schemerlicht_let_binding* expr_rit_end = expr_it - 1;
      for (; expr_rit != expr_rit_end; --expr_rit) // IMPORTANT: brackets necessary, as schemerlicht_vector_push_back is a C macro
        {

        schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry_binding(expr_rit, SCHEMERLICHT_VISITOR_LET_BINDING_PRE), schemerlicht_visitor_entry);
        }
      }
    break;
    }
    case SCHEMERLICHT_VISITOR_LET_BINDING_PRE:
    {
    if (vis->previsit_let_binding(ctxt, vis, e->binding))
      {
      schemerlicht_visitor_entry e1 = make_entry_binding(e->binding, SCHEMERLICHT_VISITOR_LET_BINDING_POST);
      schemerlicht_vector_push_back(ctxt, &(vis->v), e1, schemerlicht_visitor_entry);
      schemerlicht_visitor_entry e2 = make_entry(&(e->binding->binding_expr), SCHEMERLICHT_VISITOR_EXPRESSION_PRE);
      schemerlicht_vector_push_back(ctxt, &(vis->v), e2, schemerlicht_visitor_entry);
      }
    break;
    }
    case SCHEMERLICHT_VISITOR_LET_BINDING_POST:
    {
    vis->postvisit_let_binding(ctxt, vis, e->binding);
    break;
    }
    case SCHEMERLICHT_VISITOR_LET_BINDINGS_POST:
    {
    vis->postvisit_let_bindings(ctxt, vis, e->expr);
    schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(e->expr, SCHEMERLICHT_VISITOR_LET_POST), schemerlicht_visitor_entry);
    schemerlicht_vector* arg = &(e->expr->expr.let.body);
    schemerlicht_expression* expr_it = schemerlicht_vector_begin(arg, schemerlicht_expression);
    schemerlicht_expression* expr_it_end = schemerlicht_vector_end(arg, schemerlicht_expression);
    schemerlicht_expression* expr_rit = expr_it_end - 1;
    schemerlicht_expression* expr_rit_end = expr_it - 1;
    for (; expr_rit != expr_rit_end; --expr_rit) // IMPORTANT: brackets necessary, as schemerlicht_vector_push_back is a C macro
      {
      schemerlicht_vector_push_back(ctxt, &(vis->v), make_entry(expr_rit, SCHEMERLICHT_VISITOR_EXPRESSION_PRE), schemerlicht_visitor_entry);
      }
    break;
    }
    case SCHEMERLICHT_VISITOR_LET_POST:
    {
    vis->postvisit_let(ctxt, vis, e->expr);
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
  if (vis->previsit_program(ctxt, vis, p))
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
    vis->postvisit_program(ctxt, vis, p);
    }
  }