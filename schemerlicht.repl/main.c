#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#include <crtdbg.h>
#include <windows.h>
#endif

#if defined(MEMORY_LEAK_TRACKING) && defined(_MSC_VER)
#ifndef NDEBUG
#define MEMORY_LEAK_TRACKING_MSVC
#endif
#endif  

#include "schemerlicht/schemerlicht.h"
#include "schemerlicht/limits.h"
#include "schemerlicht/context.h"
#include "schemerlicht/parser.h"
#include "schemerlicht/token.h"
#include "schemerlicht/preprocess.h"
#include "schemerlicht/compiler.h"
#include "schemerlicht/vm.h"
#include "schemerlicht/callcc.h"
#include "schemerlicht/r5rs.h"
#include "schemerlicht/inputoutput.h"
#include "schemerlicht/modules.h"
#include "schemerlicht/modulesconf.h"
#include "schemerlicht/error.h"
#include "schemerlicht/environment.h"
#include "schemerlicht/dump.h"

char* schemerlicht_getline(char** buffer, size_t* bufferlen)
  {
  char* line = *buffer;
  char* linep = line;
  size_t len = *bufferlen;
  int c;

  if (line == NULL)
    return NULL;

  for (;;) {
    c = fgetc(stdin);
    if (c == EOF)
      break;

    if (--len == 0)
      {
      len = *bufferlen;
      intptr_t dist = line - linep;
      *buffer = realloc(*buffer, *bufferlen *= 2);

      if (*buffer == NULL) {
        return NULL;
        }
      line = *buffer + dist;
      linep = *buffer;
      }

    if ((*line++ = (char)c) == '\n')
      break;
    }
  *line = '\0';
  return linep;
  }

static void print_help()
  {
  printf("This is Schemerlicht.You are interacting with the REPL.\n");
  printf("Enter scheme commands or one of the following:\n");
  printf("  ,exit:      quit the application.\n");
  printf("  ,stack <n>: print the stack.\n");
  printf("  ,env:       print the environment.\n");
  printf("  ,mem:       print the memory usage.\n\n");
  }

static void print_stack(schemerlicht_context* ctxt, int stack_end)
  {
  schemerlicht_string stackstring = schemerlicht_show_stack(ctxt, 0, stack_end);
  printf("%s\n", stackstring.string_ptr);
  schemerlicht_string_destroy(ctxt, &stackstring);
  }

static void print_env(schemerlicht_context* ctxt)
  {
  schemerlicht_string envstring = schemerlicht_show_environment(ctxt);
  printf("%s\n", envstring.string_ptr);
  schemerlicht_string_destroy(ctxt, &envstring);
  }

static void print_mem(schemerlicht_context* ctxt)
  {
  schemerlicht_memsize nr_globals = ctxt->globals.vector_size;
  schemerlicht_memsize nr_locals = schemerlicht_maxstack;
  schemerlicht_memsize heap_size = ctxt->raw_heap.vector_size;
  printf("Global variables assigned: %d\n", nr_globals);
  printf("Stack size: %d entries\n", nr_locals);
  printf("Heap size: %d bytes\n", heap_size * (schemerlicht_memsize)sizeof(schemerlicht_object));
  printf("Heap semispace size: %d bytes\n", heap_size * (schemerlicht_memsize)sizeof(schemerlicht_object)/2);
  printf("Heap size used: %d bytes\n", ctxt->heap_pos * (schemerlicht_memsize)sizeof(schemerlicht_object));
  printf("Heap size available until overflow: %d bytes\n", (heap_size/2 - ctxt->heap_pos) * (schemerlicht_memsize)sizeof(schemerlicht_object));
  printf("Heap size available until gc: %d bytes\n", (ctxt->gc_heap_pos_threshold - ctxt->heap_pos) * (schemerlicht_memsize)sizeof(schemerlicht_object));
  }

static void init_debug()
  {
#if defined(MEMORY_LEAK_TRACKING_MSVC)
  int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
  _CrtSetDbgFlag(flags | _CRTDBG_ALLOC_MEM_DF);
#endif
  }

static void close_debug()
  {
#if defined(MEMORY_LEAK_TRACKING_MSVC)
  int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
  flags &= ~_CRTDBG_DELAY_FREE_MEM_DF;
  flags |= _CRTDBG_LEAK_CHECK_DF;
  _CrtSetDbgFlag(flags);
#endif
  }

