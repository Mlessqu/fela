#include "Backend.h++"

#include <llvm/IR/Module.h>
#include <fmt/format.h>
namespace fela::module
{
    bool emit_object_file(llvm::Module* _module, std::string_view _obj_path)
    {
    }


    bool link_library(std::string_view _obj_path, std::string_view _binary_path)
    {
        std::string command = fmt::format("cc {} -o {}",_obj_path,_binary_path);
        auto cmd_result = std::system(command.c_str());
        if (cmd_result==0)
        {
            return true;
        }
        return false;
    }
}
