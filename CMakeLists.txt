cmake_minimum_required(VERSION 3.12)
project(MIX_EXAMPLES LANGUAGES C CXX)

find_package(LLVM REQUIRED)
message(STATUS "Found LLVM in ${LLVM_INSTALL_PREFIX}")
list(APPEND CMAKE_MODULE_PATH ${LLVM_CMAKE_DIR})
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
llvm_map_components_to_libnames(LLVM_LIBS
  core executionengine native object option orcjit support)
link_libraries(${LLVM_LIBS})

if(NOT ${CMAKE_PROJECT_NAME}_TABLEGEN_EXE)
  find_program(${CMAKE_PROJECT_NAME}_TABLEGEN_EXE llvm-tblgen)
endif()
include(TableGen)

find_program(CLANG clang PATHS ${LLVM_TOOLS_BINARY_DIR} NO_DEFAULT_PATH)
if(EXISTS ${CLANG})
  message(STATUS "Found ${CLANG}, building C examples")
  add_subdirectory(clang)
endif()
