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

/*
static int init_tail_call_previsit_begin(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.beg.tail_position = 0;
  return 1;
  }
static void init_tail_call_visit_fixnum(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.lit.lit.fx.tail_position = 0;
  }
static void init_tail_call_visit_flonum(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.lit.lit.fl.tail_position = 0;
  }
static void init_tail_call_visit_nil(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.lit.lit.nil.tail_position = 0;
  }
static void init_tail_call_visit_string(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.lit.lit.str.tail_position = 0;
  }
static void init_tail_call_visit_symbol(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.lit.lit.sym.tail_position = 0;
  }
static void init_tail_call_visit_true(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.lit.lit.t.tail_position = 0;
  }
static void init_tail_call_visit_false(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.lit.lit.f.tail_position = 0;
  }
static void init_tail_call_visit_nop(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.nop.tail_position = 0;
  }
static void init_tail_call_visit_character(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.lit.lit.ch.tail_position = 0;
  }
static void init_tail_call_visit_variable(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.var.tail_position = 0;
  }
static void init_tail_call_visit_quote(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.quote.tail_position = 0;
  }
static int init_tail_call_previsit_primcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.prim.tail_position = 0;
  return 1;
  }
static int init_tail_call_previsit_funcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.funcall.tail_position = 0;
  return 1;
  }
static int init_tail_call_previsit_foreigncall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.foreign.tail_position = 0;
  return 1;
  }
static int init_tail_call_previsit_if(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.i.tail_position = 0;
  return 1;
  }
static int init_tail_call_previsit_set(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.set.tail_position = 0;
  return 1;
  }
static int init_tail_call_previsit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.lambda.tail_position = 0;
  return 1;
  }
static int init_tail_call_previsit_let(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.let.tail_position = 0;
  return 1;
  }
static int init_tail_call_previsit_case(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.cas.tail_position = 0;
  return 1;
  }
static int init_tail_call_previsit_cond(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.cond.tail_position = 0;
  return 1;
  }
static int init_tail_call_previsit_do(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  e->expr.d.tail_position = 0;
  return 1;
  }
  */

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
  /*
  v->visitor->visit_fixnum = init_tail_call_visit_fixnum;
  v->visitor->visit_flonum = init_tail_call_visit_flonum;
  v->visitor->visit_nil = init_tail_call_visit_nil;
  v->visitor->visit_string = init_tail_call_visit_string;
  v->visitor->visit_symbol = init_tail_call_visit_symbol;
  v->visitor->visit_true = init_tail_call_visit_true;
  v->visitor->visit_false = init_tail_call_visit_false;
  v->visitor->visit_nop = init_tail_call_visit_nop;
  v->visitor->visit_character = init_tail_call_visit_character;
  v->visitor->visit_variable = init_tail_call_visit_variable;
  v->visitor->visit_quote = init_tail_call_visit_quote;
  v->visitor->previsit_primcall = init_tail_call_previsit_primcall;
  v->visitor->previsit_funcall = init_tail_call_previsit_funcall;
  v->visitor->previsit_foreigncall = init_tail_call_previsit_foreigncall;
  v->visitor->previsit_begin = init_tail_call_previsit_begin;
  v->visitor->previsit_if = init_tail_call_previsit_if;
  v->visitor->previsit_set = init_tail_call_previsit_set;
  v->visitor->previsit_lambda = init_tail_call_previsit_lambda;
  v->visitor->previsit_let = init_tail_call_previsit_let;
  v->visitor->previsit_do = init_tail_call_previsit_do;
  v->visitor->previsit_case = init_tail_call_previsit_case;
  v->visitor->previsit_cond = init_tail_call_previsit_cond;
  */
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


/*
A procedure call is called a tail call if it occurs in a tail position.
A tail position is defined recursively as follows:

- the body of a procedure is in tail position
- if a let expression is in tail position, then the body
  of the let is in tail position
- if the condition expression (if test conseq altern) is in tail
  position, then the conseq and altern branches are also in tail position.
- all other expressions are not in tail position.
*/

static schemerlicht_analyse_tail_call_visitor* schemerlicht_analyse_tail_call_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_analyse_tail_call_visitor* v = schemerlicht_new(ctxt, schemerlicht_analyse_tail_call_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);  
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