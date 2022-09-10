#include "sparser.h"
#include "svector.h"
#include "stoken.h"
#include "serror.h"
#include "scontext.h"
#include "svisitor.h"

#include <string.h>
#include <stdlib.h>

schemerlicht_expression schemerlicht_make_expression(schemerlicht_context* ctxt, token** token_it, token** token_it_end);

static token make_bad_token()
  {
  token t;
  t.type = SCHEMERLICHT_T_BAD;
  return t;
  }

token popped_token;

static void invalidate_popped()
  {
  popped_token = make_bad_token();
  }

enum schemerlicht_expression_type
  {
  schemerlicht_et_primitive_call,
  schemerlicht_et_begin,
  schemerlicht_et_case,
  schemerlicht_et_cond,
  schemerlicht_et_do,
  schemerlicht_et_foreign_call,
  schemerlicht_et_if,
  schemerlicht_et_lambda,
  schemerlicht_et_let,
  schemerlicht_et_letrec,
  schemerlicht_et_let_star,
  schemerlicht_et_quote,
  schemerlicht_et_quasiquote,
  schemerlicht_et_unquote,
  schemerlicht_et_unquote_splicing,
  schemerlicht_et_set,
  schemerlicht_et_funcall_or_variable
  };

static int current_token_type(token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    return SCHEMERLICHT_T_BAD;
  return (*token_it)->type;
  }

static schemerlicht_expression make_nop()
  {
  schemerlicht_parsed_nop nop;
  nop.column_nr = -1;
  nop.line_nr = -1;
  nop.tail_position = 0;
  nop.filename.string_ptr = 0;
  nop.filename.string_capacity = 0;
  nop.filename.string_length = 0;
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_nop;
  expr.expr.nop = nop;
  return expr;
  }

static void token_next(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
    }
  else
    {
    popped_token = (**token_it);
    ++(*token_it);
    }
  }

static void token_require(schemerlicht_context* ctxt, token** token_it, token** token_it_end, const char* required)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
    }
  else
    {
    if (strcmp((*token_it)->info.value.string_ptr, required) != 0)
      {
      schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, required);
      }
    popped_token = (**token_it);
    ++(*token_it);
    }
  }

static int next_token_equals(token** token_it, token** token_it_end, const char* expected)
  {
  if (*token_it == *token_it_end)
    return 0;
  token* it = *token_it;
  ++it;
  if (it == *token_it_end)
    return 0;
  return strcmp(it->info.value.string_ptr, expected) == 0;
  }

static int current_token_equals(token** token_it, const char* expected)
  {
  if ((*token_it)->type == SCHEMERLICHT_T_FIXNUM || (*token_it)->type == SCHEMERLICHT_T_FLONUM)
    return 0;
  return strcmp((*token_it)->info.value.string_ptr, expected) == 0;
  }

static schemerlicht_string make_empty_string()
  {
  schemerlicht_string s;
  s.string_capacity = 0;
  s.string_length = 0;
  s.string_ptr = NULL;
  return s;
  }

static schemerlicht_parsed_begin initialize_begin(schemerlicht_context* ctxt)
  {
  schemerlicht_parsed_begin b;
  b.filename = make_empty_string();
  schemerlicht_vector_init(ctxt, &b.arguments, schemerlicht_expression);
  return b;
  }

schemerlicht_parsed_funcall initialize_funcall(schemerlicht_context* ctxt)
  {
  schemerlicht_parsed_funcall f;
  f.filename = make_empty_string();
  schemerlicht_vector_init(ctxt, &f.arguments, schemerlicht_expression);
  schemerlicht_vector_init(ctxt, &f.fun, schemerlicht_expression);
  return f;
  }

static schemerlicht_expression make_begin(schemerlicht_context* ctxt, token** token_it, token** token_it_end, int implicit)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
    }
  schemerlicht_parsed_begin b = initialize_begin(ctxt);
  b.line_nr = (*token_it)->line_nr;
  b.column_nr = (*token_it)->column_nr;
  if (!implicit)
    {
    if (strcmp((*token_it)->info.value.string_ptr, "begin") != 0)
      schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, b.line_nr, b.column_nr, "begin");
    token_next(ctxt, token_it, token_it_end);
    }
  invalidate_popped();
  while (!current_token_equals(token_it, ")"))
    {
    schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
    schemerlicht_vector_push_back(ctxt, &b.arguments, e, schemerlicht_expression);
    if (*token_it == *token_it_end)
      schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, b.line_nr, b.column_nr, ")");
    }
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_begin;
  expr.expr.beg = b;
  return expr;
  }

