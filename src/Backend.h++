#pragma once
#include<llvm/Target/TargetMachine.h>
namespace fela::module
{
    bool emit_object_file(llvm::Module* _module,std::string_view _obj_path);
    bool link_c_runtime_library(std::string_view _obj_path, std::string_view _binary_path);
} // fela