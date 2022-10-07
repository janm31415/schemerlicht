#include "stringvec.h"
#include "context.h"

static schemerlicht_string* partition(schemerlicht_string* it, schemerlicht_string* it_end)
  {
  schemerlicht_string* pivot = it_end-1;
  schemerlicht_string* i = it;
  for (schemerlicht_string* j = it; j != pivot; ++j)
    {
    if (schemerlicht_string_compare_less(j, pivot))
      {
      swap(*i,*j,schemerlicht_string);
      ++i;
      }
    }
  swap(*i, *pivot, schemerlicht_string);
  return i;
  }

static void quicksort(schemerlicht_string* it, schemerlicht_string* it_end)
  {
  schemerlicht_memsize distance = cast(schemerlicht_memsize, it_end-it);
  if (distance > 1)
    {
    schemerlicht_string* mid = partition(it, it_end);
    quicksort(it, mid);
    quicksort(mid+1, it_end);
    }
  }

void schemerlicht_string_vector_sort(schemerlicht_vector* v)
  {
  schemerlicht_string* it = schemerlicht_vector_begin(v, schemerlicht_string);
  schemerlicht_string* it_end = schemerlicht_vector_end(v, schemerlicht_string);
  quicksort(it, it_end);
  }