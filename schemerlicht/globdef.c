#include "globdef.h"
#include "visitor.h"
#include "environment.h"
#include "context.h"
#include "object.h"

typedef struct schemerlicht_globdef_visitor
  {
  schemerlicht_visitor* visitor;
  } schemerlicht_globdef_visitor;

static int previsit_set(schemerlicht_context* ctxt, schemerlicht_visitor* v, schemerlicht_expression* e)
  {
  if (e->expr.set.originates_from_define != 0)
    {
    schemerlicht_environment_entry entry;
    if (schemerlicht_environment_find(&entry, ctxt, &e->expr.set.name) && entry.type == SCHEMERLICHT_ENV_TYPE_GLOBAL)
      {
      /* already allocated before.
      * Example case: (load \"data/load_test.scm\") (zip (list 1 2 3 4) (list 5 6 7 8))
      * In this case, zip is allocated by the main program, since zip is called, but zip is not defined.
      * The load call will define zip. When the load call is executed, zip is already allocated and we will end up in this branch.
      */
      }
    else
      {
      entry.type = SCHEMERLICHT_ENV_TYPE_GLOBAL;
      entry.position = ctxt->globals.vector_size;
      schemerlicht_object obj;
      obj.type = schemerlicht_object_type_undefined;
      schemerlicht_vector_push_back(ctxt, &ctxt->globals, obj, schemerlicht_object);
      schemerlicht_string s;
      schemerlicht_string_copy(ctxt, &s, &e->expr.set.name);
      schemerlicht_environment_add(ctxt, &s, entry);
      }
    }
  return 1;
  }

static schemerlicht_globdef_visitor* schemerlicht_globdef_visitor_new(schemerlicht_context* ctxt)
  {
  schemerlicht_globdef_visitor* v = schemerlicht_new(ctxt, schemerlicht_globdef_visitor);
  v->visitor = schemerlicht_visitor_new(ctxt, v);
  v->visitor->previsit_set = previsit_set;
  return v;
  }

static void schemerlicht_globdef_visitor_free(schemerlicht_context* ctxt, schemerlicht_globdef_visitor* v)
  {
  if (v)
    {
    v->visitor->destroy(ctxt, v->visitor);
    schemerlicht_delete(ctxt, v);
    }
  }

void schemerlicht_global_define_environment_allocation(schemerlicht_context* ctxt, schemerlicht_program* program)
  {
  schemerlicht_globdef_visitor* v = schemerlicht_globdef_visitor_new(ctxt);
  schemerlicht_visit_program(ctxt, v->visitor, program);
  schemerlicht_globdef_visitor_free(ctxt, v);
  }