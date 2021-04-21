cmake_minimum_required(VERSION 3.18)

project(main)

add_executable(main
    main.cpp
    command.cpp
    date.cpp
    event.cpp
    database.cpp
    token.cpp
    node.cpp
    condition_parser.cpp
    condition_parser_test.cpp
    test_runner.cpp
    tests.cpp
)

target_compile_features(main PUBLIC cxx_std_17)