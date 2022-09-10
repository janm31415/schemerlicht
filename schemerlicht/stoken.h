#ifndef SCHEMERLICHT_TOKEN_H
#define SCHEMERLICHT_TOKEN_H

#include "schemerlicht.h"
#include "sstring.h"
#include "sstream.h"
#include "svector.h"

SCHEMERLICHT_API schemerlicht_flonum to_flonum(const char* value);
SCHEMERLICHT_API schemerlicht_fixnum to_fixnum(const char* value);
SCHEMERLICHT_API int is_number(int* is_real, int* is_scientific, const char* value);

enum token_type
  {
  SCHEMERLICHT_T_BAD,
  SCHEMERLICHT_T_QUOTE, // (')
  SCHEMERLICHT_T_BACKQUOTE, // (`)
  SCHEMERLICHT_T_UNQUOTE, // comma (,)
  SCHEMERLICHT_T_UNQUOTE_SPLICING, // comma at (,@)
  SCHEMERLICHT_T_LEFT_ROUND_BRACKET,
  SCHEMERLICHT_T_RIGHT_ROUND_BRACKET,
  SCHEMERLICHT_T_LEFT_SQUARE_BRACKET,
  SCHEMERLICHT_T_RIGHT_SQUARE_BRACKET,
  SCHEMERLICHT_T_FIXNUM,
  SCHEMERLICHT_T_FLONUM,
  SCHEMERLICHT_T_STRING,
  SCHEMERLICHT_T_SYMBOL,
  SCHEMERLICHT_T_ID
  };


typedef struct token 
  {
  schemerlicht_string value;
  int type;
  int line_nr;
  int column_nr;
  } token;

token make_token(schemerlicht_context* ctxt, int type, int line_nr, int column_nr, schemerlicht_string* value);
token make_token_cstr(schemerlicht_context* ctxt, int type, int line_nr, int column_nr, const char* value);

/*
* return vector contains token types
*/
SCHEMERLICHT_API schemerlicht_vector tokenize(schemerlicht_context* ctxt, schemerlicht_stream* str);

SCHEMERLICHT_API void destroy_tokens_vector(schemerlicht_context* ctxt, schemerlicht_vector* tokens);

#endif //SCHEMERLICHT_TOKEN_H