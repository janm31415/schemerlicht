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
#include "map.h"
#include "reader.h"
#include "stream.h"
#include "token.h"
#include "vm.h"

#include <string.h>

static schemerlicht_cell expression_to_cell(schemerlicht_context* ctxt, schemerlicht_expression* expr)
  {
  schemerlicht_string s = schemerlicht_dump_expression(ctxt, expr);
  schemerlicht_stream str;
  schemerlicht_memsize len = s.string_length;
  schemerlicht_stream_init(ctxt, &str, len);
  schemerlicht_stream_write(ctxt, &str, s.string_ptr, len, 0);
  schemerlicht_stream_rewind(&str);
  schemerlicht_vector tokens = tokenize(ctxt, &str);
  schemerlicht_stream_close(ctxt, &str);
  schemerlicht_string_destroy(ctxt, &s);
  token* token_it = schemerlicht_vector_begin(&tokens, token);
  token* token_it_end = schemerlicht_vector_end(&tokens, token);
  schemerlicht_cell out_cell = schemerlicht_read_quote(ctxt, &token_it, &token_it_end, 1);
  destroy_tokens_vector(ctxt, &tokens);
  return out_cell;
  }

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

static void add_to_macro_map(schemerlicht_context* ctxt, schemerlicht_string* macro_name, int variable_arity, schemerlicht_memsize number_of_arguments)
  {
  schemerlicht_object key;
  key.type = schemerlicht_object_type_string;
  schemerlicht_string_copy(ctxt, &key.value.s, macro_name);
  schemerlicht_object* value = schemerlicht_map_insert(ctxt, ctxt->macro_map, &key);
  if (value->type != schemerlicht_object_type_undefined) // if macro already exists in map
    {
    // then clean key
    schemerlicht_string_destroy(ctxt, &key.value.s);
    }
  else
    {
    ++ctxt->number_of_macros;
    }
  value->type = schemerlicht_object_type_fixnum;
  value->value.fx = cast(schemerlicht_fixnum, number_of_arguments);
  if (variable_arity)
    {
    value->value.fx = -value->value.fx;
    }
  }

