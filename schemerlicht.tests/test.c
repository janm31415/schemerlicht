#include "test_assert.h"
#include "string_tests.h"
#include "vector_tests.h"

int main(int argc, const char* argv[])
  {
  (void)argc;
  (void)argv;
  InitTestEngine();
  run_all_string_tests();
  run_all_vector_tests();
  if (!testing_fails)
    {
    TEST_OUTPUT_LINE("Succes: %d tests passed.", testing_success);
    }
  else
    {
    TEST_OUTPUT_LINE("FAILURE: %d out of %d tests failed (%d failures).", testing_fails, testing_success + testing_fails, testing_fails);
    }
  return CloseTestEngine(1);
  }
