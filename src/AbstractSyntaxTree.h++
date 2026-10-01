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
enum class AstNodeType
{
    program,
    variable_declaration,
    assign_instruction,
    if_instruction,
    while_instruction,
    return_instruction,
    primary_instruction,
    block_instruction,
    binary_expression,
    unary_expression,
    literal_expression,
    variable_expression,
    function_call,
    function_declaration,
    function_definition,
    undefined
};

    struct AstBase
    {
        AstNodeType node_type_ = AstNodeType::undefined;
        unsigned int line_=0;
        unsigned int column_=0;
        explicit AstBase(AstNodeType _type = AstNodeType::undefined) : node_type_{_type} {}
        virtual ~AstBase() = default;
    };

    //layer2
    struct AstExpression : public AstBase //value and type semantic check
    {
        explicit AstExpression(AstNodeType _type = AstNodeType::undefined) : AstBase(_type) {}
        DataType resolved_type_ = DataType::unresolved_type;
    };

    struct AstLiteralExpression : public AstExpression
    {
        AstLiteralExpression() : AstExpression(AstNodeType::literal_expression) {}
        std::variant<bool, int> value_;
    };

    struct AstVariableExpression : public AstExpression
    {
        AstVariableExpression() : AstExpression(AstNodeType::variable_expression) {}
        std::string identifier_;
    };

    struct AstUnaryExpression : public AstExpression
    {
        AstUnaryExpression() : AstExpression(AstNodeType::unary_expression) {}
        Token operator_;
        std::unique_ptr<AstExpression> rhs_ = nullptr;
    };

    struct AstBinaryExpression : public AstExpression
    {
        AstBinaryExpression() : AstExpression(AstNodeType::binary_expression) {}
        Token operator_;
        std::unique_ptr<AstExpression> lhs_ = nullptr;
        std::unique_ptr<AstExpression> rhs_ = nullptr;
    };

    struct AstFunctionCall : public AstExpression
    {
        AstFunctionCall() : AstExpression(AstNodeType::function_call) {}
        std::string identifier_;
        std::vector<std::unique_ptr<AstExpression>> arguments_;
    };

    //layer 3, executes instruction, no value produced
    struct AstInstruction : public AstBase
    {
        explicit AstInstruction(AstNodeType _type = AstNodeType::undefined) : AstBase(_type) {}
    };
    struct AstPrimaryInstruction : public AstInstruction
    {
        AstPrimaryInstruction() : AstInstruction(AstNodeType::primary_instruction) {}
        std::unique_ptr<AstExpression> expression_;
    };
    struct AstVariableDeclaration : public AstInstruction
    {
        AstVariableDeclaration() : AstInstruction(AstNodeType::variable_declaration) {}
        std::string identifier_;
        DataType type_;
        std::unique_ptr<AstExpression> init_value_ = nullptr;
    };

    struct AstAssignInstruction : public AstInstruction
    {
        AstAssignInstruction() : AstInstruction(AstNodeType::assign_instruction) {}
        std::string identifier_;
        std::unique_ptr<AstExpression> rhs_;
    };

    struct AstBlockInstruction : public AstInstruction
    {
        AstBlockInstruction() : AstInstruction(AstNodeType::block_instruction) {}
        std::vector<std::unique_ptr<AstInstruction>> body_;
    };

    struct AstIfInstruction : public AstInstruction
    {
        AstIfInstruction() : AstInstruction(AstNodeType::if_instruction) {}
        std::unique_ptr<AstExpression> condition_;
        std::unique_ptr<AstInstruction> then_;
        std::unique_ptr<AstInstruction> else_branch_;
    };

    struct AstWhileInstruction : public AstInstruction
    {
        AstWhileInstruction() : AstInstruction(AstNodeType::while_instruction) {}
        std::unique_ptr<AstExpression> condition_;
        std::unique_ptr<AstInstruction> body_;
    };
    struct AstReturnInstruction : public AstInstruction
    {
        AstReturnInstruction() : AstInstruction(AstNodeType::return_instruction) {}
        std::unique_ptr<AstExpression> expression_ = nullptr;
    };

    struct AstFunction : public AstBase
    {
        explicit AstFunction(AstNodeType _type = AstNodeType::undefined) : AstBase(_type) {}
        std::string identifier_;
        DataType return_type_;
        std::vector<VariableSymbol> parameters_;
    };

    struct AstFunctionDeclaration : public AstFunction
    {
        AstFunctionDeclaration() : AstFunction(AstNodeType::function_declaration) {}
    };

    struct AstFunctionDefinition : public AstFunction
    {
        AstFunctionDefinition() : AstFunction(AstNodeType::function_definition) {}
        std::unique_ptr<AstBlockInstruction> body_ = nullptr;
    };
    struct AstProgram : public AstBase
    {
        AstProgram() : AstBase(AstNodeType::program) {}
        std::vector<std::unique_ptr<AstBase>> nodes_;
    };
} // fela
