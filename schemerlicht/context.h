#ifndef SCHEMERLICHT_CONTEXT_H
#define SCHEMERLICHT_CONTEXT_H

#include "schemerlicht.h"
#include "object.h"
#include "map.h"
#include "reader.h"

#include <setjmp.h>

typedef struct schemerlicht_longjmp
  {
  schemerlicht_alignment dummy;
  struct schemerlicht_longjmp* previous;
  jmp_buf jmp;
  volatile int status; // error code  
  } schemerlicht_longjmp;

// global state
typedef struct schemerlicht_global_context
  {
  schemerlicht_map_node dummy_node[1]; //common node array for all empty tables
  schemerlicht_context* main_context;
  schemerlicht_map* expression_map;
  schemerlicht_map* primitives_map;
  schemerlicht_cell true_sym;
  schemerlicht_cell false_sym;
  schemerlicht_cell nil_sym;
  } schemerlicht_global_context; 

// per thread state
struct schemerlicht_context 
  {
  schemerlicht_global_context* global;
  schemerlicht_vector stack;
  schemerlicht_vector globals;
  schemerlicht_vector raw_heap;
  schemerlicht_object* heap;
  schemerlicht_memsize heap_pos;
  int number_of_syntax_errors;
  int number_of_compile_errors;
  schemerlicht_vector syntax_error_reports;
  schemerlicht_vector compile_error_reports;
  struct schemerlicht_longjmp* error_jmp;  // current error recover point
  schemerlicht_vector environment; // linked chain of environment maps
  schemerlicht_map* quote_to_index;
  schemerlicht_map* string_to_symbol;
  schemerlicht_memsize quote_to_index_size;
  };

SCHEMERLICHT_API schemerlicht_context* schemerlicht_open(schemerlicht_memsize heap_size);
SCHEMERLICHT_API void schemerlicht_close(schemerlicht_context* ctxt);

// for new threads
SCHEMERLICHT_API schemerlicht_context* schemerlicht_context_init(schemerlicht_context* ctxt, schemerlicht_memsize heap_size);
SCHEMERLICHT_API void schemerlicht_context_destroy(schemerlicht_context* ctxt);

#endif //SCHEMERLICHT_CONTEXT_H