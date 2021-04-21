cmake_minimum_required(VERSION 3.18)

project(main)

set(PROJECT_PROTOS
  contact.proto
)

set(PROJECT_HDRS
  phone_book.h
  iterator_range.h
)

set(PROJECT_SRCS
  main.cpp
  phone_book.cpp
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
# set(CMAKE_CXX_FLAGS "-Wall -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-omit-frame-pointer")
# set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak -fsanitize=undefined -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_LIBCPP_DEBUG=1")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
  ${PROJECT_PROTOS}
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

add_executable(main
  ${PROTO_SRCS} ${PROTO_HDRS}
  ${PROJECT_SRCS}
)

target_compile_features(main PUBLIC cxx_std_17)
target_link_libraries(main
  Threads::Threads
  ${Protobuf_LIBRARIES}
)

# configure_file(words.txt words.txt COPYONLY)

install(FILES
  ${PROJECT_PROTOS}
  ${PROJECT_SRCS}
  ${PROJECT_HDRS}
  DESTINATION .
)

set(CPACK_GENERATOR "7Z")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_VERBATIM_VARIABLES YES)
include(CPack)