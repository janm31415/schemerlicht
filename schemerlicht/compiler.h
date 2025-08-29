#ifndef SCHEMERLICHT_COMPILER_H
#define SCHEMERLICHT_COMPILER_H

#include "func.h"
#include "parser.h"
#include "vector.h"

SCHEMERLICHT_API schemerlicht_function* schemerlicht_compile_expression(schemerlicht_context* ctxt, schemerlicht_expression* e);
SCHEMERLICHT_API schemerlicht_vector schemerlicht_compile_program(schemerlicht_context* ctxt, schemerlicht_program* prog);

#endif //SCHEMERLICHT_COMPILER_H
