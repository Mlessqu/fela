#pragma once
#include "Lexer.h++"
#include "SymbolTable.h++"

namespace fela
{
    //based off of Specification layers thus
    //layer 1 AstType
    //layer 2 AstExpression
    //layer 3 AstInstruction
    //layer 4 AstFunction


    class AstBase
    {
    public:
        virtual ~AstBase() = default;
    };
    //layer2
    class AstExpression : public AstBase //value and type semantic check
    {
    public:
        DataType variable_type_;
    };
    class AstLiteralExpression : public AstExpression
    {
        DataType type_;
        std::variant<bool, int> value_;
    };
    class AstVariableExpression : public AstExpression
    {
        DataType type_;
        std::string identifier_;
    };
    class AstUnaryExpression : public AstExpression
    {
        TokenType operator_;
        AstExpression* expression_ = nullptr;
    };
    class AstBinaryExpression : public AstExpression
    {
        TokenType operator_;
        AstExpression* lhs_ = nullptr;
        AstExpression* rhs_ = nullptr;

    };
    class AstFunctionCall : public AstExpression
    {
        std::string identifier_;
        DataType return_type_;
        std::vector<AstExpression*> arguments_;
    };

    //layer 3, executes instruction, no value produced
    class AstInstruction : public AstBase
    {

    };
    class AstVariableDeclaration : public AstInstruction
    {
        std::string identifier_;
        DataType type_;
        AstExpression* init_value_= nullptr;
    };
    class AstAssignInstruction : public AstInstruction
    {
    public:
        std::string identifier_;
        AstExpression* rhs_;
    };
    class AstBlockInstruction : public AstInstruction
    {
    public:
        std::vector<AstInstruction*> block_instructions_;
    };
    class AstIfInstruction : public AstInstruction
    {
        AstExpression* condition_;
        AstInstruction* if_branch_;
        AstInstruction* else_branch_;

            /// if(condition) { then } else { }
    };
    class AstWhileInstruction : public AstInstruction
    {
        AstExpression* condition_;
        AstInstruction* body_;
    };


    //expression instruction (function call returning value for example)
    //layer 4,  functions
    class AstFunction : public AstBase
    {

    };
    class AstFunctionDeclaration : public AstFunction
    {

    };
    class AstFunctionDefinition : public AstFunction
    {

    };
} // fela
