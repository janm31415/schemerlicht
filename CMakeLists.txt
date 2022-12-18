cmake_minimum_required(VERSION 3.10)
project (schemerlicht)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_PDB_OUTPUT_DIRECTORY     "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

set(SCHEMERLICHT_MODULES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/scm/" CACHE STRING "Provide the location of your Scheme library folder")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/schemerlicht/modulesconf.h.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/schemerlicht/modulesconf.h" @ONLY)

add_subdirectory(schemerlicht)
add_subdirectory(schemerlicht.tests)
add_subdirectory(schemerlicht.repl)
add_subdirectory(tools/primextract)
add_subdirectory(tools/primdispatch)
add_subdirectory(examples/life)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties (primextract PROPERTIES FOLDER tools)
set_target_properties (primdispatch PROPERTIES FOLDER tools)
set_target_properties (life PROPERTIES FOLDER examples)
