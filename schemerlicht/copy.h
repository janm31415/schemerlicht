#ifndef SCHEMERLICHT_COPY_H
#define SCHEMERLICHT_COPY_H

#include "schemerlicht.h"
#include "parser.h"

SCHEMERLICHT_API schemerlicht_expression schemerlicht_expression_copy(schemerlicht_context* ctxt, schemerlicht_expression* e);
SCHEMERLICHT_API schemerlicht_program schemerlicht_program_copy(schemerlicht_context* ctxt, schemerlicht_program* p);

#endif //SCHEMERLICHT_COPY_H
