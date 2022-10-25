#include "test_assert.h"
#include "compiler_tests.h"
#include "conv_tests.h"
#include "copy_tests.h"
#include "dump_tests.h"
#include "environment_tests.h"
#include "map_tests.h"
#include "parser_tests.h"
#include "reader_tests.h"
#include "stream_tests.h"
#include "string_tests.h"
#include "stringvec_tests.h"
#include "token_tests.h"
#include "vector_tests.h"

#include <time.h>

int main(int argc, const char* argv[])
  {
  (void)argc;
  (void)argv;
  InitTestEngine();
  clock_t start_t, end_t;
  start_t = clock();
  //run_all_compiler_tests();
  run_all_conv_tests();
  run_all_copy_tests();
  run_all_dump_tests();
  run_all_environment_tests();
  run_all_map_tests();
  run_all_parser_tests();
  run_all_reader_tests();
  run_all_stream_tests();
  run_all_string_tests();
  run_all_stringvec_tests();
  run_all_token_tests();
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
