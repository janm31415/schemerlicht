#include "compiler.h"
#include "compile_error.h"
#include "context_defs.h"
#include "asm_aux.h"
#include "inlines.h"
#include "macro_data.h"
#include "preprocess.h"
#include "primitives.h"
#include "types.h"
#include "globals.h"

COMPILER_BEGIN

using namespace VM;


void compile(environment_map& env, repl_data& rd, macro_data& md, context& ctxt, VM::vmcode& code, Program& prog, const primitive_map& pm, const std::map<std::string, external_function>& external_functions, const compiler_options& options)
  {
  }

COMPILER_END
