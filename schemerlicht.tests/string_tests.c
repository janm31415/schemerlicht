#include "string_tests.h"
#include "test_assert.h"

#include "schemerlicht/schemerlicht.h"
#include "schemerlicht/sstring.h"
#include "schemerlicht/scontext.h"

void test_string_init()
  {
  schemerlicht_context ctxt;
  schemerlicht_string s;
  schemerlicht_string_init(&ctxt, &s, "Hello world!");
  TEST_EQ_INT(12, s.string_length);
  TEST_EQ_INT(13, s.string_capacity);
  schemerlicht_string_destroy(&ctxt, &s);
  }


void run_all_string_tests()
  {
  test_string_init();
  }