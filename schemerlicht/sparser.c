#include "sparser.h"
#include "svector.h"
#include "stoken.h"
#include "serror.h"

int schemerlicht_current_token_type(token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    return SCHEMERLICHT_T_BAD;
  return (*token_it)->type;
  }

schemerlicht_expression schemerlicht_make_empty_expression()
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

void schemerlicht_token_next(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
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

schemerlicht_expression schemerlicht_make_literal(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {
  if (*token_it == *token_it_end)
    {
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);
    //*token_it = *token_it_end;
    return schemerlicht_make_empty_expression();
    }
  switch (schemerlicht_current_token_type(token_it, token_it_end))
    {
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
    schemerlicht_token_next(ctxt, token_it, token_it_end);
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
    schemerlicht_token_next(ctxt, token_it, token_it_end);
    return expr;
    }
    default:
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
      *token_it = *token_it_end;
      return schemerlicht_make_empty_expression();
    }
  }

schemerlicht_expression schemerlicht_make_expression(schemerlicht_context* ctxt, token** token_it, token** token_it_end)
  {  
  if (*token_it == *token_it_end)
    {
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NO_TOKENS);  
    *token_it = *token_it_end;
    return schemerlicht_make_empty_expression();
    }

  switch (schemerlicht_current_token_type(token_it, token_it_end))
    {
    case SCHEMERLICHT_T_FIXNUM:
      return schemerlicht_make_literal(ctxt, token_it, token_it_end);
    case SCHEMERLICHT_T_FLONUM:
      return schemerlicht_make_literal(ctxt, token_it, token_it_end);
    default:
      schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);
      *token_it = *token_it_end;
      return schemerlicht_make_empty_expression();
    }
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

void schemerlicht_program_destroy(schemerlicht_context* ctxt, schemerlicht_program* p)
  {
  schemerlicht_vector_destroy(ctxt, &p->expressions);
  }