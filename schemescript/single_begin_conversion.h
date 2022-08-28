#pragma once

#include "schemescript_api.h"

#include "namespace.h"
#include "parse.h"

COMPILER_BEGIN

COMPILER_SCHEMESCRIPT_API void single_begin_conversion(Program& prog);

void remove_nested_begin_expressions(Program& p);
void remove_nested_begin_expressions(Lambda& lam);
void remove_nested_begin_expressions(Let& let);

COMPILER_END

