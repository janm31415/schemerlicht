#include "parser.h"
#include "vector.h"
#include "token.h"
#include "error.h"
#include "context.h"
#include "visitor.h"
#include "copy.h"

#include <string.h>
#include <stdlib.h>

#define SCHEMERLICHT_MAX_SYNTAX_ERRORS 5

void schemerlicht_destroy_parsed_begin(schemerlicht_context* ctxt, schemerlicht_parsed_begin* b)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_begin;
  expr.expr.beg = *b;
  schemerlicht_expression_destroy(ctxt, &expr);
  }

void schemerlicht_destroy_parsed_quote(schemerlicht_context* ctxt, schemerlicht_parsed_quote* q)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_quote;
  expr.expr.quote = *q;
  schemerlicht_expression_destroy(ctxt, &expr);
  }

void schemerlicht_destroy_literal(schemerlicht_context* ctxt, schemerlicht_literal* p)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_literal;
  expr.expr.lit = *p;
  schemerlicht_expression_destroy(ctxt, &expr);
  }

void schemerlicht_destroy_parsed_variable(schemerlicht_context* ctxt, schemerlicht_parsed_variable* p)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_variable;
  expr.expr.var = *p;
  schemerlicht_expression_destroy(ctxt, &expr);
  }

void schemerlicht_destroy_parsed_nop(schemerlicht_context* ctxt, schemerlicht_parsed_nop* p)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_nop;
  expr.expr.nop = *p;
  schemerlicht_expression_destroy(ctxt, &expr);
  }

void schemerlicht_destroy_parsed_primitive_call(schemerlicht_context* ctxt, schemerlicht_parsed_primitive_call* p)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_primitive_call;
  expr.expr.prim = *p;
  schemerlicht_expression_destroy(ctxt, &expr);
  }

void schemerlicht_destroy_parsed_foreign_call(schemerlicht_context* ctxt, schemerlicht_parsed_foreign_call* p)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_foreign_call;
  expr.expr.foreign = *p;
  schemerlicht_expression_destroy(ctxt, &expr);
  }

void schemerlicht_destroy_parsed_if(schemerlicht_context* ctxt, schemerlicht_parsed_if* p)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_if;
  expr.expr.i = *p;
  schemerlicht_expression_destroy(ctxt, &expr);
  }

void schemerlicht_destroy_parsed_cond(schemerlicht_context* ctxt, schemerlicht_parsed_cond* p)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_cond;
  expr.expr.cond = *p;
  schemerlicht_expression_destroy(ctxt, &expr);
  }

void schemerlicht_destroy_parsed_do(schemerlicht_context* ctxt, schemerlicht_parsed_do* p)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_do;
  expr.expr.d = *p;
  schemerlicht_expression_destroy(ctxt, &expr);
  }

void schemerlicht_destroy_parsed_case(schemerlicht_context* ctxt, schemerlicht_parsed_case* p)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_case;
  expr.expr.cas = *p;
  schemerlicht_expression_destroy(ctxt, &expr);
  }

void schemerlicht_destroy_parsed_let(schemerlicht_context* ctxt, schemerlicht_parsed_let* p)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_let;
  expr.expr.let = *p;
  schemerlicht_expression_destroy(ctxt, &expr);
  }

void schemerlicht_destroy_parsed_lambda(schemerlicht_context* ctxt, schemerlicht_parsed_lambda* p)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_lambda;
  expr.expr.lambda = *p;
  schemerlicht_expression_destroy(ctxt, &expr);
  }

void schemerlicht_destroy_parsed_funcall(schemerlicht_context* ctxt, schemerlicht_parsed_funcall* p)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_funcall;
  expr.expr.funcall = *p;
  schemerlicht_expression_destroy(ctxt, &expr);
  }

void schemerlicht_destroy_parsed_set(schemerlicht_context* ctxt, schemerlicht_parsed_set* p)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_set;
  expr.expr.set = *p;
  schemerlicht_expression_destroy(ctxt, &expr);
  }

schemerlicht_expression schemerlicht_make_expression(schemerlicht_context* ctxt, token** token_it, token** token_it_end);

static schemerlicht_string make_dummy_filename(schemerlicht_context* ctxt)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "dummy");
  return s;
  }

static schemerlicht_string make_let_filename(schemerlicht_context* ctxt)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "letter");
  return s;
  }

static schemerlicht_string make_funcall_filename(schemerlicht_context* ctxt)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "funcall");
  return s;
  }

static schemerlicht_string make_begin_filename(schemerlicht_context* ctxt)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "begin");
  return s;
  }

static schemerlicht_string make_prim_filename(schemerlicht_context* ctxt)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "prim");
  return s;
  }

static schemerlicht_string make_lambda_filename(schemerlicht_context* ctxt)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "lambda");
  return s;
  }

static schemerlicht_string make_set_filename(schemerlicht_context* ctxt)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "set");
  return s;
  }

static schemerlicht_string make_if_filename(schemerlicht_context* ctxt)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "if");
  return s;
  }

static schemerlicht_string make_var_filename(schemerlicht_context* ctxt)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "var");
  return s;
  }

static schemerlicht_string make_lit_filename(schemerlicht_context* ctxt)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "literal");
  return s;
  }

static schemerlicht_string make_foreign_filename(schemerlicht_context* ctxt)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "foreign");
  return s;
  }

static schemerlicht_string make_quote_filename(schemerlicht_context* ctxt)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "quote");
  return s;
  }

static token make_bad_token()
  {
  token t;
  t.type = SCHEMERLICHT_T_BAD;
  return t;
  }

token popped_token;
token last_token;

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

static int token_next(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return 0;
    }
  popped_token = (**token_it);
  last_token = (**token_it);
  ++(*token_it);
  return 1;
  }

static int token_require(schemerlicht_context* ctxt, token** token_it, token** token_it_end, const char* required)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return 0;
    }
  else
    {
    if (strcmp((*token_it)->value.string_ptr, required) != 0)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, required);
      return 0;
      }
    popped_token = (**token_it);
    last_token = (**token_it);
    ++(*token_it);
    return 1;
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

static int current_token_equals(token** token_it, token** token_it_end, const char* expected)
  {
  if (*token_it == *token_it_end)      
    return 0;    
  if ((*token_it)->type == SCHEMERLICHT_T_FIXNUM || (*token_it)->type == SCHEMERLICHT_T_FLONUM)
    return 0;
  return strcmp((*token_it)->value.string_ptr, expected) == 0;
  }

static int require_left_square_bracket(schemerlicht_context* ctxt, token** token_it)
  {
  int res = ((*token_it)->type == SCHEMERLICHT_T_LEFT_SQUARE_BRACKET);
  if (!(res || ((*token_it)->type == SCHEMERLICHT_T_LEFT_ROUND_BRACKET)))
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, "[ expected");
    }
  ++(*token_it);
  invalidate_popped();
  return res;
  }

static int require_right_square_bracket(schemerlicht_context* ctxt, token** token_it, token** token_it_end, int result_of_require_left_square_bracket)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return 0;
    }
  if (result_of_require_left_square_bracket && ((*token_it)->type != SCHEMERLICHT_T_RIGHT_SQUARE_BRACKET))
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, "] expected");    
    return 0;
    }
  if (!result_of_require_left_square_bracket && ((*token_it)->type != SCHEMERLICHT_T_RIGHT_ROUND_BRACKET))
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, ") expected");    
    return 0;
    }
  return token_next(ctxt, token_it, token_it_end);
  }

