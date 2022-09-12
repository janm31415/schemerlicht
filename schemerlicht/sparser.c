#include "sparser.h"
#include "svector.h"
#include "stoken.h"
#include "serror.h"
#include "scontext.h"
#include "svisitor.h"

#include <string.h>
#include <stdlib.h>

schemerlicht_expression schemerlicht_make_expression(schemerlicht_context* ctxt, token** token_it, token** token_it_end);

static schemerlicht_string make_dummy_filename(schemerlicht_context* ctxt)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "dummy");
  return s;
  }

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
  nop.filename = make_null_string();
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
    if (strcmp((*token_it)->value.string_ptr, required) != 0)
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
  return strcmp(it->value.string_ptr, expected) == 0;
  }

static int current_token_equals(token** token_it, const char* expected)
  {
  if ((*token_it)->type == SCHEMERLICHT_T_FIXNUM || (*token_it)->type == SCHEMERLICHT_T_FLONUM)
    return 0;
  return strcmp((*token_it)->value.string_ptr, expected) == 0;
  }

static int require_left_square_bracket(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
    }
  int res = ((*token_it)->type == SCHEMERLICHT_T_LEFT_SQUARE_BRACKET);
  if (!(res || ((*token_it)->type == SCHEMERLICHT_T_LEFT_ROUND_BRACKET)))
    {
    schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, "[");
    }
  ++(*token_it);
  invalidate_popped();
  return res;
  }

static void require_right_square_bracket(schemerlicht_context* ctxt, token** token_it, token** token_it_end, int result_of_require_left_square_bracket)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
    }
  if (result_of_require_left_square_bracket && ((*token_it)->type != SCHEMERLICHT_T_RIGHT_SQUARE_BRACKET))
    schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, "]");
  if (!result_of_require_left_square_bracket && ((*token_it)->type != SCHEMERLICHT_T_RIGHT_ROUND_BRACKET))
    schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, ")");
  token_next(ctxt, token_it, token_it_end);  
  }

static schemerlicht_expression make_begin(schemerlicht_context* ctxt, token** token_it, token** token_it_end, int implicit)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
    }
  schemerlicht_parsed_begin b;
  b.filename = make_dummy_filename(ctxt);
  schemerlicht_vector_init(ctxt, &b.arguments, schemerlicht_expression);
  b.line_nr = (*token_it)->line_nr;
  b.column_nr = (*token_it)->column_nr;
  if (!implicit)
    {
    if (strcmp((*token_it)->value.string_ptr, "begin") != 0)
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
  if (*token_it == *token_it_end)    
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID && current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_QUOTE)    
    schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, "'");
  schemerlicht_parsed_quote q;
  q.line_nr = (*token_it)->line_nr;
  q.column_nr = (*token_it)->column_nr;
  q.filename = make_dummy_filename(ctxt);
  q.qt = schemerlicht_qt_quote;
  if ((strcmp((*token_it)->value.string_ptr, "quote") != 0) && (strcmp((*token_it)->value.string_ptr, "'") != 0))
    schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, "quote");
  token_next(ctxt, token_it, token_it_end);
  q.arg = schemerlicht_read_quote(ctxt, token_it, token_it_end, 0);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_quote;
  expr.expr.quote = q;
  return expr;
  }

static schemerlicht_expression make_quasiquote(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID && current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_BACKQUOTE)
    schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, "`");
  schemerlicht_parsed_quote q;
  q.line_nr = (*token_it)->line_nr;
  q.column_nr = (*token_it)->column_nr;
  q.filename = make_dummy_filename(ctxt);
  q.qt = schemerlicht_qt_backquote;
  if ((strcmp((*token_it)->value.string_ptr, "quasiquote") != 0) && (strcmp((*token_it)->value.string_ptr, "`") != 0))
    schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, "quote");
  token_next(ctxt, token_it, token_it_end);
  q.arg = schemerlicht_read_quote(ctxt, token_it, token_it_end, 1);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_quote;
  expr.expr.quote = q;
  return expr;
  }

