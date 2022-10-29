#include "overrides.h"
#include "visitor.h"
#include "stringvec.h"
#include "context.h"

typedef struct schemerlicht_overrides_conversion_visitor
  {
  schemerlicht_visitor* visitor;
  } schemerlicht_overrides_conversion_visitor;

static void postvisit_primcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  if (schemerlicht_string_vector_binary_search(&ctxt->overrides, &e->expr.prim.name) != 0)
    {
    schemerlicht_expression var = schemerlicht_init_variable(ctxt);
    var.expr.var.name = e->expr.prim.name;
    var.expr.var.filename = e->expr.prim.filename;
    var.expr.var.line_nr = e->expr.prim.line_nr;
    var.expr.var.column_nr = e->expr.prim.column_nr;
    if (e->expr.prim.as_object)
      {
      schemerlicht_vector_destroy(ctxt, &e->expr.prim.arguments);
      *e = var;
      }
    else
      {
      schemerlicht_expression fun = schemerlicht_init_funcall(ctxt);
      schemerlicht_vector_destroy(ctxt, &fun.expr.funcall.arguments);
      fun.expr.funcall.arguments = e->expr.prim.arguments;      
      fun.expr.funcall.line_nr = e->expr.prim.line_nr;
      fun.expr.funcall.column_nr = e->expr.prim.column_nr;
      schemerlicht_vector_push_back(ctxt, &fun.expr.funcall.fun, var, schemerlicht_expression);
      *e = fun;
      }
    }
  }

static schemerlicht_overrides_conversion_visitor* schemerlicht_overrides_conversion_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_overrides_conversion_visitor* v = schemerlicht_new(ctxt, schemerlicht_overrides_conversion_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->postvisit_primcall = postvisit_primcall;
  return v;
  }

static void schemerlicht_overrides_conversion_visitor_free(schemerlicht_context* ctxt, schemerlicht_overrides_conversion_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }

void schemerlicht_overrides_conversion(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_overrides_conversion_visitor* v = schemerlicht_overrides_conversion_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_overrides_conversion_visitor_free(ctxt, v);
  }