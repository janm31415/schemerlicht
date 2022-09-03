#pragma once

#include "vm/vm.h"
#include "vm/vmcode.h"
#include "namespace.h"

#include <map>
#include <string>
#include <vector>

#include "compiler_options.h"
#include "context.h"
#include "schemerlicht_api.h"
#include "primitive_map.h"
#include "repl_data.h"
#include "environment.h"

COMPILER_BEGIN

COMPILER_SCHEMERLICHT_API void compile_primitives_library(primitive_map& pm, repl_data& rd, environment_map& env, context& ctxt, VM::vmcode& code, const compiler_options& options);
COMPILER_SCHEMERLICHT_API void assign_primitive_addresses(primitive_map& pm, const VM::first_pass_data& d, uint64_t address_start);

COMPILER_END
