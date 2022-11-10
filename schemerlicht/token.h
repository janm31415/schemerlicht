#ifndef SCHEMERLICHT_TOKEN_H
#define SCHEMERLICHT_TOKEN_H

#include "schemerlicht.h"
#include "string.h"
#include "stream.h"
#include "vector.h"

SCHEMERLICHT_API schemerlicht_flonum schemerlicht_to_flonum(const char* value);
SCHEMERLICHT_API schemerlicht_fixnum schemerlicht_to_fixnum(const char* value);
SCHEMERLICHT_API int schemerlicht_is_number(int* is_real, int* is_scientific, const char* value);

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

token schemerlicht_make_token(schemerlicht_context* ctxt, int type, int line_nr, int column_nr, schemerlicht_string* value);
token schemerlicht_make_token_cstr(schemerlicht_context* ctxt, int type, int line_nr, int column_nr, const char* value);

typedef struct schemerlicht_read_token_state
  {
  int line_nr;
  int column_nr;
  } schemerlicht_read_token_state;

int schemerlicht_read_token(token* tok, schemerlicht_context* ctxt, schemerlicht_string* buff, schemerlicht_stream* str, schemerlicht_read_token_state* state);

typedef int (*schemerlicht_get_char_fun)(char*, void*, int);
typedef void (*schemerlicht_next_char_fun)(void*);
typedef schemerlicht_fixnum (*schemerlicht_get_position_fun)(void*);
int schemerlicht_read_token_polymorph(token* tok, schemerlicht_context* ctxt, schemerlicht_string* buff, void* str, schemerlicht_read_token_state* state, schemerlicht_get_char_fun get_char, schemerlicht_next_char_fun next_char, schemerlicht_get_position_fun get_position);

void schemerlicht_token_destroy(schemerlicht_context* ctxt, token* tok);

/*
* return vector contains token types
*/
SCHEMERLICHT_API schemerlicht_vector tokenize(schemerlicht_context* ctxt, schemerlicht_stream* str);

SCHEMERLICHT_API void destroy_tokens_vector(schemerlicht_context* ctxt, schemerlicht_vector* tokens);

#endif //SCHEMERLICHT_TOKEN_H