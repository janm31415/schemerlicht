#ifndef SCHEMERLICHT_OVERRIDES_H
#define SCHEMERLICHT_OVERRIDES_H

#include "schemerlicht.h"
#include "parser.h"

// primitive calls can be overridden by the user
// in that case, we change the primitive call to a funcall.
void schemerlicht_overrides_conversion(schemerlicht_context* ctxt, schemerlicht_program* program);

#endif //SCHEMERLICHT_OVERRIDES_H