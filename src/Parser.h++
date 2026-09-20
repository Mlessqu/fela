#pragma once
#include <vector>

#include "Lexer.h++"
// so the parser will be responsible for checking grammar correctness syntax,
namespace fela
{
    class Parser
    {
    public:

        std::vector<Token> tokens_;
        size_t cursor_=0;
        const Token& consume_token();
        bool expect_and_consume(TokenType _expected_type, std::string _error_message);
    };
} // fela
