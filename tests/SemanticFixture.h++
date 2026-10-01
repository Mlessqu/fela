#pragma once
#include <doctest/doctest.h>
#include <iostream>
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
        if (!program)
        {
            std::cout << "Parser error: failed to parse source\n";
            return false;
        }
        checker_ = fela::SemanticChecker{};
        bool result = checker_.check(program.get());
        for (const auto& err : checker_.errors())
        {
            std::cout << err;
        }
        return result;
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
