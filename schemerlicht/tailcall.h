#ifndef SCHEMERLICHT_TAILCALL_H
#define SCHEMERLICHT_TAILCALL_H

#include "schemerlicht.h"
#include "parser.h"

/*
A procedure call is called a tail call if it occurs in a tail position.
A tail position is defined recursively as follows:

- the body of a procedure is in tail position
- if a let expression is in tail position, then the body
  of the let is in tail position
- if the condition expression (if test conseq altern) is in tail
  position, then the conseq and altern branches are also in tail position.
- all other expressions are not in tail position.
*/

void schemerlicht_tail_call_analysis(schemerlicht_context* ctxt, schemerlicht_program* program);
int schemerlicht_program_only_has_tail_calls(schemerlicht_context* ctxt, schemerlicht_program* program);

#endif // SCHEMERLICHT_TAILCALL_H