static schemerlicht_expression make_quote(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  UNUSED(token_it);
  UNUSED(token_it_end);
  schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
  return make_nop();
  }

static schemerlicht_expression make_quasiquote(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  UNUSED(token_it);
  UNUSED(token_it_end);
  schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
  return make_nop();
  }

static schemerlicht_expression make_unquote(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  UNUSED(token_it);
  UNUSED(token_it_end);
  schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
  return make_nop();
  }

static schemerlicht_expression make_unquote_splicing(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  UNUSED(token_it);
  UNUSED(token_it_end);
  schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
  return make_nop();
  }

static schemerlicht_expression make_case(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  UNUSED(token_it);
  UNUSED(token_it_end);
  schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
  return make_nop();
  }

static schemerlicht_expression make_cond(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  UNUSED(token_it);
  UNUSED(token_it_end);
  schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
  return make_nop();
  }

static schemerlicht_expression make_do(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  UNUSED(token_it);
  UNUSED(token_it_end);
  schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
  return make_nop();
  }

static schemerlicht_expression make_foreign_call(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  UNUSED(token_it);
  UNUSED(token_it_end);
  schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
  return make_nop();
  }

static schemerlicht_expression make_if(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  UNUSED(token_it);
  UNUSED(token_it_end);
  schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
  return make_nop();
  }

static schemerlicht_expression make_lambda(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  UNUSED(token_it);
  UNUSED(token_it_end);
  schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
  return make_nop();
  }

static schemerlicht_expression make_let(schemerlicht_context* ctxt, token** token_it, token** token_it_end, enum schemerlicht_binding_type bt)
  {
  UNUSED(token_it);
  UNUSED(token_it_end);
  UNUSED(bt);
  schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
  return make_nop();
  }

static schemerlicht_expression make_primitive_call(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  UNUSED(token_it);
  UNUSED(token_it_end);
  schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
  return make_nop();
  }

static schemerlicht_expression make_set(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  UNUSED(token_it);
  UNUSED(token_it_end);
  schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
  return make_nop();
  }

static schemerlicht_expression make_variable(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  UNUSED(token_it);
  UNUSED(token_it_end);
  schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
  return make_nop();
  }

static schemerlicht_expression make_fun(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  UNUSED(token_it);
  UNUSED(token_it_end);
  schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
  return make_nop();
  }

