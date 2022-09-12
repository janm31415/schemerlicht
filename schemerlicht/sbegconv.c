#include "sbegconv.h"
#include "svisitor.h"
#include "scontext.h"

typedef struct schemerlicht_remove_nested_begin_visitor
  {
  schemerlicht_visitor* visitor;
  } schemerlicht_remove_nested_begin_visitor;

static void postvisit_begin(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_parsed_begin* b = &e->expr.beg;
  schemerlicht_expression* it = schemerlicht_vector_begin(&b->arguments, schemerlicht_expression);
  schemerlicht_expression* it_end = schemerlicht_vector_end(&b->arguments, schemerlicht_expression);
  while (it != it_end)
    {
    if (it->type == schemerlicht_type_begin)
      {
      schemerlicht_parsed_begin* b2 = &it->expr.beg;
      schemerlicht_expression* it2 = schemerlicht_vector_begin(&b2->arguments, schemerlicht_expression);
      schemerlicht_expression* it2_end = schemerlicht_vector_end(&b2->arguments, schemerlicht_expression);
      schemerlicht_vector_erase(ctxt, &b->arguments, &it, schemerlicht_expression);
      schemerlicht_vector_insert(ctxt, &b->arguments, &it, &it2, &it2_end, schemerlicht_expression);
      }
    else
      ++it;
    }
  }

static void postvisit_program(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_program* p)
  {
  UNUSED(v);
  schemerlicht_expression* it = schemerlicht_vector_begin(&p->expressions, schemerlicht_expression);
  schemerlicht_expression* it_end = schemerlicht_vector_end(&p->expressions, schemerlicht_expression);
  while (it != it_end)
    {
    if (it->type == schemerlicht_type_begin)
      {
      schemerlicht_parsed_begin* b2 = &it->expr.beg;
      schemerlicht_expression* it2 = schemerlicht_vector_begin(&b2->arguments, schemerlicht_expression);
      schemerlicht_expression* it2_end = schemerlicht_vector_end(&b2->arguments, schemerlicht_expression);
      schemerlicht_vector_erase(ctxt, &p->expressions, &it, schemerlicht_expression);
      schemerlicht_vector_insert(ctxt, &p->expressions, &it, &it2, &it2_end, schemerlicht_expression);
      }
    else
      ++it;
    }
  }

static schemerlicht_remove_nested_begin_visitor* schemerlicht_remove_nested_begin_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_remove_nested_begin_visitor* v = schemerlicht_new(ctxt, schemerlicht_remove_nested_begin_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v); 
  v->visitor->postvisit_begin = postvisit_begin;  
  v->visitor->postvisit_program = postvisit_program;
  return v;
  }

static void schemerlicht_remove_nested_begin_visitor_free(schemerlicht_context* ctxt, schemerlicht_remove_nested_begin_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }

void schemerlicht_single_begin_conversion(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_remove_nested_begin_expressions(ctxt, program);

  if (program->expressions.vector_size > 1)
    {
    schemerlicht_parsed_begin b;
    b.filename.string_capacity = 0;
    b.filename.string_length = 0;
    b.filename.string_ptr = NULL;
    b.arguments = program->expressions;
    program->expressions.vector_ptr = NULL;
    program->expressions.vector_size = 0;
    program->expressions.vector_capacity = 0;
    schemerlicht_vector_init(ctxt, &program->expressions, schemerlicht_expression);
    schemerlicht_expression expr;
    expr.type = schemerlicht_type_begin;
    expr.expr.beg = b;
    schemerlicht_vector_push_back(ctxt, &program->expressions, expr, schemerlicht_expression);
    }
  }

void schemerlicht_remove_nested_begin_expressions(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_remove_nested_begin_visitor* v = schemerlicht_remove_nested_begin_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_remove_nested_begin_visitor_free(ctxt, v);
  }