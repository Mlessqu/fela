#include "AstWalk.h++"
#include <iostream>
#include <fmt/format.h>
#include "AbstractSyntaxTree.h++"
namespace fela
{
    std::string ast_dump_node(AstBase* _node)
    {
        std::string dumped_nodes{};
        if (!_node) return dumped_nodes;
        switch (_node->node_type_)
        {
        //TODO: case for every AstSyntaxTree Struct
        case AstNodeType::program:
            {
                auto ast_program_ptr = static_cast<AstProgram*>(_node);
                dumped_nodes += fmt::format("Ast Program node with {} nodes \n", ast_program_ptr->nodes_.size());
                int i =1;
                for (const auto& node : ast_program_ptr->nodes_)
                {
                    dumped_nodes += fmt::format("Node number: {}:\n",i);
                    dumped_nodes += ast_dump_node(node.get());
                    ++i;
                }
                break;
            }
        case AstNodeType::variable_declaration:
            {
                auto ast_variable_decl_ptr = static_cast<AstVariableDeclaration*>(_node);

                dumped_nodes+= fmt::format("Ast variable declaration node, with {} identifier",ast_variable_decl_ptr->identifier_);
                if (!ast_variable_decl_ptr->init_value_)
                {
                    dumped_nodes+= fmt::format(", without init value\n");
                }else
                {
                    dumped_nodes+= fmt::format(", with init value:\n");
                    dumped_nodes+= ast_dump_node(ast_variable_decl_ptr->init_value_.get());
                }
                break;
            }
        case AstNodeType::assign_instruction:
            {
                auto assign_instruction_ptr = static_cast<AstAssignInstruction*>(_node);
                dumped_nodes += fmt::format("Assign instruction node with {} identifier\n",
                                            assign_instruction_ptr->identifier_);
                std::cout << dumped_nodes;
                break;
            }
        case AstNodeType::if_instruction:
            {
                auto if_instruction_ptr = static_cast<AstIfInstruction*>(_node);
                dumped_nodes += fmt::format("If instruction node");

                if (if_instruction_ptr->else_branch_)
                {
                    dumped_nodes += fmt::format("with else\n");
                }else
                {
                    dumped_nodes += fmt::format("without else\n");
                }

                break;
            }
        case AstNodeType::while_instruction:
        case AstNodeType::return_instruction:
        case AstNodeType::primary_instruction:
        case AstNodeType::binary_expression:
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