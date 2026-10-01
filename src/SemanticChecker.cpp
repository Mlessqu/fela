#include "SemanticChecker.h++"

#include <cassert>
#include <fmt/format.h>

#include "AbstractSyntaxTree.h++"

namespace fela
{
    constexpr size_t MAX_ERRORS = 20;
    constexpr std::string_view ENTRY_POINT_IDENTIFIER = "main";
    constexpr bool CURRENT_SCOPE_ONLY = true;

    bool SemanticChecker::check(AstProgram* _program)
    {
        check_node(_program);
        assert(symbol_table_.is_stack_empty() && "Scope stack isnt empty, check push_scope() calls");

        return errors_.empty();
    }


    bool SemanticChecker::is_global_scope() const
    {
        return symbol_table_.is_global_scope();
    }


    bool SemanticChecker::check_main()
    {
        const Symbol* symbol = symbol_table_.lookup(std::string{ENTRY_POINT_IDENTIFIER});
        if (!symbol)
        {
            return true;
        }

        auto func_sym = std::get_if<FunctionSymbol>(&symbol->symbol_signature_);
        if (!func_sym)
        {
            push_error(nullptr, fmt::format("'{}' must be a function", ENTRY_POINT_IDENTIFIER));
            return false;
        }

        bool is_valid = true;
        if (func_sym->return_type_ != DataType::int_type)
        {
            push_error(nullptr, fmt::format("'{}' function must return 'int'", ENTRY_POINT_IDENTIFIER));
            is_valid = false;
        }

        if (!func_sym->param_types_.empty())
        {
            push_error(nullptr, fmt::format("'{}' function must take no parameters", ENTRY_POINT_IDENTIFIER));
            is_valid = false;
        }

        return is_valid;
    }


    void SemanticChecker::push_error(AstBase* _node, std::string _err_msg)
    {
        std::string err_msg{};
        if (_node)
        {
            err_msg += fmt::format("Line {},col {}: ", _node->line_, _node->column_);
        }
        err_msg += _err_msg;
        err_msg += "\n";
        errors_.push_back(err_msg);
        if (errors_.size() >= MAX_ERRORS)
        {
            should_abort_ = true;
        }
    }


    const std::vector<std::string>& SemanticChecker::errors() const
    {
        return errors_;
    }


    bool SemanticChecker::check_node(AstBase* _node)