static schemerlicht_expression make_unquote(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID && current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_UNQUOTE)
    schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, ",");
  schemerlicht_parsed_quote q;
  q.line_nr = (*token_it)->line_nr;
  q.column_nr = (*token_it)->column_nr;
  q.filename = make_dummy_filename(ctxt);
  q.qt = schemerlicht_qt_unquote;
  if ((strcmp((*token_it)->value.string_ptr, "unquote") != 0) && (strcmp((*token_it)->value.string_ptr, ",") != 0))
    schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, "quote");
  token_next(ctxt, token_it, token_it_end);
  q.arg = schemerlicht_read_quote(ctxt, token_it, token_it_end, 1);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_quote;
  expr.expr.quote = q;
  return expr;
  }

static schemerlicht_expression make_unquote_splicing(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID && current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_UNQUOTE_SPLICING)
    schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, ",@");
  schemerlicht_parsed_quote q;
  q.line_nr = (*token_it)->line_nr;
  q.column_nr = (*token_it)->column_nr;
  q.filename = make_dummy_filename(ctxt);
  q.qt = schemerlicht_qt_unquote_splicing;
  if ((strcmp((*token_it)->value.string_ptr, "unquote-splicing") != 0) && (strcmp((*token_it)->value.string_ptr, ",@") != 0))
    schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, "quote");
  token_next(ctxt, token_it, token_it_end);
  q.arg = schemerlicht_read_quote(ctxt, token_it, token_it_end, 1);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_quote;
  expr.expr.quote = q;
  return expr;
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
  if (*token_it == *token_it_end)
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr);
  if (strcmp((*token_it)->value.string_ptr, "foreign-call") != 0)
    schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, "foreign-call");
  schemerlicht_parsed_foreign_call f;
  f.line_nr = (*token_it)->line_nr;
  f.column_nr = (*token_it)->column_nr;
  f.filename = make_dummy_filename(ctxt);
  schemerlicht_vector_init(ctxt, &f.arguments, schemerlicht_expression);
  token_next(ctxt, token_it, token_it_end);    
  if (*token_it == *token_it_end)
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr);
  schemerlicht_string_copy(ctxt, &f.name, &((*token_it)->value));
  token_next(ctxt, token_it, token_it_end);
  while (!current_token_equals(token_it, ")"))
    {
    schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
    schemerlicht_vector_push_back(ctxt, &f.arguments, e, schemerlicht_expression);
    if (*token_it == *token_it_end)
      schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, f.line_nr, f.column_nr, ")");
    }
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_foreign_call;
  expr.expr.foreign = f;
  return expr;
  }

static schemerlicht_expression make_if(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr);
  schemerlicht_parsed_if i;
  schemerlicht_vector_init(ctxt, &i.arguments, schemerlicht_expression);
  i.filename = make_dummy_filename(ctxt);
  i.line_nr = (*token_it)->line_nr;
  i.column_nr = (*token_it)->column_nr;
  if (strcmp((*token_it)->value.string_ptr, "if") != 0)
    schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, i.line_nr, i.column_nr, "if");
  token_next(ctxt, token_it, token_it_end);

  while (!current_token_equals(token_it, ")"))
    {
    schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
    schemerlicht_vector_push_back(ctxt, &i.arguments, e, schemerlicht_expression);
    if (*token_it == *token_it_end)
      schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, i.line_nr, i.column_nr, ")");
    }
  if (i.arguments.vector_size < 2 || i.arguments.vector_size > 3)
    schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS, i.line_nr, i.column_nr);
  if (i.arguments.vector_size == 2)
    {
    schemerlicht_vector_push_back(ctxt, &i.arguments, make_nop(), schemerlicht_expression);
    }
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_if;
  expr.expr.i = i;
  return expr;
  }

