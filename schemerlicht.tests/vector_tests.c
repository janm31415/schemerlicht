#include "vector_tests.h"
#include "test_assert.h"

#include "schemerlicht/schemerlicht.h"
#include "schemerlicht/svector.h"
#include "schemerlicht/scontext.h"

void test_construction_destruction_1()
  {
  schemerlicht_context ctxt;
  schemerlicht_vector v;
  schemerlicht_vector_init_with_size(&ctxt, &v, 5, double);
  TEST_EQ_INT(5, v.vector_size);
  TEST_EQ_INT(5, v.vector_capacity);
  TEST_EQ_INT(sizeof(double), v.element_size);
  schemerlicht_vector_destroy(&ctxt, &v);
  }

void test_construction_destruction_2()
  {
  schemerlicht_context ctxt;
  schemerlicht_vector v;
  schemerlicht_vector_init(&ctxt, &v, double);
  TEST_EQ_INT(0, v.vector_size);
  TEST_EQ_INT(8, v.vector_capacity);
  TEST_EQ_INT(sizeof(double), v.element_size);
  schemerlicht_vector_destroy(&ctxt, &v);
  }

void test_access()
  {
  schemerlicht_context ctxt;
  schemerlicht_vector v;
  schemerlicht_vector_init_with_size(&ctxt, &v, 5, double);
  *schemerlicht_vector_at(&ctxt, &v, 0, double) = 1.0;
  *schemerlicht_vector_at(&ctxt, &v, 1, double) = 2.0;
  *schemerlicht_vector_at(&ctxt, &v, 2, double) = 3.0;
  *schemerlicht_vector_at(&ctxt, &v, 3, double) = 4.0;
  *schemerlicht_vector_at(&ctxt, &v, 4, double) = 5.0;
  TEST_EQ_DOUBLE(1.0, *schemerlicht_vector_at(&ctxt, &v, 0, double));
  TEST_EQ_DOUBLE(2.0, *schemerlicht_vector_at(&ctxt, &v, 1, double));
  TEST_EQ_DOUBLE(3.0, *schemerlicht_vector_at(&ctxt, &v, 2, double));
  TEST_EQ_DOUBLE(4.0, *schemerlicht_vector_at(&ctxt, &v, 3, double));
  TEST_EQ_DOUBLE(5.0, *schemerlicht_vector_at(&ctxt, &v, 4, double));
  schemerlicht_vector_destroy(&ctxt, &v);
  }

void test_pop_back()
  {
  schemerlicht_context ctxt;
  schemerlicht_vector v;
  schemerlicht_vector_init_with_size(&ctxt, &v, 5, double);
  TEST_EQ_INT(5, v.vector_size);
  TEST_EQ_INT(5, v.vector_capacity);
  schemerlicht_vector_pop_back(&v);
  TEST_EQ_INT(4, v.vector_size);
  TEST_EQ_INT(5, v.vector_capacity);
  schemerlicht_vector_pop_back(&v);
  TEST_EQ_INT(3, v.vector_size);
  TEST_EQ_INT(5, v.vector_capacity);
  schemerlicht_vector_pop_back(&v);
  TEST_EQ_INT(2, v.vector_size);
  TEST_EQ_INT(5, v.vector_capacity);
  schemerlicht_vector_pop_back(&v);
  TEST_EQ_INT(1, v.vector_size);
  TEST_EQ_INT(5, v.vector_capacity);
  schemerlicht_vector_pop_back(&v);
  TEST_EQ_INT(0, v.vector_size);
  TEST_EQ_INT(5, v.vector_capacity);
  schemerlicht_vector_pop_back(&v);
  TEST_EQ_INT(0, v.vector_size);
  TEST_EQ_INT(5, v.vector_capacity);
  schemerlicht_vector_destroy(&ctxt, &v);
  }

void test_push_back()
  {
  schemerlicht_context ctxt;
  schemerlicht_vector v;
  schemerlicht_vector_init(&ctxt, &v, double);
  TEST_EQ_INT(0, v.vector_size);
  TEST_EQ_INT(8, v.vector_capacity);
  schemerlicht_vector_push_back(&ctxt, &v, 7.0, double);
  TEST_EQ_INT(1, v.vector_size);
  TEST_EQ_INT(8, v.vector_capacity);
  schemerlicht_vector_push_back(&ctxt, &v, 9.0, double);
  TEST_EQ_INT(2, v.vector_size);
  TEST_EQ_INT(8, v.vector_capacity);
  TEST_EQ_DOUBLE(7.0, *schemerlicht_vector_at(&ctxt, &v, 0, double));
  TEST_EQ_DOUBLE(9.0, *schemerlicht_vector_at(&ctxt, &v, 1, double));
  schemerlicht_vector_destroy(&ctxt, &v);
  }

