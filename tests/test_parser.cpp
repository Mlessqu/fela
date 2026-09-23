#include <doctest/doctest.h>
#include <string>
#include <vector>

#include "Lexer.h++"
#include "Parser.h++"

static std::vector<fela::Token> tokenize(std::string _source)
{
    std::vector<fela::Token> tokens;
    fela::Lexer lexer;
    lexer.load_source(std::move(_source));
    while (true)
    {
        fela::Token tok = lexer.next_token();
        tokens.push_back(tok);
        if (tok.type_ == fela::TokenType::eof)
        {
            break;
        }
    }
    return tokens;
}

TEST_CASE("parser parses global variable declarations")
{
    std::vector<fela::Token> tokens = tokenize("int a = 5; bool flag = true; int b;");
    fela::Parser parser(tokens);
    CHECK_NOTHROW(parser.parse_program());
}

TEST_CASE("parser parses function declarations")
{
    std::vector<fela::Token> tokens = tokenize("void foo(); int add(int a, int b);");
    fela::Parser parser(tokens);
    CHECK_NOTHROW(parser.parse_program());
}

TEST_CASE("parser parses function definition with return")
{
    std::vector<fela::Token> tokens = tokenize("void main() { return; }");
    fela::Parser parser(tokens);
    CHECK_NOTHROW(parser.parse_program());
}

TEST_CASE("parser parses if else and while statements")
{
    const std::string source =
        "void run(int x) {"
        "    while (x > 0) {"
        "        x = x - 1;"
        "    }"
        "    if (x == 0) {"
        "        return;"
        "    } else {"
        "        return;"
        "    }"
        "}";

    std::vector<fela::Token> tokens = tokenize(source);
    fela::Parser parser(tokens);
    CHECK_NOTHROW(parser.parse_program());
}

TEST_CASE("parser parses function call with arguments")
{
    const std::string source =
        "int add(int a, int b) {"
        "    return a + b;"
        "}"
        "void main() {"
        "    int res = add(1, 2);"
        "}";

    std::vector<fela::Token> tokens = tokenize(source);
    fela::Parser parser(tokens);
    CHECK_NOTHROW(parser.parse_program());
}
