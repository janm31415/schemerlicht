#include "defconv.h"
#include "context.h"
#include "visitor.h"
#include "begconv.h"
#include "parser.h"
#include "error.h"
#include "stringvec.h"
#include "dump.h"

#include <string.h>

static schemerlicht_parsed_variable make_var(schemerlicht_context* ctxt, const char* name)
  {
  schemerlicht_parsed_variable v;
  v.filename = make_null_string();
  schemerlicht_string_init(ctxt, &v.name, name);
  return v;
  }

static int is_name(schemerlicht_expression* expr)
  {
  if (expr->type == schemerlicht_type_primitive_call)
    return expr->expr.prim.as_object;
  return (expr->type == schemerlicht_type_variable);
  }

static schemerlicht_string get_name(schemerlicht_context* ctxt, schemerlicht_expression* expr)
  {
  schemerlicht_string s;
  if (expr->type == schemerlicht_type_variable)
    schemerlicht_string_copy(ctxt, &s, &expr->expr.var.name);
  else if (expr->type == schemerlicht_type_primitive_call)
    schemerlicht_string_copy(ctxt, &s, &expr->expr.prim.name);
  else
    schemerlicht_string_init(ctxt, &s, "");
  return s;
  }

typedef struct schemerlicht_define_conversion_visitor
  {
  schemerlicht_visitor* visitor;
  int internal;
  } schemerlicht_define_conversion_visitor;

static schemerlicht_define_conversion_visitor* schemerlicht_define_conversion_visitor_new(schemerlicht_context* ctxt);
static void schemerlicht_define_conversion_visitor_free(schemerlicht_context* ctxt, schemerlicht_define_conversion_visitor* v);

