#include "context.h"
#include "memory.h"
#include "parser.h"
#include "primitives.h"
#include "error.h"
#include "environment.h"
#include "foreign.h"
#include "func.h"
#include "constfold.h"
#include "vm.h"

static schemerlicht_context* context_new(schemerlicht_context* ctxt)
  {
  schemerlicht_byte* block = (schemerlicht_byte*)schemerlicht_malloc(ctxt, sizeof(schemerlicht_context));
  if (block == NULL)
    return NULL;
  else
    return cast(schemerlicht_context*, block);
  }

static void context_free(schemerlicht_context* ctxt)
  {
  schemerlicht_vector_destroy(ctxt, &ctxt->gc_save_list);
  schemerlicht_function_free(ctxt, ctxt->empty_continuation_function);
  schemerlicht_object_destroy(ctxt, &ctxt->empty_continuation);
  schemerlicht_function_free(ctxt, ctxt->halt_continuation_function);
  schemerlicht_object_destroy(ctxt, &ctxt->halt_continuation);
  schemerlicht_map_keys_free(ctxt, ctxt->quote_to_index);
  schemerlicht_map_free(ctxt, ctxt->quote_to_index);
  schemerlicht_map_keys_free(ctxt, ctxt->macro_map);
  schemerlicht_map_free(ctxt, ctxt->macro_map);
  schemerlicht_syntax_errors_clear(ctxt);
  schemerlicht_compile_errors_clear(ctxt);
  schemerlicht_runtime_errors_clear(ctxt);
  schemerlicht_vector_destroy(ctxt, &ctxt->stack_raw);
  schemerlicht_object* it = schemerlicht_vector_begin(&ctxt->raw_heap, schemerlicht_object);
  schemerlicht_object* it_end = schemerlicht_vector_end(&ctxt->raw_heap, schemerlicht_object);
  for (; it != it_end; ++it)
    {
    if (schemerlicht_object_get_type(it) != schemerlicht_object_type_symbol) // symbols are unique and stored in the string to symbol map
      schemerlicht_object_destroy(ctxt, it);
    }
  schemerlicht_map_values_free(ctxt, ctxt->string_to_symbol);
  schemerlicht_map_keys_free(ctxt, ctxt->string_to_symbol);
  schemerlicht_map_free(ctxt, ctxt->string_to_symbol);
  schemerlicht_vector_destroy(ctxt, &ctxt->globals); // we don't destroy the objects in the globals list, as they point to constants or heap objects
  schemerlicht_vector_destroy(ctxt, &ctxt->raw_heap);
  schemerlicht_vector_destroy(ctxt, &ctxt->syntax_error_reports);  
  schemerlicht_vector_destroy(ctxt, &ctxt->compile_error_reports);
  schemerlicht_vector_destroy(ctxt, &ctxt->runtime_error_reports);
#ifdef SCHEMERLICHT_USE_POOL
  for (int i = 0; i < SCHEMERLICHT_MAX_POOL; ++i)
    schemerlicht_pool_allocator_destroy(ctxt, &ctxt->pool[i]);
#endif
  schemerlicht_string* sit = schemerlicht_vector_begin(&ctxt->overrides, schemerlicht_string);
  schemerlicht_string* sit_end = schemerlicht_vector_end(&ctxt->overrides, schemerlicht_string);
  for (; sit != sit_end; ++sit)
    {
    schemerlicht_string_destroy(ctxt, sit);
    }
  schemerlicht_vector_destroy(ctxt, &ctxt->overrides);
  sit = schemerlicht_vector_begin(&ctxt->filenames_list, schemerlicht_string);
  sit_end = schemerlicht_vector_end(&ctxt->filenames_list, schemerlicht_string);
  for (; sit != sit_end; ++sit)
    {
    schemerlicht_string_destroy(ctxt, sit);
    }
  schemerlicht_vector_destroy(ctxt, &ctxt->filenames_list);
  schemerlicht_map_free(ctxt, ctxt->externals_map);
  schemerlicht_external_function* fit = schemerlicht_vector_begin(&ctxt->externals, schemerlicht_external_function);
  schemerlicht_external_function* fit_end = schemerlicht_vector_end(&ctxt->externals, schemerlicht_external_function);
  for (; fit != fit_end; ++fit)
    {
    schemerlicht_external_function_destroy(ctxt, fit);
    }
  schemerlicht_vector_destroy(ctxt, &ctxt->externals);
  schemerlicht_environment_destroy(ctxt);  
  schemerlicht_function** lit = schemerlicht_vector_begin(&ctxt->lambdas, schemerlicht_function*);
  schemerlicht_function** lit_end = schemerlicht_vector_end(&ctxt->lambdas, schemerlicht_function*);
  for (; lit != lit_end; ++lit)
    {
    schemerlicht_function_free(ctxt, *lit);
    }
  schemerlicht_vector_destroy(ctxt, &ctxt->lambdas);
  schemerlicht_context** cit = schemerlicht_vector_begin(&ctxt->environments, schemerlicht_context*);
  schemerlicht_context** cit_end = schemerlicht_vector_end(&ctxt->environments, schemerlicht_context*);
  for (; cit != cit_end; ++cit)
    {
    context_free(*cit);
    }
  schemerlicht_vector_destroy(ctxt, &ctxt->environments);
  schemerlicht_string_destroy(ctxt, &ctxt->module_path);
  schemerlicht_free(ctxt, ctxt, sizeof(schemerlicht_context));
  }

