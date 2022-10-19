#include "quasiquote.h"
#include "context.h"
#include "memory.h"
#include "visitor.h"
#include "vector.h"
#include "dump.h"
#include "token.h"
#include "reader.h"
#include "limits.h"
#include "stream.h"
#include "error.h"

#include <string.h>

typedef struct schemerlicht_quasiquote_visitor
  {
  schemerlicht_visitor* visitor;
  } schemerlicht_quasiquote_visitor;

static int tag_is_backquote(schemerlicht_cell* c)
  {
  if (c->type == schemerlicht_ct_pair && c->value.vector.vector_size == 2)
    {
    schemerlicht_cell* p0 = schemerlicht_vector_at(&c->value.vector, 0, schemerlicht_cell);
    return ((p0->type == schemerlicht_ct_symbol) && (strcmp(p0->value.str.string_ptr, "`")==0 || strcmp(p0->value.str.string_ptr, "quasiquote") == 0));      
    }
  return 0;
  }

static int tag_is_quote(schemerlicht_cell* c)
  {
  if (c->type == schemerlicht_ct_pair && c->value.vector.vector_size == 2)
    {
    schemerlicht_cell* p0 = schemerlicht_vector_at(&c->value.vector, 0, schemerlicht_cell);
    return ((p0->type == schemerlicht_ct_symbol) && (strcmp(p0->value.str.string_ptr, "'") == 0 || strcmp(p0->value.str.string_ptr, "quote") == 0));
    }
  return 0;
  }

static int tag_is_comma(schemerlicht_cell* c)
  {
  if (c->type == schemerlicht_ct_pair && c->value.vector.vector_size == 2)
    {
    schemerlicht_cell* p0 = schemerlicht_vector_at(&c->value.vector, 0, schemerlicht_cell);
    return ((p0->type == schemerlicht_ct_symbol) && (strcmp(p0->value.str.string_ptr, ",") == 0 || strcmp(p0->value.str.string_ptr, "unquote") == 0));
    }
  return 0;
  }

static int tag_is_comma_atsign(schemerlicht_cell* c)
  {
  if (c->type == schemerlicht_ct_pair && c->value.vector.vector_size == 2)
    {
    schemerlicht_cell* p0 = schemerlicht_vector_at(&c->value.vector, 0, schemerlicht_cell);
    return ((p0->type == schemerlicht_ct_symbol) && (strcmp(p0->value.str.string_ptr, ",@") == 0 || strcmp(p0->value.str.string_ptr, "unquote-splicing") == 0));
    }
  return 0;
  }

static schemerlicht_cell* tag_data(schemerlicht_context* ctxt, schemerlicht_cell* c)
  {
  UNUSED(ctxt);
  schemerlicht_assert(tag_is_backquote(c) || tag_is_comma(c) || tag_is_comma_atsign(c));
  schemerlicht_cell* p0 = schemerlicht_vector_at(&c->value.vector, 0, schemerlicht_cell);
  if (p0->value.str.string_length > 3) // quasiquote, unquote, or unquote-splicing
    {
    schemerlicht_cell p1 = *schemerlicht_vector_at(&c->value.vector, 1, schemerlicht_cell);
    return schemerlicht_vector_at(&p1.value.vector, 0, schemerlicht_cell);
    }
  return schemerlicht_vector_at(&c->value.vector, 1, schemerlicht_cell);
  }

static int tag_is_special(schemerlicht_cell* c)
  {
  return tag_is_backquote(c) || tag_is_quote(c) || tag_is_comma(c) || tag_is_comma_atsign(c);
  }

static int is_pair(schemerlicht_cell* c)
  {
  return (c->type == schemerlicht_ct_pair && c->value.vector.vector_size == 2);
  }

static int is_nil(schemerlicht_cell* c)
  {
  return (c->type == schemerlicht_ct_pair && c->value.vector.vector_size == 0);
  }

