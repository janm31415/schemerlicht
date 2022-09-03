#ifndef SCHEMERLICHT_ERROR_H
#define SCHEMERLICHT_ERROR_H

#include "schemerlicht.h"

#define SCHEMERLICHT_ERROR_MEMORY 1

void schemerlicht_throw(schemerlicht_context* ctxt, int errorcode);

#endif //SCHEMERLICHT_ERROR_H