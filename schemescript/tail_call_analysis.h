#pragma once

#include "schemescript_api.h"

#include "namespace.h"
#include "parse.h"

COMPILER_BEGIN

COMPILER_SCHEMESCRIPT_API void tail_call_analysis(Expression& e);
COMPILER_SCHEMESCRIPT_API void tail_call_analysis(Program& prog);

COMPILER_END

