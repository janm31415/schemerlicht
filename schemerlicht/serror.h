#ifndef SCHEMERLICHT_ERROR_H
#define SCHEMERLICHT_ERROR_H

#include "schemerlicht.h"

#define SCHEMERLICHT_ERROR_MEMORY 1
#define SCHEMERLICHT_ERROR_NO_TOKENS 2
#define SCHEMERLICHT_ERROR_NOT_IMPLEMENTED 3

void schemerlicht_throw(schemerlicht_context* ctxt, int errorcode);

#endif //SCHEMERLICHT_ERROR_H