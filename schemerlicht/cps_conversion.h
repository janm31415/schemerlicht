#pragma once

#include "schemerlicht_api.h"

#include "namespace.h"
#include "parse.h"


COMPILER_BEGIN
struct compiler_options;

COMPILER_SCHEMERLICHT_API void cps_conversion(Program& prog, const compiler_options& ops);

COMPILER_END
