#ifndef SCHEMERLICHT_ENVIRONMENT_H
#define SCHEMERLICHT_ENVIRONMENT_H

#include "schemerlicht.h"
#include "string.h"
#include "limits.h"
#include "vector.h"
#include "object.h"

typedef enum
  {
  SCHEMERLICHT_ENV_TYPE_STACK,
  SCHEMERLICHT_ENV_TYPE_GLOBAL
  } schemerlicht_environment_type;

typedef struct schemerlicht_environment_entry
  {
  schemerlicht_environment_type type;
  schemerlicht_fixnum position;
  } schemerlicht_environment_entry;

void schemerlicht_environment_init(schemerlicht_context* ctxt);
void schemerlicht_environment_destroy(schemerlicht_context* ctxt);

void schemerlicht_environment_add(schemerlicht_context* ctxt, schemerlicht_string* name, schemerlicht_environment_entry entry);
void schemerlicht_environment_add_to_base(schemerlicht_context* ctxt, schemerlicht_string* name, schemerlicht_environment_entry entry);
int schemerlicht_environment_find(schemerlicht_environment_entry* entry, schemerlicht_context* ctxt, schemerlicht_string* name);
void schemerlicht_environment_update(schemerlicht_context* ctxt, schemerlicht_string* name, schemerlicht_environment_entry entry);

void schemerlicht_environment_push_child(schemerlicht_context* ctxt);
void schemerlicht_environment_pop_child(schemerlicht_context* ctxt);

// The next 2 functions are useful for garbage collection where we need to run over the environment in order to know which heap positions to keep.

// returns the number of elements in the environment at the parent/base level (thus not taking into account children)
schemerlicht_memsize schemerlicht_environment_base_size(schemerlicht_context* ctxt);

// returns 1 if a valid environment entry found at position pos, 0 otherwise. The entry is returned in 'entry'.
int schemerlicht_environment_base_at(schemerlicht_environment_entry* entry, schemerlicht_string* name, schemerlicht_context* ctxt, schemerlicht_memsize pos);

schemerlicht_object* schemerlicht_environment_find_key_given_position(schemerlicht_context* ctxt, schemerlicht_fixnum global_position);

schemerlicht_string schemerlicht_show_environment(schemerlicht_context* ctxt);

#endif //SCHEMERLICHT_ENVIRONMENT_H