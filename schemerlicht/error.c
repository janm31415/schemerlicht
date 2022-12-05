#include "error.h"
#include "limits.h"
#include "context.h"
#include "parser.h"
#include "syscalls.h"

#include <stdlib.h>
#include <setjmp.h>
#include <stdio.h>

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

static void append_error_code(schemerlicht_context* ctxt, schemerlicht_string* message, int errorcode)
  {
  switch (errorcode)
    {
    case SCHEMERLICHT_ERROR_MEMORY: schemerlicht_string_append_cstr(ctxt, message, "out of memory"); break;
    case SCHEMERLICHT_ERROR_NO_TOKENS: schemerlicht_string_append_cstr(ctxt, message, "no tokens"); break;
    case SCHEMERLICHT_ERROR_NOT_IMPLEMENTED: schemerlicht_string_append_cstr(ctxt, message, "not implemented"); break;
    case SCHEMERLICHT_ERROR_BAD_SYNTAX: schemerlicht_string_append_cstr(ctxt, message, "bad syntax"); break;
    case SCHEMERLICHT_ERROR_EXPECTED_KEYWORD: schemerlicht_string_append_cstr(ctxt, message, "expected keyword"); break;
    case SCHEMERLICHT_ERROR_RUNERROR: schemerlicht_string_append_cstr(ctxt, message, "runtime error"); break;
    case SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS: schemerlicht_string_append_cstr(ctxt, message, "invalid number of arguments"); break;
    case SCHEMERLICHT_ERROR_INVALID_ARGUMENT: schemerlicht_string_append_cstr(ctxt, message, "invalid argument"); break;
    case SCHEMERLICHT_ERROR_VARIABLE_UNKNOWN: schemerlicht_string_append_cstr(ctxt, message, "variable unknown"); break;
    case SCHEMERLICHT_ERROR_EXTERNAL_UNKNOWN: schemerlicht_string_append_cstr(ctxt, message, "external unknown"); break;
    default: break;
    }
  }

void schemerlicht_syntax_error(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, schemerlicht_string* filename, schemerlicht_string* msg)
  {
  schemerlicht_string message;
  schemerlicht_string_init(ctxt, &message, "syntax error");
  if (filename && filename->string_length > 0)
    {
    schemerlicht_string_append_cstr(ctxt, &message, " (");
    schemerlicht_string_append(ctxt, &message, filename);
    schemerlicht_string_push_back(ctxt, &message, ')');
    }
  if (line_nr >= 0 && column_nr >= 0)
    {
    char number[256];
    schemerlicht_string_append_cstr(ctxt, &message, " (");
    schemerlicht_int_to_char(number, line_nr);
    schemerlicht_string_append_cstr(ctxt, &message, number);
    schemerlicht_string_push_back(ctxt, &message, ',');
    schemerlicht_int_to_char(number, column_nr);
    schemerlicht_string_append_cstr(ctxt, &message, number);
    schemerlicht_string_push_back(ctxt, &message, ')');
    }
  schemerlicht_string_append_cstr(ctxt, &message, ": ");
  append_error_code(ctxt, &message, errorcode);  
  if (msg->string_length > 0)
    {
    schemerlicht_string_append_cstr(ctxt, &message, ": ");
    schemerlicht_string_append(ctxt, &message, msg);
    }
  schemerlicht_string_destroy(ctxt, msg);
  ++ctxt->number_of_syntax_errors;
  schemerlicht_error_report report;
  report.column_number = column_nr;
  report.line_number = line_nr;
  report.errorcode = errorcode;
  report.message = message;
  schemerlicht_vector_push_back(ctxt, &ctxt->syntax_error_reports, report, schemerlicht_error_report);
  }

void schemerlicht_syntax_error_cstr(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, schemerlicht_string* filename, const char* msg)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, msg);
  schemerlicht_syntax_error(ctxt, errorcode, line_nr, column_nr, filename, &s);
  }

void schemerlicht_compile_error(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, schemerlicht_string* filename, schemerlicht_string* msg)
  {
  schemerlicht_string message;
  schemerlicht_string_init(ctxt, &message, "compile error");
  if (filename && filename->string_length > 0)
    {
    schemerlicht_string_append_cstr(ctxt, &message, " (");
    schemerlicht_string_append(ctxt, &message, filename);
    schemerlicht_string_push_back(ctxt, &message, ')');
    }
  if (line_nr >= 0 && column_nr >= 0)
    {
    char number[256];
    schemerlicht_string_append_cstr(ctxt, &message, " (");
    schemerlicht_int_to_char(number, line_nr);
    schemerlicht_string_append_cstr(ctxt, &message, number);
    schemerlicht_string_push_back(ctxt, &message, ',');
    schemerlicht_int_to_char(number, column_nr);
    schemerlicht_string_append_cstr(ctxt, &message, number);
    schemerlicht_string_push_back(ctxt, &message, ')');
    }
  schemerlicht_string_append_cstr(ctxt, &message, ": ");
  append_error_code(ctxt, &message, errorcode);
  if (msg->string_length > 0)
    {
    schemerlicht_string_append_cstr(ctxt, &message, ": ");
    schemerlicht_string_append(ctxt, &message, msg);
    }
  schemerlicht_string_destroy(ctxt, msg);
  ++ctxt->number_of_compile_errors;
  schemerlicht_error_report report;
  report.column_number = column_nr;
  report.line_number = line_nr;
  report.errorcode = errorcode;
  report.message = message;
  schemerlicht_vector_push_back(ctxt, &ctxt->compile_error_reports, report, schemerlicht_error_report);
  }