int closing_brackets(token** token_it, token** token_it_end)
  {
  int t = current_token_type(token_it, token_it_end);
  return t == SCHEMERLICHT_T_RIGHT_ROUND_BRACKET || t == SCHEMERLICHT_T_RIGHT_SQUARE_BRACKET;
  }

static schemerlicht_expression make_begin(schemerlicht_context* ctxt, token** token_it, token** token_it_end, int implicit)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return make_nop();    
    }
  schemerlicht_parsed_begin b;
  b.filename = make_begin_filename(ctxt);
  schemerlicht_vector_init(ctxt, &b.arguments, schemerlicht_expression);
  b.line_nr = (*token_it)->line_nr;
  b.column_nr = (*token_it)->column_nr;
  if (!implicit)
    {
    if (strcmp((*token_it)->value.string_ptr, "begin") != 0)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, b.line_nr, b.column_nr, "begin expected");
      return schemerlicht_make_begin_expression(&b);
      }
    if (!token_next(ctxt, token_it, token_it_end))
      return schemerlicht_make_begin_expression(&b);
    }
  invalidate_popped();
  while (!current_token_equals(token_it, token_it_end, ")"))
    {
    schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
    schemerlicht_vector_push_back(ctxt, &b.arguments, e, schemerlicht_expression);
    if (*token_it == *token_it_end)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, last_token.line_nr, last_token.column_nr, ") expected");
      return schemerlicht_make_begin_expression(&b);
      }
    }
  return schemerlicht_make_begin_expression(&b);
  }

static schemerlicht_expression make_quote(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return make_nop();
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID && current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_QUOTE)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, "' expected");
    return make_nop();
    }
  schemerlicht_parsed_quote q;
  q.line_nr = (*token_it)->line_nr;
  q.column_nr = (*token_it)->column_nr;
  q.filename = make_quote_filename(ctxt);
  q.qt = schemerlicht_qt_quote;
  if ((strcmp((*token_it)->value.string_ptr, "quote") != 0) && (strcmp((*token_it)->value.string_ptr, "'") != 0))
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, "quote expected");
    return schemerlicht_make_quote_expression(&q);
    }
  if (!token_next(ctxt, token_it, token_it_end))
    return schemerlicht_make_quote_expression(&q);
  q.arg = schemerlicht_read_quote(ctxt, token_it, token_it_end, 0);
  return schemerlicht_make_quote_expression(&q);
  }

static schemerlicht_expression make_quasiquote(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return make_nop();
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID && current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_BACKQUOTE)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, "` expected");
    return make_nop();
    }
  schemerlicht_parsed_quote q;
  q.line_nr = (*token_it)->line_nr;
  q.column_nr = (*token_it)->column_nr;
  q.filename = make_quote_filename(ctxt);
  q.qt = schemerlicht_qt_backquote;
  if ((strcmp((*token_it)->value.string_ptr, "quasiquote") != 0) && (strcmp((*token_it)->value.string_ptr, "`") != 0))
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, "quote expected");
    return schemerlicht_make_quote_expression(&q);
    }
  if (!token_next(ctxt, token_it, token_it_end))
    return schemerlicht_make_quote_expression(&q);
  q.arg = schemerlicht_read_quote(ctxt, token_it, token_it_end, 1);
  return schemerlicht_make_quote_expression(&q);
  }

static schemerlicht_expression make_unquote(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return make_nop();
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID && current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_UNQUOTE)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, ", expected");
    return make_nop();
    }
  schemerlicht_parsed_quote q;
  q.line_nr = (*token_it)->line_nr;
  q.column_nr = (*token_it)->column_nr;
  q.filename = make_quote_filename(ctxt);
  q.qt = schemerlicht_qt_unquote;
  if ((strcmp((*token_it)->value.string_ptr, "unquote") != 0) && (strcmp((*token_it)->value.string_ptr, ",") != 0))
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, "quote expected");
    return schemerlicht_make_quote_expression(&q);
    }
  if (!token_next(ctxt, token_it, token_it_end))
    return schemerlicht_make_quote_expression(&q);
  q.arg = schemerlicht_read_quote(ctxt, token_it, token_it_end, 1);
  return schemerlicht_make_quote_expression(&q);
  }

static schemerlicht_expression make_unquote_splicing(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return make_nop();
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID && current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_UNQUOTE_SPLICING)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, ",@ expected");
    return make_nop();
    }
  schemerlicht_parsed_quote q;
  q.line_nr = (*token_it)->line_nr;
  q.column_nr = (*token_it)->column_nr;
  q.filename = make_quote_filename(ctxt);
  q.qt = schemerlicht_qt_unquote_splicing;
  if ((strcmp((*token_it)->value.string_ptr, "unquote-splicing") != 0) && (strcmp((*token_it)->value.string_ptr, ",@") != 0))
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, "quote expected");
    return schemerlicht_make_quote_expression(&q);
    }
  if (!token_next(ctxt, token_it, token_it_end))
    return schemerlicht_make_quote_expression(&q);
  q.arg = schemerlicht_read_quote(ctxt, token_it, token_it_end, 1);
  return schemerlicht_make_quote_expression(&q);
  }

static schemerlicht_expression make_variable(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return make_nop();
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, "variable name expected");
    return make_nop();
    }
  schemerlicht_parsed_variable v;
  v.filename = make_var_filename(ctxt);
  v.line_nr = (*token_it)->line_nr;
  v.column_nr = (*token_it)->column_nr;
  schemerlicht_string_copy(ctxt, &v.name, &((*token_it)->value));
  token_next(ctxt, token_it, token_it_end);
  return schemerlicht_make_variable_expression(&v);
  }

static schemerlicht_expression make_case(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return make_nop();
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, "case expected");
    return make_nop();
    }
  if (strcmp((*token_it)->value.string_ptr, "case") != 0)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, "case expected");
    return make_nop();
    }
  schemerlicht_parsed_case c;
  c.line_nr = (*token_it)->line_nr;
  c.column_nr = (*token_it)->column_nr;
  c.filename = make_dummy_filename(ctxt);
  schemerlicht_vector_init(ctxt, &c.val_expr, schemerlicht_expression);
  schemerlicht_vector_init(ctxt, &c.datum_args, schemerlicht_cell);
  schemerlicht_vector_init(ctxt, &c.then_bodies, schemerlicht_vector);
  schemerlicht_vector_init(ctxt, &c.else_body, schemerlicht_expression);
  if (!token_next(ctxt, token_it, token_it_end))
    return schemerlicht_make_case_expression(&c);
  schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
  schemerlicht_vector_push_back(ctxt, &c.val_expr, e, schemerlicht_expression);
  if (current_token_equals(token_it, token_it_end, ")"))
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, last_token.line_nr, last_token.column_nr, "[ expected");
    return schemerlicht_make_case_expression(&c);
    }
  int else_statement = 0;
  while (!current_token_equals(token_it, token_it_end, ")"))
    {
    if (else_statement)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, last_token.line_nr, last_token.column_nr, ") expected");
      return schemerlicht_make_case_expression(&c);
      }
    if (*token_it == *token_it_end)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
      return schemerlicht_make_case_expression(&c);
      }
    int rlsb = require_left_square_bracket(ctxt, token_it);
    while (!closing_brackets(token_it, token_it_end))
      {
      if (current_token_equals(token_it, token_it_end, "else"))
        {
        if (!token_next(ctxt, token_it, token_it_end))
          return schemerlicht_make_case_expression(&c);
        else_statement = 1;
        while (!closing_brackets(token_it, token_it_end))
          {
          schemerlicht_expression e2 = schemerlicht_make_expression(ctxt, token_it, token_it_end);
          schemerlicht_vector_push_back(ctxt, &c.else_body, e2, schemerlicht_expression);
          }
        }
      else
        {
        schemerlicht_cell cell = schemerlicht_read_quote(ctxt, token_it, token_it_end, 0);
        if (cell.type != schemerlicht_ct_pair)
          {
          schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, last_token.line_nr, last_token.column_nr, "");
          return schemerlicht_make_case_expression(&c);
          }
        schemerlicht_vector_push_back(ctxt, &c.datum_args, cell, schemerlicht_cell);
        schemerlicht_vector then_bodies;
        schemerlicht_vector_init(ctxt, &then_bodies, schemerlicht_expression);
        while (!closing_brackets(token_it, token_it_end))
          {
          schemerlicht_expression e2 = schemerlicht_make_expression(ctxt, token_it, token_it_end);
          schemerlicht_vector_push_back(ctxt, &then_bodies, e2, schemerlicht_expression);
          }
        schemerlicht_vector_push_back(ctxt, &c.then_bodies, then_bodies, schemerlicht_vector);
        }
      }
    if (!require_right_square_bracket(ctxt, token_it, token_it_end, rlsb))
      return schemerlicht_make_case_expression(&c);
    }
  if (else_statement == 0)
    {
    schemerlicht_vector_push_back(ctxt, &c.else_body, make_nop(), schemerlicht_expression);
    }
  return schemerlicht_make_case_expression(&c);
  }