static schemerlicht_expression make_lambda(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr);

  schemerlicht_parsed_lambda l;
  schemerlicht_vector_init(ctxt, &l.body, schemerlicht_expression);
  schemerlicht_vector_init(ctxt, &l.variables, schemerlicht_string);
  schemerlicht_vector_init(ctxt, &l.free_variables, schemerlicht_string);
  schemerlicht_vector_init(ctxt, &l.assignable_variables, schemerlicht_string);
  l.filename = make_dummy_filename(ctxt);
  l.line_nr = (*token_it)->line_nr;
  l.column_nr = (*token_it)->column_nr;
  if (strcmp((*token_it)->value.string_ptr, "lambda") != 0)
    schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, l.line_nr, l.column_nr, "lambda");
  token_next(ctxt, token_it, token_it_end);
  l.variable_arity = 0;
  if (!current_token_equals(token_it, "(")) // rest argument
    {
    l.variable_arity = 1;
    schemerlicht_string s;
    schemerlicht_string_copy(ctxt, &s, &(*token_it)->value);
    schemerlicht_vector_push_back(ctxt, &l.variables, s, schemerlicht_string);
    token_next(ctxt, token_it, token_it_end);
    }
  else
    {
    token_require(ctxt, token_it, token_it_end, "(");
    int count_after_var = 0;
    while (!current_token_equals(token_it, ")"))
      {
      if (l.variable_arity)
        ++count_after_var;
      if ((*token_it)->type != SCHEMERLICHT_T_ID && strcmp((*token_it)->value.string_ptr, ".") != 0)
        schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, (*token_it)->line_nr, (*token_it)->column_nr);
      if (strcmp((*token_it)->value.string_ptr, ".") == 0)
        {
        if (l.variable_arity)
          schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, (*token_it)->line_nr, (*token_it)->column_nr);
        l.variable_arity = 1;
        }
      else
        {
        schemerlicht_string s;
        schemerlicht_string_copy(ctxt, &s, &(*token_it)->value);
        schemerlicht_vector_push_back(ctxt, &l.variables, s, schemerlicht_string);
        }
      token_next(ctxt, token_it, token_it_end);
      }
    if (l.variable_arity && (count_after_var != 1))
      schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, l.line_nr, l.column_nr);
    token_next(ctxt, token_it, token_it_end);
    }
  schemerlicht_expression begin = make_begin(ctxt, token_it, token_it_end, 1);
  while (begin.expr.beg.arguments.vector_size == 1 && schemerlicht_vector_at(&begin.expr.beg.arguments, 0, schemerlicht_expression)->type == schemerlicht_type_begin)
    {
    schemerlicht_expression tmp = begin;
    begin = *schemerlicht_vector_at(&tmp.expr.beg.arguments, 0, schemerlicht_expression);
    schemerlicht_vector_destroy(ctxt, &tmp.expr.beg.arguments);
    schemerlicht_string_destroy(ctxt, &tmp.expr.beg.filename);
    }
  schemerlicht_vector_push_back(ctxt, &l.body, begin, schemerlicht_expression);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_lambda;
  expr.expr.lambda = l;
  return expr;
  }

static schemerlicht_expression make_let(schemerlicht_context* ctxt, token** token_it, token** token_it_end, enum schemerlicht_binding_type bt)
  {
  if (*token_it == *token_it_end)    
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr);
  schemerlicht_parsed_let l;
  l.filename = make_dummy_filename(ctxt);
  l.line_nr = (*token_it)->line_nr;
  l.column_nr = (*token_it)->column_nr;
  l.bt = bt;
  l.named_let = 0;
  l.let_name = make_dummy_filename(ctxt);
  schemerlicht_vector_init(ctxt, &l.bindings, schemerlicht_let_binding);
  schemerlicht_vector_init(ctxt, &l.body, schemerlicht_expression);
  schemerlicht_vector_init(ctxt, &l.assignable_variables, schemerlicht_string);
  switch (bt)
    {
    case schemerlicht_bt_let:
    {
      if (strcmp((*token_it)->value.string_ptr, "let") != 0)
        schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, l.line_nr, l.column_nr, "let");
      break;
    }
    case schemerlicht_bt_let_star:
    {
    if (strcmp((*token_it)->value.string_ptr, "let*") != 0)
      schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, l.line_nr, l.column_nr, "let*");
    break;
    }
    case schemerlicht_bt_letrec:
    {
    if (strcmp((*token_it)->value.string_ptr, "letrec") != 0)
      schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, l.line_nr, l.column_nr, "letrec");
    break;
    }
    }
  token_next(ctxt, token_it, token_it_end);
  if (*token_it == *token_it_end)
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
  if ((*token_it)->type == SCHEMERLICHT_T_ID) // named let?
    {
    if (bt != schemerlicht_bt_let)
      schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, l.line_nr, l.column_nr, "(");
    l.named_let = 1;
    token_next(ctxt, token_it, token_it_end);
    }
  token_require(ctxt, token_it, token_it_end, "(");
  while (!current_token_equals(token_it, ")"))
    {
    int rlsb = require_left_square_bracket(ctxt, token_it, token_it_end);
    if (*token_it == *token_it_end)
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
    if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
      schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, (*token_it)->line_nr, (*token_it)->column_nr);
    schemerlicht_let_binding b;
    schemerlicht_string_copy(ctxt, &b.binding_name, &((*token_it)->value));
    token_next(ctxt, token_it, token_it_end);
    b.binding_expr = schemerlicht_make_expression(ctxt, token_it, token_it_end);       
    schemerlicht_vector_push_back(ctxt, &l.bindings, b, schemerlicht_let_binding);
    require_right_square_bracket(ctxt, token_it, token_it_end, rlsb);
    }
  token_next(ctxt, token_it, token_it_end);
  schemerlicht_expression begin = make_begin(ctxt, token_it, token_it_end, 1);
  while (begin.expr.beg.arguments.vector_size == 1 && schemerlicht_vector_at(&begin.expr.beg.arguments, 0, schemerlicht_expression)->type == schemerlicht_type_begin)
    {
    schemerlicht_expression tmp = begin;
    begin = *schemerlicht_vector_at(&tmp.expr.beg.arguments, 0, schemerlicht_expression);
    schemerlicht_vector_destroy(ctxt, &tmp.expr.beg.arguments);
    schemerlicht_string_destroy(ctxt, &tmp.expr.beg.filename);
    }
  schemerlicht_vector_push_back(ctxt, &l.body, begin, schemerlicht_expression);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_let;
  expr.expr.let = l;
  return expr;
  }

