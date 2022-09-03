#include "string_tests.h"
#include "test_assert.h"

extern "C"
  {
#include "schemerlicht/schemerlicht.h"
#include "schemerlicht/sstring.h"
#include "schemerlicht/scontext.h"
  }

namespace
  {
  void test_string_init()
    {
    schemerlicht_context ctxt;
    schemerlicht_string s;
    schemerlicht_string_init(&ctxt, &s, "Hello world!");
    TEST_EQ(12, s.string_length);
    TEST_EQ(13, s.string_capacity);
    schemerlicht_string_destroy(&ctxt, &s);
    }
  }

void run_all_string_tests()
  {
  test_string_init();
  }