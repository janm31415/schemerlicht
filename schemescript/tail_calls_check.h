#pragma once

#include "schemescript_api.h"

#include "namespace.h"
#include "parse.h"

COMPILER_BEGIN

COMPILER_SCHEMESCRIPT_API bool only_tail_calls(Program& prog);
COMPILER_SCHEMESCRIPT_API bool only_tail_calls(Expression& e);

COMPILER_END
