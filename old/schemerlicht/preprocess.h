#pragma once

#include "namespace.h"
#include "schemerlicht_api.h"
#include "repl_data.h"
#include "compiler_options.h"
#include "macro_data.h"
#include "primitive_map.h"
#include "environment.h"
#include "context.h"
#include "parse.h"

COMPILER_BEGIN

struct cinput_data;

COMPILER_SCHEMERLICHT_API void preprocess(environment_map& env, repl_data& data, macro_data& md, context& ctxt, cinput_data& cinput, Program& prog, const primitive_map& pm, const compiler_options& options);

COMPILER_END
