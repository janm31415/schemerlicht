#include "token_tests.h"

#include "schemerlicht/stoken.h"

#include "test_assert.h"

void test_number_recognition()
  {
  schemerlicht_flonum fl = to_flonum("1.2");
  TEST_EQ_DOUBLE(1.2, fl);
  fl = to_flonum("3.14159");
  TEST_EQ_DOUBLE(3.14159, fl);
  fl = to_flonum("-3.14159");
  TEST_EQ_DOUBLE(-3.14159, fl);
  fl = to_flonum("10");
  TEST_EQ_DOUBLE(10.0, fl);
  }

void run_all_token_tests()
  {
  test_number_recognition();
  }