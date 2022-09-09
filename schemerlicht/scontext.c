#include "scontext.h"
#include "smemory.h"

static schemerlicht_context* context_new(schemerlicht_context* ctxt)
  {
  schemerlicht_byte* block = (schemerlicht_byte*)schemerlicht_malloc(ctxt, sizeof(schemerlicht_context));
  if (block == NULL)
    return NULL;
  else
    return cast(schemerlicht_context*, block);
  }

static void context_free(schemerlicht_context* ctxt, schemerlicht_context* ctxt_to_free)
  {
  schemerlicht_free(ctxt, ctxt_to_free, sizeof(schemerlicht_context));
  }

schemerlicht_context* schemerlicht_open()
  {
  schemerlicht_context* ctxt = context_new(NULL);
  if (ctxt)
    {
    schemerlicht_global_context* g = schemerlicht_new(NULL, schemerlicht_global_context);
    ctxt->global = g;
    get_key(g->dummy_node)->type = schemerlicht_object_type_nil;
    get_value(g->dummy_node)->type = schemerlicht_object_type_nil;
    g->dummy_node->next = NULL;
    g->main_context = ctxt;
    ctxt->error_jmp = NULL;
    }
  return ctxt;
  }

void schemerlicht_close(schemerlicht_context* ctxt)
  {
  ctxt = ctxt->global->main_context;
  schemerlicht_free(ctxt, ctxt->global, sizeof(schemerlicht_global_context));
  context_free(NULL, ctxt);
  }