static void execute_scheme(schemerlicht_context* ctxt, schemerlicht_stream* str)
  {
  schemerlicht_syntax_errors_clear(ctxt);
  schemerlicht_compile_errors_clear(ctxt);
  schemerlicht_runtime_errors_clear(ctxt);
  schemerlicht_vector tokens = tokenize(ctxt, str);
  if (schemerlicht_context_is_error_free(ctxt) == 0)
    {
    schemerlicht_print_any_error(ctxt);
    return;
    }
  schemerlicht_program prog = make_program(ctxt, &tokens);
  if (schemerlicht_context_is_error_free(ctxt) == 0)
    {
    destroy_tokens_vector(ctxt, &tokens);
    schemerlicht_print_any_error(ctxt);
    return;
    }
  schemerlicht_preprocess(ctxt, &prog);
  if (schemerlicht_context_is_error_free(ctxt) == 0)
    {
    destroy_tokens_vector(ctxt, &tokens);
    schemerlicht_program_destroy(ctxt, &prog);
    schemerlicht_print_any_error(ctxt);
    return;
    }
  schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);
#if 0
  schemerlicht_string dumped = schemerlicht_dump(ctxt, &prog);
  printf("%s\n", dumped.string_ptr);
  schemerlicht_string_destroy(ctxt, &dumped);
#endif
  if (schemerlicht_context_is_error_free(ctxt) == 0)
    {
    destroy_tokens_vector(ctxt, &tokens);
    schemerlicht_program_destroy(ctxt, &prog);
    schemerlicht_print_any_error(ctxt);
    return;
    }
  schemerlicht_object* res = schemerlicht_run_program(ctxt, &compiled_program);
  schemerlicht_print_any_error(ctxt);
  schemerlicht_string s = schemerlicht_object_to_string(ctxt, res, 0);
  printf("%s\n", s.string_ptr);
  schemerlicht_string_destroy(ctxt, &s);

  schemerlicht_compiled_program_register(ctxt, &compiled_program);

  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  }

static void execute_scheme_cstr(schemerlicht_context* ctxt, const char* script)
  {
  schemerlicht_stream str;
  schemerlicht_stream_init(ctxt, &str, 10);
  schemerlicht_memsize len = cast(schemerlicht_memsize, strlen(script));
  schemerlicht_stream_write(ctxt, &str, script, len, 0);
  schemerlicht_stream_rewind(&str);
  execute_scheme(ctxt, &str);
  schemerlicht_stream_close(ctxt, &str);
  }

static void run_scheme_from_file(schemerlicht_context* ctxt, const char* filename)
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
    execute_scheme(ctxt, &str);
    schemerlicht_stream_close(ctxt, &str);
    schemerlicht_string_destroy(ctxt, schemerlicht_vector_back(&ctxt->filenames_list, schemerlicht_string));
    schemerlicht_vector_pop_back(&ctxt->filenames_list);
    }
  }

#define COMMAND_BUFFER_LENGTH 32

int main(int argc, char** argv)
  {
  init_debug();

  size_t line_buffer_len = COMMAND_BUFFER_LENGTH*4;
  char* line_buffer = malloc(line_buffer_len);


  char command_buffer[COMMAND_BUFFER_LENGTH];

  schemerlicht_context* ctxt = schemerlicht_open(1024 * 1024 * 4);
  schemerlicht_compile_callcc(ctxt);
  schemerlicht_compile_r5rs(ctxt);
  schemerlicht_compile_input_output(ctxt);
  schemerlicht_compile_modules(ctxt, SCHEMERLICHT_MODULES_PATH);

  int quit = 0;

  printf("This is the Schemerlicht REPL.\n");
  printf("Type ,exit to quit or ,? for help.\n\n");

  for (int idx = 1; idx < argc; ++idx)
    {
    char* filename = argv[idx];
    run_scheme_from_file(ctxt, filename);
    }


  while (quit == 0)
    {
    printf("> ");
    char* line = schemerlicht_getline(&line_buffer, &line_buffer_len);
    int input_is_scheme_command = 1;
    if (line[0] == ',') // check for commands
      {
      sscanf(line, "%s", command_buffer);
      if (strcmp(command_buffer, ",exit") == 0)
        {
        input_is_scheme_command = 0;
        quit = 1;
        }
      if (strcmp(command_buffer, ",?") == 0)
        {
        input_is_scheme_command = 0;
        print_help();
        }
      if (strcmp(command_buffer, ",env") == 0)
        {
        input_is_scheme_command = 0;
        print_env(ctxt);
        }
      if (strcmp(command_buffer, ",mem") == 0)
        {
        input_is_scheme_command = 0;
        print_mem(ctxt);
        }
      if (strcmp(command_buffer, ",stack") == 0)
        {
        int stack_end = 9;
        sscanf(line, ",stack %d", &stack_end);
        input_is_scheme_command = 0;
        print_stack(ctxt, stack_end);
        }
      }
    if (input_is_scheme_command)
      {
      execute_scheme_cstr(ctxt, line);
      }
    }

  free(line_buffer);
  schemerlicht_close(ctxt);

  close_debug();
  }
