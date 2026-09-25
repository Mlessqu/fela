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
        src/AbstractSyntaxTree.cpp
        src/AbstractSyntaxTree.h++
        src/SymbolTable.cpp
        src/SymbolTable.h++
        src/SemanticChecker.cpp
        src/SemanticChecker.h++
)

add_executable(fela_tests
        tests/test_lexer.cpp
        src/Lexer.cpp
        src/Lexer.h++
        src/Parser.cpp
        src/Parser.h++
        src/AbstractSyntaxTree.cpp
        src/AbstractSyntaxTree.h++
        src/SymbolTable.cpp
        src/SymbolTable.h++
        src/SemanticChecker.cpp
        src/SemanticChecker.h++
        tests/test_parser.cpp)
target_include_directories(fela_tests PRIVATE src)

