#include "Parser.h++"
#include<fmt/core.h>

namespace fela
{
    const Token& Parser::get_current_token()
    {
        return tokens_[cursor_];
    }


    bool Parser::is_type(TokenType _type)
    {
        return get_current_token().type_ == _type;
    }


    bool Parser::is_not_type(TokenType _type)
    {
        return !is_type(_type);
    }


    const Token& Parser::consume_token()
    {
        const Token& token = tokens_[cursor_];
        if (token.type_ != TokenType::eof)
        {
            cursor_++;
        }
        return token;
    }


    bool Parser::expect_and_consume(TokenType _expected_type, std::string _error_message)
    {
        if (is_type(_expected_type))
        {
            consume_token();
            return true;
        }
        //scawy error msg here
        return false;
    }


    Parser::Parser(std::vector<Token> _tokens)
    {
    }


    void Parser::parse_program()
    {
        //entrypoint grammar here
        while (is_not_type(TokenType::eof))
        {
            parse_instruction();
        }
        std::cout << "ayo parsed";
    }


    void Parser::parse_instruction()
    {
        if (is_type(TokenType::return_keyword))
        {
        }
        else if (is_type(TokenType::if_keyword))
        {
            //parse if instruction
        }
        else if (is_type(TokenType::else_keyword))
        {
            //parse else instruction
        }
        else if (is_type(TokenType::while_keyword))
        {
            //parse while instruction
        }
        else if (is_type(TokenType::open_group))
        {
            //parse group instruction
        }
        else if (is_type(TokenType::type_int) || is_type(TokenType::type_bool))
        {
            //parse variable declaration
        }
        else
        {
            //parse expressions
            parse_expression();
        }
    }


    void Parser::parse_return_instruction()
    {
    }


    void Parser::parse_while_instruction()
    {
    }


    void Parser::parse_if_instruction()
    {
    }


    void Parser::parse_variable_declaration_instruction()
    {
    }


    void Parser::parse_block_instruction()
    {
    }


    void Parser::parse_assign_instruction()
    {
    }


    void Parser::parse_primary_instruction()
    {
    }


    void Parser::parse_expression()
    {
        if (is_type(TokenType::open_group))
        {
            parse_grouped_expression();
        }else if (is_type(TokenType::integer_literal) || is_type(TokenType::boolean_literal) || is_type(TokenType::identifier) )
        {
            parse_primary_expression();
        }else if (is_type(TokenType::negation_op)|| is_type(TokenType::minus))
        {
            parse_unary_expression();
        }else //...
    }


    void Parser::parse_grouped_expression()
    {
    }


    void Parser::parse_primary_expression()
    {
    }


    void Parser::parse_unary_expression()
    {
    }


    void Parser::parse_multiplying_expression()
    {
    }


    void Parser::parse_additive_expression()
    {
    }


    void Parser::parse_relational_expression()
    {
    }


    void Parser::parse_equality_expression()
    {
    }


    void Parser::parse_boolean_logic_and_expression()
    {
    }


    void Parser::parse_boolean_logic_or_expression()
    {
    }


    //implementation details here
} // fela
