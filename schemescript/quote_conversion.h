#pragma once

#include "schemescript_api.h"

#include "namespace.h"
#include "environment.h"
#include "parse.h"
#include "repl_data.h"
#include "context.h"

COMPILER_BEGIN

COMPILER_SCHEMESCRIPT_API void quote_conversion(Program& prog, repl_data& data, environment_map& env, context& ctxt);

COMPILER_END