static void rewrite_prim_define(schemerlicht_context* ctxt, schemerlicht_visitor* vtor, schemerlicht_expression* expr)
  {
  if (expr->expr.prim.arguments.vector_size < 2)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS, expr->expr.prim.line_nr, expr->expr.prim.column_nr, "at least 2 arguments expected");
    return;
    }
  int alternative_syntax = 0;
  schemerlicht_expression* first_arg = schemerlicht_vector_at(&expr->expr.prim.arguments, 0, schemerlicht_expression);
  if (first_arg->type == schemerlicht_type_funcall)
    alternative_syntax = 1;
  if (first_arg->type == schemerlicht_type_primitive_call && first_arg->expr.prim.as_object == 0)
    alternative_syntax = 1;

  if (alternative_syntax)
    {
    schemerlicht_parsed_funcall f;
    if (first_arg->type == schemerlicht_type_funcall)
      f = first_arg->expr.funcall;
    else
      {
      f.filename = make_null_string();
      schemerlicht_vector_init(ctxt, &f.fun, schemerlicht_expression);
      schemerlicht_parsed_variable v;
      v.filename = make_null_string();
      v.name = first_arg->expr.prim.name;
      schemerlicht_expression e = schemerlicht_make_variable_expression(&v);
      schemerlicht_vector_push_back(ctxt, &f.fun, e, schemerlicht_expression);
      f.arguments = first_arg->expr.prim.arguments;
      schemerlicht_string_destroy(ctxt, &first_arg->expr.prim.filename);
      }
    schemerlicht_expression* var = schemerlicht_vector_at(&f.fun, 0, schemerlicht_expression);
    if (var->type != schemerlicht_type_variable)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, expr->expr.prim.line_nr, expr->expr.prim.column_nr, "");
      schemerlicht_destroy_parsed_funcall(ctxt, &f);
      return;
      }
    schemerlicht_parsed_lambda lam;
    lam.variable_arity = 0;
    schemerlicht_vector_init(ctxt, &lam.body, schemerlicht_expression);
    schemerlicht_vector_init(ctxt, &lam.variables, schemerlicht_string);
    schemerlicht_vector_init(ctxt, &lam.free_variables, schemerlicht_string);
    schemerlicht_vector_init(ctxt, &lam.assignable_variables, schemerlicht_string);
    lam.filename = make_null_string();
    if (f.arguments.vector_size >= 2 && schemerlicht_vector_at(&f.arguments, f.arguments.vector_size - 2, schemerlicht_expression)->type == schemerlicht_type_literal)
      {
      // variable arguments
      schemerlicht_expression* lit = schemerlicht_vector_at(&f.arguments, f.arguments.vector_size - 2, schemerlicht_expression);
      if (lit->expr.lit.type != schemerlicht_type_flonum)
        {
        schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, expr->expr.prim.line_nr, expr->expr.prim.column_nr, ". expected");
        schemerlicht_destroy_parsed_lambda(ctxt, &lam);
        schemerlicht_destroy_parsed_funcall(ctxt, &f);
        return;
        }
      lam.variable_arity = 1;
      schemerlicht_expression* it = schemerlicht_vector_begin(&f.arguments, schemerlicht_expression);
      schemerlicht_expression* it_end = schemerlicht_vector_end(&f.arguments, schemerlicht_expression);
      for (; it != it_end; ++it)
        {
        if (it == lit)
          {
          schemerlicht_expression_destroy(ctxt, it);
          continue;
          }
        if (!is_name(it))
          {
          schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, expr->expr.prim.line_nr, expr->expr.prim.column_nr, "name expected");
          }
        schemerlicht_vector_push_back(ctxt, &lam.variables, get_name(ctxt, it), schemerlicht_string);
        schemerlicht_expression_destroy(ctxt, it);
        }
      }
    else
      {
      schemerlicht_expression* it = schemerlicht_vector_begin(&f.arguments, schemerlicht_expression);
      schemerlicht_expression* it_end = schemerlicht_vector_end(&f.arguments, schemerlicht_expression);
      for (; it != it_end; ++it)
        {
        if (!is_name(it))
          schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, expr->expr.prim.line_nr, expr->expr.prim.column_nr, "name expected");
        schemerlicht_vector_push_back(ctxt, &lam.variables, get_name(ctxt, it), schemerlicht_string);
        schemerlicht_expression_destroy(ctxt, it);
        }
      }
    schemerlicht_parsed_begin beg;
    schemerlicht_vector_init(ctxt, &beg.arguments, schemerlicht_expression);
    beg.filename = make_null_string();
    schemerlicht_expression* it = schemerlicht_vector_begin(&beg.arguments, schemerlicht_expression);
    schemerlicht_expression* p_it = schemerlicht_vector_begin(&expr->expr.prim.arguments, schemerlicht_expression) + 1;
    schemerlicht_expression* p_it_end = schemerlicht_vector_end(&expr->expr.prim.arguments, schemerlicht_expression);
    schemerlicht_vector_insert(ctxt, &beg.arguments, &it, &p_it, &p_it_end, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &lam.body, schemerlicht_make_begin_expression(&beg), schemerlicht_expression);
    expr->expr.prim.arguments.vector_size = 0;
    schemerlicht_vector_push_back(ctxt, &expr->expr.prim.arguments, *var, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &expr->expr.prim.arguments, schemerlicht_make_lambda_expression(&lam), schemerlicht_expression);

    schemerlicht_string_destroy(ctxt, &f.filename);
    schemerlicht_vector_destroy(ctxt, &f.fun);
    schemerlicht_vector_destroy(ctxt, &f.arguments);

    //printf("ENTER\n");
    schemerlicht_define_conversion_visitor* vis = (schemerlicht_define_conversion_visitor*)(vtor->impl);
    schemerlicht_define_conversion_visitor* vis2 = schemerlicht_define_conversion_visitor_new(ctxt);
    vis2->internal = vis->internal+1;
    schemerlicht_remove_nested_begin(ctxt, expr);
    schemerlicht_visit_expression(ctxt, vis2->visitor, expr);
    schemerlicht_define_conversion_visitor_free(ctxt, vis2);
    //printf("RELEASE\n");
    }
  }

