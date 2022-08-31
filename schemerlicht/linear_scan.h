
#pragma once

#include "schemerlicht_api.h"

#include "namespace.h"
#include <stdint.h>

#include "parse.h"

COMPILER_BEGIN

struct compiler_options;

enum linear_scan_algorithm
  {
  lsa_naive,
  lsa_detailed
  };

COMPILER_SCHEMERLICHT_API void linear_scan(Program& prog, linear_scan_algorithm lsa, const compiler_options& ops);

COMPILER_END
