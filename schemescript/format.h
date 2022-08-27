#pragma once

#include "namespace.h"
#include "schemescript_api.h"

#include <string>

COMPILER_BEGIN

struct format_options
  {
  COMPILER_SCHEMESCRIPT_API format_options();

  int indent_offset;
  int max_width;
  int min_width;
  };

COMPILER_SCHEMESCRIPT_API std::string format(const std::string& in, const format_options& ops);

COMPILER_END