static schemerlicht_expression make_literal(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
    //*token_it = *token_it_end;
    //return make_nop();
    }
  switch (current_token_type(token_it, token_it_end))
    {
    case SCHEMERLICHT_T_LEFT_ROUND_BRACKET:
    {
    schemerlicht_parsed_nil n;
    n.line_nr = (*token_it)->line_nr;
    n.column_nr = (*token_it)->column_nr;
    n.filename = make_empty_string();
    schemerlicht_expression expr;
    expr.type = schemerlicht_type_literal;
    expr.expr.lit.type = schemerlicht_type_nil;
    expr.expr.lit.lit.nil = n;    
    token_next(ctxt, token_it, token_it_end);
    token_require(ctxt, token_it, token_it_end, ")");
    return expr;
    }
    case SCHEMERLICHT_T_FIXNUM:
    {
    schemerlicht_parsed_fixnum f;
    f.line_nr = (*token_it)->line_nr;
    f.column_nr = (*token_it)->column_nr;
    f.value = (*token_it)->info.fixnum;
    f.filename = make_empty_string();
    schemerlicht_expression expr;
    expr.type = schemerlicht_type_literal;
    expr.expr.lit.type = schemerlicht_type_fixnum;
    expr.expr.lit.lit.fx = f;
    token_next(ctxt, token_it, token_it_end);
    return expr;
    }
    case SCHEMERLICHT_T_FLONUM:
    {
    schemerlicht_parsed_flonum f;
    f.line_nr = (*token_it)->line_nr;
    f.column_nr = (*token_it)->column_nr;
    f.value = (*token_it)->info.flonum;
    f.filename = make_empty_string();
    schemerlicht_expression expr;
    expr.type = schemerlicht_type_literal;
    expr.expr.lit.type = schemerlicht_type_flonum;
    expr.expr.lit.lit.fl = f;
    token_next(ctxt, token_it, token_it_end);
    return expr;
    }
    case SCHEMERLICHT_T_STRING:
    {
    schemerlicht_parsed_string s;
    s.line_nr = (*token_it)->line_nr;
    s.column_nr = (*token_it)->column_nr;
    s.filename = make_empty_string();
    schemerlicht_string_copy(ctxt, &s.value, &(*token_it)->info.value);
    schemerlicht_expression expr;
    expr.type = schemerlicht_type_literal;
    expr.expr.lit.type = schemerlicht_type_string;
    expr.expr.lit.lit.str = s;
    token_next(ctxt, token_it, token_it_end);
    return expr;
    }
    case SCHEMERLICHT_T_SYMBOL:
    {
    int ln = (*token_it)->line_nr;
    int cn = (*token_it)->column_nr;
    if (current_token_equals(token_it, "#t"))
      {
      schemerlicht_parsed_true t;
      t.line_nr = ln;
      t.column_nr = cn;
      t.filename = make_empty_string();
      schemerlicht_expression expr;
      expr.type = schemerlicht_type_literal;
      expr.expr.lit.type = schemerlicht_type_true;
      expr.expr.lit.lit.t = t;
      token_next(ctxt, token_it, token_it_end);
      return expr;
      }
    else if (current_token_equals(token_it, "#f"))
      {
      schemerlicht_parsed_false f;
      f.line_nr = ln;
      f.column_nr = cn;
      f.filename = make_empty_string();
      schemerlicht_expression expr;
      expr.type = schemerlicht_type_literal;
      expr.expr.lit.type = schemerlicht_type_false;
      expr.expr.lit.lit.f = f;
      token_next(ctxt, token_it, token_it_end);
      return expr;
      }
    else
      {
      schemerlicht_string s = (*token_it)->info.value;
      if (s.string_length >= 2 && (*schemerlicht_string_at(&s, 0) == '#') && (*schemerlicht_string_at(&s, 1) == '\\'))
        {
        schemerlicht_string substr;
        schemerlicht_string_init_ranged(ctxt, &substr, s.string_ptr + 2, s.string_ptr + s.string_length);
        schemerlicht_parsed_character ch;
        ch.line_nr = ln;
        ch.column_nr = cn;
        ch.filename = make_empty_string();
        if (substr.string_length == 1)
          {
          ch.value = substr.string_ptr[0];
          }
        else if (substr.string_length == 0)
          {
          ch.value = 32;
          }
        else
          {
          ch.value = 0;
          switch (substr.string_ptr[0])
            {
            case 'b': if (strcmp(substr.string_ptr, "backspace") == 0) ch.value = 8; break;
            case 't': if (strcmp(substr.string_ptr, "tab") == 0) ch.value = 9; break;
            case 'n': if (strcmp(substr.string_ptr, "newline") == 0) ch.value = 10; break;
            case 'l': if (strcmp(substr.string_ptr, "linefeed") == 0) ch.value = 10; break;
            case 'v': if (strcmp(substr.string_ptr, "vtab") == 0) ch.value = 11; break;
            case 'p': if (strcmp(substr.string_ptr, "page") == 0) ch.value = 12; break;
            case 'r': if (strcmp(substr.string_ptr, "return") == 0) ch.value = 13; break;
            case 's': if (strcmp(substr.string_ptr, "space") == 0) ch.value = 32; break;
            }
          if (ch.value == 0)
            {
            ch.value = (char)(atoi(substr.string_ptr));
            }
          }
        schemerlicht_string_destroy(ctxt, &substr);
        schemerlicht_expression expr;
        expr.type = schemerlicht_type_literal;
        expr.expr.lit.type = schemerlicht_type_character;
        expr.expr.lit.lit.ch = ch;
        token_next(ctxt, token_it, token_it_end);
        return expr;
        }
      else if (s.string_length >= 2 && (*schemerlicht_string_at(&s, 0) == '#') && (*schemerlicht_string_at(&s, 1) == 'x'))
        {
        //hexadecimal
        schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
        }
      else if (s.string_length >= 2 && (*schemerlicht_string_at(&s, 0) == '#') && (*schemerlicht_string_at(&s, 1) == 'b'))
        {
        //binary
        schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
        }
      else if (s.string_length >= 2 && (*schemerlicht_string_at(&s, 0) == '#') && (*schemerlicht_string_at(&s, 1) == 'o'))
        {
        //octal
        schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
        }
      }
    }
    default:
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
    }
  return make_nop();
  }

