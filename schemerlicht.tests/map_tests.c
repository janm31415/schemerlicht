#include "map_tests.h"
#include "test_assert.h"

#include "schemerlicht/scontext.h"
#include "schemerlicht/smap.h"

void test_map_construction()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_map* m = schemerlicht_map_new(ctxt, 10, 4);

  TEST_EQ_INT(10, m->array_size);
  TEST_EQ_INT(4, m->log_node_size);

  schemerlicht_map_free(ctxt, m);
  schemerlicht_close(ctxt);
  }

void test_map_indexed()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_map* m = schemerlicht_map_new(ctxt, 10, 4);

  schemerlicht_object* obj = schemerlicht_map_get_indexed(m, 0);
  TEST_EQ_INT(schemerlicht_object_type_nil, obj->type);

  obj = schemerlicht_map_insert_indexed(ctxt, m, 0);
  TEST_EQ_INT(schemerlicht_object_type_nil, obj->type);
  TEST_EQ_INT(10, m->array_size);
  TEST_EQ_INT(4, m->log_node_size);
  obj->type = schemerlicht_object_type_flonum;
  obj->value.fl = 3.14;

  obj = schemerlicht_map_get_indexed(m, 0);
  TEST_EQ_INT(schemerlicht_object_type_flonum, obj->type);
  TEST_EQ_DOUBLE(3.14, obj->value.fl);

  schemerlicht_map_free(ctxt, m);
  schemerlicht_close(ctxt);
  }

void test_map_indexed_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_map* m = schemerlicht_map_new(ctxt, 10, 4);

  schemerlicht_object* obj = schemerlicht_map_get_indexed(m, 12);
  TEST_EQ_INT(0, obj);

  obj = schemerlicht_map_insert_indexed(ctxt, m, 12);
  TEST_EQ_INT(schemerlicht_object_type_nil, obj->type);
  TEST_EQ_INT(10, m->array_size);
  TEST_EQ_INT(4, m->log_node_size);
  obj->type = schemerlicht_object_type_flonum;
  obj->value.fl = 3.14;

  obj = schemerlicht_map_get_indexed(m, 12);
  TEST_EQ_INT(schemerlicht_object_type_flonum, obj->type);
  TEST_EQ_DOUBLE(3.14, obj->value.fl);

  schemerlicht_map_free(ctxt, m);
  schemerlicht_close(ctxt);
  }

void test_map_indexed_3()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_map* m = schemerlicht_map_new(ctxt, 10, 4);

  schemerlicht_object* obj = schemerlicht_map_get_indexed(m, 200000);
  TEST_EQ_INT(0, obj);

  obj = schemerlicht_map_insert_indexed(ctxt, m, 200000);
  TEST_EQ_INT(schemerlicht_object_type_nil, obj->type);
  TEST_EQ_INT(10, m->array_size);
  TEST_EQ_INT(4, m->log_node_size);
  obj->type = schemerlicht_object_type_flonum;
  obj->value.fl = 3.14;

  obj = schemerlicht_map_get_indexed(m, 200000);
  TEST_EQ_INT(schemerlicht_object_type_flonum, obj->type);
  TEST_EQ_DOUBLE(3.14, obj->value.fl);

  schemerlicht_map_free(ctxt, m);
  schemerlicht_close(ctxt);
  }

void run_all_map_tests()
  {
  test_map_construction();
  test_map_indexed();
  test_map_indexed_2();
  test_map_indexed_3();
  }