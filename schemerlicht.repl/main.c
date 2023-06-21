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

#include "schemerlicht/schemerlicht_api.h"

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
  schemerlicht_string stackstring;
  schemerlicht_string_init(ctxt, &stackstring, "");
  schemerlicht_show_stack(ctxt, &stackstring, 0, stack_end);
  printf("%s\n", stackstring.string_ptr);
  schemerlicht_string_destroy(ctxt, &stackstring);
  }

static void print_env(schemerlicht_context* ctxt)
  {
  schemerlicht_string envstring;
  schemerlicht_string_init(ctxt, &envstring, "");
  schemerlicht_show_environment(ctxt, &envstring);
  printf("%s\n", envstring.string_ptr);
  schemerlicht_string_destroy(ctxt, &envstring);
  }

static void print_mem(schemerlicht_context* ctxt)
  {
  schemerlicht_string memstring;
  schemerlicht_string_init(ctxt, &memstring, "");
  schemerlicht_show_memory(ctxt, &memstring);
  printf("%s\n", memstring.string_ptr);
  schemerlicht_string_destroy(ctxt, &memstring);  
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

void print_result(schemerlicht_context* ctxt, schemerlicht_object* result)
  {
  schemerlicht_print_any_error(ctxt);
  if (result)
    {
    schemerlicht_string s;
    schemerlicht_string_init(ctxt, &s, "");
    schemerlicht_show_object(ctxt, result, &s);
    printf("%s\n", s.string_ptr);
    schemerlicht_string_destroy(ctxt, &s);
    }
  }

static void print_hello_world()
  {
  printf("Hello world!\n");
  }

#define COMMAND_BUFFER_LENGTH 32

int main(int argc, char** argv)
  {
  init_debug();

  size_t line_buffer_len = COMMAND_BUFFER_LENGTH * 4;
  char* line_buffer = malloc(line_buffer_len);


  char command_buffer[COMMAND_BUFFER_LENGTH];

  schemerlicht_context* ctxt = schemerlicht_open(1024 * 1024 * 4);
  schemerlicht_build_base(ctxt);

  schemerlicht_register_external_primitive(ctxt, "hello", (SCHEMERLICHT_FUNPTR())&print_hello_world, schemerlicht_foreign_void, 0);

  int quit = 0;

  printf("This is the Schemerlicht REPL.\n");
  printf("Type ,exit to quit or ,? for help.\n\n");

  for (int idx = 1; idx < argc; ++idx)
    {
    char* filename = argv[idx];
    schemerlicht_object* res = schemerlicht_execute_file(ctxt, filename);
    print_result(ctxt, res);
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
      schemerlicht_object* res = schemerlicht_execute(ctxt, line);
      print_result(ctxt, res);
      }
    }

  free(line_buffer);
  schemerlicht_close(ctxt);

  close_debug();
  }