static schemerlicht_expression make_cond(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return make_nop();
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, "cond expected");
    return make_nop();
    }
  if (strcmp((*token_it)->value.string_ptr, "cond") != 0)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, "cond expected");
    return make_nop();
    }
  schemerlicht_parsed_cond c;
  c.line_nr = (*token_it)->line_nr;
  c.column_nr = (*token_it)->column_nr;
  c.filename = make_dummy_filename(ctxt);
  schemerlicht_vector_init(ctxt, &c.arguments, schemerlicht_vector);
  schemerlicht_vector_init(ctxt, &c.is_proc, int);
  if (!token_next(ctxt, token_it, token_it_end))
    return schemerlicht_make_cond_expression(&c);
  int else_statement = 0;
  while (!current_token_equals(token_it, token_it_end, ")"))
    {
    schemerlicht_vector_push_back(ctxt, &c.is_proc, 0, int);
    if (else_statement)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, last_token.line_nr, last_token.column_nr, ") expected");
      return schemerlicht_make_cond_expression(&c);
      }
    schemerlicht_vector expr;
    schemerlicht_vector_init(ctxt, &expr, schemerlicht_expression);
    if (*token_it == *token_it_end)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
      return schemerlicht_make_cond_expression(&c);
      }
    int rlsb = require_left_square_bracket(ctxt, token_it);
    while (!closing_brackets(token_it, token_it_end))
      {
      if (current_token_equals(token_it, token_it_end, "else"))
        {
        if (!token_next(ctxt, token_it, token_it_end))
          return schemerlicht_make_cond_expression(&c);
        else_statement = 1;
        schemerlicht_parsed_true t;
        t.column_nr = -1;
        t.line_nr = -1;
        t.filename = make_null_string();
        schemerlicht_literal lit;
        lit.type = schemerlicht_type_true;
        lit.lit.t = t;
        schemerlicht_expression true_exp;
        true_exp.type = schemerlicht_type_literal;
        true_exp.expr.lit = lit;
        schemerlicht_vector_push_back(ctxt, &expr, true_exp, schemerlicht_expression);
        }
      else
        {
        if (current_token_equals(token_it, token_it_end, "=>"))
          {
          *schemerlicht_vector_back(&c.is_proc, int) = 1;
          if (!token_next(ctxt, token_it, token_it_end))
            return schemerlicht_make_cond_expression(&c);
          if (*token_it == *token_it_end)
            {
            schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
            return schemerlicht_make_cond_expression(&c);
            }
          schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
          schemerlicht_vector_push_back(ctxt, &expr, e, schemerlicht_expression);
          if (!closing_brackets(token_it, token_it_end))
            {
            schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, ") expected");
            return schemerlicht_make_cond_expression(&c);
            }
          }
        else
          {
          schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
          schemerlicht_vector_push_back(ctxt, &expr, e, schemerlicht_expression);
          if (*token_it == *token_it_end)
            {
            schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
            return schemerlicht_make_cond_expression(&c);
            }
          }
        }
      }
    if (!require_right_square_bracket(ctxt, token_it, token_it_end, rlsb))
      return schemerlicht_make_cond_expression(&c);
    schemerlicht_vector_push_back(ctxt, &c.arguments, expr, schemerlicht_vector);
    }
  return schemerlicht_make_cond_expression(&c);
  }

static schemerlicht_expression make_do(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return make_nop();
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, "do expected");
    return make_nop();
    }
  if (strcmp((*token_it)->value.string_ptr, "do") != 0)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, "do expected");
    return make_nop();
    }
  schemerlicht_parsed_do d;
  d.line_nr = (*token_it)->line_nr;
  d.column_nr = (*token_it)->column_nr;
  d.filename = make_dummy_filename(ctxt);
  schemerlicht_vector_init(ctxt, &d.bindings, schemerlicht_vector);
  schemerlicht_vector_init(ctxt, &d.test, schemerlicht_expression);
  schemerlicht_vector_init(ctxt, &d.commands, schemerlicht_expression);
  if (!token_next(ctxt, token_it, token_it_end))
    return schemerlicht_make_do_expression(&d);

  if (!token_require(ctxt, token_it, token_it_end, "("))
    return schemerlicht_make_do_expression(&d);
  while (!current_token_equals(token_it, token_it_end, ")"))
    {
    if (*token_it == *token_it_end)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
      return schemerlicht_make_do_expression(&d);
      }
    int rlsb = require_left_square_bracket(ctxt, token_it);
    schemerlicht_expression var = make_variable(ctxt, token_it, token_it_end);
    schemerlicht_expression init = schemerlicht_make_expression(ctxt, token_it, token_it_end);
    schemerlicht_vector exprs;
    schemerlicht_vector_init(ctxt, &exprs, schemerlicht_expression);
    if (closing_brackets(token_it, token_it_end))
      {
      schemerlicht_vector_push_back(ctxt, &exprs, var, schemerlicht_expression);
      schemerlicht_vector_push_back(ctxt, &exprs, init, schemerlicht_expression);
      schemerlicht_vector_push_back(ctxt, &exprs, schemerlicht_expression_copy(ctxt, &var), schemerlicht_expression);
      }
    else
      {
      schemerlicht_expression step = schemerlicht_make_expression(ctxt, token_it, token_it_end);
      schemerlicht_vector_push_back(ctxt, &exprs, var, schemerlicht_expression);
      schemerlicht_vector_push_back(ctxt, &exprs, init, schemerlicht_expression);
      schemerlicht_vector_push_back(ctxt, &exprs, step, schemerlicht_expression);
      }
    schemerlicht_vector_push_back(ctxt, &d.bindings, exprs, schemerlicht_vector);
    if (!require_right_square_bracket(ctxt, token_it, token_it_end, rlsb))
      return schemerlicht_make_do_expression(&d);
    }
  if (!token_next(ctxt, token_it, token_it_end))
    return schemerlicht_make_do_expression(&d);
  if (!token_require(ctxt, token_it, token_it_end, "("))
    return schemerlicht_make_do_expression(&d);
  invalidate_popped();
  while (!current_token_equals(token_it, token_it_end, ")"))
    {
    schemerlicht_expression test = schemerlicht_make_expression(ctxt, token_it, token_it_end);
    schemerlicht_vector_push_back(ctxt, &d.test, test, schemerlicht_expression);
    }
  if (d.test.vector_size == 1) // only test, no expressions
    {
    schemerlicht_vector_push_back(ctxt, &d.test, make_nop(), schemerlicht_expression);
    }
  if (!token_next(ctxt, token_it, token_it_end))
    return schemerlicht_make_do_expression(&d);
  while (!current_token_equals(token_it, token_it_end, ")"))
    {
    schemerlicht_expression command = schemerlicht_make_expression(ctxt, token_it, token_it_end);
    schemerlicht_vector_push_back(ctxt, &d.commands, command, schemerlicht_expression);
    }
  return schemerlicht_make_do_expression(&d);
  }

