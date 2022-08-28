#pragma once

#include "schemescript_api.h"

#include "namespace.h"
#include "parse.h"
#include "repl_data.h"

COMPILER_BEGIN

COMPILER_SCHEMESCRIPT_API void inline_primitives(Program& prog, uint64_t& alpha_conversion_index, bool safe_primitives, bool standard_bindings);

COMPILER_END
