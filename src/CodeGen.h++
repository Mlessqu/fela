#pragma once
//TODO: construct CodeGen, needs AST to constructs, initializes LLVM module and builder, then walks AST and emits eeach node via llvm::IRBuilder into llvm::Module
#include <llvm/IR/LLVMContext.h> // Core context
#include <llvm/IR/Module.h>      // Represents the .ll file
#include <llvm/IR/IRBuilder.h>   // Instruction emitter

#pragma once

    #include <memory>
    #include <string_view>
    #include <llvm/IR/IRBuilder.h>
    #include <llvm/IR/LLVMContext.h>
    #include <llvm/IR/Module.h>

    #include "AbstractSyntaxTree.h++"

    namespace fela
    {

        class CodeGen
        {
        public:
            explicit CodeGen(std::string_view _module_name = "fela_module")
                : context_{std::make_unique<llvm::LLVMContext>()},
                  module_{std::make_unique<llvm::Module>(_module_name, *context_)},
                  builder_{*context_}
            {
            }

            bool generate_node(AstBase* _node);
            llvm::Function* codegen_function(AstFunctionDefinition* _func);
            llvm::Value* codegen_expression(AstExpression* _expression);
            bool codegen_instruction(AstInstruction* _instruction);
        private:
            void new_scope();
            void close_scope();
            llvm::AllocaInst* lookup_var(std::string _identifier);
            llvm::Type* to_llvm_type(DataType _type);
            std::unique_ptr<llvm::LLVMContext> context_;
            std::unique_ptr<llvm::Module> module_;
            llvm::IRBuilder<> builder_;
            std::vector<std::unordered_map<std::string, llvm::AllocaInst*>> scopes_;
        };
    } // fela