static enum schemerlicht_expression_type find_current_expression_type(schemerlicht_context* ctxt, token** token_it)
  {
  schemerlicht_object* obj = schemerlicht_map_get_string(ctxt->global->expression_map, (*token_it)->info.value.string_ptr);
  if (obj->type == schemerlicht_object_type_nil)
    return schemerlicht_et_funcall_or_variable;
  schemerlicht_assert(obj->type == schemerlicht_object_type_fixnum);
  return obj->value.fx;
  }

schemerlicht_expression schemerlicht_make_expression(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
    }

  switch (current_token_type(token_it, token_it_end))
    {
    case SCHEMERLICHT_T_LEFT_ROUND_BRACKET:
    {
    if (next_token_equals(token_it, token_it_end, ")"))
      {
      return make_literal(ctxt, token_it, token_it_end);
      }
    int function_call = (popped_token.type == SCHEMERLICHT_T_LEFT_ROUND_BRACKET);
    int ln = (*token_it)->line_nr;
    int cn = (*token_it)->column_nr;
    token_next(ctxt, token_it, token_it_end);
    if (function_call)
      {
      schemerlicht_parsed_funcall f = initialize_funcall(ctxt);
      f.line_nr = ln;
      f.column_nr = cn;
      schemerlicht_vector_push_back(ctxt, &f.fun, schemerlicht_make_expression(ctxt, token_it, token_it_end), schemerlicht_expression);
      token_require(ctxt, token_it, token_it_end, ")");
      if (!current_token_equals(token_it, ")"))
        {
        while (!current_token_equals(token_it, ")"))
          {
          schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
          schemerlicht_vector_push_back(ctxt, &f.arguments, e, schemerlicht_expression);
          if (*token_it == *token_it_end)
            schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, f.line_nr, f.column_nr, ")");
          }
        }      
      schemerlicht_expression output_expr;
      output_expr.type = schemerlicht_type_funcall;
      output_expr.expr.funcall = f;
      schemerlicht_expression* fun_expr = schemerlicht_vector_at(ctxt, &f.fun, 0, schemerlicht_expression);
      if (f.arguments.vector_size==0 && fun_expr->type == schemerlicht_type_literal)
        {
        // this is a literal, not a function
        output_expr.type = schemerlicht_type_literal;
        output_expr.expr.lit = fun_expr->expr.lit;
        schemerlicht_vector_destroy(ctxt, &f.arguments);
        schemerlicht_vector_destroy(ctxt, &f.fun);
        schemerlicht_string_destroy(ctxt, &f.filename);
        }
      return output_expr;
      }
    else
      {
      schemerlicht_expression expr = schemerlicht_make_expression(ctxt, token_it, token_it_end);
      if (current_token_equals(token_it, ")"))
        {
        //token_require(ctxt, token_it, token_it_end, ")");
        token_next(ctxt, token_it, token_it_end);
        return expr;
        }
      else
        {
        schemerlicht_parsed_funcall f = initialize_funcall(ctxt);
        f.line_nr = ln;
        f.column_nr = cn;
        schemerlicht_vector_push_back(ctxt, &f.fun, expr, schemerlicht_expression);
        while (!current_token_equals(token_it, ")"))
          {
          schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
          schemerlicht_vector_push_back(ctxt, &f.arguments, e, schemerlicht_expression);
          if (*token_it == *token_it_end)
            schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, f.line_nr, f.column_nr, ")");
          }
        token_require(ctxt, token_it, token_it_end, ")");
        schemerlicht_expression output_expr;
        output_expr.type = schemerlicht_type_funcall;
        output_expr.expr.funcall = f;
        return output_expr;
        }
      }
    }
    case SCHEMERLICHT_T_RIGHT_ROUND_BRACKET:
    {
    schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr);
    }
    case SCHEMERLICHT_T_BAD:
    {
    schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr);
    }
    case SCHEMERLICHT_T_FIXNUM:
      return make_literal(ctxt, token_it, token_it_end);
    case SCHEMERLICHT_T_FLONUM:
      return make_literal(ctxt, token_it, token_it_end);
    case SCHEMERLICHT_T_STRING:
      return make_literal(ctxt, token_it, token_it_end);
    case SCHEMERLICHT_T_SYMBOL:
      if (current_token_equals(token_it, "#undefined"))
        return make_nop();
      return make_literal(ctxt, token_it, token_it_end);
    case SCHEMERLICHT_T_QUOTE:
    {
    return make_quote(ctxt, token_it, token_it_end);
    }
    case SCHEMERLICHT_T_BACKQUOTE:
    {
    return make_quasiquote(ctxt, token_it, token_it_end);
    }
    case SCHEMERLICHT_T_UNQUOTE:
    {
    return make_unquote(ctxt, token_it, token_it_end);
    }
    case SCHEMERLICHT_T_UNQUOTE_SPLICING:
    {
    return make_unquote_splicing(ctxt, token_it, token_it_end);
    }
    case SCHEMERLICHT_T_ID:
    {
    enum schemerlicht_expression_type t = find_current_expression_type(ctxt, token_it);
    switch (t)
      {
      case schemerlicht_et_begin: return make_begin(ctxt, token_it, token_it_end, 0 /*false*/);
      case schemerlicht_et_case: return make_case(ctxt, token_it, token_it_end);
      case schemerlicht_et_cond: return make_cond(ctxt, token_it, token_it_end);
      case schemerlicht_et_do: return make_do(ctxt, token_it, token_it_end);
      case schemerlicht_et_foreign_call: return make_foreign_call(ctxt, token_it, token_it_end);
      case schemerlicht_et_if: return make_if(ctxt, token_it, token_it_end);
      case schemerlicht_et_lambda: return make_lambda(ctxt, token_it, token_it_end);
      case schemerlicht_et_let: return make_let(ctxt, token_it, token_it_end, schemerlicht_bt_let);
      case schemerlicht_et_letrec: return make_let(ctxt, token_it, token_it_end, schemerlicht_bt_letrec);
      case schemerlicht_et_let_star: return make_let(ctxt, token_it, token_it_end, schemerlicht_bt_let_star);
      case schemerlicht_et_quote: return make_quote(ctxt, token_it, token_it_end);
      case schemerlicht_et_quasiquote: return make_quasiquote(ctxt, token_it, token_it_end);
      case schemerlicht_et_unquote: return make_unquote(ctxt, token_it, token_it_end);
      case schemerlicht_et_unquote_splicing: return make_unquote_splicing(ctxt, token_it, token_it_end);
      case schemerlicht_et_primitive_call: return make_primitive_call(ctxt, token_it, token_it_end);
      case schemerlicht_et_set: return make_set(ctxt, token_it, token_it_end);
      case schemerlicht_et_funcall_or_variable:
      {
      if (popped_token.type == SCHEMERLICHT_T_LEFT_ROUND_BRACKET)
        {
        invalidate_popped();
        return make_fun(ctxt, token_it, token_it_end);
        }
      return make_variable(ctxt, token_it, token_it_end);
      }
      default:
        schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED, (*token_it)->line_nr, (*token_it)->column_nr);
      }
    }
    default:
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
    }
  return make_nop();
  }

