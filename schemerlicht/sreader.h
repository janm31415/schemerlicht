#ifndef SCHEMERLICHT_READER_H
#define SCHEMERLICHT_READER_H

#include "schemerlicht.h"
#include "svector.h"
#include "sstring.h"
#include "stoken.h"

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

void schemerlicht_dump_cell_to_string(schemerlicht_context* ctxt, schemerlicht_cell* c, schemerlicht_string* s);

#endif //SCHEMERLICHT_READER_H