#ifndef SCHEMERLICHT_TAILCALL_H
#define SCHEMERLICHT_TAILCALL_H

#include "schemerlicht.h"
#include "parser.h"

void schemerlicht_tail_call_analysis(schemerlicht_context* ctxt, schemerlicht_program* program);
int schemerlicht_program_only_has_tail_calls(schemerlicht_context* ctxt, schemerlicht_program* program);

#endif // SCHEMERLICHT_TAILCALL_H