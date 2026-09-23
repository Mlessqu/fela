#pragma once
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
