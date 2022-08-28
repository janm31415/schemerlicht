#pragma once

#include "schemescript_api.h"

#include "namespace.h"
#include "parse.h"

COMPILER_BEGIN

COMPILER_SCHEMESCRIPT_API void constant_propagation(Program& prog);

COMPILER_END
