#ifndef SCHEMERLICHT_CONTEXT_H
#define SCHEMERLICHT_CONTEXT_H

#include "schemerlicht.h"
#include "object.h"
#include "map.h"
#include "reader.h"
#include "pool.h"
#include "vector.h"
#include "func.h"

#include <setjmp.h>

#define SCHEMERLICHT_USE_POOL
#define SCHEMERLICHT_MAX_POOL 8

#define SCHEMERLICHT_STRING_PORT_ID -2

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
  schemerlicht_vector foldable_primitives;
  } schemerlicht_global_context; 

// per thread state
struct schemerlicht_context 
  {
  schemerlicht_global_context* global;
  schemerlicht_vector stack_raw;
  schemerlicht_vector stack;
  schemerlicht_vector globals;
  schemerlicht_vector raw_heap;
  schemerlicht_object* heap;
  schemerlicht_memsize heap_pos;
  schemerlicht_memsize gc_heap_pos_threshold;
  int number_of_syntax_errors;
  int number_of_compile_errors;
  int number_of_runtime_errors;
  schemerlicht_vector syntax_error_reports;
  schemerlicht_vector compile_error_reports;
  schemerlicht_vector runtime_error_reports;
  struct schemerlicht_longjmp* error_jmp;  // current error recover point
  schemerlicht_vector environment; // linked chain of environment maps
  schemerlicht_map* quote_to_index;
  schemerlicht_map* string_to_symbol;
  schemerlicht_memsize quote_to_index_size;
  schemerlicht_vector overrides; // stringvec with names of primitives that were overridden
#ifdef SCHEMERLICHT_DEBUG
  uint64_t time_spent_gc;
#endif
#ifdef SCHEMERLICHT_USE_POOL
  schemerlicht_pool_allocator pool[SCHEMERLICHT_MAX_POOL];
#endif
  schemerlicht_vector externals;
  schemerlicht_map* externals_map;
  schemerlicht_vector lambdas;
  schemerlicht_vector environments; // pointers to other context instantiations that can be used by eval
  schemerlicht_map* macro_map;
  schemerlicht_memsize number_of_macros;
  schemerlicht_string module_path;
  schemerlicht_function* empty_continuation_function;
  schemerlicht_object empty_continuation;
  schemerlicht_function* halt_continuation_function;
  schemerlicht_object halt_continuation;
  schemerlicht_vector gc_save_list;
  schemerlicht_function* callcc_fun;
  schemerlicht_vector filenames_list;
  };

SCHEMERLICHT_API schemerlicht_context* schemerlicht_open(schemerlicht_memsize heap_size);
SCHEMERLICHT_API void schemerlicht_close(schemerlicht_context* ctxt);

// for new threads
SCHEMERLICHT_API schemerlicht_context* schemerlicht_context_init(schemerlicht_context* ctxt, schemerlicht_memsize heap_size);
SCHEMERLICHT_API void schemerlicht_context_destroy(schemerlicht_context* ctxt);

#endif //SCHEMERLICHT_CONTEXT_H