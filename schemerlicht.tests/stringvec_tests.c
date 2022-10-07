#include "stringvec_tests.h"
#include "test_assert.h"

#include "schemerlicht/schemerlicht.h"
#include "schemerlicht/stringvec.h"
#include "schemerlicht/context.h"


static void test_stringvec_sort()
  {
  schemerlicht_context* ctxt = schemerlicht_open();
  schemerlicht_vector v;
  schemerlicht_vector_init(ctxt, &v, schemerlicht_string);

  schemerlicht_string a,b,c,d,e;
  schemerlicht_string_init(ctxt, &a, "a");
  schemerlicht_string_init(ctxt, &b, "b");
  schemerlicht_string_init(ctxt, &c, "c");
  schemerlicht_string_init(ctxt, &d, "d");
  schemerlicht_string_init(ctxt, &e, "e");
  
  schemerlicht_vector_push_back(ctxt, &v, b, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v, a, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v, c, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v, e, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v, d, schemerlicht_string);

  schemerlicht_string_vector_sort(&v);

  TEST_EQ_STRING("a", schemerlicht_vector_at(&v, 0, schemerlicht_string)->string_ptr);
  TEST_EQ_STRING("b", schemerlicht_vector_at(&v, 1, schemerlicht_string)->string_ptr);
  TEST_EQ_STRING("c", schemerlicht_vector_at(&v, 2, schemerlicht_string)->string_ptr);
  TEST_EQ_STRING("d", schemerlicht_vector_at(&v, 3, schemerlicht_string)->string_ptr);
  TEST_EQ_STRING("e", schemerlicht_vector_at(&v, 4, schemerlicht_string)->string_ptr);

  schemerlicht_string_destroy(ctxt, &a);
  schemerlicht_string_destroy(ctxt, &b);
  schemerlicht_string_destroy(ctxt, &c);
  schemerlicht_string_destroy(ctxt, &d);
  schemerlicht_string_destroy(ctxt, &e);
  schemerlicht_vector_destroy(ctxt, &v);
  schemerlicht_close(ctxt);
  }

void run_all_stringvec_tests()
  {
  test_stringvec_sort();
  }