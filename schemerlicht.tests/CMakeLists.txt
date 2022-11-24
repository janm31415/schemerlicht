set(HDRS
compiler_tests.h
conv_tests.h
copy_tests.h
dump_tests.h
environment_tests.h
map_tests.h
module_tests.h
parser_tests.h
pool_tests.h
reader_tests.h
stream_tests.h
string_tests.h
stringvec_tests.h
test_assert.h
token_tests.h
vector_tests.h
)
	
set(SRCS
compiler_tests.c
conv_tests.c
copy_tests.c
dump_tests.c
environment_tests.c
map_tests.c
module_tests.c
parser_tests.c
pool_tests.c
reader_tests.c
stream_tests.c
string_tests.c
stringvec_tests.c
test.c
test_assert.c
token_tests.c
vector_tests.c
)

#if (WIN32)
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:8388608")
#endif (WIN32)

if (WIN32)
set(CMAKE_C_FLAGS_DEBUG "/W4 /MP /GF /RTCu /Od /MDd /Zi")
set(CMAKE_C_FLAGS_RELEASE "/W4 /MP /GF /O2 /Ob2 /Oi /Ot /MD /Zi /DNDEBUG")
endif(WIN32)

add_definitions(-DMEMORY_LEAK_TRACKING)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(schemerlicht.tests ${HDRS} ${SRCS})
source_group("Header Files" FILES ${hdrs})
source_group("Source Files" FILES ${srcs})

target_include_directories(schemerlicht.tests
    PRIVATE
	  ${CMAKE_CURRENT_SOURCE_DIR}/..
    )	
	
target_link_libraries(schemerlicht.tests
    PRIVATE		 
    schemerlicht
    )
    

if (WIN32)
add_custom_command(TARGET schemerlicht.tests POST_BUILD 
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/data" ${CMAKE_CURRENT_BINARY_DIR}/data)
endif (WIN32)

add_custom_command(TARGET schemerlicht.tests POST_BUILD 
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/data" "$<TARGET_FILE_DIR:schemerlicht.tests>/data")