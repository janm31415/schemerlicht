#pragma once

#include "schemescript_api.h"

#include "namespace.h"
#include <stdint.h>

#define SCHEME_VARIABLE_DEBUG_STACK_SIZE 5

COMPILER_BEGIN

struct context {
  uint64_t* alloc; // offset 0
  uint64_t* limit; // offset 8
  uint64_t* from_space; // offset 16
  uint64_t* from_space_end; // offset 24
  uint64_t* to_space; // offset 32
  uint64_t* to_space_end; // offset 40
  uint64_t total_heap_size; // offset 48
  uint64_t from_space_reserve; // offset 56
  uint64_t* locals; // offset 64
  uint64_t number_of_locals; // offset 72
  uint64_t* temporary_flonum; // offset 80
  uint64_t* gc_save; //offset 88
  uint64_t* globals; // offset 96
  uint64_t* globals_end; //offset 104
  uint64_t* rsp_save; // offset 112
  uint64_t* buffer; // offset 120
  uint64_t* dcvt; // offset 128
  uint64_t* ocvt; // offset 136
  uint64_t* xcvt; // offset 144
  uint64_t* gcvt; // offset 152
  uint64_t* stack_top; // offset 160
  uint64_t* stack; // offset 168
  uint64_t* stack_save; // offset 176
  uint64_t* error_label; // offset 184
  void* rsp; // offset 192
  uint64_t* stack_end; // offset 200
  uint64_t last_global_variable_used[SCHEME_VARIABLE_DEBUG_STACK_SIZE]; // offset 208

  uint64_t* memory_allocated;
  };


COMPILER_SCHEMESCRIPT_API context create_context(uint64_t heap_size, uint64_t globals_stack, uint32_t local_stack, uint64_t scheme_stack);
COMPILER_SCHEMESCRIPT_API void destroy_context(context& ctxt);
COMPILER_SCHEMESCRIPT_API context clone_context(const context& ctxt);

COMPILER_END