static void context_init(schemerlicht_context* ctxt, schemerlicht_memsize heap_size)
  {
  schemerlicht_assert(ctxt->global != NULL);
  ctxt->error_jmp = NULL;
  ctxt->user_data = NULL;
  ctxt->number_of_syntax_errors = 0;
  ctxt->number_of_compile_errors = 0;
  schemerlicht_vector_init(ctxt, &ctxt->gc_save_list, schemerlicht_object);
  schemerlicht_vector_init_with_size(ctxt, &ctxt->stack_raw, schemerlicht_maxstack, schemerlicht_object);
  schemerlicht_object* it = schemerlicht_vector_begin(&ctxt->stack_raw, schemerlicht_object);
  schemerlicht_object* it_end = schemerlicht_vector_end(&ctxt->stack_raw, schemerlicht_object);
  ctxt->stack = ctxt->stack_raw;
  for (; it != it_end; ++it)
    {
    it->type = schemerlicht_object_type_blocking;
    }
#ifdef SCHEMERLICHT_DEBUG
  ctxt->time_spent_gc = 0;
#endif
  ctxt->heap_pos = 0;
  schemerlicht_vector_init_with_size(ctxt, &ctxt->raw_heap, heap_size, schemerlicht_object);
  it = schemerlicht_vector_begin(&ctxt->raw_heap, schemerlicht_object);
  it_end = schemerlicht_vector_end(&ctxt->raw_heap, schemerlicht_object);
  for (; it != it_end; ++it)
    {
    it->type = schemerlicht_object_type_blocking;
    }
  ctxt->heap = cast(schemerlicht_object*, ctxt->raw_heap.vector_ptr);
  ctxt->gc_heap_pos_threshold = heap_size*8/20; // 80% of total heap size
  ctxt->quote_to_index = schemerlicht_map_new(ctxt, 0, 8);
  ctxt->quote_to_index_size = 0;
  ctxt->string_to_symbol = schemerlicht_map_new(ctxt, 0, 8);
  ctxt->macro_map = schemerlicht_map_new(ctxt, 0, 4);
  ctxt->number_of_macros = 0;
  ctxt->number_of_runtime_errors = 0;
  ctxt->number_of_compile_errors = 0;
  ctxt->number_of_syntax_errors = 0;
#ifdef SCHEMERLICHT_USE_POOL
  for (int i = 0; i < SCHEMERLICHT_MAX_POOL; ++i)
    schemerlicht_pool_allocator_init(ctxt, &ctxt->pool[i], 256, sizeof(schemerlicht_object)*(i+1));
#endif
  schemerlicht_vector_init(ctxt, &ctxt->globals, schemerlicht_object);
  schemerlicht_vector_init(ctxt, &ctxt->syntax_error_reports, schemerlicht_error_report);
  schemerlicht_vector_init(ctxt, &ctxt->compile_error_reports, schemerlicht_error_report);
  schemerlicht_vector_init(ctxt, &ctxt->runtime_error_reports, schemerlicht_error_report);
  schemerlicht_vector_init(ctxt, &ctxt->overrides, schemerlicht_string);
  schemerlicht_vector_init(ctxt, &ctxt->filenames_list, schemerlicht_string);
  schemerlicht_vector_init(ctxt, &ctxt->externals, schemerlicht_external_function);
  ctxt->externals_map = schemerlicht_map_new(ctxt, 0, 4);
  schemerlicht_environment_init(ctxt);  
  schemerlicht_vector_init(ctxt, &ctxt->lambdas, schemerlicht_function*);
  schemerlicht_vector_init(ctxt, &ctxt->environments, schemerlicht_context*);
  schemerlicht_string_init(ctxt, &ctxt->module_path, "");
  ctxt->empty_continuation_function = schemerlicht_function_new(ctxt);
  schemerlicht_instruction i = 0;
  SCHEMERLICHT_SET_OPCODE(i, SCHEMERLICHT_OPCODE_RETURN);
  SCHEMERLICHT_SETARG_A(i, 0);
  SCHEMERLICHT_SETARG_B(i, 0);
  schemerlicht_vector_push_back(ctxt, &ctxt->empty_continuation_function->code, i, schemerlicht_instruction);
  ctxt->empty_continuation = make_schemerlicht_object_closure(ctxt, 1);
  schemerlicht_object* empty_lambda = schemerlicht_vector_begin(&ctxt->empty_continuation.value.v, schemerlicht_object);
  empty_lambda->type = schemerlicht_object_type_lambda;
  empty_lambda->value.ptr = ctxt->empty_continuation_function;

  ctxt->halt_continuation_function = schemerlicht_function_new(ctxt);
  i = 0;
  SCHEMERLICHT_SET_OPCODE(i, SCHEMERLICHT_OPCODE_RETURN);
  SCHEMERLICHT_SETARG_A(i, 1);
  SCHEMERLICHT_SETARG_B(i, 1);
  schemerlicht_vector_push_back(ctxt, &ctxt->halt_continuation_function->code, i, schemerlicht_instruction);
  ctxt->halt_continuation = make_schemerlicht_object_closure(ctxt, 1);
  schemerlicht_object* halt_lambda = schemerlicht_vector_begin(&ctxt->halt_continuation.value.v, schemerlicht_object);
  halt_lambda->type = schemerlicht_object_type_lambda;
  halt_lambda->value.ptr = ctxt->halt_continuation_function;

  ctxt->callcc_fun = NULL;
  }

