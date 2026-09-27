#include <doctest/doctest.h>
#include <sstream>
#include <string>
#include <utility>

#include "AstWalk.h++"
#include "Lexer.h++"
#include "Parser.h++"

struct ParserFixture
{
    static std::string strip_indent(const std::string& _str)
    {
        std::string result;
        std::istringstream stream(_str);
        std::string line;
        while (std::getline(stream, line))
        {
            const size_t first = line.find_first_not_of(" \t\r");
            if (first != std::string::npos)
            {
                const size_t last = line.find_last_not_of(" \t\r");
                result += line.substr(first, last - first + 1);
            }
            result += '\n';
        }
        return result;
    }

    std::string dump(std::string _source)
    {
        INFO("Source info:", _source);
        fela::Lexer lexer;
        lexer.load_from_string(std::move(_source));
        auto tokens = lexer.tokenize();
        fela::Parser parser(tokens);
        auto program = parser.parse_program();
        if (!program)
        {
            return "<parse error>\n";
        }
        std::string ast_dump = fela::ast_dump_node(program.get());
        INFO("Actual AST Dump:\n", ast_dump);
        return strip_indent(ast_dump);
    }
};

TEST_SUITE("parser")
{
    TEST_CASE_FIXTURE(ParserFixture, "leaf node: integer literal")
    {
        const std::string expected =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with a identifier, with init value:\n"
            "    Literal expression node:\n"
            "    Value:42\n";

        CHECK(dump("int a = 42;") == strip_indent(expected));
    }

    TEST_CASE_FIXTURE(ParserFixture, "leaf node: boolean literals")
    {
        const std::string expected_true =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with flag identifier, with init value:\n"
            "    Literal expression node:\n"
            "    Value:true\n";

        CHECK(dump("bool flag = true;") == strip_indent(expected_true));

        const std::string expected_false =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with flag identifier, with init value:\n"
            "    Literal expression node:\n"
            "    Value:false\n";

        CHECK(dump("bool flag = false;") == strip_indent(expected_false));
    }

    TEST_CASE_FIXTURE(ParserFixture, "leaf node: variable declaration without init value")
    {
        const std::string expected =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with x identifier, without init value\n";

        CHECK(dump("int x;") == strip_indent(expected));
    }

    TEST_CASE_FIXTURE(ParserFixture, "leaf node: variable expression")
    {
        const std::string expected =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with x identifier, with init value:\n"
            "    Ast variable expression node:\n"
            "    identifier: y\n";

        CHECK(dump("int x = y;") == strip_indent(expected));
    }

    TEST_CASE_FIXTURE(ParserFixture, "leaf node: function declaration without params")
    {
        const std::string expected =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Function declaration node:\n"
            "  identifier: foo\n"
            "  return type: void\n"
            "  with 0 params:\n";

        CHECK(dump("void foo();") == strip_indent(expected));
    }
}
