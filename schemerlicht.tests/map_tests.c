#include "map_tests.h"
#include "test_assert.h"

#include "schemerlicht/scontext.h"
#include "schemerlicht/smap.h"

void test_map_construction()
  {
  schemerlicht_context ctxt;
  schemerlicht_map* m = schemerlicht_map_new(&ctxt, 10, 4);

  schemerlicht_map_free(&ctxt, m);
  }

void run_all_map_tests()
  {
  test_map_construction();
  }