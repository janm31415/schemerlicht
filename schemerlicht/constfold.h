#ifndef SCHEMERLICHT_CONSTFOLD_H
#define SCHEMERLICHT_CONSTFOLD_H

#include "schemerlicht.h"
#include "parser.h"
#include "vector.h"

void schemerlicht_constant_folding(schemerlicht_context* ctxt, schemerlicht_program* program);
schemerlicht_vector schemerlicht_generate_foldable_primitives_set(schemerlicht_context* ctxt);


#endif //SCHEMERLICHT_CONSTFOLD_H