#include "reader.h"
#include "context.h"
#include <string.h>

int schemerlicht_cell_equals(const schemerlicht_cell* left, const schemerlicht_cell* right)
  {
  if (left->type != right->type)
    return 0;
  switch (left->type)
    {
    case schemerlicht_ct_symbol:
    case schemerlicht_ct_string:
    case schemerlicht_ct_fixnum:
    case schemerlicht_ct_flonum:
      return (strcmp(left->value.str.string_ptr, right->value.str.string_ptr) == 0);
    case schemerlicht_ct_pair:
    {
    if (left->value.vector.vector_size != right->value.vector.vector_size)
      return 0;
    if (left->value.vector.vector_size == 0) // nil
      return 1;
    schemerlicht_cell* left0 = schemerlicht_vector_at(&left->value.vector, 0, schemerlicht_cell);
    schemerlicht_cell* right0 = schemerlicht_vector_at(&right->value.vector, 0, schemerlicht_cell);
    if (!schemerlicht_cell_equals(left0, right0))
      return 0;
    schemerlicht_cell* left1 = schemerlicht_vector_at(&left->value.vector, 1, schemerlicht_cell);
    schemerlicht_cell* right1 = schemerlicht_vector_at(&right->value.vector, 1, schemerlicht_cell);
    if (!schemerlicht_cell_equals(left1, right1))
      return 0;
    return 1;
    }
    case schemerlicht_ct_vector:
    {
    if (left->value.vector.vector_size != right->value.vector.vector_size)
      return 0;
    for (schemerlicht_memsize j = 0; j < left->value.vector.vector_size; ++j)
      {
      schemerlicht_cell* left_j = schemerlicht_vector_at(&left->value.vector, j, schemerlicht_cell);
      schemerlicht_cell* right_j = schemerlicht_vector_at(&right->value.vector, j, schemerlicht_cell);
      if (!schemerlicht_cell_equals(left_j, right_j))
        return 0;
      }
    return 1;
    }
    default:
      break;
    }
  return 0;
  }

void schemerlicht_destroy_cell(schemerlicht_context* ctxt, schemerlicht_cell* c)
  {
  switch (c->type)
    {
    case schemerlicht_ct_symbol:
    case schemerlicht_ct_string:
    case schemerlicht_ct_fixnum:
    case schemerlicht_ct_flonum:
      schemerlicht_string_destroy(ctxt, &c->value.str);
      break;
    case schemerlicht_ct_pair:
    case schemerlicht_ct_vector:
    {
    schemerlicht_cell* it = schemerlicht_vector_begin(&c->value.vector, schemerlicht_cell);
    schemerlicht_cell* it_end = schemerlicht_vector_end(&c->value.vector, schemerlicht_cell);
    for (; it != it_end; ++it)
      schemerlicht_destroy_cell(ctxt, it);
    schemerlicht_vector_destroy(ctxt, &c->value.vector);
    break;
    }
    }
  }

schemerlicht_cell schemerlicht_make_false_sym_cell(schemerlicht_context* ctxt)
  {
  schemerlicht_cell c;
  c.type = schemerlicht_ct_symbol;
  schemerlicht_string_init(ctxt, &c.value.str, "#f");
  return c;
  }

schemerlicht_cell schemerlicht_make_true_sym_cell(schemerlicht_context* ctxt)
  {
  schemerlicht_cell c;
  c.type = schemerlicht_ct_symbol;
  schemerlicht_string_init(ctxt, &c.value.str, "#t");
  return c;
  }

schemerlicht_cell schemerlicht_make_nil_sym_cell(schemerlicht_context* ctxt)
  {
  schemerlicht_cell c;
  c.type = schemerlicht_ct_pair;
  schemerlicht_vector_init(ctxt, &c.value.vector, schemerlicht_cell);
  return c;
  }

static schemerlicht_cell get_vector(schemerlicht_context* ctxt, schemerlicht_vector* v)
  {
  schemerlicht_cell c;
  c.type = schemerlicht_ct_vector;
  schemerlicht_cell* it = schemerlicht_vector_begin(v, schemerlicht_cell);
  schemerlicht_cell* it_end = schemerlicht_vector_end(v, schemerlicht_cell);
  schemerlicht_vector_init_with_size(ctxt, &c.value.vector, (it_end - it), schemerlicht_cell);
  c.value.vector.vector_size = 0;
  for (; it != it_end; ++it)
    {
    schemerlicht_vector_push_back(ctxt, &c.value.vector, *it, schemerlicht_cell);
    }
  return c;
  }

