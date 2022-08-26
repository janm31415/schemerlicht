set(HDRS
vm_api.h
vmcode.h
namespace.h
vm.h
)
	
set(SRCS
vmcode.cpp
vm.cpp
)

# general build definitions
add_definitions(-D_SCL_SECURE_NO_WARNINGS)

add_library(vm SHARED ${HDRS} ${SRCS})
source_group("Header Files" FILES ${hdrs})
source_group("Source Files" FILES ${srcs})

target_include_directories(vm
    PRIVATE
    )	
	
target_link_libraries(vm
    PRIVATE	
    )	
