cmake_minimum_required(VERSION 3.18)

project(red_final)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(red_final
    main.cpp
    parse.cpp
    search_server.cpp
    index.cpp
    string_util.cpp
    search_result.cpp
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

target_link_libraries(red_final Threads::Threads)
target_compile_features(red_final PUBLIC cxx_std_17)

configure_file(words.txt words.txt COPYONLY)

install(FILES
    main.cpp
    parse.h
    parse.cpp
    search_server.h
    search_server.cpp
    index.h
    index.cpp
    string_util.h
    string_util.cpp
    search_result.h
    search_result.cpp
    iterator_range.h
    profile.h
    macro.h
    DESTINATION .
)

set(CPACK_GENERATOR "7Z")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_VERBATIM_VARIABLES YES)
include(CPack)