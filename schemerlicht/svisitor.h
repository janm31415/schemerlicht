#ifndef SCHEMERLICHT_VISITOR_H
#define SCHEMERLICHT_VISITOR_H

#include "schemerlicht.h"
#include "sparser.h"
#include "svector.h"

enum schemerlicht_visitor_entry_type
  {
  SCHEMERLICHT_VISITOR_EXPRESSION_PRE,
  SCHEMERLICHT_VISITOR_EXPRESSION_POST,
  SCHEMERLICHT_VISITOR_LITERAL,
  SCHEMERLICHT_VISITOR_BEGIN_PRE,
  SCHEMERLICHT_VISITOR_BEGIN_POST,
  SCHEMERLICHT_VISITOR_FOREIGNCALL_PRE,
  SCHEMERLICHT_VISITOR_FOREIGNCALL_POST,
  SCHEMERLICHT_VISITOR_FUNCALL_PRE,
  SCHEMERLICHT_VISITOR_FUNCALL_POST,
  SCHEMERLICHT_VISITOR_PRIMCALL_PRE,
  SCHEMERLICHT_VISITOR_PRIMCALL_POST,
  SCHEMERLICHT_VISITOR_IF_PRE,
  SCHEMERLICHT_VISITOR_IF_POST,
  SCHEMERLICHT_VISITOR_LAMBDA_PRE,
  SCHEMERLICHT_VISITOR_LAMBDA_POST,
  SCHEMERLICHT_VISITOR_SET_PRE,
  SCHEMERLICHT_VISITOR_SET_POST,
  SCHEMERLICHT_VISITOR_LET_PRE,
  SCHEMERLICHT_VISITOR_LET_BINDINGS,
  SCHEMERLICHT_VISITOR_LET_POST,
  SCHEMERLICHT_VISITOR_NOP,
  SCHEMERLICHT_VISITOR_QUOTE,
  SCHEMERLICHT_VISITOR_VARIABLE,
  SCHEMERLICHT_VISITOR_COND_PRE,
  SCHEMERLICHT_VISITOR_CASE_PRE,
  SCHEMERLICHT_VISITOR_DO_PRE
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
 
  int (*previsit_begin)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*postvisit_begin)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);

  void (*visit_fixnum)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*visit_flonum)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*visit_nil)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*visit_string)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*visit_symbol)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*visit_true)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*visit_false)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*visit_nop)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*visit_character)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*visit_variable)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*visit_quote)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);

  int (*previsit_primcall)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*postvisit_primcall)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  int (*previsit_funcall)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*postvisit_funcall)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  int (*previsit_foreigncall)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*postvisit_foreigncall)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  int (*previsit_if)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*postvisit_if)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  int (*previsit_set)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*postvisit_set)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  int (*previsit_lambda)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*postvisit_lambda)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  int (*previsit_let)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*visit_let_bindings)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);
  void (*postvisit_let)(schemerlicht_context*, schemerlicht_visitor*, schemerlicht_expression*);

  void (*destroy)(schemerlicht_context*, schemerlicht_visitor*);
  void* impl; // Implementation, for example schemerlicht_dump_visitor

  schemerlicht_vector v;
  };

schemerlicht_visitor* schemerlicht_visitor_new(schemerlicht_context* ctxt, void* impl);


void schemerlicht_visit_expression(schemerlicht_context* ctxt, schemerlicht_visitor* vis, schemerlicht_expression* expr);
void schemerlicht_visit_program(schemerlicht_context* ctxt, schemerlicht_visitor* vis, schemerlicht_program* p);

#endif //SCHEMERLICHT_VISITOR_H