#include "quoteconv.h"
#include "context.h"
#include "map.h"
#include "visitor.h"
#include "vector.h"
#include "stringvec.h"
#include "reader.h"
#include "parser.h"

#include <string.h>
#include <stdio.h>

static schemerlicht_expression _parse(schemerlicht_context* ctxt, schemerlicht_cell* c)
  {
  return schemerlicht_init_fixnum(ctxt);
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
    quote_object = schemerlicht_map_get(ctxt->quote_to_index, &key);
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
      }
    }
  quote_object = schemerlicht_map_get(ctxt->quote_to_index, &key);
  schemerlicht_assert(quote_object != NULL);
  schemerlicht_assert(quote_object->type == schemerlicht_object_type_fixnum);
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
  v->index = 0;
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
  schemerlicht_expression* it = schemerlicht_vector_begin(&v->quote_expressions, schemerlicht_expression);
  schemerlicht_expression* it_end = schemerlicht_vector_end(&v->quote_expressions, schemerlicht_expression);
  schemerlicht_expression* prog_it = schemerlicht_vector_begin(&program->expressions, schemerlicht_expression);
  schemerlicht_vector_insert(ctxt, &program->expressions, &prog_it, &it, &it_end, schemerlicht_expression);
  schemerlicht_quote_conversion_visitor_free(ctxt, v);
  }