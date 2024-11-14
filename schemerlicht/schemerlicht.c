#include "schemerlicht.h"
#include "context.h"
#include "limits.h"
#include "context.h"
#include "parser.h"
#include "token.h"
#include "preprocess.h"
#include "compiler.h"
#include "vm.h"
#include "callcc.h"
#include "r5rs.h"
#include "inputoutput.h"
#include "modules.h"
#include "modulesconf.h"
#include "error.h"
#include "environment.h"
#include "dump.h"
#include "string.h"
#include "syscalls.h"
#include "foreign.h"

#include <stdio.h>
#include <string.h>

void schemerlicht_build_base(schemerlicht_context* ctxt)
  {
  schemerlicht_compile_callcc(ctxt);
  schemerlicht_compile_r5rs(ctxt);
  schemerlicht_compile_input_output(ctxt);
  schemerlicht_compile_modules(ctxt, SCHEMERLICHT_MODULES_PATH);
  }

static schemerlicht_vector compile_scheme(schemerlicht_context* ctxt, schemerlicht_stream* str)
  {
  schemerlicht_syntax_errors_clear(ctxt);
  schemerlicht_compile_errors_clear(ctxt);
  schemerlicht_runtime_errors_clear(ctxt);
  schemerlicht_vector tokens = tokenize(ctxt, str);
  if (schemerlicht_context_is_error_free(ctxt) == 0)
    {
    schemerlicht_vector v;
    v.vector_size = 0;
    v.vector_capacity = 0;
    v.vector_ptr = NULL;
    v.element_size = 0;
    return v;
    }
  schemerlicht_program prog = make_program(ctxt, &tokens);
  if (schemerlicht_context_is_error_free(ctxt) == 0)
    {
    destroy_tokens_vector(ctxt, &tokens);
    schemerlicht_vector v;
    v.vector_size = 0;
    v.vector_capacity = 0;
    v.vector_ptr = NULL;
    v.element_size = 0;
    return v;
    }
  schemerlicht_preprocess(ctxt, &prog);
  if (schemerlicht_context_is_error_free(ctxt) == 0)
    {
    destroy_tokens_vector(ctxt, &tokens);
    schemerlicht_program_destroy(ctxt, &prog);
    schemerlicht_vector v;
    v.vector_size = 0;
    v.vector_capacity = 0;
    v.vector_ptr = NULL;
    v.element_size = 0;
    return v;
    }
  schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);  
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  return compiled_program;
  }

void schemerlicht_dump_compiled_program(schemerlicht_context* ctxt, schemerlicht_string* s, const char* script) {
  schemerlicht_stream str;
  schemerlicht_stream_init(ctxt, &str, 10);
  schemerlicht_memsize len = cast(schemerlicht_memsize, strlen(script));
  schemerlicht_stream_write(ctxt, &str, script, len, 0);
  schemerlicht_stream_rewind(&str);
  schemerlicht_syntax_errors_clear(ctxt);
  schemerlicht_compile_errors_clear(ctxt);
  schemerlicht_runtime_errors_clear(ctxt);
  schemerlicht_vector tokens = tokenize(ctxt, &str);
  if (schemerlicht_context_is_error_free(ctxt) == 0) {
    destroy_tokens_vector(ctxt, &tokens);
    schemerlicht_stream_close(ctxt, &str);
    return;
  }
  schemerlicht_program prog = make_program(ctxt, &tokens);
  if (schemerlicht_context_is_error_free(ctxt) == 0) {
    destroy_tokens_vector(ctxt, &tokens);
    schemerlicht_stream_close(ctxt, &str);
    schemerlicht_program_destroy(ctxt, &prog);
    return;
  }
  schemerlicht_preprocess(ctxt, &prog);
  if (schemerlicht_context_is_error_free(ctxt) == 0) {
    destroy_tokens_vector(ctxt, &tokens);
    schemerlicht_stream_close(ctxt, &str);
    schemerlicht_program_destroy(ctxt, &prog);
    return;
  }
  schemerlicht_string result = schemerlicht_dump(ctxt, &prog, 0);
  schemerlicht_string_append(ctxt, s, &result);
  schemerlicht_string_destroy(ctxt, &result);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_stream_close(ctxt, &str);
}

