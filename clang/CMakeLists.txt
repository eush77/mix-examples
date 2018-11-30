set(CMAKE_C_COMPILER ${CLANG})
set(CMAKE_C_FLAGS "-std=c11 -fPIC -O1 -Wall -Wextra")
set(CMAKE_CXX_FLAGS
  "-std=c++14 -fno-rtti -fPIC -g -O2 -Wall -Wextra -Wno-unused-parameter")

set(LLVM_TARGET_DEFINITIONS Options.td)
tablegen(${CMAKE_PROJECT_NAME} Options.inc -gen-opt-parser-defs
  -I${LLVM_INCLUDE_DIR})
add_public_tablegen_target(options)

add_library(main Main.cpp Example.cpp)
add_dependencies(main options)
target_include_directories(main PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(Power)
