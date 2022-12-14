#include "dump.h"
#include "memory.h"
#include "reader.h"
#include "syscalls.h"

#include <string.h>
#include <stdio.h>

static void visit_fixnum(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  char buffer[80];
  memset(buffer, 0, 80 * sizeof(char));
  schemerlicht_fixnum_to_char(buffer, e->expr.lit.lit.fx.value);
  schemerlicht_string_append_cstr(ctxt, &(d->s), buffer);
  schemerlicht_string_push_back(ctxt, &(d->s), ' ');
  }

static void visit_flonum(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  char buffer[80];
  memset(buffer, 0, 80 * sizeof(char));
  schemerlicht_flonum_to_char(buffer, e->expr.lit.lit.fl.value);
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
  schemerlicht_int_to_char(buffer, (int)ch);
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
  schemerlicht_string_append_cstr(ctxt, &(d->s), "( foreign-call ");
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
  //schemerlicht_string_append_cstr(ctxt, &(d->s), "[ ");
  schemerlicht_string_push_back(ctxt, &(d->s), d->binding_left_bracket);
  schemerlicht_string_push_back(ctxt, &(d->s), ' ');
  schemerlicht_string_append(ctxt, &(d->s), &b->binding_name);
  schemerlicht_string_push_back(ctxt, &(d->s), ' ');
  return 1;
  }
static void postvisit_let_binding(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_let_binding* b)
  {
  UNUSED(b);
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  //schemerlicht_string_append_cstr(ctxt, &(d->s), "] ");
  schemerlicht_string_push_back(ctxt, &(d->s), d->binding_right_bracket);
  schemerlicht_string_push_back(ctxt, &(d->s), ' ');
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
static int previsit_case(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), "( case ");

  schemerlicht_visit_expression(ctxt, v, schemerlicht_vector_at(&e->expr.cas.val_expr, 0, schemerlicht_expression));

  schemerlicht_assert(e->expr.cas.datum_args.vector_size == e->expr.cas.then_bodies.vector_size);

  for (schemerlicht_memsize j = 0; j < e->expr.cas.datum_args.vector_size; ++j)
    {
    schemerlicht_cell* c = schemerlicht_vector_at(&e->expr.cas.datum_args, j, schemerlicht_cell);
    //schemerlicht_string_append_cstr(ctxt, &(d->s), "[ ");
    schemerlicht_string_push_back(ctxt, &(d->s), d->binding_left_bracket);
    schemerlicht_string_push_back(ctxt, &(d->s), ' ');
    schemerlicht_dump_cell_to_string(ctxt, c, &(d->s));
    schemerlicht_string_append_cstr(ctxt, &(d->s), " ");
    schemerlicht_vector* then_body = schemerlicht_vector_at(&e->expr.cas.then_bodies, j, schemerlicht_vector);
    schemerlicht_expression* it = schemerlicht_vector_begin(then_body, schemerlicht_expression);
    schemerlicht_expression* it_end = schemerlicht_vector_end(then_body, schemerlicht_expression);
    for (; it != it_end; ++it)
      {
      schemerlicht_visit_expression(ctxt, v, it);
      }
    //schemerlicht_string_append_cstr(ctxt, &(d->s), "] ");
    schemerlicht_string_push_back(ctxt, &(d->s), d->binding_right_bracket);
    schemerlicht_string_push_back(ctxt, &(d->s), ' ');
    }
  if (e->expr.cas.else_body.vector_size > 0)
    {
    schemerlicht_expression* else_body = schemerlicht_vector_at(&e->expr.cas.else_body, 0, schemerlicht_expression);
    //schemerlicht_string_append_cstr(ctxt, &(d->s), "[ else ");
    schemerlicht_string_push_back(ctxt, &(d->s), d->binding_left_bracket);
    schemerlicht_string_append_cstr(ctxt, &(d->s), " else ");
    schemerlicht_visit_expression(ctxt, v, else_body);
    //schemerlicht_string_append_cstr(ctxt, &(d->s), "] ");
    schemerlicht_string_push_back(ctxt, &(d->s), d->binding_right_bracket);
    schemerlicht_string_push_back(ctxt, &(d->s), ' ');
    }

  schemerlicht_string_append_cstr(ctxt, &(d->s), ") ");
  return 0;
  }
