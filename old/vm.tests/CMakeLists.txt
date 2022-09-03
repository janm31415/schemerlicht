
set(HDRS
test_assert.h
vmtests.h
    )
	
set(SRCS
test_assert.cpp
test.cpp
vmtests.cpp
)

# general build definitions
add_definitions(-D_SCL_SECURE_NO_WARNINGS)

add_definitions(-DMEMORY_LEAK_TRACKING)

add_executable(vm.tests ${HDRS} ${SRCS})
source_group("Header Files" FILES ${hdrs})
source_group("Source Files" FILES ${srcs})

target_include_directories(vm.tests
    PRIVATE
	  ${CMAKE_CURRENT_SOURCE_DIR}/../
    )
	
target_link_libraries(vm.tests
    PRIVATE
    vm
    )	