static schemerlicht_object* execute_scheme(schemerlicht_context* ctxt, schemerlicht_stream* str)
  {
  /*
  schemerlicht_syntax_errors_clear(ctxt);
  schemerlicht_compile_errors_clear(ctxt);
  schemerlicht_runtime_errors_clear(ctxt);
  schemerlicht_vector tokens = tokenize(ctxt, str);
  if (schemerlicht_context_is_error_free(ctxt) == 0)
    {
    return NULL;
    }
  schemerlicht_program prog = make_program(ctxt, &tokens);
  if (schemerlicht_context_is_error_free(ctxt) == 0)
    {
    destroy_tokens_vector(ctxt, &tokens);
    return NULL;
    }
  schemerlicht_preprocess(ctxt, &prog);
  if (schemerlicht_context_is_error_free(ctxt) == 0)
    {
    destroy_tokens_vector(ctxt, &tokens);
    schemerlicht_program_destroy(ctxt, &prog);
    return NULL;
    }
  schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);
  if (schemerlicht_context_is_error_free(ctxt) == 0)
    {
    destroy_tokens_vector(ctxt, &tokens);
    schemerlicht_program_destroy(ctxt, &prog);
    return NULL;
    }
  schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program);

  schemerlicht_compiled_program_register(ctxt, &compiled_program);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  return res;
  */
  schemerlicht_vector compiled_program = compile_scheme(ctxt, str);
  if (compiled_program.vector_size > 0)
    {
    schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program);
    schemerlicht_compiled_program_register(ctxt, &compiled_program);
    return res;
    }
  else
    return NULL;
  }

void schemerlicht_compile(schemerlicht_context* ctxt, schemerlicht_vector* compiled_program, const char* script)
  {
  schemerlicht_stream str;
  schemerlicht_stream_init(ctxt, &str, 10);
  schemerlicht_memsize len = cast(schemerlicht_memsize, strlen(script));
  schemerlicht_stream_write(ctxt, &str, script, len, 0);
  schemerlicht_stream_rewind(&str);
  schemerlicht_vector compiled = compile_scheme(ctxt, &str);
  schemerlicht_stream_close(ctxt, &str);
  schemerlicht_function** it = schemerlicht_vector_begin(&compiled, schemerlicht_function*);
  schemerlicht_function** it_end = schemerlicht_vector_end(&compiled, schemerlicht_function*);
  schemerlicht_function** where_it = schemerlicht_vector_end(compiled_program, schemerlicht_function*);
  schemerlicht_vector_insert(ctxt, compiled_program, &where_it, &it, &it_end, schemerlicht_function*);
  schemerlicht_vector_destroy(ctxt, &compiled);
  }

void schemerlicht_compile_file(schemerlicht_context* ctxt, schemerlicht_vector* compiled_program, const char* filename)
  {
  FILE* f = fopen(filename, "r");
  if (f)
    {
    schemerlicht_stream str;
    schemerlicht_stream_init(ctxt, &str, 256);
    char buffer[256];
    size_t bytes_read = fread(buffer, 1, 256, f);
    while (bytes_read)
      {
      schemerlicht_stream_write(ctxt, &str, buffer, cast(schemerlicht_memsize, bytes_read), 0);
      bytes_read = fread(buffer, 1, 256, f);
      }
    fclose(f);
    schemerlicht_stream_rewind(&str);
    schemerlicht_string filename_loading;
    schemerlicht_string_init(ctxt, &filename_loading, filename);
    schemerlicht_vector_push_back(ctxt, &ctxt->filenames_list, filename_loading, schemerlicht_string);
    schemerlicht_vector compiled = compile_scheme(ctxt, &str);
    schemerlicht_stream_close(ctxt, &str);
    schemerlicht_function** it = schemerlicht_vector_begin(&compiled, schemerlicht_function*);
    schemerlicht_function** it_end = schemerlicht_vector_end(&compiled, schemerlicht_function*);
    schemerlicht_function** where_it = schemerlicht_vector_end(compiled_program, schemerlicht_function*);
    schemerlicht_vector_insert(ctxt, compiled_program, &where_it, &it, &it_end, schemerlicht_function*);
    schemerlicht_vector_destroy(ctxt, &compiled);
    schemerlicht_stream_close(ctxt, &str);
    schemerlicht_string_destroy(ctxt, schemerlicht_vector_back(&ctxt->filenames_list, schemerlicht_string));
    schemerlicht_vector_pop_back(&ctxt->filenames_list);
    }
  }

