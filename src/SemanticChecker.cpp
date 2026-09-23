#include "SemanticChecker.h++"

#include "AbstractSyntaxTree.h++"

namespace fela
{
    std::unique_ptr<AstExpression> SemanticChecker::literal_expression(std::variant<bool, int> _literal_expr)
    {
        auto expression = std::make_unique<AstLiteralExpression>();
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
        auto variable = std::make_unique<AstVariableExpression>();
        variable->identifier_ = _name_identifier;
        variable->resolved_type_ = std::get<VariableSymbol>(symbol.symbol_signature_).type_;
        return variable;
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
        auto binary_operation = std::make_unique<AstBinaryExpression>();
        binary_operation->operator_ = _operator;

        //+ - * /  > <  musi byc int
        // || && - musi byc boolean
        // == moga byc oba
        switch (binary_operation->operator_)
        {
        case TokenType::equal_op:
        case TokenType::not_equal_op:
            {
              if (_lhs->resolved_type_ == DataType::bool_type &&  _rhs->resolved_type_ == DataType::bool_type)
              {
                  binary_operation->resolved_type_=DataType::bool_type;;
                  binary_operation->lhs_ = std::move(_lhs);
                  binary_operation->rhs_ = std::move(_rhs);
                  return binary_operation;
              }
                if (_lhs->resolved_type_ == DataType::int_type && _rhs->resolved_type_== DataType::int_type)
              {
                  binary_operation->resolved_type_=DataType::bool_type;
                  binary_operation->lhs_ = std::move(_lhs);
                  binary_operation->rhs_ = std::move(_rhs);
                  return binary_operation;
              }
             return nullptr;
            }
            break;
        case TokenType::and_op:
        case TokenType::or_op:
            {
                // && and ||
                if (_lhs->resolved_type_ == DataType::bool_type && _rhs->resolved_type_ == DataType::bool_type)
                {
                    binary_operation->resolved_type_ = DataType::bool_type;
                    binary_operation->lhs_ = std::move(_lhs);
                    binary_operation->rhs_ = std::move(_rhs);
                    return binary_operation;
                }
            }
            break;
        case TokenType::minus:
        case TokenType::plus:
        case TokenType::multiply_op:
        case TokenType::divide_op:
            {
                if (_lhs->resolved_type_ == DataType::int_type && _rhs->resolved_type_ == DataType::int_type)
                {
                    binary_operation->resolved_type_ = DataType::bool_type;
                    binary_operation->lhs_ = std::move(_lhs);
                    binary_operation->rhs_ = std::move(_rhs);
                    return binary_operation;
                }
            }
            break;
        default:
            {
                //error kurwy, undefined operator
                return nullptr;
            }
            break;
        }


    }


    std::unique_ptr<AstExpression> SemanticChecker::function_call(std::string _identifier,
                                                                  std::vector<std::unique_ptr<AstExpression>>
                                                                  _arguments)
    {
        if (this->symbol_table_.find(_identifier)== symbol_table_.end())
        {
            //nie ma takiej funckji nawet!
            return nullptr;
        }
        auto function_call = symbol_table_.find(_identifier);
        auto params = std::get<FunctionSymbol>(function_call->second.symbol_signature_).param_types_;
        for (int i=0;i< _arguments.size();++i)
        {
            if (_arguments[i]->resolved_type_ == params[i]) continue;
            //error wrong argument type/mismatch!
            return nullptr;
        }
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
