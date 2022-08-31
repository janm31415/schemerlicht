#pragma once

#include "namespace.h"

#include <map>
#include <string>
#include <vector>

#include "compiler_options.h"
#include "schemescript_api.h"
#include "environment.h"
#include "repl_data.h"
#include "macro_data.h"
#include "context.h"
#include "vm/vmcode.h"
#include "primitive_map.h"

COMPILER_BEGIN

COMPILER_SCHEMESCRIPT_API bool load_lib(const std::string& libname, environment_map& env, repl_data& rd, macro_data& md, context& ctxt, VM::vmcode& code, const primitive_map& pm, const compiler_options& options);

COMPILER_SCHEMESCRIPT_API bool load_apply(environment_map& env, repl_data& rd, macro_data& md, context& ctxt, VM::vmcode& code, const primitive_map& pm, const compiler_options& options);
COMPILER_SCHEMESCRIPT_API bool load_modules(environment_map& env, repl_data& rd, macro_data& md, context& ctxt, VM::vmcode& code, const primitive_map& pm, const compiler_options& options);
COMPILER_SCHEMESCRIPT_API bool load_string_to_symbol(environment_map& env, repl_data& rd, macro_data& md, context& ctxt, VM::vmcode& code, const primitive_map& pm, const compiler_options& options);
COMPILER_SCHEMESCRIPT_API bool load_r5rs(environment_map& env, repl_data& rd, macro_data& md, context& ctxt, VM::vmcode& code, const primitive_map& pm, const compiler_options& options);
COMPILER_SCHEMESCRIPT_API bool load_callcc(environment_map& env, repl_data& rd, macro_data& md, context& ctxt, VM::vmcode& code, const primitive_map& pm, const compiler_options& options);


COMPILER_END