schemerlicht_object* schemerlicht_execute(schemerlicht_context* ctxt, const char* script)
  {
  schemerlicht_stream str;
  schemerlicht_stream_init(ctxt, &str, 10);
  schemerlicht_memsize len = cast(schemerlicht_memsize, strlen(script));
  schemerlicht_stream_write(ctxt, &str, script, len, 0);
  schemerlicht_stream_rewind(&str);
  schemerlicht_object* res = execute_scheme(ctxt, &str);
  schemerlicht_stream_close(ctxt, &str);
  return res;
  }

schemerlicht_object* schemerlicht_execute_file(schemerlicht_context* ctxt, const char* filename)
  {
  FILE* f = fopen(filename, "r");
  if (f)
    {
    schemerlicht_stream str;
    schemerlicht_stream_init(ctxt, &str, 256);
    char buffer[256];
    size_t bytes_read = fread(buffer, 1, 256, f);
    while (bytes_read)
      {
      schemerlicht_stream_write(ctxt, &str, buffer, cast(schemerlicht_memsize, bytes_read), 0);
      bytes_read = fread(buffer, 1, 256, f);
      }
    fclose(f);
    schemerlicht_stream_rewind(&str);
    schemerlicht_string filename_loading;
    schemerlicht_string_init(ctxt, &filename_loading, filename);
    schemerlicht_vector_push_back(ctxt, &ctxt->filenames_list, filename_loading, schemerlicht_string);
    schemerlicht_object* res = execute_scheme(ctxt, &str);
    schemerlicht_stream_close(ctxt, &str);
    //schemerlicht_string_destroy(ctxt, schemerlicht_vector_back(&ctxt->filenames_list, schemerlicht_string));
    //schemerlicht_vector_pop_back(&ctxt->filenames_list);
    return res;
    }
  return NULL;
  }

void schemerlicht_show_memory(schemerlicht_context* ctxt, schemerlicht_string* s)
  {
  char buffer[256];
  schemerlicht_memsize nr_globals = ctxt->globals.vector_size;
  schemerlicht_memsize nr_locals = schemerlicht_maxstack;
  schemerlicht_memsize heap_size = ctxt->raw_heap.vector_size;
  schemerlicht_string_append_cstr(ctxt, s, "Global variables assigned: ");
  schemerlicht_memsize_to_char(buffer, nr_globals);
  schemerlicht_string_append_cstr(ctxt, s, buffer);
  schemerlicht_string_append_cstr(ctxt, s, "\nStack size: ");
  schemerlicht_memsize_to_char(buffer, nr_locals);
  schemerlicht_string_append_cstr(ctxt, s, buffer);
  schemerlicht_string_append_cstr(ctxt, s, " entries\nHeap size: ");
  schemerlicht_memsize_to_char(buffer, heap_size * (schemerlicht_memsize)sizeof(schemerlicht_object));
  schemerlicht_string_append_cstr(ctxt, s, buffer);
  schemerlicht_string_append_cstr(ctxt, s, " bytes\nHeap semispace size: ");
  schemerlicht_memsize_to_char(buffer, heap_size * (schemerlicht_memsize)sizeof(schemerlicht_object) / 2);
  schemerlicht_string_append_cstr(ctxt, s, buffer);
  schemerlicht_string_append_cstr(ctxt, s, " bytes\nHeap size used: ");
  schemerlicht_memsize_to_char(buffer, ctxt->heap_pos * (schemerlicht_memsize)sizeof(schemerlicht_object));
  schemerlicht_string_append_cstr(ctxt, s, buffer);
  schemerlicht_string_append_cstr(ctxt, s, " bytes\nHeap size available until overflow: ");
  schemerlicht_memsize_to_char(buffer, (heap_size / 2 - ctxt->heap_pos) * (schemerlicht_memsize)sizeof(schemerlicht_object));
  schemerlicht_string_append_cstr(ctxt, s, buffer);
  schemerlicht_string_append_cstr(ctxt, s, " bytes\nHeap size available until gc: ");
  schemerlicht_memsize_to_char(buffer, (ctxt->gc_heap_pos_threshold - ctxt->heap_pos) * (schemerlicht_memsize)sizeof(schemerlicht_object));
  schemerlicht_string_append_cstr(ctxt, s, buffer);
  schemerlicht_string_append_cstr(ctxt, s, " bytes\n");
  }

