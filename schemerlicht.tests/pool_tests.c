#include "parser_tests.h"
#include "test_assert.h"

#include "schemerlicht/context.h"
#include "schemerlicht/pool.h"

static void test_pool_allocator_construction()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_pool_allocator pool;
  schemerlicht_pool_allocator_init(ctxt, &pool, 4, sizeof(double));
  TEST_EQ_INT(4, pool.chunks_per_block);
  TEST_EQ_INT(sizeof(double), pool.chunk_size);
  schemerlicht_pool_allocator_destroy(ctxt, &pool);
  schemerlicht_close(ctxt);
  }

static void test_pool_allocator_1()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_pool_allocator pool;
  schemerlicht_pool_allocator_init(ctxt, &pool, 4, sizeof(double));
  double* d0 = cast(double*, schemerlicht_pool_allocate(ctxt, &pool));
  *d0 = 0.0;
  double* d1 = cast(double*, schemerlicht_pool_allocate(ctxt, &pool));
  *d1 = 1.0;
  double* d2 = cast(double*, schemerlicht_pool_allocate(ctxt, &pool));
  *d2 = 2.0;
  double* d3 = cast(double*, schemerlicht_pool_allocate(ctxt, &pool));
  *d3 = 3.0;
  double* d4 = cast(double*, schemerlicht_pool_allocate(ctxt, &pool)); // here we create a new block
  *d4 = 4.0;
  TEST_EQ_INT(2, pool.block_addresses.vector_size);
  schemerlicht_pool_deallocate(&pool, d2);
  TEST_EQ_DOUBLE(0.0, *d0);
  TEST_EQ_DOUBLE(1.0, *d1);
  TEST_EQ_DOUBLE(3.0, *d3);
  TEST_EQ_DOUBLE(4.0, *d4);
  double* d5 = cast(double*, schemerlicht_pool_allocate(ctxt, &pool));
  *d5 = 5.0;
  TEST_EQ_DOUBLE(0.0, *d0);
  TEST_EQ_DOUBLE(1.0, *d1);
  TEST_EQ_DOUBLE(3.0, *d3);
  TEST_EQ_DOUBLE(4.0, *d4);
  TEST_EQ_DOUBLE(5.0, *d5);
  schemerlicht_pool_allocator_destroy(ctxt, &pool);
  schemerlicht_close(ctxt);
  }

typedef struct large_object
  {
  double d[4];
  } large_object;

static void test_pool_allocator_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_pool_allocator pool;
  schemerlicht_pool_allocator_init(ctxt, &pool, 4, sizeof(large_object));
  large_object* d0 = cast(large_object*, schemerlicht_pool_allocate(ctxt, &pool));
  for (int i = 0; i < 4; ++i)
    d0->d[i] = 0.0*i;
  large_object* d1 = cast(large_object*, schemerlicht_pool_allocate(ctxt, &pool));
  for (int i = 0; i < 4; ++i)
    d1->d[i] = 1.0 * i;
  large_object* d2 = cast(large_object*, schemerlicht_pool_allocate(ctxt, &pool));
  for (int i = 0; i < 4; ++i)
    d2->d[i] = 2.0 * i;
  large_object* d3 = cast(large_object*, schemerlicht_pool_allocate(ctxt, &pool));
  for (int i = 0; i < 4; ++i)
    d3->d[i] = 3.0 * i;
  large_object* d4 = cast(large_object*, schemerlicht_pool_allocate(ctxt, &pool)); // here we create a new block
  for (int i = 0; i < 4; ++i)
    d4->d[i] = 4.0 * i;
  TEST_EQ_INT(2, pool.block_addresses.vector_size);
  schemerlicht_pool_deallocate(&pool, d2);
  for (int i = 0; i < 4; ++i)
    TEST_EQ_DOUBLE(0.0*i, d0->d[i]);
  for (int i = 0; i < 4; ++i)
    TEST_EQ_DOUBLE(1.0 * i, d1->d[i]);
  for (int i = 0; i < 4; ++i)
    TEST_EQ_DOUBLE(3.0 * i, d3->d[i]);
  for (int i = 0; i < 4; ++i)
    TEST_EQ_DOUBLE(4.0 * i, d4->d[i]);
  large_object* d5 = cast(large_object*, schemerlicht_pool_allocate(ctxt, &pool));
  for (int i = 0; i < 4; ++i)
    d5->d[i] = 5.0 * i;
  for (int i = 0; i < 4; ++i)
    TEST_EQ_DOUBLE(0.0 * i, d0->d[i]);
  for (int i = 0; i < 4; ++i)
    TEST_EQ_DOUBLE(1.0 * i, d1->d[i]);
  for (int i = 0; i < 4; ++i)
    TEST_EQ_DOUBLE(3.0 * i, d3->d[i]);
  for (int i = 0; i < 4; ++i)
    TEST_EQ_DOUBLE(4.0 * i, d4->d[i]);
  for (int i = 0; i < 4; ++i)
    TEST_EQ_DOUBLE(5.0 * i, d5->d[i]);
  schemerlicht_pool_allocator_destroy(ctxt, &pool);
  schemerlicht_close(ctxt);
  }

void run_all_pool_tests()
  {
  test_pool_allocator_construction();
  test_pool_allocator_1();
  test_pool_allocator_2();
  }
