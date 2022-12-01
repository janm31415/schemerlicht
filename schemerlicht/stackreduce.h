#ifndef SCHEMERLICHT_STACKREDUCE_H
#define SCHEMERLICHT_STACKREDUCE_H

#include "schemerlicht.h"
#include "parser.h"

void schemerlicht_stack_reduce_conversion(schemerlicht_context* ctxt, schemerlicht_program* program, int max_vars_on_stack_allowed);

#endif //SCHEMERLICHT_STACKREDUCE_H