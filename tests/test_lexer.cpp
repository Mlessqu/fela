#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "Lexer.h++"

constexpr unsigned int INITIAL_LINE_VAL = 1;
constexpr unsigned int FIRST_COLUMN_VAL = 1;
constexpr unsigned int SECOND_LINE_VAL = 2;

TEST_CASE("lexer returns eof on empty source")
{
    fela::Lexer lexer;
    lexer.load_source("");

    fela::Token tok = lexer.next_token();
    CHECK(tok.type_ == fela::TokenType::eof);
}

TEST_CASE("lexer recognizes keywords")
{
    fela::Lexer lexer;
    lexer.load_source("int bool void if else while return true false");

    CHECK(lexer.next_token().type_ == fela::TokenType::type_int);
    CHECK(lexer.next_token().type_ == fela::TokenType::type_bool);
    CHECK(lexer.next_token().type_ == fela::TokenType::type_void);
    CHECK(lexer.next_token().type_ == fela::TokenType::if_keyword);
    CHECK(lexer.next_token().type_ == fela::TokenType::else_keyword);
    CHECK(lexer.next_token().type_ == fela::TokenType::while_keyword);
    CHECK(lexer.next_token().type_ == fela::TokenType::return_keyword);
    CHECK(lexer.next_token().type_ == fela::TokenType::true_boolean);
    CHECK(lexer.next_token().type_ == fela::TokenType::false_boolean);
    CHECK(lexer.next_token().type_ == fela::TokenType::eof);
}

TEST_CASE("lexer recognizes operators and symbols")
{
    fela::Lexer lexer;
    lexer.load_source("; , = == + - ! != / * > < ( ) { } && ||");

    CHECK(lexer.next_token().type_ == fela::TokenType::semi);
    CHECK(lexer.next_token().type_ == fela::TokenType::coma);
    CHECK(lexer.next_token().type_ == fela::TokenType::assign);
    CHECK(lexer.next_token().type_ == fela::TokenType::equal_op);
    CHECK(lexer.next_token().type_ == fela::TokenType::plus);
    CHECK(lexer.next_token().type_ == fela::TokenType::minus);
    CHECK(lexer.next_token().type_ == fela::TokenType::negation_op);
    CHECK(lexer.next_token().type_ == fela::TokenType::not_equal_op);
    CHECK(lexer.next_token().type_ == fela::TokenType::divide_op);
    CHECK(lexer.next_token().type_ == fela::TokenType::multiply_op);
    CHECK(lexer.next_token().type_ == fela::TokenType::greater_op);
    CHECK(lexer.next_token().type_ == fela::TokenType::smaller_op);
    CHECK(lexer.next_token().type_ == fela::TokenType::open_group);
    CHECK(lexer.next_token().type_ == fela::TokenType::close_group);
    CHECK(lexer.next_token().type_ == fela::TokenType::open_scope);
    CHECK(lexer.next_token().type_ == fela::TokenType::close_scope);
    CHECK(lexer.next_token().type_ == fela::TokenType::and_op);
    CHECK(lexer.next_token().type_ == fela::TokenType::or_op);
    CHECK(lexer.next_token().type_ == fela::TokenType::eof);
}

TEST_CASE("lexer recognizes identifiers and numbers")
{
    fela::Lexer lexer;
    lexer.load_source("x _counter val123 42 0");

    fela::Token tok1 = lexer.next_token();
    CHECK(tok1.type_ == fela::TokenType::identifier);
    CHECK(tok1.payload_ == "x");

    fela::Token tok2 = lexer.next_token();
    CHECK(tok2.type_ == fela::TokenType::identifier);
    CHECK(tok2.payload_ == "_counter");

    fela::Token tok3 = lexer.next_token();
    CHECK(tok3.type_ == fela::TokenType::identifier);
    CHECK(tok3.payload_ == "val123");

    fela::Token tok4 = lexer.next_token();
    CHECK(tok4.type_ == fela::TokenType::integer_literal);
    CHECK(tok4.payload_ == "42");

    fela::Token tok5 = lexer.next_token();
    CHECK(tok5.type_ == fela::TokenType::integer_literal);
    CHECK(tok5.payload_ == "0");

    CHECK(lexer.next_token().type_ == fela::TokenType::eof);
}

TEST_CASE("lexer skips comments and whitespace")
{
    fela::Lexer lexer;
    lexer.load_source("// single line comment\n42 /* block comment */ return");

    fela::Token tok1 = lexer.next_token();
    CHECK(tok1.type_ == fela::TokenType::integer_literal);
    CHECK(tok1.payload_ == "42");
    CHECK(tok1.line_ == SECOND_LINE_VAL);

    fela::Token tok2 = lexer.next_token();
    CHECK(tok2.type_ == fela::TokenType::return_keyword);
    CHECK(tok2.payload_ == "return");

    CHECK(lexer.next_token().type_ == fela::TokenType::eof);
}

TEST_CASE("lexer tracks line and column numbers")
{
    fela::Lexer lexer;
    lexer.load_source("int");

    fela::Token tok = lexer.next_token();
    CHECK(tok.type_ == fela::TokenType::type_int);
    CHECK(tok.line_ == INITIAL_LINE_VAL);
    CHECK(tok.column_ == FIRST_COLUMN_VAL);
}
