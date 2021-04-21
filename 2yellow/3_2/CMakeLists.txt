cmake_minimum_required(VERSION 3.18)

project(main)

add_executable(main
    main.cpp
    tests.cpp
    phone_number.cpp
    test_runner.cpp)

target_compile_features(main PUBLIC cxx_std_17)