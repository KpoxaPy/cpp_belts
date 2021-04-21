cmake_minimum_required(VERSION 3.18)

project(main)

set(PROJECT_HDRS
  lexer.h
  test_runner.h
)

set(PROJECT_SRCS
  lexer.cpp
  lexer_test.cpp
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

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
  lexer.h
  lexer.cpp
  DESTINATION .
)

set(CPACK_GENERATOR "7Z")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_VERBATIM_VARIABLES YES)
include(CPack)