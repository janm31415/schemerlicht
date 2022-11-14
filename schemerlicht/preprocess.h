#ifndef SCHEMERLICHT_PREPROCESS_H
#define SCHEMERLICHT_PREPROCESS_H

#include "schemerlicht.h"
#include "parser.h"

void schemerlicht_preprocess(schemerlicht_context* ctxt, schemerlicht_program* prog);
void schemerlicht_preprocess_internal_libs(schemerlicht_context* ctxt, schemerlicht_program* prog);

#endif //SCHEMERLICHT_PREPROCESS_H