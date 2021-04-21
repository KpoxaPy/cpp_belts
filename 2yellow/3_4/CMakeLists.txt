cmake_minimum_required(VERSION 3.18)

project(main)

add_executable(main
    main.cpp
    query.cpp
    responses.cpp
    bus_manager.cpp)

target_compile_features(main PUBLIC cxx_std_17)