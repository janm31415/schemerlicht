#ifndef SCHEMERLICHT_ASSIGNABLEVARCONV_H
#define SCHEMERLICHT_ASSIGNABLEVARCONV_H

#include "schemerlicht.h"
#include "parser.h"

void schemerlicht_find_assignable_variables(schemerlicht_context* ctxt, schemerlicht_program* program);
void schemerlicht_assignable_variable_conversion(schemerlicht_context* ctxt, schemerlicht_program* program);

#endif //SCHEMERLICHT_ASSIGNABLEVARCONV_H