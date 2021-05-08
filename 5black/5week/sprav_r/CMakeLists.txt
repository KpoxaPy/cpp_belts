cmake_minimum_required(VERSION 3.18)

project(main)

set(ADDITIONAL_HDRS
  test_runner.h
)

set(PROJECT_HDRS
  bus.h
  graph.h
  json.h
  macro.h
  map_builder.h
  pages.h
  paletter.h
  point_projector.h
  profile.h
  profile.h
  reader.h
  reader_tests.h
  render_settings.h
  request.h
  request_base_bus.h
  request_base_stop.h
  request_find.h
  request_map.h
  request_route.h
  request_stat_bus.h
  request_stat_stop.h
  router.h
  routing_settings.h
  serialization_settings.h
  sprav.h
  sprav_impl.h
  sprav_mapper.h
  sprav_tests.h
  spravio.h
  spravio_tests.h
  stop.h
  string_stream_utils.h
  string_view_utils.h
  svg.h
  tests.h
)

set(PROJECT_SRCS
  bus.cpp
  json.cpp
  main.cpp
  map_builder.cpp
  pages.cpp
  paletter.cpp
  point_projector.cpp
  profile.cpp
  reader_tests.cpp
  render_settings.cpp
  request.cpp
  request_base_bus.cpp
  request_base_stop.cpp
  request_find.cpp
  request_map.cpp
  request_route.cpp
  request_stat_bus.cpp
  request_stat_stop.cpp
  routing_settings.cpp
  serialization_settings.cpp
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

set(PROTOS
  address.proto
  company.proto
  database.proto
  database_queries.proto
  mapper.proto
  name.proto
  phone.proto
  render_settings.proto
  rubric.proto
  sphere.proto
  svg.proto
  transport_catalog.proto
  url.proto
  working_time.proto
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

find_package(Protobuf REQUIRED)

protobuf_generate_cpp(
  PROTO_SRCS
  PROTO_HDRS
  ${PROTOS}
)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(main
  ${PROJECT_SRCS}
  ${PROTO_SRCS}
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

target_compile_features(main PUBLIC cxx_std_17)
target_link_libraries(main Threads::Threads)
target_link_libraries(main ${Protobuf_LIBRARIES})

install(FILES
  ${PROJECT_HDRS}
  ${PROJECT_SRCS}
  ${PROTOS}
  DESTINATION .
)

set(CPACK_GENERATOR "7Z")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_VERBATIM_VARIABLES YES)
include(CPack)