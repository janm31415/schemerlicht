set(HDRS
schemerlicht.h
scontext.h
sdump.h
serror.h
slimits.h
smap.h
smemory.h
sobject.h
sparser.h
sstream.h
sstring.h
stoken.h
svector.h
svisitor.h
)
	
set(SRCS
scontext.c
sdump.c
serror.c
smap.c
smemory.c
sobject.c
sparser.c
sstream.c
sstring.c
stoken.c
svisitor.c
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
    