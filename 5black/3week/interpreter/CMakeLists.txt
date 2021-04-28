cmake_minimum_required(VERSION 3.18)

project(main)

set(ADDITIONAL_HDRS
  test_runner.h
)

set(PROJECT_HDRS
  comparators.h
  lexer.h
  object_holder.h
  object.h
  parse.h
  runtime_error.h
  statement.h
)

set(PROJECT_SRCS
  comparators.cpp
  lexer_test.cpp
  lexer.cpp
  mython.cpp
  object_holder_test.cpp
  object_holder.cpp
  object_test.cpp
  object.cpp
  parse_test.cpp
  parse.cpp
  statement_test.cpp
  statement.cpp
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

include_directories(${CMAKE_SOURCE_DIR})

add_executable(main
  ${PROJECT_SRCS}
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

target_compile_features(main PUBLIC cxx_std_17)
target_link_libraries(main Threads::Threads)

# configure_file(example1.in.json example1.in.json COPYONLY)
# configure_file(example2.in.json example2.in.json COPYONLY)

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