static void convert_internal_define(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* body) // input body is a begin statement
  {
  schemerlicht_assert(body->type == schemerlicht_type_begin);
  schemerlicht_vector define_args;
  schemerlicht_vector define_exprs;
  schemerlicht_vector_init(ctxt, &define_args, schemerlicht_memsize);
  schemerlicht_vector_init(ctxt, &define_exprs, schemerlicht_expression);
  schemerlicht_memsize index = 0;
  schemerlicht_expression* it = schemerlicht_vector_begin(&body->expr.beg.arguments, schemerlicht_expression);
  schemerlicht_expression* it_end = schemerlicht_vector_end(&body->expr.beg.arguments, schemerlicht_expression);
  for (; it != it_end; ++it)
    {
    if (it->type == schemerlicht_type_primitive_call)
      {
      if (strcmp(it->expr.prim.name.string_ptr, "define") == 0)
        {
        rewrite_prim_define(ctxt, v, it);
        schemerlicht_vector_push_back(ctxt, &define_args, index, schemerlicht_memsize);
        schemerlicht_vector_push_back(ctxt, &define_exprs, *it, schemerlicht_expression);
        }
      }
    ++index;
    }
  if (define_args.vector_size != 0)
    {
    schemerlicht_vector* begin_args = &body->expr.beg.arguments;
    schemerlicht_memsize* it2 = schemerlicht_vector_begin(&define_args, schemerlicht_memsize);
    schemerlicht_memsize* it2_end = schemerlicht_vector_end(&define_args, schemerlicht_memsize);
    schemerlicht_memsize* rit = it2_end - 1;
    schemerlicht_memsize* rit_end = it2 - 1;
    for (; rit != rit_end; --rit)
      {
      schemerlicht_memsize idx = *rit;
      schemerlicht_expression* begin_args_it_at_index = schemerlicht_vector_begin(begin_args, schemerlicht_expression) + idx;
      schemerlicht_vector_erase(begin_args, &begin_args_it_at_index, schemerlicht_expression);
      }
    schemerlicht_parsed_let let;
    let.bt = schemerlicht_bt_letrec;
    let.filename = make_null_string();
    let.let_name = make_null_string();
    let.named_let = 0;
    schemerlicht_vector_init(ctxt, &let.bindings, schemerlicht_let_binding);
    schemerlicht_vector_init(ctxt, &let.body, schemerlicht_expression);
    schemerlicht_vector_init(ctxt, &let.assignable_variables, schemerlicht_string);
    schemerlicht_expression* eit = schemerlicht_vector_begin(&define_exprs, schemerlicht_expression);
    schemerlicht_expression* eit_end = schemerlicht_vector_end(&define_exprs, schemerlicht_expression);
    for (; eit != eit_end; ++eit)
      {
      schemerlicht_assert(strcmp(eit->expr.prim.name.string_ptr, "define") == 0);
      if (eit->expr.prim.arguments.vector_size != 2)
        {
        schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS, eit->expr.prim.line_nr, eit->expr.prim.column_nr, "2 arguments expected");
        continue;
        }
      schemerlicht_expression* arg0 = schemerlicht_vector_at(&eit->expr.prim.arguments, 0, schemerlicht_expression);
      schemerlicht_expression* arg1 = schemerlicht_vector_at(&eit->expr.prim.arguments, 1, schemerlicht_expression);
      if (arg0->type != schemerlicht_type_variable)
        {
        schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, eit->expr.prim.line_nr, eit->expr.prim.column_nr, "");
        continue;
        }
      schemerlicht_let_binding b;
      b.binding_name = arg0->expr.var.name;
      b.binding_expr = *arg1;
      schemerlicht_string_destroy(ctxt, &eit->expr.prim.name);
      schemerlicht_string_destroy(ctxt, &eit->expr.prim.filename);
      schemerlicht_string_destroy(ctxt, &arg0->expr.var.filename);
      schemerlicht_vector_push_back(ctxt, &let.bindings, b, schemerlicht_let_binding);
      schemerlicht_vector_destroy(ctxt, &eit->expr.prim.arguments);
      }
    schemerlicht_expression body_copy = *body;
    schemerlicht_vector_push_back(ctxt, &let.body, body_copy, schemerlicht_expression);
    body->expr.beg.arguments.vector_size = 0;
    body->expr.beg.filename = make_null_string();
    schemerlicht_vector_init(ctxt, &body->expr.beg.arguments, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &body->expr.beg.arguments, schemerlicht_make_let_expression(&let), schemerlicht_expression);

    }
  schemerlicht_vector_destroy(ctxt, &define_args);
  schemerlicht_vector_destroy(ctxt, &define_exprs);
  }

