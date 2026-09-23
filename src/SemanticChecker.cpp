#include "SemanticChecker.h++"

#include "AbstractSyntaxTree.h++"

namespace fela
{
    std::unique_ptr<AstExpression> SemanticChecker::literal_expression(std::variant<bool, int> _literal_expr)
    {
        auto expression = std::make_unique<AstExpression>();
        if (std::holds_alternative<int>(_literal_expr))
        {
            expression->resolved_type_ = DataType::int_type;
        }
        else if (std::holds_alternative<bool>(_literal_expr))
        {
            expression->resolved_type_ = DataType::bool_type;
        }
        else
        {
            //literal expression must be bool or int error!
            expression->resolved_type_ = DataType::void_type;
        }
        return expression;
    }


    std::unique_ptr<AstExpression> SemanticChecker::variable_expression(std::string _name_identifier)
    {
        auto map_lookup = symbol_table_.find(_name_identifier);
        if (symbol_table_.end() == map_lookup)
        {
            //no such expression!
            return nullptr;
        }
        const Symbol& symbol = map_lookup->second;
        if (!std::holds_alternative<VariableSymbol>(symbol.symbol_signature_))
        {
            //error, expected it to be variable!
            return nullptr;
        }
        auto expr = std::make_unique<AstVariableExpression>();
        expr->identifier_ = _name_identifier;
        expr->resolved_type_ = std::get<VariableSymbol>(symbol.symbol_signature_).type_;
        return expr;
    }


    std::unique_ptr<AstExpression> SemanticChecker::unary_operation(TokenType _operator,
                                                                    std::unique_ptr<AstExpression> _unary_expression)
    {
        //+ - !
        if (!_unary_expression)
        {
            return nullptr;
        }
        auto expr = std::make_unique<AstUnaryExpression>();
        expr->operator_ = _operator;
        if (_operator == TokenType::negation_op)
        {
            if (_unary_expression->resolved_type_ != DataType::bool_type)
            {
                //error! must be bool type!
                return nullptr;
            }
            expr->resolved_type_ = DataType::bool_type;
        }
        else if (_operator == TokenType::plus || _operator == TokenType::minus)
        {
            if (_unary_expression->resolved_type_ != DataType::int_type)
            {
                //error must be integer type!
                return nullptr;
            }
            expr->resolved_type_ = DataType::int_type;
        }
        else
        {
            return nullptr;
        }
        expr->operand_ = std::move(_unary_expression);
        return expr;
    }


    std::unique_ptr<AstExpression> SemanticChecker::binary_operation(TokenType _operator,
                                                                     std::unique_ptr<AstExpression> _lhs,
                                                                     std::unique_ptr<AstExpression> _rhs)
    {
        if (!_lhs) return nullptr;
        if (!_rhs) return nullptr;
        auto expression = std::make_unique<AstBinaryExpression>();
        expression->operator_=_operator;
        //+ - * /  > <  musi byc int
        // || && - musi byc boolean
        // == moga byc oba
        if (TokenType::equal_op == expression->operator_)
        {
            if (_lhs->resolved_type_ == _rhs->resolved_type_)
            {
                expression->resolved_type_ = DataType::bool_type;
                return expression;
            }
        }

    }


    std::unique_ptr<AstExpression> SemanticChecker::function_call(std::string _identifier,
                                                                  std::vector<std::unique_ptr<AstExpression>>
                                                                  _arguments)
    {
    }


    std::unique_ptr<AstInstruction> SemanticChecker::if_instruction(std::unique_ptr<AstExpression> _condition,
                                                                    std::unique_ptr<AstInstruction> _if_branch,
                                                                    std::unique_ptr<AstInstruction> _else_branch)
    {
    }


    std::unique_ptr<AstInstruction> SemanticChecker::while_instruction(std::unique_ptr<AstExpression> _condition,
                                                                       std::unique_ptr<AstInstruction> _body)
    {
    }


    std::unique_ptr<AstInstruction> SemanticChecker::assign_instruction(TokenType _operator, std::string _identifier,
                                                                        std::unique_ptr<AstExpression> _rhs)
    {
    }


    std::unique_ptr<AstInstruction> SemanticChecker::variable_declaration(DataType _type, std::string _identifier,
                                                                          std::unique_ptr<AstExpression> _init_value)
    {
    }


    std::unique_ptr<AstInstruction> SemanticChecker::block_instruction(
        std::vector<std::unique_ptr<AstInstruction>> _instructions)
    {
    }


    std::unique_ptr<AstFunction> SemanticChecker::return_instruction(std::unique_ptr<AstExpression> _return_expr)
    {
    }


    std::unique_ptr<AstFunction> SemanticChecker::function_declaration(DataType _return_type, std::string _identifier,
                                                                       std::vector<VariableSymbol> _params)
    {
    }


    std::unique_ptr<AstFunction> SemanticChecker::function_definition(DataType _return_type, std::string _identifier,
                                                                      std::vector<VariableSymbol> _params,
                                                                      std::unique_ptr<AstInstruction> _body)
    {
    }
}
