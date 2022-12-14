#include "memory.h"
#include "error.h"

#include <stdlib.h>


/*
** definition for realloc function. It must assure that schemerlicht_realloc(NULL,
** 0, x) allocates a new block (ANSI C assures that). ('old_size' is the old
** block size; some allocators may use that.)
*/
#ifndef SCHEMERLICHT_REALLOC
#ifdef _WIN32
#define SCHEMERLICHT_REALLOC(ptr, old_size, size) _aligned_realloc(ptr, size, 32)
#else
#define SCHEMERLICHT_REALLOC(ptr, old_size, size) realloc(ptr, size)
#endif
#endif

/*
** definition for free function. ('old_size' is the old block size; some
** allocators may use that.)
*/
#ifndef SCHEMERLICHT_FREE
#ifdef _WIN32
#define SCHEMERLICHT_FREE(ptr,old_size) _aligned_free(ptr)
#else
#define SCHEMERLICHT_FREE(ptr,old_size) free(ptr)
#endif
#endif

void* schemerlicht_realloc(schemerlicht_context* ctxt, void* chunk, schemerlicht_memsize old_size, schemerlicht_memsize new_size)
  {
  UNUSED(old_size);
  schemerlicht_assert((old_size == 0) == (chunk == NULL));
  if (new_size == 0)
    {
    if (chunk != NULL)
      {
      SCHEMERLICHT_FREE(chunk, old_size);
      chunk = NULL;
      }
    else return NULL;
    }
  else
    {
    chunk = SCHEMERLICHT_REALLOC(chunk, old_size, new_size);
    if (chunk == NULL)
      {
      if (ctxt)
        schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_MEMORY);
      else
        return NULL;
      }
    }

  return chunk;
  }

void* schemerlicht_growvector_aux(schemerlicht_context* ctxt, void* chunk, schemerlicht_memsize* size, schemerlicht_memsize element_size)
  {
  void* newblock;
  schemerlicht_memsize newsize = (*size) * 2;
  if (newsize < SCHEMERLICHT_MINSIZEVECTOR)
    newsize = SCHEMERLICHT_MINSIZEVECTOR;  /* minimum size */
  newblock = schemerlicht_realloc(ctxt, chunk,
    *size * element_size,
    newsize * element_size);
  *size = newsize;  /* update only when everything else is OK */
  return newblock;
  }
