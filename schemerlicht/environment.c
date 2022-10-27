#include "environment.h"
#include "map.h"
#include "vector.h"
#include "context.h"

void schemerlicht_environment_init(schemerlicht_context* ctxt)
  {
  schemerlicht_vector_init(ctxt, &ctxt->environment, schemerlicht_map*);
  schemerlicht_map* default_environment = schemerlicht_map_new(ctxt, 0, 10);
  schemerlicht_vector_push_back(ctxt, &ctxt->environment, default_environment, schemerlicht_map*);
  }

void schemerlicht_environment_destroy(schemerlicht_context* ctxt)
  {
  schemerlicht_map** map_it = schemerlicht_vector_begin(&ctxt->environment, schemerlicht_map*);
  schemerlicht_map** map_it_end = schemerlicht_vector_end(&ctxt->environment, schemerlicht_map*);
  for (; map_it != map_it_end; ++map_it)
    {
    schemerlicht_map_keys_free(ctxt, *map_it);
    schemerlicht_map_free(ctxt, *map_it);
    }
  schemerlicht_vector_destroy(ctxt, &ctxt->environment);
  }

void schemerlicht_environment_add(schemerlicht_context* ctxt, schemerlicht_string* name, schemerlicht_environment_entry entry)
  {
  schemerlicht_assert(ctxt->environment.vector_size > 0);
  schemerlicht_map** active_map = schemerlicht_vector_back(&ctxt->environment, schemerlicht_map*);
  schemerlicht_object key;
  key.type = schemerlicht_object_type_string;
  key.value.s = *name;
  schemerlicht_object* entry_object = schemerlicht_map_insert(ctxt, *active_map, &key);
  //abusing schemerlicht_object type fo fit in schemerlicht_environment_entry, but avoiding 0
  entry_object->type = cast(int, entry.type) + 1;
  entry_object->value.fx = entry.position;
  }

int schemerlicht_environment_find(schemerlicht_environment_entry* entry, schemerlicht_context* ctxt, schemerlicht_string* name)
  {
  schemerlicht_object key;
  key.type = schemerlicht_object_type_string;
  key.value.s = *name;
  schemerlicht_map** map_it = schemerlicht_vector_begin(&ctxt->environment, schemerlicht_map*);
  schemerlicht_map** map_it_end = schemerlicht_vector_end(&ctxt->environment, schemerlicht_map*);
  schemerlicht_map** map_rit = map_it_end - 1;
  schemerlicht_map** map_rit_end = map_it - 1;
  for (; map_rit != map_rit_end; --map_rit)
    {
    schemerlicht_object* entry_object = schemerlicht_map_get(*map_rit, &key);
    if (entry_object != NULL)
      {
      entry->type = entry_object->type - 1;
      entry->position = entry_object->value.fx;
      return 1;
      }
    }
  return 0;
  }

void schemerlicht_environment_push_child(schemerlicht_context* ctxt)
  {
  schemerlicht_map* child_env = schemerlicht_map_new(ctxt, 0, 10);
  schemerlicht_vector_push_back(ctxt, &ctxt->environment, child_env, schemerlicht_map*);
  }

void schemerlicht_environment_pop_child(schemerlicht_context* ctxt)
  {
  schemerlicht_assert(ctxt->environment.vector_size > 1); // don't pop the root
  schemerlicht_map** child_map = schemerlicht_vector_back(&ctxt->environment, schemerlicht_map*);
  schemerlicht_map_keys_free(ctxt, *child_map);  
  schemerlicht_map_free(ctxt, *child_map);
  schemerlicht_vector_pop_back(&ctxt->environment);
  }

schemerlicht_memsize schemerlicht_environment_base_size(schemerlicht_context* ctxt)
  {
  schemerlicht_map** parent_map = schemerlicht_vector_at(&ctxt->environment, 0, schemerlicht_map*);
  return node_size((*parent_map));
  }

int schemerlicht_environment_base_at(schemerlicht_environment_entry* entry, schemerlicht_context* ctxt, schemerlicht_memsize pos)
  {
  schemerlicht_map** parent_map = schemerlicht_vector_at(&ctxt->environment, 0, schemerlicht_map*);
  schemerlicht_assert(pos < cast(schemerlicht_memsize, node_size((*parent_map))));
  if ((*parent_map)->node[pos].key.type == schemerlicht_object_type_string) // valid node
    {
    entry->type = (*parent_map)->node[pos].value.type - 1;
    entry->position = (*parent_map)->node[pos].value.value.fx;
    return 1;
    }
  return 0;
  }