void schemerlicht_compile_error_cstr(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, schemerlicht_string* filename, const char* msg)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, msg);
  schemerlicht_compile_error(ctxt, errorcode, line_nr, column_nr, filename, &s);
  }

void schemerlicht_runtime_error(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, schemerlicht_string* msg)
  {
  schemerlicht_string message;
  schemerlicht_string_init(ctxt, &message, "runtime error");
  if (line_nr >= 0 && column_nr >= 0)
    {
    char number[256];
    schemerlicht_string_append_cstr(ctxt, &message, " (");
    schemerlicht_int_to_char(number, line_nr);
    schemerlicht_string_append_cstr(ctxt, &message, number);
    schemerlicht_string_push_back(ctxt, &message, ',');
    schemerlicht_int_to_char(number, column_nr);
    schemerlicht_string_append_cstr(ctxt, &message, number);
    schemerlicht_string_push_back(ctxt, &message, ')');
    }
  schemerlicht_string_append_cstr(ctxt, &message, ": ");
  append_error_code(ctxt, &message, errorcode);
  if (msg->string_length > 0)
    {
    schemerlicht_string_append_cstr(ctxt, &message, ": ");
    schemerlicht_string_append(ctxt, &message, msg);
    }
  schemerlicht_string_destroy(ctxt, msg);
  ++ctxt->number_of_runtime_errors;
  schemerlicht_error_report report;
  report.column_number = column_nr;
  report.line_number = line_nr;
  report.errorcode = errorcode;
  report.message = message;
  schemerlicht_vector_push_back(ctxt, &ctxt->runtime_error_reports, report, schemerlicht_error_report);
  }

void schemerlicht_runtime_error_cstr(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, const char* msg)
  {
  schemerlicht_string s;
  schemerlicht_string_init(ctxt, &s, msg);
  schemerlicht_runtime_error(ctxt, errorcode, line_nr, column_nr, &s);
  }

void schemerlicht_syntax_errors_clear(schemerlicht_context* ctxt)
  {
  schemerlicht_error_report* it = schemerlicht_vector_begin(&ctxt->syntax_error_reports, schemerlicht_error_report);
  schemerlicht_error_report* it_end = schemerlicht_vector_end(&ctxt->syntax_error_reports, schemerlicht_error_report);
  for (; it != it_end; ++it)
    {
    schemerlicht_string_destroy(ctxt, &it->message);
    }
  ctxt->syntax_error_reports.vector_size = 0;
  ctxt->number_of_syntax_errors = 0;
  }

void schemerlicht_compile_errors_clear(schemerlicht_context* ctxt)
  {
  schemerlicht_error_report* it = schemerlicht_vector_begin(&ctxt->compile_error_reports, schemerlicht_error_report);
  schemerlicht_error_report* it_end = schemerlicht_vector_end(&ctxt->compile_error_reports, schemerlicht_error_report);
  for (; it != it_end; ++it)
    {
    schemerlicht_string_destroy(ctxt, &it->message);
    }
  ctxt->compile_error_reports.vector_size = 0;
  ctxt->number_of_compile_errors = 0;
  }

void schemerlicht_runtime_errors_clear(schemerlicht_context* ctxt)
  {
  schemerlicht_error_report* it = schemerlicht_vector_begin(&ctxt->runtime_error_reports, schemerlicht_error_report);
  schemerlicht_error_report* it_end = schemerlicht_vector_end(&ctxt->runtime_error_reports, schemerlicht_error_report);
  for (; it != it_end; ++it)
    {
    schemerlicht_string_destroy(ctxt, &it->message);
    }
  ctxt->runtime_error_reports.vector_size = 0;
  ctxt->number_of_runtime_errors = 0;
  }

void schemerlicht_print_any_error(schemerlicht_context* ctxt)
  {
  if (ctxt->number_of_compile_errors > 0)
    {
    schemerlicht_error_report* it = schemerlicht_vector_begin(&ctxt->compile_error_reports, schemerlicht_error_report);
    schemerlicht_error_report* it_end = schemerlicht_vector_end(&ctxt->compile_error_reports, schemerlicht_error_report);
    for (; it != it_end; ++it)
      printf("%s\n", it->message.string_ptr);
    }
  if (ctxt->number_of_syntax_errors > 0)
    {
    schemerlicht_error_report* it = schemerlicht_vector_begin(&ctxt->syntax_error_reports, schemerlicht_error_report);
    schemerlicht_error_report* it_end = schemerlicht_vector_end(&ctxt->syntax_error_reports, schemerlicht_error_report);
    for (; it != it_end; ++it)
      printf("%s\n", it->message.string_ptr);
    }
  if (ctxt->number_of_runtime_errors > 0)
    {
    schemerlicht_error_report* it = schemerlicht_vector_begin(&ctxt->runtime_error_reports, schemerlicht_error_report);
    schemerlicht_error_report* it_end = schemerlicht_vector_end(&ctxt->runtime_error_reports, schemerlicht_error_report);
    for (; it != it_end; ++it)
      printf("%s\n", it->message.string_ptr);
    }
  }

int schemerlicht_context_is_error_free(schemerlicht_context* ctxt)
  {
  if (ctxt->number_of_compile_errors > 0)
    return 0;
  if (ctxt->number_of_syntax_errors > 0)
    return 0;
  if (ctxt->number_of_runtime_errors > 0)
    return 0;
  return 1;
  }