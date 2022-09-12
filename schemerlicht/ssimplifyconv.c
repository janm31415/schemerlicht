#include "ssimplifyconv.h"
#include "scontext.h"
#include "svisitor.h"
#include "smemory.h"
#include "sparser.h"

#include <string.h>

typedef struct schemerlicht_simplify_to_core_forms_visitor
  {
  schemerlicht_visitor* visitor;
  uint64_t letrec_index;
  } schemerlicht_simplify_to_core_forms_visitor;

static schemerlicht_expression make_true()
  {
  schemerlicht_parsed_true t;
  t.filename = make_null_string();
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_literal;
  expr.expr.lit.type = schemerlicht_type_true;
  expr.expr.lit.lit.t = t;
  return expr;
  }

static schemerlicht_expression make_false()
  {
  schemerlicht_parsed_false f;
  f.filename = make_null_string();
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_literal;
  expr.expr.lit.type = schemerlicht_type_false;
  expr.expr.lit.lit.f = f;
  return expr;
  }

static schemerlicht_expression make_primitive_call(schemerlicht_context* ctxt, const char* name, schemerlicht_expression* first, schemerlicht_expression* last)
  {
  schemerlicht_parsed_primitive_call p;
  p.filename = make_null_string();
  schemerlicht_string_init(ctxt, &p.name, name);
  schemerlicht_vector_init(ctxt, &p.arguments, schemerlicht_expression);
  schemerlicht_expression* it = schemerlicht_vector_begin(&p.arguments, schemerlicht_expression);
  schemerlicht_vector_insert(ctxt, &p.arguments, &it, &first, &last, schemerlicht_expression);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_primitive_call;
  expr.expr.prim = p;
  return expr;
  }

static schemerlicht_expression make_if(schemerlicht_context* ctxt, schemerlicht_expression* a, schemerlicht_expression* b, schemerlicht_expression* c)
  {
  schemerlicht_parsed_if i;
  i.filename = make_null_string();
  schemerlicht_vector_init(ctxt, &i.arguments, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &i.arguments, *a, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &i.arguments, *b, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &i.arguments, *c, schemerlicht_expression);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_if;
  expr.expr.i = i;
  return expr;
  }

static schemerlicht_expression make_var(schemerlicht_context* ctxt, const char* name)
  {
  schemerlicht_parsed_variable v;
  v.filename = make_null_string();
  schemerlicht_string_init(ctxt, &v.name, name);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_variable;
  expr.expr.var = v;
  return expr;
  }

static void convert_and(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
   /*
   syntax (and expr ...)

   If no exprs are provided, then result is #t.
   If a single expr is provided, then it is in tail position, so the results of the and expression are the results of the expr.
   Otherwise, the first expr is evaluated. If it produces #f, the result of the and expression is #f.
   Otherwise, the result is the same as an and expression with the remaining exprs in tail position with respect to the original and form.
   */
  schemerlicht_assert(strcmp(e->expr.prim.name.string_ptr, "and") == 0);
  if (e->expr.prim.arguments.vector_size == 0)
    {
    schemerlicht_vector_destroy(ctxt, &e->expr.prim.arguments);
    schemerlicht_string_destroy(ctxt, &e->expr.prim.name);
    schemerlicht_string_destroy(ctxt, &e->expr.prim.filename);
    *e = make_true();
    }
  else if (e->expr.prim.arguments.vector_size == 1)
    {
    schemerlicht_expression expr = *schemerlicht_vector_at(&e->expr.prim.arguments, 0, schemerlicht_expression);
    schemerlicht_vector_destroy(ctxt, &e->expr.prim.arguments);
    schemerlicht_string_destroy(ctxt, &e->expr.prim.name);
    schemerlicht_string_destroy(ctxt, &e->expr.prim.filename);
    *e = expr;
    }
  else
    {
    schemerlicht_expression* a = schemerlicht_vector_at(&e->expr.prim.arguments, 0, schemerlicht_expression);
    schemerlicht_expression b = make_primitive_call(ctxt, "and", a+1, schemerlicht_vector_end(&e->expr.prim.arguments, schemerlicht_expression));
    schemerlicht_expression c = make_false();
    schemerlicht_expression expr = make_if(ctxt, a, &b, &c);
    schemerlicht_vector_destroy(ctxt, &e->expr.prim.arguments);
    schemerlicht_string_destroy(ctxt, &e->expr.prim.name);
    schemerlicht_string_destroy(ctxt, &e->expr.prim.filename);
    *e = expr;
    schemerlicht_visit_expression(ctxt, v, e);
    }
  }

