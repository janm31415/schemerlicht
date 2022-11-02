#include "pool.h"
#include "context.h"
#include "memory.h"

void schemerlicht_pool_allocator_init(schemerlicht_context* ctxt, schemerlicht_pool_allocator* pool, schemerlicht_memsize chunks_per_block, schemerlicht_memsize chunk_size)
  {
  pool->alloc = NULL;
  pool->chunks_per_block = chunks_per_block;
  pool->chunk_size = chunk_size;
  schemerlicht_vector_init(ctxt, &pool->block_addresses, void*);
  }

void schemerlicht_pool_allocator_destroy(schemerlicht_context* ctxt, schemerlicht_pool_allocator* pool)
  {
  const schemerlicht_memsize block_size = pool->chunks_per_block * pool->chunk_size;
  void** it = schemerlicht_vector_begin(&pool->block_addresses, void*);
  void** it_end = schemerlicht_vector_end(&pool->block_addresses, void*);
  for (; it != it_end; ++it)
    {
    schemerlicht_free(ctxt, *it, block_size);
    }
  schemerlicht_vector_destroy(ctxt, &pool->block_addresses);
  }

static schemerlicht_chunk* schemerlicht_pool_allocate_block(schemerlicht_context* ctxt, schemerlicht_pool_allocator* pool)
  {
  const schemerlicht_memsize block_size = pool->chunks_per_block * pool->chunk_size;
  void* new_memory = schemerlicht_malloc(ctxt, block_size);
  // store the new memory in our block addresses list, so that we can free them when schemerlicht_pool_allocator_destroy is called
  schemerlicht_vector_push_back(ctxt, &pool->block_addresses, new_memory, void*);
  schemerlicht_chunk* block = cast(schemerlicht_chunk*, new_memory);  
  schemerlicht_chunk* chunk = block;
  // chain all the chunks in the block
  for (schemerlicht_memsize i = 1; i < pool->chunks_per_block; ++i)
    {
    chunk->next = cast(schemerlicht_chunk*, cast(char*, chunk) + pool->chunk_size);
    chunk = chunk->next;
    }
  chunk->next = NULL;  
  return block;
  }

void* schemerlicht_pool_allocate(schemerlicht_context* ctxt, schemerlicht_pool_allocator* pool)
  {
  if (pool->alloc == NULL) // no free chunks left. Allocate a new block.
    {
    pool->alloc = schemerlicht_pool_allocate_block(ctxt, pool);
    }
  schemerlicht_chunk* free_chunk = pool->alloc; // first free chunk
  pool->alloc = pool->alloc->next; // point to the next free chunk
  return free_chunk;
  }

void schemerlicht_pool_deallocate(schemerlicht_pool_allocator* pool, void* chunk)
  {
  // The freed chunk's next pointer points to the current allocation pointer
  cast(schemerlicht_chunk*, chunk)->next = pool->alloc;
  // And the allocation pointer is now set to the returned (free) chunk
  pool->alloc = cast(schemerlicht_chunk*, chunk);
  }