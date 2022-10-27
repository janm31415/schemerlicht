#include "gc.h"
#include "context.h"
#include "environment.h"
#include "map.h"

typedef struct gc_state
  {
  schemerlicht_map* gc_objects_treated;
  schemerlicht_memsize gc_heap_pos;
  } gc_state;

// returns 0 if active heap is first part of raw heap, 1 otherwise
static int active_heap(schemerlicht_context* ctxt)
  {
  return (ctxt->heap == ctxt->raw_heap.vector_ptr) ? 0 : 1;
  }

static schemerlicht_object* get_active_heap(schemerlicht_context* ctxt)
  {
  return ctxt->heap;
  }

static schemerlicht_memsize get_heap_size(schemerlicht_context* ctxt)
  {
  return ctxt->raw_heap.vector_size / 2;
  }

static schemerlicht_object* get_inactive_heap(schemerlicht_context* ctxt)
  {
  return cast(schemerlicht_object*, ctxt->raw_heap.vector_ptr) + (1 - active_heap(ctxt)) * get_heap_size(ctxt);
  }

int schemerlicht_need_to_perform_gc(schemerlicht_context* ctxt)
  {
  return ctxt->heap_pos > 8 * get_heap_size(ctxt) / 10;
  }

void schemerlicht_check_garbage_collection(schemerlicht_context* ctxt)
  {
  if (schemerlicht_need_to_perform_gc(ctxt))
    schemerlicht_collect_garbage(ctxt);
  }

static int is_value_object(schemerlicht_object* obj)
  {
  switch (obj->type)
    {
    case schemerlicht_object_type_vector:
    case schemerlicht_object_type_string:
    case schemerlicht_object_type_closure:
    case schemerlicht_object_type_symbol:
    case schemerlicht_object_type_pair:
    case schemerlicht_object_type_lambda:
      return 0;
    default:
      return 1;
    }
  }

static void* get_object_pointer(schemerlicht_object* obj)
  {
  switch (obj->type)
    {
    case schemerlicht_object_type_vector:
      return obj->value.v.vector_ptr;
    case schemerlicht_object_type_string:
      return cast(void*, obj->value.s.string_ptr);
    case schemerlicht_object_type_closure:
      return obj->value.v.vector_ptr;
    case schemerlicht_object_type_symbol:
      return cast(void*, obj->value.s.string_ptr);
    case schemerlicht_object_type_pair:
      return obj->value.v.vector_ptr;
    case schemerlicht_object_type_lambda:
      return obj->value.ptr;
    default:
      return NULL;
    }
  }

// returns position in the new heap of the object
static schemerlicht_memsize collect_object(schemerlicht_context* ctxt, schemerlicht_object* obj, gc_state* state)
  {
  schemerlicht_memsize ret = 0;
  schemerlicht_object* target_heap = get_inactive_heap(ctxt);
  if (is_value_object(obj))
    {
    ret = state->gc_heap_pos;
    target_heap[ret] = *obj;
    ++(state->gc_heap_pos);
    }
  else
    {
    void* ptr = get_object_pointer(obj);
    schemerlicht_object key;
    key.type = schemerlicht_object_type_lambda; // hack for working with pointers
    key.value.ptr = ptr;
    schemerlicht_object* value = schemerlicht_map_get(state->gc_objects_treated, &key);
    if (value != NULL)
      {
      schemerlicht_assert(value->type == schemerlicht_object_type_fixnum);
      ret = cast(schemerlicht_memsize, value->value.fx);
      }
    else
      {
      value = schemerlicht_map_insert(ctxt, state->gc_objects_treated, &key);
      value->type = schemerlicht_object_type_fixnum;
      ret = state->gc_heap_pos;
      value->value.fx = cast(schemerlicht_fixnum, ret);
      ++(state->gc_heap_pos);
      target_heap[ret] = *obj;
      }
    }
  return ret;
  }

static void sweep_stack(schemerlicht_context* ctxt, gc_state* state)
  {
  schemerlicht_object* it = schemerlicht_vector_begin(&ctxt->stack, schemerlicht_object);
  schemerlicht_object* it_end = schemerlicht_vector_end(&ctxt->stack, schemerlicht_object);
  for (; it != it_end; ++it)
    {
    if (it->type == schemerlicht_object_type_blocking)
      break;
    if (is_value_object(it) == 0)
      collect_object(ctxt, it, state);
    }
  }

