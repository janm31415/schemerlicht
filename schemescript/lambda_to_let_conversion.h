#pragma once

#include "schemescript_api.h"

#include "namespace.h"
#include "parse.h"

COMPILER_BEGIN

COMPILER_SCHEMESCRIPT_API void lambda_to_let_conversion(Program& prog);

COMPILER_END
