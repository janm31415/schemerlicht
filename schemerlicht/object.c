#include "object.h"
#include <string.h>
#include <stdio.h>

int schemerlicht_objects_equal(const schemerlicht_object* obj1, const schemerlicht_object* obj2)
  {
  if (obj1->type != obj2->type)
    return 0;
  switch (obj1->type)
    {
    case schemerlicht_object_type_undefined:
    case schemerlicht_object_type_true:
    case schemerlicht_object_type_false:
    case schemerlicht_object_type_nil:
    case schemerlicht_object_type_void:
      return 1;
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
      return obj1->value.ptr == obj2->value.ptr;
    default:
      return obj1->value.v.vector_ptr == obj2->value.v.vector_ptr;
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

void schemerlicht_object_destroy(schemerlicht_context* ctxt, schemerlicht_object* obj)
  {
  switch (obj->type)
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
    schemerlicht_vector_destroy(ctxt, &(obj->value.v));
    break;
    }
    case schemerlicht_object_type_pair:
    {
    schemerlicht_vector_destroy(ctxt, &(obj->value.v));
    break;
    }
    case schemerlicht_object_type_closure:
    {
    schemerlicht_vector_destroy(ctxt, &(obj->value.v));
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

schemerlicht_string schemerlicht_object_to_string(schemerlicht_context* ctxt, schemerlicht_object* input_obj)
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
      switch (current_task.obj->type)
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
          schemerlicht_string_append_cstr(ctxt, &s, "<lambda>");
          break;
        case schemerlicht_object_type_lambda:
          schemerlicht_string_append_cstr(ctxt, &s, "<lambda>");
          break;
        case schemerlicht_object_type_void:
          schemerlicht_string_append_cstr(ctxt, &s, "#<void>");
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
          schemerlicht_string_append_cstr(ctxt, &s, "\"");
          schemerlicht_string_append(ctxt, &s, &current_task.obj->value.s);
          schemerlicht_string_push_back(ctxt, &s, '"');
          break;
        case schemerlicht_object_type_symbol:
          schemerlicht_string_append(ctxt, &s, &current_task.obj->value.s);
          break;
        case schemerlicht_object_type_char:
        {
        schemerlicht_string_append_cstr(ctxt, &s, "#\\");
        if (current_task.obj->value.ch > 31 && current_task.obj->value.ch < 127)
          {
          schemerlicht_string_push_back(ctxt, &s, current_task.obj->value.ch);
          }
        else
          {
          int v = (int)current_task.obj->value.ch;
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
        if (second->type != schemerlicht_object_type_nil)
          {
          schemerlicht_runtime_task task = make_object_task(second);
          task.second_item_of_pair = 1;
          schemerlicht_vector_push_back(ctxt, &tasks, task, schemerlicht_runtime_task);
          if (second->type != schemerlicht_object_type_pair)
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