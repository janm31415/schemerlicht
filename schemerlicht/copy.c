#include "copy.h"
#include "visitor.h"
#include <string.h>

typedef struct schemerlicht_copy_visitor
  {
  schemerlicht_visitor* visitor;
  schemerlicht_vector copied_expressions_stack;
  } schemerlicht_copy_visitor;

#define INIT_FOR_COPYING \
  schemerlicht_copy_visitor* vis = cast(schemerlicht_copy_visitor*, v->impl); \
  schemerlicht_expression cp; \
  cp.type = e->type \

#define PUSH_RESULT \
  schemerlicht_vector_push_back(ctxt, &vis->copied_expressions_stack, cp, schemerlicht_expression)

#define COPY_COMMON(type) \
  cp.expr.type.line_nr = e->expr.type.line_nr; \
  cp.expr.type.column_nr = e->expr.type.column_nr; \
  cp.expr.type.tail_position = e->expr.type.tail_position; \
  schemerlicht_string_copy(ctxt, &cp.expr.type.filename, &e->expr.type.filename) \

#define COPY_COMMON_LITERAL(typ) \
  cp.expr.lit.lit.typ.line_nr = e->expr.lit.lit.typ.line_nr; \
  cp.expr.lit.lit.typ.column_nr = e->expr.lit.lit.typ.column_nr; \
  cp.expr.lit.type = e->expr.lit.type; \
  schemerlicht_string_copy(ctxt, &cp.expr.lit.lit.typ.filename, &e->expr.lit.lit.typ.filename) \

#define COPY_VECTOR(typ, vector_name) \
  { \
  schemerlicht_memsize sz = e->expr.typ.vector_name.vector_size; \
  schemerlicht_vector_init_reserve(ctxt, &cp.expr.typ.vector_name, sz, schemerlicht_expression); \
  schemerlicht_expression* stack_top = schemerlicht_vector_end(&vis->copied_expressions_stack, schemerlicht_expression); \
  schemerlicht_expression* insert_pos = schemerlicht_vector_begin(&cp.expr.typ.vector_name, schemerlicht_expression); \
  schemerlicht_expression* stack_first = stack_top - sz; \
  schemerlicht_vector_insert(ctxt, &cp.expr.typ.vector_name, &insert_pos, &stack_first, &stack_top, schemerlicht_expression); \
  schemerlicht_vector_erase_range(&vis->copied_expressions_stack, &stack_first, &stack_top, schemerlicht_expression); \
  }

#define COPY_STRING_VECTOR(typ, vector_name) \
  { \
  schemerlicht_memsize sz_str_v = e->expr.typ.vector_name.vector_size; \
  schemerlicht_vector_init_reserve(ctxt, &cp.expr.typ.vector_name, sz_str_v, schemerlicht_string); \
  for (schemerlicht_memsize j = 0; j < sz_str_v; ++j) \
    { \
    schemerlicht_string* s = schemerlicht_vector_at(&e->expr.typ.vector_name, j, schemerlicht_string); \
    schemerlicht_string s_copy; \
    schemerlicht_string_copy(ctxt, &s_copy, s); \
    schemerlicht_vector_push_back(ctxt, &cp.expr.typ.vector_name, s_copy, schemerlicht_string); \
    } \
  }

static void postvisit_program(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_program* p)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(p);
  }
static void postvisit_begin(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON(beg);
  COPY_VECTOR(beg, arguments);
  PUSH_RESULT;
  }
static void postvisit_primcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON(prim);
  schemerlicht_string_copy(ctxt, &cp.expr.prim.name, &e->expr.prim.name);
  cp.expr.prim.as_object = e->expr.prim.as_object;
  COPY_VECTOR(prim, arguments);
  PUSH_RESULT;
  }
static void visit_fixnum(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON_LITERAL(fx);
  cp.expr.lit.lit.fx.value = e->expr.lit.lit.fx.value;
  PUSH_RESULT;
  }
static void visit_flonum(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON_LITERAL(fl);
  cp.expr.lit.lit.fl.value = e->expr.lit.lit.fl.value;
  PUSH_RESULT;
  }
static void visit_nil(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON_LITERAL(nil);
  PUSH_RESULT;
  }
static void visit_true(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON_LITERAL(t);
  PUSH_RESULT;
  }
static void visit_false(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON_LITERAL(f);
  PUSH_RESULT;
  }
static void visit_nop(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON(nop);
  PUSH_RESULT;
  }
static void visit_character(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON_LITERAL(ch);
  cp.expr.lit.lit.ch.value = e->expr.lit.lit.ch.value;
  PUSH_RESULT;
  }
static void visit_variable(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON(var);
  schemerlicht_string_copy(ctxt, &cp.expr.var.name, &e->expr.var.name);
  PUSH_RESULT;
  }
static void visit_quote(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON(quote);
  cp.expr.quote.qt = e->expr.quote.qt;
  cp.expr.quote.arg = schemerlicht_cell_copy(ctxt, &e->expr.quote.arg);
  PUSH_RESULT;
  }
static void visit_string(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON_LITERAL(str);
  schemerlicht_string_copy(ctxt, &cp.expr.lit.lit.str.value, &e->expr.lit.lit.str.value);
  PUSH_RESULT;
  }
static void visit_symbol(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON_LITERAL(sym);
  schemerlicht_string_copy(ctxt, &cp.expr.lit.lit.sym.value, &e->expr.lit.lit.sym.value);
  PUSH_RESULT;
  }
static void postvisit_funcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON(funcall);
  COPY_VECTOR(funcall, arguments);
  COPY_VECTOR(funcall, fun);
  PUSH_RESULT;
  }
