#pragma once

#include "namespace.h"
#include <stdint.h>
#include "environment.h"
#include "reg_alloc.h"
#include "reg_alloc_map.h"
#include "schemescript_api.h"
#include <string>
#include <memory>

COMPILER_BEGIN

struct context;

struct compile_data
  {
  std::unique_ptr<reg_alloc> ra;
  reg_alloc_map ra_map;
  uint64_t first_ra_map_time_point_to_elapse; // helper variable for speeding up method register_allocation_expire_old_intervals
  uint32_t local_stack_size;
  uint64_t globals_stack;
  uint64_t heap_size;
  std::string halt_label;
  context* p_ctxt;
  };


COMPILER_SCHEMESCRIPT_API compile_data create_compile_data(uint64_t heap_size, uint64_t globals_stack, uint32_t local_stack, context* p_ctxt);

COMPILER_END
