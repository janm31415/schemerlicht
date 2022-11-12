#include "macro.h"
#include "context.h"
#include "visitor.h"
#include "error.h"
#include "vector.h"
#include "string.h"
#include "dump.h"
#include "preprocess.h"
#include "func.h"
#include "compiler.h"

typedef struct schemerlicht_macro_finder_visitor
  {
  schemerlicht_visitor* visitor;
  schemerlicht_vector macros;
  schemerlicht_vector lisp_macros;
  } schemerlicht_macro_finder_visitor;

static void postvisit_primcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  if (strcmp(e->expr.prim.name.string_ptr, "define-macro") == 0)
    {
    schemerlicht_macro_finder_visitor* vis = (schemerlicht_macro_finder_visitor*)(v->impl);
    schemerlicht_vector_push_back(ctxt, &vis->macros, *e, schemerlicht_expression);
    *e = schemerlicht_init_nop(ctxt);
    }
  else if (strcmp(e->expr.prim.name.string_ptr, "defmacro") == 0)
    {
    schemerlicht_macro_finder_visitor* vis = (schemerlicht_macro_finder_visitor*)(v->impl);
    schemerlicht_vector_push_back(ctxt, &vis->lisp_macros, *e, schemerlicht_expression);
    *e = schemerlicht_init_nop(ctxt);
    }
  }

static schemerlicht_macro_finder_visitor* schemerlicht_macro_finder_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_macro_finder_visitor* v = schemerlicht_new(ctxt, schemerlicht_macro_finder_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->postvisit_primcall = postvisit_primcall;
  schemerlicht_vector_init(ctxt, &v->macros, schemerlicht_expression);
  schemerlicht_vector_init(ctxt, &v->lisp_macros, schemerlicht_expression);
  return v;
  }

static void schemerlicht_macro_finder_visitor_free(schemerlicht_context* ctxt, schemerlicht_macro_finder_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_expression* it = schemerlicht_vector_begin(&v->macros, schemerlicht_expression);
    schemerlicht_expression* it_end = schemerlicht_vector_end(&v->macros, schemerlicht_expression);
    for (; it != it_end; ++it)
      {
      schemerlicht_expression_destroy(ctxt, it);
      }
    it = schemerlicht_vector_begin(&v->lisp_macros, schemerlicht_expression);
    it_end = schemerlicht_vector_end(&v->lisp_macros, schemerlicht_expression);
    for (; it != it_end; ++it)
      {
      schemerlicht_expression_destroy(ctxt, it);
      }
    schemerlicht_vector_destroy(ctxt, &v->macros);
    schemerlicht_vector_destroy(ctxt, &v->lisp_macros);
    schemerlicht_delete(ctxt, v);
    }
  }

static int is_name(schemerlicht_expression* expr)
  {
  if (expr->type == schemerlicht_type_primitive_call)
    return expr->expr.prim.as_object;
  return expr->type == schemerlicht_type_variable ? 1 : 0;
  }

static schemerlicht_string copy_name(schemerlicht_context* ctxt, schemerlicht_expression* expr)
  {
  schemerlicht_string name;
  if (expr->type == schemerlicht_type_primitive_call)
    schemerlicht_string_copy(ctxt, &name, &expr->expr.prim.name);
  else
    schemerlicht_string_copy(ctxt, &name, &expr->expr.var.name);
  return name;
  }

