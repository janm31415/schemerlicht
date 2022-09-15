#ifndef SCHEMERLICHT_DUMP_H
#define SCHEMERLICHT_DUMP_H

#include "schemerlicht.h"
#include "visitor.h"
#include "string.h"

typedef struct schemerlicht_dump_visitor 
  {
    schemerlicht_visitor* visitor;
    schemerlicht_string s;
  } schemerlicht_dump_visitor;

schemerlicht_dump_visitor* schemerlicht_dump_visitor_new(schemerlicht_context* ctxt);
void schemerlicht_dump_visitor_free(schemerlicht_context* ctxt, schemerlicht_dump_visitor* v);

schemerlicht_string schemerlicht_dump(schemerlicht_context* ctxt, schemerlicht_program* prog);
schemerlicht_string schemerlicht_dump_expression(schemerlicht_context* ctxt, schemerlicht_expression* e);

#endif //SCHEMERLICHT_DUMP_H