static schemerlicht_cell get_pair(schemerlicht_context* ctxt, schemerlicht_vector* v)
  {
  int recurse = 1;
  int iter = 0;
  schemerlicht_cell global_out;
  schemerlicht_cell* last_cell = NULL;
  schemerlicht_cell* it = schemerlicht_vector_begin(v, schemerlicht_cell);
  schemerlicht_cell* it_end = schemerlicht_vector_end(v, schemerlicht_cell);
  while (recurse)
    {
    recurse = 0;
    schemerlicht_cell out;
    size_t sz = cast(size_t, it_end - it);
    if (sz == 0)
      out = schemerlicht_make_nil_sym_cell(ctxt);
    else if (sz == 1)
      {
      schemerlicht_cell c;
      c.type = schemerlicht_ct_pair;
      schemerlicht_vector_init(ctxt, &c.value.vector, schemerlicht_cell);
      if (it->type != schemerlicht_ct_pair && it->type != schemerlicht_ct_vector && strcmp(it->value.str.string_ptr, ".") == 0)
        {
        schemerlicht_vector_push_back(ctxt, &c.value.vector, schemerlicht_make_nil_sym_cell(ctxt), schemerlicht_cell);
        schemerlicht_string_destroy(ctxt, &it->value.str);
        }
      else
        {
        schemerlicht_vector_push_back(ctxt, &c.value.vector, *it, schemerlicht_cell);
        }
      schemerlicht_vector_push_back(ctxt, &c.value.vector, schemerlicht_make_nil_sym_cell(ctxt), schemerlicht_cell);
      out = c;
      }
    else
      {
      schemerlicht_cell c;
      c.type = schemerlicht_ct_pair;
      schemerlicht_vector_init(ctxt, &c.value.vector, schemerlicht_cell);
      if (it->type != schemerlicht_ct_pair && it->type != schemerlicht_ct_vector && strcmp(it->value.str.string_ptr, ".") == 0)
        {
        schemerlicht_vector_push_back(ctxt, &c.value.vector, schemerlicht_make_nil_sym_cell(ctxt), schemerlicht_cell);
        schemerlicht_string_destroy(ctxt, &it->value.str);
        }
      else
        {
        schemerlicht_vector_push_back(ctxt, &c.value.vector, *it, schemerlicht_cell);
        }
      ++it;
      schemerlicht_cell* next_it = it;
      if (it->type != schemerlicht_ct_pair && it->type != schemerlicht_ct_vector && strcmp(it->value.str.string_ptr, ".") == 0)
        {
        schemerlicht_string_destroy(ctxt, &it->value.str);
        size_t sz2 = cast(size_t, it_end - (it + 1));
        if (sz2 == 0)
          {
          schemerlicht_vector_push_back(ctxt, &c.value.vector, schemerlicht_make_nil_sym_cell(ctxt), schemerlicht_cell);
          }
        else if (sz2 == 1)
          {
          schemerlicht_vector_push_back(ctxt, &c.value.vector, *(it + 1), schemerlicht_cell);
          }
        else
          {
          recurse = 1;
          next_it = it + 1;
          }
        }
      else
        recurse = 1;
      it = next_it;
      out = c;
      }
    if (iter)
      {
      schemerlicht_vector_push_back(ctxt, &last_cell->value.vector, out, schemerlicht_cell);
      last_cell = schemerlicht_vector_back(&last_cell->value.vector, schemerlicht_cell);
      }
    else
      {
      global_out = out;
      last_cell = &global_out;
      }
    ++iter;
    }
  return global_out;
  }

static schemerlicht_cell atom(schemerlicht_context* ctxt, token* t)
  {
  switch (t->type)
    {
    case SCHEMERLICHT_T_FIXNUM:
    {
    schemerlicht_cell c;
    c.type = schemerlicht_ct_fixnum;
    schemerlicht_string_init(ctxt, &c.value.str, t->value.string_ptr);
    return c;
    }
    case SCHEMERLICHT_T_FLONUM:
    {
    schemerlicht_cell c;
    c.type = schemerlicht_ct_flonum;
    schemerlicht_string_init(ctxt, &c.value.str, t->value.string_ptr);
    return c;
    }
    case SCHEMERLICHT_T_STRING:
    {
    schemerlicht_cell c;
    c.type = schemerlicht_ct_string;
    schemerlicht_string_init(ctxt, &c.value.str, t->value.string_ptr);
    return c;
    }
    default:
    {
    schemerlicht_cell c;
    c.type = schemerlicht_ct_symbol;
    schemerlicht_string_init(ctxt, &c.value.str, t->value.string_ptr);
    return c;
    }
    }
  }

