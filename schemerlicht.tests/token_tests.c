#include "token_tests.h"

#include "schemerlicht/stoken.h"
#include "schemerlicht/scontext.h"
#include "schemerlicht/sstream.h"

#include "test_assert.h"

#include <string.h>

static void test_number_recognition()
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

schemerlicht_vector script2tokens(schemerlicht_context* ctxt, const char* script)
  {
  schemerlicht_stream str;
  schemerlicht_stream_init(ctxt, &str, 10);
  schemerlicht_memsize len = cast(schemerlicht_memsize, strlen(script));
  schemerlicht_stream_write(ctxt, &str, script, len, 0);
  schemerlicht_stream_rewind(&str);
  schemerlicht_vector tokens = tokenize(ctxt, &str);
  schemerlicht_stream_close(ctxt, &str);
  return tokens;
  }

static void tokenize_symbol()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "#f");
  TEST_EQ_INT(1, tokens.vector_size);
  token* first_token = schemerlicht_vector_at(ctxt, &tokens, 0, token);
  TEST_EQ_INT(SCHEMERLICHT_T_SYMBOL, first_token->type);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_close(ctxt);
  }

static void tokenize_list()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(list a b)");
  TEST_EQ_INT(5, tokens.vector_size);  
  TEST_EQ_INT(SCHEMERLICHT_T_LEFT_ROUND_BRACKET, schemerlicht_vector_at(ctxt, &tokens, 0, token)->type);
  TEST_EQ_INT(SCHEMERLICHT_T_ID, schemerlicht_vector_at(ctxt, &tokens, 1, token)->type);
  TEST_EQ_INT(SCHEMERLICHT_T_ID, schemerlicht_vector_at(ctxt, &tokens, 2, token)->type);
  TEST_EQ_INT(SCHEMERLICHT_T_ID, schemerlicht_vector_at(ctxt, &tokens, 3, token)->type);
  TEST_EQ_INT(SCHEMERLICHT_T_RIGHT_ROUND_BRACKET, schemerlicht_vector_at(ctxt, &tokens, 4, token)->type);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_close(ctxt);
  }

static void tokenize_string()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(print\"spaghetti () \")");
  TEST_EQ_INT(4, tokens.vector_size);
  TEST_EQ_INT(SCHEMERLICHT_T_LEFT_ROUND_BRACKET, schemerlicht_vector_at(ctxt, &tokens, 0, token)->type);
  TEST_EQ_INT(SCHEMERLICHT_T_ID, schemerlicht_vector_at(ctxt, &tokens, 1, token)->type);
  TEST_EQ_INT(SCHEMERLICHT_T_STRING, schemerlicht_vector_at(ctxt, &tokens, 2, token)->type);  
  schemerlicht_string s = schemerlicht_vector_at(ctxt, &tokens, 2, token)->info.value;
  TEST_EQ_INT(15, s.string_length);
  TEST_EQ_INT(SCHEMERLICHT_T_RIGHT_ROUND_BRACKET, schemerlicht_vector_at(ctxt, &tokens, 3, token)->type);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_close(ctxt);
  }

static void tokenize_fixnum_real()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(+ 1 2.0)");
  TEST_EQ_INT(5, tokens.vector_size);
  TEST_EQ_INT(SCHEMERLICHT_T_LEFT_ROUND_BRACKET, schemerlicht_vector_at(ctxt, &tokens, 0, token)->type);
  TEST_EQ_INT(SCHEMERLICHT_T_ID, schemerlicht_vector_at(ctxt, &tokens, 1, token)->type);
  TEST_EQ_INT(SCHEMERLICHT_T_FIXNUM, schemerlicht_vector_at(ctxt, &tokens, 2, token)->type);
  TEST_EQ_INT(SCHEMERLICHT_T_FLONUM, schemerlicht_vector_at(ctxt, &tokens, 3, token)->type);
  TEST_EQ_INT(SCHEMERLICHT_T_RIGHT_ROUND_BRACKET, schemerlicht_vector_at(ctxt, &tokens, 4, token)->type);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_close(ctxt);
  }

static void tokenize_string_w_escape_chars()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(print\"line1\\nline2\")");
  TEST_EQ_INT(4, tokens.vector_size);
  TEST_EQ_INT(SCHEMERLICHT_T_LEFT_ROUND_BRACKET, schemerlicht_vector_at(ctxt, &tokens, 0, token)->type);
  TEST_EQ_INT(SCHEMERLICHT_T_ID, schemerlicht_vector_at(ctxt, &tokens, 1, token)->type);
  TEST_EQ_INT(SCHEMERLICHT_T_STRING, schemerlicht_vector_at(ctxt, &tokens, 2, token)->type);
  schemerlicht_string s = schemerlicht_vector_at(ctxt, &tokens, 2, token)->info.value;
  TEST_EQ_INT(13, s.string_length);
  TEST_EQ_INT(SCHEMERLICHT_T_RIGHT_ROUND_BRACKET, schemerlicht_vector_at(ctxt, &tokens, 3, token)->type);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_close(ctxt);
  }

static void tokenize_string_w_escape_chars_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector tokens = script2tokens(ctxt, "(print\"oh\\a\\b\\n\\t\\r\")");
  TEST_EQ_INT(4, tokens.vector_size);
  TEST_EQ_INT(SCHEMERLICHT_T_LEFT_ROUND_BRACKET, schemerlicht_vector_at(ctxt, &tokens, 0, token)->type);
  TEST_EQ_INT(SCHEMERLICHT_T_ID, schemerlicht_vector_at(ctxt, &tokens, 1, token)->type);
  TEST_EQ_INT(SCHEMERLICHT_T_STRING, schemerlicht_vector_at(ctxt, &tokens, 2, token)->type);
  schemerlicht_string s = schemerlicht_vector_at(ctxt, &tokens, 2, token)->info.value;
  TEST_EQ_INT(9, s.string_length);
  TEST_EQ_INT(SCHEMERLICHT_T_RIGHT_ROUND_BRACKET, schemerlicht_vector_at(ctxt, &tokens, 3, token)->type);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_close(ctxt);
  }

void run_all_token_tests()
  {
  test_number_recognition();
  tokenize_symbol();
  tokenize_list();
  tokenize_string();
  tokenize_fixnum_real();
  tokenize_string_w_escape_chars();
  tokenize_string_w_escape_chars_2();
  }