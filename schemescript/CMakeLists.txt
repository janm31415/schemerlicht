set(HDRS
runtime.h
reg_alloc.h
reg_alloc_map.h
compile_data.h
asm_aux.h
context_defs.h
inlines.h
primitives.h
globals.h
syscalls.h
file_utils.h
utf8.h
alpha_conversion.h
assignable_var_conversion.h
cps_conversion.h
closure_conversion.h
constant_folding.h
constant_propagation.h
define_conversion.h
free_var_analysis.h
global_define_env.h
inline_primitives_conversion.h
lambda_to_let_conversion.h
include_handler.h
linear_scan.h
linear_scan_index.h
macro_expander.h
quasiquote_conversion.h
quote_collector.h
quote_conversion.h
remove_single_begins.h
single_begin_conversion.h
tail_call_analysis.h
tail_calls_check.h
compile_error.h
compiler_options.h
concurrency.h
context.h
defines.h
dump.h
environment.h
filename_setter.h
format.h
liveness_range.h
macro_data.h
namespace.h
parse.h
preprocess.h
primitive_map.h
reader.h
repl_data.h
schemescript_api.h
simplify_to_core.h
tokenize.h
types.h
visitor.h
)
	
set(SRCS
runtime.cpp
reg_alloc.cpp
reg_alloc_map.cpp
compile_data.cpp
asm_aux.cpp
inlines.cpp
asm_aux.cpp
globals.cpp
primitives.cpp
syscalls.cpp
file_utils.cpp
alpha_conversion.cpp
assignable_var_conversion.cpp
cps_conversion.cpp
closure_conversion.cpp
constant_folding.cpp
constant_propagation.cpp
define_conversion.cpp
free_var_analysis.cpp
global_define_env.cpp
inline_primitives_conversion.cpp
lambda_to_let_conversion.cpp
include_handler.cpp
linear_scan.cpp
linear_scan_index.cpp
macro_expander.cpp
quasiquote_conversion.cpp
quote_collector.cpp
quote_conversion.cpp
remove_single_begins.cpp
single_begin_conversion.cpp
tail_call_analysis.cpp
tail_calls_check.cpp

compile_error.cpp
compiler_options.cpp
context.cpp
dump.cpp
filename_setter.cpp
format.cpp
macro_data.cpp
parse.cpp
preprocess.cpp
reader.cpp
repl_data.cpp
simplify_to_core.cpp
tokenize.cpp
)

if (WIN32)
set(CMAKE_C_FLAGS_DEBUG "/W4 /MP /GF /RTCu /Od /MDd /Zi")
set(CMAKE_CXX_FLAGS_DEBUG "/W4 /MP /GF /RTCu /Od /MDd /Zi")
set(CMAKE_C_FLAGS_RELEASE "/W4 /MP /GF /O2 /Ob2 /Oi /Ot /MD /Zi /DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "/W4 /MP /GF /O2 /Ob2 /Oi /Ot /MD /Zi /DNDEBUG")
endif(WIN32)

# general build definitions
add_definitions(-D_UNICODE)
add_definitions(-DUNICODE)
add_definitions(-D_SCL_SECURE_NO_WARNINGS)

add_library(schemescript SHARED ${HDRS} ${SRCS})
source_group("Header Files" FILES ${hdrs})
source_group("Source Files" FILES ${srcs})

target_include_directories(schemescript
    PRIVATE
	  ${CMAKE_CURRENT_SOURCE_DIR}/..
    )	
	
target_link_libraries(schemescript
    PRIVATE	
	  vm
    )	
