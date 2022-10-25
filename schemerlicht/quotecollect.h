#ifndef SCHEMERLICHT_QUOTECOLLECT_H
#define SCHEMERLICHT_QUOTECOLLECT_H

#include "schemerlicht.h"
#include "parser.h"

/*
* returns a sorted stringvec containing all quotes as string
*/
schemerlicht_vector schemerlicht_quote_collection(schemerlicht_context* ctxt, schemerlicht_program* program);

#endif //SCHEMERLICHT_QUOTECOLLECT_H