#pragma once

#include "namespace.h"
#include "schemescript_api.h"
#include "vm/vmcode.h"
#include "context.h"
#include "compile_data.h"
#include "compiler_options.h"
#include "environment.h"
#include "parse.h"
#include "repl_data.h"
#include "primitive_map.h"
#include <memory>
#include <map>

COMPILER_BEGIN

struct macro_data;

struct external_function
  {
  enum argtype
    {
    T_BOOL,
    T_CHAR_POINTER,
    T_DOUBLE,
    T_INT64,
    T_VOID,
    T_SCM
    };
  std::string name;
  uint64_t address;
  std::vector<argtype> arguments;
  argtype return_type;
  };
  
typedef void(*fun_ptr)(VM::vmcode&, const compiler_options&);

typedef std::map<std::string, fun_ptr> function_map;
function_map generate_function_map();

COMPILER_SCHEMESCRIPT_API void compile(environment_map& env, repl_data& rd, macro_data& md, context& ctxt, VM::vmcode& code, Program& prog, const primitive_map& pm, const std::map<std::string, external_function>& external_functions, const compiler_options& options);

COMPILER_END
