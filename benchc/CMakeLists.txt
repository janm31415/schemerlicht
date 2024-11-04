set(HDRS
)
	
set(SRCS
main.c
)

if (WIN32)
set(CMAKE_C_FLAGS_DEBUG "/W4 /MP /GF /RTCu /Od /MDd /Zi")
set(CMAKE_C_FLAGS_RELEASE "/W4 /MP /GF /O2 /Ob2 /Oi /Ot /MD /Zi /DNDEBUG")
endif(WIN32)

add_definitions(-DMEMORY_LEAK_TRACKING)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(benchc ${HDRS} ${SRCS})
source_group("Header Files" FILES ${hdrs})
source_group("Source Files" FILES ${srcs})

target_include_directories(benchc
    PRIVATE
	  ${CMAKE_CURRENT_SOURCE_DIR}/..
    )	
	
target_link_libraries(benchc
    PRIVATE		 
    schemerlicht
    )    

if (WIN32)
add_custom_command(TARGET benchc POST_BUILD 
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../bench" ${CMAKE_CURRENT_BINARY_DIR}/bench)
endif (WIN32)

add_custom_command(TARGET benchc POST_BUILD 
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../bench" "$<TARGET_FILE_DIR:benchc>/bench")