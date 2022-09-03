#include "test_assert.h"
#include "string_tests.h"
#include "vector_tests.h"

#include <time.h>

int main(int argc, const char* argv[])
  {
  (void)argc;
  (void)argv;
  InitTestEngine();
  clock_t start_t, end_t;
  start_t = clock();
  run_all_string_tests();
  run_all_vector_tests();
  end_t = clock();
  double total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
  if (!testing_fails)
    {
    TEST_OUTPUT_LINE("Succes: %d tests passed.", testing_success);
    }
  else
    {
    TEST_OUTPUT_LINE("FAILURE: %d out of %d tests failed (%d failures).", testing_fails, testing_success + testing_fails, testing_fails);
    }
  TEST_OUTPUT_LINE("Test time: %f seconds.", total_t);
  return CloseTestEngine(1);
  }