schemerlicht_context* schemerlicht_open(schemerlicht_memsize heap_size)
  {
  schemerlicht_context* ctxt = context_new(NULL);
  if (ctxt)
    {
    schemerlicht_global_context* g = schemerlicht_new(NULL, schemerlicht_global_context);
    ctxt->global = g;
    get_key(g->dummy_node)->type = schemerlicht_object_type_undefined;
    get_value(g->dummy_node)->type = schemerlicht_object_type_undefined;
    g->dummy_node->next = NULL;
    g->main_context = ctxt;
    g->expression_map = generate_expression_map(ctxt);
    g->primitives_map = generate_primitives_map(ctxt);
    g->true_sym = schemerlicht_make_true_sym_cell(ctxt);
    g->false_sym = schemerlicht_make_false_sym_cell(ctxt);
    g->nil_sym = schemerlicht_make_nil_sym_cell(ctxt);
    g->foldable_primitives = schemerlicht_generate_foldable_primitives_set(ctxt);
    context_init(ctxt, heap_size);
    }
  return ctxt;
  }

void schemerlicht_close(schemerlicht_context* ctxt)
  {
  ctxt = ctxt->global->main_context;
  schemerlicht_map_keys_free(ctxt, ctxt->global->expression_map);
  schemerlicht_map_free(ctxt, ctxt->global->expression_map);
  schemerlicht_map_keys_free(ctxt, ctxt->global->primitives_map);
  schemerlicht_map_free(ctxt, ctxt->global->primitives_map);
  schemerlicht_destroy_cell(ctxt, &ctxt->global->true_sym);
  schemerlicht_destroy_cell(ctxt, &ctxt->global->false_sym);
  schemerlicht_destroy_cell(ctxt, &ctxt->global->nil_sym);
  schemerlicht_string* sit = schemerlicht_vector_begin(&ctxt->global->foldable_primitives, schemerlicht_string);
  schemerlicht_string* sit_end = schemerlicht_vector_end(&ctxt->global->foldable_primitives, schemerlicht_string);
  for (; sit != sit_end; ++sit)
    {
    schemerlicht_string_destroy(ctxt, sit);
    }
  schemerlicht_vector_destroy(ctxt, &ctxt->global->foldable_primitives);
  schemerlicht_free(ctxt, ctxt->global, sizeof(schemerlicht_global_context));
  context_free(ctxt);
  }

schemerlicht_context* schemerlicht_context_init(schemerlicht_context* ctxt, schemerlicht_memsize heap_size)
  {
  schemerlicht_assert(ctxt->global != NULL);
  schemerlicht_context* ctxt_new = context_new(ctxt);
  if (ctxt_new)
    {
    ctxt_new->global = ctxt->global;
    context_init(ctxt_new, heap_size);
    }
  return ctxt_new;
  }

void schemerlicht_context_destroy(schemerlicht_context* ctxt)
  {
  schemerlicht_assert(ctxt->global != NULL);
  schemerlicht_assert(ctxt != ctxt->global->main_context);
  context_free(ctxt);
  }
