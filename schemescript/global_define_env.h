#pragma once

#include "schemescript_api.h"

#include "namespace.h"
#include <stdint.h>
#include <memory>
#include <string>

#include "parse.h"
#include "environment.h"
#include "repl_data.h"
#include "context.h"

COMPILER_BEGIN

/*
For each global define, allocates a variable in the environment.
*/
COMPILER_SCHEMESCRIPT_API void global_define_environment_allocation(Program& prog, environment_map& env, repl_data& data, context& ctxt);

COMPILER_END

