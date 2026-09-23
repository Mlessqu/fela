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
        AstBase();
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
        std::variant<bool, int> value_;
    };
    class AstUnaryExpression : public AstExpression
    {
        TokenType operator_;
        std::variant<bool, int> value_;
    };
    class AstBinaryExpression : public AstExpression
    {
        TokenType operator_;
        std::variant<bool, int> lhs_;
        std::variant<bool, int> rhs_;

    };
    class AstFunctionCall : public AstExpression
    {
        std::string identifier_;
        DataType return_type_;
        std::vector<DataType> arguments_;
    };
    //literal -> bool/int literal
    //variable expr -> read variable by name
    //unary expr -> op + expr
    //binary expr -> lh_exp + op + rh_exp
    //Call func expr -> look up symbol -> validate arg list correctness

    //layer 3, executes instruction, no value produced
    class AstInstruction : public AstBase
    {

    };
    //variable decl
    //assign
    //block of instruction {...}
    //if instruction
    //while instruction
    //expression instruction (function call returning value for example)
    //layer 4,  functions
    class AstFunction : public AstBase
    {

    };
} // fela
