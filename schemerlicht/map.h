#ifndef SCHEMERLICHT_MAP_H
#define SCHEMERLICHT_MAP_H

/*
Based on table from LUA
*/

#include "schemerlicht.h"
#include "object.h"
#include "limits.h"

typedef struct schemerlicht_map_node 
  {
  schemerlicht_object key;
  schemerlicht_object value;
  struct schemerlicht_map_node* next; 
  } schemerlicht_map_node;


typedef struct schemerlicht_map 
  {
  schemerlicht_memsize log_node_size;  /* log2 of size of `node' array */
  schemerlicht_object* array;  /* array part */
  schemerlicht_map_node* node;
  schemerlicht_map_node* first_free;  /* this position is free; all positions after it are full */
  schemerlicht_memsize array_size;  /* size of `array' array */
  } schemerlicht_map;


#define node_size(m) (1<<((m)->log_node_size))
#define get_node(m, i)  (&(m)->node[i])
#define get_key(n) (&(n)->key)
#define get_value(n) (&(n)->value)

schemerlicht_map* schemerlicht_map_new(schemerlicht_context* ctxt, schemerlicht_memsize array_size, schemerlicht_memsize log_node_size);
void schemerlicht_map_free(schemerlicht_context* ctxt, schemerlicht_map* map);
// the keys could be used by someone else so they are not destroyed by default
void schemerlicht_map_keys_free(schemerlicht_context* ctxt, schemerlicht_map* map);
// the values could be used by someone else so they are not destroyed by default
void schemerlicht_map_values_free(schemerlicht_context* ctxt, schemerlicht_map* map);
schemerlicht_object* schemerlicht_map_insert_indexed(schemerlicht_context* ctxt, schemerlicht_map* map, schemerlicht_memsize index);
schemerlicht_object* schemerlicht_map_insert(schemerlicht_context* ctxt, schemerlicht_map* map, const schemerlicht_object* key);
schemerlicht_object* schemerlicht_map_insert_string(schemerlicht_context* ctxt, schemerlicht_map* map, const char* str);
schemerlicht_object* schemerlicht_map_get_indexed(schemerlicht_map* map, schemerlicht_memsize index);
schemerlicht_object* schemerlicht_map_get(schemerlicht_map* map, const schemerlicht_object* key);
schemerlicht_object* schemerlicht_map_get_string(schemerlicht_map* map, const char* str);

#endif //SCHEMERLICHT_MAP_H