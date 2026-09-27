#include "AstWalk.h++"
#include <fmt/format.h>
#include "AbstractSyntaxTree.h++"

namespace fela
{
    constexpr int SPACES_PER_INDENT = 2;


    std::string ast_dump_node(AstBase* _node, int _indent)
    {
        std::string dumped_nodes{};
        std::string pad(_indent * SPACES_PER_INDENT, ' ');
        if (!_node) return dumped_nodes;
        switch (_node->node_type_)
        {
        //TODO: case for every AstSyntaxTree Struct
        case AstNodeType::program:
            {
                auto ast_program_ptr = static_cast<AstProgram*>(_node);
                dumped_nodes += fmt::format("{}Ast Program node with {} nodes\n", pad, ast_program_ptr->nodes_.size());
                int i = 1;
                for (const auto& node : ast_program_ptr->nodes_)
                {
                    dumped_nodes += fmt::format("{}Node number {}:\n", pad, i);
                    dumped_nodes += ast_dump_node(node.get(), _indent + 1);
                    ++i;
                }
                break;
            }
        case AstNodeType::variable_declaration:
            {
                auto ast_variable_decl_ptr = static_cast<AstVariableDeclaration*>(_node);

                dumped_nodes += fmt::format("{}Ast variable declaration node with {} identifier",
                                            pad, ast_variable_decl_ptr->identifier_);
                if (!ast_variable_decl_ptr->init_value_)
                {
                    dumped_nodes += ", without init value\n";
                }
                else
                {
                    dumped_nodes += ", with init value:\n";
                    dumped_nodes += ast_dump_node(ast_variable_decl_ptr->init_value_.get(), _indent + 1);
                }
                break;
            }
        case AstNodeType::assign_instruction:
            {
                auto assign_instruction_ptr = static_cast<AstAssignInstruction*>(_node);
                dumped_nodes += fmt::format("{}Assign instruction node with {} identifier:\n",
                                            pad, assign_instruction_ptr->identifier_);
                if (assign_instruction_ptr->rhs_)
                {
                    dumped_nodes += ast_dump_node(assign_instruction_ptr->rhs_.get(), _indent + 1);
                }
                break;
            }
        case AstNodeType::if_instruction:
            {
                auto if_instruction_ptr = static_cast<AstIfInstruction*>(_node);
                dumped_nodes += fmt::format("{}If instruction node\n", pad);

                if (if_instruction_ptr->condition_)
                {
                    dumped_nodes += fmt::format("{}with condition:\n", pad);
                    dumped_nodes += ast_dump_node(if_instruction_ptr->condition_.get(), _indent + 1);
                }
                if (if_instruction_ptr->then_)
                {
                    dumped_nodes += fmt::format("{}with body:\n", pad);
                    dumped_nodes += ast_dump_node(if_instruction_ptr->then_.get(), _indent + 1);
                }
                if (if_instruction_ptr->else_branch_)
                {
                    dumped_nodes += fmt::format("{}with else:\n", pad);
                    dumped_nodes += ast_dump_node(if_instruction_ptr->else_branch_.get(), _indent + 1);
                }
                else
                {
                    dumped_nodes += fmt::format("{}without else\n", pad);
                }
                break;
            }
        case AstNodeType::while_instruction:
            {
                auto while_instruction_ptr = static_cast<AstWhileInstruction*>(_node);
                dumped_nodes += fmt::format("{}Ast while instruction node\n", pad);
                dumped_nodes += fmt::format("{}Ast while instruction condition:\n", pad);
                if (while_instruction_ptr->condition_)
                {
                    dumped_nodes += ast_dump_node(while_instruction_ptr->condition_.get(), _indent + 1);
                }
                dumped_nodes += fmt::format("{}Ast while instruction body:\n", pad);
                if (while_instruction_ptr->body_)
                {
                    dumped_nodes += ast_dump_node(while_instruction_ptr->body_.get(), _indent + 1);
                }
                break;
            }
        case AstNodeType::return_instruction:
            {
                auto return_instruction_pointer = static_cast<AstReturnInstruction*>(_node);
                dumped_nodes += fmt::format("{}Ast return instruction node:\n", pad);
                if (return_instruction_pointer->expression_)
                {
                    dumped_nodes += ast_dump_node(return_instruction_pointer->expression_.get(), _indent + 1);
                }
                break;
            }
        case AstNodeType::primary_instruction:
            {
                auto primary_instruction_ptr = static_cast<AstPrimaryInstruction*>(_node);
                dumped_nodes += fmt::format("{}Ast primary instruction node:\n", pad);
                if (primary_instruction_ptr->expression_)
                {
                    dumped_nodes += ast_dump_node(primary_instruction_ptr->expression_.get(), _indent + 1);
                }
                break;
            }
        case AstNodeType::binary_expression:
            {
                auto binary_expr_ptr = static_cast<AstBinaryExpression*>(_node);
                dumped_nodes += fmt::format("{}Ast binary expression node:\n", pad);
                dumped_nodes += fmt::format("{}lhs:\n", pad);
                if (binary_expr_ptr->lhs_)
                {
                    dumped_nodes += ast_dump_node(binary_expr_ptr->lhs_.get());
                }


                Token binary_operator = binary_expr_ptr->operator_;
                dumped_nodes += fmt::format("{}Binary operator:{}\n", pad, binary_operator.payload_);


                dumped_nodes += fmt::format("{}Rhs:\n", pad);
                if (binary_expr_ptr->rhs_)
                {
                    dumped_nodes += ast_dump_node(binary_expr_ptr->rhs_.get(), _indent + 1);
                }
                break;
            }
        case AstNodeType::unary_expression:
            {
                auto unary_expr_ptr = static_cast<AstUnaryExpression*>(_node);
                dumped_nodes += fmt::format("{}Unary expression node:\n", pad);


                Token unary_operator = unary_expr_ptr->operator_;
                dumped_nodes += fmt::format("{}Unary operator:{}\n", pad, unary_operator.payload_);

                dumped_nodes += fmt::format("{}Rhs:\n", pad);
                if (unary_expr_ptr->rhs_)
                {
                    dumped_nodes += ast_dump_node(unary_expr_ptr->rhs_.get(), _indent + 1);
                }
                break;
            }
        case AstNodeType::literal_expression:
            {
                auto literal_expr_ptr = static_cast<AstLiteralExpression*>(_node);
                dumped_nodes += fmt::format("{}Literal expression node:\n", pad);
                const bool is_int = std::holds_alternative<int>(literal_expr_ptr->value_);
                const bool is_bool = std::holds_alternative<bool>(literal_expr_ptr->value_);
                if (is_int)
                {
                    int value = std::get<int>(literal_expr_ptr->value_);
                    dumped_nodes += fmt::format("{}Value:{}", pad, value);
                }
                if (is_bool)
                {
                    bool value = std::get<bool>(literal_expr_ptr->value_);
                    dumped_nodes += fmt::format("{}Value:{}\n", pad, value);
                }
                break;
            }
        case AstNodeType::variable_expression:
            {
                auto variable_expr_ptr = static_cast<AstVariableExpression*>(_node);
                dumped_nodes += fmt::format("{}Ast variable expression node:\n with identifier: \n", pad,
                                            variable_expr_ptr->identifier_);
                break;
            }
        case AstNodeType::function_call:
            {
                auto function_call_ptr = static_cast<AstFunctionCall*>(_node);
                const std::string& identifier = function_call_ptr->identifier_;
                int arg_size = function_call_ptr->arguments_.size();
                DataType return_type = function_call_ptr->resolved_type_;
                std::string ret_string{data_type_to_string_view(return_type)};
                dumped_nodes += fmt::format("{}Ast function call node:\n"
                                            "with identifier:{}\n"
                                            "return type:{}\n"
                                            "And {} arguments:\n", pad, identifier, ret_string, arg_size);

                for (const auto& arg : function_call_ptr->arguments_)
                {
                    dumped_nodes += ast_dump_node(arg.get(), _indent + 1);
                }
                break;
            }
        case AstNodeType::function_declaration:
            {
                auto function_declaration_ptr = static_cast<AstFunctionDeclaration*>(_node);
                const std::string& identifier = function_declaration_ptr->identifier_;
                DataType return_type = function_declaration_ptr->return_type_;
                std::string ret_string{data_type_to_string_view(return_type)};
                int param_size = function_declaration_ptr->parameters_.size();
                dumped_nodes += fmt::format("{}Function declaration node:\n"
                                            "identifier:{}\n"
                                            "return type:{}\n"
                                            "and {} params",pad,identifier,ret_string,param_size);
                for (const auto& param : function_declaration_ptr->parameters_)
                {

                }
                break;
            }
        case AstNodeType::function_definition:
            {
                auto function_declaration_ptr = static_cast<AstFunctionDefinition*>(_node);
                const std::string& identifier = function_declaration_ptr->identifier_;
                DataType return_type = function_declaration_ptr->return_type_;
                std::string ret_string{data_type_to_string_view(return_type)};
                int param_size = function_declaration_ptr->parameters_.size();
                dumped_nodes += fmt::format("{}Function definition node:\n"
                                            "identifier:{}\n"
                                            "return type:{}\n"
                                            "{} params\n"
                                            "Body:\n",pad,identifier,ret_string,param_size);
                for (const auto& param : function_declaration_ptr->parameters_)
                {

                }
                dumped_nodes+= ast_dump_node(function_declaration_ptr->body_.get(),_indent+1);

                break;
            }
        case AstNodeType::undefined:
            {
                dumped_nodes+= fmt::format("{}Unrecognized node! Consider updating dump walk function, however if it's correct then wtf is going on here? Where did this node came from?",pad);
                break;
            }
        }
        return dumped_nodes;
    }


    //implementation details here
} // fela
