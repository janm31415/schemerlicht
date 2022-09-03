#ifndef SCHEMERLICHT_VECTOR_H
#define SCHEMERLICHT_VECTOR_H

#include "schemerlicht.h"
#include "smemory.h"

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
  (vec)->element_size = sizeof(element_type);

#define schemerlicht_vector_init(ctxt, vec, element_type) \
  (vec)->vector_ptr = schemerlicht_newvector(ctxt, 8, element_type); \
  (vec)->vector_capacity = cast(schemerlicht_memsize, 8); \
  (vec)->vector_size = 0; \
  (vec)->element_size = sizeof(element_type);

#define schemerlicht_vector_destroy(ctxt, vec) \
  schemerlicht_realloc(ctxt, (vec)->vector_ptr, (vec)->vector_capacity*(vec)->element_size, 0);

#define schemerlicht_vector_at(ctxt, vec, index, element_type) \
  cast(element_type*, (cast(schemerlicht_byte*, (vec)->vector_ptr) + (index)*(vec)->element_size))

#define schemerlicht_vector_popback(vec) \
  if ((vec)->vector_size > 0) --(vec)->vector_size;

#endif //SCHEMERLICHT_VECTOR_H