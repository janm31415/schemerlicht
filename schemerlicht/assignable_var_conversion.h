#pragma once

#include "schemerlicht_api.h"

#include "namespace.h"

#include "parse.h"

COMPILER_BEGIN

struct compiler_options;
/*
Assumes that alpha conversion has been done and that all variable names are unique
*/
COMPILER_SCHEMERLICHT_API void assignable_variable_conversion(Program& prog, const compiler_options& ops);

COMPILER_END
