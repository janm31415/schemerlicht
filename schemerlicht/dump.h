#ifndef SCHEMERLICHT_DUMP_H
#define SCHEMERLICHT_DUMP_H

#include "schemerlicht.h"
#include "visitor.h"
#include "string.h"

typedef struct schemerlicht_dump_visitor 
  {
    schemerlicht_visitor* visitor;
    schemerlicht_string s;
    char binding_left_bracket;
    char binding_right_bracket;
  } schemerlicht_dump_visitor;

schemerlicht_dump_visitor* schemerlicht_dump_visitor_new(schemerlicht_context* ctxt);
void schemerlicht_dump_visitor_free(schemerlicht_context* ctxt, schemerlicht_dump_visitor* v);

schemerlicht_string schemerlicht_dump(schemerlicht_context* ctxt, schemerlicht_program* prog, int square_brackets);
schemerlicht_string schemerlicht_dump_expression(schemerlicht_context* ctxt, schemerlicht_expression* e, int square_brackets);

#endif //SCHEMERLICHT_DUMP_H