static void postvisit_case(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static int previsit_cond(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), "( cond ");
  schemerlicht_vector* vit = schemerlicht_vector_begin(&e->expr.cond.arguments, schemerlicht_vector);
  schemerlicht_vector* vit_end = schemerlicht_vector_end(&e->expr.cond.arguments, schemerlicht_vector);
  int* proc_it = schemerlicht_vector_begin(&e->expr.cond.is_proc, int);
  //int* proc_it_end = schemerlicht_vector_end(&e->expr.cond.is_proc, int);
  for (; vit != vit_end; ++vit, ++proc_it)
    {
    //schemerlicht_string_append_cstr(ctxt, &(d->s), "[ ");
    schemerlicht_string_push_back(ctxt, &(d->s), d->binding_left_bracket);
    schemerlicht_string_push_back(ctxt, &(d->s), ' ');
    schemerlicht_expression* it = schemerlicht_vector_begin(vit, schemerlicht_expression);
    schemerlicht_expression* it_end = schemerlicht_vector_end(vit, schemerlicht_expression);
    if (*proc_it)
      {
      if (it != it_end)
        schemerlicht_visit_expression(ctxt, v, it++);
      schemerlicht_string_append_cstr(ctxt, &(d->s), " => ");
      for (; it != it_end; ++it)
        {
        schemerlicht_visit_expression(ctxt, v, it);
        }
      }
    else
      {
      for (; it != it_end; ++it)
        {
        schemerlicht_visit_expression(ctxt, v, it);
        }
      }
    //schemerlicht_string_append_cstr(ctxt, &(d->s), "] ");
    schemerlicht_string_push_back(ctxt, &(d->s), d->binding_right_bracket);
    schemerlicht_string_push_back(ctxt, &(d->s), ' ');
    }
  schemerlicht_string_append_cstr(ctxt, &(d->s), ") ");
  return 0;
  }
static void postvisit_cond(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }
static int previsit_do(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_dump_visitor* d = (schemerlicht_dump_visitor*)(v->impl);
  schemerlicht_string_append_cstr(ctxt, &(d->s), "( do ");
  schemerlicht_vector* vit = schemerlicht_vector_begin(&e->expr.d.bindings, schemerlicht_vector);
  schemerlicht_vector* vit_end = schemerlicht_vector_end(&e->expr.d.bindings, schemerlicht_vector);
  for (; vit != vit_end; ++vit)
    {
    //schemerlicht_string_append_cstr(ctxt, &(d->s), "[ ");
    schemerlicht_string_push_back(ctxt, &(d->s), d->binding_left_bracket);
    schemerlicht_string_push_back(ctxt, &(d->s), ' ');
    schemerlicht_expression* it = schemerlicht_vector_begin(vit, schemerlicht_expression);
    schemerlicht_expression* it_end = schemerlicht_vector_end(vit, schemerlicht_expression);
    
    for (; it != it_end; ++it)
      {
      schemerlicht_visit_expression(ctxt, v, it);
      }
      
    //schemerlicht_string_append_cstr(ctxt, &(d->s), "] ");
    schemerlicht_string_push_back(ctxt, &(d->s), d->binding_right_bracket);
    schemerlicht_string_push_back(ctxt, &(d->s), ' ');
    }
  schemerlicht_string_append_cstr(ctxt, &(d->s), ") ( ");
  schemerlicht_expression* it = schemerlicht_vector_begin(&e->expr.d.test, schemerlicht_expression);
  schemerlicht_expression* it_end = schemerlicht_vector_end(&e->expr.d.test, schemerlicht_expression);
  for (; it != it_end; ++it)
    {
    schemerlicht_visit_expression(ctxt, v, it);
    }
  schemerlicht_string_append_cstr(ctxt, &(d->s), ") ");
  it = schemerlicht_vector_begin(&e->expr.d.commands, schemerlicht_expression);
  it_end = schemerlicht_vector_end(&e->expr.d.commands, schemerlicht_expression);
  for (; it != it_end; ++it)
    {
    schemerlicht_visit_expression(ctxt, v, it);
    }
  schemerlicht_string_append_cstr(ctxt, &(d->s), ") ");
  return 0;
  }
static void postvisit_do(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  }

schemerlicht_dump_visitor* schemerlicht_dump_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_dump_visitor* v = schemerlicht_new(ctxt, schemerlicht_dump_visitor);
  v->binding_left_bracket = '(';
  v->binding_right_bracket = ')';
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
  v->visitor->previsit_cond = previsit_cond;
  v->visitor->postvisit_cond = postvisit_cond;
  v->visitor->previsit_case = previsit_case;
  v->visitor->postvisit_case = postvisit_case;
  v->visitor->previsit_do = previsit_do;
  v->visitor->postvisit_do = postvisit_do;
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

schemerlicht_string schemerlicht_dump(schemerlicht_context* ctxt, schemerlicht_program* prog, int square_brackets)
  {
  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  if (square_brackets)
    {
    dumper->binding_left_bracket = '[';
    dumper->binding_right_bracket = ']';
    }
  schemerlicht_visit_program(ctxt, dumper->visitor, prog);
  schemerlicht_string s;
  schemerlicht_string_copy(ctxt, &s, &dumper->s);
  schemerlicht_dump_visitor_free(ctxt, dumper);
  return s;
  }

schemerlicht_string schemerlicht_dump_expression(schemerlicht_context* ctxt, schemerlicht_expression* e, int square_brackets)
  {
  schemerlicht_dump_visitor* dumper = schemerlicht_dump_visitor_new(ctxt);
  if (square_brackets)
    {
    dumper->binding_left_bracket = '[';
    dumper->binding_right_bracket = ']';
    }
  schemerlicht_visit_expression(ctxt, dumper->visitor, e);
  schemerlicht_string s;
  schemerlicht_string_copy(ctxt, &s, &dumper->s);
  schemerlicht_dump_visitor_free(ctxt, dumper);
  return s;
  }