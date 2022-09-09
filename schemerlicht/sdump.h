#ifndef SCHEMERLICHT_DUMP_H
#define SCHEMERLICHT_DUMP_H

#include "schemerlicht.h"
#include "svisitor.h"
#include "sstring.h"

typedef struct schemerlicht_dump_visitor schemerlicht_dump_visitor;

struct schemerlicht_dump_visitor 
  {
    schemerlicht_visitor* visitor;
    schemerlicht_string s;
  };

schemerlicht_dump_visitor* schemerlicht_dump_visitor_new(schemerlicht_context* ctxt);
void schemerlicht_dump_visitor_free(schemerlicht_context* ctxt, schemerlicht_dump_visitor* v);

#endif //SCHEMERLICHT_DUMP_H