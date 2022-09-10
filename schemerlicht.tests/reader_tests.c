#include "reader_tests.h"
#include "test_assert.h"

#include "schemerlicht/sreader.h"
#include "schemerlicht/scontext.h"

static void test_sym_cells()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_cell f1 = make_false_sym_cell(ctxt);
  schemerlicht_cell f2 = make_false_sym_cell(ctxt);
  schemerlicht_cell t1 = make_true_sym_cell(ctxt);
  schemerlicht_cell t2 = make_true_sym_cell(ctxt);
  schemerlicht_cell n1 = make_nil_sym_cell(ctxt);
  schemerlicht_cell n2 = make_nil_sym_cell(ctxt);
  TEST_EQ_INT(1, schemerlicht_cell_equals(&f1, &f2));
  TEST_EQ_INT(1, schemerlicht_cell_equals(&t1, &t2));
  TEST_EQ_INT(1, schemerlicht_cell_equals(&n1, &n2));
  TEST_EQ_INT(0, schemerlicht_cell_equals(&f1, &t2));
  TEST_EQ_INT(0, schemerlicht_cell_equals(&f1, &n2));
  TEST_EQ_INT(0, schemerlicht_cell_equals(&t1, &f2));
  TEST_EQ_INT(0, schemerlicht_cell_equals(&t1, &n2));
  TEST_EQ_INT(0, schemerlicht_cell_equals(&n1, &f2));
  TEST_EQ_INT(0, schemerlicht_cell_equals(&n1, &t2));
  schemerlicht_destroy_cell(ctxt, &f1);
  schemerlicht_destroy_cell(ctxt, &f2);
  schemerlicht_destroy_cell(ctxt, &t1);
  schemerlicht_destroy_cell(ctxt, &t2);
  schemerlicht_destroy_cell(ctxt, &n1);
  schemerlicht_destroy_cell(ctxt, &n2);
  schemerlicht_close(ctxt);
  }

void run_all_reader_tests()
  {
  test_sym_cells();
  }