static void postvisit_let(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_define_conversion_visitor* vis = (schemerlicht_define_conversion_visitor*)(v->impl);
#if 1
  schemerlicht_expression* body = schemerlicht_vector_at(&e->expr.let.body, 0, schemerlicht_expression);
  convert_internal_define(ctxt, v, body);
#endif
  --vis->internal;
  }

static void postvisit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_define_conversion_visitor* vis = (schemerlicht_define_conversion_visitor*)(v->impl);
#if 1
  schemerlicht_expression* body = schemerlicht_vector_at(&e->expr.lambda.body, 0, schemerlicht_expression);
  convert_internal_define(ctxt, v, body);
#endif
  --vis->internal;
  }

static int previsit_let(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_define_conversion_visitor* vis = (schemerlicht_define_conversion_visitor*)(v->impl);
  ++vis->internal;
#if 0
  schemerlicht_expression* body = schemerlicht_vector_at(&e->expr.let.body, 0, schemerlicht_expression);
  convert_internal_define(ctxt, v, body);
#endif
  UNUSED(ctxt);
  UNUSED(e);
  return 1;
  }


static int previsit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_define_conversion_visitor* vis = (schemerlicht_define_conversion_visitor*)(v->impl);
  ++vis->internal;
#if 0
  schemerlicht_expression* body = schemerlicht_vector_at(&e->expr.lambda.body, 0, schemerlicht_expression);
  convert_internal_define(ctxt, v, body);
#endif
  UNUSED(ctxt);
  UNUSED(e);
  return 1;
  }

static int previsit_primcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* expr)
  {
  if (strcmp(expr->expr.prim.name.string_ptr, "define") == 0)
    {
    if (expr->expr.prim.arguments.vector_size < 2)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS, expr->expr.prim.line_nr, expr->expr.prim.column_nr, "at least 2 arguments expected");
      return 0;
      }
    int alternative_syntax = 0;
    schemerlicht_expression* first_arg = schemerlicht_vector_at(&expr->expr.prim.arguments, 0, schemerlicht_expression);
    if (first_arg->type == schemerlicht_type_funcall)
      alternative_syntax = 1;
    if (first_arg->type == schemerlicht_type_primitive_call && first_arg->expr.prim.as_object == 0)
      alternative_syntax = 1;
    if (alternative_syntax)
      {
      schemerlicht_define_conversion_visitor* vis = (schemerlicht_define_conversion_visitor*)(v->impl);
      ++vis->internal;
      }
    }
  return 1;
  }

static void postvisit_primcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_define_conversion_visitor* vis = (schemerlicht_define_conversion_visitor*)(v->impl);

  int cmp_define = strcmp(e->expr.prim.name.string_ptr, "define");
  if (cmp_define == 0)
    {   
    int alternative_syntax = 0;
    schemerlicht_expression* first_arg = schemerlicht_vector_at(&e->expr.prim.arguments, 0, schemerlicht_expression);
    if (first_arg->type == schemerlicht_type_funcall)
      alternative_syntax = 1;
    if (first_arg->type == schemerlicht_type_primitive_call && first_arg->expr.prim.as_object == 0)
      alternative_syntax = 1;
    if (alternative_syntax)
      {      
      --vis->internal;
      }
    }

