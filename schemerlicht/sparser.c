#include "sparser.h"
#include "svector.h"
#include "stoken.h"
#include "serror.h"

#include <string.h>
#include <stdlib.h>

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
  return strcmp((*token_it)->info.value.string_ptr, expected) == 0;
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
      if (s.string_length >= 2 && (*schemerlicht_string_at(&s,0) == '#') && (*schemerlicht_string_at(&s, 1) == '\\'))
        {
        schemerlicht_string substr;
        schemerlicht_string_init_ranged(ctxt, &substr, s.string_ptr+2, s.string_ptr + s.string_length);
        schemerlicht_parsed_character ch;
        ch.line_nr = ln;
        ch.column_nr = cn;
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
      //*token_it = *token_it_end;
      //return make_nop();
    }
  return make_nop();
  }

schemerlicht_expression schemerlicht_make_expression(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
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
    if (next_token_equals(token_it, token_it_end, ")"))
      {
      return make_literal(ctxt, token_it, token_it_end);
      }
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
    //*token_it = *token_it_end;
    //return make_nop();
    }
    case SCHEMERLICHT_T_RIGHT_ROUND_BRACKET:
    {
    schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr);
    //*token_it = *token_it_end;
    //return make_nop();
    }
    case SCHEMERLICHT_T_BAD:
    {
    schemerlicht_throw_parser(ctxt, SCHEMERLICHT_ERROR_BAD_SYNTAX, (*token_it)->line_nr, (*token_it)->column_nr);
    //*token_it = *token_it_end;
    //return make_nop();
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
    default:
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
      //*token_it = *token_it_end;
      //return make_nop();
    }
  return make_nop();
  }

schemerlicht_program make_program(schemerlicht_context* ctxt, schemerlicht_vector* tokens)
  {
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

static void literal_destroy(schemerlicht_context* ctxt, schemerlicht_literal* lit)
  {
  switch (lit->type)
    {
    case schemerlicht_type_string:
      schemerlicht_string_destroy(ctxt, &lit->lit.str.value);
      break;
    }
  }

static void expression_destroy(schemerlicht_context* ctxt, schemerlicht_expression* e)
  {
  switch (e->type)
    {
    case schemerlicht_type_literal:
      literal_destroy(ctxt, &e->expr.lit);
      break;
    }
  }

void schemerlicht_program_destroy(schemerlicht_context* ctxt, schemerlicht_program* p)
  {
  schemerlicht_expression* expr_it = schemerlicht_vector_begin(&p->expressions, schemerlicht_expression);
  schemerlicht_expression* expr_it_end = schemerlicht_vector_end(&p->expressions, schemerlicht_expression);
  for (; expr_it != expr_it_end; ++expr_it)
    {
    expression_destroy(ctxt, expr_it);
    }
  schemerlicht_vector_destroy(ctxt, &p->expressions);
  }