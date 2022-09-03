

set(HDRS
    )
	
set(SRCS
main.cpp
)

if (WIN32)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:8388608")
endif (WIN32)

#if (UNIX)
#  if (APPLE)
#    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-stack_size,0x120000")
#  else()
#    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,stack-size=16000000")
#  endif (APPLE)
#endif (UNIX)

# general build definitions
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(repl ${HDRS} ${SRCS})
source_group("Header Files" FILES ${hdrs})
source_group("Source Files" FILES ${srcs})

 target_include_directories(repl
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    )	
	
target_link_libraries(repl
    PRIVATE
    vm
    schemerlicht
    )	

add_custom_command(TARGET repl POST_BUILD 
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../schemerlicht/scm" "$<TARGET_FILE_DIR:repl>/scm") 