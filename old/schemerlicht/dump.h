#pragma once

#include "schemerlicht_api.h"
#include "namespace.h"

#include <string>

#include <ostream>
#include "parse.h"

COMPILER_BEGIN

COMPILER_SCHEMERLICHT_API void dump(std::ostream& out, Program& prog, bool use_square_brackets = true);

void dump(std::ostream& out, Expression& expr, bool use_square_brackets = true);

COMPILER_END
