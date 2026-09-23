#pragma once
#include <memory>

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
        virtual ~AstBase() = default;
    };

    //layer2
    struct AstExpression : public AstBase //value and type semantic check
    {

        DataType resolved_type_;
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
        std::unique_ptr<AstExpression> operand_ = nullptr;
    };

    struct AstBinaryExpression : public AstExpression
    {
        TokenType operator_;
        std::unique_ptr<AstExpression> lhs_ = nullptr;
        std::unique_ptr<AstExpression> rhs_ = nullptr;
    };

    struct AstFunctionCall : public AstExpression
    {
        std::string identifier_;
        std::vector<std::unique_ptr<AstExpression>> arguments_;
    };

    //layer 3, executes instruction, no value produced
    struct AstInstruction : public AstBase
    {
    };

    struct AstVariableDeclaration : public AstInstruction
    {
        std::string identifier_;
        DataType type_;
        std::unique_ptr<AstExpression> init_value_ = nullptr;
    };

    struct AstAssignInstruction : public AstInstruction
    {
        std::string identifier_;
        std::unique_ptr<AstExpression> rhs_;
    };

    struct AstBlockInstruction : public AstInstruction
    {
    
        std::vector<std::unique_ptr<AstInstruction>> body_;
    };

    struct AstIfInstruction : public AstInstruction
    {
        std::unique_ptr<AstExpression> condition_;
        std::unique_ptr<AstInstruction> then_;
        std::unique_ptr<AstInstruction> else_branch_;
    };

    struct AstWhileInstruction : public AstInstruction
    {
        std::unique_ptr<AstExpression> condition_;
        std::unique_ptr<AstInstruction> body_;
    };
    struct AstReturnInstruction : public AstInstruction
    {
        std::unique_ptr<AstExpression> value_ = nullptr;
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
        std::unique_ptr<AstBlockInstruction> body_ = nullptr;
    };
    struct AstProgram : public AstBase
    {
        std::vector<AstBase*> nodes_;
    };
} // fela
