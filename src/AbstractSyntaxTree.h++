#pragma once

namespace fela
{
    //based off of Specification layers thus
    //layer 1 AstType
    //layer 2 AstExpression
    //layer 3 AstInstruction
    //layer 4 AstFunction


    class AstNodeBase
    {
    public:
        AstNodeBase();
    };
    //layer2
    class AstNodeExpression : public AstNodeBase //value and type semantic check
    {

    };
    //layer 3, executes instruction, no value produced
    class AstNodeInstruction : public AstNodeBase
    {

    };
    //layer 4,  functions
    class AstNodeFunction : public AstNodeBase
    {

    };
} // fela