schemerlicht_program make_program(schemerlicht_context* ctxt, schemerlicht_vector* tokens)
  {
  invalidate_popped();
  schemerlicht_program prog;
  schemerlicht_vector_init(ctxt, &prog.expressions, schemerlicht_expression);

  token* token_it = schemerlicht_vector_begin(tokens, token);
  token* token_it_end = schemerlicht_vector_end(tokens, token);

  for (; token_it != token_it_end;)
    {
    schemerlicht_expression expr = schemerlicht_make_expression(ctxt, &token_it, &token_it_end);
    schemerlicht_vector_push_back(ctxt, &prog.expressions, expr, schemerlicht_expression);
    }

  return prog;
  }

static void postvisit_program(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_program* p)
  {
  UNUSED(v);
  schemerlicht_vector_destroy(ctxt, &p->expressions);
  }
static void postvisit_begin(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_vector_destroy(ctxt, &e->expr.beg.arguments);
  schemerlicht_string_destroy(ctxt, &e->expr.beg.filename);
  }
static void visit_string(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &(e->expr.lit.lit.str.value));
  schemerlicht_string_destroy(ctxt, &e->expr.lit.lit.str.filename);
  }
static void visit_symbol(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &(e->expr.lit.lit.sym.value));
  schemerlicht_string_destroy(ctxt, &e->expr.lit.lit.sym.filename);
  }

typedef struct schemerlicht_program_destroy_visitor
  {
  schemerlicht_visitor* visitor;
  } schemerlicht_program_destroy_visitor;

