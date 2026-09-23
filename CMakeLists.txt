cmake_minimum_required(VERSION 4.3)
project(fela)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(fela
        src/main.cpp
        src/Lexer.cpp
        src/Lexer.h++
        src/Parser.cpp
        src/Parser.h++
)

add_executable(fela_tests
        tests/test_lexer.cpp
        src/Lexer.cpp
        src/Lexer.h++
        tests/test_parser.cpp)
target_include_directories(fela_tests PRIVATE src)

