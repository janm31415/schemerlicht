#include "dump.h"
#include "memory.h"
#include "reader.h"

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
static void visit_nil(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(e);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), "() ");
  }
static void visit_string(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_push_back(ctxt, &(d->s), '"');
  schemerlicht_string_append(ctxt, &(d->s), &(e->expr.lit.lit.str.value));
  schemerlicht_string_append_cstr(ctxt, &(d->s), "\" ");
  }
static void visit_symbol(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append(ctxt, &(d->s), &(e->expr.lit.lit.sym.value));
  schemerlicht_string_push_back(ctxt, &(d->s), ' ');
  }
static void visit_true(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(e);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), "#t ");
  }
static void visit_false(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(e);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), "#f ");
  }
static void visit_nop(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(e);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), "#undefined ");
  }
static void visit_character(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  char ch = e->expr.lit.lit.ch.value;
  char buffer[20];
  memset(buffer, 0, 20 * sizeof(char));
  sprintf(buffer, "%d", (int)ch);
  schemerlicht_string_append_cstr(ctxt, &(d->s), "#\\");
  schemerlicht_string_append_cstr(ctxt, &(d->s), buffer);
  schemerlicht_string_push_back(ctxt, &(d->s), ' ');
  }
static void visit_variable(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append(ctxt, &(d->s), &(e->expr.var.name));
  schemerlicht_string_push_back(ctxt, &(d->s), ' ');
  }
static void visit_quote(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  switch (e->expr.quote.qt)
    {
    case schemerlicht_qt_quote: schemerlicht_string_append_cstr(ctxt, &(d->s), "( quote "); break;
    case schemerlicht_qt_backquote: schemerlicht_string_append_cstr(ctxt, &(d->s), "( quasiquote "); break;
    case schemerlicht_qt_unquote: schemerlicht_string_append_cstr(ctxt, &(d->s), "( unquote "); break;
    case schemerlicht_qt_unquote_splicing: schemerlicht_string_append_cstr(ctxt, &(d->s), "( unquote-splicing "); break;
    }
  schemerlicht_dump_cell_to_string(ctxt, &e->expr.quote.arg, &(d->s));
  schemerlicht_string_append_cstr(ctxt, &(d->s), " ) ");
  }
static int previsit_funcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(e);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), "( ");
  return 1;
  }
static void postvisit_funcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(e);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), ") ");
  }
static int previsit_foreigncall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_parsed_foreign_call* f = &(e->expr.foreign);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), "( ");
  schemerlicht_string_append(ctxt, &(d->s), &(f->name));
  schemerlicht_string_push_back(ctxt, &(d->s), ' ');
  return 1;
  }
static void postvisit_foreigncall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(e);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), ") ");
  }
static int previsit_if(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(e);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), "( if ");
  return 1;
  }
static void postvisit_if(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(e);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), ") ");
  }
static int previsit_set(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_parsed_set* s = &(e->expr.set);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), "( set! ");
  schemerlicht_string_append(ctxt, &(d->s), &(s->name));
  schemerlicht_string_push_back(ctxt, &(d->s), ' ');
  return 1;
  }
static void postvisit_set(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(e);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), ") ");
  }
static int previsit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_parsed_lambda* l = &(e->expr.lambda);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), "( lambda ( ");
  schemerlicht_string* it = schemerlicht_vector_begin(&(l->variables), schemerlicht_string);
  schemerlicht_string* it_end = schemerlicht_vector_end(&(l->variables), schemerlicht_string);
  for (; it != it_end; ++it)
    {
    if (l->variable_arity && (it + 1) == it_end)
      schemerlicht_string_append_cstr(ctxt, &(d->s), ". ");    
    schemerlicht_string_append(ctxt, &(d->s), it);
    schemerlicht_string_push_back(ctxt, &(d->s), ' ');
    }
  schemerlicht_string_append_cstr(ctxt, &(d->s), ") ");
  return 1;
  }
static void postvisit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(e);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), ") ");
  }
static int previsit_let(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_parsed_let* l = &(e->expr.let);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  switch (l->bt)
    {
    case schemerlicht_bt_let: schemerlicht_string_append_cstr(ctxt, &(d->s), "( let "); break;
    case schemerlicht_bt_let_star: schemerlicht_string_append_cstr(ctxt, &(d->s), "( let* "); break;
    case schemerlicht_bt_letrec: schemerlicht_string_append_cstr(ctxt, &(d->s), "( letrec "); break;
    }
  if (l->named_let)
    {
    schemerlicht_string_append(ctxt, &(d->s), &l->let_name);
    schemerlicht_string_append_cstr(ctxt, &(d->s), " ");
    }
  schemerlicht_string_append_cstr(ctxt, &(d->s), "( ");
  return 1;
  }
static int previsit_let_binding(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_let_binding* b)
  {
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), "[ ");
  schemerlicht_string_append(ctxt, &(d->s), &b->binding_name);
  schemerlicht_string_push_back(ctxt, &(d->s), ' ');
  return 1;
  }
static void postvisit_let_binding(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_let_binding* b)
  {
  UNUSED(b);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), "] ");  
  }
static void postvisit_let_bindings(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(e);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), ") ");
  }
static void postvisit_let(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(e);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), ") ");
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
  v->visitor->visit_nil = visit_nil;
  v->visitor->visit_string = visit_string;
  v->visitor->visit_symbol = visit_symbol;
  v->visitor->visit_true = visit_true;
  v->visitor->visit_false = visit_false;
  v->visitor->visit_nop = visit_nop;
  v->visitor->visit_character = visit_character;
  v->visitor->visit_variable = visit_variable;
  v->visitor->visit_quote = visit_quote;
  v->visitor->previsit_funcall = previsit_funcall;
  v->visitor->postvisit_funcall = postvisit_funcall;
  v->visitor->previsit_foreigncall = previsit_foreigncall;
  v->visitor->postvisit_foreigncall = postvisit_foreigncall;
  v->visitor->previsit_if = previsit_if;
  v->visitor->postvisit_if = postvisit_if;
  v->visitor->previsit_set = previsit_set;
  v->visitor->postvisit_set = postvisit_set;
  v->visitor->previsit_lambda = previsit_lambda;
  v->visitor->postvisit_lambda = postvisit_lambda;
  v->visitor->previsit_let = previsit_let;
  v->visitor->postvisit_let_bindings = postvisit_let_bindings;
  v->visitor->previsit_let_binding = previsit_let_binding;
  v->visitor->postvisit_let_binding = postvisit_let_binding;
  v->visitor->postvisit_let = postvisit_let;
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