static schemerlicht_expression make_foreign_call(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return make_nop();
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, "foreign-call expected");
    return make_nop();
    }
  if (strcmp((*token_it)->value.string_ptr, "foreign-call") != 0)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, (*token_it)->line_nr, (*token_it)->column_nr, "foreign-call expected");
    return make_nop();
    }
  schemerlicht_parsed_foreign_call f;
  f.line_nr = (*token_it)->line_nr;
  f.column_nr = (*token_it)->column_nr;
  f.filename = make_foreign_filename(ctxt);
  schemerlicht_vector_init(ctxt, &f.arguments, schemerlicht_expression);
  if (!token_next(ctxt, token_it, token_it_end))
    return schemerlicht_make_foreign_call_expression(&f);
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, f.line_nr, f.column_nr, "");
    return schemerlicht_make_foreign_call_expression(&f);
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, "");
    return schemerlicht_make_foreign_call_expression(&f);
    }
  schemerlicht_string_copy(ctxt, &f.name, &((*token_it)->value));
  if (!token_next(ctxt, token_it, token_it_end))
    return schemerlicht_make_foreign_call_expression(&f);
  while (!current_token_equals(token_it, token_it_end, ")"))
    {
    schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
    schemerlicht_vector_push_back(ctxt, &f.arguments, e, schemerlicht_expression);
    if (*token_it == *token_it_end)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, last_token.line_nr, last_token.column_nr, ") expected");
      return schemerlicht_make_foreign_call_expression(&f);
      }
    }
  return schemerlicht_make_foreign_call_expression(&f);
  }

static schemerlicht_expression make_if(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return make_nop();
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, "if expected");
    return make_nop();
    }
  schemerlicht_parsed_if i;
  schemerlicht_vector_init(ctxt, &i.arguments, schemerlicht_expression);
  i.filename = make_if_filename(ctxt);
  i.line_nr = (*token_it)->line_nr;
  i.column_nr = (*token_it)->column_nr;
  if (strcmp((*token_it)->value.string_ptr, "if") != 0)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, i.line_nr, i.column_nr, "if expected");
    return schemerlicht_make_if_expression(&i);
    }
  if (!token_next(ctxt, token_it, token_it_end))
    return schemerlicht_make_if_expression(&i);

  while (!current_token_equals(token_it, token_it_end, ")"))
    {
    schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
    schemerlicht_vector_push_back(ctxt, &i.arguments, e, schemerlicht_expression);
    if (*token_it == *token_it_end)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, last_token.line_nr, last_token.column_nr, ") expected");
      return schemerlicht_make_if_expression(&i);
      }
    }
  if (i.arguments.vector_size < 2 || i.arguments.vector_size > 3)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS, last_token.line_nr, last_token.column_nr, "");
    return schemerlicht_make_if_expression(&i);
    }
  if (i.arguments.vector_size == 2)
    {
    schemerlicht_vector_push_back(ctxt, &i.arguments, make_nop(), schemerlicht_expression);
    }
  return schemerlicht_make_if_expression(&i);
  }

static schemerlicht_expression make_lambda(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return make_nop();
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, "lambda expected");
    return make_nop();
    }

  schemerlicht_parsed_lambda l;
  schemerlicht_vector_init(ctxt, &l.body, schemerlicht_expression);
  schemerlicht_vector_init(ctxt, &l.variables, schemerlicht_string);
  schemerlicht_vector_init(ctxt, &l.free_variables, schemerlicht_string);
  schemerlicht_vector_init(ctxt, &l.assignable_variables, schemerlicht_string);
  l.filename = make_lambda_filename(ctxt);
  l.line_nr = (*token_it)->line_nr;
  l.column_nr = (*token_it)->column_nr;
  if (strcmp((*token_it)->value.string_ptr, "lambda") != 0)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, l.line_nr, l.column_nr, "lambda expected");
    return schemerlicht_make_lambda_expression(&l);
    }
  if (!token_next(ctxt, token_it, token_it_end))
    return schemerlicht_make_lambda_expression(&l);
  l.variable_arity = 0;
  if (!current_token_equals(token_it, token_it_end, "(")) // rest argument
    {
    l.variable_arity = 1;
    schemerlicht_string s;
    schemerlicht_string_copy(ctxt, &s, &(*token_it)->value);
    schemerlicht_vector_push_back(ctxt, &l.variables, s, schemerlicht_string);
    if (!token_next(ctxt, token_it, token_it_end))
      return schemerlicht_make_lambda_expression(&l);
    }
  else
    {
    if (!token_require(ctxt, token_it, token_it_end, "("))
      return schemerlicht_make_lambda_expression(&l);
    int count_after_var = 0;
    while (!current_token_equals(token_it, token_it_end, ")"))
      {
      if (l.variable_arity)
        ++count_after_var;
      if ((*token_it)->type != SCHEMERLICHT_T_ID && strcmp((*token_it)->value.string_ptr, ".") != 0)
        {
        schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, (*token_it)->line_nr, (*token_it)->column_nr, ".");
        return schemerlicht_make_lambda_expression(&l);
        }
      if (strcmp((*token_it)->value.string_ptr, ".") == 0)
        {
        if (l.variable_arity)
          {
          schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, (*token_it)->line_nr, (*token_it)->column_nr, ".");
          return schemerlicht_make_lambda_expression(&l);
          }
        l.variable_arity = 1;
        }
      else
        {
        schemerlicht_string s;
        schemerlicht_string_copy(ctxt, &s, &(*token_it)->value);
        schemerlicht_vector_push_back(ctxt, &l.variables, s, schemerlicht_string);
        }
      if (!token_next(ctxt, token_it, token_it_end))
        return schemerlicht_make_lambda_expression(&l);
      }
    if (l.variable_arity && (count_after_var != 1))
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, last_token.line_nr, last_token.column_nr, "");
      return schemerlicht_make_lambda_expression(&l);
      }
    if (!token_next(ctxt, token_it, token_it_end))
      return schemerlicht_make_lambda_expression(&l);
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
  return schemerlicht_make_lambda_expression(&l);
  }