    {
        if (!_node) return false;
        if (should_abort_ == true) return false;
        switch (_node->node_type_)
        {
        case AstNodeType::program:
            {
                auto ast_program_ptr = static_cast<AstProgram*>(_node);
                symbol_table_.push_scope();
                for (const auto& node : ast_program_ptr->nodes_)
                {
                    check_node(node.get());
                }
                symbol_table_.pop_scope();
                break;
            }
        case AstNodeType::variable_declaration:
            {
                auto ast_variable_decl_ptr = static_cast<AstVariableDeclaration*>(_node);
                const auto& identifier = ast_variable_decl_ptr->identifier_;
                if (ast_variable_decl_ptr->type_ == DataType::unresolved_type)
                {
                    std::string err_message = fmt::format("variable '{}' has unresolved type", identifier);
                    push_error(ast_variable_decl_ptr, err_message);
                }
                if (ast_variable_decl_ptr->type_ == DataType::void_type)
                {
                    std::string err_msg = fmt::format("variable '{}' cannot be void", identifier);
                    push_error(_node, err_msg);
                }
                if (symbol_table_.lookup(identifier, CURRENT_SCOPE_ONLY) != nullptr)
                {
                    std::string err_msg = fmt::format("Redefinition of '{}' identifier", identifier);
                    push_error(_node, err_msg);
                }
                if (!symbol_table_.insert_var_symbol(identifier, ast_variable_decl_ptr->type_))
                {
                }
                //genuinely don't know if I should even handle bool

                if (ast_variable_decl_ptr->init_value_)
                {
                    check_node(ast_variable_decl_ptr->init_value_.get());
                    if (ast_variable_decl_ptr->init_value_->resolved_type_ != ast_variable_decl_ptr->type_)
                    {
                        std::string error_msg("types must match!");
                        push_error(_node, error_msg);
                    }
                }
                break;
            }
        case AstNodeType::assign_instruction:
            {
                auto assign_instruction_ptr = static_cast<AstAssignInstruction*>(_node);
                const std::string& identifier = assign_instruction_ptr->identifier_;
                assign_instruction_ptr->rhs_;
                const Symbol* symbol = symbol_table_.lookup(identifier);
                if (!symbol)
                {
                    std::string err_msg = fmt::format("Unknown identifier: {}", identifier);
                    push_error(_node, err_msg);
                    break;
                }
                auto symbol_type = std::get_if<VariableSymbol>(&symbol->symbol_signature_);
                if (!symbol_type)
                {
                    std::string err_msg = fmt::format("You can't assign value to the symbol {}", symbol_type->name_);
                    push_error(_node, err_msg);
                    break;
                }

                if (!check_node(assign_instruction_ptr->rhs_.get()))
                {
                    break;
                }
                if (symbol_type->type_ != assign_instruction_ptr->rhs_->resolved_type_)
                {
                    std::string err_msg = fmt::format("Type of {}, doesn't match the result type of expression",
                                                      identifier);
                    push_error(_node, err_msg);
                    break;
                }

                break;
            }
        case AstNodeType::if_instruction:
            {
                auto if_instruction_ptr = static_cast<AstIfInstruction*>(_node);
                if (!check_node(if_instruction_ptr->condition_.get()))
                {
                    break;
                }

                if (if_instruction_ptr->condition_->resolved_type_ != DataType::bool_type)
                {
                    push_error(_node, "if conditional must be boolean type");
                    break;
                }
                symbol_table_.push_scope();
                if (!check_node(if_instruction_ptr->then_.get()))
                {
                    symbol_table_.pop_scope();
                    break;
                }
                symbol_table_.pop_scope();
                symbol_table_.push_scope();
                if (!check_node(if_instruction_ptr->else_branch_.get()))
                {
                    symbol_table_.pop_scope();
                    break;
                }
                symbol_table_.pop_scope();
                break;
            }

        case AstNodeType::while_instruction:
            {
                auto while_instruction_ptr = static_cast<AstWhileInstruction*>(_node);
                if (!check_node(while_instruction_ptr->condition_.get()))
                {
                    break;
                }
                symbol_table_.push_scope();
                if (!check_node(while_instruction_ptr->body_.get()))
                {
                    symbol_table_.pop_scope();
                    break;
                }
                symbol_table_.pop_scope();
                break;
            }
        case AstNodeType::return_instruction:
            {
                auto return_instruction_pointer = static_cast<AstReturnInstruction*>(_node);
                if (!check_node(return_instruction_pointer->expression_.get()))
                {
                    break;
                }
                break;
            }
        case AstNodeType::primary_instruction:
            {
                auto primary_instruction_ptr = static_cast<AstPrimaryInstruction*>(_node);
                if (!check_node(primary_instruction_ptr->expression_.get()))
                {
                    break;
                }
                break;
            }
        case AstNodeType::block_instruction:
            {
                auto block_instruction_ptr = static_cast<AstBlockInstruction*>(_node);
                symbol_table_.push_scope();
                for (const auto& instruction : block_instruction_ptr->body_)
                {
                    check_node(instruction.get());
                }
                symbol_table_.pop_scope();
                break;
            }
        case AstNodeType::binary_expression:
            {
                auto binary_expr_ptr = static_cast<AstBinaryExpression*>(_node);
                symbol_table_.push_scope();
                if (!check_binary_expression(binary_expr_ptr))
                {
                    symbol_table_.pop_scope();
                    break;
                }
                symbol_table_.pop_scope();
                break;
            }
        case AstNodeType::unary_expression:
            {
                auto unary_expr_ptr = static_cast<AstUnaryExpression*>(_node);
                symbol_table_.push_scope();
                if (!check_unary_expression(unary_expr_ptr))
                {
                    symbol_table_.pop_scope();
                    break;
                }
                symbol_table_.pop_scope();
                break;
            }
        case AstNodeType::literal_expression:
            {
                auto literal_expr_ptr = static_cast<AstLiteralExpression*>(_node);

                if (std::holds_alternative<int>(literal_expr_ptr->value_))
                {
                    literal_expr_ptr->resolved_type_ = DataType::int_type;



                }
                else if (std::holds_alternative<bool>(literal_expr_ptr->value_))
                {
                    literal_expr_ptr->resolved_type_ = DataType::bool_type;

                }

                break;
            }
        case AstNodeType::variable_expression:
            {
                auto variable_expr_ptr = static_cast<AstVariableExpression*>(_node);
                const std::string& identifier = variable_expr_ptr->identifier_;
                const Symbol* symbol = symbol_table_.lookup(identifier);
                if (!symbol)
                {
                    std::string err_msg = fmt::format("unkown identifier {}", identifier);
                    push_error(_node, err_msg);
                    break;
                }
                auto symbol_type = get_if<VariableSymbol>(&symbol->symbol_signature_);
                if (!symbol_type)
                {
                    std::string err_msg = fmt::format(" identifier \"{}\" must be variable", identifier);
                    push_error(_node, err_msg);
                    break;
                }
                variable_expr_ptr->resolved_type_ = symbol_type->type_;
                break;
            }

        case AstNodeType::function_call:
            {
                auto function_call_ptr = static_cast<AstFunctionCall*>(_node);
                const std::string& identifier = function_call_ptr->identifier_;

                const Symbol* symbol = symbol_table_.lookup(identifier);
                if (!symbol)
                {
                    std::string err_msg = fmt::format("Unknown function '{}'", identifier);
                    push_error(_node, err_msg);
                    break;
                }

                auto func_sym = std::get_if<FunctionSymbol>(&symbol->symbol_signature_);
                if (!func_sym)
                {
                    std::string err_msg = fmt::format("Identifier '{}' is not a function", identifier);
                    push_error(_node, err_msg);
                    break;
                }

                const size_t expected_args = func_sym->param_types_.size();
                const size_t actual_args = function_call_ptr->arguments_.size();
                if (actual_args != expected_args)
                {
                    std::string err_msg = fmt::format("Function '{}' expects {} arguments, got {}",
                                                      identifier, expected_args, actual_args);
                    push_error(_node, err_msg);
                    break;
                }

                for (size_t i = 0; i < actual_args; ++i)
                {
                    auto* arg_ptr = function_call_ptr->arguments_[i].get();
                    check_node(arg_ptr);
                    if (should_abort_)
                    {
                        break;
                    }

                    if (arg_ptr->resolved_type_ != func_sym->param_types_[i].type_)
                    {
                        std::string err_msg = fmt::format(
                            "Argument {} of function '{}' type mismatch: expected {}, got {}",
                            i + 1,
                            identifier,
                            data_type_to_string_view(func_sym->param_types_[i].type_),
                            data_type_to_string_view(arg_ptr->resolved_type_));
                        push_error(arg_ptr, err_msg);
                    }
                }

                function_call_ptr->resolved_type_ = func_sym->return_type_;
                break;
            }
        case AstNodeType::function_declaration:
            {
                auto function_declaration_ptr = static_cast<AstFunctionDeclaration*>(_node);

                const std::string& identifier = function_declaration_ptr->identifier_;
                DataType return_type = function_declaration_ptr->return_type_;
                const auto& params = function_declaration_ptr->parameters_;

                if (!is_global_scope())
                {
                    std::string err_msg = fmt::format("Function declaration '{}' must be at global scope", identifier);
                    push_error(_node, err_msg);
                    break;
                }

                if (return_type == DataType::unresolved_type)
                {
                    std::string err_msg = fmt::format("Function '{}' has unresolved return type", identifier);
                    push_error(_node, err_msg);
                    break;
                }

                const Symbol* symbol = symbol_table_.lookup(identifier, CURRENT_SCOPE_ONLY);
                if (symbol != nullptr)
                {
                    std::string err_msg = fmt::format("Redefinition of identifier '{}'", identifier);
                    push_error(_node, err_msg);
                    break;
                }

                bool has_param_error = false;
                for (size_t i = 0; i < params.size(); ++i)
                {
                    const auto& param = params[i];
                    if (param.type_ == DataType::void_type || param.type_ == DataType::unresolved_type)
                    {
                        std::string err_msg = fmt::format("Parameter '{}' cannot be {}",
                                                          param.name_,
                                                          data_type_to_string_view(param.type_));
                        push_error(_node, err_msg);
                        has_param_error = true;
                    }

                    for (size_t j = i + 1; j < params.size(); ++j)
                    {
                        if (param.name_ == params[j].name_)
                        {
                            std::string err_msg = fmt::format("Duplicate parameter name '{}' in function '{}'",
                                                              param.name_,
                                                              identifier);
                            push_error(_node, err_msg);
                            has_param_error = true;
                            break;
                        }
                    }
                }

                if (has_param_error)
                {
                    break;
                }

                symbol_table_.insert_func_symbol(identifier, return_type, params);
                if (identifier == ENTRY_POINT_IDENTIFIER)
                {
                    check_main();
                }
                break;
            }
        case AstNodeType::function_definition:
            {
                auto function_def_ptr = static_cast<AstFunctionDefinition*>(_node);
                const std::string& identifier = function_def_ptr->identifier_;
                DataType return_type = function_def_ptr->return_type_;
                const auto& params = function_def_ptr->parameters_;

                if (!is_global_scope())
                {
                    std::string err_msg = fmt::format("Function definition '{}' must be at global scope", identifier);
                    push_error(_node, err_msg);
                    break;
                }

                if (return_type == DataType::unresolved_type)
                {
                    std::string err_msg = fmt::format("Function '{}' has unresolved return type", identifier);
                    push_error(_node, err_msg);
                    break;
                }

                const Symbol* symbol = symbol_table_.lookup(identifier, CURRENT_SCOPE_ONLY);
                if (symbol != nullptr)
                {
                    std::string err_msg = fmt::format("Redefinition of identifier '{}'", identifier);
                    push_error(_node, err_msg);
                    break;
                }

                bool has_param_error = false;
                for (size_t i = 0; i < params.size(); ++i)
                {
                    const auto& param = params[i];
                    if (param.type_ == DataType::void_type || param.type_ == DataType::unresolved_type)
                    {
                        std::string err_msg = fmt::format("Parameter '{}' cannot be {}",
                                                          param.name_,
                                                          data_type_to_string_view(param.type_));
                        push_error(_node, err_msg);
                        has_param_error = true;
                    }

                    for (size_t j = i + 1; j < params.size(); ++j)
                    {
                        if (param.name_ == params[j].name_)
                        {
                            std::string err_msg = fmt::format("Duplicate parameter name '{}' in function '{}'",
                                                              param.name_,
                                                              identifier);
                            push_error(_node, err_msg);
                            has_param_error = true;
                            break;
                        }
                    }
                }

                if (has_param_error)
                {
                    break;
                }

                symbol_table_.insert_func_symbol(identifier, return_type, params);
                if (identifier == ENTRY_POINT_IDENTIFIER)
                {
                    check_main();
                }
                symbol_table_.push_scope();
                for (const auto& param:params)
                {
                    symbol_table_.insert_var_symbol(param.name_,param.type_);
                }
                check_node(function_def_ptr->body_.get());
                symbol_table_.pop_scope();
                break;
            }
        case AstNodeType::undefined:
        default:
            {
                push_error(_node, "Unrecognized or undefined AST node");
                should_abort_ = true;
                break;
            }
        }

        return !should_abort_;
    }


