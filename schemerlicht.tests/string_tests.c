#include "string_tests.h"
#include "test_assert.h"

#include "schemerlicht/schemerlicht.h"
#include "schemerlicht/string.h"
#include "schemerlicht/context.h"

static void test_string_init()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "Hello world!");
  TEST_EQ_INT(12, s.string_length);
  TEST_EQ_INT(13, s.string_capacity);
  schemerlicht_string_destroy(ctxt, &s);
  schemerlicht_close(ctxt);
  }

static void test_string_functions()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "Hello world!");
  
  TEST_EQ_INT('H', *schemerlicht_string_begin(&s));
  TEST_EQ_INT('!', *schemerlicht_string_back(&s));
  TEST_EQ_INT(0, *schemerlicht_string_end(&s));
  TEST_EQ_INT('o', *schemerlicht_string_at(&s, 4));

  *schemerlicht_string_back(&s) = '?';
  TEST_EQ_INT('?', *schemerlicht_string_back(&s));
  schemerlicht_string_destroy(ctxt, &s);
  schemerlicht_close(ctxt);
  }

static void test_string_push_back()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "Hello world!");
  schemerlicht_string_push_back(ctxt, &s, '!');
  schemerlicht_string_push_back(ctxt, &s, '!');
  TEST_EQ_INT(14, s.string_length);
  TEST_EQ_INT(26, s.string_capacity);
  schemerlicht_string_pop_back(&s);
  TEST_EQ_INT(13, s.string_length);
  TEST_EQ_INT(26, s.string_capacity);
  schemerlicht_string_destroy(ctxt, &s);
  schemerlicht_close(ctxt);
  }

static void test_string_append()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_string s1;
  schemerlicht_string_init(ctxt, &s1, "Hello ");
  TEST_EQ_INT(6, s1.string_length);
  schemerlicht_string s2;
  schemerlicht_string_init(ctxt, &s2, "world!");
  schemerlicht_string_append(ctxt, &s1, &s2);
  TEST_EQ_INT(12, s1.string_length);
  TEST_EQ_INT('H', *schemerlicht_string_begin(&s1));
  TEST_EQ_INT('!', *schemerlicht_string_back(&s1));
  TEST_EQ_INT(0, *schemerlicht_string_end(&s1));
  TEST_EQ_INT('o', *schemerlicht_string_at(&s1, 4));
  schemerlicht_string_destroy(ctxt, &s1);
  schemerlicht_string_destroy(ctxt, &s2);
  schemerlicht_close(ctxt);
  }

static void test_string_append_2()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_string s1;
  schemerlicht_string_init(ctxt, &s1, "Hello ");
  TEST_EQ_INT(6, s1.string_length);
  schemerlicht_string_append_cstr(ctxt, &s1, "world!");
  TEST_EQ_INT(12, s1.string_length);
  TEST_EQ_INT('H', *schemerlicht_string_begin(&s1));
  TEST_EQ_INT('!', *schemerlicht_string_back(&s1));
  TEST_EQ_INT(0, *schemerlicht_string_end(&s1));
  TEST_EQ_INT('o', *schemerlicht_string_at(&s1, 4));
  schemerlicht_string_destroy(ctxt, &s1);
  schemerlicht_close(ctxt);
  }

static void test_string_compare_less_aux(const char* left, const char* right, int expected)
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_string s1, s2;
  schemerlicht_string_init(ctxt, &s1, left);
  schemerlicht_string_init(ctxt, &s2, right);
  int compare_result = schemerlicht_string_compare_less(&s1, &s2);
  TEST_EQ_INT(expected, compare_result);
  schemerlicht_string_destroy(ctxt, &s1);
  schemerlicht_string_destroy(ctxt, &s2);
  schemerlicht_close(ctxt);
  }

static void test_string_compare()
  {
  test_string_compare_less_aux("a", "b", 1);
  test_string_compare_less_aux("b", "a", 0);
  test_string_compare_less_aux("a", "a", 0);
  test_string_compare_less_aux("brabbelauto", "brabbelbox", 1);
  test_string_compare_less_aux("brabbelbox", "brabbelauto", 0);
  test_string_compare_less_aux("brabbelauto", "brabbelauto", 0);
  }

void run_all_string_tests()
  {
  test_string_init();
  test_string_functions();
  test_string_push_back();
  test_string_append();
  test_string_append_2();
  test_string_compare();
  }