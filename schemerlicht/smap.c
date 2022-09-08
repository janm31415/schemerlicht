#include "smap.h"
#include "smemory.h"

schemerlicht_map* schemerlicht_map_new(schemerlicht_context* ctxt, schemerlicht_memsize array_size, schemerlicht_memsize log_hash_size)
  {
  schemerlicht_map* m = schemerlicht_new(ctxt, schemerlicht_map);
  m->array = NULL;
  m->hash = NULL;
  m->size_array = 0;
  m->log_size_node = 0;
  return m;
  }

void schemerlicht_map_free(schemerlicht_context* ctxt, schemerlicht_map* map)
  {
  schemerlicht_delete(ctxt, map);
  }