schemerlicht_cell schemerlicht_read_quote(schemerlicht_context* ctxt, token** token_it, token** token_it_end, int quasiquote)
  {
  if ((*token_it)->type == SCHEMERLICHT_T_LEFT_ROUND_BRACKET)
    {
    int is_vector = 0;
    if (strcmp((*token_it)->value.string_ptr, "#(") == 0)
      is_vector = 1;
    ++(*token_it);
    schemerlicht_vector items;
    schemerlicht_vector_init(ctxt, &items, schemerlicht_cell);
    while ((*token_it != *token_it_end) && ((*token_it)->type != SCHEMERLICHT_T_RIGHT_ROUND_BRACKET))
      {
      schemerlicht_cell new_cell = schemerlicht_read_quote(ctxt, token_it, token_it_end, quasiquote);
      schemerlicht_vector_push_back(ctxt, &items, new_cell, schemerlicht_cell);
      }
    schemerlicht_cell c;
    if (is_vector)
      c = get_vector(ctxt, &items);
    else
      c = get_pair(ctxt, &items);
    ++(*token_it);
    schemerlicht_vector_destroy(ctxt, &items);
    return c;
    }
  else
    {
    if (quasiquote && ((*token_it)->type == SCHEMERLICHT_T_QUOTE || (*token_it)->type == SCHEMERLICHT_T_BACKQUOTE || (*token_it)->type == SCHEMERLICHT_T_UNQUOTE || (*token_it)->type == SCHEMERLICHT_T_UNQUOTE_SPLICING))
      {
      schemerlicht_cell c;
      c.type = schemerlicht_ct_pair;
      schemerlicht_cell p1;
      p1.type = schemerlicht_ct_symbol;
      switch ((*token_it)->type)
        {
        case SCHEMERLICHT_T_QUOTE:
          schemerlicht_string_init(ctxt, &p1.value.str, "quote"); break;
        case SCHEMERLICHT_T_BACKQUOTE:
          schemerlicht_string_init(ctxt, &p1.value.str, "quasiquote"); break;
        case SCHEMERLICHT_T_UNQUOTE:
          schemerlicht_string_init(ctxt, &p1.value.str, "unquote"); break;
        case SCHEMERLICHT_T_UNQUOTE_SPLICING:
          schemerlicht_string_init(ctxt, &p1.value.str, "unquote-splicing"); break;
        }
      schemerlicht_vector_init(ctxt, &c.value.vector, schemerlicht_cell);
      schemerlicht_vector_push_back(ctxt, &c.value.vector, p1, schemerlicht_cell);
      schemerlicht_cell p2;
      p2.type = schemerlicht_ct_pair;
      schemerlicht_vector_init(ctxt, &p2.value.vector, schemerlicht_cell);
      ++(*token_it);
      schemerlicht_vector_push_back(ctxt, &p2.value.vector, schemerlicht_read_quote(ctxt, token_it, token_it_end, 1), schemerlicht_cell);
      schemerlicht_vector_push_back(ctxt, &p2.value.vector, schemerlicht_make_nil_sym_cell(ctxt), schemerlicht_cell);
      schemerlicht_vector_push_back(ctxt, &c.value.vector, p2, schemerlicht_cell);
      return c;
      }
    else
      {
      if ((*token_it)->type == SCHEMERLICHT_T_QUOTE || (*token_it)->type == SCHEMERLICHT_T_BACKQUOTE || (*token_it)->type == SCHEMERLICHT_T_UNQUOTE || (*token_it)->type == SCHEMERLICHT_T_UNQUOTE_SPLICING)
        {
        schemerlicht_cell c;
        c.type = schemerlicht_ct_pair;
        schemerlicht_cell p1;
        p1.type = schemerlicht_ct_symbol;
        switch ((*token_it)->type)
          {
          case SCHEMERLICHT_T_QUOTE:
            schemerlicht_string_init(ctxt, &p1.value.str, "quote"); break;
          case SCHEMERLICHT_T_BACKQUOTE:
            schemerlicht_string_init(ctxt, &p1.value.str, "quasiquote"); break;
          case SCHEMERLICHT_T_UNQUOTE:
            schemerlicht_string_init(ctxt, &p1.value.str, "unquote"); break;
          case SCHEMERLICHT_T_UNQUOTE_SPLICING:
            schemerlicht_string_init(ctxt, &p1.value.str, "unquote-splicing"); break;
          }
        schemerlicht_vector_init(ctxt, &c.value.vector, schemerlicht_cell);
        schemerlicht_vector_push_back(ctxt, &c.value.vector, p1, schemerlicht_cell);
        schemerlicht_cell p2;
        p2.type = schemerlicht_ct_pair;
        schemerlicht_vector_init(ctxt, &p2.value.vector, schemerlicht_cell);
        ++(*token_it);
        schemerlicht_vector_push_back(ctxt, &p2.value.vector, schemerlicht_read_quote(ctxt, token_it, token_it_end, 0), schemerlicht_cell);
        schemerlicht_vector_push_back(ctxt, &p2.value.vector, schemerlicht_make_nil_sym_cell(ctxt), schemerlicht_cell);
        schemerlicht_vector_push_back(ctxt, &c.value.vector, p2, schemerlicht_cell);
        return c;
        }
      else
        {
        schemerlicht_cell c = atom(ctxt, *token_it);
        ++(*token_it);
        return c;
        }
      }
    }
  }

