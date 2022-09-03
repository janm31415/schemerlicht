#pragma once

#include "schemerlicht_api.h"

#include "namespace.h"
#include "parse.h"

COMPILER_BEGIN

COMPILER_SCHEMERLICHT_API void tail_call_analysis(Expression& e);
COMPILER_SCHEMERLICHT_API void tail_call_analysis(Program& prog);

COMPILER_END

