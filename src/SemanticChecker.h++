#pragma once
#include <memory>
#include <unordered_map>

#include "SymbolTable.h++"

namespace fela
{
    struct AstUnaryExpression;
    struct AstBinaryExpression;
    struct AstBase;
    struct AstProgram;
    struct AstBlockInstruction;
    struct AstFunction;
    class Token;
    class AstExpression;
    class AstInstruction;

    class SemanticChecker
    {
    public:
        bool check(AstProgram* _program);
        const std::vector<std::string>& errors() const;
    private:
        void push_error(AstBase* _node, std::string _err_msg);
        [[nodiscard]]bool check_binary_expression(AstBinaryExpression* _binary_expression_node);
        [[nodiscard]] bool check_unary_expression(AstUnaryExpression* _unary_node);
        bool should_abort_=false;
        bool check_node(AstBase* _node);
        ScopeStack symbol_table_;
        std::vector<std::string> errors_;
    };
} // fela