void schemerlicht_dump_cell_to_string(schemerlicht_context* ctxt, schemerlicht_cell* c, schemerlicht_string* s)
  {
  switch (c->type)
    {
    case schemerlicht_ct_fixnum:
    case schemerlicht_ct_flonum:
    case schemerlicht_ct_string:
    case schemerlicht_ct_symbol:
      schemerlicht_string_append(ctxt, s, &c->value.str);
      break;
    case schemerlicht_ct_vector:
    {
    schemerlicht_string_append_cstr(ctxt, s, "#(");
    schemerlicht_cell* it = schemerlicht_vector_begin(&c->value.vector, schemerlicht_cell);
    schemerlicht_cell* it_end = schemerlicht_vector_end(&c->value.vector, schemerlicht_cell);
    for (; it != it_end; ++it)
      {
      schemerlicht_dump_cell_to_string(ctxt, it, s);
      if (it + 1 != it_end)
        schemerlicht_string_append_cstr(ctxt, s, " ");
      }
    schemerlicht_string_append_cstr(ctxt, s, ")");
    break;
    }
    case schemerlicht_ct_pair:
    {
    if (c->value.vector.vector_size == 0)
      {
      schemerlicht_string_append_cstr(ctxt, s, "()");
      break;
      }
    schemerlicht_string_push_back(ctxt, s, '(');
    schemerlicht_cell* p0 = schemerlicht_vector_at(&c->value.vector, 0, schemerlicht_cell);
    schemerlicht_cell* p1 = schemerlicht_vector_at(&c->value.vector, 1, schemerlicht_cell);
    schemerlicht_dump_cell_to_string(ctxt, p0, s);
    if (p1->type == schemerlicht_ct_pair)
      {
      while ((p1->type == schemerlicht_ct_pair) &&
        !schemerlicht_cell_equals(p1, &ctxt->global->nil_sym) &&
        (schemerlicht_vector_at(&p1->value.vector, 1, schemerlicht_cell)->type == schemerlicht_ct_pair))
        {
        schemerlicht_string_push_back(ctxt, s, ' ');
        schemerlicht_dump_cell_to_string(ctxt, schemerlicht_vector_at(&p1->value.vector, 0, schemerlicht_cell), s);
        p1 = schemerlicht_vector_at(&p1->value.vector, 1, schemerlicht_cell);
        }
      if (!schemerlicht_cell_equals(p1, &ctxt->global->nil_sym))
        {
        schemerlicht_string_push_back(ctxt, s, ' ');
        schemerlicht_dump_cell_to_string(ctxt, schemerlicht_vector_at(&p1->value.vector, 0, schemerlicht_cell), s);
        schemerlicht_string_append_cstr(ctxt, s, " . ");
        schemerlicht_dump_cell_to_string(ctxt, schemerlicht_vector_at(&p1->value.vector, 1, schemerlicht_cell), s);
        }
      schemerlicht_string_push_back(ctxt, s, ')');
      }
    else
      {
      schemerlicht_string_append_cstr(ctxt, s, " . ");
      schemerlicht_dump_cell_to_string(ctxt, p1, s);
      schemerlicht_string_push_back(ctxt, s, ')');
      }
    break;
    }
    }
  }