static int is_simple(schemerlicht_context* ctxt, schemerlicht_cell* in)
  {
  schemerlicht_vector todo;
  schemerlicht_vector_init(ctxt, &todo, schemerlicht_cell*);
  schemerlicht_vector_push_back(ctxt, &todo, in, schemerlicht_cell*);

  while (todo.vector_size > 0)
    {
    schemerlicht_cell* c = *schemerlicht_vector_back(&todo, schemerlicht_cell*);
    schemerlicht_vector_pop_back(&todo);
    if (tag_is_special(c))
      {
      schemerlicht_vector_destroy(ctxt, &todo);
      return 0;
      }
    if (is_pair(c))
      {
      schemerlicht_vector_push_back(ctxt, &todo, schemerlicht_vector_at(&c->value.vector, 0, schemerlicht_cell), schemerlicht_cell*);
      schemerlicht_vector_push_back(ctxt, &todo, schemerlicht_vector_at(&c->value.vector, 1, schemerlicht_cell), schemerlicht_cell*);
      }
    else if (c->type == schemerlicht_ct_vector)
      {
      for (schemerlicht_memsize j = 0; j < c->value.vector.vector_size; ++j)
        {
        schemerlicht_vector_push_back(ctxt, &todo, schemerlicht_vector_at(&c->value.vector, j, schemerlicht_cell), schemerlicht_cell*);
        }
      }
    }
  schemerlicht_vector_destroy(ctxt, &todo);
  return 1;
  }

static int has_splicing(schemerlicht_context* ctxt, schemerlicht_cell* in)
  {
  schemerlicht_vector todo;
  schemerlicht_vector_init(ctxt, &todo, schemerlicht_cell*);
  schemerlicht_vector_push_back(ctxt, &todo, in, schemerlicht_cell*);

  while (todo.vector_size > 0)
    {
    schemerlicht_cell* c = *schemerlicht_vector_back(&todo, schemerlicht_cell*);
    schemerlicht_vector_pop_back(&todo);
    if (tag_is_comma_atsign(c))
      {
      schemerlicht_vector_destroy(ctxt, &todo);
      return 1;
      }
    if (is_pair(c))
      {
      schemerlicht_vector_push_back(ctxt, &todo, schemerlicht_vector_at(&c->value.vector, 0, schemerlicht_cell), schemerlicht_cell*);
      schemerlicht_vector_push_back(ctxt, &todo, schemerlicht_vector_at(&c->value.vector, 1, schemerlicht_cell), schemerlicht_cell*);
      }
    else if (c->type == schemerlicht_ct_vector)
      {
      for (schemerlicht_memsize j = 0; j < c->value.vector.vector_size; ++j)
        {
        schemerlicht_vector_push_back(ctxt, &todo, schemerlicht_vector_at(&c->value.vector, j, schemerlicht_cell), schemerlicht_cell*);
        }
      }
    }
  schemerlicht_vector_destroy(ctxt, &todo);
  return 0;
  }

static schemerlicht_cell expression_to_cell(schemerlicht_context* ctxt, schemerlicht_expression* e)
  {
  schemerlicht_string s = schemerlicht_dump_expression(ctxt, e);
  schemerlicht_stream str;
  schemerlicht_stream_init(ctxt, &str, s.string_capacity);
  schemerlicht_stream_write_str(ctxt, &str, s.string_ptr);
  schemerlicht_stream_rewind(&str);
  schemerlicht_vector tokens = tokenize(ctxt, &str);
  schemerlicht_stream_close(ctxt, &str);
  schemerlicht_string_destroy(ctxt, &s);

  token* token_it = schemerlicht_vector_begin(&tokens, token);
  token* token_it_end = schemerlicht_vector_end(&tokens, token);

  schemerlicht_cell result = schemerlicht_read_quote(ctxt, &token_it, &token_it_end, 1);
  destroy_tokens_vector(ctxt, &tokens);

  schemerlicht_expression_destroy(ctxt, e);
  return result;
  }

