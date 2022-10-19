#include "error.h"
#include "limits.h"
#include "context.h"
#include "parser.h"

#include <stdlib.h>
#include <setjmp.h>

void schemerlicht_throw(schemerlicht_context* ctxt, int errorcode)
  {  
  UNUSED(ctxt);
  UNUSED(errorcode);
  if (ctxt->error_jmp)
    {
    ctxt->error_jmp->status = errorcode;
    longjmp(ctxt->error_jmp->jmp, 1);
    }
  exit(errorcode);
  }

void schemerlicht_throw_parser(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr)
  {
  UNUSED(line_nr);
  UNUSED(column_nr);
  schemerlicht_throw(ctxt, errorcode);
  }

void schemerlicht_throw_parser_required(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, const char* required)
  {
  UNUSED(required);
  schemerlicht_throw_parser(ctxt, errorcode, line_nr, column_nr);
  }

void schemerlicht_runerror(schemerlicht_context* ctxt, const char* fmt, ...)
  {
  va_list argp;
  va_start(argp, fmt);
  
  va_end(argp);
  schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_RUNERROR);
  }

void schemerlicht_syntax_error(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, const char* msg)
  {
  ++ctxt->number_of_syntax_errors;
  }