#ifndef SCHEMERLICHT_BEGCONV_H
#define SCHEMERLICHT_BEGCONV_H

#include "schemerlicht.h"
#include "parser.h"

void schemerlicht_single_begin_conversion(schemerlicht_context* ctxt, schemerlicht_program* program);

void schemerlicht_remove_nested_begin_expressions(schemerlicht_context* ctxt, schemerlicht_program* program);

void schemerlicht_remove_nested_begin(schemerlicht_context* ctxt, schemerlicht_expression* expr);

#endif //SCHEMERLICHT_BEGCONV_H