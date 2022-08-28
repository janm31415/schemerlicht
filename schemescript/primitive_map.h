#pragma once
#include "namespace.h"
#include <string>
#include <stdint.h>
#include <map>

COMPILER_BEGIN

struct primitive_entry
  {
  std::string label_name;
  uint64_t address;
  };

typedef std::map<std::string, primitive_entry> primitive_map;

COMPILER_END
