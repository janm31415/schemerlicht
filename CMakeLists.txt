cmake_minimum_required(VERSION 2.8.11)
project (schemescript)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_PDB_OUTPUT_DIRECTORY     "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)


add_subdirectory(schemescript)
add_subdirectory(schemescript.tests)
add_subdirectory(vm)
add_subdirectory(vm.tests)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)