cmake_minimum_required(VERSION 3.18)

project(main)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_executable(main
  main.cpp
  Solution.cpp
  Textures.cpp
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

target_compile_features(main PUBLIC cxx_std_17)
target_link_libraries(main Threads::Threads)

# configure_file(words.txt words.txt COPYONLY)

# install(FILES
#     main.cpp
#     parse.h
#     parse.cpp
#     search_server.h
#     search_server.cpp
#     index.h
#     index.cpp
#     string_util.h
#     string_util.cpp
#     search_result.h
#     search_result.cpp
#     iterator_range.h
#     profile.h
#     macro.h
#     DESTINATION .
# )

# set(CPACK_GENERATOR "7Z")
# set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}")
# set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
# set(CPACK_VERBATIM_VARIABLES YES)
# include(CPack)