    bool SemanticChecker::check_unary_expression(AstUnaryExpression* _unary_node)
    {
        if (!_unary_node) return false;
        if (!_unary_node->rhs_) return false;
        symbol_table_.push_scope();
        if (!check_node(_unary_node->rhs_.get()))
        {
            symbol_table_.pop_scope();
            return false;
        }
        bool had_error = false;
        DataType rhs_data_type = _unary_node->rhs_->resolved_type_;
        TokenType operator_name = _unary_node->operator_.type_;
        switch (operator_name)
        {
        case TokenType::minus:
            if (rhs_data_type != DataType::int_type)
            {
                std::string err_msg = fmt::format("Expected integer type with unary operation {}",
                                                  _unary_node->operator_.payload_);
                push_error(_unary_node, err_msg);

                symbol_table_.pop_scope();
                return false;
            }
            break;
        case TokenType::negation_op:
            if (rhs_data_type != DataType::bool_type)
            {
                std::string err_msg = fmt::format("Expected boolean type with unary operation {}",
                                                  _unary_node->operator_.payload_);
                push_error(_unary_node, err_msg);
                symbol_table_.pop_scope();
                return false;
            }
            break;
        default:
            break;
        }
        symbol_table_.pop_scope();
    }


    bool SemanticChecker::check_binary_expression(AstBinaryExpression* _binary_expression_node)
    {
        if (!_binary_expression_node) return false;
        if (!_binary_expression_node->rhs_) return false;
        if (!_binary_expression_node->lhs_) return false;
        if (!check_node(_binary_expression_node->rhs_.get()))
        {
            return false;
        }
        if (!check_node(_binary_expression_node->lhs_.get()))
        {
            return false;
        }
        DataType lhs_data_type = _binary_expression_node->lhs_->resolved_type_;
        DataType rhs_data_type = _binary_expression_node->rhs_->resolved_type_;
        const std::string& operator_name = _binary_expression_node->operator_.payload_;

        switch (_binary_expression_node->operator_.type_)
        {
        case TokenType::plus:
        case TokenType::minus:
        case TokenType::multiply_op:
        case TokenType::divide_op:

            if (rhs_data_type != DataType::int_type || lhs_data_type != DataType::int_type)
            {
                std::string err_msg = fmt::format("Both types must be integer for {} operator", operator_name);
                push_error(_binary_expression_node, err_msg);
                return false;
            }
            _binary_expression_node->resolved_type_ = DataType::int_type;
            return true;
        case TokenType::greater_op:
        case TokenType::smaller_op:
            if (rhs_data_type != DataType::int_type || lhs_data_type != DataType::int_type)
            {
                std::string err_msg = fmt::format("Both types must be integer for {} operator", operator_name);
                push_error(_binary_expression_node, err_msg);
                return false;
            }
            _binary_expression_node->resolved_type_ = DataType::bool_type;
            return true;
        case TokenType::equal_op:
        case TokenType::not_equal_op:
            if (rhs_data_type != DataType::bool_type || lhs_data_type != DataType::bool_type)
            {
                std::string err_msg = fmt::format("Both types must be boolean for {} operator", operator_name);
                push_error(_binary_expression_node, err_msg);
                return false;
            }
            _binary_expression_node->resolved_type_ = DataType::bool_type;
            return true;
        case TokenType::or_op:
        case TokenType::and_op:
            if (rhs_data_type != lhs_data_type)
            {
                std::string err_msg = fmt::format("Types missmatch, {} can't compare boolean to int", operator_name);
                push_error(_binary_expression_node, err_msg);
                return false;
            }
            _binary_expression_node->resolved_type_ = DataType::bool_type;
            return true;
        default:
            return false;

        }
    }
}