static void postvisit_foreigncall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON(foreign);
  schemerlicht_string_copy(ctxt, &cp.expr.foreign.name, &e->expr.foreign.name);
  COPY_VECTOR(foreign, arguments);
  PUSH_RESULT;
  }
static void postvisit_if(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON(i);
  COPY_VECTOR(i, arguments);
  PUSH_RESULT;
  }
static void postvisit_set(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON(set);
  schemerlicht_string_copy(ctxt, &cp.expr.set.name, &e->expr.set.name);
  cp.expr.set.originates_from_define = e->expr.set.originates_from_define;
  cp.expr.set.originates_from_quote = e->expr.set.originates_from_quote;
  COPY_VECTOR(set, value);
  PUSH_RESULT;
  }
static void postvisit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON(lambda);
  COPY_VECTOR(lambda, body);

  cp.expr.lambda.variable_arity = e->expr.lambda.variable_arity;
  
  COPY_STRING_VECTOR(lambda, variables);
  COPY_STRING_VECTOR(lambda, free_variables);
  COPY_STRING_VECTOR(lambda, assignable_variables);

  PUSH_RESULT;
  }
static void postvisit_let(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  INIT_FOR_COPYING;
  COPY_COMMON(let);
  cp.expr.let.bt = e->expr.let.bt;
  cp.expr.let.named_let = e->expr.let.named_let;
  schemerlicht_string_copy(ctxt, &cp.expr.let.let_name, &e->expr.let.let_name);
  COPY_VECTOR(let, body);

  schemerlicht_memsize sz = e->expr.let.bindings.vector_size; 
  schemerlicht_vector_init_reserve(ctxt, &cp.expr.let.bindings, sz, schemerlicht_let_binding);
  schemerlicht_expression* stack_top = schemerlicht_vector_end(&vis->copied_expressions_stack, schemerlicht_expression); 
  schemerlicht_expression* stack_first = stack_top - sz;
  
  for (schemerlicht_memsize j = 0; j < sz; ++j)
    {
    schemerlicht_let_binding* bind_input = schemerlicht_vector_at(&e->expr.let.bindings, j, schemerlicht_let_binding);
    schemerlicht_let_binding bind;
    schemerlicht_string_copy(ctxt, &bind.binding_name, &bind_input->binding_name);
    bind.binding_expr = *(stack_first + j);
    schemerlicht_vector_push_back(ctxt, &cp.expr.let.bindings, bind, schemerlicht_let_binding);
    }
  schemerlicht_vector_erase_range(&vis->copied_expressions_stack, &stack_first, &stack_top, schemerlicht_expression); 

  COPY_STRING_VECTOR(let, assignable_variables);
  PUSH_RESULT;
  }
static void postvisit_case(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  assert(0); // todo
  }
static void postvisit_cond(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  assert(0); // todo
  }
static void postvisit_do(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(ctxt);
  UNUSED(v);
  UNUSED(e);
  assert(0); // todo
  }

schemerlicht_expression schemerlicht_expression_copy(schemerlicht_context* ctxt, schemerlicht_expression* e)
  {
  schemerlicht_copy_visitor copycat;
  copycat.visitor = schemerlicht_visitor_new(ctxt, &copycat);

  copycat.visitor->postvisit_program = postvisit_program;
  copycat.visitor->visit_fixnum = visit_fixnum;
  copycat.visitor->visit_flonum = visit_flonum;
  copycat.visitor->visit_nil = visit_nil;
  copycat.visitor->visit_string = visit_string;
  copycat.visitor->visit_symbol = visit_symbol;
  copycat.visitor->visit_true = visit_true;
  copycat.visitor->visit_false = visit_false;
  copycat.visitor->visit_nop = visit_nop;
  copycat.visitor->visit_character = visit_character;
  copycat.visitor->visit_variable = visit_variable;
  copycat.visitor->visit_quote = visit_quote;
  copycat.visitor->postvisit_primcall = postvisit_primcall;
  copycat.visitor->postvisit_funcall = postvisit_funcall;
  copycat.visitor->postvisit_foreigncall = postvisit_foreigncall;
  copycat.visitor->postvisit_begin = postvisit_begin;
  copycat.visitor->postvisit_if = postvisit_if;
  copycat.visitor->postvisit_set = postvisit_set;
  copycat.visitor->postvisit_lambda = postvisit_lambda;
  copycat.visitor->postvisit_let = postvisit_let;
  copycat.visitor->postvisit_case = postvisit_case;
  copycat.visitor->postvisit_cond = postvisit_cond;
  copycat.visitor->postvisit_do = postvisit_do;

  schemerlicht_vector_init(ctxt, &copycat.copied_expressions_stack, schemerlicht_expression);

  schemerlicht_visit_expression(ctxt, copycat.visitor, e);

  copycat.visitor->destroy(ctxt, copycat.visitor);

  schemerlicht_expression result = *schemerlicht_vector_at(&copycat.copied_expressions_stack, 0, schemerlicht_expression);

  schemerlicht_assert(copycat.copied_expressions_stack.vector_size == 1);

  schemerlicht_vector_destroy(ctxt, &copycat.copied_expressions_stack);

  return result;
  }

schemerlicht_program schemerlicht_program_copy(schemerlicht_context* ctxt, schemerlicht_program* p)
  {
  schemerlicht_program prog;
  schemerlicht_vector_init_reserve(ctxt, &prog.expressions, p->expressions.vector_size, schemerlicht_expression);
  for (schemerlicht_memsize j = 0; j < p->expressions.vector_size; ++j)
    {
    schemerlicht_expression expr = schemerlicht_expression_copy(ctxt, schemerlicht_vector_at(&p->expressions, j, schemerlicht_expression));
    schemerlicht_vector_push_back(ctxt, &prog.expressions, expr, schemerlicht_expression);
    }
  return prog;
  }