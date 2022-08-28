#pragma once

#include "schemescript_api.h"

#include "namespace.h"
#include <stdint.h>

#include "parse.h"

COMPILER_BEGIN

struct compiler_options;

COMPILER_SCHEMESCRIPT_API void closure_conversion(Program& prog, const compiler_options& ops);

COMPILER_END

