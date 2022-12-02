#include "object.h"
#include "pool.h"
#include "context.h"
#include <string.h>
#include <stdio.h>

int schemerlicht_objects_eq(const schemerlicht_object* obj1, const schemerlicht_object* obj2)
  {
  if (schemerlicht_object_get_type(obj1) != schemerlicht_object_get_type(obj2))
    return 0;
  switch (schemerlicht_object_get_type(obj1))
    {
    case schemerlicht_object_type_undefined:
    case schemerlicht_object_type_true:
    case schemerlicht_object_type_false:
    case schemerlicht_object_type_nil:
    case schemerlicht_object_type_void:
    case schemerlicht_object_type_eof:
    case schemerlicht_object_type_blocking:
      return 1;
    case schemerlicht_object_type_unassigned:
    case schemerlicht_object_type_primitive:
    case schemerlicht_object_type_primitive_object:
    case schemerlicht_object_type_fixnum:
      return obj1->value.fx == obj2->value.fx;
    case schemerlicht_object_type_flonum:
      return obj1->value.fl == obj2->value.fl;
    case schemerlicht_object_type_char:
      return obj1->value.ch == obj2->value.ch;
    case schemerlicht_object_type_symbol:
    case schemerlicht_object_type_string:
      return obj1->value.s.string_ptr == obj2->value.s.string_ptr ? 1 : 0;
    case schemerlicht_object_type_lambda:
    case schemerlicht_object_type_environment:
      return obj1->value.ptr == obj2->value.ptr;
    default:
      return obj1->value.v.vector_ptr == obj2->value.v.vector_ptr;
    }
  }

int schemerlicht_objects_eqv(const schemerlicht_object* obj1, const schemerlicht_object* obj2)
  {
  return schemerlicht_objects_eq(obj1, obj2);
  }