void schemerlicht_show_object(schemerlicht_context* ctxt, schemerlicht_object* obj, schemerlicht_string* s)
  {
  schemerlicht_object_append_to_string(ctxt, obj, s, 0);
  }

void schemerlicht_register_external_primitive(schemerlicht_context* ctxt, const char* name, SCHEMERLICHT_FUNPTR(func_ptr), schemerlicht_foreign_return_type return_type, int number_of_arguments)
  {
  schemerlicht_external_function ext = schemerlicht_external_function_init(ctxt, name, func_ptr, return_type);
  schemerlicht_register_external_function(ctxt, &ext);

  schemerlicht_stream str;
  schemerlicht_stream_init(ctxt, &str, 10);

  schemerlicht_stream_write_str(ctxt, &str, "(define (");
  schemerlicht_stream_write_str(ctxt, &str, name);
  for (int i = 0; i < number_of_arguments; ++i)
    {
    schemerlicht_stream_write_byte(ctxt, &str, ' ');
    schemerlicht_stream_write_byte(ctxt, &str, (char)(i+97));    
    }
  schemerlicht_stream_write_str(ctxt, &str, ") (foreign-call ");
  schemerlicht_stream_write_str(ctxt, &str, name);
  for (int i = 0; i < number_of_arguments; ++i)
    {
    schemerlicht_stream_write_byte(ctxt, &str, ' ');
    schemerlicht_stream_write_byte(ctxt, &str, (char)(i + 97));
    }
  schemerlicht_stream_write_str(ctxt, &str, "))");
  schemerlicht_stream_rewind(&str);
  
  execute_scheme(ctxt, &str);

  schemerlicht_stream_close(ctxt, &str);
  }

schemerlicht_object* schemerlicht_find_global(schemerlicht_context* ctxt, const char* name)
  {
  schemerlicht_object* result = NULL;
  schemerlicht_environment_entry entry;
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, name);
  if (schemerlicht_environment_find(&entry, ctxt, &s))
    {
    if (entry.type == SCHEMERLICHT_ENV_TYPE_GLOBAL)
      {
      result = schemerlicht_vector_at(&ctxt->globals, entry.position, schemerlicht_object);
      }
    }
  schemerlicht_string_destroy(ctxt, &s);
  return result;
  }

void schemerlicht_register_user_data(schemerlicht_context* ctxt, void* user_data)
  {
  ctxt->global->main_context->user_data = user_data;
  }

void* schemerlicht_get_user_data(schemerlicht_context* ctxt)
  {
  return ctxt->global->main_context->user_data;
  }

schemerlicht_function* schemerlicht_lookup(schemerlicht_context* ctxt, const char* function_name)
  {
  schemerlicht_environment_entry lookup_entry;
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, function_name);
  int find_var = schemerlicht_environment_find(&lookup_entry, ctxt, &s);
  schemerlicht_string_destroy(ctxt, &s);
  if (find_var != 0)
    {
    if (lookup_entry.type == SCHEMERLICHT_ENV_TYPE_GLOBAL)
      {
      const schemerlicht_object* global = schemerlicht_vector_at(&ctxt->globals, lookup_entry.position, schemerlicht_object);
      if (global->type == schemerlicht_object_type_lambda)
        {
        schemerlicht_function* fun = cast(schemerlicht_function*, global->value.ptr);
        return fun;
        }
      }
    }
  return NULL;
  }