static void sweep_environment(schemerlicht_context* ctxt, gc_state* state)
  {
  const schemerlicht_memsize sz = schemerlicht_environment_base_size(ctxt);
  for (schemerlicht_memsize i = 0; i < sz; ++i)
    {
    schemerlicht_environment_entry entry;
    schemerlicht_string name;
    if (schemerlicht_environment_base_at(&entry, &name, ctxt, i))
      {
      schemerlicht_assert(entry.type == SCHEMERLICHT_ENV_TYPE_GLOBAL);
      schemerlicht_object* obj = ctxt->heap + entry.position;
      entry.position = collect_object(ctxt, obj, state);
      schemerlicht_environment_update(ctxt, &name, entry);
      }
    }
  }

static void scan_target_space(schemerlicht_context* ctxt, gc_state* state)
  {
  schemerlicht_object* target_heap = get_inactive_heap(ctxt);
  const schemerlicht_memsize heap_size = get_heap_size(ctxt);
  for (schemerlicht_memsize i = 0; i < heap_size; ++i)
    {
    schemerlicht_object* obj = target_heap + i;
    switch (obj->type)
      {
      case schemerlicht_object_type_vector:
      case schemerlicht_object_type_pair:
      case schemerlicht_object_type_closure:
      {
      schemerlicht_object* it = schemerlicht_vector_begin(&obj->value.v, schemerlicht_object);
      schemerlicht_object* it_end = schemerlicht_vector_end(&obj->value.v, schemerlicht_object);
      for (; it != it_end; ++it)
        {
        collect_object(ctxt, it, state);
        }
      break;
      }
      }
    if (obj->type == schemerlicht_object_type_blocking)
      break;
    }
  }

void schemerlicht_collect_garbage(schemerlicht_context* ctxt)
  {
#if 0
  schemerlicht_object* source_heap = get_active_heap(ctxt);
  schemerlicht_assert(ctxt->heap == source_heap);  
  schemerlicht_object* target_heap = get_inactive_heap(ctxt);
  schemerlicht_assert((abs((int)source_heap - (int)target_heap)) == ctxt->raw_heap.vector_size/2*sizeof(schemerlicht_object));
  const schemerlicht_memsize heap_size = get_heap_size(ctxt);
  schemerlicht_assert(target_heap->type == schemerlicht_object_type_blocking);
  for (schemerlicht_memsize i = 0; i < heap_size; ++i)
    {
    *(target_heap + i) = *(source_heap + i);
    source_heap[i].type = schemerlicht_object_type_blocking;
    }
  ctxt->heap = target_heap;
  target_heap = get_inactive_heap(ctxt);
  schemerlicht_assert(target_heap->type == schemerlicht_object_type_blocking);
#else
  schemerlicht_assert(ctxt->heap_pos < ctxt->raw_heap.vector_size/2);
  gc_state state;
  state.gc_heap_pos = 0;
  state.gc_objects_treated = schemerlicht_map_new(ctxt, 0, 4);
  sweep_environment(ctxt, &state);
  sweep_stack(ctxt, &state);
  scan_target_space(ctxt, &state);
  schemerlicht_object* source_heap = get_active_heap(ctxt);
  schemerlicht_object* target_heap = get_inactive_heap(ctxt);
  const schemerlicht_memsize heap_size = get_heap_size(ctxt);
  for (schemerlicht_memsize i = 0; i < heap_size; ++i)
    {
    void* ptr = get_object_pointer(source_heap + i);
    if (ptr)
      {
      schemerlicht_object key;
      key.type = schemerlicht_object_type_lambda; // hack for working with pointers
      key.value.ptr = ptr;
      schemerlicht_object* value = schemerlicht_map_get(state.gc_objects_treated, &key);
      if (value == NULL)
        {
        schemerlicht_object_destroy(ctxt, source_heap + i);
        }
      }
    source_heap[i].type = schemerlicht_object_type_blocking;
    }
  ctxt->heap = target_heap;
  ctxt->heap_pos = state.gc_heap_pos;
  schemerlicht_map_free(ctxt, state.gc_objects_treated);
#endif
  }