set(HDRS
)
	
set(SRCS
main.cpp
)

if (WIN32)
set(CMAKE_C_FLAGS_DEBUG "/W4 /MP /GF /RTCu /Od /MDd /Zi")
set(CMAKE_C_FLAGS_RELEASE "/W4 /MP /GF /O2 /Ob2 /Oi /Ot /MD /Zi /DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "/W4 /MP /GF /RTCu /Od /MDd /Zi")
set(CMAKE_CXX_FLAGS_RELEASE "/W4 /MP /GF /O2 /Ob2 /Oi /Ot /MD /Zi /DNDEBUG")
else(WIN32)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11")
endif(WIN32)

add_definitions(-DMEMORY_LEAK_TRACKING)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(primdispatch ${HDRS} ${SRCS})
source_group("Header Files" FILES ${hdrs})
source_group("Source Files" FILES ${srcs})

target_include_directories(primdispatch
    PRIVATE
	  ${CMAKE_CURRENT_SOURCE_DIR}/..
    )	
	
target_link_libraries(primdispatch
    PRIVATE		    
    )    
