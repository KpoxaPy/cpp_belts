cmake_minimum_required(VERSION 3.18)

project(main)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_executable(main
  stop.cpp
  bus.cpp
  reader_tests.cpp
  request.cpp
  main.cpp
  string_view_utils.cpp
  sprav.cpp
  sprav_tests.cpp
  spravio.cpp
  spravio_tests.cpp
  tests.cpp
  json.cpp
  profile.cpp
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

target_compile_features(main PUBLIC cxx_std_17)
target_link_libraries(main Threads::Threads)

configure_file(input4.json input.json COPYONLY)
# configure_file(input_coursera_1.json input.json COPYONLY)

install(FILES
  router.h
  graph.h
  request.h
  request.cpp
  json.h
  json.cpp
  stop.h
  stop.cpp
  bus.h
  bus.cpp
  main.cpp
  profile.h
  reader.h
  reader_tests.h
  reader_tests.cpp
  string_view_utils.h
  string_view_utils.cpp
  string_stream_utils.h
  sprav.h
  sprav.cpp
  sprav_tests.h
  sprav_tests.cpp
  spravio.h
  spravio.cpp
  spravio_tests.h
  spravio_tests.cpp
  tests.h
  tests.cpp
  profile.h
  profile.cpp
  macro.h
  DESTINATION .
)

set(CPACK_GENERATOR "7Z")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_VERBATIM_VARIABLES YES)
include(CPack)