#include "macro_data.h"
#include "vm/vm.h"

COMPILER_BEGIN

macro_data create_macro_data()
  {
  macro_data md;
  return md;
  }

void destroy_macro_data(macro_data& md)
  {
  for (auto& f : md.compiled_macros)
    VM::free_bytecode((void*)f.first, f.second);
  md.compiled_macros.clear();
  }

COMPILER_END
