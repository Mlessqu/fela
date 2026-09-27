#pragma once
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
