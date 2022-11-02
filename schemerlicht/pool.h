#ifndef SCHEMERLICHT_POOL_H
#define SCHEMERLICHT_POOL_H

#include "schemerlicht.h"
#include "object.h"
#include "vector.h"

typedef struct schemerlicht_chunk schemerlicht_chunk;

typedef struct schemerlicht_chunk
  {
  /*
  * When a chunk is free, the `next` contains the
  * address of the next chunk in a list.
  *
  * When it's allocated, this space is used by
  * the user.
  */
  schemerlicht_chunk* next;
  } schemerlicht_chunk;

typedef struct schemerlicht_pool_allocator
  {
  schemerlicht_memsize chunks_per_block;
  schemerlicht_memsize chunk_size;
  schemerlicht_chunk* alloc; // allocation pointer
  schemerlicht_vector block_addresses; // for clean up at the end
  } schemerlicht_pool_allocator;

void schemerlicht_pool_allocator_init(schemerlicht_context* ctxt, schemerlicht_pool_allocator* pool, schemerlicht_memsize chunks_per_block, schemerlicht_memsize chunk_size);
void schemerlicht_pool_allocator_destroy(schemerlicht_context* ctxt, schemerlicht_pool_allocator* pool);

void* schemerlicht_pool_allocate(schemerlicht_context* ctxt, schemerlicht_pool_allocator* pool);
void schemerlicht_pool_deallocate(schemerlicht_pool_allocator* pool, void* chunk);

#endif //SCHEMERLICHT_POOL_H