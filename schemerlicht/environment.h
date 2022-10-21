#ifndef SCHEMERLICHT_ENVIRONMENT_H
#define SCHEMERLICHT_ENVIRONMENT_H

#include "schemerlicht.h"
#include "string.h"
#include "limits.h"
#include "vector.h"

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
int schemerlicht_environment_find(schemerlicht_environment_entry* entry, schemerlicht_context* ctxt, schemerlicht_string* name);

void schemerlicht_environment_push_child(schemerlicht_context* ctxt);
void schemerlicht_environment_pop_child(schemerlicht_context* ctxt);

#endif //SCHEMERLICHT_ENVIRONMENT_H