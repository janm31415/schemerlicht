#pragma once

#include "namespace.h"
#include "schemescript_api.h"
#include <string>
#include <map>
#include <vector>

COMPILER_BEGIN

struct macro_entry
  {
  std::string name;
  std::vector<std::string> variables;
  bool variable_arity;
  };

typedef std::map<std::string, macro_entry> macro_map;

struct macro_data
  {
  macro_map m;
  std::vector<std::pair<void*, uint64_t>> compiled_macros;
  };

COMPILER_SCHEMESCRIPT_API macro_data create_macro_data();
COMPILER_SCHEMESCRIPT_API void destroy_macro_data(macro_data& md);

COMPILER_END

