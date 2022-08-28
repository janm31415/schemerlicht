#pragma once

#include "namespace.h"
#include "schemescript_api.h"
#include "environment.h"
#include <map>


COMPILER_BEGIN

struct repl_data
  {
  COMPILER_SCHEMESCRIPT_API repl_data() : alpha_conversion_index(0), global_index(0) {}

  //environment<alpha_conversion_data> alpha_conversion_env;
  uint64_t alpha_conversion_index;
  std::map<std::string, uint64_t> quote_to_index;
  uint64_t global_index;
  };

COMPILER_END
