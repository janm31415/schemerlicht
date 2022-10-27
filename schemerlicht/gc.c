#include "gc.h"
#include "context.h"

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
  return ctxt->raw_heap.vector_size/2;
  }

static schemerlicht_object* get_inactive_heap(schemerlicht_context* ctxt)
  {
  return cast(schemerlicht_object*, ctxt->raw_heap.vector_ptr) + active_heap(ctxt)*get_heap_size(ctxt);
  }

int schemerlicht_need_to_perform_gc(schemerlicht_context* ctxt)
  {
  return ctxt->heap_pos > 8*get_heap_size(ctxt)/10;
  }

void schemerlicht_check_garbage_collection(schemerlicht_context* ctxt)
  {
  if (schemerlicht_need_to_perform_gc(ctxt))
    schemerlicht_collect_garbage(ctxt);
  }

void schemerlicht_collect_garbage(schemerlicht_context* ctxt)
  {
  }