static schemerlicht_expression make_primitive_call(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr);
  schemerlicht_parsed_primitive_call p;
  p.as_object = 0;
  schemerlicht_vector_init(ctxt, &p.arguments, schemerlicht_expression);
  p.filename = make_dummy_filename(ctxt);
  p.line_nr = (*token_it)->line_nr;
  p.column_nr = (*token_it)->column_nr;
  schemerlicht_string_copy(ctxt, &p.name, &((*token_it)->value));
  if (popped_token.type != SCHEMERLICHT_T_LEFT_ROUND_BRACKET)
    {
    token_next(ctxt, token_it, token_it_end);
    p.as_object = 1;
    schemerlicht_expression expr;
    expr.type = schemerlicht_type_primitive_call;
    expr.expr.prim = p;
    return expr;
    }
  token_next(ctxt, token_it, token_it_end);
  while (!current_token_equals(token_it, ")"))
    {
    schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
    schemerlicht_vector_push_back(ctxt, &p.arguments, e, schemerlicht_expression);
    if (*token_it == *token_it_end)
      schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, p.line_nr, p.column_nr, ")");
    }
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_primitive_call;
  expr.expr.prim = p;
  return expr;
  }

static schemerlicht_expression make_set(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr);
  schemerlicht_parsed_set s;
  s.filename = make_dummy_filename(ctxt);
  s.line_nr = (*token_it)->line_nr;
  s.column_nr = (*token_it)->column_nr;
  if (strcmp((*token_it)->value.string_ptr, "set!") != 0)
    schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, s.line_nr, s.column_nr, "set!");
  token_next(ctxt, token_it, token_it_end);
  if (*token_it == *token_it_end)
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr);
  schemerlicht_string_copy(ctxt, &s.name, &((*token_it)->value));
  token_next(ctxt, token_it, token_it_end);
  schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
  schemerlicht_vector_init(ctxt, &s.value, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &s.value, e, schemerlicht_expression);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_set;
  expr.expr.set = s;
  return expr;
  }

static schemerlicht_expression make_variable(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr);
  schemerlicht_parsed_variable v;
  v.filename = make_dummy_filename(ctxt);
  v.line_nr = (*token_it)->line_nr;
  v.column_nr = (*token_it)->column_nr;
  schemerlicht_string_copy(ctxt, &v.name, &((*token_it)->value));
  token_next(ctxt, token_it, token_it_end);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_variable;
  expr.expr.var = v;
  return expr;
  }

