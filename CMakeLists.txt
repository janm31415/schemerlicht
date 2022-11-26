cmake_minimum_required(VERSION 3.10)
project (schemerlicht)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_PDB_OUTPUT_DIRECTORY     "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

set(schemerlicht_modules_path "${CMAKE_CURRENT_SOURCE_DIR}/scm/")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/schemerlicht/modulesconf.h.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/schemerlicht/modulesconf.h" @ONLY)

add_subdirectory(schemerlicht)
add_subdirectory(schemerlicht.tests)
add_subdirectory(schemerlicht.repl)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
