#pragma once

#include <string>

#include "namespace.h"
#include "schemerlicht_api.h"
#include "environment.h"

#include <ostream>
#include <memory>


COMPILER_BEGIN

struct context;
struct environment_entry;
struct repl_data;

COMPILER_SCHEMERLICHT_API void print_last_global_variable_used(std::ostream& out, std::shared_ptr<environment<environment_entry>> env, const repl_data& rd, const context* p_ctxt);

COMPILER_SCHEMERLICHT_API void scheme_runtime(uint64_t rax, std::ostream& out, std::shared_ptr<environment<environment_entry>> env, const repl_data& rd, const context* p_ctxt);

COMPILER_END