void test_push_back_2()
  {
  schemerlicht_context ctxt;
  schemerlicht_vector v;
  schemerlicht_vector_init_with_size(&ctxt, &v, 0, double);
  TEST_EQ_INT(0, v.vector_size);
  TEST_EQ_INT(0, v.vector_capacity);
  schemerlicht_vector_push_back(&ctxt, &v, 7.0, double);
  TEST_EQ_INT(1, v.vector_size);
  TEST_EQ_INT(4, v.vector_capacity);
  schemerlicht_vector_push_back(&ctxt, &v, 9.0, double);
  TEST_EQ_INT(2, v.vector_size);
  TEST_EQ_INT(4, v.vector_capacity);
  TEST_EQ_DOUBLE(7.0, *schemerlicht_vector_at(&ctxt, &v, 0, double));
  TEST_EQ_DOUBLE(9.0, *schemerlicht_vector_at(&ctxt, &v, 1, double));
  schemerlicht_vector_push_back(&ctxt, &v, 11.0, double);
  TEST_EQ_INT(3, v.vector_size);
  TEST_EQ_INT(4, v.vector_capacity);
  TEST_EQ_DOUBLE(7.0, *schemerlicht_vector_at(&ctxt, &v, 0, double));
  TEST_EQ_DOUBLE(9.0, *schemerlicht_vector_at(&ctxt, &v, 1, double));
  TEST_EQ_DOUBLE(11.0, *schemerlicht_vector_at(&ctxt, &v, 2, double));
  schemerlicht_vector_push_back(&ctxt, &v, 13.0, double);
  TEST_EQ_INT(4, v.vector_size);
  TEST_EQ_INT(4, v.vector_capacity);
  TEST_EQ_DOUBLE(7.0, *schemerlicht_vector_at(&ctxt, &v, 0, double));
  TEST_EQ_DOUBLE(9.0, *schemerlicht_vector_at(&ctxt, &v, 1, double));
  TEST_EQ_DOUBLE(11.0, *schemerlicht_vector_at(&ctxt, &v, 2, double));
  TEST_EQ_DOUBLE(13.0, *schemerlicht_vector_at(&ctxt, &v, 3, double));
  schemerlicht_vector_push_back(&ctxt, &v, 15.0, double);
  TEST_EQ_INT(5, v.vector_size);
  TEST_EQ_INT(8, v.vector_capacity);
  TEST_EQ_DOUBLE(7.0, *schemerlicht_vector_at(&ctxt, &v, 0, double));
  TEST_EQ_DOUBLE(9.0, *schemerlicht_vector_at(&ctxt, &v, 1, double));
  TEST_EQ_DOUBLE(11.0, *schemerlicht_vector_at(&ctxt, &v, 2, double));
  TEST_EQ_DOUBLE(13.0, *schemerlicht_vector_at(&ctxt, &v, 3, double));
  TEST_EQ_DOUBLE(15.0, *schemerlicht_vector_at(&ctxt, &v, 4, double));
  schemerlicht_vector_destroy(&ctxt, &v);
  }

void test_iterator()
  {
  schemerlicht_context ctxt;
  schemerlicht_vector v;
  schemerlicht_vector_init(&ctxt, &v, double);
  TEST_EQ_INT(0, v.vector_size);
  TEST_EQ_INT(8, v.vector_capacity);
  schemerlicht_vector_push_back(&ctxt, &v, 7.0, double);
  TEST_EQ_INT(1, v.vector_size);
  TEST_EQ_INT(8, v.vector_capacity);
  schemerlicht_vector_push_back(&ctxt, &v, 9.0, double);
  TEST_EQ_INT(2, v.vector_size);
  TEST_EQ_INT(8, v.vector_capacity);
  double* it = schemerlicht_vector_begin(&v, double);
  double* it_end = schemerlicht_vector_end(&v, double);
  int counter = 0;
  for (; it != it_end; ++it, ++counter)
    {
    if (counter == 0)
      TEST_EQ_DOUBLE(7.0, *it);
    if (counter == 1)
      TEST_EQ_DOUBLE(9.0, *it);
    }
  TEST_EQ_INT(2, counter);
  schemerlicht_vector_destroy(&ctxt, &v);
  }

void test_iterator_2()
  {
  schemerlicht_context ctxt;
  schemerlicht_vector v;
  schemerlicht_vector_init(&ctxt, &v, double);
  TEST_EQ_INT(0, v.vector_size);
  TEST_EQ_INT(8, v.vector_capacity);
  schemerlicht_vector_push_back(&ctxt, &v, 7.0, double);
  TEST_EQ_INT(1, v.vector_size);
  TEST_EQ_INT(8, v.vector_capacity);
  schemerlicht_vector_push_back(&ctxt, &v, 9.0, double);
  TEST_EQ_INT(2, v.vector_size);
  TEST_EQ_INT(8, v.vector_capacity);
  double* it = schemerlicht_vector_begin(&v, double);
  double* it_end = schemerlicht_vector_end(&v, double);
  int counter = 0;
  for (; it != it_end; ++it, ++counter)
    {
    *it = (double)counter;
    }
  TEST_EQ_INT(2, counter);
  TEST_EQ_DOUBLE(0.0, *schemerlicht_vector_at(&ctxt, &v, 0, double));
  TEST_EQ_DOUBLE(1.0, *schemerlicht_vector_at(&ctxt, &v, 1, double));
  schemerlicht_vector_destroy(&ctxt, &v);
  }

void run_all_vector_tests()
  {
  test_construction_destruction_1();
  test_construction_destruction_2();
  test_access();
  test_pop_back();
  test_push_back();
  test_push_back_2();
  test_iterator();
  test_iterator_2();
  }