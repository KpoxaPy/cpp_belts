cmake_minimum_required(VERSION 3.16)

project(main)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Werror")
# set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak -fsanitize=undefined -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_LIBCPP_DEBUG=1")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_executable(main
  descriptions.cpp
  json.cpp
  main.cpp
  requests.cpp
  sphere.cpp
  transport_catalog.cpp
  transport_router.cpp
  utils.cpp
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

target_compile_features(main PUBLIC cxx_std_17)
target_link_libraries(main Threads::Threads)

# configure_file(words.txt words.txt COPYONLY)

install(FILES
  descriptions.cpp
  descriptions.h
  graph.h
  json.cpp
  json.h
  main.cpp
  requests.cpp
  requests.h
  router.h
  sphere.cpp
  sphere.h
  transport_catalog.cpp
  transport_catalog.h
  transport_router.cpp
  transport_router.h
  utils.cpp
  utils.h
  DESTINATION .
)

set(CPACK_GENERATOR "7Z")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_VERBATIM_VARIABLES YES)
include(CPack)