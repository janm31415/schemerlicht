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

int main(int argc, char** argv)
  {
  init_debug();

  size_t line_buffer_len = 10;
  char* line_buffer = malloc(line_buffer_len);

  UNUSED(argc);
  UNUSED(argv);
  schemerlicht_context* ctxt = schemerlicht_open(1024 * 1024);
  schemerlicht_compile_callcc(ctxt);
  schemerlicht_compile_r5rs(ctxt);
  schemerlicht_compile_input_output(ctxt);
  schemerlicht_compile_modules(ctxt, SCHEMERLICHT_MODULES_PATH);

  int quit = 0;

  printf("This is the Schemerlicht REPL.\n");
  printf("Type ,exit to quit.\n\n");

  while (quit == 0)
    {
    printf("> ");
    char* line = getline(&line_buffer, &line_buffer_len);
    if (strcmp(line, ",exit\n") == 0)
      {
      quit = 1;
      }
    else
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