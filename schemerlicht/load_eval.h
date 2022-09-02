#pragma once

#include "namespace.h"
#include <stdint.h>

COMPILER_BEGIN

uint64_t c_prim_load(const char* filename, uint64_t context_address, uint64_t repl_data_address, uint64_t env_address);
uint64_t c_prim_eval(const char* script, uint64_t context_address, uint64_t repl_data_address, uint64_t env_address);

COMPILER_END
