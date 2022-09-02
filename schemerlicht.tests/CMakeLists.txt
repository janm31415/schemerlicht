set(HDRS
compile_tests.h
conversion_tests.h
format_tests.h
parse_tests.h
preprocess_tests.h
schemerlicht_tests.h
test_assert.h
tokenize_tests.h
    )
	
set(SRCS
compile_tests.cpp
conversion_tests.cpp
format_tests.cpp
parse_tests.cpp
preprocess_tests.cpp
schemerlicht_tests.cpp
test.cpp
test_assert.cpp
tokenize_tests.cpp
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

add_executable(schemerlicht.tests ${HDRS} ${SRCS})
source_group("Header Files" FILES ${hdrs})
source_group("Source Files" FILES ${srcs})

target_include_directories(schemerlicht.tests
    PRIVATE
	  ${CMAKE_CURRENT_SOURCE_DIR}/..
    )
	
target_link_libraries(schemerlicht.tests
    PRIVATE
    vm
    schemerlicht
    )	

if (WIN32)
add_custom_command(TARGET schemerlicht.tests POST_BUILD 
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/data" ${CMAKE_CURRENT_BINARY_DIR}/data)
endif (WIN32)

if (UNIX)
add_custom_command(TARGET schemerlicht.tests POST_BUILD 
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/data" "$<TARGET_FILE_DIR:schemerlicht.tests>/data")
endif (UNIX)