void schemerlicht_program_destroy(schemerlicht_context* ctxt, schemerlicht_program* p)
  {
  schemerlicht_program_destroy_visitor destroyer;
  destroyer.visitor = schemerlicht_visitor_new(ctxt, &destroyer);

  destroyer.visitor->visit_string = visit_string;
  destroyer.visitor->visit_symbol = visit_symbol;
  destroyer.visitor->postvisit_begin = postvisit_begin;
  destroyer.visitor->postvisit_program = postvisit_program;
  schemerlicht_visit_program(ctxt, destroyer.visitor, p);

  destroyer.visitor->destroy(ctxt, destroyer.visitor);
  }

static void map_insert(schemerlicht_context* ctxt, schemerlicht_map* m, const char* str, enum schemerlicht_expression_type value)
  {
  schemerlicht_object* obj = schemerlicht_map_insert_string(ctxt, m, str);
  obj->type = schemerlicht_object_type_fixnum;
  obj->value.fx = (schemerlicht_fixnum)value;
  }

schemerlicht_map* generate_expression_map(schemerlicht_context* ctxt)
  {
  schemerlicht_map* m = schemerlicht_map_new(ctxt, 0, 8);
  map_insert(ctxt, m, "begin", schemerlicht_et_begin);
  map_insert(ctxt, m, "case", schemerlicht_et_case);
  map_insert(ctxt, m, "cond", schemerlicht_et_cond);
  map_insert(ctxt, m, "do", schemerlicht_et_do);
  map_insert(ctxt, m, "foreign-call", schemerlicht_et_foreign_call);
  map_insert(ctxt, m, "if", schemerlicht_et_if);
  map_insert(ctxt, m, "lambda", schemerlicht_et_lambda);
  map_insert(ctxt, m, "let", schemerlicht_et_let);
  map_insert(ctxt, m, "letrec", schemerlicht_et_letrec);
  map_insert(ctxt, m, "let*", schemerlicht_et_let_star);
  map_insert(ctxt, m, "quote", schemerlicht_et_quote);
  map_insert(ctxt, m, "quasiquote", schemerlicht_et_quasiquote);
  map_insert(ctxt, m, "set!", schemerlicht_et_set);

  map_insert(ctxt, m, "+", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "-", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "*", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "/", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "=", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "!=", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "<", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "<=", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, ">", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, ">=", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "%allocate-symbol", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "%slot-ref", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "%slot-set!", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "%quiet-undefined", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "%undefined", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "add1", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "and", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "%apply", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "arithmetic-shift", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "assoc", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "assv", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "assq", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "bitwise-and", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "bitwise-not", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "bitwise-or", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "bitwise-xor", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "boolean?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "car", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "cdr", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "char->fixnum", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "char?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "char=?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "char>?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "char<?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "char>=?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "char<=?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "close-file", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "closure", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "closure?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "closure-ref", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "cons", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "compare-strings", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "compare-strings-ci", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "current-seconds", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "current-milliseconds", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "define", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "defmacro", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "define-macro", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "delay", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "eq?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "eqv?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "%eqv?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "eof-object?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "equal?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "%error", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "%eval", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "file-exists?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fixnum->char", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fixnum->flonum", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fixnum?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fixnum-expt", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "flonum?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "flonum->fixnum", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "flonum-expt", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "%flush-output-port", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fx=?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fx>?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fx<?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fx>=?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fx<=?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fx+", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fx-", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fx*", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fx/", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fxadd1", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fxsub1", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fxzero?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "getenv", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "halt", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "ieee754-sign", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "ieee754-exponent", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "ieee754-mantissa", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "ieee754-sin", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "ieee754-cos", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "ieee754-tan", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "ieee754-asin", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "ieee754-acos", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "ieee754-atan1", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "ieee754-log", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "ieee754-round", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "ieee754-sqrt", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "ieee754-truncate", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "ieee754-pi", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "include", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "input-port?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "load", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "length", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "list", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "make-port", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "%make-promise", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "make-string", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "make-vector", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "max", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "member", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "memv", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "memq", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "min", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "not", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "null?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "num2str", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "or", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "open-file", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "output-port?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "pair?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "%peek-char", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "port?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "procedure?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "promise?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "putenv", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "quotient", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "%read-char", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "reclaim", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "reclaim-garbage", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "remainder", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "set-car!", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "set-cdr!", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "str2num", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-append1", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-copy", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-fill!", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-hash", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-length", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-ref", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-set!", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "substring", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "symbol?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "symbol->string", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "sub1", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "unless", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "vector", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "vector?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "vector-fill!", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "vector-length", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "vector-ref", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "vector-set!", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "when", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "%write-char", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "%write-string", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "zero?", schemerlicht_et_primitive_call);

  return m;
  }