static int find_in_macro_map(schemerlicht_context* ctxt, int* variable_arity, schemerlicht_memsize* number_of_arguments, schemerlicht_string* macro_name)
  {
  schemerlicht_object key;
  key.type = schemerlicht_object_type_string;
  key.value.s = *macro_name;
  schemerlicht_object* value = schemerlicht_map_get(ctxt, ctxt->macro_map, &key);
  if (value)
    {
    if (value->value.fx < 0)
      {
      *variable_arity = 1;
      *number_of_arguments = cast(schemerlicht_memsize, -value->value.fx);
      }
    else
      {
      *variable_arity = 0;
      *number_of_arguments = cast(schemerlicht_memsize, value->value.fx);
      }
    return 1;
    }
  return 0;
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
      if (lit->expr.lit.type == schemerlicht_type_flonum)
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
    add_to_macro_map(ctxt, &macro_name, variable_arity, variable_names.vector_size);
    }

  it = schemerlicht_vector_begin(&v->lisp_macros, schemerlicht_expression);
  it_end = schemerlicht_vector_end(&v->lisp_macros, schemerlicht_expression);

  for (; it != it_end; ++it)
    {
    //(defmacro id formals body ...+)
    //formals = (id ...)
    //          | id
    //          | (id ...+ . id)
    schemerlicht_assert(it->type == schemerlicht_type_primitive_call);
    if (it->expr.prim.arguments.vector_size < 3)
      {
      schemerlicht_compile_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS, it->expr.prim.line_nr, it->expr.prim.column_nr, "defmacro expects at least 2 arguments");
      schemerlicht_macro_finder_visitor_free(ctxt, v);
      return prog_out;
      }
    schemerlicht_expression* var = schemerlicht_vector_begin(&it->expr.prim.arguments, schemerlicht_expression);
    if (var->type != schemerlicht_type_variable)
      {
      schemerlicht_compile_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, it->expr.prim.line_nr, it->expr.prim.column_nr, "defmacro invalid first argument");
      schemerlicht_macro_finder_visitor_free(ctxt, v);
      return prog_out;
      }
    int variable_arity = 0;
    schemerlicht_vector variable_names;
    schemerlicht_vector_init(ctxt, &variable_names, schemerlicht_string);
    schemerlicht_string macro_name = copy_name(ctxt, var);
    schemerlicht_expression* arg1 = schemerlicht_vector_at(&it->expr.prim.arguments, 1, schemerlicht_expression);
    if (arg1->type == schemerlicht_type_funcall)
      {
      schemerlicht_expression* fun = arg1;
      schemerlicht_expression* funfun = schemerlicht_vector_begin(&fun->expr.funcall.fun, schemerlicht_expression);
      if (funfun->type != schemerlicht_type_variable)
        {
        schemerlicht_string_destroy(ctxt, &macro_name);
        schemerlicht_vector_destroy(ctxt, &variable_names);
        schemerlicht_compile_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, it->expr.prim.line_nr, it->expr.prim.column_nr, "defmacro invalid first argument");
        schemerlicht_macro_finder_visitor_free(ctxt, v);
        return prog_out;
        }
      schemerlicht_vector_push_back(ctxt, &variable_names, copy_name(ctxt, funfun), schemerlicht_string);
      if (fun->expr.funcall.arguments.vector_size >= 2 && schemerlicht_vector_at(&fun->expr.funcall.arguments, fun->expr.funcall.arguments.vector_size - 2, schemerlicht_expression)->type == schemerlicht_type_literal)
        {
        schemerlicht_expression* lit = schemerlicht_vector_at(&fun->expr.funcall.arguments, fun->expr.funcall.arguments.vector_size - 2, schemerlicht_expression);
        if (lit->expr.lit.type == schemerlicht_type_flonum)
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
      }
    else if (arg1->type == schemerlicht_type_variable)
      {
      variable_arity = 1;
      schemerlicht_vector_push_back(ctxt, &variable_names, copy_name(ctxt, arg1), schemerlicht_string);
      }

    schemerlicht_expression beg = schemerlicht_init_begin(ctxt);
    schemerlicht_expression* prim_arg = schemerlicht_vector_begin(&it->expr.prim.arguments, schemerlicht_expression) + 2;
    schemerlicht_expression* prim_arg_end = schemerlicht_vector_end(&it->expr.prim.arguments, schemerlicht_expression);
    for (; prim_arg != prim_arg_end; ++prim_arg)
      {
      schemerlicht_vector_push_back(ctxt, &beg.expr.beg.arguments, *prim_arg, schemerlicht_expression);
      }
    it->expr.prim.arguments.vector_size = 2; // for clean up

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
    add_to_macro_map(ctxt, &macro_name, variable_arity, variable_names.vector_size);
    }

  schemerlicht_macro_finder_visitor_free(ctxt, v);

  return prog_out;
  }

typedef struct schemerlicht_macro_expander_visitor
  {
  int macros_expanded;
  schemerlicht_visitor* visitor;
  } schemerlicht_macro_expander_visitor;