#if 0
  if (strcmp(e->expr.prim.name.string_ptr, "define") == 0)
    {
    printf("postvisit_primcall level %d\n", vis->internal);
    schemerlicht_string s = schemerlicht_dump_expression(ctxt, e);
    printf(" --> %s\n", s.string_ptr);
    schemerlicht_string_destroy(ctxt, &s);
    }
#endif
  if (vis->internal == 0 && cmp_define == 0)
    {
    rewrite_prim_define(ctxt, v, e);
    if (e->expr.prim.arguments.vector_size != 2)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS, e->expr.prim.line_nr, e->expr.prim.column_nr, "");
      return;
      }
    schemerlicht_parsed_set s;
    s.filename = make_null_string();
    schemerlicht_expression* first_arg = schemerlicht_vector_at(&e->expr.prim.arguments, 0, schemerlicht_expression);
    if (first_arg->type == schemerlicht_type_variable)
      {
      s.name = first_arg->expr.var.name;
      s.line_nr = first_arg->expr.var.line_nr;
      s.column_nr = first_arg->expr.var.column_nr;
      s.filename = first_arg->expr.var.filename;
      //schemerlicht_string_destroy(ctxt, &first_arg->expr.var.filename);
      }
    else if (first_arg->type == schemerlicht_type_primitive_call)
      {
      s.name = first_arg->expr.prim.name;
      s.line_nr = first_arg->expr.prim.line_nr;
      s.column_nr = first_arg->expr.prim.column_nr;
      s.filename = first_arg->expr.prim.filename;
      //schemerlicht_string_destroy(ctxt, &first_arg->expr.prim.filename);
      schemerlicht_vector_destroy(ctxt, &first_arg->expr.prim.arguments);
      if (schemerlicht_string_vector_binary_search(&ctxt->overrides, &s.name) == 0)
        {
        schemerlicht_string overridden_prim;
        schemerlicht_string_copy(ctxt, &overridden_prim, &s.name);
        schemerlicht_string_vector_insert_sorted(ctxt, &ctxt->overrides, &overridden_prim);
        }
      }
    else
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, e->expr.prim.line_nr, e->expr.prim.column_nr, "");
      return;
      }
    s.originates_from_define = 1;
    s.originates_from_quote = 0;
    schemerlicht_vector_init(ctxt, &s.value, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &s.value, *schemerlicht_vector_at(&e->expr.prim.arguments, 1, schemerlicht_expression), schemerlicht_expression);
    schemerlicht_string_destroy(ctxt, &e->expr.prim.filename);
    schemerlicht_string_destroy(ctxt, &e->expr.prim.name);
    schemerlicht_vector_destroy(ctxt, &e->expr.prim.arguments);
    e->type = schemerlicht_type_set;
    e->expr.set = s;
    }
  else if (schemerlicht_string_vector_binary_search(&ctxt->overrides, &e->expr.prim.name) != 0)
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

static schemerlicht_define_conversion_visitor* schemerlicht_define_conversion_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_define_conversion_visitor* v = schemerlicht_new(ctxt, schemerlicht_define_conversion_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->postvisit_lambda = postvisit_lambda;
  v->visitor->postvisit_let = postvisit_let;
  v->visitor->previsit_lambda = previsit_lambda;
  v->visitor->previsit_let = previsit_let;
  v->visitor->postvisit_primcall = postvisit_primcall;
  v->visitor->previsit_primcall = previsit_primcall;
  v->internal = 0;
  return v;
  }

static void schemerlicht_define_conversion_visitor_free(schemerlicht_context* ctxt, schemerlicht_define_conversion_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }

void schemerlicht_define_conversion(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_remove_nested_begin_expressions(ctxt, program);
  schemerlicht_define_conversion_visitor* v = schemerlicht_define_conversion_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_define_conversion_visitor_free(ctxt, v);
  }