static schemerlicht_expression make_list(schemerlicht_context* ctxt, schemerlicht_expression* e)
  {
  schemerlicht_expression p = schemerlicht_init_primcall(ctxt);
  schemerlicht_string_init(ctxt, &p.expr.prim.name, "cons");
  schemerlicht_expression q = schemerlicht_init_quote(ctxt);
  q.expr.quote.arg.type = schemerlicht_ct_pair;
  schemerlicht_vector_push_back(ctxt, &p.expr.prim.arguments, *e, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &p.expr.prim.arguments, q, schemerlicht_expression);
  return p;
  }

static schemerlicht_expression cell_to_expression(schemerlicht_context* ctxt, schemerlicht_cell* c, int as_list)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "");
  schemerlicht_dump_cell_to_string(ctxt, c, &s);
  schemerlicht_stream str;
  schemerlicht_stream_init(ctxt, &str, s.string_capacity);
  schemerlicht_stream_write_str(ctxt, &str, s.string_ptr);
  schemerlicht_stream_rewind(&str);
  schemerlicht_vector tokens = tokenize(ctxt, &str);
  schemerlicht_stream_close(ctxt, &str);
  schemerlicht_string_destroy(ctxt, &s);
  token* token_it = schemerlicht_vector_begin(&tokens, token);
  token* token_it_end = schemerlicht_vector_end(&tokens, token);

  schemerlicht_expression result = schemerlicht_make_expression(ctxt, &token_it, &token_it_end);
  destroy_tokens_vector(ctxt, &tokens);

  if (!as_list)
    return result;
  return make_list(ctxt, &result);
  }

static schemerlicht_expression qq_expand(schemerlicht_context* ctxt, schemerlicht_cell* c, int depth);

static schemerlicht_expression make_quote(schemerlicht_context* ctxt, const char* quote_type, schemerlicht_cell* c, int list, int depth)
  {
  schemerlicht_expression q = schemerlicht_init_quote(ctxt);
  q.expr.quote.arg.type = schemerlicht_ct_symbol;
  schemerlicht_string_init(ctxt, &q.expr.quote.arg.value.str, quote_type);
  schemerlicht_cell td = schemerlicht_cell_copy(ctxt, tag_data(ctxt, c));
  int target_depth = depth - 1;
  if (strcmp(quote_type, "quasiquote") == 0)
    target_depth = depth + 1;
  schemerlicht_expression e = qq_expand(ctxt, &td, target_depth);
  schemerlicht_expression p = schemerlicht_init_primcall(ctxt);
  schemerlicht_string_init(ctxt, &p.expr.prim.name, "cons");
  schemerlicht_vector_push_back(ctxt, &p.expr.prim.arguments, q, schemerlicht_expression);
  schemerlicht_expression p2 = schemerlicht_init_primcall(ctxt);
  schemerlicht_string_init(ctxt, &p2.expr.prim.name, "cons");
  schemerlicht_vector_push_back(ctxt, &p2.expr.prim.arguments, e, schemerlicht_expression);
  schemerlicht_expression q2 = schemerlicht_init_quote(ctxt);
  q2.expr.quote.arg.type = schemerlicht_ct_pair;
  schemerlicht_vector_push_back(ctxt, &p2.expr.prim.arguments, q2, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &p.expr.prim.arguments, p2, schemerlicht_expression);
  if (!list)
    return p;
  return make_list(ctxt, &p);
  }

