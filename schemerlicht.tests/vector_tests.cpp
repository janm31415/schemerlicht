#include "vector_tests.h"
#include "test_assert.h"

extern "C"
  {
#include "schemerlicht/schemerlicht.h"
#include "schemerlicht/svector.h"
#include "schemerlicht/scontext.h"
  }

namespace
  {
  void test_construction_destruction_1()
    {
    schemerlicht_context ctxt;
    schemerlicht_vector v;
    schemerlicht_vector_init_with_size(&ctxt, &v, 5, double)
    TEST_EQ(5, v.vector_size);
    TEST_EQ(5, v.vector_capacity);
    TEST_EQ(sizeof(double), v.element_size);
    schemerlicht_vector_destroy(&ctxt, &v);
    }

  void test_construction_destruction_2()
    {
    schemerlicht_context ctxt;
    schemerlicht_vector v;
    schemerlicht_vector_init(&ctxt, &v, double)
    TEST_EQ(0, v.vector_size);
    TEST_EQ(8, v.vector_capacity);
    TEST_EQ(sizeof(double), v.element_size);
    schemerlicht_vector_destroy(&ctxt, &v);
    }

  void test_access()
    {
    schemerlicht_context ctxt;
    schemerlicht_vector v;
    schemerlicht_vector_init_with_size(&ctxt, &v, 5, double)
    *schemerlicht_vector_at(&ctxt, &v, 0, double) = 1.0;
    *schemerlicht_vector_at(&ctxt, &v, 1, double) = 2.0;
    *schemerlicht_vector_at(&ctxt, &v, 2, double) = 3.0;
    *schemerlicht_vector_at(&ctxt, &v, 3, double) = 4.0;
    *schemerlicht_vector_at(&ctxt, &v, 4, double) = 5.0;
    TEST_EQ(1.0, *schemerlicht_vector_at(&ctxt, &v, 0, double));
    TEST_EQ(2.0, *schemerlicht_vector_at(&ctxt, &v, 1, double));
    TEST_EQ(3.0, *schemerlicht_vector_at(&ctxt, &v, 2, double));
    TEST_EQ(4.0, *schemerlicht_vector_at(&ctxt, &v, 3, double));
    TEST_EQ(5.0, *schemerlicht_vector_at(&ctxt, &v, 4, double));
    schemerlicht_vector_destroy(&ctxt, &v);
    }

  void test_popback()
    {
    schemerlicht_context ctxt;
    schemerlicht_vector v;
    schemerlicht_vector_init_with_size(&ctxt, &v, 5, double)
    TEST_EQ(5, v.vector_size);
    TEST_EQ(5, v.vector_capacity);
    schemerlicht_vector_popback(&v);
    TEST_EQ(4, v.vector_size);
    TEST_EQ(5, v.vector_capacity);
    schemerlicht_vector_popback(&v);
    TEST_EQ(3, v.vector_size);
    TEST_EQ(5, v.vector_capacity);
    schemerlicht_vector_popback(&v);
    TEST_EQ(2, v.vector_size);
    TEST_EQ(5, v.vector_capacity);
    schemerlicht_vector_popback(&v);
    TEST_EQ(1, v.vector_size);
    TEST_EQ(5, v.vector_capacity);
    schemerlicht_vector_popback(&v);
    TEST_EQ(0, v.vector_size);
    TEST_EQ(5, v.vector_capacity);
    schemerlicht_vector_popback(&v);
    TEST_EQ(0, v.vector_size);
    TEST_EQ(5, v.vector_capacity);
    schemerlicht_vector_destroy(&ctxt, &v);
    }
  }

void run_all_vector_tests()
  {
  test_construction_destruction_1();
  test_construction_destruction_2();
  test_access();
  test_popback();
  }