#ifndef SCHEMERLICHT_MAP_H
#define SCHEMERLICHT_MAP_H

#include "schemerlicht.h"
#include "sobject.h"
#include "slimits.h"

typedef struct schemerlicht_map_node 
  {
  schemerlicht_object key;
  schemerlicht_object value;
  struct schemerlicht_map_node* next; 
  } schemerlicht_map_node;


typedef struct schemerlicht_map 
  {
  schemerlicht_memsize log_size_node;  /* log2 of size of `node' array */
  schemerlicht_object* array;  /* array part */
  schemerlicht_map_node* hash;
  schemerlicht_map_node* first_free;  /* this position is free; all positions after it are full */
  schemerlicht_memsize size_array;  /* size of `array' array */
  } schemerlicht_map;


schemerlicht_map* schemerlicht_map_new(schemerlicht_context* ctxt, schemerlicht_memsize array_size, schemerlicht_memsize log_hash_size);
void schemerlicht_map_free(schemerlicht_context* ctxt, schemerlicht_map* map);

#endif //SCHEMERLICHT_MAP_H