static schemerlicht_expression make_fun(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)    
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
  schemerlicht_parsed_funcall f;
  f.line_nr = (*token_it)->line_nr;
  f.column_nr = (*token_it)->column_nr;
  f.filename = make_dummy_filename(ctxt);
  schemerlicht_vector_init(ctxt, &f.arguments, schemerlicht_expression);
  schemerlicht_vector_init(ctxt, &f.fun, schemerlicht_expression);
  schemerlicht_expression e_fun = schemerlicht_make_expression(ctxt, token_it, token_it_end);
  schemerlicht_vector_push_back(ctxt, &f.fun, e_fun, schemerlicht_expression);
  while (!current_token_equals(token_it, ")"))
    {
    schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
    schemerlicht_vector_push_back(ctxt, &f.arguments, e, schemerlicht_expression);
    if (*token_it == *token_it_end)
      schemerlicht_throw_parser_required(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, f.line_nr, f.column_nr, ")");
    }
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_funcall;
  expr.expr.funcall = f;
  return expr;
  }

static schemerlicht_expression make_literal(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
    }
  switch (current_token_type(token_it, token_it_end))
    {
    case SCHEMERLICHT_T_LEFT_ROUND_BRACKET:
    {
    schemerlicht_parsed_nil n;
    n.line_nr = (*token_it)->line_nr;
    n.column_nr = (*token_it)->column_nr;
    n.filename = make_dummy_filename(ctxt);
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
    f.value = to_fixnum((*token_it)->value.string_ptr);
    f.filename = make_dummy_filename(ctxt);
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
    f.value = to_flonum((*token_it)->value.string_ptr);
    f.filename = make_dummy_filename(ctxt);
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
    s.filename = make_dummy_filename(ctxt);
    schemerlicht_string_init_ranged(ctxt, &s.value, (*token_it)->value.string_ptr+1, (*token_it)->value.string_ptr + (*token_it)->value.string_length - 1);
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
      t.filename = make_dummy_filename(ctxt);
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
      f.filename = make_dummy_filename(ctxt);
      schemerlicht_expression expr;
      expr.type = schemerlicht_type_literal;
      expr.expr.lit.type = schemerlicht_type_false;
      expr.expr.lit.lit.f = f;
      token_next(ctxt, token_it, token_it_end);
      return expr;
      }
    else
      {
      schemerlicht_string s = (*token_it)->value;
      if (s.string_length >= 2 && (*schemerlicht_string_at(&s, 0) == '#') && (*schemerlicht_string_at(&s, 1) == '\\'))
        {
        schemerlicht_string substr;
        schemerlicht_string_init_ranged(ctxt, &substr, s.string_ptr + 2, s.string_ptr + s.string_length);
        schemerlicht_parsed_character ch;
        ch.line_nr = ln;
        ch.column_nr = cn;
        ch.filename = make_dummy_filename(ctxt);
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
  schemerlicht_object* obj = schemerlicht_map_get_string(ctxt->global->expression_map, (*token_it)->value.string_ptr);
  if (obj == NULL || obj->type == schemerlicht_object_type_nil)
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
      schemerlicht_parsed_funcall f;
      f.filename = make_dummy_filename(ctxt);
      schemerlicht_vector_init(ctxt, &f.arguments, schemerlicht_expression);
      schemerlicht_vector_init(ctxt, &f.fun, schemerlicht_expression);
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
      schemerlicht_expression* fun_expr = schemerlicht_vector_at(&f.fun, 0, schemerlicht_expression);
      if (f.arguments.vector_size == 0 && fun_expr->type == schemerlicht_type_literal)
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
        schemerlicht_parsed_funcall f;
        f.filename = make_dummy_filename(ctxt);
        schemerlicht_vector_init(ctxt, &f.arguments, schemerlicht_expression);
        schemerlicht_vector_init(ctxt, &f.fun, schemerlicht_expression);
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
static void postvisit_primcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_vector_destroy(ctxt, &e->expr.prim.arguments);
  schemerlicht_string_destroy(ctxt, &e->expr.prim.filename);
  schemerlicht_string_destroy(ctxt, &e->expr.prim.name);
  }
static void visit_fixnum(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &e->expr.lit.lit.fx.filename);
  }
static void visit_flonum(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &e->expr.lit.lit.fl.filename);
  }
static void visit_nil(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &e->expr.lit.lit.nil.filename);
  }
static void visit_true(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &e->expr.lit.lit.t.filename);
  }
