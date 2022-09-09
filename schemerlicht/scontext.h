#ifndef SCHEMERLICHT_CONTEXT_H
#define SCHEMERLICHT_CONTEXT_H

#include "schemerlicht.h"
#include "sobject.h"
#include "smap.h"

#include <setjmp.h>

struct schemerlicht_longjmp
  {
  schemerlicht_alignment dummy;
  struct schemerlicht_longjmp* previous;
  jmp_buf jmp;
  volatile int status; // error code
  };

// global state
typedef struct schemerlicht_global_context
  {
  schemerlicht_map_node dummy_node[1]; //common node array for all empty tables
  } schemerlicht_global_context; 

// per thread state
struct schemerlicht_context 
  {
  schemerlicht_global_context* global;
  schemerlicht_object* objects;
  struct schemerlicht_longjmp* error_jmp;  // current error recover point
  };

#endif //SCHEMERLICHT_CONTEXT_H