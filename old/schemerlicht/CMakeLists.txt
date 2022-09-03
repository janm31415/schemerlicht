set(HDRS
alpha_conversion.h
asm_aux.h
assignable_var_conversion.h
cinput_conversion.h
cinput_data.h
closure_conversion.h
compile_data.h
compile_error.h
compiler.h
compiler_options.h
concurrency.h
constant_folding.h
constant_propagation.h
context.h
context_defs.h
cps_conversion.h
define_conversion.h
dump.h
environment.h
file_utils.h
filename_setter.h
format.h
free_var_analysis.h
global_define_env.h
globals.h
include_handler.h
inline_primitives_conversion.h
inlines.h
lambda_to_let_conversion.h
linear_scan.h
linear_scan_index.h
liveness_range.h
load_eval.h
load_lib.h
macro_data.h
macro_expander.h
namespace.h
parse.h
preprocess.h
primitive_map.h
primitives.h
primitives_lib.h
quasiquote_conversion.h
quote_collector.h
quote_conversion.h
reader.h
reg_alloc.h
reg_alloc_map.h
remove_single_begins.h
repl_data.h
runtime.h
schemerlicht.h
schemerlicht_api.h
simplify_to_core.h
single_begin_conversion.h
syscalls.h
tail_call_analysis.h
tail_calls_check.h
tokenize.h
types.h
utf8.h
visitor.h
)
	
set(SRCS
alpha_conversion.cpp
asm_aux.cpp
assignable_var_conversion.cpp
cinput_conversion.cpp
closure_conversion.cpp
compile_data.cpp
compile_error.cpp
compiler.cpp
compiler_options.cpp
constant_folding.cpp
constant_propagation.cpp
context.cpp
cps_conversion.cpp
define_conversion.cpp
dump.cpp
file_utils.cpp
filename_setter.cpp
format.cpp
free_var_analysis.cpp
global_define_env.cpp
globals.cpp
include_handler.cpp
inline_primitives_conversion.cpp
inlines.cpp
lambda_to_let_conversion.cpp
linear_scan.cpp
linear_scan_index.cpp
load_lib.cpp
macro_data.cpp
macro_expander.cpp
parse.cpp
preprocess.cpp
primitives.cpp
primitives_lib.cpp
quasiquote_conversion.cpp
quote_collector.cpp
quote_conversion.cpp
reader.cpp
reg_alloc.cpp
reg_alloc_map.cpp
remove_single_begins.cpp
repl_data.cpp
runtime.cpp
schemerlicht.cpp
simplify_to_core.cpp
single_begin_conversion.cpp
syscalls.cpp
tail_call_analysis.cpp
tail_calls_check.cpp
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

add_library(schemerlicht SHARED ${HDRS} ${SRCS})
source_group("Header Files" FILES ${hdrs})
source_group("Source Files" FILES ${srcs})

target_include_directories(schemerlicht
    PRIVATE
	  ${CMAKE_CURRENT_SOURCE_DIR}/..
    )	
	
target_link_libraries(schemerlicht
    PRIVATE	
	  vm
    )	

if (WIN32)
add_custom_command(TARGET schemerlicht POST_BUILD 
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/scm" "${CMAKE_CURRENT_BINARY_DIR}/scm")
endif (WIN32)


add_custom_command(TARGET schemerlicht POST_BUILD 
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/scm" "$<TARGET_FILE_DIR:vm.tests>/scm")
