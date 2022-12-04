#include "reader.h"
#include "context.h"
#include "error.h"
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
    case schemerlicht_ct_invalid_cell:
      break;
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
  global_out.type = schemerlicht_ct_invalid_cell;
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
  if (*token_it >= *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, -1, -1, NULL, "");
    return schemerlicht_make_nil_sym_cell(ctxt);
    }
  if ((*token_it)->type == SCHEMERLICHT_T_LEFT_ROUND_BRACKET)
    {
    int is_vector = 0;
    if (strcmp((*token_it)->value.string_ptr, "#(") == 0)
      is_vector = 1;
    ++(*token_it);
    if (*token_it == *token_it_end)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, -1, -1, NULL, "");
      return schemerlicht_make_nil_sym_cell(ctxt);
      }
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

static schemerlicht_cell make_invalid_cell()
  {
  schemerlicht_cell c;
  c.type = schemerlicht_ct_invalid_cell;
  return c;
  }

schemerlicht_cell schemerlicht_read_datum(schemerlicht_context* ctxt, token* first_token, schemerlicht_string* buff, void* stream, schemerlicht_get_char_fun get_char, schemerlicht_next_char_fun next_char, schemerlicht_get_position_fun get_position)
  {
  schemerlicht_read_token_state state;
  state.line_nr = 1;
  state.column_nr = 1;
  token tok;
  if (first_token)
    {
    tok = *first_token;
    }
  else if (first_token == NULL && schemerlicht_read_token_polymorph(&tok, ctxt, buff, stream, &state, get_char, next_char, get_position)==0)
    {
    return make_invalid_cell();
    }
  if (tok.type == SCHEMERLICHT_T_LEFT_ROUND_BRACKET)
    {
    int is_vector = 0;
    if (strcmp(tok.value.string_ptr, "#(") == 0)
      is_vector = 1;
    schemerlicht_vector items;
    schemerlicht_vector_init(ctxt, &items, schemerlicht_cell);
    int tokens_remaining = 1;
    schemerlicht_token_destroy(ctxt, &tok);
    tokens_remaining = schemerlicht_read_token_polymorph(&tok, ctxt, buff, stream, &state, get_char, next_char, get_position);
    while (tokens_remaining && (tok.type != SCHEMERLICHT_T_RIGHT_ROUND_BRACKET))
      {
      schemerlicht_cell new_cell = schemerlicht_read_datum(ctxt, &tok, buff, stream, get_char, next_char, get_position);
      schemerlicht_vector_push_back(ctxt, &items, new_cell, schemerlicht_cell);
      tokens_remaining = schemerlicht_read_token_polymorph(&tok, ctxt, buff, stream, &state, get_char, next_char, get_position);
      }
    schemerlicht_token_destroy(ctxt, &tok);
    schemerlicht_cell c;
    if (is_vector)
      c = get_vector(ctxt, &items);
    else
      c = get_pair(ctxt, &items);
    schemerlicht_vector_destroy(ctxt, &items);
    return c;
    }
  else
    {
    if (tok.type == SCHEMERLICHT_T_QUOTE || tok.type == SCHEMERLICHT_T_BACKQUOTE || tok.type == SCHEMERLICHT_T_UNQUOTE || tok.type == SCHEMERLICHT_T_UNQUOTE_SPLICING)
      {
      schemerlicht_cell c;
      c.type = schemerlicht_ct_pair;
      schemerlicht_cell p1;
      p1.type = schemerlicht_ct_symbol;
      switch (tok.type)
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
      schemerlicht_token_destroy(ctxt, &tok);
      schemerlicht_vector_init(ctxt, &c.value.vector, schemerlicht_cell);
      schemerlicht_vector_push_back(ctxt, &c.value.vector, p1, schemerlicht_cell);
      schemerlicht_cell p2;
      p2.type = schemerlicht_ct_pair;
      schemerlicht_vector_init(ctxt, &p2.value.vector, schemerlicht_cell);
      schemerlicht_vector_push_back(ctxt, &p2.value.vector, schemerlicht_read_datum(ctxt, NULL, buff, stream, get_char, next_char, get_position), schemerlicht_cell);
      schemerlicht_vector_push_back(ctxt, &p2.value.vector, schemerlicht_make_nil_sym_cell(ctxt), schemerlicht_cell);
      schemerlicht_vector_push_back(ctxt, &c.value.vector, p2, schemerlicht_cell);
      return c;
      }
    else
      {
      schemerlicht_cell c = atom(ctxt, &tok);
      schemerlicht_token_destroy(ctxt, &tok);
      return c;
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
    case schemerlicht_ct_invalid_cell:
      break;
    }
  }
/*
schemerlicht_cell schemerlicht_cell_copy_recursive(schemerlicht_context* ctxt, schemerlicht_cell* c)
  {
  schemerlicht_cell out;
  out.type = c->type;
  switch (c->type)
    {
    case schemerlicht_ct_fixnum:
    case schemerlicht_ct_flonum:
    case schemerlicht_ct_string:
    case schemerlicht_ct_symbol:
    {
    schemerlicht_cell_copy_recursive(ctxt, &out.value.str, &c->value.str);
    break;
    }
    case schemerlicht_ct_pair:
    case schemerlicht_ct_vector:
    {
    schemerlicht_vector_init_reserve(ctxt, &out.value.vector, c->value.vector.vector_size, schemerlicht_cell);
    for (schemerlicht_memsize j = 0; j < c->value.vector.vector_size; ++j)
      {
      schemerlicht_cell cc = schemerlicht_cell_copy_recursive(ctxt, schemerlicht_vector_at(&c->value.vector, j, schemerlicht_cell));
      schemerlicht_vector_push_back(ctxt, &out.value.vector, cc, schemerlicht_cell);
      }
    break;
    }
    }
  return out;
  }
  */

schemerlicht_cell schemerlicht_cell_copy(schemerlicht_context* ctxt, schemerlicht_cell* c_in)
  {
  schemerlicht_vector out, in;
  schemerlicht_vector_init(ctxt, &out, schemerlicht_cell);
  schemerlicht_vector_init(ctxt, &in, schemerlicht_cell*);
  schemerlicht_vector_push_back(ctxt, &in, c_in, schemerlicht_cell*);
  schemerlicht_vector counter;
  schemerlicht_vector size;
  schemerlicht_vector_init(ctxt, &counter, schemerlicht_memsize);
  schemerlicht_vector_init(ctxt, &size, schemerlicht_memsize);
  while (in.vector_size > 0)
    {
    if (counter.vector_size > 0)
      {
      schemerlicht_assert(*schemerlicht_vector_back(&counter, schemerlicht_memsize) > 0);
      *schemerlicht_vector_back(&counter, schemerlicht_memsize) -= 1;
      }
    schemerlicht_cell* c = *schemerlicht_vector_back(&in, schemerlicht_cell*);
    schemerlicht_vector_pop_back(&in);
    switch (c->type)
      {
      case schemerlicht_ct_fixnum:
      case schemerlicht_ct_flonum:
      case schemerlicht_ct_string:
      case schemerlicht_ct_symbol:
      {
      schemerlicht_cell c_copy;
      c_copy.type = c->type;
      schemerlicht_string_copy(ctxt, &c_copy.value.str, &c->value.str);
      schemerlicht_vector_push_back(ctxt, &out, c_copy, schemerlicht_cell);
      break;
      }
      case schemerlicht_ct_pair:
      case schemerlicht_ct_vector:
      {
      schemerlicht_vector_push_back(ctxt, &counter, c->value.vector.vector_size, schemerlicht_memsize);
      schemerlicht_memsize vec_size = c->value.vector.vector_size;
      if (c->type == schemerlicht_ct_pair)
        vec_size |= schemerlicht_memsize_sign_bit;
      schemerlicht_vector_push_back(ctxt, &size, vec_size, schemerlicht_memsize);
      for (schemerlicht_memsize j = 0; j < c->value.vector.vector_size; ++j)
        {
        schemerlicht_cell* cc = schemerlicht_vector_at(&c->value.vector, j, schemerlicht_cell);
        schemerlicht_vector_push_back(ctxt, &in, cc, schemerlicht_cell*);
        }
      break;
      }
      case schemerlicht_ct_invalid_cell:
        break;
      } // switch (c->type)
    while (counter.vector_size > 0 && *schemerlicht_vector_back(&counter, schemerlicht_memsize) == 0)
      {
      schemerlicht_cell c_copy;
      schemerlicht_memsize sz = *schemerlicht_vector_back(&size, schemerlicht_memsize);
      schemerlicht_vector_pop_back(&counter);
      schemerlicht_vector_pop_back(&size);
      if (sz & schemerlicht_memsize_sign_bit)
        c_copy.type = schemerlicht_ct_pair;
      else
        c_copy.type = schemerlicht_ct_vector;
      sz &= ~schemerlicht_memsize_sign_bit;
      schemerlicht_vector_init_reserve(ctxt, &c_copy.value.vector, sz, schemerlicht_cell);
      for (schemerlicht_memsize j = 0; j < sz; ++j)
        {
        schemerlicht_cell entry = *schemerlicht_vector_back(&out, schemerlicht_cell);
        schemerlicht_vector_pop_back(&out);
        schemerlicht_vector_push_back(ctxt, &c_copy.value.vector, entry, schemerlicht_cell);
        }
      schemerlicht_vector_push_back(ctxt, &out, c_copy, schemerlicht_cell);
      }

    }
  schemerlicht_cell output = *schemerlicht_vector_back(&out, schemerlicht_cell);
  schemerlicht_vector_destroy(ctxt, &in);
  schemerlicht_vector_destroy(ctxt, &out);
  schemerlicht_vector_destroy(ctxt, &counter);
  schemerlicht_vector_destroy(ctxt, &size);
  return output;
  }

schemerlicht_object schemerlicht_cell_to_object(schemerlicht_context* ctxt, schemerlicht_cell* inputcell)
  {
  schemerlicht_object result;
  schemerlicht_object* last_result = NULL;
  schemerlicht_vector parent;
  schemerlicht_vector_init(ctxt, &parent, schemerlicht_object*);
  schemerlicht_vector todo;
  schemerlicht_vector_init(ctxt, &todo, schemerlicht_cell);
  schemerlicht_vector_push_back(ctxt, &todo, *inputcell, schemerlicht_cell);

  while (todo.vector_size > 0)
    {
    schemerlicht_object res;
    schemerlicht_cell c = *schemerlicht_vector_back(&todo, schemerlicht_cell);
    schemerlicht_vector_pop_back(&todo);
    schemerlicht_memsize parents_to_add = 0;
    if (parent.vector_size > 0)
      {
      last_result = *schemerlicht_vector_back(&parent, schemerlicht_object*);
      schemerlicht_vector_pop_back(&parent);
      }
    switch (c.type)
      {
      case schemerlicht_ct_fixnum:
      {
      res.type = schemerlicht_object_type_fixnum;
      res.value.fx = schemerlicht_to_fixnum(c.value.str.string_ptr);
      break;
      }
      case schemerlicht_ct_flonum:
      {
      res.type = schemerlicht_object_type_flonum;
      res.value.fl = schemerlicht_to_flonum(c.value.str.string_ptr);
      break;
      }
      case schemerlicht_ct_string:
      {
      const char* from = c.value.str.string_ptr + 1;
      const char* to = c.value.str.string_ptr + c.value.str.string_length - 1;
      res.type = schemerlicht_object_type_string;
      schemerlicht_string_init_ranged(ctxt, &res.value.s, from, to);
      schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
      schemerlicht_set_object(heap_obj, &res);
      ++ctxt->heap_pos;
      break;
      }
      case schemerlicht_ct_symbol:
      {
      char chval;
      if (schemerlicht_cell_equals(&c, &ctxt->global->true_sym))
        {
        res.type = schemerlicht_object_type_true;
        }
      else if (schemerlicht_cell_equals(&c, &ctxt->global->false_sym))
        {
        res.type = schemerlicht_object_type_false;
        }
      else if (schemerlicht_is_char(&c, &chval))
        {
        res.type = schemerlicht_object_type_char;
        res.value.ch = chval;
        }
      else
        {
        schemerlicht_object key;
        key.type = schemerlicht_object_type_string;
        key.value.s = c.value.str;
        schemerlicht_object* symbol = schemerlicht_map_get(ctxt, ctxt->string_to_symbol, &key);
        if (symbol != NULL)
          {
          schemerlicht_set_object(&res, symbol);
          }
        else
          {
          schemerlicht_object key;
          key.type = schemerlicht_object_type_string;
          schemerlicht_string_copy(ctxt, &key.value.s, &c.value.str);
          schemerlicht_object* new_symbol = schemerlicht_map_insert(ctxt, ctxt->string_to_symbol, &key);
          new_symbol->type = schemerlicht_object_type_symbol;
          schemerlicht_string_copy(ctxt, &new_symbol->value.s, &c.value.str);
          schemerlicht_set_object(&res, new_symbol);
          }
        }
      break;
      }
      case schemerlicht_ct_pair:
      {
      if (schemerlicht_cell_equals(&c, &ctxt->global->nil_sym))
        {
        res.type = schemerlicht_object_type_nil;
        }
      else
        {
        schemerlicht_assert(c.value.vector.vector_size == 2);
        res = make_schemerlicht_object_pair(ctxt);
        res.value.v.vector_size = 0; // we set the size to 0, so that we can push back without memory consequences
        schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
        schemerlicht_set_object(heap_obj, &res);
        ++ctxt->heap_pos;
        schemerlicht_cell* c0 = schemerlicht_vector_at(&c.value.vector, 0, schemerlicht_cell);
        schemerlicht_cell* c1 = schemerlicht_vector_at(&c.value.vector, 1, schemerlicht_cell);
        schemerlicht_vector_push_back(ctxt, &todo, *c1, schemerlicht_cell);
        schemerlicht_vector_push_back(ctxt, &todo, *c0, schemerlicht_cell);
        parents_to_add = 2;
        }
      break;
      }
      case schemerlicht_ct_vector:
      {
      res = make_schemerlicht_object_vector(ctxt, c.value.vector.vector_size);
      res.value.v.vector_size = 0; // we set the size to 0, so that we can push back without memory consequences
      schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
      schemerlicht_set_object(heap_obj, &res);
      ++ctxt->heap_pos;
      schemerlicht_cell* it = schemerlicht_vector_begin(&c.value.vector, schemerlicht_cell);
      schemerlicht_cell* it_end = schemerlicht_vector_end(&c.value.vector, schemerlicht_cell);
      schemerlicht_cell* rit = it_end - 1;
      schemerlicht_cell* rit_end = it - 1;
      for (; rit != rit_end; --rit)
        {
        schemerlicht_vector_push_back(ctxt, &todo, *rit, schemerlicht_cell);
        }
      parents_to_add = c.value.vector.vector_size;
      break;
      }
      case schemerlicht_ct_invalid_cell:
        break;
      }
    if (last_result)
      {
      schemerlicht_vector_push_back(ctxt, &last_result->value.v, res, schemerlicht_object);
      schemerlicht_object* add_to_parent = schemerlicht_vector_back(&last_result->value.v, schemerlicht_object);
      for (int i = 0; i < parents_to_add; ++i)
        {
        schemerlicht_vector_push_back(ctxt, &parent, add_to_parent, schemerlicht_object*);
        }
      }
    else
      {
      result = res;
      for (int i = 0; i < parents_to_add; ++i)
        {
        schemerlicht_vector_push_back(ctxt, &parent, &result, schemerlicht_object*);
        }
      }
    }

  schemerlicht_vector_destroy(ctxt, &parent);
  schemerlicht_vector_destroy(ctxt, &todo);
  return result;
  }

int schemerlicht_is_char(schemerlicht_cell* c, char* ch)
  {
  if (c->value.str.string_length < 2)
    return 0;
  if (c->value.str.string_ptr[0] == '#' && c->value.str.string_ptr[1] == '\\')
    {
    schemerlicht_memsize len = c->value.str.string_length - 2;
    const char* str = c->value.str.string_ptr + 2;
    if (len == 0)
      {
      *ch = 32;
      return 1;
      }
    else if (len == 1)
      {
      *ch = *str;
      return 1;
      }
    else if (strcmp(str, "backspace") == 0)
      {
      *ch = 8;
      return 1;
      }
    else if (strcmp(str, "tab") == 0)
      {
      *ch = 9;
      return 1;
      }
    else if (strcmp(str, "newline") == 0)
      {
      *ch = 10;
      return 1;
      }
    else if (strcmp(str, "linefeed") == 0)
      {
      *ch = 10;
      return 1;
      }
    else if (strcmp(str, "vtab") == 0)
      {
      *ch = 11;
      return 1;
      }
    else if (strcmp(str, "page") == 0)
      {
      *ch = 12;
      return 1;
      }
    else if (strcmp(str, "return") == 0)
      {
      *ch = 13;
      return 1;
      }
    else if (strcmp(str, "space") == 0)
      {
      *ch = 32;
      return 1;
      }
    }
  return 0;
  }
