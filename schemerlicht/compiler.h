#ifndef SCHEMERLICHT_COMPILER_H
#define SCHEMERLICHT_COMPILER_H

#include "func.h"
#include "parser.h"

SCHEMERLICHT_API schemerlicht_function schemerlicht_compile_expression(schemerlicht_context* ctxt, schemerlicht_expression* e);

#endif //SCHEMERLICHT_COMPILER_H