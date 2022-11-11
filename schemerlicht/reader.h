#ifndef SCHEMERLICHT_READER_H
#define SCHEMERLICHT_READER_H

#include "schemerlicht.h"
#include "vector.h"
#include "string.h"
#include "token.h"
#include "object.h"

enum schemerlicht_cell_type
  {
  schemerlicht_ct_symbol,
  schemerlicht_ct_fixnum,
  schemerlicht_ct_flonum,
  schemerlicht_ct_string,
  schemerlicht_ct_pair,
  schemerlicht_ct_vector,
  schemerlicht_ct_invalid_cell
  };

typedef struct schemerlicht_cell
  {
  enum schemerlicht_cell_type type;
  union
    {
    schemerlicht_string str;
    schemerlicht_vector vector;
    } value;
  } schemerlicht_cell;

int schemerlicht_cell_equals(const schemerlicht_cell* left, const schemerlicht_cell* right);

void schemerlicht_destroy_cell(schemerlicht_context* ctxt, schemerlicht_cell* c);

schemerlicht_cell schemerlicht_make_false_sym_cell(schemerlicht_context* ctxt);
schemerlicht_cell schemerlicht_make_true_sym_cell(schemerlicht_context* ctxt);
schemerlicht_cell schemerlicht_make_nil_sym_cell(schemerlicht_context* ctxt);

schemerlicht_cell schemerlicht_read_quote(schemerlicht_context* ctxt, token** token_it, token** token_it_end, int quasiquote);

schemerlicht_cell schemerlicht_read_datum(schemerlicht_context* ctxt, schemerlicht_string* buff, void* stream, schemerlicht_get_char_fun get_char, schemerlicht_next_char_fun next_char, schemerlicht_get_position_fun get_position);

void schemerlicht_dump_cell_to_string(schemerlicht_context* ctxt, schemerlicht_cell* c, schemerlicht_string* s);

SCHEMERLICHT_API schemerlicht_cell schemerlicht_cell_copy(schemerlicht_context* ctxt, schemerlicht_cell* c);

schemerlicht_object schemerlicht_cell_to_object(schemerlicht_context* ctxt, schemerlicht_cell* c);

int schemerlicht_is_char(schemerlicht_cell* c, char* ch);

#endif //SCHEMERLICHT_READER_H