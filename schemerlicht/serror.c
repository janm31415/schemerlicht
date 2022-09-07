#include "serror.h"
#include "slimits.h"

void schemerlicht_throw(schemerlicht_context* ctxt, int errorcode)
  {
  UNUSED(ctxt);
  UNUSED(errorcode);
  }

void schemerlicht_throw_parser(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr)
  {
  UNUSED(ctxt);
  UNUSED(errorcode);
  UNUSED(line_nr);
  UNUSED(column_nr);
  }

void schemerlicht_throw_parser_required(schemerlicht_context* ctxt, int errorcode, int line_nr, int column_nr, const char* required)
  {
  UNUSED(ctxt);
  UNUSED(errorcode);
  UNUSED(line_nr);
  UNUSED(column_nr);
  UNUSED(required);
  }