static schemerlicht_expression qq_expand_list(schemerlicht_context* ctxt, schemerlicht_cell* c, int depth)
  {
  if (is_simple(ctxt, c))
    {
    schemerlicht_expression q = schemerlicht_init_quote(ctxt);
    q.expr.quote.arg.type = schemerlicht_ct_pair;
    schemerlicht_vector_init(ctxt, &q.expr.quote.arg.value.vector, schemerlicht_cell);
    schemerlicht_vector_push_back(ctxt, &q.expr.quote.arg.value.vector, *c, schemerlicht_cell);
    schemerlicht_vector_push_back(ctxt, &q.expr.quote.arg.value.vector, schemerlicht_make_nil_sym_cell(ctxt), schemerlicht_cell);    
    return q;
    }
  else if (tag_is_comma(c))
    {
    if (depth == 0)
      {
      schemerlicht_cell* td = tag_data(ctxt, c);
      schemerlicht_expression expr = cell_to_expression(ctxt, td, 1);
      schemerlicht_destroy_cell(ctxt, c);
      return expr;
      }
    else
      {
      schemerlicht_expression expr = make_quote(ctxt, "unquote", c, 1, depth);
      schemerlicht_destroy_cell(ctxt, c);
      return expr;
      }
    }
  else if (tag_is_comma_atsign(c))
    {
    if (depth == 0)
      {
      schemerlicht_cell* td = tag_data(ctxt, c);
      schemerlicht_expression expr = cell_to_expression(ctxt, td, 0);
      schemerlicht_destroy_cell(ctxt, c);
      return expr;
      }
    else
      {
      schemerlicht_expression expr = make_quote(ctxt, "unquote-splicing", c, 1, depth);
      schemerlicht_destroy_cell(ctxt, c);
      return expr;
      }
    }
  else if (tag_is_backquote(c))
    {
    schemerlicht_expression expr = make_quote(ctxt, "quasiquote", c, 1, depth);
    schemerlicht_destroy_cell(ctxt, c);
    return expr;
    }
  else if (is_pair(c))
    {
    schemerlicht_cell* p0 = schemerlicht_vector_at(&c->value.vector, 0, schemerlicht_cell);
    schemerlicht_cell* p1 = schemerlicht_vector_at(&c->value.vector, 1, schemerlicht_cell);
    if (!has_splicing(ctxt, p0))
      {
      schemerlicht_expression expr1 = qq_expand(ctxt, p0, depth);
      schemerlicht_expression expr2 = qq_expand(ctxt, p1, depth);
      schemerlicht_expression p = schemerlicht_init_primcall(ctxt);
      schemerlicht_string_init(ctxt, &p.expr.prim.name, "cons");
      schemerlicht_vector_push_back(ctxt, &p.expr.prim.arguments, expr1, schemerlicht_expression);
      schemerlicht_vector_push_back(ctxt, &p.expr.prim.arguments, expr2, schemerlicht_expression);
      schemerlicht_vector_destroy(ctxt, &c->value.vector);
      return make_list(ctxt, &p);
      }
    else if (is_nil(p1))
      {
      schemerlicht_expression expr1 = qq_expand_list(ctxt, p0, depth);
      schemerlicht_vector_destroy(ctxt, &c->value.vector);
      return make_list(ctxt, &expr1);
      }
    else
      {
      schemerlicht_expression expr1 = qq_expand_list(ctxt, p0, depth);
      schemerlicht_expression expr2 = qq_expand(ctxt, p1, depth);
      schemerlicht_expression f = schemerlicht_init_funcall(ctxt);
      schemerlicht_expression v = schemerlicht_init_variable(ctxt);
      schemerlicht_string_init(ctxt, &v.expr.var.name, "append");
      schemerlicht_vector_push_back(ctxt, &f.expr.funcall.fun, v, schemerlicht_expression);
      schemerlicht_vector_push_back(ctxt, &f.expr.funcall.arguments, expr1, schemerlicht_expression);
      schemerlicht_vector_push_back(ctxt, &f.expr.funcall.arguments, expr2, schemerlicht_expression);
      schemerlicht_vector_destroy(ctxt, &c->value.vector);
      return make_list(ctxt, &f);
      }
    }
  else if (c->type == schemerlicht_ct_vector)
    {
    schemerlicht_cell empty_pair_cell;
    empty_pair_cell.type = schemerlicht_ct_pair;
    empty_pair_cell.value.vector.vector_size = 0;
    empty_pair_cell.value.vector.vector_capacity = 0;
    empty_pair_cell.value.vector.element_size = 0;
    empty_pair_cell.value.vector.vector_ptr = NULL;
    schemerlicht_cell list_cell = empty_pair_cell;
    schemerlicht_cell* parent = &list_cell;
    for (schemerlicht_memsize j = 0; j < c->value.vector.vector_size; ++j)
      {
      schemerlicht_assert(parent->value.vector.vector_size == 0);
      schemerlicht_vector_init(ctxt, &parent->value.vector, schemerlicht_cell);
      schemerlicht_cell* val = schemerlicht_vector_at(&c->value.vector, j, schemerlicht_cell);
      schemerlicht_vector_push_back(ctxt, &parent->value.vector, *val, schemerlicht_cell);
      schemerlicht_vector_push_back(ctxt, &parent->value.vector, empty_pair_cell, schemerlicht_cell);
      schemerlicht_cell* p1 = schemerlicht_vector_at(&parent->value.vector, 1, schemerlicht_cell);
      parent = p1;
      }
    schemerlicht_expression expr = qq_expand(ctxt, &list_cell, depth);
    schemerlicht_expression f = schemerlicht_init_funcall(ctxt);
    schemerlicht_expression v = schemerlicht_init_variable(ctxt);
    schemerlicht_string_init(ctxt, &v.expr.var.name, "list->vector");
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.fun, v, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.arguments, expr, schemerlicht_expression);
    schemerlicht_vector_destroy(ctxt, &c->value.vector);
    return make_list(ctxt, &f);
    }
  else
    {
    schemerlicht_expression q = schemerlicht_init_quote(ctxt);
    q.expr.quote.arg = *c;
    return q;
    }
  return schemerlicht_init_nop(ctxt);
  }

