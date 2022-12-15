#ifndef SCHEMERLICHT_MEMORY_H
#define SCHEMERLICHT_MEMORY_H

#include "schemerlicht.h"
#include "limits.h"

#define SCHEMERLICHT_MINSIZEVECTOR 2

SCHEMERLICHT_API void* schemerlicht_realloc(schemerlicht_context* ctxt, void* chunk, schemerlicht_memsize old_size, schemerlicht_memsize new_size);

#define schemerlicht_free(ctxt, chunk, chunksize) schemerlicht_realloc(ctxt, (chunk), (chunksize), 0)
#define schemerlicht_delete(ctxt, chunk) schemerlicht_realloc(ctxt, (chunk), sizeof(*(chunk)), 0)
#define schemerlicht_freevector(ctxt, chunk, vector_size, element_type)	schemerlicht_realloc(ctxt, (chunk), cast(schemerlicht_memsize, vector_size)*cast(schemerlicht_memsize, sizeof(element_type)), 0)

#define schemerlicht_malloc(ctxt, size) schemerlicht_realloc(ctxt, NULL, 0, (size))
#define schemerlicht_new(ctxt, element_type) cast(element_type*, schemerlicht_malloc(ctxt, sizeof(element_type)))
#define schemerlicht_newvector(ctxt, vector_size, element_type) cast(element_type*, schemerlicht_malloc(ctxt, cast(schemerlicht_memsize, vector_size)*cast(schemerlicht_memsize, sizeof(element_type))))

#define schemerlicht_reallocvector(ctxt, vec, old_vector_size, new_vector_size, element_type) \
   ((vec)=cast(element_type*, schemerlicht_realloc(ctxt, vec, cast(schemerlicht_memsize, old_vector_size)*cast(schemerlicht_memsize, sizeof(element_type)), \
          cast(schemerlicht_memsize, new_vector_size)*cast(schemerlicht_memsize, sizeof(element_type)))))

void* schemerlicht_growvector_aux(schemerlicht_context* ctxt, void* chunk, schemerlicht_memsize* size, schemerlicht_memsize element_size);

#define schemerlicht_growvector(ctxt, vec, number_of_elements, vector_size, element_type) \
          if ((number_of_elements) > (vector_size)) \
            ((vec)=cast(element_type*, schemerlicht_growvector_aux(ctxt, vec, &(vector_size), sizeof(element_type))))

#endif //SCHEMERLICHT_MEMORY_H