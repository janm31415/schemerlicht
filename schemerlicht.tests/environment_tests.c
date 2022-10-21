#include "environment_tests.h"
#include "test_assert.h"

#include "schemerlicht/context.h"
#include "schemerlicht/environment.h"
#include "schemerlicht/string.h"

static void test_environment_1()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_string name;
  schemerlicht_string_init(ctxt, &name, "myVariable");

  schemerlicht_environment_entry entry;
  entry.type = SCHEMERLICHT_ENV_TYPE_STACK;
  entry.position = 845;
  schemerlicht_environment_push(ctxt, &name, entry);

  schemerlicht_string lookup_name_1;
  schemerlicht_string_init(ctxt, &lookup_name_1, "myVariable");
  schemerlicht_string lookup_name_2;
  schemerlicht_string_init(ctxt, &lookup_name_2, "myVariable2");

  schemerlicht_environment_entry lookup_entry;
  TEST_EQ_INT(1, schemerlicht_environment_find(&lookup_entry, ctxt, &lookup_name_1));
  TEST_EQ_INT(entry.type, lookup_entry.type);
  TEST_EQ_INT(entry.position, lookup_entry.position);
  TEST_EQ_INT(0, schemerlicht_environment_find(&lookup_entry, ctxt, &lookup_name_2));

  schemerlicht_string_destroy(ctxt, &lookup_name_1);
  schemerlicht_string_destroy(ctxt, &lookup_name_2);
  schemerlicht_close(ctxt);
  }

void run_all_environment_tests()
  {
  test_environment_1();
  }