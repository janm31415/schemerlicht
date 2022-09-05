#include "token_tests.h"

#include "schemerlicht/stoken.h"
#include "schemerlicht/scontext.h"
#include "schemerlicht/sstream.h"

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

  int is_real;
  int is_scientific;
  TEST_EQ_INT(1, is_number(&is_real, &is_scientific, "1"));
  TEST_EQ_INT(0, is_real);
  TEST_EQ_INT(0, is_scientific);
  TEST_EQ_INT(1, is_number(&is_real, &is_scientific, "1.0"));
  TEST_EQ_INT(1, is_real);
  TEST_EQ_INT(0, is_scientific);
  TEST_EQ_INT(1, is_number(&is_real, &is_scientific, "1e+0"));
  TEST_EQ_INT(1, is_real);
  TEST_EQ_INT(1, is_scientific);
  }

void tokenize_symbol()
  {
  schemerlicht_context ctxt;
  schemerlicht_stream str;
  schemerlicht_stream_init(&ctxt, &str, 10);
  schemerlicht_stream_write(&ctxt, &str, "#f", 2, 0);
  schemerlicht_stream_rewind(&str);
  schemerlicht_vector tokens = tokenize(&ctxt, &str);
  TEST_EQ_INT(1, tokens.vector_size);
  token* first_token = schemerlicht_vector_at(&ctxt, &tokens, 0, token);
  TEST_EQ_INT(T_SYMBOL, first_token->type);
  schemerlicht_stream_close(&ctxt, &str);
  token* it = schemerlicht_vector_begin(&tokens, token);
  token* it_end = schemerlicht_vector_end(&tokens, token);
  for (; it != it_end; ++it)
    {
    if (it->type != T_FLONUM && it->type != T_FIXNUM)
      {
      schemerlicht_string_destroy(&ctxt, &it->info.value);
      }
    }
  schemerlicht_vector_destroy(&ctxt, &tokens);
  }

void run_all_token_tests()
  {
  test_number_recognition();
  tokenize_symbol();
  }