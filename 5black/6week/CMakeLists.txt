cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(spreadsheet)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Werror -Wno-unused-parameter -Wno-implicit-fallthrough")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(ANTLR_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/antlr-4.7.2-complete.jar)
include(${CMAKE_CURRENT_SOURCE_DIR}/FindANTLR.cmake)

set(ADDITIONAL
  common.h
  formula.h
  main.cpp
  test_runner.h
)

set(PROJECT_HDRS
  binary_op.h
  cell_impl.h
  common_etc.h
  formula_impl.h
  formula_node.h
  formula_node_full.h
  formula_node_part.h
  hash_extra.h
  hashing.h
  macro.h
  paginator.h
  profile.h
  sheet_impl.h
  string_stream_utils.h
  string_view_utils.h
  unary_op.h
  utils_it.h
)

set(PROJECT_SRCS
  binary_op.cpp
  cell_impl.cpp
  formula_error.cpp
  formula_impl.cpp
  formula_node.cpp
  formula_parser.cpp
  position.cpp
  position.cpp
  profile.cpp
  sheet.cpp
  sheet_impl.cpp
  size.cpp
  string_view_utils.cpp
  unary_op.cpp
)

add_definitions(
  -DANTLR4CPP_STATIC
  -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
)

set(WITH_STATIC_CRT OFF CACHE BOOL "Visual C++ static CRT for ANTLR" FORCE)
add_subdirectory(antlr4_runtime)

antlr_target(FormulaParser Formula.g4 LEXER PARSER LISTENER)

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${ANTLR4_INCLUDE_DIRS}
  ${ANTLR_FormulaParser_OUTPUT_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/antlr4_runtime/runtime/src)

add_executable(spreadsheet
  ${ADDITIONAL}
  ${PROJECT_HDRS}
  ${PROJECT_SRCS}
  ${ANTLR_FormulaParser_CXX_OUTPUTS}
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

target_compile_features(spreadsheet PUBLIC cxx_std_17)
target_link_libraries(
  spreadsheet
  Threads::Threads
  antlr4_static
)

install(FILES
  ${PROJECT_HDRS}
  ${PROJECT_SRCS}
  DESTINATION .
)

set(CPACK_GENERATOR "7Z")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_VERBATIM_VARIABLES YES)
include(CPack)

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT spreadsheet)