static void visit_false(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &e->expr.lit.lit.f.filename);
  }
static void visit_nop(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &e->expr.nop.filename);
  }
static void visit_character(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &e->expr.lit.lit.ch.filename);
  }
static void visit_variable(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &e->expr.var.filename);
  schemerlicht_string_destroy(ctxt, &e->expr.var.name);
  }
static void visit_quote(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_destroy_cell(ctxt, &e->expr.quote.arg);
  schemerlicht_string_destroy(ctxt, &e->expr.quote.filename);
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
static void postvisit_funcall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &e->expr.funcall.filename);
  schemerlicht_vector_destroy(ctxt, &e->expr.funcall.arguments);
  schemerlicht_vector_destroy(ctxt, &e->expr.funcall.fun);
  }
static void postvisit_foreigncall(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &e->expr.foreign.filename);
  schemerlicht_vector_destroy(ctxt, &e->expr.foreign.arguments);
  schemerlicht_string_destroy(ctxt, &e->expr.foreign.name);
  }
static void postvisit_if(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &e->expr.i.filename);
  schemerlicht_vector_destroy(ctxt, &e->expr.i.arguments);
  }
static void postvisit_set(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &e->expr.set.filename);
  schemerlicht_string_destroy(ctxt, &e->expr.set.name);
  schemerlicht_vector_destroy(ctxt, &e->expr.set.value);
  }
static void postvisit_lambda(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &e->expr.lambda.filename);
  schemerlicht_vector_destroy(ctxt, &e->expr.lambda.body);
  schemerlicht_string* it = schemerlicht_vector_begin(&e->expr.lambda.variables, schemerlicht_string);
  schemerlicht_string* it_end = schemerlicht_vector_end(&e->expr.lambda.variables, schemerlicht_string);
  for (; it != it_end; ++it)
    schemerlicht_string_destroy(ctxt, it);
  schemerlicht_vector_destroy(ctxt, &e->expr.lambda.variables);
  it = schemerlicht_vector_begin(&e->expr.lambda.free_variables, schemerlicht_string);
  it_end = schemerlicht_vector_end(&e->expr.lambda.free_variables, schemerlicht_string);
  for (; it != it_end; ++it)
    schemerlicht_string_destroy(ctxt, it);
  schemerlicht_vector_destroy(ctxt, &e->expr.lambda.free_variables);
  it = schemerlicht_vector_begin(&e->expr.lambda.assignable_variables, schemerlicht_string);
  it_end = schemerlicht_vector_end(&e->expr.lambda.assignable_variables, schemerlicht_string);
  for (; it != it_end; ++it)
    schemerlicht_string_destroy(ctxt, it);
  schemerlicht_vector_destroy(ctxt, &e->expr.lambda.assignable_variables);
  }
static void postvisit_let(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &e->expr.let.filename);
  schemerlicht_let_binding* bit = schemerlicht_vector_begin(&e->expr.let.bindings, schemerlicht_let_binding);
  schemerlicht_let_binding* bit_end = schemerlicht_vector_end(&e->expr.let.bindings, schemerlicht_let_binding);
  for (; bit != bit_end; ++bit)
    schemerlicht_string_destroy(ctxt, &bit->binding_name);
  schemerlicht_vector_destroy(ctxt, &e->expr.let.bindings);
  schemerlicht_vector_destroy(ctxt, &e->expr.let.body);
  schemerlicht_string* it = schemerlicht_vector_begin(&e->expr.let.assignable_variables, schemerlicht_string);
  schemerlicht_string* it_end = schemerlicht_vector_end(&e->expr.let.assignable_variables, schemerlicht_string);
  for (; it != it_end; ++it)
    schemerlicht_string_destroy(ctxt, it);
  schemerlicht_vector_destroy(ctxt, &e->expr.let.assignable_variables);
  schemerlicht_string_destroy(ctxt, &e->expr.let.let_name);
  }

typedef struct schemerlicht_program_destroy_visitor
  {
  schemerlicht_visitor* visitor;
  } schemerlicht_program_destroy_visitor;

