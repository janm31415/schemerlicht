#pragma once

#include "namespace.h"

#include "environment.h"
#include "repl_data.h"
#include "parse.h"

COMPILER_BEGIN

struct cinput_data;

void cinput_conversion(cinput_data& cinput, Program& prog, environment_map& env, repl_data& rd);

COMPILER_END
