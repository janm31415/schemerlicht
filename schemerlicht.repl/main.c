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

char* getline(char** buffer, size_t* bufferlen)
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
  printf("Heap size: %d bytes\n", heap_size * sizeof(schemerlicht_object));
  printf("Heap semispace size: %d bytes\n", heap_size * sizeof(schemerlicht_object)/2);
  printf("Heap size used: %d bytes\n", ctxt->heap_pos * sizeof(schemerlicht_object));
  printf("Heap size available until overflow: %d bytes\n", (heap_size/2 - ctxt->heap_pos) * sizeof(schemerlicht_object));
  printf("Heap size available until gc: %d bytes\n", (ctxt->gc_heap_pos_threshold - ctxt->heap_pos) * sizeof(schemerlicht_object));
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

#define COMMAND_BUFFER_LENGTH 32

int main(int argc, char** argv)
  {
  init_debug();

  size_t line_buffer_len = COMMAND_BUFFER_LENGTH*4;
  char* line_buffer = malloc(line_buffer_len);


  char command_buffer[COMMAND_BUFFER_LENGTH];

  UNUSED(argc);
  UNUSED(argv);
  schemerlicht_context* ctxt = schemerlicht_open(1024 * 1024);
  schemerlicht_compile_callcc(ctxt);
  schemerlicht_compile_r5rs(ctxt);
  schemerlicht_compile_input_output(ctxt);
  schemerlicht_compile_modules(ctxt, SCHEMERLICHT_MODULES_PATH);

  int quit = 0;

  printf("This is the Schemerlicht REPL.\n");
  printf("Type ,exit to quit or ,? for help.\n\n");

  while (quit == 0)
    {
    printf("> ");
    char* line = getline(&line_buffer, &line_buffer_len);
    int input_is_scheme_command = 1;
    if (line[0] == ',') // check for commands
      {
      //memcpy(command_buffer, line, COMMAND_BUFFER_LENGTH);
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
      schemerlicht_syntax_errors_clear(ctxt);
      schemerlicht_compile_errors_clear(ctxt);
      schemerlicht_runtime_errors_clear(ctxt);
      schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, line);
      if (schemerlicht_context_is_error_free(ctxt) == 0)
        {
        schemerlicht_print_any_error(ctxt);
        continue;
        }
      schemerlicht_program prog = make_program(ctxt, &tokens);
      if (schemerlicht_context_is_error_free(ctxt) == 0)
        {
        destroy_tokens_vector(ctxt, &tokens);
        schemerlicht_print_any_error(ctxt);
        continue;
        }
      schemerlicht_preprocess(ctxt, &prog);
      if (schemerlicht_context_is_error_free(ctxt) == 0)
        {
        destroy_tokens_vector(ctxt, &tokens);
        schemerlicht_program_destroy(ctxt, &prog);
        schemerlicht_print_any_error(ctxt);
        continue;
        }
      schemerlicht_vector compiled_program = schemerlicht_compile_program(ctxt, &prog);
      if (schemerlicht_context_is_error_free(ctxt) == 0)
        {
        destroy_tokens_vector(ctxt, &tokens);
        schemerlicht_program_destroy(ctxt, &prog);
        schemerlicht_print_any_error(ctxt);
        continue;
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
    }

  free(line_buffer);
  schemerlicht_close(ctxt);

  close_debug();
  }