static int previsit_funcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  schemerlicht_macro_expander_visitor* vis = (schemerlicht_macro_expander_visitor*)(v->impl);
  schemerlicht_expression* var = schemerlicht_vector_begin(&e->expr.funcall.fun, schemerlicht_expression);
  if (var->type == schemerlicht_type_variable)
    {
    int variable_arity;
    schemerlicht_memsize number_of_arguments;
    if (find_in_macro_map(ctxt, &variable_arity, &number_of_arguments, &var->expr.var.name))
      {
      if (variable_arity == 0)
        {
        if (e->expr.funcall.arguments.vector_size != number_of_arguments)
          {
          schemerlicht_compile_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS, e->expr.funcall.line_nr, e->expr.funcall.column_nr, "macro has invalid number of arguments");
          return 0;
          }
        }
      else
        {
        if (e->expr.funcall.arguments.vector_size < number_of_arguments)
          {
          schemerlicht_compile_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS, e->expr.funcall.line_nr, e->expr.funcall.column_nr, "macro has invalid number of arguments");
          return 0;
          }
        }

      schemerlicht_expression* arg_it = schemerlicht_vector_begin(&e->expr.funcall.arguments, schemerlicht_expression);
      schemerlicht_expression* arg_it_end = schemerlicht_vector_end(&e->expr.funcall.arguments, schemerlicht_expression);

      for (; arg_it != arg_it_end; ++arg_it)
        {
        schemerlicht_expression q = schemerlicht_init_quote(ctxt);
        q.expr.quote.arg = expression_to_cell(ctxt, arg_it);
        schemerlicht_expression_destroy(ctxt, arg_it);
        *arg_it = q;
        }

      schemerlicht_program pr;
      schemerlicht_vector_init(ctxt, &pr.expressions, schemerlicht_expression);
      schemerlicht_vector_push_back(ctxt, &pr.expressions, *e, schemerlicht_expression);
      schemerlicht_map* macro_map_store = ctxt->macro_map;
      schemerlicht_memsize number_of_macros_store = ctxt->number_of_macros;
      ctxt->number_of_macros = 0;
      ctxt->macro_map = schemerlicht_map_new(ctxt, 0, 1); // remove all macros, as the macros now exist in the environment
      schemerlicht_preprocess(ctxt, &pr);      
      schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &pr);
      schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program);
      schemerlicht_print_any_error(ctxt);
      schemerlicht_string s = schemerlicht_object_to_string(ctxt, res, 0);      
      schemerlicht_compiled_program_destroy(ctxt, &compiled_program);
      schemerlicht_map_keys_free(ctxt, ctxt->macro_map);
      schemerlicht_map_free(ctxt, ctxt->macro_map);
      ctxt->macro_map = macro_map_store; // restore original macro map
      ctxt->number_of_macros = number_of_macros_store;

      schemerlicht_stream str;
      schemerlicht_memsize len = s.string_length;
      schemerlicht_stream_init(ctxt, &str, len);
      schemerlicht_stream_write(ctxt, &str, s.string_ptr, len, 0);
      schemerlicht_stream_rewind(&str);
      schemerlicht_vector tokens = tokenize(ctxt, &str);
      schemerlicht_stream_close(ctxt, &str);
      schemerlicht_string_destroy(ctxt, &s);
      schemerlicht_program result = make_program(ctxt, &tokens);
      destroy_tokens_vector(ctxt, &tokens);
      schemerlicht_program_destroy(ctxt, &pr);

      if (result.expressions.vector_size == 0)
        {
        *e = schemerlicht_init_nop(ctxt);
        schemerlicht_vector_destroy(ctxt, &result.expressions);
        }
      else if (result.expressions.vector_size == 1)
        {
        *e = *schemerlicht_vector_begin(&result.expressions, schemerlicht_expression);
        schemerlicht_vector_destroy(ctxt, &result.expressions);
        }
      else
        {
        *e = schemerlicht_init_begin(ctxt);
        schemerlicht_vector_destroy(ctxt, &e->expr.beg.arguments);
        e->expr.beg.arguments = result.expressions;
        }
      vis->macros_expanded = 1;
      return 0;
      }
    }

  return 1;
  }

static schemerlicht_macro_expander_visitor* schemerlicht_macro_expander_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_macro_expander_visitor* v = schemerlicht_new(ctxt, schemerlicht_macro_expander_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->previsit_funcall = previsit_funcall;
  v->macros_expanded = 0;
  return v;
  }

static void schemerlicht_macro_expander_visitor_free(schemerlicht_context* ctxt, schemerlicht_macro_expander_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }


static int expand_existing_macros(schemerlicht_context* ctxt, schemerlicht_program* prog)
  {
  if (ctxt->number_of_macros == 0)
    return 0;
  schemerlicht_macro_expander_visitor* v = schemerlicht_macro_expander_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, prog);
  int macros_expanded = v->macros_expanded;
  schemerlicht_macro_expander_visitor_free(ctxt, v);
  return macros_expanded;
  }

void schemerlicht_expand_macros(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  int macros_expanded = 1;
  while (macros_expanded)
    {
    macros_expanded = 0;
    schemerlicht_program macro_program = get_macros(ctxt, program);
    if (macro_program.expressions.vector_size > 0)
      {
#if 0
      schemerlicht_string s = schemerlicht_dump(ctxt, &macro_program);
      printf("%s\n", s.string_ptr);
      schemerlicht_string_destroy(ctxt, &s);
#endif

      schemerlicht_preprocess(ctxt, &macro_program);
      schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &macro_program);
      schemerlicht_run_program(ctxt, &compiled_program);
      schemerlicht_print_any_error(ctxt);
      schemerlicht_compiled_program_register(ctxt, &compiled_program);
      }
    macros_expanded = expand_existing_macros(ctxt, program);
    schemerlicht_print_any_error(ctxt);
    schemerlicht_program_destroy(ctxt, &macro_program);
    }
  }