void schemerlicht_program_destroy(schemerlicht_context* ctxt, schemerlicht_program* p)
  {
  schemerlicht_program_destroy_visitor destroyer;
  destroyer.visitor = schemerlicht_visitor_new(ctxt, &destroyer);

  destroyer.visitor->postvisit_program = postvisit_program;
  destroyer.visitor->visit_fixnum = visit_fixnum;
  destroyer.visitor->visit_flonum = visit_flonum;
  destroyer.visitor->visit_nil = visit_nil;
  destroyer.visitor->visit_string = visit_string;
  destroyer.visitor->visit_symbol = visit_symbol;
  destroyer.visitor->visit_true = visit_true;
  destroyer.visitor->visit_false = visit_false;
  destroyer.visitor->visit_nop = visit_nop;
  destroyer.visitor->visit_character = visit_character;
  destroyer.visitor->visit_variable = visit_variable;
  destroyer.visitor->visit_quote = visit_quote;
  destroyer.visitor->postvisit_primcall = postvisit_primcall;
  destroyer.visitor->postvisit_funcall = postvisit_funcall;
  destroyer.visitor->postvisit_foreigncall = postvisit_foreigncall;
  destroyer.visitor->postvisit_begin = postvisit_begin;
  destroyer.visitor->postvisit_if = postvisit_if;
  destroyer.visitor->postvisit_set = postvisit_set;
  destroyer.visitor->postvisit_lambda = postvisit_lambda;
  destroyer.visitor->postvisit_let = postvisit_let;
  schemerlicht_visit_program(ctxt, destroyer.visitor, p);

  destroyer.visitor->destroy(ctxt, destroyer.visitor);
  }

void schemerlicht_expression_destroy(schemerlicht_context* ctxt, schemerlicht_expression* e)
  {
  schemerlicht_program_destroy_visitor destroyer;
  destroyer.visitor = schemerlicht_visitor_new(ctxt, &destroyer);

  destroyer.visitor->postvisit_program = postvisit_program;
  destroyer.visitor->visit_fixnum = visit_fixnum;
  destroyer.visitor->visit_flonum = visit_flonum;
  destroyer.visitor->visit_nil = visit_nil;
  destroyer.visitor->visit_string = visit_string;
  destroyer.visitor->visit_symbol = visit_symbol;
  destroyer.visitor->visit_true = visit_true;
  destroyer.visitor->visit_false = visit_false;
  destroyer.visitor->visit_nop = visit_nop;
  destroyer.visitor->visit_character = visit_character;
  destroyer.visitor->visit_variable = visit_variable;
  destroyer.visitor->visit_quote = visit_quote;
  destroyer.visitor->postvisit_primcall = postvisit_primcall;
  destroyer.visitor->postvisit_funcall = postvisit_funcall;
  destroyer.visitor->postvisit_foreigncall = postvisit_foreigncall;
  destroyer.visitor->postvisit_begin = postvisit_begin;
  destroyer.visitor->postvisit_if = postvisit_if;
  destroyer.visitor->postvisit_set = postvisit_set;
  destroyer.visitor->postvisit_lambda = postvisit_lambda;
  destroyer.visitor->postvisit_let = postvisit_let;
  schemerlicht_visit_expression(ctxt, destroyer.visitor, e);

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

  schemerlicht_expression schemerlicht_make_variable_expression(schemerlicht_parsed_variable* v)
    {
    schemerlicht_expression expr;
    expr.type = schemerlicht_type_variable;
    expr.expr.var = *v;
    return expr;
    }

  schemerlicht_expression schemerlicht_make_let_expression(schemerlicht_parsed_let* l)
    {
    schemerlicht_expression expr;
    expr.type = schemerlicht_type_let;
    expr.expr.let = *l;
    return expr;
    }

  schemerlicht_expression schemerlicht_make_lambda_expression(schemerlicht_parsed_lambda* l)
    {
    schemerlicht_expression expr;
    expr.type = schemerlicht_type_lambda;
    expr.expr.lambda = *l;
    return expr;
    }

  schemerlicht_expression schemerlicht_make_begin_expression(schemerlicht_parsed_begin* b)
    {
    schemerlicht_expression expr;
    expr.type = schemerlicht_type_begin;
    expr.expr.beg = *b;
    return expr;
    }

  schemerlicht_expression schemerlicht_make_literal_expression(schemerlicht_literal* l)
    {
    schemerlicht_expression expr;
    expr.type = schemerlicht_type_literal;
    expr.expr.lit = *l;
    return expr;
    }