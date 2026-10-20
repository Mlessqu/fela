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
        case AstNodeType::block_instruction:
        case AstNodeType::if_instruction:
        case AstNodeType::return_instruction:
        case AstNodeType::assign_instruction:
        case AstNodeType::while_instruction:
            return codegen_instruction(static_cast<AstInstruction*>(_node));

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
        llvm::FunctionType* func_type = llvm::FunctionType::get(ret_type, param_types, false);

        llvm::Function* func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, _func->identifier_,
                                                      module_.get());

        llvm::BasicBlock* entry_block = llvm::BasicBlock::Create(*context_, "entry", func);
        builder_.SetInsertPoint(entry_block);
    }


    llvm::AllocaInst* CodeGen::lookup_var(std::string _identifier)
    {
        for (auto scopes = scopes_.rbegin(); scopes != scopes_.rend(); ++scopes)
        {
            auto result = scopes->find(_identifier);
            if (result != scopes->end())
            {
                return result->second;
            }
        }
        return nullptr;
    }


    void CodeGen::new_scope()
    {
        scopes_.emplace_back();
    }


    void CodeGen::close_scope()
    {
        scopes_.pop_back();
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
        //generate type
        switch (_expression->node_type_)
        {
        case AstNodeType::binary_expression:
            {
                const auto& binary_exp = static_cast<AstBinaryExpression*>(_expression);
                const auto& lhs_value = codegen_expression(binary_exp->lhs_.get());
                const auto& rhs_value = codegen_expression(binary_exp->rhs_.get());
                if (!lhs_value || !rhs_value) return nullptr;
                switch (binary_exp->operator_.type_)
                {
                case TokenType::plus:
                    {
                        return builder_.CreateAdd(lhs_value, rhs_value);
                    }
                case TokenType::minus:
                    {
                        return builder_.CreateSub(lhs_value, rhs_value);
                    }
                case TokenType::divide_op:
                    {
                        return builder_.CreateSDiv(lhs_value, rhs_value);
                    }
                case TokenType::multiply_op:
                    {
                        return builder_.CreateMul(lhs_value, rhs_value);
                    }
                case TokenType::smaller_op:
                    {
                        return builder_.CreateICmpSLT(lhs_value, rhs_value);
                    }
                case TokenType::greater_op:
                    {
                        return builder_.CreateICmpSGT(lhs_value, rhs_value);
                    }
                case TokenType::equal_op:
                    {
                        return builder_.CreateICmpEQ(lhs_value, rhs_value);
                    }
                case TokenType::not_equal_op:
                    {
                        return builder_.CreateICmpNE(lhs_value, rhs_value);
                    }
                case TokenType::or_op:
                    {
                        return builder_.CreateOr(lhs_value, rhs_value);
                    }
                case TokenType::and_op:
                    {
                        return builder_.CreateAnd(lhs_value, rhs_value);
                    }
                default:
                    return nullptr;
                }
            }


        case AstNodeType::literal_expression:
            {
                const auto& literal_exp = static_cast<AstLiteralExpression*>(_expression);
                llvm::Type* type = to_llvm_type(literal_exp->resolved_type_);
                switch (literal_exp->resolved_type_)
                {
                case DataType::bool_type:
                    {
                        return builder_.getInt1(std::get<bool>(literal_exp->value_));
                    }
                case DataType::int_type:
                    {
                        return builder_.getInt32(std::get<int>(literal_exp->value_));
                    }
                case DataType::unresolved_type:
                case DataType::void_type:
                default:
                    return nullptr;
                }
            }
        case AstNodeType::unary_expression:
            {
                const auto& unary_expr = static_cast<AstUnaryExpression*>(_expression);
                switch (unary_expr->operator_.type_)
                {
                case TokenType::minus:
                    {
                        const auto& rhs_value = codegen_expression(unary_expr->rhs_.get());
                        return builder_.CreateNeg(rhs_value);
                    }
                case TokenType::negation_op:
                    {
                        const auto& rhs_value = codegen_expression(unary_expr->rhs_.get());
                        return builder_.CreateNot(rhs_value);
                    }

                    return nullptr;
                }
            }


        case AstNodeType::variable_expression:
            {
                const auto& var_expr = static_cast<AstVariableExpression*>(_expression);
                llvm::AllocaInst* look_up = lookup_var(var_expr->identifier_);
                if (!look_up) return nullptr;
                return builder_.CreateLoad(look_up->getAllocatedType(), look_up, var_expr->identifier_);
            }

        default:
            return nullptr;
        }
    }


    bool CodeGen::codegen_instruction(AstInstruction* _instruction)
    {
        if (!_instruction) return false;
        switch (_instruction->node_type_)
        {
        case AstNodeType::primary_instruction:
            {
                const auto& primary_instr = static_cast<AstPrimaryInstruction*>(_instruction);
                llvm::Value* resolved_val = codegen_expression(primary_instr->expression_.get());
                if (!resolved_val) return false;
                return true;
            }
        case AstNodeType::variable_declaration:
            {
                const auto& var_decl = static_cast<AstVariableDeclaration*>(_instruction);


                llvm::Type* type = to_llvm_type(var_decl->type_);
                if (!type) return false;
                llvm::AllocaInst* var_name_alloca = builder_.CreateAlloca(
                    type, nullptr, var_decl->identifier_);


                scopes_.back()[var_decl->identifier_] = var_name_alloca;
                if (var_decl->init_value_)
                {
                    llvm::Value* value = codegen_expression(var_decl->init_value_.get());
                    builder_.CreateStore(value, var_name_alloca);
                }
                return true;

            }
        case AstNodeType::assign_instruction:
            {
                const auto& assign_instr = static_cast<AstAssignInstruction*>(_instruction);
                auto lhs = lookup_var(assign_instr->identifier_);
                if (!lhs) return false;
                llvm::Value* value = codegen_expression(assign_instr->rhs_.get());
                builder_.CreateStore(value, lhs);
                return true;
            }
        case AstNodeType::block_instruction:
            {
                const auto& block_instr = static_cast<AstBlockInstruction*>(_instruction);
                new_scope();
                for (auto& instruction : block_instr->body_)
                {
                    if (!codegen_instruction(instruction.get()))
                    {
                        return false;
                    }
                }
                close_scope();
                return true;
            }
        case AstNodeType::if_instruction:
            {
                const auto& if_instr = static_cast<AstIfInstruction*>(_instruction);
                return true;
            }
        case AstNodeType::while_instruction:
            {
                const auto& while_instr = static_cast<AstWhileInstruction*>(_instruction);
                llvm::Function* current_func = builder_.GetInsertBlock()->getParent();
                llvm::BasicBlock* loop_condition = llvm::BasicBlock::Create(*context_, "loop_start",current_func);
                llvm::BasicBlock* loop = llvm::BasicBlock::Create(*context_, "loop",current_func);
                llvm::BasicBlock* end = llvm::BasicBlock::Create(*context_, "end",current_func);
                builder_.SetInsertPoint(loop_condition);
                llvm::Value* loop_conditional = codegen_expression(while_instr->condition_.get());
                builder_.CreateCondBr(loop_conditional,loop,end);
                builder_.SetInsertPoint(loop);
                new_scope();
                bool result = codegen_instruction(while_instr->body_.get());
                close_scope();
                return result;
            }
        case AstNodeType::return_instruction:
            {
                const auto& return_instr = static_cast<AstReturnInstruction*>(_instruction);
                llvm::Value* value = codegen_expression(return_instr->expression_.get());
                if (!return_instr->expression_)
                {
                    builder_.CreateRetVoid();
                    return true;
                }
                builder_.CreateRet(value);
                return true;
            }
        default:
            return false;
        }
    }
} // fela