static int schemerlicht_objects_equal_recursive(schemerlicht_context* ctxt, const schemerlicht_object* obj1, const schemerlicht_object* obj2)
  {
#if 0
  schemerlicht_string s1, s2;
  s1 = schemerlicht_object_to_string(ctxt, obj1, 0);
  s2 = schemerlicht_object_to_string(ctxt, obj2, 0);
  printf("%s\n", s1.string_ptr);
  printf("%s\n", s2.string_ptr);
  schemerlicht_string_destroy(ctxt, &s1);
  schemerlicht_string_destroy(ctxt, &s2);
#endif
  schemerlicht_assert(schemerlicht_object_get_type(obj1) == schemerlicht_object_get_type(obj2));
  schemerlicht_assert(obj1->value.v.vector_size == obj2->value.v.vector_size);
  schemerlicht_vector left_queue, right_queue;
  schemerlicht_vector_init(ctxt, &left_queue, schemerlicht_object);
  schemerlicht_vector_init(ctxt, &right_queue, schemerlicht_object);
  schemerlicht_object* it = schemerlicht_vector_begin(&obj1->value.v, schemerlicht_object);
  schemerlicht_object* it_end = schemerlicht_vector_end(&obj1->value.v, schemerlicht_object);
  schemerlicht_object* insertion_pos = schemerlicht_vector_begin(&left_queue, schemerlicht_object);
  schemerlicht_vector_insert(ctxt, &left_queue, &insertion_pos, &it, &it_end, schemerlicht_object);
  it = schemerlicht_vector_begin(&obj2->value.v, schemerlicht_object);
  it_end = schemerlicht_vector_end(&obj2->value.v, schemerlicht_object);
  insertion_pos = schemerlicht_vector_begin(&right_queue, schemerlicht_object);
  schemerlicht_vector_insert(ctxt, &right_queue, &insertion_pos, &it, &it_end, schemerlicht_object);

  while (left_queue.vector_size > 0)
    {
    schemerlicht_object* o1 = schemerlicht_vector_back(&left_queue, schemerlicht_object);
    schemerlicht_object* o2 = schemerlicht_vector_back(&right_queue, schemerlicht_object);
    schemerlicht_vector_pop_back(&left_queue);
    schemerlicht_vector_pop_back(&right_queue);
    if (schemerlicht_object_get_type(o1) != schemerlicht_object_get_type(o2))
      {
      schemerlicht_vector_destroy(ctxt, &left_queue);
      schemerlicht_vector_destroy(ctxt, &right_queue);
      return 0;
      }
    switch (schemerlicht_object_get_type(o1))
      {
      case schemerlicht_object_type_undefined:
      case schemerlicht_object_type_true:
      case schemerlicht_object_type_false:
      case schemerlicht_object_type_nil:
      case schemerlicht_object_type_void:
      case schemerlicht_object_type_eof:
      case schemerlicht_object_type_blocking:
        break;
      case schemerlicht_object_type_unassigned:
      case schemerlicht_object_type_primitive:
      case schemerlicht_object_type_primitive_object:
      case schemerlicht_object_type_fixnum:
        if (o1->value.fx != o2->value.fx)
          {
          schemerlicht_vector_destroy(ctxt, &left_queue);
          schemerlicht_vector_destroy(ctxt, &right_queue);
          return 0;
          }
        break;
      case schemerlicht_object_type_flonum:
        if (o1->value.fl != o2->value.fl)
          {
          schemerlicht_vector_destroy(ctxt, &left_queue);
          schemerlicht_vector_destroy(ctxt, &right_queue);
          return 0;
          }
        break;
      case schemerlicht_object_type_char:
        if (o1->value.ch != o2->value.ch)
          {
          schemerlicht_vector_destroy(ctxt, &left_queue);
          schemerlicht_vector_destroy(ctxt, &right_queue);
          return 0;
          }
        break;
      case schemerlicht_object_type_symbol:
      case schemerlicht_object_type_string:
        if (strcmp(o1->value.s.string_ptr, o2->value.s.string_ptr) != 0)
          {
          schemerlicht_vector_destroy(ctxt, &left_queue);
          schemerlicht_vector_destroy(ctxt, &right_queue);
          return 0;
          }
        break;
      case schemerlicht_object_type_lambda:
      case schemerlicht_object_type_environment:
        if (o1->value.ptr != o2->value.ptr)
          {
          schemerlicht_vector_destroy(ctxt, &left_queue);
          schemerlicht_vector_destroy(ctxt, &right_queue);
          return 0;
          }
        break;
      default:
        if (o1->value.v.vector_size != o2->value.v.vector_size)
          {
          schemerlicht_vector_destroy(ctxt, &left_queue);
          schemerlicht_vector_destroy(ctxt, &right_queue);
          return 0;
          }
        it = schemerlicht_vector_begin(&o1->value.v, schemerlicht_object);
        it_end = schemerlicht_vector_end(&o1->value.v, schemerlicht_object);
        insertion_pos = schemerlicht_vector_begin(&left_queue, schemerlicht_object);
        schemerlicht_vector_insert(ctxt, &left_queue, &insertion_pos, &it, &it_end, schemerlicht_object);
        it = schemerlicht_vector_begin(&o2->value.v, schemerlicht_object);
        it_end = schemerlicht_vector_end(&o2->value.v, schemerlicht_object);
        insertion_pos = schemerlicht_vector_begin(&right_queue, schemerlicht_object);
        schemerlicht_vector_insert(ctxt, &right_queue, &insertion_pos, &it, &it_end, schemerlicht_object);
        break;
      }
    }

  schemerlicht_vector_destroy(ctxt, &left_queue);
  schemerlicht_vector_destroy(ctxt, &right_queue);
  return 1;
  }

