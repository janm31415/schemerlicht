#include "reader_tests.h"
#include "test_assert.h"
#include "token_tests.h"

#include "schemerlicht/sreader.h"
#include "schemerlicht/scontext.h"
#include "schemerlicht/svector.h"
#include "schemerlicht/stoken.h"

static void test_sym_cells()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_cell f1 = schemerlicht_make_false_sym_cell(ctxt);
  schemerlicht_cell f2 = schemerlicht_make_false_sym_cell(ctxt);
  schemerlicht_cell t1 = schemerlicht_make_true_sym_cell(ctxt);
  schemerlicht_cell t2 = schemerlicht_make_true_sym_cell(ctxt);
  schemerlicht_cell n1 = schemerlicht_make_nil_sym_cell(ctxt);
  schemerlicht_cell n2 = schemerlicht_make_nil_sym_cell(ctxt);
  TEST_EQ_INT(1, schemerlicht_cell_equals(&f1, &f2));
  TEST_EQ_INT(1, schemerlicht_cell_equals(&t1, &t2));
  TEST_EQ_INT(1, schemerlicht_cell_equals(&n1, &n2));
  TEST_EQ_INT(0, schemerlicht_cell_equals(&f1, &t2));
  TEST_EQ_INT(0, schemerlicht_cell_equals(&f1, &n2));
  TEST_EQ_INT(0, schemerlicht_cell_equals(&t1, &f2));
  TEST_EQ_INT(0, schemerlicht_cell_equals(&t1, &n2));
  TEST_EQ_INT(0, schemerlicht_cell_equals(&n1, &f2));
  TEST_EQ_INT(0, schemerlicht_cell_equals(&n1, &t2));
  TEST_EQ_INT(1, schemerlicht_cell_equals(&f1, &ctxt->global->false_sym));
  TEST_EQ_INT(1, schemerlicht_cell_equals(&t1, &ctxt->global->true_sym));
  TEST_EQ_INT(1, schemerlicht_cell_equals(&n1, &ctxt->global->nil_sym));
  schemerlicht_destroy_cell(ctxt, &f1);
  schemerlicht_destroy_cell(ctxt, &f2);
  schemerlicht_destroy_cell(ctxt, &t1);
  schemerlicht_destroy_cell(ctxt, &t2);
  schemerlicht_destroy_cell(ctxt, &n1);
  schemerlicht_destroy_cell(ctxt, &n2);
  schemerlicht_close(ctxt);
  }

static void test_read_vector_cells()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "#(1 2 3)");

  token* token_it = schemerlicht_vector_begin(&tokens, token);
  token* token_it_end = schemerlicht_vector_end(&tokens, token);

  schemerlicht_cell c = schemerlicht_read_quote(ctxt, &token_it, &token_it_end, 0);

  TEST_EQ_INT(schemerlicht_ct_vector, c.type);
  TEST_EQ_INT(3, c.value.vector.vector_size);

  schemerlicht_destroy_cell(ctxt, &c);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_close(ctxt);
  }

static void test_read_pair_cells_1()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(1 2 3)");

  token* token_it = schemerlicht_vector_begin(&tokens, token);
  token* token_it_end = schemerlicht_vector_end(&tokens, token);

  schemerlicht_cell c = schemerlicht_read_quote(ctxt, &token_it, &token_it_end, 0);

  TEST_EQ_INT(schemerlicht_ct_pair, c.type);
  TEST_EQ_INT(2, c.value.vector.vector_size);

  schemerlicht_destroy_cell(ctxt, &c);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_close(ctxt);
  }

void run_all_reader_tests()
  {
  test_sym_cells();
  test_read_vector_cells();
  test_read_pair_cells_1();
  }