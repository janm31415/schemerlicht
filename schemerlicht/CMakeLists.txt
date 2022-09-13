set(HDRS
begconv.h
schemerlicht.h
context.h
cps.h
defconv.h
dump.h
error.h
limits.h
map.h
memory.h
object.h
parser.h
reader.h
simplifyconv.h
stream.h
string.h
tailcall.h
token.h
vector.h
visitor.h
)
	
set(SRCS
begconv.c
context.c
cps.c
defconv.c
dump.c
error.c
map.c
memory.c
object.c
parser.c
reader.c
simplifyconv.c
stream.c
string.c
tailcall.c
token.c
visitor.c
)

if (WIN32)
set(CMAKE_C_FLAGS_DEBUG "/W4 /MP /GF /RTCu /Od /MDd /Zi")
set(CMAKE_C_FLAGS_RELEASE "/W4 /MP /GF /O2 /Ob2 /Oi /Ot /MD /Zi /DNDEBUG")
endif(WIN32)

add_library(schemerlicht STATIC ${HDRS} ${SRCS})
source_group("Header Files" FILES ${hdrs})
source_group("Source Files" FILES ${srcs})

target_include_directories(schemerlicht
    PRIVATE
	  ${CMAKE_CURRENT_SOURCE_DIR}/..
    )	
	
target_link_libraries(schemerlicht
    PRIVATE		  
    )
    