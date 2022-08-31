#pragma once

#include "schemerlicht_api.h"

#include "namespace.h"
#include <stdint.h>

#include "environment.h"
#include "parse.h"

COMPILER_BEGIN

COMPILER_SCHEMERLICHT_API void free_variable_analysis(Program& prog, environment_map& env);
void free_variable_analysis(Lambda& lam, environment_map& env);

COMPILER_END
