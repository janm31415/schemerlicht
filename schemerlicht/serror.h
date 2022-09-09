#ifndef SCHEMERLICHT_ERROR_H
#define SCHEMERLICHT_ERROR_H

#include "schemerlicht.h"

#define SCHEMERLICHT_ERROR_MEMORY 1
#define SCHEMERLICHT_ERROR_NO_TOKENS 2
#define SCHEMERLICHT_ERROR_NOT_IMPLEMENTED 3
#define SCHEMERLICHT_ERROR_BAD_SYNTAX 4
#define SCHEMERLICHT_ERROR_EXPECTED_KEYWORD 5
#define SCHEMERLICHT_ERROR_RUNERROR 6

void schemerlicht_throw(schemerlicht_context* ctxt, int errorcode);
void schemerlicht_throw_parser(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr);
void schemerlicht_throw_parser_required(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, const char* required);

void schemerlicht_runerror(schemerlicht_context* ctxt, const char* fmt, ...);

#endif //SCHEMERLICHT_ERROR_H