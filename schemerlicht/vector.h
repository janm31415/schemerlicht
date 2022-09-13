#ifndef SCHEMERLICHT_VECTOR_H
#define SCHEMERLICHT_VECTOR_H

#include "schemerlicht.h"
#include "memory.h"

typedef struct schemerlicht_vector
  {
  void* vector_ptr;
  schemerlicht_memsize vector_capacity; // the memory capacity of the vector
  schemerlicht_memsize vector_size; // the number of elements in the vector
  schemerlicht_memsize element_size; // the size of the elements in the vector
  } schemerlicht_vector;

#define schemerlicht_vector_init_with_size(ctxt, vec, size, element_type) \
  (vec)->vector_ptr = schemerlicht_newvector(ctxt, size, element_type); \
  (vec)->vector_capacity = cast(schemerlicht_memsize, size); \
  (vec)->vector_size = cast(schemerlicht_memsize, size); \
  (vec)->element_size = sizeof(element_type)

#define schemerlicht_vector_init(ctxt, vec, element_type) \
  (vec)->vector_ptr = schemerlicht_newvector(ctxt, MINSIZEVECTOR, element_type); \
  (vec)->vector_capacity = cast(schemerlicht_memsize, MINSIZEVECTOR); \
  (vec)->vector_size = 0; \
  (vec)->element_size = sizeof(element_type)

#define schemerlicht_vector_init_reserve(ctxt, vec, size, element_type) \
  (vec)->vector_ptr = schemerlicht_newvector(ctxt, size, element_type); \
  (vec)->vector_capacity = cast(schemerlicht_memsize, size); \
  (vec)->vector_size = 0; \
  (vec)->element_size = sizeof(element_type)

#define schemerlicht_vector_destroy(ctxt, vec) \
  schemerlicht_realloc(ctxt, (vec)->vector_ptr, (vec)->vector_capacity*(vec)->element_size, 0)

#define schemerlicht_vector_at(vec, index, element_type) \
  (cast(element_type*, (vec)->vector_ptr) + (index))

#define schemerlicht_vector_pop_back(vec) \
  if ((vec)->vector_size > 0) --((vec)->vector_size)

#define schemerlicht_vector_pop_front(vec) \
  if ((vec)->vector_size > 0) { \
    --((vec)->vector_size); \
    memmove(cast(char*, (vec)->vector_ptr), cast(char*, (vec)->vector_ptr)+(vec)->element_size, (vec)->vector_size*(vec)->element_size); \
  }

#define schemerlicht_vector_push_back(ctxt, vec, element_value, element_type) \
  schemerlicht_growvector(ctxt, (vec)->vector_ptr, (vec)->vector_size+1, (vec)->vector_capacity, element_type); \
  *schemerlicht_vector_at(vec, (vec)->vector_size, element_type) = element_value; \
  ++((vec)->vector_size)

#define schemerlicht_vector_push_front(ctxt, vec, element_value, element_type) \
  schemerlicht_growvector(ctxt, (vec)->vector_ptr, (vec)->vector_size+1, (vec)->vector_capacity, element_type); \
  memmove(cast(element_type*, (vec)->vector_ptr)+1, cast(element_type*, (vec)->vector_ptr), (vec)->vector_size*sizeof(element_type)); \
  *schemerlicht_vector_at(vec, 0, element_type) = element_value; \
  ++((vec)->vector_size)

#define schemerlicht_vector_begin(vec, element_type) \
  cast(element_type*, (vec)->vector_ptr)

#define schemerlicht_vector_end(vec, element_type) \
  (cast(element_type*, (vec)->vector_ptr) + (vec)->vector_size)

#define schemerlicht_vector_back(vec, element_type) \
  (cast(element_type*, (vec)->vector_ptr) + ((vec)->vector_size - 1))

#define schemerlicht_vector_erase(ctxt, vec, iterator, element_type) \
  { element_type* it_end_obfuscate_asdfasdfds = (cast(element_type*, (vec)->vector_ptr) + (vec)->vector_size); \
  memmove(*(iterator), cast(element_type*, *(iterator))+1, (size_t)(it_end_obfuscate_asdfasdfds-cast(element_type*, *(iterator))-1)*sizeof(element_type)); }\
  --((vec)->vector_size)

#define schemerlicht_vector_insert(ctxt, vec, iterator, range_it_begin, range_it_end, element_type) \
  { schemerlicht_memsize range_size = cast(schemerlicht_memsize, cast(element_type*, *(range_it_end))-cast(element_type*, *(range_it_begin))); \
    schemerlicht_memsize mem_needed = (vec)->vector_size + range_size; \
    if ((vec)->vector_capacity < mem_needed) { \
      schemerlicht_memsize iterator_index = cast(schemerlicht_memsize, cast(element_type*, *(iterator)) - cast(element_type*, (vec)->vector_ptr)); \
      schemerlicht_reallocvector(ctxt, (vec)->vector_ptr, (vec)->vector_capacity, mem_needed, element_type); \
      *(iterator) = (cast(element_type*, (vec)->vector_ptr) + iterator_index); \
      (vec)->vector_capacity = mem_needed; } \
    element_type* it_end = (cast(element_type*, (vec)->vector_ptr) + (vec)->vector_size); \
    (vec)->vector_size += range_size; \
    memmove(cast(element_type*, *(iterator))+range_size, cast(element_type*, *(iterator)), (it_end - cast(element_type*, *(iterator)))*sizeof(element_type)); \
    memcpy(*(iterator), *(range_it_begin), range_size*sizeof(element_type)); }
  

#endif //SCHEMERLICHT_VECTOR_H