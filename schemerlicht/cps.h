#ifndef SCHEMERLICHT_CPS_H
#define SCHEMERLICHT_CPS_H

#include "schemerlicht.h"
#include "parser.h"

// source: The 90 minute Scheme to C compiler, Marc Feeley


// this method assumes that the program is simplified to the core forms (schemerlicht_simplify_to_core_forms), no case, cond, or do allowed
void schemerlicht_continuation_passing_style(schemerlicht_context* ctxt, schemerlicht_program* program);

#endif //SCHEMERLICHT_CPS_H