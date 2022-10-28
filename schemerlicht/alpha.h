#ifndef SCHEMERLICHT_ALPHA_H
#define SCHEMERLICHT_ALPHA_H

#include "schemerlicht.h"
#include "parser.h"

schemerlicht_string schemerlicht_make_alpha_name(schemerlicht_context* ctxt, schemerlicht_string* original, schemerlicht_memsize index);
schemerlicht_string schemerlicht_get_original_name_from_alpha(schemerlicht_context* ctxt, schemerlicht_string* alpha_name);

void schemerlicht_alpha_conversion(schemerlicht_context* ctxt, schemerlicht_program* program);

#endif //SCHEMERLICHT_ALPHA_H