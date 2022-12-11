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

#include <stdio.h>
#include <string.h>

void schemerlicht_build_base(schemerlicht_context* ctxt)
  {
  schemerlicht_compile_callcc(ctxt);
  schemerlicht_compile_r5rs(ctxt);
  schemerlicht_compile_input_output(ctxt);
  schemerlicht_compile_modules(ctxt, SCHEMERLICHT_MODULES_PATH);
  }

static schemerlicht_object* execute_scheme(schemerlicht_context* ctxt, schemerlicht_stream* str)
  {
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
    schemerlicht_string_destroy(ctxt, schemerlicht_vector_back(&ctxt->filenames_list, schemerlicht_string));
    schemerlicht_vector_pop_back(&ctxt->filenames_list);
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