schemerlicht_object* schemerlicht_call_0(schemerlicht_context* ctxt, schemerlicht_function* fun)
  {
  schemerlicht_object* st0 = schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
  schemerlicht_object* st1 = schemerlicht_vector_at(&ctxt->stack, 1, schemerlicht_object);
  *st0 = ctxt->empty_continuation;
  *st1 = ctxt->halt_continuation;
  return schemerlicht_run(ctxt, fun);
  }
  
schemerlicht_object* schemerlicht_call_1(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_object* arg1)
  {
  schemerlicht_object* st0 = schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
  schemerlicht_object* st1 = schemerlicht_vector_at(&ctxt->stack, 1, schemerlicht_object);
  schemerlicht_object* st2 = schemerlicht_vector_at(&ctxt->stack, 2, schemerlicht_object);
  *st0 = ctxt->empty_continuation;
  *st1 = ctxt->halt_continuation;
  *st2 = *arg1;
  return schemerlicht_run(ctxt, fun);
  }
  
schemerlicht_object* schemerlicht_call_2(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_object* arg1, schemerlicht_object* arg2)
  {
  schemerlicht_object* st0 = schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
  schemerlicht_object* st1 = schemerlicht_vector_at(&ctxt->stack, 1, schemerlicht_object);
  schemerlicht_object* st2 = schemerlicht_vector_at(&ctxt->stack, 2, schemerlicht_object);
  schemerlicht_object* st3 = schemerlicht_vector_at(&ctxt->stack, 3, schemerlicht_object);
  *st0 = ctxt->empty_continuation;
  *st1 = ctxt->halt_continuation;
  *st2 = *arg1;
  *st3 = *arg2;
  return schemerlicht_run(ctxt, fun);
  }
  
schemerlicht_object* schemerlicht_call_3(schemerlicht_context* ctxt, schemerlicht_function* fun, schemerlicht_object* arg1, schemerlicht_object* arg2, schemerlicht_object* arg3)
  {
  schemerlicht_object* st0 = schemerlicht_vector_at(&ctxt->stack, 0, schemerlicht_object);
  schemerlicht_object* st1 = schemerlicht_vector_at(&ctxt->stack, 1, schemerlicht_object);
  schemerlicht_object* st2 = schemerlicht_vector_at(&ctxt->stack, 2, schemerlicht_object);
  schemerlicht_object* st3 = schemerlicht_vector_at(&ctxt->stack, 3, schemerlicht_object);
  schemerlicht_object* st4 = schemerlicht_vector_at(&ctxt->stack, 4, schemerlicht_object);
  *st0 = ctxt->empty_continuation;
  *st1 = ctxt->halt_continuation;
  *st2 = *arg1;
  *st3 = *arg2;
  *st4 = *arg3;
  return schemerlicht_run(ctxt, fun);
  }

schemerlicht_object schemerlicht_make_fixnum(schemerlicht_fixnum fx)
  {
  return make_schemerlicht_object_fixnum(fx);
  }
  
schemerlicht_object schemerlicht_make_flonum(schemerlicht_flonum fl)
  {
  return make_schemerlicht_object_flonum(fl);
  }
  
schemerlicht_object schemerlicht_make_true()
  {
  return make_schemerlicht_object_true();
  }
  
schemerlicht_object schemerlicht_make_false()
  {
  return make_schemerlicht_object_false();
  }

schemerlicht_object schemerlicht_make_nil()
  {
  return make_schemerlicht_object_nil();
  }
  
schemerlicht_object schemerlicht_make_char(schemerlicht_byte ch)
  {
  return make_schemerlicht_object_char(ch);
  }
  
schemerlicht_object schemerlicht_make_string(schemerlicht_context* ctxt, const char* s)
  {
  return make_schemerlicht_object_string(ctxt, s);
  }
  
schemerlicht_object schemerlicht_make_symbol(schemerlicht_context* ctxt, const char* s)
  {
  return make_schemerlicht_object_symbol(ctxt, s);  
  }
