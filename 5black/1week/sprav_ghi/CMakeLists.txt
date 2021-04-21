cmake_minimum_required(VERSION 3.18)

project(main)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_executable(main
  bus.cpp
  json.cpp
  main.cpp
  profile.cpp
  reader_tests.cpp
  render_settings.cpp
  request.cpp
  request_base_bus.cpp
  request_base_stop.cpp
  request_map.cpp
  request_route.cpp
  request_stat_bus.cpp
  request_stat_stop.cpp
  routing_settings.cpp
  sprav.cpp
  sprav_impl.cpp
  sprav_mapper.cpp
  sprav_tests.cpp
  spravio.cpp
  spravio_tests.cpp
  stop.cpp
  string_view_utils.cpp
  svg.cpp
  tests.cpp
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

target_compile_features(main PUBLIC cxx_std_17)
target_link_libraries(main Threads::Threads)

configure_file(example1.in.json example1.in.json COPYONLY)
configure_file(example2.in.json example2.in.json COPYONLY)

install(FILES
  bus.cpp
  bus.h
  graph.h
  json.cpp
  json.h
  macro.h
  main.cpp
  profile.cpp
  profile.h
  profile.h
  reader.h
  reader_tests.cpp
  reader_tests.h
  render_settings.cpp
  render_settings.h
  request.cpp
  request.h
  request_base_bus.cpp
  request_base_bus.h
  request_base_stop.cpp
  request_base_stop.h
  request_map.cpp
  request_map.h
  request_route.cpp
  request_route.h
  request_stat_bus.cpp
  request_stat_bus.h
  request_stat_stop.cpp
  request_stat_stop.h
  router.h
  routing_settings.cpp
  routing_settings.h
  sprav.cpp
  sprav.h
  sprav_impl.cpp
  sprav_impl.h
  sprav_mapper.cpp
  sprav_mapper.h
  sprav_tests.cpp
  sprav_tests.h
  spravio.cpp
  spravio.h
  spravio_tests.cpp
  spravio_tests.h
  stop.cpp
  stop.h
  string_stream_utils.h
  string_view_utils.cpp
  string_view_utils.h
  svg.cpp
  svg.h
  tests.cpp
  tests.h
  DESTINATION .
)

set(CPACK_GENERATOR "7Z")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_VERBATIM_VARIABLES YES)
include(CPack)