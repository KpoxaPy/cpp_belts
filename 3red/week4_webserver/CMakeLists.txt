cmake_minimum_required(VERSION 3.18)

project(main)

add_executable(main
    server_stats.cpp
    stats.cpp
)

target_compile_features(main PUBLIC cxx_std_17)