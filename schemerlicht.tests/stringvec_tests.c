#include "stringvec_tests.h"
#include "test_assert.h"

#include "schemerlicht/schemerlicht.h"
#include "schemerlicht/stringvec.h"
#include "schemerlicht/context.h"


static void test_stringvec_sort()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
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


static void test_stringvec_binary_search()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector v;
  schemerlicht_vector_init(ctxt, &v, schemerlicht_string);

  schemerlicht_string a, b, c, d, e, f;
  schemerlicht_string_init(ctxt, &a, "a");
  schemerlicht_string_init(ctxt, &b, "b");
  schemerlicht_string_init(ctxt, &c, "c");
  schemerlicht_string_init(ctxt, &d, "d");
  schemerlicht_string_init(ctxt, &e, "e");
  schemerlicht_string_init(ctxt, &f, "f");

  schemerlicht_vector_push_back(ctxt, &v, b, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v, a, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v, c, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v, e, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v, d, schemerlicht_string);

  schemerlicht_string_vector_sort(&v);

  int found = schemerlicht_string_vector_binary_search(&v, &f);
  TEST_EQ_INT(0, found);
  found = schemerlicht_string_vector_binary_search(&v, &a);
  TEST_EQ_INT(1, found);
  found = schemerlicht_string_vector_binary_search(&v, &b);
  TEST_EQ_INT(1, found);
  found = schemerlicht_string_vector_binary_search(&v, &c);
  TEST_EQ_INT(1, found);
  found = schemerlicht_string_vector_binary_search(&v, &d);
  TEST_EQ_INT(1, found);
  found = schemerlicht_string_vector_binary_search(&v, &e);
  TEST_EQ_INT(1, found);

  schemerlicht_string_destroy(ctxt, &a);
  schemerlicht_string_destroy(ctxt, &b);
  schemerlicht_string_destroy(ctxt, &c);
  schemerlicht_string_destroy(ctxt, &d);
  schemerlicht_string_destroy(ctxt, &e);
  schemerlicht_string_destroy(ctxt, &f);
  schemerlicht_vector_destroy(ctxt, &v);
  schemerlicht_close(ctxt);
  }

static void test_stringvec_insert_sorted()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector v;
  schemerlicht_vector_init(ctxt, &v, schemerlicht_string);

  schemerlicht_string a, b, c, d, e, f;
  schemerlicht_string_init(ctxt, &a, "a");
  schemerlicht_string_init(ctxt, &b, "b");
  schemerlicht_string_init(ctxt, &c, "c");
  schemerlicht_string_init(ctxt, &d, "d");
  schemerlicht_string_init(ctxt, &e, "e");
  schemerlicht_string_init(ctxt, &f, "crate");

  schemerlicht_vector_push_back(ctxt, &v, b, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v, a, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v, c, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v, e, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v, d, schemerlicht_string);

  schemerlicht_string_vector_sort(&v);

  schemerlicht_string_vector_insert_sorted(ctxt, &v, &f);
  TEST_EQ_STRING("a", schemerlicht_vector_at(&v, 0, schemerlicht_string)->string_ptr);
  TEST_EQ_STRING("b", schemerlicht_vector_at(&v, 1, schemerlicht_string)->string_ptr);
  TEST_EQ_STRING("c", schemerlicht_vector_at(&v, 2, schemerlicht_string)->string_ptr);
  TEST_EQ_STRING("crate", schemerlicht_vector_at(&v, 3, schemerlicht_string)->string_ptr);
  TEST_EQ_STRING("d", schemerlicht_vector_at(&v, 4, schemerlicht_string)->string_ptr);
  TEST_EQ_STRING("e", schemerlicht_vector_at(&v, 5, schemerlicht_string)->string_ptr);


  schemerlicht_string_destroy(ctxt, &a);
  schemerlicht_string_destroy(ctxt, &b);
  schemerlicht_string_destroy(ctxt, &c);
  schemerlicht_string_destroy(ctxt, &d);
  schemerlicht_string_destroy(ctxt, &e);
  schemerlicht_string_destroy(ctxt, &f);
  schemerlicht_vector_destroy(ctxt, &v);
  schemerlicht_close(ctxt);
  }

static void test_stringvec_unique()
  {
  schemerlicht_context* ctxt = schemerlicht_open(256);
  schemerlicht_vector v;
  schemerlicht_vector_init(ctxt, &v, schemerlicht_string);

  schemerlicht_string a, b, c, d, e, f, g;
  schemerlicht_string_init(ctxt, &a, "a");
  schemerlicht_string_init(ctxt, &b, "b");
  schemerlicht_string_init(ctxt, &c, "b");
  schemerlicht_string_init(ctxt, &d, "c");
  schemerlicht_string_init(ctxt, &e, "c");
  schemerlicht_string_init(ctxt, &f, "c");
  schemerlicht_string_init(ctxt, &g, "d");

  schemerlicht_vector_push_back(ctxt, &v, b, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v, a, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v, c, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v, e, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v, d, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v, g, schemerlicht_string);
  schemerlicht_vector_push_back(ctxt, &v, f, schemerlicht_string);

  schemerlicht_string_vector_sort(&v);
  TEST_EQ_INT(7, v.vector_size);

  schemerlicht_string* it = schemerlicht_string_vector_unique(&v);
  schemerlicht_string* it_end = schemerlicht_vector_end(&v, schemerlicht_string);
  schemerlicht_vector_erase_range(&v, &it, &it_end, schemerlicht_string);
  TEST_EQ_INT(4, v.vector_size);

  TEST_EQ_STRING("a", schemerlicht_vector_at(&v, 0, schemerlicht_string)->string_ptr);
  TEST_EQ_STRING("b", schemerlicht_vector_at(&v, 1, schemerlicht_string)->string_ptr);
  TEST_EQ_STRING("c", schemerlicht_vector_at(&v, 2, schemerlicht_string)->string_ptr);
  TEST_EQ_STRING("d", schemerlicht_vector_at(&v, 3, schemerlicht_string)->string_ptr);

  schemerlicht_string_destroy(ctxt, &a);
  schemerlicht_string_destroy(ctxt, &b);
  schemerlicht_string_destroy(ctxt, &c);
  schemerlicht_string_destroy(ctxt, &d);
  schemerlicht_string_destroy(ctxt, &e);
  schemerlicht_string_destroy(ctxt, &f);
  schemerlicht_string_destroy(ctxt, &g);
  schemerlicht_vector_destroy(ctxt, &v);
  schemerlicht_close(ctxt);
  }

void run_all_stringvec_tests()
  {
  test_stringvec_sort();
  test_stringvec_binary_search();
  test_stringvec_insert_sorted();
  test_stringvec_unique();
  }