int schemerlicht_objects_equal(schemerlicht_context* ctxt, const schemerlicht_object* obj1, const schemerlicht_object* obj2)
  {
  if (schemerlicht_object_get_type(obj1) != schemerlicht_object_get_type(obj2))
    return 0;
  switch (schemerlicht_object_get_type(obj1))
    {
    case schemerlicht_object_type_undefined:
    case schemerlicht_object_type_true:
    case schemerlicht_object_type_false:
    case schemerlicht_object_type_nil:
    case schemerlicht_object_type_void:
    case schemerlicht_object_type_eof:
    case schemerlicht_object_type_blocking:
      return 1;
    case schemerlicht_object_type_unassigned:
    case schemerlicht_object_type_primitive:
    case schemerlicht_object_type_primitive_object:
    case schemerlicht_object_type_fixnum:
      return obj1->value.fx == obj2->value.fx;
    case schemerlicht_object_type_flonum:
      return obj1->value.fl == obj2->value.fl;
    case schemerlicht_object_type_char:
      return obj1->value.ch == obj2->value.ch;
    case schemerlicht_object_type_symbol:
    case schemerlicht_object_type_string:
      return strcmp(obj1->value.s.string_ptr, obj2->value.s.string_ptr) == 0 ? 1 : 0;
    case schemerlicht_object_type_lambda:
    case schemerlicht_object_type_environment:
      return obj1->value.ptr == obj2->value.ptr;
    default:
      if (obj1->value.v.vector_size != obj2->value.v.vector_size)
        return 0;
      return schemerlicht_objects_equal_recursive(ctxt, obj1, obj2);
    }
  }

int schemerlicht_log2(uint32_t x)
  {
  static const schemerlicht_byte log_8[255] = {
    0,
    1,1,
    2,2,2,2,
    3,3,3,3,3,3,3,3,
    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
    };
  if (x >= 0x00010000)
    {
    if (x >= 0x01000000)
      return log_8[((x >> 24) & 0xff) - 1] + 24;
    else
      return log_8[((x >> 16) & 0xff) - 1] + 16;
    }
  else {
    if (x >= 0x00000100)
      return log_8[((x >> 8) & 0xff) - 1] + 8;
    else if (x)
      return log_8[(x & 0xff) - 1];
    return -1;  /* special `log' for 0 */
    }
  }

schemerlicht_object make_schemerlicht_object_fixnum(schemerlicht_fixnum fx)
  {
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_fixnum;
  obj.value.fx = fx;
  return obj;
  }

schemerlicht_object make_schemerlicht_object_flonum(schemerlicht_flonum fl)
  {
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_flonum;
  obj.value.fl = fl;
  return obj;
  }

schemerlicht_object make_schemerlicht_object_true()
  {
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_true;
  return obj;
  }

schemerlicht_object make_schemerlicht_object_false()
  {
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_false;
  return obj;
  }

schemerlicht_object make_schemerlicht_object_nil()
  {
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_nil;
  return obj;
  }

schemerlicht_object make_schemerlicht_object_char(schemerlicht_byte ch)
  {
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_char;
  obj.value.ch = ch;
  return obj;
  }

schemerlicht_object make_schemerlicht_object_string(schemerlicht_context* ctxt, const char* s)
  {
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_string;
  schemerlicht_string_init(ctxt, &(obj.value.s), s);
  return obj;
  }

schemerlicht_object make_schemerlicht_object_symbol(schemerlicht_context* ctxt, const char* s)
  {
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_symbol;
  schemerlicht_string_init(ctxt, &(obj.value.s), s);
  return obj;
  }

schemerlicht_object make_schemerlicht_object_pair(schemerlicht_context* ctxt)
  {
#ifdef SCHEMERLICHT_USE_POOL
  schemerlicht_assert(SCHEMERLICHT_MAX_POOL > 1);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_pair;
  obj.value.v.vector_capacity = 2;
  obj.value.v.vector_size = 2;
  obj.value.v.element_size = sizeof(schemerlicht_object);
  obj.value.v.vector_ptr = schemerlicht_pool_allocate(ctxt, &ctxt->pool[1]);
#else
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_pair;
  schemerlicht_vector_init_with_size(ctxt, &obj.value.v, 2, schemerlicht_object);
#endif
  return obj;
  }

schemerlicht_object make_schemerlicht_object_closure(schemerlicht_context* ctxt, int closure_size)
  {
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_closure;
#ifdef SCHEMERLICHT_USE_POOL
  if (closure_size <= SCHEMERLICHT_MAX_POOL)
    {
    obj.value.v.vector_capacity = closure_size;
    obj.value.v.vector_size = closure_size;
    obj.value.v.element_size = sizeof(schemerlicht_object);
    obj.value.v.vector_ptr = schemerlicht_pool_allocate(ctxt, &ctxt->pool[closure_size - 1]);
    }
  else
    {
    schemerlicht_vector_init_with_size(ctxt, &obj.value.v, closure_size, schemerlicht_object);
    }
#else
  schemerlicht_vector_init_with_size(ctxt, &obj.value.v, closure_size, schemerlicht_object);
#endif
  return obj;
  }