schemerlicht_expression schemerlicht_init_let(schemerlicht_context* ctxt)
  {
  schemerlicht_parsed_let l;
  l.filename = make_null_string();
  l.line_nr = -1;
  l.column_nr = -1;
  l.bt = schemerlicht_bt_let;
  l.named_let = 0;
  l.let_name = make_null_string();
  schemerlicht_vector_init(ctxt, &l.bindings, schemerlicht_let_binding);
  schemerlicht_vector_init(ctxt, &l.body, schemerlicht_expression);
  schemerlicht_vector_init(ctxt, &l.assignable_variables, schemerlicht_string);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_let;
  expr.expr.let = l;
  return expr;
  }

schemerlicht_expression schemerlicht_init_begin(schemerlicht_context* ctxt)
  {
  schemerlicht_parsed_begin b;
  b.filename = make_null_string();
  b.line_nr = -1;
  b.column_nr = -1;
  schemerlicht_vector_init(ctxt, &b.arguments, schemerlicht_expression);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_begin;
  expr.expr.beg = b;
  return expr;
  }

schemerlicht_expression schemerlicht_init_lambda(schemerlicht_context* ctxt)
  {
  schemerlicht_parsed_lambda l;
  l.filename = make_null_string();
  l.line_nr = -1;
  l.column_nr = -1;
  l.variable_arity = 0;
  schemerlicht_vector_init(ctxt, &l.body, schemerlicht_expression);
  schemerlicht_vector_init(ctxt, &l.variables, schemerlicht_string);
  schemerlicht_vector_init(ctxt, &l.free_variables, schemerlicht_string);
  schemerlicht_vector_init(ctxt, &l.assignable_variables, schemerlicht_string);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_lambda;
  expr.expr.lambda = l;
  return expr;
  }

schemerlicht_expression schemerlicht_init_fixnum(schemerlicht_context* ctxt)
  {
  UNUSED(ctxt);
  schemerlicht_parsed_fixnum fx;
  fx.column_nr = -1;
  fx.line_nr = -1;
  fx.filename = make_null_string();
  fx.value = 0;
  schemerlicht_literal l;
  l.type = schemerlicht_type_fixnum;
  l.lit.fx = fx;
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_literal;
  expr.expr.lit = l;
  return expr;
  }

schemerlicht_expression schemerlicht_init_flonum(schemerlicht_context* ctxt)
  {
  UNUSED(ctxt);
  schemerlicht_parsed_flonum fl;
  fl.column_nr = -1;
  fl.line_nr = -1;
  fl.filename = make_null_string();
  fl.value = 0;
  schemerlicht_literal l;
  l.type = schemerlicht_type_flonum;
  l.lit.fl = fl;
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_literal;
  expr.expr.lit = l;
  return expr;
  }

schemerlicht_expression schemerlicht_init_string(schemerlicht_context* ctxt)
  {
  UNUSED(ctxt);
  schemerlicht_parsed_string s;
  s.column_nr = -1;
  s.line_nr = -1;
  s.filename = make_null_string();
  s.value = make_null_string();
  schemerlicht_literal l;
  l.type = schemerlicht_type_string;
  l.lit.str = s;
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_literal;
  expr.expr.lit = l;
  return expr;
  }

schemerlicht_expression schemerlicht_init_true(schemerlicht_context* ctxt)
  {
  UNUSED(ctxt);
  schemerlicht_parsed_true t;
  t.column_nr = -1;
  t.line_nr = -1;
  t.filename = make_null_string();
  schemerlicht_literal l;
  l.type = schemerlicht_type_true;
  l.lit.t = t;
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_literal;
  expr.expr.lit = l;
  return expr;
  }

schemerlicht_expression schemerlicht_init_false(schemerlicht_context* ctxt)
  {
  UNUSED(ctxt);
  schemerlicht_parsed_false f;
  f.column_nr = -1;
  f.line_nr = -1;
  f.filename = make_null_string();
  schemerlicht_literal l;
  l.type = schemerlicht_type_false;
  l.lit.f = f;
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_literal;
  expr.expr.lit = l;
  return expr;
  }

schemerlicht_expression schemerlicht_init_nil(schemerlicht_context* ctxt)
  {
  UNUSED(ctxt);
  schemerlicht_parsed_nil n;
  n.column_nr = -1;
  n.line_nr = -1;
  n.filename = make_null_string();
  schemerlicht_literal l;
  l.type = schemerlicht_type_nil;
  l.lit.nil = n;
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_literal;
  expr.expr.lit = l;
  return expr;
  }

schemerlicht_expression schemerlicht_init_char(schemerlicht_context* ctxt)
  {
  UNUSED(ctxt);
  schemerlicht_parsed_character ch;
  ch.column_nr = -1;
  ch.line_nr = -1;
  ch.filename = make_null_string();
  ch.value = 0;
  schemerlicht_literal l;
  l.type = schemerlicht_type_character;
  l.lit.ch = ch;
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_literal;
  expr.expr.lit = l;
  return expr;
  }

schemerlicht_expression schemerlicht_init_funcall(schemerlicht_context* ctxt)
  {
  schemerlicht_parsed_funcall f;
  f.filename = make_null_string();
  f.line_nr = -1;
  f.column_nr = -1;
  schemerlicht_vector_init(ctxt, &f.fun, schemerlicht_expression);
  schemerlicht_vector_init(ctxt, &f.arguments, schemerlicht_expression);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_funcall;
  expr.expr.funcall = f;
  return expr;
  }

schemerlicht_expression schemerlicht_init_primcall(schemerlicht_context* ctxt)
  {
  schemerlicht_parsed_primitive_call p;
  p.filename = make_null_string();
  p.line_nr = -1;
  p.column_nr = -1;
  p.as_object = 0;
  //schemerlicht_string_init(ctxt, &p.name, "");
  p.name = make_null_string();
  schemerlicht_vector_init(ctxt, &p.arguments, schemerlicht_expression);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_primitive_call;
  expr.expr.prim = p;
  return expr;
  }

schemerlicht_expression schemerlicht_init_foreign(schemerlicht_context* ctxt)
  {
  schemerlicht_parsed_foreign_call f;
  f.filename = make_null_string();
  f.line_nr = -1;
  f.column_nr = -1;
  f.name = make_null_string();
  schemerlicht_vector_init(ctxt, &f.arguments, schemerlicht_expression);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_foreign_call;
  expr.expr.foreign = f;
  return expr;
  }

schemerlicht_expression schemerlicht_init_if(schemerlicht_context* ctxt)
  {
  schemerlicht_parsed_if i;
  i.filename = make_null_string();
  i.line_nr = -1;
  i.column_nr = -1;
  schemerlicht_vector_init(ctxt, &i.arguments, schemerlicht_expression);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_if;
  expr.expr.i = i;
  return expr;
  }

schemerlicht_expression schemerlicht_init_quote(schemerlicht_context* ctxt)
  {
  UNUSED(ctxt);
  schemerlicht_parsed_quote q;
  q.filename = make_null_string();
  q.line_nr = -1;
  q.column_nr = -1;
  q.qt = schemerlicht_qt_quote;
  q.arg.type = schemerlicht_ct_pair;
  q.arg.value.vector.vector_size = 0;
  q.arg.value.vector.vector_capacity = 0;
  q.arg.value.vector.vector_ptr = NULL;
  q.arg.value.vector.element_size = 0;
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_quote;
  expr.expr.quote = q;
  return expr;
  }

