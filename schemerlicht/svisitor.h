#ifndef SCHEMERLICHT_VISITOR_H
#define SCHEMERLICHT_VISITOR_H

#include "schemerlicht.h"
#include "sparser.h"
#include "svector.h"

enum schemerlicht_visitor_entry_type
  {
  SCHEMERLICHT_VISITOR_EXPRESSION_PRE,
  SCHEMERLICHT_VISITOR_EXPRESSION_POST,
  SCHEMERLICHT_VISITOR_LITERAL
  };

typedef struct schemerlicht_visitor_entry
  {
  schemerlicht_expression* expr;
  enum schemerlicht_visitor_entry_type type;
  } schemerlicht_visitor_entry;

typedef struct schemerlicht_visitor schemerlicht_visitor;

struct schemerlicht_visitor 
  {
  int (*previsit_program)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_program*);
  void (*postvisit_program)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_program*);

  int (*previsit_expression)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*postvisit_expression)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);

  void (*visit_fixnum)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_parsed_fixnum*);
  void (*visit_flonum)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_parsed_flonum*);

  void (*destroy)(schemerlicht_context*, schemerlicht_visitor*);
  void* impl; // Implementation, for example schemerlicht_dump_visitor

  schemerlicht_vector v;
  };

schemerlicht_visitor* schemerlicht_visitor_new(schemerlicht_context* ctxt, void* impl);


void schemerlicht_visit_expression(schemerlicht_context* ctxt, schemerlicht_visitor* vis, schemerlicht_expression* expr);
void schemerlicht_visit_program(schemerlicht_context* ctxt, schemerlicht_visitor* vis, schemerlicht_program* p);

#endif //SCHEMERLICHT_VISITOR_H