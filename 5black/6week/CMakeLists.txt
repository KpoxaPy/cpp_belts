cmake_minimum_required(VERSION 3.18)

project(main)

set(ADDITIONAL_HDRS
  test_runner.h
)

set(PROJECT_HDRS
  hashing.h
  macro.h
  paginator.h
  profile.h
  string_stream_utils.h
  string_view_utils.h
  utils_it.h
)

set(PROJECT_SRCS
  profile.cpp
  string_view_utils.cpp
)

set(PROTOS
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