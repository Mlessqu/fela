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


    struct AstBase
    {
    public:
        virtual ~AstBase() = default;
    };

    //layer2
    struct AstExpression : public AstBase //value and type semantic check
    {
    public:
        DataType variable_type_;
    };

    struct AstLiteralExpression : public AstExpression
    {
        std::variant<bool, int> value_;
    };

    struct AstVariableExpression : public AstExpression
    {
        std::string identifier_;
    };

    struct AstUnaryExpression : public AstExpression
    {
        TokenType operator_;
        AstExpression* expression_ = nullptr;
    };

    struct AstBinaryExpression : public AstExpression
    {
        TokenType operator_;
        AstExpression* lhs_ = nullptr;
        AstExpression* rhs_ = nullptr;
    };

    struct AstFunctionCall : public AstExpression
    {
        std::string identifier_;
        DataType return_type_;
        std::vector<AstExpression*> arguments_;
    };

    //layer 3, executes instruction, no value produced
    struct AstInstruction : public AstBase
    {
    };

    struct AstVariableDeclaration : public AstInstruction
    {
        std::string identifier_;
        DataType type_;
        AstExpression* init_value_ = nullptr;
    };

    struct AstAssignInstruction : public AstInstruction
    {
    public:
        std::string identifier_;
        AstExpression* rhs_;
    };

    struct AstBlockInstruction : public AstInstruction
    {
    public:
        std::vector<AstInstruction*> block_instructions_;
    };

    struct AstIfInstruction : public AstInstruction
    {
        AstExpression* condition_;
        AstInstruction* if_branch_;
        AstInstruction* else_branch_;
    };

    struct AstWhileInstruction : public AstInstruction
    {
        AstExpression* condition_;
        AstInstruction* body_;
    };
    struct AstReturnInstruction : public AstInstruction
    {
        AstExpression* return_expression_= nullptr;
    };

    struct AstFunction : public AstBase
    {
        std::string identifier_;
        DataType return_type_;
        std::vector<VariableSymbol> parameters_;
    };

    struct AstFunctionDeclaration : public AstFunction
    {
    };

    struct AstFunctionDefinition : public AstFunction
    {
    public:
        AstBlockInstruction* body_ = nullptr;
    };
    struct AstProgram : public AstBase
    {
        std::vector<AstBase*> nodes_;
    };
} // fela