static schemerlicht_program get_macros(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_program prog_out;
  schemerlicht_vector_init(ctxt, &prog_out.expressions, schemerlicht_expression);

  schemerlicht_macro_finder_visitor* v = schemerlicht_macro_finder_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, program);

  schemerlicht_expression* it = schemerlicht_vector_begin(&v->macros, schemerlicht_expression);
  schemerlicht_expression* it_end = schemerlicht_vector_end(&v->macros, schemerlicht_expression);

  for (; it != it_end; ++it)
    {
    schemerlicht_assert(it->type == schemerlicht_type_primitive_call);
    if (it->expr.prim.arguments.vector_size < 2)
      {
      schemerlicht_compile_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS, it->expr.prim.line_nr, it->expr.prim.column_nr, "define-macro expects at least 2 arguments");
      schemerlicht_macro_finder_visitor_free(ctxt, v);
      return prog_out;
      }
    schemerlicht_expression* fun = schemerlicht_vector_begin(&it->expr.prim.arguments, schemerlicht_expression);
    if (fun->type != schemerlicht_type_funcall)
      {
      schemerlicht_compile_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, it->expr.prim.line_nr, it->expr.prim.column_nr, "define-macro invalid first argument");
      schemerlicht_macro_finder_visitor_free(ctxt, v);
      return prog_out;
      }
    schemerlicht_expression* var = schemerlicht_vector_begin(&fun->expr.funcall.fun, schemerlicht_expression);
    if (var->type != schemerlicht_type_variable)
      {
      schemerlicht_compile_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, it->expr.prim.line_nr, it->expr.prim.column_nr, "define-macro invalid first argument");
      schemerlicht_macro_finder_visitor_free(ctxt, v);
      return prog_out;
      }
    int variable_arity = 0;
    schemerlicht_vector variable_names;
    schemerlicht_vector_init(ctxt, &variable_names, schemerlicht_string);
    schemerlicht_string macro_name = copy_name(ctxt, var);
    if (fun->expr.funcall.arguments.vector_size >= 2 && schemerlicht_vector_at(&fun->expr.funcall.arguments, fun->expr.funcall.arguments.vector_size - 2, schemerlicht_expression)->type == schemerlicht_type_literal)
      {
      schemerlicht_expression* lit = schemerlicht_vector_at(&fun->expr.funcall.arguments, fun->expr.funcall.arguments.vector_size - 2, schemerlicht_expression);
      if (lit->type == schemerlicht_type_flonum)
        {
        if (lit->expr.lit.lit.fl.value != 0.f)
          {
          schemerlicht_string_destroy(ctxt, &macro_name);
          schemerlicht_string* sit = schemerlicht_vector_begin(&variable_names, schemerlicht_string);
          schemerlicht_string* sit_end = schemerlicht_vector_end(&variable_names, schemerlicht_string);
          for (; sit != sit_end; ++sit)
            {
            schemerlicht_string_destroy(ctxt, sit);
            }
          schemerlicht_vector_destroy(ctxt, &variable_names);
          schemerlicht_compile_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, it->expr.prim.line_nr, it->expr.prim.column_nr, "define-macro invalid argument");
          schemerlicht_macro_finder_visitor_free(ctxt, v);
          return prog_out;
          }
        variable_arity = 1;
        for (schemerlicht_memsize j = 0; j < fun->expr.funcall.arguments.vector_size; ++j)
          {
          if (j == (fun->expr.funcall.arguments.vector_size - 2))
            continue;
          schemerlicht_expression* arg = schemerlicht_vector_at(&fun->expr.funcall.arguments, j, schemerlicht_expression);
          if (!is_name(arg))
            {
            schemerlicht_string_destroy(ctxt, &macro_name);
            schemerlicht_string* sit = schemerlicht_vector_begin(&variable_names, schemerlicht_string);
            schemerlicht_string* sit_end = schemerlicht_vector_end(&variable_names, schemerlicht_string);
            for (; sit != sit_end; ++sit)
              {
              schemerlicht_string_destroy(ctxt, sit);
              }
            schemerlicht_vector_destroy(ctxt, &variable_names);
            schemerlicht_compile_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, it->expr.prim.line_nr, it->expr.prim.column_nr, "define-macro invalid argument");
            schemerlicht_macro_finder_visitor_free(ctxt, v);
            return prog_out;
            }
          schemerlicht_vector_push_back(ctxt, &variable_names, copy_name(ctxt, arg), schemerlicht_string);
          }
        }
      else
        {
        schemerlicht_compile_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, it->expr.prim.line_nr, it->expr.prim.column_nr, "define-macro invalid argument");
        schemerlicht_macro_finder_visitor_free(ctxt, v);
        return prog_out;
        }
      }
    else
      {
      for (schemerlicht_memsize j = 0; j < fun->expr.funcall.arguments.vector_size; ++j)
        {
        schemerlicht_expression* arg = schemerlicht_vector_at(&fun->expr.funcall.arguments, j, schemerlicht_expression);
        if (!is_name(arg))
          {
          schemerlicht_string_destroy(ctxt, &macro_name);
          schemerlicht_string* sit = schemerlicht_vector_begin(&variable_names, schemerlicht_string);
          schemerlicht_string* sit_end = schemerlicht_vector_end(&variable_names, schemerlicht_string);
          for (; sit != sit_end; ++sit)
            {
            schemerlicht_string_destroy(ctxt, sit);
            }
          schemerlicht_vector_destroy(ctxt, &variable_names);
          schemerlicht_compile_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, it->expr.prim.line_nr, it->expr.prim.column_nr, "define-macro invalid argument");
          schemerlicht_macro_finder_visitor_free(ctxt, v);
          return prog_out;
          }
        schemerlicht_vector_push_back(ctxt, &variable_names, copy_name(ctxt, arg), schemerlicht_string);
        }
      }

    schemerlicht_expression beg = schemerlicht_init_begin(ctxt);
    schemerlicht_expression* prim_arg = schemerlicht_vector_begin(&it->expr.prim.arguments, schemerlicht_expression) + 1;
    schemerlicht_expression* prim_arg_end = schemerlicht_vector_end(&it->expr.prim.arguments, schemerlicht_expression);
    for (; prim_arg != prim_arg_end; ++prim_arg)
      {
      schemerlicht_vector_push_back(ctxt, &beg.expr.beg.arguments, *prim_arg, schemerlicht_expression);
      }
    it->expr.prim.arguments.vector_size = 1; // for clean up

    schemerlicht_expression lam = schemerlicht_init_lambda(ctxt);
    lam.expr.lambda.variable_arity = variable_arity;
    schemerlicht_vector_destroy(ctxt, &lam.expr.lambda.variables);
    lam.expr.lambda.variables = variable_names;
    schemerlicht_vector_push_back(ctxt, &lam.expr.lambda.body, beg, schemerlicht_expression);

    schemerlicht_expression prim = schemerlicht_init_primcall(ctxt);
    schemerlicht_string_init(ctxt, &prim.expr.prim.name, "define");
    schemerlicht_expression macrovar = schemerlicht_init_variable(ctxt);
    macrovar.expr.var.name = macro_name;
    schemerlicht_vector_push_back(ctxt, &prim.expr.prim.arguments, macrovar, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &prim.expr.prim.arguments, lam, schemerlicht_expression);
    schemerlicht_vector_push_back(ctxt, &prog_out.expressions, prim, schemerlicht_expression);
    }

  schemerlicht_macro_finder_visitor_free(ctxt, v);

  return prog_out;
  }

void schemerlicht_expand_macros(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_program pr = get_macros(ctxt, program);
  if (pr.expressions.vector_size > 0)
    {
    schemerlicht_string s = schemerlicht_dump(ctxt, &pr);
    printf("%s\n", s.string_ptr);
    schemerlicht_string_destroy(ctxt, &s);
    schemerlicht_preprocess(ctxt, &pr);
    schemerlicht_expression* expr = schemerlicht_vector_at(&pr.expressions, 0, schemerlicht_expression);
    schemerlicht_function* func = schemerlicht_compile_expression(ctxt, expr);
    schemerlicht_object* res = schemerlicht_run(ctxt, func);
    schemerlicht_vector_push_back(ctxt, &ctxt->lambdas, func, schemerlicht_function*);
    }
  schemerlicht_program_destroy(ctxt, &pr);
  }