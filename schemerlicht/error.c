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

void schemerlicht_throw_compiler(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, schemerlicht_string* filename)
  {
  UNUSED(line_nr);
  UNUSED(column_nr);
  UNUSED(filename);
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

void schemerlicht_syntax_error(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, schemerlicht_string* msg)
  {
  schemerlicht_string message;
  schemerlicht_string_init(ctxt, &message, "syntax error");
  if (line_nr >= 0 && column_nr >= 0)
    {
    char number[256];
    schemerlicht_string_append_cstr(ctxt, &message, " (");
    sprintf(number, "%d", line_nr);
    schemerlicht_string_append_cstr(ctxt, &message, number);
    schemerlicht_string_push_back(ctxt, &message, ',');
    sprintf(number, "%d", column_nr);
    schemerlicht_string_append_cstr(ctxt, &message, number);
    schemerlicht_string_push_back(ctxt, &message, ')');
    }
  schemerlicht_string_append_cstr(ctxt, &message, ": ");
  switch (errorcode)
    {
    case SCHEMERLICHT_ERROR_MEMORY: schemerlicht_string_append_cstr(ctxt, &message, "out of memory"); break;
    case SCHEMERLICHT_ERROR_NO_TOKENS: schemerlicht_string_append_cstr(ctxt, &message, "no tokens"); break;
    case SCHEMERLICHT_ERROR_NOT_IMPLEMENTED: schemerlicht_string_append_cstr(ctxt, &message, "not implemented"); break;
    case SCHEMERLICHT_ERROR_BAD_SYNTAX: schemerlicht_string_append_cstr(ctxt, &message, "bad syntax"); break;
    case SCHEMERLICHT_ERROR_EXPECTED_KEYWORD: schemerlicht_string_append_cstr(ctxt, &message, "expected keyword"); break;
    case SCHEMERLICHT_ERROR_RUNERROR: schemerlicht_string_append_cstr(ctxt, &message, "runtime error"); break;
    case SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS: schemerlicht_string_append_cstr(ctxt, &message, "invalid number of arguments"); break;
    case SCHEMERLICHT_ERROR_INVALID_ARGUMENT: schemerlicht_string_append_cstr(ctxt, &message, "invalid argument"); break;
    default: break;
    }
  if (msg->string_length > 0)
    {
    schemerlicht_string_append_cstr(ctxt, &message, ": ");
    schemerlicht_string_append(ctxt, &message, msg);
    }
  schemerlicht_string_destroy(ctxt, msg);
  ++ctxt->number_of_syntax_errors;
  schemerlicht_syntax_error_report report;
  report.column_number = column_nr;
  report.line_number = line_nr;
  report.errorcode = errorcode;
  report.message = message;
  schemerlicht_vector_push_back(ctxt, &ctxt->syntax_error_reports, report, schemerlicht_syntax_error_report);
  }

void schemerlicht_syntax_error_cstr(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, const char* msg)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, msg);
  schemerlicht_syntax_error(ctxt, errorcode, line_nr, column_nr, &s);
  }

void schemerlicht_syntax_errors_clear(schemerlicht_context* ctxt)
  {
  schemerlicht_syntax_error_report* it = schemerlicht_vector_begin(&ctxt->syntax_error_reports, schemerlicht_syntax_error_report);
  schemerlicht_syntax_error_report* it_end = schemerlicht_vector_end(&ctxt->syntax_error_reports, schemerlicht_syntax_error_report);
  for (; it != it_end; ++it)
    {
    schemerlicht_string_destroy(ctxt, &it->message);
    }
  ctxt->syntax_error_reports.vector_size = 0;
  ctxt->number_of_syntax_errors = 0;
  }