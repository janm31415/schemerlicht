set(HDRS
compile_error.h
compiler_options.h
context.h
defines.h
dump.h
environment.h
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
compile_error.cpp
compiler_options.cpp
context.cpp
dump.cpp
format.cpp
macro_data.cpp
parse.cpp
preprocess.cpp
reader.cpp
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
