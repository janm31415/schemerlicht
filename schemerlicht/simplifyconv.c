#include "simplifyconv.h"
#include "context.h"
#include "visitor.h"
#include "memory.h"
#include "parser.h"

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
/*
static schemerlicht_expression make_var_string(schemerlicht_context* ctxt, schemerlicht_string* s)
  {
  schemerlicht_parsed_variable v;
  v.filename = make_null_string();
  v.name = *s;
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_variable;
  expr.expr.var = v;
  return expr;
  }
*/
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

static void convert_letrec(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->expr.let.bt == schemerlicht_bt_letrec);
  /*
   (letrec([x1 e1] ...[xn en]) body)

   becomes:
   (let([x1 undefined] ...[xn undefined])
   (let([t1 e1] ...[tn en])
   (set! x1 t1)
   ...
   (set! xn tn))
   body)
   */
  schemerlicht_expression internal_let = schemerlicht_init_let(ctxt);
  schemerlicht_expression internal_begin = schemerlicht_init_begin(ctxt);
  schemerlicht_let_binding* it = schemerlicht_vector_begin(&e->expr.let.bindings, schemerlicht_let_binding);
  schemerlicht_let_binding* it_end = schemerlicht_vector_end(&e->expr.let.bindings, schemerlicht_let_binding);
  char buffer[20];
  for (; it != it_end; ++it)
    {   
    memset(buffer, 0, 20 * sizeof(char));
    sprintf(buffer, "%lld", cast(schemerlicht_simplify_to_core_forms_visitor*, v->impl)->letrec_index);
    schemerlicht_string str;
    schemerlicht_string_init(ctxt, &str, "#%t");
    schemerlicht_string_append_cstr(ctxt, &str, buffer);
    schemerlicht_let_binding b;
    b.binding_name = str;
    b.binding_expr = it->binding_expr;
    schemerlicht_vector_push_back(ctxt, &internal_let.expr.let.bindings, b, schemerlicht_let_binding);
    schemerlicht_expression set = schemerlicht_init_set(ctxt);
    schemerlicht_string_copy(ctxt, &set.expr.set.name, &it->binding_name);
    schemerlicht_vector_push_back(ctxt, &set.expr.set.value, make_var(ctxt, str.string_ptr), schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &internal_begin.expr.beg.arguments, set, schemerlicht_expression);
    it->binding_expr = schemerlicht_init_nop(ctxt);
    ++(cast(schemerlicht_simplify_to_core_forms_visitor*, v->impl)->letrec_index);
    }
  schemerlicht_vector_push_back(ctxt, &internal_let.expr.let.body, internal_begin, schemerlicht_expression);
  schemerlicht_expression* beg = schemerlicht_vector_at(&e->expr.let.body, 0, schemerlicht_expression);
  schemerlicht_assert(beg->type == schemerlicht_type_begin);
  schemerlicht_vector_push_front(ctxt, &beg->expr.beg.arguments, internal_let, schemerlicht_expression);
  e->expr.let.bt = schemerlicht_bt_let;
  }

static void convert_let_star(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->expr.let.bt == schemerlicht_bt_let_star);
  if (e->expr.let.bindings.vector_size <= 1)
    {
    e->expr.let.bt = schemerlicht_bt_let;
    }
  else
    {
    schemerlicht_expression new_let = schemerlicht_init_let(ctxt);
    schemerlicht_vector_push_back(ctxt, &new_let.expr.let.bindings, *schemerlicht_vector_at(&e->expr.let.bindings, 0, schemerlicht_expression), schemerlicht_expression);
    schemerlicht_vector_pop_front(&e->expr.let.bindings);
    schemerlicht_expression b = schemerlicht_init_begin(ctxt);
    schemerlicht_vector_push_back(ctxt, &b.expr.beg.arguments, *e, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &new_let.expr.let.body, b, schemerlicht_expression);
    *e = new_let;
    convert_let_star(ctxt, v, schemerlicht_vector_at(&schemerlicht_vector_at(&e->expr.let.body, 0, schemerlicht_expression)->expr.beg.arguments, 0, schemerlicht_expression));
    }
  }

static void convert_named_let(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_assert(e->expr.let.named_let);
  schemerlicht_assert(e->expr.let.bt == schemerlicht_bt_let);
  /*
   (let <variable 0> ([<variable 1> <init 1>] ...) <body>)
   ==
   ((letrec ([<variable 0> (lambda (<variable 1> ...) <body>)]) <variable 0>) <init 1> ...)
   */
  schemerlicht_expression lambda = schemerlicht_init_lambda(ctxt);
  schemerlicht_expression f = schemerlicht_init_funcall(ctxt);
  schemerlicht_vector_destroy(ctxt, &lambda.expr.lambda.body);
  lambda.expr.lambda.body = e->expr.let.body;
  schemerlicht_let_binding* it = schemerlicht_vector_begin(&e->expr.let.bindings, schemerlicht_let_binding);
  schemerlicht_let_binding* it_end = schemerlicht_vector_end(&e->expr.let.bindings, schemerlicht_let_binding);
  for (; it != it_end; ++it)
    {
    schemerlicht_vector_push_back(ctxt, &lambda.expr.lambda.variables, it->binding_name, schemerlicht_string);
    schemerlicht_vector_push_back(ctxt, &f.expr.funcall.arguments, it->binding_expr, schemerlicht_expression);
    }

  schemerlicht_expression letr = schemerlicht_init_let(ctxt);
  letr.expr.let.bt = schemerlicht_bt_letrec;
  schemerlicht_let_binding bind;
  bind.binding_name = e->expr.let.let_name;
  bind.binding_expr = lambda;
  schemerlicht_vector_push_back(ctxt, &letr.expr.let.bindings, bind, schemerlicht_let_binding);
  schemerlicht_expression beg = schemerlicht_init_begin(ctxt);
  schemerlicht_vector_push_back(ctxt, &beg.expr.beg.arguments, make_var(ctxt, e->expr.let.let_name.string_ptr), schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &letr.expr.let.body, beg, schemerlicht_expression);
  
  convert_letrec(ctxt, v, &letr);

  schemerlicht_vector_push_back(ctxt, &f.expr.funcall.fun, letr, schemerlicht_expression);  
  
  schemerlicht_vector_destroy(ctxt, &e->expr.let.assignable_variables);
  schemerlicht_vector_destroy(ctxt, &e->expr.let.bindings);
  schemerlicht_string_destroy(ctxt, &e->expr.let.filename);  

  *e = f;
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
      switch (e->expr.let.bt)
        {
        case schemerlicht_bt_letrec:
          convert_letrec(ctxt, v, e);
          break;
        case schemerlicht_bt_let_star:
          convert_let_star(ctxt, v, e);
          break;
        }
      if (e->expr.let.named_let)
        {
        convert_named_let(ctxt, v, e);
        }
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