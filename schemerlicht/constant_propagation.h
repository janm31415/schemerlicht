#pragma once

#include "schemerlicht_api.h"

#include "namespace.h"
#include "parse.h"

COMPILER_BEGIN

COMPILER_SCHEMERLICHT_API void constant_propagation(Program& prog);

COMPILER_END
