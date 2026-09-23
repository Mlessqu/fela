#pragma once
#include <vector>

#include "Lexer.h++"
// so the parser will be responsible for checking grammar correctness syntax,
//no ast just syntax
namespace fela
{
    class Parser
    {
        std::vector<Token> tokens_;
        size_t cursor_=0;
        //doesn't movve the cursor
        const Token& get_current_token();
        bool is_type(TokenType _type);
        bool is_not_type(TokenType _type);
        bool is_type_specifier_keyword();
        const TokenType& look_ahead(size_t _offset);
        //advances cursor
        const Token& consume_token();
        bool expect_and_consume(TokenType _expected_type, std::string _error_message);
        std::string expected_diff_symbol_error(std::string_view _expected);
    public:
        explicit Parser(std::vector<Token> _tokens);
        void parse_program();
    private:
        //functions
        void parse_param_list();
        void parse_function();

        void parse_argument_list();
        void parse_function_call();
        //instructions
        void parse_instruction();
        void parse_return_instruction();
        void parse_while_instruction();
        void parse_if_instruction();
        void parse_variable_declaration_instruction();
        void parse_block_instruction();
        void parse_assign_instruction();
        void parse_primary_instruction();
        //expressions
        void parse_expression();
        void parse_grouped_expression();
        void parse_primary_expression();
        void parse_unary_expression();
        void parse_multiplying_expression();
        void parse_additive_expression();
        void parse_relational_expression();
        void parse_equality_expression();
        void parse_boolean_logic_and_expression();
        void parse_boolean_logic_or_expression();

    };
} // fela
