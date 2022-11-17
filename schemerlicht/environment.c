#include "environment.h"
#include "map.h"
#include "vector.h"
#include "context.h"
#include "map.h"
#include "object.h"

void schemerlicht_environment_init(schemerlicht_context* ctxt)
  {
  schemerlicht_vector_init(ctxt, &ctxt->environment, schemerlicht_map*);
  schemerlicht_map* default_environment = schemerlicht_map_new(ctxt, 0, 3);
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

void schemerlicht_environment_add_to_base(schemerlicht_context* ctxt, schemerlicht_string* name, schemerlicht_environment_entry entry)
  {
  schemerlicht_assert(ctxt->environment.vector_size > 0);
  schemerlicht_map** active_map = schemerlicht_vector_begin(&ctxt->environment, schemerlicht_map*);
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
    schemerlicht_object* entry_object = schemerlicht_map_get(ctxt, *map_rit, &key);
    if (entry_object != NULL)
      {
      entry->type = entry_object->type - 1;
      entry->position = entry_object->value.fx;
      return 1;
      }
    }
  return 0;
  }

void schemerlicht_environment_update(schemerlicht_context* ctxt, schemerlicht_string* name, schemerlicht_environment_entry entry)
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
    schemerlicht_object* entry_object = schemerlicht_map_get(ctxt, *map_rit, &key);
    if (entry_object != NULL)
      {
      entry_object->type = entry.type + 1;
      entry_object->value.fx = entry.position;
      }
    }
  }

void schemerlicht_environment_push_child(schemerlicht_context* ctxt)
  {
  schemerlicht_map* child_env = schemerlicht_map_new(ctxt, 0, 3);
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

int schemerlicht_environment_base_at(schemerlicht_environment_entry* entry, schemerlicht_string* name, schemerlicht_context* ctxt, schemerlicht_memsize pos)
  {
  schemerlicht_map** parent_map = schemerlicht_vector_at(&ctxt->environment, 0, schemerlicht_map*);
  schemerlicht_assert(pos < cast(schemerlicht_memsize, node_size((*parent_map))));
  if ((*parent_map)->node[pos].key.type == schemerlicht_object_type_string) // valid node
    {
    entry->type = (*parent_map)->node[pos].value.type - 1;
    entry->position = (*parent_map)->node[pos].value.value.fx;
    *name = (*parent_map)->node[pos].key.value.s;
    return 1;
    }  
  return 0;
  }

schemerlicht_string schemerlicht_show_environment(schemerlicht_context* ctxt)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "SCHEMERLICHT ENVIRONMENT:\n");
  schemerlicht_map** map_it = schemerlicht_vector_begin(&ctxt->environment, schemerlicht_map*);
  schemerlicht_map** map_it_end = schemerlicht_vector_end(&ctxt->environment, schemerlicht_map*);
  for (; map_it != map_it_end; ++map_it)
    {
    schemerlicht_map* m = *map_it;
    schemerlicht_memsize size = node_size(m);
    for (schemerlicht_memsize i = 0; i < size; ++i)
      {
      if (m->node[i].key.type == schemerlicht_object_type_string)
        {
        schemerlicht_string_append(ctxt, &s, &m->node[i].key.value.s);
        schemerlicht_string_append_cstr(ctxt, &s, ": ");
        schemerlicht_environment_entry entry;
        entry.type = m->node[i].value.type - 1;
        entry.position = m->node[i].value.value.fx;
        schemerlicht_string tmp;
        if (entry.type == SCHEMERLICHT_ENV_TYPE_STACK)
          {
          schemerlicht_object* local = schemerlicht_vector_at(&ctxt->stack, entry.position, schemerlicht_object);
          tmp = schemerlicht_object_to_string(ctxt, local, 0);
          }
        else
          {
          schemerlicht_string tmp2 = schemerlicht_object_to_string(ctxt, &m->node[i].value, 0);
          schemerlicht_string_append(ctxt, &s, &tmp2);
          schemerlicht_string_destroy(ctxt, &tmp2);
          schemerlicht_string_append_cstr(ctxt, &s, ": ");
          schemerlicht_object* global = schemerlicht_vector_at(&ctxt->globals, entry.position, schemerlicht_object);
          tmp = schemerlicht_object_to_string(ctxt, global, 0);
          }
        schemerlicht_string_append(ctxt, &s, &tmp);

        schemerlicht_string_append_cstr(ctxt, &s, "\n");
        schemerlicht_string_destroy(ctxt, &tmp);
        }      
      }
    }
  return s;
  }

schemerlicht_object* schemerlicht_environment_find_key_given_position(schemerlicht_context* ctxt, schemerlicht_fixnum global_position)
  {
  schemerlicht_assert(ctxt->environment.vector_size > 0);
  schemerlicht_map* base_map = *schemerlicht_vector_begin(&ctxt->environment, schemerlicht_map*);
  schemerlicht_memsize size = node_size(base_map);
  for (schemerlicht_memsize i = 0; i < size; ++i)
    {
    if (base_map->node[i].key.type == schemerlicht_object_type_string)
      {
      if ((base_map->node[i].value.type == SCHEMERLICHT_ENV_TYPE_GLOBAL + 1) && (base_map->node[i].value.value.fx == global_position))
        {
        return &base_map->node[i].key;
        }
      }
    }
  return NULL;
  }