schemerlicht_expression schemerlicht_init_set(schemerlicht_context* ctxt)
  {
  schemerlicht_parsed_set s;
  s.filename = make_null_string();
  s.name = make_null_string();
  s.line_nr = -1;
  s.column_nr = -1;
  s.originates_from_define = 0;
  s.originates_from_quote = 0;
  schemerlicht_vector_init(ctxt, &s.value, schemerlicht_expression);
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_set;
  expr.expr.set = s;
  return expr;
  }

schemerlicht_expression schemerlicht_init_variable(schemerlicht_context* ctxt)
  {
  UNUSED(ctxt);
  schemerlicht_parsed_variable v;
  v.filename = make_null_string();
  v.line_nr = -1;
  v.column_nr = -1;
  v.name = make_null_string();
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_variable;
  expr.expr.var = v;
  return expr;
  }

schemerlicht_expression schemerlicht_init_nop(schemerlicht_context* ctxt)
  {
  UNUSED(ctxt);
  schemerlicht_parsed_nop n;
  n.filename = make_null_string();
  n.line_nr = -1;
  n.column_nr = -1;
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_nop;
  expr.expr.nop = n;
  return expr;
  }

static schemerlicht_expression make_let(schemerlicht_context* ctxt, token** token_it, token** token_it_end, enum schemerlicht_binding_type bt)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return make_nop();
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, "let expected");
    return make_nop();
    }
  schemerlicht_parsed_let l;
  l.filename = make_let_filename(ctxt);
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
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, l.line_nr, l.column_nr, "let expected");
      return schemerlicht_make_let_expression(&l);
      }
    break;
    }
    case schemerlicht_bt_let_star:
    {
    if (strcmp((*token_it)->value.string_ptr, "let*") != 0)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, l.line_nr, l.column_nr, "let* expected");
      return schemerlicht_make_let_expression(&l);
      }
    break;
    }
    case schemerlicht_bt_letrec:
    {
    if (strcmp((*token_it)->value.string_ptr, "letrec") != 0)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, l.line_nr, l.column_nr, "letrec expected");
      return schemerlicht_make_let_expression(&l);
      }
    break;
    }
    }
  if (!token_next(ctxt, token_it, token_it_end))
    return schemerlicht_make_let_expression(&l);
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return schemerlicht_make_let_expression(&l);
    }
  if ((*token_it)->type == SCHEMERLICHT_T_ID) // named let?
    {
    if (bt != schemerlicht_bt_let)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, last_token.line_nr, last_token.column_nr, "( expected");
      return schemerlicht_make_let_expression(&l);
      }
    l.named_let = 1;
    schemerlicht_string_clear(&l.let_name);
    schemerlicht_string_append(ctxt, &l.let_name, &(*token_it)->value);
    if (!token_next(ctxt, token_it, token_it_end))
      return schemerlicht_make_let_expression(&l);
    }
  if (!token_require(ctxt, token_it, token_it_end, "("))
    return schemerlicht_make_let_expression(&l);
  while (!current_token_equals(token_it, token_it_end, ")"))
    {
    if (*token_it == *token_it_end)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
      return schemerlicht_make_let_expression(&l);
      }
    int rlsb = require_left_square_bracket(ctxt, token_it);
    if (*token_it == *token_it_end)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
      return schemerlicht_make_let_expression(&l);
      }
    if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_INVALID_ARGUMENT, (*token_it)->line_nr, (*token_it)->column_nr, "");
      return schemerlicht_make_let_expression(&l);
      }
    schemerlicht_let_binding b;
    schemerlicht_string_copy(ctxt, &b.binding_name, &((*token_it)->value));
    if (!token_next(ctxt, token_it, token_it_end))
      return schemerlicht_make_let_expression(&l);
    b.binding_expr = schemerlicht_make_expression(ctxt, token_it, token_it_end);
    schemerlicht_vector_push_back(ctxt, &l.bindings, b, schemerlicht_let_binding);
    if (!require_right_square_bracket(ctxt, token_it, token_it_end, rlsb))
      return schemerlicht_make_let_expression(&l);
    }
  if (!token_next(ctxt, token_it, token_it_end))
    return schemerlicht_make_let_expression(&l);
  schemerlicht_expression begin = make_begin(ctxt, token_it, token_it_end, 1);
  while (begin.expr.beg.arguments.vector_size == 1 && schemerlicht_vector_at(&begin.expr.beg.arguments, 0, schemerlicht_expression)->type == schemerlicht_type_begin)
    {
    schemerlicht_expression tmp = begin;
    begin = *schemerlicht_vector_at(&tmp.expr.beg.arguments, 0, schemerlicht_expression);
    schemerlicht_vector_destroy(ctxt, &tmp.expr.beg.arguments);
    schemerlicht_string_destroy(ctxt, &tmp.expr.beg.filename);
    }
  schemerlicht_vector_push_back(ctxt, &l.body, begin, schemerlicht_expression);
  return schemerlicht_make_let_expression(&l);
  }

static schemerlicht_expression make_primitive_call(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return make_nop();
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, "");
    return make_nop();
    }
  schemerlicht_parsed_primitive_call p;
  p.as_object = 0;
  schemerlicht_vector_init(ctxt, &p.arguments, schemerlicht_expression);
  p.filename = make_prim_filename(ctxt);
  p.line_nr = (*token_it)->line_nr;
  p.column_nr = (*token_it)->column_nr;
  schemerlicht_string_copy(ctxt, &p.name, &((*token_it)->value));
  if (popped_token.type != SCHEMERLICHT_T_LEFT_ROUND_BRACKET)
    {
    if (!token_next(ctxt, token_it, token_it_end))
      return schemerlicht_make_primitive_call_expression(&p);
    p.as_object = 1;
    schemerlicht_expression expr;
    expr.type = schemerlicht_type_primitive_call;
    expr.expr.prim = p;
    return expr;
    }
  if (!token_next(ctxt, token_it, token_it_end))
    return schemerlicht_make_primitive_call_expression(&p);
  while (!current_token_equals(token_it, token_it_end, ")"))
    {
    schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
    schemerlicht_vector_push_back(ctxt, &p.arguments, e, schemerlicht_expression);
    if (*token_it == *token_it_end)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, last_token.line_nr, last_token.column_nr, ") expected");
      return schemerlicht_make_primitive_call_expression(&p);
      }
    }
  return schemerlicht_make_primitive_call_expression(&p);
  }

static schemerlicht_expression make_set(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return make_nop();
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, "set! expected");
    return make_nop();
    }
  schemerlicht_parsed_set s;
  s.filename = make_set_filename(ctxt);
  s.line_nr = (*token_it)->line_nr;
  s.column_nr = (*token_it)->column_nr;
  s.originates_from_define = 0;
  s.originates_from_quote = 0;
  if (strcmp((*token_it)->value.string_ptr, "set!") != 0)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, s.line_nr, s.column_nr, "set! expected");
    return schemerlicht_make_set_expression(&s);
    }
  if (!token_next(ctxt, token_it, token_it_end))
    return schemerlicht_make_set_expression(&s);
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return schemerlicht_make_set_expression(&s);
    }
  if (current_token_type(token_it, token_it_end) != SCHEMERLICHT_T_ID)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, "");
    return schemerlicht_make_set_expression(&s);
    }
  schemerlicht_string_copy(ctxt, &s.name, &((*token_it)->value));
  if (!token_next(ctxt, token_it, token_it_end))
    return schemerlicht_make_set_expression(&s);
  schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
  schemerlicht_vector_init(ctxt, &s.value, schemerlicht_expression);
  schemerlicht_vector_push_back(ctxt, &s.value, e, schemerlicht_expression);
  return schemerlicht_make_set_expression(&s);
  }

