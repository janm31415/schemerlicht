#pragma once

#include "schemescript_api.h"

#include "namespace.h"
#include <stdint.h>

#include "environment.h"
#include "parse.h"

COMPILER_BEGIN

COMPILER_SCHEMESCRIPT_API void free_variable_analysis(Program& prog, environment_map& env);
void free_variable_analysis(Lambda& lam, environment_map& env);

COMPILER_END
