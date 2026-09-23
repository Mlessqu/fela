#pragma once
#include <memory>
#include <vector>

#include "AbstractSyntaxTree.h++"
#include "Lexer.h++"
#include "SymbolTable.h++"
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
        std::unique_ptr<AstProgram> parse_program();
    private:
        //functions
        //func helpers
        std::vector<DataType> parse_param_list();
        std::vector<DataType> parse_argument_list();
        std::unique_ptr<AstFunction> parse_function();

        std::unique_ptr<AstExpression> parse_function_call();
        //instructions
        std::unique_ptr<AstInstruction> parse_instruction();
        std::unique_ptr<AstInstruction> parse_return_instruction();
        std::unique_ptr<AstInstruction> parse_while_instruction();
        std::unique_ptr<AstInstruction> parse_if_instruction();
        std::unique_ptr<AstInstruction> parse_variable_declaration_instruction();
        std::unique_ptr<AstInstruction> parse_block_instruction();
        std::unique_ptr<AstInstruction> parse_assign_instruction();
        std::unique_ptr<AstInstruction> parse_primary_instruction();
        //expressions
        std::unique_ptr<AstExpression> parse_expression();
        std::unique_ptr<AstExpression> parse_grouped_expression();
        std::unique_ptr<AstExpression> parse_primary_expression();
        std::unique_ptr<AstExpression> parse_unary_expression();
        std::unique_ptr<AstExpression> parse_multiplying_expression();
        std::unique_ptr<AstExpression> parse_additive_expression();
        std::unique_ptr<AstExpression> parse_relational_expression();
        std::unique_ptr<AstExpression> parse_equality_expression();
        std::unique_ptr<AstExpression> parse_boolean_logic_and_expression();
        std::unique_ptr<AstExpression> parse_boolean_logic_or_expression();

    };
} // fela
