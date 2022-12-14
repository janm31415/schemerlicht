

set(HDRS
life.h
window.h
    )
	
set(SRCS
main.cpp
window.cpp
)

if (UNIX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")
endif (UNIX)

# general build definitions
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(gameoflife ${HDRS} ${SRCS})
source_group("Header Files" FILES ${hdrs})
source_group("Source Files" FILES ${srcs})
    
if (WIN32)
target_include_directories(gameoflife
   PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/../../
   )

target_link_libraries(gameoflife
    PRIVATE
    schemerlicht
    )
endif (WIN32)    

if (UNIX)

set(XLIBINCLUDE "")
set(XLIBLIBRARY "X11.so")

if (APPLE)
set(XLIBINCLUDE "/usr/X11/include/")
set(XLIBLIBRARY "/usr/X11/lib/libX11.dylib")
endif (APPLE)

target_include_directories(gameoflife
   PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/../../
   ${XLIBINCLUDE}
   )
   
target_link_libraries(gameoflife
    PRIVATE
    schemerlicht
    ${XLIBLIBRARY}
    )
endif (UNIX)
