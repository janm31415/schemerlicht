#include "context.h"
#include "memory.h"
#include "parser.h"

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
  schemerlicht_vector_destroy(ctxt, &ctxt_to_free->stack);
  schemerlicht_free(ctxt, ctxt_to_free, sizeof(schemerlicht_context));
  }

static void context_init(schemerlicht_context* ctxt)
  {
  schemerlicht_assert(ctxt->global != NULL);
  ctxt->error_jmp = NULL;
  schemerlicht_vector_init_with_size(ctxt, &ctxt->stack, schemerlicht_maxstack, schemerlicht_object);
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
    context_init(ctxt);
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

schemerlicht_context* schemerlicht_context_init(schemerlicht_context* ctxt)
  {
  schemerlicht_assert(ctxt->global != NULL);
  schemerlicht_context* ctxt_new = context_new(ctxt);
  if (ctxt_new)
    {
    ctxt_new->global = ctxt->global;
    context_init(ctxt_new);
    }
  return ctxt_new;
  }

void schemerlicht_context_destroy(schemerlicht_context* ctxt)
  {
  schemerlicht_assert(ctxt->global != NULL);
  schemerlicht_assert(ctxt != ctxt->global->main_context);
  context_free(ctxt->global->main_context, ctxt);
  }