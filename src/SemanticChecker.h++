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
        AstExpression* literal_expression(AstExpression* _literal_expr);
        AstExpression* variable_expression(std::string _name_identifier);
        AstExpression* unary_operation(TokenType _operator);
        AstExpression* binary_operation(TokenType _operator);

        AstInstruction* if_instruction();
        AstInstruction* while_instruction();
        AstInstruction* assign_instruction();
    private:
        std::unordered_map<std::string, Symbol> symbol_table_;
    };
} // fela
