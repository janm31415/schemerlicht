#include "tailcall.h"

#include "visitor.h"

typedef struct schemerlicht_init_tail_call_visitor
  {
  schemerlicht_visitor* visitor;
  } schemerlicht_init_tail_call_visitor;

typedef struct schemerlicht_analyse_tail_call_visitor
  {
  schemerlicht_visitor* visitor;
  } schemerlicht_analyse_tail_call_visitor;

static void unset_tail_call(schemerlicht_expression* e)
  {
  switch (e->type)
    {
    case schemerlicht_type_literal:
    {
    switch (e->expr.lit.type)
      {
      case schemerlicht_type_fixnum: e->expr.lit.lit.fx.tail_position = 0; break;
      case schemerlicht_type_flonum: e->expr.lit.lit.fl.tail_position = 0; break;
      case schemerlicht_type_nil: e->expr.lit.lit.nil.tail_position = 0; break;
      case schemerlicht_type_string: e->expr.lit.lit.str.tail_position = 0; break;
      case schemerlicht_type_true: e->expr.lit.lit.t.tail_position = 0; break;
      case schemerlicht_type_false: e->expr.lit.lit.f.tail_position = 0; break;
      case schemerlicht_type_character: e->expr.lit.lit.ch.tail_position = 0; break;
      case schemerlicht_type_symbol: e->expr.lit.lit.sym.tail_position = 0; break;
      }
    break;
    }
    case schemerlicht_type_variable: e->expr.var.tail_position = 0; break;
    case schemerlicht_type_nop: e->expr.nop.tail_position = 0; break;
    case schemerlicht_type_primitive_call: e->expr.prim.tail_position = 0; break;
    case schemerlicht_type_foreign_call: e->expr.foreign.tail_position = 0; break;
    case schemerlicht_type_if: e->expr.i.tail_position = 0; break;
    case schemerlicht_type_cond: e->expr.cond.tail_position = 0; break;
    case schemerlicht_type_do: e->expr.d.tail_position = 0; break;
    case schemerlicht_type_case: e->expr.cas.tail_position = 0; break;
    case schemerlicht_type_quote: e->expr.quote.tail_position = 0; break;
    case schemerlicht_type_let: e->expr.let.tail_position = 0; break;
    case schemerlicht_type_lambda: e->expr.lambda.tail_position = 0; break;
    case schemerlicht_type_funcall: e->expr.funcall.tail_position = 0; break;
    case schemerlicht_type_begin: e->expr.beg.tail_position = 0; break;
    case schemerlicht_type_set: e->expr.set.tail_position = 0; break;
    }
  }


static void set_tail_call(schemerlicht_expression* e)
  {
  switch (e->type)
    {
    case schemerlicht_type_literal:
    {
    switch (e->expr.lit.type)
      {
      case schemerlicht_type_fixnum: e->expr.lit.lit.fx.tail_position = 1; break;
      case schemerlicht_type_flonum: e->expr.lit.lit.fl.tail_position = 1; break;
      case schemerlicht_type_nil: e->expr.lit.lit.nil.tail_position = 1; break;
      case schemerlicht_type_string: e->expr.lit.lit.str.tail_position = 1; break;
      case schemerlicht_type_true: e->expr.lit.lit.t.tail_position = 1; break;
      case schemerlicht_type_false: e->expr.lit.lit.f.tail_position = 1; break;
      case schemerlicht_type_character: e->expr.lit.lit.ch.tail_position = 1; break;
      case schemerlicht_type_symbol: e->expr.lit.lit.sym.tail_position = 1; break;
      }
    break;
    }
    case schemerlicht_type_variable: e->expr.var.tail_position = 1; break;
    case schemerlicht_type_nop: e->expr.nop.tail_position = 1; break;
    case schemerlicht_type_primitive_call: e->expr.prim.tail_position = 1; break;
    case schemerlicht_type_foreign_call: e->expr.foreign.tail_position = 1; break;
    case schemerlicht_type_if: e->expr.i.tail_position = 1; break;
    case schemerlicht_type_cond: e->expr.cond.tail_position = 1; break;
    case schemerlicht_type_do: e->expr.d.tail_position = 1; break;
    case schemerlicht_type_case: e->expr.cas.tail_position = 1; break;
    case schemerlicht_type_quote: e->expr.quote.tail_position = 1; break;
    case schemerlicht_type_let: e->expr.let.tail_position = 1; break;
    case schemerlicht_type_lambda: e->expr.lambda.tail_position = 1; break;
    case schemerlicht_type_funcall: e->expr.funcall.tail_position = 1; break;
    case schemerlicht_type_begin: e->expr.beg.tail_position = 1; break;
    case schemerlicht_type_set: e->expr.set.tail_position = 1; break;
    }
  }

