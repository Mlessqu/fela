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


    std::unique_ptr<AstExpression> SemanticChecker::variable_expression(std::string_view _name_identifier)
    {
        const Symbol* map_lookup = symbol_table_.lookup(std::string(_name_identifier));
        if (!map_lookup)
        {
            return nullptr;
        }
        if (!std::holds_alternative<VariableSymbol>(map_lookup->symbol_signature_))
        {
            //error, expected it to be variable!
            return nullptr;
        }
        auto variable = std::make_unique<AstVariableExpression>();
        variable->identifier_ = _name_identifier;
        variable->resolved_type_ = std::get<VariableSymbol>(map_lookup->symbol_signature_).type_;
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
                if (_lhs->resolved_type_ == DataType::bool_type && _rhs->resolved_type_ == DataType::bool_type)
                {
                    binary_operation->resolved_type_ = DataType::bool_type;;
                    binary_operation->lhs_ = std::move(_lhs);
                    binary_operation->rhs_ = std::move(_rhs);
                    return binary_operation;
                }
                if (_lhs->resolved_type_ == DataType::int_type && _rhs->resolved_type_ == DataType::int_type)
                {
                    binary_operation->resolved_type_ = DataType::bool_type;
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


    std::unique_ptr<AstExpression> SemanticChecker::function_call(std::string_view _identifier,
                                                                  std::vector<std::unique_ptr<AstExpression>>
                                                                  _arguments)
    {
        std::string identifier{_identifier};
        auto function_call = std::unique_ptr<AstFunctionCall>();

        const Symbol* symbol = symbol_table_.lookup(identifier);

        if (!symbol)
        {
            return nullptr;
        }

        if (!std::holds_alternative<FunctionSymbol>(symbol->symbol_signature_))
        {
            return nullptr;
        }
        auto& function_signature = std::get<FunctionSymbol>(symbol->symbol_signature_);
        auto& param_types = function_signature.param_types_;
        if (param_types.size() != _arguments.size())
        {
            return nullptr;
        }
        for (int i = 0; i < _arguments.size(); ++i)
        {
            if (_arguments[i]->resolved_type_ != param_types[i]) //types list missmatch
            {
                return nullptr;
            }
            //error wrong argument type/mismatch!
        }
        std::unique_ptr<AstFunctionCall> ret_node;
        ret_node->resolved_type_ = function_signature.return_type_;
        ret_node->identifier_ = _identifier;
        ret_node->arguments_ = std::move(_arguments);
        return ret_node;
        //here we call function
    }


    std::unique_ptr<AstInstruction> SemanticChecker::if_instruction(std::unique_ptr<AstExpression> _condition,
                                                                    std::unique_ptr<AstInstruction> _if_branch,
                                                                    std::unique_ptr<AstInstruction> _else_branch)
    {
        //if(condition){instruction}else {}
        std::unique_ptr<AstIfInstruction> ret_node;
        if (_condition->resolved_type_ != DataType::bool_type)
        {
            return nullptr;
        }
        ret_node->condition_ = std::move(_condition);
        ret_node->then_ = std::move(_if_branch);
        if (!_else_branch)
        {
            ret_node->else_branch_ = std::move(_else_branch);
        }
        return ret_node;
    }


    std::unique_ptr<AstInstruction> SemanticChecker::while_instruction(std::unique_ptr<AstExpression> _condition,
                                                                       std::unique_ptr<AstInstruction> _body)
    {
        std::unique_ptr<AstWhileInstruction> ret_node;
        if (_condition->resolved_type_ != DataType::bool_type)
        {
            return nullptr;
        }
        ret_node->condition_ = std::move(_condition);
        ret_node->body_ = std::move(_body);
        return ret_node;
    }


    std::unique_ptr<AstInstruction> SemanticChecker::assign_instruction(
        TokenType _operator, std::string_view _identifier,
        std::unique_ptr<AstExpression> _rhs)
    {
        if (_operator != TokenType::assign || !_rhs)
        {
            return nullptr;
        }

        const Symbol* symbol = symbol_table_.lookup(std::string(_identifier));
        if (!symbol || !std::holds_alternative<VariableSymbol>(symbol->symbol_signature_))
        {
            return nullptr;
        }

        const auto& lhs_signature = std::get<VariableSymbol>(symbol->symbol_signature_);
        if (lhs_signature.type_ != _rhs->resolved_type_)
        {
            return nullptr;
        }

        auto ret_node = std::make_unique<AstAssignInstruction>();
        ret_node->identifier_ = _identifier;
        ret_node->rhs_ = std::move(_rhs);
        return ret_node;
    }


    std::unique_ptr<AstInstruction> SemanticChecker::variable_declaration(DataType _type, std::string_view _identifier,
                                                                          std::unique_ptr<AstExpression> _init_value)
    {
        if (_type == DataType::void_type)
        {
            //var can't be null
            return nullptr;
        }
        const Symbol* symbol = symbol_table_.lookup(std::string(_identifier));
        if (!symbol)
        {
            //name already exists in this scope
            return nullptr;
        }
        VariableSymbol var_symbol{std::string(_identifier),_type};
        Symbol ins_symbol{var_symbol};
        symbol_table_.insert_symbol(std::string{_identifier},ins_symbol);
        std::unique_ptr<AstVariableDeclaration> ret_node = std::make_unique<AstVariableDeclaration>();
        ret_node->identifier_=_identifier;

    }


    std::unique_ptr<AstInstruction> SemanticChecker::block_instruction(
        std::vector<std::unique_ptr<AstInstruction>> _instructions)
    {
    }


    std::unique_ptr<AstFunction> SemanticChecker::function_declaration(DataType _return_type,
                                                                       std::string_view _identifier,
                                                                       std::vector<VariableSymbol> _params)
    {
    }


    std::unique_ptr<AstFunction> SemanticChecker::function_definition(DataType _return_type,
                                                                      std::string_view _identifier,
                                                                      std::vector<VariableSymbol> _params,
                                                                      std::unique_ptr<AstInstruction> _body)
    {
    }
}
