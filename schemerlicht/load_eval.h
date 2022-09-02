#pragma once

#include "namespace.h"
#include <stdint.h>

COMPILER_BEGIN

uint64_t c_prim_load(const char* filename);
uint64_t c_prim_eval(const char* script);

COMPILER_END
