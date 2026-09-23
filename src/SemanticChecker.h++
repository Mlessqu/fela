#pragma once
#include <memory>
#include <unordered_map>

#include "SymbolTable.h++"

namespace fela
{
    struct AstFunction;
    enum class TokenType;
    class AstExpression;
    class AstInstruction;

    class SemanticChecker
    {
    public:
        std::unique_ptr<AstExpression> literal_expression(std::variant<bool, int> _literal_expr);
        std::unique_ptr<AstExpression> variable_expression(std::string _name_identifier);
        std::unique_ptr<AstExpression> unary_operation(TokenType _operator, std::unique_ptr<AstExpression> _unary_expression);
        std::unique_ptr<AstExpression> binary_operation(TokenType _operator, std::unique_ptr<AstExpression> _lhs, std::unique_ptr<AstExpression> _rhs);
        std::unique_ptr<AstExpression> function_call(std::string _identifier, std::vector<std::unique_ptr<AstExpression>> _arguments);


        std::unique_ptr<AstInstruction> if_instruction(std::unique_ptr<AstExpression> _condition, std::unique_ptr<AstInstruction> _if_branch,
                                       std::unique_ptr<AstInstruction> _else_branch =nullptr);
        std::unique_ptr<AstInstruction> while_instruction(std::unique_ptr<AstExpression> _condition, std::unique_ptr<AstInstruction> _body);
        std::unique_ptr<AstInstruction> assign_instruction(TokenType _operator, std::string _identifier, std::unique_ptr<AstExpression> _rhs);
        std::unique_ptr<AstInstruction> variable_declaration(DataType _type, std::string _identifier, std::unique_ptr<AstExpression> _init_value);
        std::unique_ptr<AstInstruction> block_instruction(std::vector<std::unique_ptr<AstInstruction>> _instructions);

        std::unique_ptr<AstFunction> function_declaration(DataType _return_type,std::string _identifier, std::vector<VariableSymbol> _params);
        std::unique_ptr<AstFunction> function_definition(DataType _return_type, std::string _identifier,  std::vector<VariableSymbol> _params, std::unique_ptr<AstInstruction> _body);

        //function call
        //function def
        //function declaration
    private:
        std::unordered_map<std::string, Symbol> symbol_table_;
    };
} // fela
