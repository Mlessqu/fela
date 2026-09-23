#include "SemanticChecker.h++"

namespace fela
{
    std::unique_ptr<AstExpression> SemanticChecker::literal_expression(std::variant<bool, int> _literal_expr)
    {
    }


    std::unique_ptr<AstExpression> SemanticChecker::variable_expression(std::string _name_identifier)
    {
    }


    std::unique_ptr<AstExpression> SemanticChecker::unary_operation(TokenType _operator,
        std::unique_ptr<AstExpression> _lhs)
    {
    }


    std::unique_ptr<AstExpression> SemanticChecker::binary_operation(TokenType _operator,
        std::unique_ptr<AstExpression> _lhs, std::unique_ptr<AstExpression> _rhs)
    {
    }


    std::unique_ptr<AstExpression> SemanticChecker::function_call(std::string _identifier,
        std::vector<std::unique_ptr<AstExpression>> _arguments)
    {
    }


    std::unique_ptr<AstInstruction> SemanticChecker::if_instruction(std::unique_ptr<AstExpression> _condition,
        std::unique_ptr<AstInstruction> _if_branch, std::unique_ptr<AstInstruction> _else_branch)
    {
    }


    std::unique_ptr<AstInstruction> SemanticChecker::while_instruction(std::unique_ptr<AstExpression> _condition,
        std::unique_ptr<AstInstruction> _body)
    {
    }


    std::unique_ptr<AstInstruction> SemanticChecker::assign_instruction(TokenType _operator, std::string _identifier,
        std::unique_ptr<AstExpression> _rhs)
    {
    }


    std::unique_ptr<AstInstruction> SemanticChecker::variable_declaration(DataType _type, std::string _identifier,
        std::unique_ptr<AstExpression> _init_value)
    {
    }


    std::unique_ptr<AstInstruction> SemanticChecker::block_instruction(
        std::vector<std::unique_ptr<AstInstruction>> _instructions)
    {
    }


    std::unique_ptr<AstFunction> SemanticChecker::return_instruction(std::unique_ptr<AstExpression> _return_expr)
    {
    }


    std::unique_ptr<AstFunction> SemanticChecker::function_declaration(DataType _return_type, std::string _identifier,
        std::vector<VariableSymbol> _params)
    {
    }


    std::unique_ptr<AstFunction> SemanticChecker::function_definition(DataType _return_type, std::string _identifier,
        std::vector<VariableSymbol> _params, std::unique_ptr<AstInstruction> _body)
    {
    }
}