static void convert_or(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  /*
   (or expr ...)

   If no exprs are provided, then result is #f.
   If a single expr is provided, then it is in tail position, so the results of the or expression are the results of the expr.
   Otherwise, the first expr is evaluated. If it produces a value other than #f, that result is the result of the or expression. Otherwise, the result is the same as an or expression with the remaining exprs in tail position with respect to the original or form.
   */
  schemerlicht_assert(strcmp(e->expr.prim.name.string_ptr, "or") == 0);
  if (e->expr.prim.arguments.vector_size == 0)
    {
    schemerlicht_vector_destroy(ctxt, &e->expr.prim.arguments);
    schemerlicht_string_destroy(ctxt, &e->expr.prim.name);
    schemerlicht_string_destroy(ctxt, &e->expr.prim.filename);
    *e = make_false();
    }
  else if (e->expr.prim.arguments.vector_size == 1)
    {
    schemerlicht_expression expr = *schemerlicht_vector_at(&e->expr.prim.arguments, 0, schemerlicht_expression);
    schemerlicht_vector_destroy(ctxt, &e->expr.prim.arguments);
    schemerlicht_string_destroy(ctxt, &e->expr.prim.name);
    schemerlicht_string_destroy(ctxt, &e->expr.prim.filename);
    *e = expr;
    }
  else
    {
    schemerlicht_expression l = schemerlicht_init_let(ctxt);
    schemerlicht_expression beg = schemerlicht_init_begin(ctxt);
    schemerlicht_let_binding binding;
    schemerlicht_string_init(ctxt, &binding.binding_name, "#%x");
    binding.binding_expr = *schemerlicht_vector_at(&e->expr.prim.arguments, 0, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &l.expr.let.bindings, binding, schemerlicht_let_binding);

    schemerlicht_expression a = make_var(ctxt, "#%x");
    schemerlicht_expression b = make_var(ctxt, "#%x");
    schemerlicht_expression c = make_primitive_call(ctxt, "or", schemerlicht_vector_at(&e->expr.prim.arguments, 0, schemerlicht_expression) + 1, schemerlicht_vector_end(&e->expr.prim.arguments, schemerlicht_expression));
    schemerlicht_vector_push_back(ctxt, &beg.expr.beg.arguments, make_if(ctxt, &a, &b, &c), schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &l.expr.let.body, beg, schemerlicht_expression);
    schemerlicht_vector_destroy(ctxt, &e->expr.prim.arguments);
    schemerlicht_string_destroy(ctxt, &e->expr.prim.name);
    schemerlicht_string_destroy(ctxt, &e->expr.prim.filename);
    *e = l;
    schemerlicht_visit_expression(ctxt, v, e);
    }
  }

static void postvisit_expression(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  switch (e->type)
    {
    case schemerlicht_type_primitive_call:
      if (strcmp(e->expr.prim.name.string_ptr, "and")==0)
        {
        convert_and(ctxt, v, e);
        }
      else if (strcmp(e->expr.prim.name.string_ptr, "or") == 0)
        {
        convert_or(ctxt, v, e);
        }
      break;
    case schemerlicht_type_let:
      break;
    case schemerlicht_type_cond:
      break;
    case schemerlicht_type_case:
      break;
    case schemerlicht_type_do:
      //schemerlicht_visit_expression(ctxt, v, e);
      break;
    }
  }

static schemerlicht_simplify_to_core_forms_visitor* schemerlicht_simplify_to_core_forms_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_simplify_to_core_forms_visitor* v = schemerlicht_new(ctxt, schemerlicht_simplify_to_core_forms_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->postvisit_expression = postvisit_expression;  
  return v;
  }

static void schemerlicht_simplify_to_core_forms_visitor_free(schemerlicht_context* ctxt, schemerlicht_simplify_to_core_forms_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }

void schemerlicht_simplify_to_core_forms(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_simplify_to_core_forms_visitor* v = schemerlicht_simplify_to_core_forms_visitor_new(ctxt);
  v->letrec_index = 0;
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_simplify_to_core_forms_visitor_free(ctxt, v);
  }