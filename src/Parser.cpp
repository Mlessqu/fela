#include "Parser.h++"
#define FMT_HEADER_ONLY
#include <fmt/format.h>

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


    std::string Parser::format_expected_error(std::string_view _expected)
    {
        const Token& tok = get_current_token();
        return fmt::format("At line {}, col {} expected '{}' but got '{}'", tok.line_, tok.column_, _expected, tok.payload_);
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

    }


    void Parser::parse_instruction()
    {
        if (is_type(TokenType::return_keyword))
        {
            parse_return_instruction();
        }
        else if (is_type(TokenType::if_keyword))
        {
            parse_if_instruction();
        }
        else if (is_type(TokenType::while_keyword))
        {
            parse_while_instruction();
        }
        else if (is_type(TokenType::open_group))
        {
            parse_block_instruction();
        }
        else if (is_type(TokenType::type_int) || is_type(TokenType::type_bool))
        {
            parse_variable_declaration_instruction();
        }else if (is_type(TokenType::identifier)&& look_ahead(1) == TokenType::assign)
        {
            parse_assign_instruction();
        }
        else
        {
            parse_primary_instruction();
        }
    }


    void Parser::parse_return_instruction()
    {
        consume_token();
        //how to identify return type?
    }


    void Parser::parse_while_instruction()
    {
        consume_token();
        expect_and_consume(TokenType::open_group,"Invalid syntax, expected '(' after while");
        parse_expression();
        expect_and_consume(TokenType::close_group,format_expected_error(")"));
        parse_instruction();
    }


    void Parser::parse_if_instruction()
    {
        consume_token();
        expect_and_consume(TokenType::open_group, "Invalid syntax, expected '(' after if");
        parse_expression();
        expect_and_consume(TokenType::close_group,format_expected_error(")"));
        parse_instruction();
        if (is_type(TokenType::else_keyword))
        {
            consume_token();
            parse_instruction();
        }
    }


    void Parser::parse_variable_declaration_instruction()
    {
        consume_token();
        expect_and_consume(TokenType::identifier,"Unexpected identifier syntax");
        if (is_type(TokenType::assign))
        {
            consume_token();
            parse_expression();
        }
        expect_and_consume(TokenType::semi, format_expected_error(";"));
    }


    void Parser::parse_block_instruction()
    {
        consume_token();
        parse_instruction();
        expect_and_consume(TokenType::close_scope,format_expected_error("}"));
    }


    void Parser::parse_assign_instruction()
    {
        consume_token();
        consume_token();
        parse_expression();
        expect_and_consume(TokenType::semi, format_expected_error(";"));
    }


    void Parser::parse_primary_instruction()
    {
        parse_expression();
        expect_and_consume(TokenType::semi, format_expected_error(";"));
    }


    void Parser::parse_expression()
    {
        parse_boolean_logic_or_expression();
    }


    const TokenType& Parser::look_ahead(size_t _offset =1)
    {
        return tokens_[cursor_+_offset].type_;
    }


    void Parser::parse_grouped_expression()
    {
        consume_token();
        parse_expression();
        expect_and_consume(TokenType::close_group, format_expected_error(")"));
    }


    void Parser::parse_primary_expression()
    {
            //identifier + ( - func_call
            //identifier conflict
        if (is_type(TokenType::identifier))
        {
            auto ahead = look_ahead();
            if (ahead == TokenType::open_group)
            {
                //func call
            }
            //identifier
        }
        if (is_type(TokenType::integer_literal) || is_type(TokenType::boolean_literal))
        {
        //literal
        }
        if(is_type(TokenType::open_group))
        {
            parse_grouped_expression();
        }
    }


    void Parser::parse_unary_expression()
    {
        while (is_type(TokenType::plus)|| is_type(TokenType::minus)||is_type(TokenType::negation_op))
        {
            consume_token();
        }
        parse_primary_expression();
    }


    void Parser::parse_multiplying_expression()
    {
        parse_unary_expression();
        while (is_type(TokenType::multiply_op)|| is_type(TokenType::divide_op))
        {
            consume_token();
            parse_unary_expression();
        }
    }


    void Parser::parse_additive_expression()
    {
        parse_multiplying_expression();
        while (is_type(TokenType::minus)|| is_type(TokenType::plus))
        {
            consume_token();
            parse_multiplying_expression();
        }
    }


    void Parser::parse_relational_expression()
    {
        parse_additive_expression();
        while (is_type(TokenType::smaller_op)|| is_type(TokenType::greater_op))
        {
            consume_token();
            parse_additive_expression();
        }
    }


    void Parser::parse_equality_expression()
    {
        parse_relational_expression();
        while(is_type(TokenType::equal_op)||is_type(TokenType::not_equal_op))
        {
            consume_token();
            parse_relational_expression();
        }
    }


    void Parser::parse_boolean_logic_and_expression()
    {
        parse_equality_expression();
        while (is_type(TokenType::and_op))
        {
            consume_token();
            parse_equality_expression();
        }
    }


    void Parser::parse_boolean_logic_or_expression()
    {
        parse_boolean_logic_and_expression();
        while (is_type(TokenType::or_op))
        {
            consume_token();
            parse_boolean_logic_and_expression();
        }
    }


    //implementation details here
} // fela
