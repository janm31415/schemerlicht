#pragma once

#include "namespace.h"
#include "schemerlicht_api.h"

#include <string>

COMPILER_BEGIN

struct format_options
  {
  COMPILER_SCHEMERLICHT_API format_options();

  int indent_offset;
  int max_width;
  int min_width;
  };

COMPILER_SCHEMERLICHT_API std::string format(const std::string& in, const format_options& ops);

COMPILER_END
