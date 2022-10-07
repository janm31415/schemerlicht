#include "stringvec.h"
#include "context.h"

#include <string.h>

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

static schemerlicht_string* lower_bound(schemerlicht_string* first, schemerlicht_string* last, schemerlicht_string* value)
  {
  schemerlicht_string* it;
  schemerlicht_memsize count = cast(schemerlicht_memsize, last - first);
  schemerlicht_memsize step;
  while (count > 0)
    {
    it = first;
    step = count / 2;
    it += step;
    if (schemerlicht_string_compare_less(it, value))
      {
      first = ++it;
      count -= step+1;
      }
    else
      {
      count = step;
      }
    }
  return first;
  }

int schemerlicht_string_vector_binary_search(schemerlicht_vector* v, schemerlicht_string* s)
  {
  schemerlicht_string* it = schemerlicht_vector_begin(v, schemerlicht_string);
  schemerlicht_string* it_end = schemerlicht_vector_end(v, schemerlicht_string);
  it = lower_bound(it, it_end, s);
  return (!(it == it_end) && !(schemerlicht_string_compare_less(s, it))) ? 1 : 0;  
  }

void schemerlicht_string_vector_insert_sorted(schemerlicht_context* ctxt, schemerlicht_vector* v, schemerlicht_string* s)
  {
  schemerlicht_string* sit = schemerlicht_vector_begin(v, schemerlicht_string);
  schemerlicht_string* sit_end = schemerlicht_vector_end(v, schemerlicht_string);
  sit = lower_bound(sit, sit_end, s);
  schemerlicht_string* s_end = s+1;
  schemerlicht_vector_insert(ctxt, v, &sit, &s, &s_end, schemerlicht_string);
  }