static schemerlicht_expression make_fun(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return make_nop();
    }
  schemerlicht_parsed_funcall f;
  f.line_nr = (*token_it)->line_nr;
  f.column_nr = (*token_it)->column_nr;
  f.filename = make_funcall_filename(ctxt);
  schemerlicht_vector_init(ctxt, &f.arguments, schemerlicht_expression);
  schemerlicht_vector_init(ctxt, &f.fun, schemerlicht_expression);
  schemerlicht_expression e_fun = schemerlicht_make_expression(ctxt, token_it, token_it_end);
  schemerlicht_vector_push_back(ctxt, &f.fun, e_fun, schemerlicht_expression);
  while (!current_token_equals(token_it, token_it_end, ")"))
    {
    schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
    schemerlicht_vector_push_back(ctxt, &f.arguments, e, schemerlicht_expression);
    if (*token_it == *token_it_end)
      {
      schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, last_token.line_nr, last_token.column_nr, ") expected");
      return schemerlicht_make_funcall_expression(&f);
      }
    }
  return schemerlicht_make_funcall_expression(&f);
  }

static schemerlicht_expression make_literal(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return make_nop();
    }
  switch (current_token_type(token_it, token_it_end))
    {
    case SCHEMERLICHT_T_LEFT_ROUND_BRACKET:
    {
    schemerlicht_parsed_nil n;
    n.line_nr = (*token_it)->line_nr;
    n.column_nr = (*token_it)->column_nr;
    n.filename = make_lit_filename(ctxt);
    schemerlicht_expression expr;
    expr.type = schemerlicht_type_literal;
    expr.expr.lit.type = schemerlicht_type_nil;
    expr.expr.lit.lit.nil = n;
    if (!token_next(ctxt, token_it, token_it_end))
      return expr;
    token_require(ctxt, token_it, token_it_end, ")");
    return expr;
    }
    case SCHEMERLICHT_T_FIXNUM:
    {
    schemerlicht_parsed_fixnum f;
    f.line_nr = (*token_it)->line_nr;
    f.column_nr = (*token_it)->column_nr;
    f.value = schemerlicht_to_fixnum((*token_it)->value.string_ptr);
    f.filename = make_lit_filename(ctxt);
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
    f.value = schemerlicht_to_flonum((*token_it)->value.string_ptr);
    f.filename = make_lit_filename(ctxt);
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
    s.filename = make_lit_filename(ctxt);
    schemerlicht_string_init_ranged(ctxt, &s.value, (*token_it)->value.string_ptr + 1, (*token_it)->value.string_ptr + (*token_it)->value.string_length - 1);
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
    if (current_token_equals(token_it, token_it_end, "#t"))
      {
      schemerlicht_parsed_true t;
      t.line_nr = ln;
      t.column_nr = cn;
      t.filename = make_lit_filename(ctxt);
      schemerlicht_expression expr;
      expr.type = schemerlicht_type_literal;
      expr.expr.lit.type = schemerlicht_type_true;
      expr.expr.lit.lit.t = t;
      token_next(ctxt, token_it, token_it_end);
      return expr;
      }
    else if (current_token_equals(token_it, token_it_end, "#f"))
      {
      schemerlicht_parsed_false f;
      f.line_nr = ln;
      f.column_nr = cn;
      f.filename = make_lit_filename(ctxt);
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
        ch.filename = make_lit_filename(ctxt);
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
            case 'r': if (strcmp(substr.string_ptr, "return") == 0) ch.value = 13; if (strcmp(substr.string_ptr, "rubout") == 0) ch.value = 127; break;
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
  if (obj == NULL || obj->type == schemerlicht_object_type_undefined)
    return schemerlicht_et_funcall_or_variable;
  schemerlicht_assert(obj->type == schemerlicht_object_type_fixnum);
  return obj->value.fx;
  }

schemerlicht_expression schemerlicht_make_expression(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (ctxt->number_of_syntax_errors > SCHEMERLICHT_MAX_SYNTAX_ERRORS)
    return make_nop();
  if (*token_it == *token_it_end)
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS, last_token.line_nr, last_token.column_nr, "");
    return make_nop();
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
    if (!token_next(ctxt, token_it, token_it_end))
      return make_nop();
    if (function_call)
      {
      schemerlicht_parsed_funcall f;
      f.filename = make_funcall_filename(ctxt);
      schemerlicht_vector_init(ctxt, &f.arguments, schemerlicht_expression);
      schemerlicht_vector_init(ctxt, &f.fun, schemerlicht_expression);
      f.line_nr = ln;
      f.column_nr = cn;
      schemerlicht_vector_push_back(ctxt, &f.fun, schemerlicht_make_expression(ctxt, token_it, token_it_end), schemerlicht_expression);
      if (!token_require(ctxt, token_it, token_it_end, ")"))
        return schemerlicht_make_funcall_expression(&f);
      if (!current_token_equals(token_it, token_it_end, ")"))
        {
        while (!current_token_equals(token_it, token_it_end, ")"))
          {
          schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
          schemerlicht_vector_push_back(ctxt, &f.arguments, e, schemerlicht_expression);
          if (*token_it == *token_it_end)
            {
            schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, last_token.line_nr, last_token.column_nr, ") expected");
            return schemerlicht_make_funcall_expression(&f);
            }
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
      if (current_token_equals(token_it, token_it_end, ")"))
        {
        token_next(ctxt, token_it, token_it_end);
        return expr;
        }
      else
        {
        schemerlicht_parsed_funcall f;
        f.filename = make_funcall_filename(ctxt);
        schemerlicht_vector_init(ctxt, &f.arguments, schemerlicht_expression);
        schemerlicht_vector_init(ctxt, &f.fun, schemerlicht_expression);
        f.line_nr = ln;
        f.column_nr = cn;
        schemerlicht_vector_push_back(ctxt, &f.fun, expr, schemerlicht_expression);
        while (!current_token_equals(token_it, token_it_end, ")"))
          {
          schemerlicht_expression e = schemerlicht_make_expression(ctxt, token_it, token_it_end);
          schemerlicht_vector_push_back(ctxt, &f.arguments, e, schemerlicht_expression);
          if (*token_it == *token_it_end)
            {
            schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_EXPECTED_KEYWORD, last_token.line_nr, last_token.column_nr, ") expected");
            return schemerlicht_make_funcall_expression(&f);
            }
          }
        token_require(ctxt, token_it, token_it_end, ")");
        return schemerlicht_make_funcall_expression(&f);
        }
      }
    }
    case SCHEMERLICHT_T_RIGHT_ROUND_BRACKET:
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, ") expected");
    return make_nop();
    }
    case SCHEMERLICHT_T_BAD:
    {
    schemerlicht_syntax_error_cstr(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr, "");
    return make_nop();
    }
    case SCHEMERLICHT_T_FIXNUM:
      return make_literal(ctxt, token_it, token_it_end);
    case SCHEMERLICHT_T_FLONUM:
      return make_literal(ctxt, token_it, token_it_end);
    case SCHEMERLICHT_T_STRING:
      return make_literal(ctxt, token_it, token_it_end);
    case SCHEMERLICHT_T_SYMBOL:
      if (current_token_equals(token_it, token_it_end, "#undefined"))
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
  last_token = make_bad_token();
  schemerlicht_syntax_errors_clear(ctxt);
  invalidate_popped();
  schemerlicht_program prog;
  schemerlicht_vector_init(ctxt, &prog.expressions, schemerlicht_expression);

  token* token_it = schemerlicht_vector_begin(tokens, token);
  token* token_it_end = schemerlicht_vector_end(tokens, token);

  for (; token_it != token_it_end;)
    {
    if (ctxt->number_of_syntax_errors > SCHEMERLICHT_MAX_SYNTAX_ERRORS)
      return prog;
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
static void postvisit_case(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &e->expr.cas.filename);
  schemerlicht_cell* it = schemerlicht_vector_begin(&e->expr.cas.datum_args, schemerlicht_cell);
  schemerlicht_cell* it_end = schemerlicht_vector_end(&e->expr.cas.datum_args, schemerlicht_cell);
  for (; it != it_end; ++it)
    schemerlicht_destroy_cell(ctxt, it);
  schemerlicht_vector* vit = schemerlicht_vector_begin(&e->expr.cas.then_bodies, schemerlicht_vector);
  schemerlicht_vector* vit_end = schemerlicht_vector_end(&e->expr.cas.then_bodies, schemerlicht_vector);
  for (; vit != vit_end; ++vit)
    schemerlicht_vector_destroy(ctxt, vit);
  schemerlicht_vector_destroy(ctxt, &e->expr.cas.datum_args);
  schemerlicht_vector_destroy(ctxt, &e->expr.cas.val_expr);
  schemerlicht_vector_destroy(ctxt, &e->expr.cas.then_bodies);
  schemerlicht_vector_destroy(ctxt, &e->expr.cas.else_body);
  }
