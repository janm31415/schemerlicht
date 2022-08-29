#include "repl_data.h"

COMPILER_BEGIN

repl_data::repl_data() : alpha_conversion_index(0), global_index(0) {}

repl_data make_deep_copy(const repl_data& rd)
  {
  repl_data out = rd;
  out.alpha_conversion_env = make_deep_copy(rd.alpha_conversion_env);
  return out;
  }
  
COMPILER_END
