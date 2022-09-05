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
  T_BAD,
  T_QUOTE, // (')
  T_BACKQUOTE, // (`)
  T_UNQUOTE, // comma (,)
  T_UNQUOTE_SPLICING, // comma at (,@)
  T_LEFT_ROUND_BRACKET,
  T_RIGHT_ROUND_BRACKET,
  T_LEFT_SQUARE_BRACKET,
  T_RIGHT_SQUARE_BRACKET,
  T_FIXNUM,
  T_FLONUM,
  T_STRING,
  T_SYMBOL,
  T_ID
  };

typedef union 
  {
  schemerlicht_flonum flonum;
  schemerlicht_fixnum fixnum;
  schemerlicht_string value;
  } semantics_info;


typedef struct token 
  {
  semantics_info info;
  int type;
  int line_nr;
  int column_nr;
  } token;

token make_token(schemerlicht_context* ctxt, int type, int line_nr, int column_nr, schemerlicht_string* value);
token make_token_cstr(schemerlicht_context* ctxt, int type, int line_nr, int column_nr, const char* value);
token make_token_flonum(int line_nr, int column_nr, schemerlicht_flonum value);
token make_token_fixnum(int line_nr, int column_nr, schemerlicht_fixnum value);

/*
* return vector contains token types
*/
SCHEMERLICHT_API schemerlicht_vector tokenize(schemerlicht_context* ctxt, schemerlicht_stream* str);

SCHEMERLICHT_API void destroy_tokens_vector(schemerlicht_context* ctxt, schemerlicht_vector* tokens);

#endif //SCHEMERLICHT_TOKEN_H