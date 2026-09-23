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


    std::string Parser::expected_diff_symbol_error(std::string_view _expected)
    {
        const Token& tok = get_current_token();
        return fmt::format("At line {}, col {} expected '{}' but got '{}'", tok.line_, tok.column_, _expected,
                           tok.payload_);
    }


    Parser::Parser(std::vector<Token> _tokens) : tokens_(_tokens)
    {
    }


    void Parser::parse_program()
    {
        //TODO: finish, cause not finished, might also need to update in EBNF specification if that's really what I want
        //entrypoint grammar here
        while (is_not_type(TokenType::eof))
        {
            if (is_type_specifier_keyword())
            {
                if (look_ahead(2) == TokenType::open_group)
                {
                    parse_function();
                }else
                {
                    parse_variable_declaration_instruction();
                }
            }else
            {
                //unexpected token at top level, error here
            }
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
        else if (is_type(TokenType::open_scope))
        {
            parse_block_instruction();
        }
        else if (is_type(TokenType::type_int) || is_type(TokenType::type_bool))
        {
            parse_variable_declaration_instruction();
        }
        else if (is_type(TokenType::identifier) && look_ahead(1) == TokenType::assign)
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
        if (is_not_type(TokenType::semi))
        {
            parse_expression();
        }
        expect_and_consume(TokenType::semi, expected_diff_symbol_error(";"));
    }


    void Parser::parse_while_instruction()
    {
        consume_token();
        expect_and_consume(TokenType::open_group, "Invalid syntax, expected '(' after while");
        parse_expression();
        expect_and_consume(TokenType::close_group, expected_diff_symbol_error(")"));
        parse_instruction();
    }


    void Parser::parse_if_instruction()
    {
        consume_token();
        expect_and_consume(TokenType::open_group, "Invalid syntax, expected '(' after if");
        parse_expression();
        expect_and_consume(TokenType::close_group, expected_diff_symbol_error(")"));
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
        expect_and_consume(TokenType::identifier, "Unexpected identifier syntax");
        if (is_type(TokenType::assign))
        {
            consume_token();
            parse_expression();
        }
        expect_and_consume(TokenType::semi, expected_diff_symbol_error(";"));
    }


    void Parser::parse_block_instruction()
    {
        consume_token();
        while (is_not_type(TokenType::close_scope) && is_not_type(TokenType::eof))
        {
            parse_instruction();
        }
        expect_and_consume(TokenType::close_scope, expected_diff_symbol_error("}"));
    }


    void Parser::parse_assign_instruction()
    {
        consume_token();
        consume_token();
        parse_expression();
        expect_and_consume(TokenType::semi, expected_diff_symbol_error(";"));
    }


    void Parser::parse_primary_instruction()
    {
        parse_expression();
        expect_and_consume(TokenType::semi, expected_diff_symbol_error(";"));
    }


    void Parser::parse_expression()
    {
        parse_boolean_logic_or_expression();
    }


    const TokenType& Parser::look_ahead(size_t _offset = 1)
    {
        const size_t assumed_size = cursor_ + _offset;
        if (assumed_size >= tokens_.size())
        {
            return TokenType::eof;
        }
        return tokens_[cursor_ + _offset].type_;
    }


    void Parser::parse_grouped_expression()
    {
        consume_token();
        parse_expression();
        expect_and_consume(TokenType::close_group, expected_diff_symbol_error(")"));
    }


    void Parser::parse_primary_expression()
    {
        if (is_type(TokenType::identifier))
        {
            TokenType ahead = look_ahead();
            if (ahead == TokenType::open_group)
            {
                parse_function_call();
            }else
            {
                consume_token();
            }
        }
        if (is_type(TokenType::integer_literal) || is_type(TokenType::false_boolean) ||
            is_type(TokenType::true_boolean))
        {
            consume_token();
        }
        if (is_type(TokenType::open_group))
        {
            parse_grouped_expression();
        }
    }


    void Parser::parse_unary_expression()
    {
        while (is_type(TokenType::plus) || is_type(TokenType::minus) || is_type(TokenType::negation_op))
        {
            consume_token();
        }
        parse_primary_expression();
    }


    void Parser::parse_multiplying_expression()
    {
        parse_unary_expression();
        while (is_type(TokenType::multiply_op) || is_type(TokenType::divide_op))
        {
            consume_token();
            parse_unary_expression();
        }
    }


    void Parser::parse_additive_expression()
    {
        parse_multiplying_expression();
        while (is_type(TokenType::minus) || is_type(TokenType::plus))
        {
            consume_token();
            parse_multiplying_expression();
        }
    }


    void Parser::parse_relational_expression()
    {
        parse_additive_expression();
        while (is_type(TokenType::smaller_op) || is_type(TokenType::greater_op))
        {
            consume_token();
            parse_additive_expression();
        }
    }


    void Parser::parse_equality_expression()
    {
        parse_relational_expression();
        while (is_type(TokenType::equal_op) || is_type(TokenType::not_equal_op))
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


    void Parser::parse_param_list()
    {
        consume_token();
        //check if paramlist empty
        if (is_type(TokenType::close_group))
        {
            consume_token();
            return;
        }
        if (!is_type_specifier_keyword())
        {

            //error here!
            return;
        }
        consume_token();
        //the problem is I can expect only one token type! not arbitrary amount, DO i make special "type_specifier" rule or something?
        expect_and_consume(TokenType::identifier, "err, stub");
        while (is_not_type(TokenType::close_group)) //here need to check if nested "()" exist somehow
        {
            expect_and_consume(TokenType::coma, "err stub");
            if (!is_type_specifier_keyword())
            {
                //syntax error
            }
            consume_token();
            expect_and_consume(TokenType::identifier, "err stub");
        }
        consume_token();
    }




    void Parser::parse_function()
    {
        consume_token();
        expect_and_consume(TokenType::identifier, "Expected function name");
        parse_param_list();
        if (is_type(TokenType::open_scope))
        {
            parse_block_instruction();
        }else
        {
            expect_and_consume(TokenType::semi, expected_diff_symbol_error(";"));
        }

    }


    std::vector<__resharper_unknown_type> Parser::parse_argument_list()
    {
        consume_token();
        if (is_not_type(TokenType::close_group))
        {
            parse_expression();
        }
        while (is_not_type(TokenType::close_group))
        {
            expect_and_consume(TokenType::coma, "err stub");
            parse_expression();
        }
        consume_token();
    }


    bool Parser::is_type_specifier_keyword()
    {
        if (is_type(TokenType::type_bool)) return true;
        if (is_type(TokenType::type_int)) return true;
        if (is_type(TokenType::type_void)) return true;
        return false;
    }


    void Parser::parse_function_call()
    {
        expect_and_consume(TokenType::identifier, "err stub");
        parse_argument_list();
    }


    //implementation details here
} // fela
