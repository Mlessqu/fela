#pragma once

namespace fela
{
    class AstExpression;
class AstInstruction;
    class SemanticChecker
    {
    public:
        AstExpression* literal_expression();
        AstExpression* variable_expression();
        AstExpression* unary_operation();
        AstExpression* binary_operation();

        AstInstruction* if_instruction();
        AstInstruction* while_instruction();
        AstInstruction* assign_instruction();

    };
} // fela
