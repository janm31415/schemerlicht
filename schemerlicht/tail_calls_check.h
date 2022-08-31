#pragma once

#include "schemerlicht_api.h"

#include "namespace.h"
#include "parse.h"

COMPILER_BEGIN

COMPILER_SCHEMERLICHT_API bool only_tail_calls(Program& prog);
COMPILER_SCHEMERLICHT_API bool only_tail_calls(Expression& e);

COMPILER_END
