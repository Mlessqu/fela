#pragma once
#include <unordered_map>

#include "SymbolTable.h++"

namespace fela
{
    enum class TokenType;
    class AstExpression;
    class AstInstruction;

    class SemanticChecker
    {
    public:
        AstExpression* literal_expression(std::variant<bool, int> _literal_expr);
        AstExpression* variable_expression(std::string _name_identifier);
        AstExpression* unary_operation(TokenType _operator, AstExpression* _lhs);
        AstExpression* binary_operation(TokenType _operator, AstExpression* _lhs, AstExpression* _rhs);
        AstExpression* function_call(std::string _identifier, std::vector<AstExpression*> _arguments);


        AstInstruction* if_instruction(AstExpression* _condition, AstInstruction* _if_branch,
                                       AstInstruction* _else_branch =nullptr);
        AstInstruction* while_instruction(AstExpression* _condition, AstInstruction* _body);
        AstInstruction* assign_instruction(TokenType _operator, std::string _identifier, AstExpression* _rhs);
        AstInstruction* variable_declaration(DataType _type, std::string _identifier, AstExpression* _init_value);
        AstInstruction* block_instruction(std::vector<AstInstruction*> _instructions);
        AstInstruction* return_instruction(AstExpression* _return_expr);
        AstInstruction* function_declaration(DataType _return_type,std::string _identifier, std::vector<VariableSymbol> _params);
        AstInstruction* function_definition(DataType _return_type, std::string _identifier,  std::vector<VariableSymbol> _params);

        //function call
        //function def
        //function declaration
    private:
        std::unordered_map<std::string, Symbol> symbol_table_;
    };
} // fela