static void postvisit_cond(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &e->expr.cond.filename);
  schemerlicht_vector* vit = schemerlicht_vector_begin(&e->expr.cond.arguments, schemerlicht_vector);
  schemerlicht_vector* vit_end = schemerlicht_vector_end(&e->expr.cond.arguments, schemerlicht_vector);
  for (; vit != vit_end; ++vit)
    schemerlicht_vector_destroy(ctxt, vit);
  schemerlicht_vector_destroy(ctxt, &e->expr.cond.arguments);
  schemerlicht_vector_destroy(ctxt, &e->expr.cond.is_proc);
  }
static void postvisit_do(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  UNUSED(v);
  schemerlicht_string_destroy(ctxt, &e->expr.d.filename);
  schemerlicht_vector* vit = schemerlicht_vector_begin(&e->expr.d.bindings, schemerlicht_vector);
  schemerlicht_vector* vit_end = schemerlicht_vector_end(&e->expr.d.bindings, schemerlicht_vector);
  for (; vit != vit_end; ++vit)
    schemerlicht_vector_destroy(ctxt, vit);
  schemerlicht_vector_destroy(ctxt, &e->expr.d.bindings);
  schemerlicht_vector_destroy(ctxt, &e->expr.d.test);
  schemerlicht_vector_destroy(ctxt, &e->expr.d.commands);
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
  destroyer.visitor->postvisit_case = postvisit_case;
  destroyer.visitor->postvisit_cond = postvisit_cond;
  destroyer.visitor->postvisit_do = postvisit_do;
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
  map_insert(ctxt, m, "add1", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "and", schemerlicht_et_primitive_call);  
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
  //map_insert(ctxt, m, "close-file", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "closure", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "closure?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "closure-ref", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "cons", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "current-seconds", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "current-milliseconds", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "define", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "defmacro", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "define-macro", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "delay", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "eq?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "eqv?", schemerlicht_et_primitive_call);  
  //map_insert(ctxt, m, "eof-object?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "equal?", schemerlicht_et_primitive_call);  
  //map_insert(ctxt, m, "file-exists?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fixnum->char", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fixnum->flonum", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "fixnum?", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "fixnum-expt", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "flonum?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "flonum->fixnum", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "flonum-expt", schemerlicht_et_primitive_call);  
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
  //map_insert(ctxt, m, "getenv", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "halt", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "ieee754-sign", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "ieee754-exponent", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "ieee754-mantissa", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "ieee754-sin", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "ieee754-cos", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "ieee754-tan", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "ieee754-asin", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "ieee754-acos", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "ieee754-atan1", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "ieee754-log", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "ieee754-round", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "ieee754-sqrt", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "ieee754-truncate", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "ieee754-pi", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "include", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "input-port?", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "load", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "length", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "list", schemerlicht_et_primitive_call);  
  map_insert(ctxt, m, "list?", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "make-port", schemerlicht_et_primitive_call);  
  map_insert(ctxt, m, "make-string", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "make-vector", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "max", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "member", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "memv", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "memq", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "min", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "not", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "null?", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "num2str", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "or", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "open-file", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "output-port?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "pair?", schemerlicht_et_primitive_call);  
  //map_insert(ctxt, m, "port?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "procedure?", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "promise?", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "putenv", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "quotient", schemerlicht_et_primitive_call);  
  map_insert(ctxt, m, "reclaim", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "reclaim-garbage", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "remainder", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "set-car!", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "set-cdr!", schemerlicht_et_primitive_call);
  //map_insert(ctxt, m, "str2num", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-append", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-copy", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-fill!", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-hash", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-length", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-ref", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-set!", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string->symbol", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string=?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string>?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string<?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string>=?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string<=?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-ci=?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-ci>?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-ci<?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-ci>=?", schemerlicht_et_primitive_call);
  map_insert(ctxt, m, "string-ci<=?", schemerlicht_et_primitive_call);
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

schemerlicht_expression schemerlicht_make_nop_expression(schemerlicht_parsed_nop* l)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_nop;
  expr.expr.nop = *l;
  return expr;
  }

schemerlicht_expression schemerlicht_make_primitive_call_expression(schemerlicht_parsed_primitive_call* l)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_primitive_call;
  expr.expr.prim = *l;
  return expr;
  }

schemerlicht_expression schemerlicht_make_foreign_call_expression(schemerlicht_parsed_foreign_call* l)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_foreign_call;
  expr.expr.foreign = *l;
  return expr;
  }

schemerlicht_expression schemerlicht_make_if_expression(schemerlicht_parsed_if* l)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_if;
  expr.expr.i = *l;
  return expr;
  }

schemerlicht_expression schemerlicht_make_cond_expression(schemerlicht_parsed_cond* l)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_cond;
  expr.expr.cond = *l;
  return expr;
  }

schemerlicht_expression schemerlicht_make_case_expression(schemerlicht_parsed_case* l)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_case;
  expr.expr.cas = *l;
  return expr;
  }

schemerlicht_expression schemerlicht_make_do_expression(schemerlicht_parsed_do* l)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_do;
  expr.expr.d = *l;
  return expr;
  }

schemerlicht_expression schemerlicht_make_quote_expression(schemerlicht_parsed_quote* l)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_quote;
  expr.expr.quote = *l;
  return expr;
  }

schemerlicht_expression schemerlicht_make_funcall_expression(schemerlicht_parsed_funcall* l)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_funcall;
  expr.expr.funcall = *l;
  return expr;
  }

schemerlicht_expression schemerlicht_make_set_expression(schemerlicht_parsed_set* l)
  {
  schemerlicht_expression expr;
  expr.type = schemerlicht_type_set;
  expr.expr.set = *l;
  return expr;
  }