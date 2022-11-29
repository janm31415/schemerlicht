#include "quoteconv.h"
#include "context.h"
#include "map.h"
#include "visitor.h"
#include "vector.h"
#include "stringvec.h"
#include "reader.h"
#include "parser.h"
#include "environment.h"

#include <string.h>
#include <stdio.h>


static schemerlicht_expression _parse(schemerlicht_context* ctxt, schemerlicht_cell* inputcell)
  {
  schemerlicht_expression result;
  schemerlicht_expression* last_result = NULL;
  schemerlicht_vector parent;
  schemerlicht_vector_init(ctxt, &parent, schemerlicht_expression*);
  schemerlicht_vector todo;
  schemerlicht_vector_init(ctxt, &todo, schemerlicht_cell);
  schemerlicht_vector_push_back(ctxt, &todo, *inputcell, schemerlicht_cell);

  while (todo.vector_size > 0)
    {
    schemerlicht_expression res;
    schemerlicht_cell c = *schemerlicht_vector_back(&todo, schemerlicht_cell);
    schemerlicht_vector_pop_back(&todo);
    schemerlicht_memsize parents_to_add = 0;
    if (parent.vector_size > 0)
      {
      last_result = *schemerlicht_vector_back(&parent, schemerlicht_expression*);
      schemerlicht_vector_pop_back(&parent);
      }
    switch (c.type)
      {
      case schemerlicht_ct_fixnum:
      {
      res = schemerlicht_init_fixnum(ctxt);
      res.expr.lit.lit.fx.value = schemerlicht_to_fixnum(c.value.str.string_ptr);
      break;
      }
      case schemerlicht_ct_flonum:
      {
      res = schemerlicht_init_flonum(ctxt);
      res.expr.lit.lit.fl.value = schemerlicht_to_flonum(c.value.str.string_ptr);
      break;
      }
      case schemerlicht_ct_string:
      {
      res = schemerlicht_init_string(ctxt);
      const char* from = c.value.str.string_ptr + 1;
      const char* to = c.value.str.string_ptr + c.value.str.string_length - 1;
      schemerlicht_string_init_ranged(ctxt, &res.expr.lit.lit.str.value, from, to);
      break;
      }
      case schemerlicht_ct_symbol:
      {
      char chval;
      if (schemerlicht_cell_equals(&c, &ctxt->global->true_sym))
        {
        res = schemerlicht_init_true(ctxt);
        }
      else if (schemerlicht_cell_equals(&c, &ctxt->global->false_sym))
        {
        res = schemerlicht_init_false(ctxt);
        }
      else if (schemerlicht_is_char(&c, &chval))
        {
        res = schemerlicht_init_char(ctxt);
        res.expr.lit.lit.ch.value = chval;
        }
      else
        {
        res = schemerlicht_init_primcall(ctxt);
        schemerlicht_string_init(ctxt, &res.expr.prim.name, "string->symbol");
        schemerlicht_expression str = schemerlicht_init_string(ctxt);
        schemerlicht_string_copy(ctxt, &str.expr.lit.lit.str.value, &c.value.str);
        schemerlicht_vector_push_back(ctxt, &res.expr.prim.arguments, str, schemerlicht_expression);
        }
      break;
      }
      case schemerlicht_ct_pair:
      {
      if (schemerlicht_cell_equals(&c, &ctxt->global->nil_sym))
        {
        res = schemerlicht_init_nil(ctxt);
        }
      else
        {
        schemerlicht_assert(c.value.vector.vector_size == 2);
        res = schemerlicht_init_primcall(ctxt);
        schemerlicht_string_init(ctxt, &res.expr.prim.name, "cons");
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
      res = schemerlicht_init_primcall(ctxt);
      schemerlicht_string_init(ctxt, &res.expr.prim.name, "vector");
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
      schemerlicht_vector_push_back(ctxt, &last_result->expr.prim.arguments, res, schemerlicht_expression);
      schemerlicht_expression* add_to_parent = schemerlicht_vector_back(&last_result->expr.prim.arguments, schemerlicht_expression);
      for (int i = 0; i < parents_to_add; ++i)
        {
        schemerlicht_vector_push_back(ctxt, &parent, add_to_parent, schemerlicht_expression*);
        }
      }
    else
      {
      result = res;
      for (int i = 0; i < parents_to_add; ++i)
        {
        schemerlicht_vector_push_back(ctxt, &parent, &result, schemerlicht_expression*);
        }
      }
    }

  schemerlicht_vector_destroy(ctxt, &parent);
  schemerlicht_vector_destroy(ctxt, &todo);
  return result;
  }


typedef struct schemerlicht_quote_conversion_visitor
  {
  schemerlicht_memsize index;
  schemerlicht_vector* collected_quotes;
  schemerlicht_vector quote_expressions;
  schemerlicht_visitor* visitor;
  } schemerlicht_quote_conversion_visitor;

static void visit_quote(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  char buffer[256];
  schemerlicht_quote_conversion_visitor* vis = (schemerlicht_quote_conversion_visitor*)(v->impl);
  schemerlicht_object key;
  key.type = schemerlicht_object_type_string;
  schemerlicht_string_init(ctxt, &key.value.s, "");
  schemerlicht_dump_cell_to_string(ctxt, &e->expr.quote.arg, &key.value.s);
  int clean_up_key_string = 1;

  schemerlicht_object* quote_object = NULL;
  if (schemerlicht_string_vector_binary_search(vis->collected_quotes, &key.value.s))
    {
    quote_object = schemerlicht_map_get(ctxt, ctxt->quote_to_index, &key);
    if (quote_object == NULL)
      {
      schemerlicht_expression set = schemerlicht_init_set(ctxt);
      set.expr.set.originates_from_quote = 1;
      schemerlicht_string_init(ctxt, &set.expr.set.name, "#%q");
      sprintf(buffer, "%d", vis->index);
      schemerlicht_string_append_cstr(ctxt, &set.expr.set.name, buffer);

      quote_object = schemerlicht_map_insert(ctxt, ctxt->quote_to_index, &key);
      quote_object->type = schemerlicht_object_type_fixnum;
      quote_object->value.fx = vis->index;
      clean_up_key_string = 0;

      ++vis->index;
      schemerlicht_expression val = _parse(ctxt, &e->expr.quote.arg);
      schemerlicht_vector_push_back(ctxt, &set.expr.set.value, val, schemerlicht_expression);
      schemerlicht_vector_push_back(ctxt, &vis->quote_expressions, set, schemerlicht_expression);

      schemerlicht_environment_entry entry;
      entry.type = SCHEMERLICHT_ENV_TYPE_GLOBAL;
      entry.position = ctxt->globals.vector_size;
      schemerlicht_string s;
      schemerlicht_string_copy(ctxt, &s, &set.expr.set.name);
      schemerlicht_environment_add(ctxt, &s, entry);
      schemerlicht_object obj;
      obj.type = schemerlicht_object_type_undefined;
      schemerlicht_vector_push_back(ctxt, &ctxt->globals, obj, schemerlicht_object);
      }
    }
  quote_object = schemerlicht_map_get(ctxt, ctxt->quote_to_index, &key);
  schemerlicht_assert(quote_object != NULL);
  schemerlicht_assert(schemerlicht_object_get_type(quote_object) == schemerlicht_object_type_fixnum);
  schemerlicht_expression var = schemerlicht_init_variable(ctxt);
  schemerlicht_string_init(ctxt, &var.expr.var.name, "#%q");
  sprintf(buffer, "%lld", quote_object->value.fx);
  schemerlicht_string_append_cstr(ctxt, &var.expr.var.name, buffer);

  if (clean_up_key_string)
    schemerlicht_string_destroy(ctxt, &key.value.s);

  schemerlicht_destroy_cell(ctxt, &e->expr.quote.arg);
  schemerlicht_string_destroy(ctxt, &e->expr.quote.filename);

  *e = var;
  }

static schemerlicht_quote_conversion_visitor* schemerlicht_quote_conversion_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_quote_conversion_visitor* v = schemerlicht_new(ctxt, schemerlicht_quote_conversion_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->visit_quote = visit_quote;
  v->index = ctxt->quote_to_index_size;
  schemerlicht_vector_init(ctxt, &v->quote_expressions, schemerlicht_expression);
  return v;
  }

static void schemerlicht_quote_conversion_visitor_free(schemerlicht_context* ctxt, schemerlicht_quote_conversion_visitor* v)
  {
  if (v)
    {
    schemerlicht_vector_destroy(ctxt, &v->quote_expressions);
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }


void schemerlicht_quote_conversion(schemerlicht_context* ctxt, schemerlicht_program* program, schemerlicht_vector* collected_quotes)
  {
  schemerlicht_quote_conversion_visitor* v = schemerlicht_quote_conversion_visitor_new(ctxt);
  v->collected_quotes = collected_quotes;
  schemerlicht_visit_program(ctxt, v->visitor, program);
  ctxt->quote_to_index_size = v->index;

  schemerlicht_expression* it = schemerlicht_vector_begin(&v->quote_expressions, schemerlicht_expression);
  schemerlicht_expression* it_end = schemerlicht_vector_end(&v->quote_expressions, schemerlicht_expression);
  if (program->expressions.vector_size == 1)
    {
    schemerlicht_expression* single_program_expr = schemerlicht_vector_at(&program->expressions, 0, schemerlicht_expression);
    if (single_program_expr->type == schemerlicht_type_begin)
      {
      schemerlicht_expression* beg_it = schemerlicht_vector_begin(&single_program_expr->expr.beg.arguments, schemerlicht_expression);
      schemerlicht_vector_insert(ctxt, &single_program_expr->expr.beg.arguments, &beg_it, &it, &it_end, schemerlicht_expression);
      }
    else
      {
      schemerlicht_expression begin = schemerlicht_init_begin(ctxt);
      schemerlicht_expression* beg_it = schemerlicht_vector_begin(&begin.expr.beg.arguments, schemerlicht_expression);
      schemerlicht_vector_insert(ctxt, &begin.expr.beg.arguments, &beg_it, &it, &it_end, schemerlicht_expression);
      schemerlicht_vector_push_back(ctxt, &begin.expr.beg.arguments, *single_program_expr, schemerlicht_expression);
      *single_program_expr = begin;
      }
    }
  else
    {
    schemerlicht_expression* prog_it = schemerlicht_vector_begin(&program->expressions, schemerlicht_expression);
    schemerlicht_vector_insert(ctxt, &program->expressions, &prog_it, &it, &it_end, schemerlicht_expression);
    }
  schemerlicht_quote_conversion_visitor_free(ctxt, v);
  }