schemerlicht_object make_schemerlicht_object_port(schemerlicht_context* ctxt)
  {
#ifdef SCHEMERLICHT_USE_POOL
  schemerlicht_assert(SCHEMERLICHT_MAX_POOL > 6);
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_port;
  obj.value.v.vector_capacity = 7;
  obj.value.v.vector_size = 7;
  obj.value.v.element_size = sizeof(schemerlicht_object);
  obj.value.v.vector_ptr = schemerlicht_pool_allocate(ctxt, &ctxt->pool[6]);
#else
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_port;
  schemerlicht_vector_init_with_size(ctxt, &obj.value.v, 7, schemerlicht_object);
#endif
  return obj;
  }

schemerlicht_object make_schemerlicht_object_promise(schemerlicht_context* ctxt)
  {
#ifdef SCHEMERLICHT_USE_POOL
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_promise;
  obj.value.v.vector_capacity = 1;
  obj.value.v.vector_size = 1;
  obj.value.v.element_size = sizeof(schemerlicht_object);
  obj.value.v.vector_ptr = schemerlicht_pool_allocate(ctxt, &ctxt->pool[0]);
#else
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_promise;
  schemerlicht_vector_init_with_size(ctxt, &obj.value.v, 1, schemerlicht_object);
#endif
  return obj;
  }

schemerlicht_object make_schemerlicht_object_vector(schemerlicht_context* ctxt, int vector_size)
  {
  schemerlicht_object obj;
  obj.type = schemerlicht_object_type_vector;
#ifdef SCHEMERLICHT_USE_POOL
  if (vector_size == 0)
    {
    obj.value.v.vector_capacity = 1;
    obj.value.v.vector_size = 0;
    obj.value.v.element_size = sizeof(schemerlicht_object);
    obj.value.v.vector_ptr = schemerlicht_pool_allocate(ctxt, &ctxt->pool[0]);
    }
  else if (vector_size <= SCHEMERLICHT_MAX_POOL)
    {
    obj.value.v.vector_capacity = vector_size;
    obj.value.v.vector_size = vector_size;
    obj.value.v.element_size = sizeof(schemerlicht_object);
    obj.value.v.vector_ptr = schemerlicht_pool_allocate(ctxt, &ctxt->pool[vector_size - 1]);
    }
  else
    {
    schemerlicht_vector_init_with_size(ctxt, &obj.value.v, vector_size, schemerlicht_object);
    }
#else
  if (vector_size == 0)
    {
    schemerlicht_vector_init(ctxt, &obj.value.v, schemerlicht_object);
    }
  else
    {
    schemerlicht_vector_init_with_size(ctxt, &obj.value.v, vector_size, schemerlicht_object);
    }
#endif
  return obj;
  }

