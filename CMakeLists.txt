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
        src/AstWalk.cpp
        src/AstWalk.h++
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
        src/AstWalk.cpp
        src/AstWalk.h++
        tests/ParserFixture.h++
        tests/test_parser_leaves.cpp
        tests/test_parser_expressions.cpp
        tests/test_parser_instructions.cpp
        tests/SemanticFixture.h++
        tests/test_semantic_checker.cpp)
target_include_directories(fela_tests PRIVATE src)

