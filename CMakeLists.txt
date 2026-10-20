cmake_minimum_required(VERSION 4.3)
project(fela)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

llvm_map_components_to_libnames(llvm_libs core support)

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
        src/CodeGen.cpp
        src/CodeGen.h++
        src/Backend.cpp
        src/Backend.h++
)
target_link_libraries(fela PRIVATE LLVM)

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

