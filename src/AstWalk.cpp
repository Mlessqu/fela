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
                dumped_nodes+= fmt::format("{}Ast binary expression node:\n",pad);
                dumped_nodes+= fmt::format( "{}lhs:\n",pad);
                if (binary_expr_ptr->lhs_)
                {
                dumped_nodes += ast_dump_node(binary_expr_ptr->lhs_.get());
                }
                Token binary_operator = binary_expr_ptr->operator_;
                dumped_nodes+= fmt::format("{}Binary operator:{}\n",pad,)

                break;
            }
        case AstNodeType::unary_expression:
        case AstNodeType::literal_expression:
        case AstNodeType::variable_expression:
        case AstNodeType::function_call:
        case AstNodeType::function_declaration:
        case AstNodeType::function_definition:
        case AstNodeType::undefined:



        }
        return dumped_nodes;
    }


    //implementation details here
} // fela
