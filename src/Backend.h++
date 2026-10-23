#pragma once
#include<llvm/Passes/PassBuilder.h>
#include<llvm/Transforms/Utils/Mem2Reg.h>
namespace fela::backend
{
    bool emit_object_file(llvm::Module* _module,std::string_view _obj_path);
    void optimize_module(llvm::Module* _module);
    bool link_c_runtime_library(std::string_view _obj_path, std::string_view _binary_path);
} // fela