static int get_tail_call(schemerlicht_expression* e)
  {
  switch (e->type)
    {
    case schemerlicht_type_literal:
    {
    switch (e->expr.lit.type)
      {
      case schemerlicht_type_fixnum: return e->expr.lit.lit.fx.tail_position;
      case schemerlicht_type_flonum: return e->expr.lit.lit.fl.tail_position;
      case schemerlicht_type_nil: return e->expr.lit.lit.nil.tail_position;
      case schemerlicht_type_string: return e->expr.lit.lit.str.tail_position;
      case schemerlicht_type_true: return e->expr.lit.lit.t.tail_position;
      case schemerlicht_type_false: return e->expr.lit.lit.f.tail_position;
      case schemerlicht_type_character: return e->expr.lit.lit.ch.tail_position;
      case schemerlicht_type_symbol: return e->expr.lit.lit.sym.tail_position;
      }
    break;
    }
    case schemerlicht_type_variable: return e->expr.var.tail_position;
    case schemerlicht_type_nop: return e->expr.nop.tail_position;
    case schemerlicht_type_primitive_call: return e->expr.prim.tail_position;
    case schemerlicht_type_foreign_call: return e->expr.foreign.tail_position;
    case schemerlicht_type_if: return e->expr.i.tail_position;
    case schemerlicht_type_cond: return e->expr.cond.tail_position;
    case schemerlicht_type_do: return e->expr.d.tail_position;
    case schemerlicht_type_case: return e->expr.cas.tail_position;
    case schemerlicht_type_quote: return e->expr.quote.tail_position;
    case schemerlicht_type_let: return e->expr.let.tail_position;
    case schemerlicht_type_lambda: return e->expr.lambda.tail_position;
    case schemerlicht_type_funcall: return e->expr.funcall.tail_position;
    case schemerlicht_type_begin: return e->expr.beg.tail_position;
    case schemerlicht_type_set: return e->expr.set.tail_position;
    }
  return -1;
  }

static void init_tail_call_postvisit_expression(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  unset_tail_call(e);
  }

static schemerlicht_init_tail_call_visitor* schemerlicht_init_tail_call_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_init_tail_call_visitor* v = schemerlicht_new(ctxt, schemerlicht_init_tail_call_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->postvisit_expression = init_tail_call_postvisit_expression;  
  return v;
  }

static void schemerlicht_init_tail_call_visitor_free(schemerlicht_context* ctxt, schemerlicht_init_tail_call_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }

static int previsit_program(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_program* p)
  {
  if (p->expressions.vector_size == 0)
    return 0;
  if (p->expressions.vector_size == 1)
    {
    schemerlicht_expression* expr = schemerlicht_vector_at(&p->expressions, 0, schemerlicht_expression);
    if (expr->type == schemerlicht_type_begin)
      {
      schemerlicht_expression* it = schemerlicht_vector_begin(&expr->expr.beg.arguments, schemerlicht_expression);
      schemerlicht_expression* it_end = schemerlicht_vector_end(&expr->expr.beg.arguments, schemerlicht_expression);
      for (; it != it_end; ++it)
        set_tail_call(it);
      }
    else
      {
      set_tail_call(schemerlicht_vector_back(&p->expressions, schemerlicht_expression));
      }
    }
  else
    {
    set_tail_call(schemerlicht_vector_back(&p->expressions, schemerlicht_expression));
    }
  return 1;
  }
static int previsit_let(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  if (e->expr.let.tail_position)
    set_tail_call(schemerlicht_vector_at(&e->expr.let.body, 0, schemerlicht_expression));
  return 1;
  }
static int previsit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  set_tail_call(schemerlicht_vector_at(&e->expr.lambda.body, 0, schemerlicht_expression));
  return 1;
  }
