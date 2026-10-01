#pragma once
#include <doctest/doctest.h>
#include <string>
#include <string_view>
#include <utility>

#include "Lexer.h++"
#include "Parser.h++"
#include "SemanticChecker.h++"

struct SemanticFixture
{
    fela::SemanticChecker checker_;

    bool check(std::string _source)
    {
        INFO("Source:\n", _source);
        fela::Lexer lexer;
        lexer.load_from_string(std::move(_source));
        auto tokens = lexer.tokenize();
        fela::Parser parser(tokens);
        auto program = parser.parse_program();
        REQUIRE(program != nullptr);
        return checker_.check(program.get());
    }

    bool has_error_containing(std::string_view _substr) const
    {
        for (const auto& err : checker_.errors())
        {
            if (err.find(_substr) != std::string::npos)
            {
                return true;
            }
        }
        return false;
    }
};