static schemerlicht_expression qq_expand(schemerlicht_context* ctxt, schemerlicht_cell* c, int depth)
  {
  if (is_simple(ctxt, c))
    {
    schemerlicht_expression q = schemerlicht_init_quote(ctxt);
    q.expr.quote.arg = *c;
    return q;
    }
  else if (tag_is_comma(c))
    {
    if (depth == 0)
      {
      schemerlicht_cell* td = tag_data(ctxt, c);
      schemerlicht_expression expr = cell_to_expression(ctxt, td, 0);
      schemerlicht_destroy_cell(ctxt, c);
      return expr;
      }
    else
      {
      schemerlicht_expression expr = make_quote(ctxt, "unquote", c, 0, depth);
      schemerlicht_destroy_cell(ctxt, c);
      return expr;
      }
    }
  else if (tag_is_comma_atsign(c))
    {
    if (depth == 0)
      {      
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, -1, -1, "");
      //schemerlicht_expression q = schemerlicht_init_quote(ctxt);
      //q.expr.quote.arg = *c;
      //return q;
      return schemerlicht_init_nop(ctxt);
      }
    else
      {
      schemerlicht_expression expr = make_quote(ctxt, "unquote-splicing", c, 0, depth);
      schemerlicht_destroy_cell(ctxt, c);
      return expr;
      }
    }
  else if (tag_is_backquote(c))
    {
    schemerlicht_expression expr = make_quote(ctxt, "quasiquote", c, 0, depth);
    schemerlicht_destroy_cell(ctxt, c);
    return expr;
    }
  else if (is_pair(c))
    {
    schemerlicht_cell* p0 = schemerlicht_vector_at(&c->value.vector, 0, schemerlicht_cell);
    schemerlicht_cell* p1 = schemerlicht_vector_at(&c->value.vector, 1, schemerlicht_cell);
    if (!has_splicing(ctxt, p0))
      {
      schemerlicht_expression expr1 = qq_expand(ctxt, p0, depth);
      schemerlicht_expression expr2 = qq_expand(ctxt, p1, depth);
      schemerlicht_expression p = schemerlicht_init_primcall(ctxt);
      schemerlicht_string_init(ctxt, &p.expr.prim.name, "cons");
      schemerlicht_vector_push_back(ctxt, &p.expr.prim.arguments, expr1, schemerlicht_expression);
      schemerlicht_vector_push_back(ctxt, &p.expr.prim.arguments, expr2, schemerlicht_expression);
      schemerlicht_vector_destroy(ctxt, &c->value.vector);
      return p;
      }
    else if (is_nil(p1))
      {
      schemerlicht_expression expr1 = qq_expand_list(ctxt, p0, depth);
      schemerlicht_vector_destroy(ctxt, &p1->value.vector);
      schemerlicht_vector_destroy(ctxt, &c->value.vector);
      return expr1;
      }
    else
      {
      schemerlicht_expression expr1 = qq_expand_list(ctxt, p0, depth);
      schemerlicht_expression expr2 = qq_expand(ctxt, p1, depth);
      schemerlicht_expression f = schemerlicht_init_funcall(ctxt);
      schemerlicht_expression v = schemerlicht_init_variable(ctxt);
      schemerlicht_string_init(ctxt, &v.expr.var.name, "append");      
      schemerlicht_vector_push_back(ctxt, &f.expr.funcall.fun, v, schemerlicht_expression);
      schemerlicht_vector_push_back(ctxt, &f.expr.funcall.arguments, expr1, schemerlicht_expression);
      schemerlicht_vector_push_back(ctxt, &f.expr.funcall.arguments, expr2, schemerlicht_expression);
      schemerlicht_vector_destroy(ctxt, &c->value.vector);
      return f;
      }
    }
  else if (c->type == schemerlicht_ct_vector)
    {
    schemerlicht_cell empty_pair_cell;
    empty_pair_cell.type = schemerlicht_ct_pair;
    empty_pair_cell.value.vector.vector_size = 0;
    empty_pair_cell.value.vector.vector_capacity = 0;
    empty_pair_cell.value.vector.element_size = 0;
    empty_pair_cell.value.vector.vector_ptr = NULL;
    schemerlicht_cell list_cell = empty_pair_cell;
    schemerlicht_cell* parent = &list_cell;
    for (schemerlicht_memsize j = 0; j < c->value.vector.vector_size; ++j)
      {
      schemerlicht_assert(parent->value.vector.vector_size == 0);
      schemerlicht_vector_init(ctxt, &parent->value.vector, schemerlicht_cell);
      schemerlicht_cell* val = schemerlicht_vector_at(&c->value.vector, j, schemerlicht_cell);
      schemerlicht_vector_push_back(ctxt, &parent->value.vector, *val, schemerlicht_cell);
      schemerlicht_vector_push_back(ctxt, &parent->value.vector, empty_pair_cell, schemerlicht_cell);
      schemerlicht_cell* p1 = schemerlicht_vector_at(&parent->value.vector, 1, schemerlicht_cell);
      parent = p1;
      }
    schemerlicht_expression expr = qq_expand(ctxt, &list_cell, depth);
    schemerlicht_expression f = schemerlicht_init_funcall(ctxt);
    schemerlicht_expression v = schemerlicht_init_variable(ctxt);
    schemerlicht_string_init(ctxt, &v.expr.var.name, "list->vector");
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.fun, v, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.arguments, expr, schemerlicht_expression);    
    schemerlicht_vector_destroy(ctxt, &c->value.vector);
    return f;
    }
  else
    {
    schemerlicht_expression q = schemerlicht_init_quote(ctxt);
    q.expr.quote.arg = *c;
    return q;
    }
  return schemerlicht_init_nop(ctxt);
  }

static void convert(schemerlicht_context* ctxt, schemerlicht_expression* e)
  {
  schemerlicht_expression new_e = qq_expand(ctxt, &e->expr.quote.arg, 0);
  schemerlicht_string_destroy(ctxt, &e->expr.quote.filename);
  *e = new_e;
  }

static void visit_quote(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  if (e->expr.quote.qt == schemerlicht_qt_backquote)
    {
    convert(ctxt, e);
    }
  }

static schemerlicht_quasiquote_visitor* schemerlicht_quasiquote_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_quasiquote_visitor* v = schemerlicht_new(ctxt, schemerlicht_quasiquote_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->visit_quote = visit_quote;
  return v;
  }

static void schemerlicht_quasiquote_visitor_free(schemerlicht_context* ctxt, schemerlicht_quasiquote_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }

void schemerlicht_quasiquote_conversion(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_quasiquote_visitor* v = schemerlicht_quasiquote_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_quasiquote_visitor_free(ctxt, v);
  }