static int previsit_if(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  if (e->expr.i.tail_position)
    {
    if (e->expr.i.arguments.vector_size >= 2)     
      set_tail_call(schemerlicht_vector_at(&e->expr.i.arguments, 1, schemerlicht_expression));
    if (e->expr.i.arguments.vector_size >= 3)
      set_tail_call(schemerlicht_vector_at(&e->expr.i.arguments, 2, schemerlicht_expression));
    }
  return 1;
  }
static int previsit_begin(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  if (e->expr.beg.tail_position && e->expr.beg.arguments.vector_size>0)
    {
    set_tail_call(schemerlicht_vector_back(&e->expr.beg.arguments, schemerlicht_expression));
    }
  return 1;
  }
static schemerlicht_analyse_tail_call_visitor* schemerlicht_analyse_tail_call_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_analyse_tail_call_visitor* v = schemerlicht_new(ctxt, schemerlicht_analyse_tail_call_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);  
  v->visitor->previsit_program = previsit_program;
  v->visitor->previsit_let = previsit_let;
  v->visitor->previsit_lambda = previsit_lambda;
  v->visitor->previsit_if = previsit_if;
  v->visitor->previsit_begin = previsit_begin;
  return v;
  }

static void schemerlicht_analyse_tail_call_visitor_free(schemerlicht_context* ctxt, schemerlicht_analyse_tail_call_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }

void schemerlicht_tail_call_analysis(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_init_tail_call_visitor* v = schemerlicht_init_tail_call_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_init_tail_call_visitor_free(ctxt, v);
  schemerlicht_analyse_tail_call_visitor* v2 = schemerlicht_analyse_tail_call_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v2->visitor, program);
  schemerlicht_analyse_tail_call_visitor_free(ctxt, v2);
  }



typedef struct schemerlicht_has_tail_call_visitor
  {
  schemerlicht_visitor* visitor;
  int so_far_all_tail_calls;
  } schemerlicht_has_tail_call_visitor;

static int has_tail_call_previsit_expression(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  schemerlicht_has_tail_call_visitor* vv = cast(schemerlicht_has_tail_call_visitor*, v->impl);
  if (vv->so_far_all_tail_calls)
    {
    if (e->type == schemerlicht_type_funcall)
      vv->so_far_all_tail_calls = get_tail_call(e);
    }
  return vv->so_far_all_tail_calls;
  }

static schemerlicht_has_tail_call_visitor* schemerlicht_has_tail_call_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_has_tail_call_visitor* v = schemerlicht_new(ctxt, schemerlicht_has_tail_call_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->previsit_expression = has_tail_call_previsit_expression;
  return v;
  }

static void schemerlicht_has_tail_call_visitor_free(schemerlicht_context* ctxt, schemerlicht_has_tail_call_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }

int schemerlicht_program_only_has_tail_calls(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_has_tail_call_visitor* v = schemerlicht_has_tail_call_visitor_new(ctxt);
  v->so_far_all_tail_calls = 1;
  schemerlicht_visit_program(ctxt, v->visitor, program);
  int res = v->so_far_all_tail_calls;
  schemerlicht_has_tail_call_visitor_free(ctxt, v);
  return res;
  }

void schemerlicht_expression_tail_call_analysis(schemerlicht_context* ctxt, schemerlicht_expression* e)
  {
  schemerlicht_init_tail_call_visitor* v = schemerlicht_init_tail_call_visitor_new(ctxt);
  schemerlicht_visit_expression(ctxt, v->visitor, e);
  schemerlicht_init_tail_call_visitor_free(ctxt, v);
  schemerlicht_analyse_tail_call_visitor* v2 = schemerlicht_analyse_tail_call_visitor_new(ctxt);
  schemerlicht_visit_expression(ctxt, v2->visitor, e);
  schemerlicht_analyse_tail_call_visitor_free(ctxt, v2);
  }

int schemerlicht_expression_only_has_tail_calls(schemerlicht_context* ctxt, schemerlicht_expression* e)
  {
  schemerlicht_has_tail_call_visitor* v = schemerlicht_has_tail_call_visitor_new(ctxt);
  v->so_far_all_tail_calls = 1;
  schemerlicht_visit_expression(ctxt, v->visitor, e);
  int res = v->so_far_all_tail_calls;
  schemerlicht_has_tail_call_visitor_free(ctxt, v);
  return res;
  }