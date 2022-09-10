#include "scontext.h"
#include "smemory.h"
#include "sparser.h"

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
    g->expression_map = generate_expression_map(ctxt);
    g->true_sym = schemerlicht_make_true_sym_cell(ctxt);
    g->false_sym = schemerlicht_make_false_sym_cell(ctxt);
    g->nil_sym = schemerlicht_make_nil_sym_cell(ctxt);
    ctxt->error_jmp = NULL;
    }
  return ctxt;
  }

void schemerlicht_close(schemerlicht_context* ctxt)
  {
  ctxt = ctxt->global->main_context;
  schemerlicht_map_free(ctxt, ctxt->global->expression_map);
  schemerlicht_destroy_cell(ctxt, &ctxt->global->true_sym);
  schemerlicht_destroy_cell(ctxt, &ctxt->global->false_sym);
  schemerlicht_destroy_cell(ctxt, &ctxt->global->nil_sym);
  schemerlicht_free(ctxt, ctxt->global, sizeof(schemerlicht_global_context));
  context_free(NULL, ctxt);
  }