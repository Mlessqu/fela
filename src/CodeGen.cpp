#include "CodeGen.h++"

namespace fela
{
    //implementation details here


    bool CodeGen::generate_node(AstBase* _node)
    {
        if (!_node)return false;

        switch (_node->node_type_)
        {
        case AstNodeType::primary_instruction:
        case AstNodeType::assign_instruction:
        case AstNodeType::block_instruction:
        case AstNodeType::if_instruction:
        case AstNodeType::return_instruction:
        case AstNodeType::while_instruction:
            codegen_instruction(static_cast<AstInstruction*>(_node));
            return true;

        case AstNodeType::binary_expression:
        case AstNodeType::literal_expression:
        case AstNodeType::unary_expression:
        case AstNodeType::variable_expression:
            return codegen_expression(static_cast<AstExpression*>(_node)) != nullptr;

        case AstNodeType::function_definition:
            return codegen_function(static_cast<AstFunctionDefinition*>(_node)) != nullptr;

        case AstNodeType::program:
            for (const auto& node : static_cast<AstProgram*>(_node)->nodes_)
            {
                generate_node(node.get());
            }
            return true;
        }
    }


    llvm::Function* CodeGen::codegen_function(AstFunctionDefinition* _func)
    {
        llvm::Type* ret_type = to_llvm_type(_func->return_type_);
        std::vector<llvm::Type*> param_types;
        for (const auto& param : _func->parameters_)
        {
            param_types.push_back(to_llvm_type(param.type_));
        }
        llvm::FunctionType* func_type = llvm::FunctionType::get(ret_type,param_types,false);

        llvm::Function* func = llvm::Function::Create(func_type,llvm::Function::ExternalLinkage,_func->identifier_,module_.get());

        llvm::BasicBlock* entry_block = llvm::BasicBlock::Create(*context_,"entry",func);
        builder_.SetInsertPoint(entry_block);
    }


    llvm::Type* CodeGen::to_llvm_type(DataType _type)
    {
        switch (_type)
        {
        case DataType::int_type:
            return builder_.getInt32Ty();
        case DataType::bool_type:
            return builder_.getInt1Ty();
        case DataType::void_type:
            return builder_.getVoidTy();
        default:
            return nullptr;
        }
    }


    llvm::Value* CodeGen::codegen_expression(AstExpression* _expression)
    {
        switch (_expression->resolved_type_)
        {
        case DataType::bool_type:
        case DataType::void_type:
        case DataType::int_type:
            {
            }
        }
    }


    void CodeGen::codegen_instruction(AstInstruction* _instruction)
    {
    }
} // fela
