set(HDRS
compile_error.h
defines.h
dump.h
format.h
namespace.h
parse.h
reader.h
schemescript_api.h
tokenize.h
visitor.h
)
	
set(SRCS
compile_error.cpp
dump.cpp
format.cpp
parse.cpp
reader.cpp
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
