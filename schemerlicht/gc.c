#include "gc.h"
#include "context.h"
#include "environment.h"
#include "error.h"

#include <time.h>

typedef struct gc_state
  {  
  schemerlicht_memsize gc_heap_pos;
  schemerlicht_object* source_heap;
  schemerlicht_object * target_heap;
  schemerlicht_memsize heap_size;
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
  return ctxt->heap_pos > ctxt->gc_heap_pos_threshold;
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
    case schemerlicht_object_type_port:
    case schemerlicht_object_type_promise:
    case schemerlicht_object_type_symbol:
    case schemerlicht_object_type_pair:
      return 0;
    default:
      return 1;
    }
  }

static void mark_object_pointer(schemerlicht_object* obj)
  {
  schemerlicht_assert(is_value_object(obj) == 0);
  schemerlicht_assert(MINSIZEVECTOR > 0); // this asserts that each vector has at least one object
  switch (obj->type)
    {
    case schemerlicht_object_type_vector:
    case schemerlicht_object_type_closure:
    case schemerlicht_object_type_port:
    case schemerlicht_object_type_promise:
    case schemerlicht_object_type_pair:
    {
      schemerlicht_object* first_arg = cast(schemerlicht_object*, obj->value.v.vector_ptr);
      if (first_arg->type & schemerlicht_int_gcmark_bit) // invalid type, possible if vector is empty and thus previously unassigned
        {
        schemerlicht_assert(obj->value.v.vector_size == 0);
        first_arg->type = 0;
        }
      first_arg->type |= schemerlicht_int_gcmark_bit; // mark by setting sign bit of type
      break;
    }
    case schemerlicht_object_type_string:
    case schemerlicht_object_type_symbol:
      obj->value.s.string_ptr[obj->value.s.string_length] = 1; // abuse invariable: strings end with 0 character
      break;
    default:
      schemerlicht_assert(0);
    }
  }

static void unmark_object_pointer(schemerlicht_object* obj)
  {
  schemerlicht_assert(is_value_object(obj) == 0);
  schemerlicht_assert(MINSIZEVECTOR > 0); // this asserts that each vector has at least one object
  switch (obj->type)
    {
    case schemerlicht_object_type_vector:
    case schemerlicht_object_type_closure:
    case schemerlicht_object_type_port:
    case schemerlicht_object_type_promise:
    case schemerlicht_object_type_pair:
    {
    schemerlicht_object* first_arg = cast(schemerlicht_object*, obj->value.v.vector_ptr);
    first_arg->type &= ~schemerlicht_int_gcmark_bit; // unmark by flipping sign bit of type
    break;
    }
    case schemerlicht_object_type_string:
    case schemerlicht_object_type_symbol:
      obj->value.s.string_ptr[obj->value.s.string_length] = 0; // abuse invariable: strings end with 0 character
      break;
    default:
      schemerlicht_assert(0);
    }
  }

static int is_marked(schemerlicht_object* obj)
  {
  schemerlicht_assert(MINSIZEVECTOR > 0); // this asserts that each vector has at least one object
  switch (obj->type)
    {
    case schemerlicht_object_type_vector:
    case schemerlicht_object_type_closure:
    case schemerlicht_object_type_port:
    case schemerlicht_object_type_promise:
    case schemerlicht_object_type_pair:
    {
    schemerlicht_object* first_arg = cast(schemerlicht_object*, obj->value.v.vector_ptr);
    return (first_arg->type & schemerlicht_int_gcmark_bit);
    }
    case schemerlicht_object_type_string:
    case schemerlicht_object_type_symbol:
      return obj->value.s.string_ptr[obj->value.s.string_length]; // abuse invariable: strings end with 0 character
    default:     
      return 0;
    }
  }

// returns position in the new heap of the object
static schemerlicht_memsize collect_object(schemerlicht_context* ctxt, schemerlicht_object* obj, gc_state* state)
  {
  schemerlicht_assert(is_value_object(obj) == 0);  
  if (is_marked(obj))
    {
    return schemerlicht_mem_invalid_size;
    }
  else
    {
    mark_object_pointer(obj);
    schemerlicht_memsize ret = state->gc_heap_pos;
    ++(state->gc_heap_pos);
    state->target_heap[ret] = *obj;
    return ret;
    }  
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
      if (is_value_object(obj))
        {
        entry.position = state->gc_heap_pos;
        state->target_heap[entry.position] = *obj;
        ++(state->gc_heap_pos);
        }
      else
        {
        if (obj->type == schemerlicht_object_type_unassigned)
          {
          printf("debug");
          }
        entry.position = collect_object(ctxt, obj, state);
        }
      schemerlicht_assert(entry.position != schemerlicht_mem_invalid_size);
      schemerlicht_environment_update(ctxt, &name, entry);
      }
    }
  }

static void scan_target_space(schemerlicht_context* ctxt, gc_state* state)
  {
  for (schemerlicht_memsize i = 0; i < state->heap_size; ++i)
    {
    schemerlicht_object* obj = state->target_heap + i;
    if (obj->type == schemerlicht_object_type_blocking)
      break;
    switch (obj->type)
      {
      case schemerlicht_object_type_vector:
      case schemerlicht_object_type_pair:
      case schemerlicht_object_type_closure:
      case schemerlicht_object_type_port:
      case schemerlicht_object_type_promise:
      {
      schemerlicht_object* it = schemerlicht_vector_begin(&obj->value.v, schemerlicht_object);
      schemerlicht_object* it_end = schemerlicht_vector_end(&obj->value.v, schemerlicht_object);
      for (; it != it_end; ++it)
        {
        if (is_value_object(it) == 0)
          collect_object(ctxt, it, state);
        }
      break;
      }
      }
    }
  }

void schemerlicht_collect_garbage(schemerlicht_context* ctxt)
  {
  int c0 = clock();
  schemerlicht_assert(ctxt->heap_pos < ctxt->raw_heap.vector_size / 2);
  gc_state state;
  state.gc_heap_pos = 0;
  state.source_heap = get_active_heap(ctxt);
  state.target_heap = get_inactive_heap(ctxt);
  state.heap_size = get_heap_size(ctxt);
  sweep_environment(ctxt, &state);
  sweep_stack(ctxt, &state);
  scan_target_space(ctxt, &state);
  for (schemerlicht_memsize i = 0; i < state.heap_size; ++i)
    {
    schemerlicht_object* obj = state.source_heap + i;
    if (obj->type == schemerlicht_object_type_blocking)
      break;
    if (is_marked(obj))
      {
      unmark_object_pointer(obj);
      }
    else
      {
      schemerlicht_object_destroy(ctxt, state.source_heap + i);
      }
    obj->type = schemerlicht_object_type_blocking;    
    }
  ctxt->heap = state.target_heap;
  ctxt->heap_pos = state.gc_heap_pos;
  int c1 = clock();
  ctxt->time_spent_gc += c1 - c0;
  if (schemerlicht_need_to_perform_gc(ctxt))
    {
    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_MEMORY);
    }
  }