void schemerlicht_object_destroy(schemerlicht_context* ctxt, schemerlicht_object* obj)
  {
  switch (schemerlicht_object_get_type(obj))
    {
    case schemerlicht_object_type_string:
    {
    schemerlicht_string_destroy(ctxt, &(obj->value.s));
    break;
    }
    case schemerlicht_object_type_symbol:
    {
    schemerlicht_string_destroy(ctxt, &(obj->value.s));
    break;
    }
    case schemerlicht_object_type_vector:
    {
#ifdef SCHEMERLICHT_USE_POOL
    if (obj->value.v.vector_capacity > SCHEMERLICHT_MAX_POOL)
      {
      schemerlicht_vector_destroy(ctxt, &(obj->value.v));
      }
    else
      {
      schemerlicht_pool_deallocate(&ctxt->pool[obj->value.v.vector_capacity - 1], obj->value.v.vector_ptr);
      }
#else
    schemerlicht_vector_destroy(ctxt, &(obj->value.v));
#endif
    break;
    }
    case schemerlicht_object_type_port:
    {
#ifdef SCHEMERLICHT_USE_POOL
    schemerlicht_pool_deallocate(&ctxt->pool[6], obj->value.v.vector_ptr);
#else
    schemerlicht_vector_destroy(ctxt, &(obj->value.v));
#endif
    break;
    }
    case schemerlicht_object_type_promise:
    {
#ifdef SCHEMERLICHT_USE_POOL
    schemerlicht_pool_deallocate(&ctxt->pool[0], obj->value.v.vector_ptr);
#else
    schemerlicht_vector_destroy(ctxt, &(obj->value.v));
#endif
    break;
    }
    case schemerlicht_object_type_pair:
    {
#ifdef SCHEMERLICHT_USE_POOL
    schemerlicht_pool_deallocate(&ctxt->pool[1], obj->value.v.vector_ptr);
#else
    schemerlicht_vector_destroy(ctxt, &(obj->value.v));
#endif
    break;
    }
    case schemerlicht_object_type_closure:
    {
#ifdef SCHEMERLICHT_USE_POOL
    if (obj->value.v.vector_capacity > SCHEMERLICHT_MAX_POOL)
      {
      schemerlicht_vector_destroy(ctxt, &(obj->value.v));
      }
    else
      {
      schemerlicht_pool_deallocate(&ctxt->pool[obj->value.v.vector_capacity - 1], obj->value.v.vector_ptr);
      }
#else
    schemerlicht_vector_destroy(ctxt, &(obj->value.v));
#endif
    break;
    }
    default:
      break;
    }
  }

typedef struct schemerlicht_runtime_task
  {
  const char* text;
  schemerlicht_object* obj;
  int second_item_of_pair;
  } schemerlicht_runtime_task;

static schemerlicht_runtime_task make_empty_task()
  {
  schemerlicht_runtime_task task;
  task.text = 0;
  task.obj = 0;
  task.second_item_of_pair = 0;
  return task;
  }

static schemerlicht_runtime_task make_object_task(schemerlicht_object* obj)
  {
  schemerlicht_runtime_task task;
  task.text = 0;
  task.obj = obj;
  task.second_item_of_pair = 0;
  return task;
  }

static schemerlicht_runtime_task make_text_task(const char* txt)
  {
  schemerlicht_runtime_task task;
  task.text = txt;
  task.obj = 0;
  task.second_item_of_pair = 0;
  return task;
  }

