#include "reader_tests.h"
#include "test_assert.h"
#include "token_tests.h"

#include "schemerlicht/reader.h"
#include "schemerlicht/context.h"
#include "schemerlicht/vector.h"
#include "schemerlicht/token.h"

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

static void test_dump_cell(const char* script)
  {

  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, script);

  token* token_it = schemerlicht_vector_begin(&tokens, token);
  token* token_it_end = schemerlicht_vector_end(&tokens, token);

  schemerlicht_cell c = schemerlicht_read_quote(ctxt, &token_it, &token_it_end, 0);

  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "");

  schemerlicht_dump_cell_to_string(ctxt, &c, &s);

  TEST_EQ_STRING(script, s.string_ptr);

  schemerlicht_string_destroy(ctxt, &s);

  schemerlicht_destroy_cell(ctxt, &c);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_close(ctxt);
  }

void run_all_reader_tests()
  {
  test_sym_cells();
  test_read_vector_cells();
  test_read_pair_cells_1();
  test_dump_cell("(1 2 3)");
  test_dump_cell("#(1 2 3)");
  test_dump_cell("#t");
  test_dump_cell("#f");
  test_dump_cell("()");
  test_dump_cell("30531");
  test_dump_cell("3.14");
  test_dump_cell("(1 2 3 4 5)");
  test_dump_cell("(#(2 1) 4)");
  test_dump_cell("(1 2 #(2 1) 4 5)");
  test_dump_cell("(1 . 2)");
  test_dump_cell("(1 2 #(2 1) . #(4 5))");
  }