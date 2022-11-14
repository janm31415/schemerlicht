#ifndef SCHEMERLICHT_CONSTPROP_H
#define SCHEMERLICHT_CONSTPROP_H

#include "schemerlicht.h"
#include "parser.h"

void schemerlicht_constant_propagation(schemerlicht_context* ctxt, schemerlicht_program* program);

#endif //SCHEMERLICHT_CONSTPROP_H