schemerlicht_string schemerlicht_object_to_string(schemerlicht_context* ctxt, schemerlicht_object* input_obj, int display)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, "");
  schemerlicht_vector tasks;
  schemerlicht_vector_init(ctxt, &tasks, schemerlicht_runtime_task);
  schemerlicht_runtime_task first_task = make_object_task(input_obj);
  schemerlicht_vector_push_back(ctxt, &tasks, first_task, schemerlicht_runtime_task);
  while (tasks.vector_size > 0)
    {
    schemerlicht_runtime_task current_task = *schemerlicht_vector_back(&tasks, schemerlicht_runtime_task);
    schemerlicht_vector_pop_back(&tasks);
    if (current_task.text != 0)
      {
      schemerlicht_string_append_cstr(ctxt, &s, current_task.text);
      }
    else
      {
      switch (schemerlicht_object_get_type(current_task.obj))
        {
        case schemerlicht_object_type_undefined:
          schemerlicht_string_append_cstr(ctxt, &s, "#undefined");
          break;
        case schemerlicht_object_type_false:
          schemerlicht_string_append_cstr(ctxt, &s, "#f");
          break;
        case schemerlicht_object_type_true:
          schemerlicht_string_append_cstr(ctxt, &s, "#t");
          break;
        case schemerlicht_object_type_nil:
          schemerlicht_string_append_cstr(ctxt, &s, "()");
          break;
        case schemerlicht_object_type_closure:
          schemerlicht_string_append_cstr(ctxt, &s, "<closure>");
          break;
        case schemerlicht_object_type_lambda:
          schemerlicht_string_append_cstr(ctxt, &s, "<lambda>");
          break;
        case schemerlicht_object_type_environment:
          schemerlicht_string_append_cstr(ctxt, &s, "<environment>");
          break;
        case schemerlicht_object_type_eof:
          schemerlicht_string_append_cstr(ctxt, &s, "#eof");
          break;
        case schemerlicht_object_type_unassigned:
          schemerlicht_string_append_cstr(ctxt, &s, "#unassigned");
          break;
        case schemerlicht_object_type_void:
          schemerlicht_string_append_cstr(ctxt, &s, "#<void>");
          break;
        case schemerlicht_object_type_blocking:
          schemerlicht_string_append_cstr(ctxt, &s, "#blocking");
          break;
        case schemerlicht_object_type_primitive:
        case schemerlicht_object_type_primitive_object:
          schemerlicht_string_append_cstr(ctxt, &s, "<procedure>");
          break;
        case schemerlicht_object_type_fixnum:
        {
        char str[256];
        sprintf(str, "%lld", current_task.obj->value.fx);
        schemerlicht_string_append_cstr(ctxt, &s, str);
        break;
        }
        case schemerlicht_object_type_flonum:
        {
        char str[256];
        sprintf(str, "%f", current_task.obj->value.fl);
        schemerlicht_string_append_cstr(ctxt, &s, str);
        break;
        }
        case schemerlicht_object_type_string:
        {
        if (!display)
          {
          schemerlicht_string_append_cstr(ctxt, &s, "\"");
          schemerlicht_string tmp = schemerlicht_string_add_escape_chars(ctxt, &current_task.obj->value.s);
          schemerlicht_string_append(ctxt, &s, &tmp);
          schemerlicht_string_destroy(ctxt, &tmp);
          schemerlicht_string_push_back(ctxt, &s, '"');
          }
        else
          {
          schemerlicht_string_append(ctxt, &s, &current_task.obj->value.s);
          }
        break;
        }
        case schemerlicht_object_type_symbol:
          schemerlicht_string_append(ctxt, &s, &current_task.obj->value.s);
          break;
        case schemerlicht_object_type_char:
        {
        if (!display)
          schemerlicht_string_append_cstr(ctxt, &s, "#\\");
        int v = (int)current_task.obj->value.ch;
        int done = 0;
        if (display)
          {
          switch (v)
            {
            case 9:
              schemerlicht_string_append_cstr(ctxt, &s, "\t");
              done = 1;
              break;
            case 10:
              schemerlicht_string_append_cstr(ctxt, &s, "\n");
              done = 1;
              break;
            case 13:
              schemerlicht_string_append_cstr(ctxt, &s, "\r");
              done = 1;
              break;
            }
          }
        else
          {
          switch (v)
            {
            case 8:
              schemerlicht_string_append_cstr(ctxt, &s, "backspace");
              done = 1;
              break;
            case 9:
              schemerlicht_string_append_cstr(ctxt, &s, "tab");
              done = 1;
              break;
            case 10:
              schemerlicht_string_append_cstr(ctxt, &s, "newline");
              done = 1;
              break;
            case 11:
              schemerlicht_string_append_cstr(ctxt, &s, "vtab");
              done = 1;
              break;
            case 12:
              schemerlicht_string_append_cstr(ctxt, &s, "page");
              done = 1;
              break;
            case 13:
              schemerlicht_string_append_cstr(ctxt, &s, "return");
              done = 1;
              break;
            case 32:
              schemerlicht_string_append_cstr(ctxt, &s, "space");
              done = 1;
              break;
            }
          }
        if (done)
          break;
        if (current_task.obj->value.ch > 31 && current_task.obj->value.ch < 127)
          {
          schemerlicht_string_push_back(ctxt, &s, current_task.obj->value.ch);
          }
        else
          {
          char str[4];
          sprintf(str, "%d", v);
          str[3] = 0;
          if (str[1] == 0)
            {
            str[2] = str[0];
            str[1] = '0';
            str[0] = '0';
            }
          else if (str[2] == 0)
            {
            str[2] = str[1];
            str[1] = str[0];
            str[0] = '0';
            }
          schemerlicht_string_append_cstr(ctxt, &s, str);
          }
        break;
        }
        case schemerlicht_object_type_port:
        {
        schemerlicht_string_append_cstr(ctxt, &s, "<port>: ");
        schemerlicht_object* port_name = schemerlicht_vector_at(&current_task.obj->value.v, 1, schemerlicht_object);
        schemerlicht_runtime_task task = make_object_task(port_name);
        schemerlicht_vector_push_back(ctxt, &tasks, task, schemerlicht_runtime_task);
        break;
        }
        case schemerlicht_object_type_promise:
        {
        schemerlicht_string_append_cstr(ctxt, &s, "<promise>");
        break;
        }
        case schemerlicht_object_type_vector:
        {
        schemerlicht_string_append_cstr(ctxt, &s, "#(");
        schemerlicht_runtime_task task = make_text_task(")");
        schemerlicht_vector_push_back(ctxt, &tasks, task, schemerlicht_runtime_task);
        for (schemerlicht_memsize j = current_task.obj->value.v.vector_size; j > 0; --j)
          {
          if (j < current_task.obj->value.v.vector_size)
            {
            task = make_text_task(" ");
            schemerlicht_vector_push_back(ctxt, &tasks, task, schemerlicht_runtime_task);
            }
          schemerlicht_object* new_obj = schemerlicht_vector_at(&current_task.obj->value.v, j - 1, schemerlicht_object);
          task = make_object_task(new_obj);
          schemerlicht_vector_push_back(ctxt, &tasks, task, schemerlicht_runtime_task);
          }
        break;
        }
        case schemerlicht_object_type_pair:
        {
        //In general, the rule for printing a pair is as follows : use the dot notation always,
        //but if the dot is immediately followed by an open parenthesis, then remove the dot, the
        //open parenthesis, and the matching close parenthesis.Thus, (0 . (1 . 2)) becomes
        //(0 1 . 2), and (1 . (2 . (3 . ()))) becomes (1 2 3).
        schemerlicht_assert(current_task.obj->value.v.vector_size == 2);
        schemerlicht_object* first = schemerlicht_vector_at(&current_task.obj->value.v, 0, schemerlicht_object);
        schemerlicht_object* second = schemerlicht_vector_at(&current_task.obj->value.v, 1, schemerlicht_object);
        if (!current_task.second_item_of_pair)
          {
          schemerlicht_string_append_cstr(ctxt, &s, "(");
          schemerlicht_runtime_task task = make_text_task(")");
          schemerlicht_vector_push_back(ctxt, &tasks, task, schemerlicht_runtime_task);
          }
        if (schemerlicht_object_get_type(second) != schemerlicht_object_type_nil)
          {
          schemerlicht_runtime_task task = make_object_task(second);
          task.second_item_of_pair = 1;
          schemerlicht_vector_push_back(ctxt, &tasks, task, schemerlicht_runtime_task);
          if (schemerlicht_object_get_type(second) != schemerlicht_object_type_pair)
            {
            task = make_text_task(" . ");
            schemerlicht_vector_push_back(ctxt, &tasks, task, schemerlicht_runtime_task);
            }
          else
            {
            task = make_text_task(" ");
            schemerlicht_vector_push_back(ctxt, &tasks, task, schemerlicht_runtime_task);
            }
          }
        schemerlicht_runtime_task task = make_object_task(first);
        schemerlicht_vector_push_back(ctxt, &tasks, task, schemerlicht_runtime_task);

        break;
        }
        }
      }
    }

  schemerlicht_vector_destroy(ctxt, &tasks);
  return s;
  }


