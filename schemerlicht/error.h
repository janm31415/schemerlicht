#ifndef SCHEMERLICHT_ERROR_H
#define SCHEMERLICHT_ERROR_H

#include "schemerlicht.h"
#include "string.h"

#define SCHEMERLICHT_ERROR_MEMORY 1
#define SCHEMERLICHT_ERROR_NO_TOKENS 2
#define SCHEMERLICHT_ERROR_NOT_IMPLEMENTED 3
#define SCHEMERLICHT_ERROR_BAD_SYNTAX 4
#define SCHEMERLICHT_ERROR_EXPECTED_KEYWORD 5
#define SCHEMERLICHT_ERROR_RUNERROR 6
#define SCHEMERLICHT_ERROR_INVALID_NUMBER_OF_ARGUMENTS 7
#define SCHEMERLICHT_ERROR_INVALID_ARGUMENT 8
#define SCHEMERLICHT_ERROR_VARIABLE_UNKNOWN 9

void schemerlicht_throw(schemerlicht_context* ctxt, int errorcode);
void schemerlicht_throw_parser(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr);
void schemerlicht_throw_parser_required(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, const char* required);

void schemerlicht_throw_compiler(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, schemerlicht_string* filename);

void schemerlicht_runerror(schemerlicht_context* ctxt, const char* fmt, ...);

void schemerlicht_syntax_error(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, schemerlicht_string* msg);
void schemerlicht_syntax_error_cstr(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, const char* msg);
void schemerlicht_compile_error(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, schemerlicht_string* msg);
void schemerlicht_compile_error_cstr(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, const char* msg);

typedef struct schemerlicht_error_report
  {
  int errorcode;
  int line_number;
  int column_number;
  schemerlicht_string message;
  } schemerlicht_error_report;

void schemerlicht_syntax_errors_clear(schemerlicht_context* ctxt);
void schemerlicht_compile_errors_clear(schemerlicht_context* ctxt);

#endif //SCHEMERLICHT_ERROR_H