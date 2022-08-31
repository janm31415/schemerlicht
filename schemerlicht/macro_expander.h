#pragma once

#include "schemerlicht_api.h"

#include "namespace.h"
#include <stdint.h>

#include "macro_data.h"
#include "environment.h"
#include "parse.h"
#include "repl_data.h"
#include "context.h"
#include "primitive_map.h"

COMPILER_BEGIN

struct compiler_options;

COMPILER_SCHEMERLICHT_API void expand_macros(Program& prog, environment_map& env, repl_data& rd, macro_data& md, context& ctxt, const primitive_map& pm, const compiler_options& ops);

COMPILER_END