schemerlicht_object schemerlicht_object_deep_copy(schemerlicht_context* ctxt, schemerlicht_object* input_obj)
  {
  schemerlicht_object result;
  schemerlicht_object* last_result = NULL;
  schemerlicht_vector parent;
  schemerlicht_vector_init(ctxt, &parent, schemerlicht_object*);
  schemerlicht_vector todo;
  schemerlicht_vector_init(ctxt, &todo, schemerlicht_object);
  schemerlicht_vector_push_back(ctxt, &todo, *input_obj, schemerlicht_object);

  while (todo.vector_size > 0)
    {
    schemerlicht_object res;
    schemerlicht_object obj = *schemerlicht_vector_back(&todo, schemerlicht_object);
    schemerlicht_vector_pop_back(&todo);
    schemerlicht_memsize parents_to_add = 0;
    if (parent.vector_size > 0)
      {
      last_result = *schemerlicht_vector_back(&parent, schemerlicht_object*);
      schemerlicht_vector_pop_back(&parent);
      }
    switch (schemerlicht_object_get_type(&obj))
      {
      case schemerlicht_object_type_undefined:
      case schemerlicht_object_type_char:
      case schemerlicht_object_type_fixnum:
      case schemerlicht_object_type_flonum:
      case schemerlicht_object_type_true:
      case schemerlicht_object_type_false:
      case schemerlicht_object_type_nil:
      case schemerlicht_object_type_void:
      case schemerlicht_object_type_unassigned:
      case schemerlicht_object_type_lambda:
      case schemerlicht_object_type_environment:
      case schemerlicht_object_type_primitive:
      case schemerlicht_object_type_primitive_object:
      case schemerlicht_object_type_blocking:
      case schemerlicht_object_type_eof:
      default:
      {
      schemerlicht_set_object(&res, &obj);
      break;
      }
      case schemerlicht_object_type_symbol:
      {
      schemerlicht_object key;
      key.type = schemerlicht_object_type_string;
      key.value.s = obj.value.s;
      schemerlicht_object* symbol = schemerlicht_map_get(ctxt, ctxt->string_to_symbol, &key);
      if (symbol != NULL)
        {
        schemerlicht_set_object(&res, symbol);
        }
      else
        {
        schemerlicht_object key;
        key.type = schemerlicht_object_type_string;
        schemerlicht_string_copy(ctxt, &key.value.s, &obj.value.s);
        schemerlicht_object* new_symbol = schemerlicht_map_insert(ctxt, ctxt->string_to_symbol, &key);
        new_symbol->type = schemerlicht_object_type_symbol;
        schemerlicht_string_copy(ctxt, &new_symbol->value.s, &obj.value.s);
        schemerlicht_set_object(&res, new_symbol);
        }
      break;
      }
      case schemerlicht_object_type_string:
      {
      res.type = schemerlicht_object_type_string;
      schemerlicht_string_copy(ctxt, &res.value.s, &obj.value.s);
      schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
      schemerlicht_set_object(heap_obj, &res);
      ++ctxt->heap_pos;
      break;
      }
      case schemerlicht_object_type_closure:
      case schemerlicht_object_type_vector:
      case schemerlicht_object_type_pair:
      case schemerlicht_object_type_port:
      case schemerlicht_object_type_promise:
      {
      res = make_schemerlicht_object_vector(ctxt, obj.value.v.vector_size);
      res.type = schemerlicht_object_get_type(&obj);
      res.value.v.vector_size = 0; // we set the size to 0, so that we can push back without memory consequences
      schemerlicht_object* heap_obj = &ctxt->heap[ctxt->heap_pos];
      schemerlicht_set_object(heap_obj, &res);
      ++ctxt->heap_pos;
      schemerlicht_object* it = schemerlicht_vector_begin(&obj.value.v, schemerlicht_object);
      schemerlicht_object* it_end = schemerlicht_vector_end(&obj.value.v, schemerlicht_object);
      schemerlicht_object* rit = it_end - 1;
      schemerlicht_object* rit_end = it - 1;
      for (; rit != rit_end; --rit)
        {
        schemerlicht_vector_push_back(ctxt, &todo, *rit, schemerlicht_object);
        }
      parents_to_add = obj.value.v.vector_size;
      break;
      }
      }
    if (last_result)
      {
      schemerlicht_vector_push_back(ctxt, &last_result->value.v, res, schemerlicht_object);
      schemerlicht_object* add_to_parent = schemerlicht_vector_back(&last_result->value.v, schemerlicht_object);
      for (int i = 0; i < parents_to_add; ++i)
        {
        schemerlicht_vector_push_back(ctxt, &parent, add_to_parent, schemerlicht_object*);
        }
      }
    else
      {
      result = res;
      for (int i = 0; i < parents_to_add; ++i)
        {
        schemerlicht_vector_push_back(ctxt, &parent, &result, schemerlicht_object*);
        }
      }
    }

  schemerlicht_vector_destroy(ctxt, &parent);
  schemerlicht_vector_destroy(ctxt, &todo);
  return result;
  }
