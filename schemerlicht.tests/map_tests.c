#include "map_tests.h"
#include "test_assert.h"

#include "schemerlicht/context.h"
#include "schemerlicht/map.h"

static void test_map_construction()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_map* m = schemerlicht_map_new(ctxt, 10, 4);

  TEST_EQ_INT(10, m->array_size);
  TEST_EQ_INT(4, m->log_node_size);

  schemerlicht_map_free(ctxt, m);
  schemerlicht_close(ctxt);
  }

static void test_map()
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

static void test_map_2()
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

static void test_map_3()
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

static void test_map_4()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_map* m = schemerlicht_map_new(ctxt, 10, 1);

  schemerlicht_object* obj20 = schemerlicht_map_insert_indexed(ctxt, m, 20);
  TEST_EQ_INT(schemerlicht_object_type_nil, obj20->type);
  TEST_EQ_INT(10, m->array_size);
  TEST_EQ_INT(1, m->log_node_size);
  obj20->type = schemerlicht_object_type_fixnum;
  obj20->value.fx = 20;
  
  schemerlicht_object* obj21 = schemerlicht_map_insert_indexed(ctxt, m, 21);
  TEST_EQ_INT(schemerlicht_object_type_nil, obj21->type);
  TEST_EQ_INT(0, m->array_size);
  TEST_EQ_INT(2, m->log_node_size);
  obj21->type = schemerlicht_object_type_fixnum;
  obj21->value.fx = 21;

  schemerlicht_object* obj22 = schemerlicht_map_insert_indexed(ctxt, m, 22);
  TEST_EQ_INT(schemerlicht_object_type_nil, obj22->type);
  TEST_EQ_INT(0, m->array_size);
  TEST_EQ_INT(2, m->log_node_size);
  obj22->type = schemerlicht_object_type_fixnum;
  obj22->value.fx = 22;

  schemerlicht_object* obj23 = schemerlicht_map_insert_indexed(ctxt, m, 23);
  TEST_EQ_INT(schemerlicht_object_type_nil, obj23->type);
  TEST_EQ_INT(0, m->array_size);
  TEST_EQ_INT(3, m->log_node_size);
  obj23->type = schemerlicht_object_type_fixnum;
  obj23->value.fx = 23;

  TEST_EQ_INT(schemerlicht_object_type_fixnum, schemerlicht_map_get_indexed(m, 20)->type);
  TEST_EQ_INT(schemerlicht_object_type_fixnum, schemerlicht_map_get_indexed(m, 21)->type);
  TEST_EQ_INT(schemerlicht_object_type_fixnum, schemerlicht_map_get_indexed(m, 22)->type);
  TEST_EQ_INT(schemerlicht_object_type_fixnum, schemerlicht_map_get_indexed(m, 23)->type);
  TEST_EQ_INT(20, schemerlicht_map_get_indexed(m, 20)->value.fx);
  TEST_EQ_INT(21, schemerlicht_map_get_indexed(m, 21)->value.fx);
  TEST_EQ_INT(22, schemerlicht_map_get_indexed(m, 22)->value.fx);
  TEST_EQ_INT(23, schemerlicht_map_get_indexed(m, 23)->value.fx);
  TEST_EQ_INT(0, m->array_size);
  TEST_EQ_INT(3, m->log_node_size);

  schemerlicht_map_free(ctxt, m);
  schemerlicht_close(ctxt);
  }

static void test_map_with_string_keys()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_map* m = schemerlicht_map_new(ctxt, 0, 1);

  schemerlicht_object* obj_alpha = schemerlicht_map_insert_string(ctxt, m, "alpha");
  obj_alpha->type = schemerlicht_object_type_fixnum;
  obj_alpha->value.fx = 2000;

  schemerlicht_object* obj_beta = schemerlicht_map_insert_string(ctxt, m, "beta");
  obj_beta->type = schemerlicht_object_type_fixnum;
  obj_beta->value.fx = 2001;

  schemerlicht_object* obj_gamma = schemerlicht_map_insert_string(ctxt, m, "gamma");
  obj_gamma->type = schemerlicht_object_type_fixnum;
  obj_gamma->value.fx = 2002;

  schemerlicht_object* obj_delta = schemerlicht_map_insert_string(ctxt, m, "delta");
  obj_delta->type = schemerlicht_object_type_fixnum;
  obj_delta->value.fx = 2003;

  schemerlicht_object* obj_find_alpha = schemerlicht_map_get_string(m, "alpha");
  TEST_EQ_INT(schemerlicht_object_type_fixnum, obj_find_alpha->type);
  TEST_EQ_INT(2000, obj_find_alpha->value.fx);

  schemerlicht_object* obj_find_beta = schemerlicht_map_get_string(m, "beta");
  TEST_EQ_INT(schemerlicht_object_type_fixnum, obj_find_beta->type);
  TEST_EQ_INT(2001, obj_find_beta->value.fx);

  schemerlicht_object* obj_find_gamma = schemerlicht_map_get_string(m, "gamma");
  TEST_EQ_INT(schemerlicht_object_type_fixnum, obj_find_gamma->type);
  TEST_EQ_INT(2002, obj_find_gamma->value.fx);

  schemerlicht_object* obj_find_delta = schemerlicht_map_get_string(m, "delta");
  TEST_EQ_INT(schemerlicht_object_type_fixnum, obj_find_delta->type);
  TEST_EQ_INT(2003, obj_find_delta->value.fx);

  schemerlicht_map_free(ctxt, m);
  schemerlicht_close(ctxt);
  }

void run_all_map_tests()
  {
  test_map_construction();
  test_map();
  test_map_2();
  test_map_3();
  test_map_4();
  test_map_with_string_keys();
  }