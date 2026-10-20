#include "Backend.h++"

#include <llvm/IR/Module.h>
#include <fmt/format.h>
#include<llvm/Support/TargetSelect.h>
#include<llvm/TargetParser/Host.h>
#include<llvm/MC/TargetRegistry.h>
#include<llvm/Target/TargetMachine.h>
#include<llvm/IR/LegacyPassManager.h>
#include<llvm/Support/FileSystem.h>
namespace fela::backend
{
    /* TODO:
    * 1. Wake up LLVM's native x86 backend:
    • Call llvm::InitializeNativeTarget();
    • Call llvm::InitializeNativeTargetAsmPrinter();
    2. Get your machine's "Target Triple" & find the llvm::Target*:
    • std::string triple_str = llvm::sys::getDefaultTargetTriple();
    • llvm::Triple triple(triple_str);
    • _module->setTargetTriple(triple);
    • std::string err;
    • const llvm::Target* target = llvm::TargetRegistry::lookupTarget(triple, err); (return false if !target).
    3. Create the llvm::TargetMachine & set DataLayout on _module:
    • llvm::TargetOptions opt;
    • llvm::TargetMachine* machine = target->createTargetMachine(triple, "generic", "", opt, llvm::Reloc::PIC_);
    • _module->setDataLayout(machine->createDataLayout());
    4. Open the .o file stream (same way you did in save_ir_to_file):
    • llvm::raw_fd_ostream dest(_obj_path, ec, llvm::sys::fs::OF_None);
    5. Run the object-file emission pass:
    • Create llvm::legacy::PassManager pass;
    • Register the .o writer: machine->addPassesToEmitFile(pass, dest, nullptr, llvm::CodeGenFileType::ObjectFile)
    (note: returns true on error!).
    • Run it: pass.run(*_module);
    • dest.flush();
*/
    bool emit_object_file(llvm::Module* _module, std::string_view _obj_path)
    {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmParser();
        std::string the_famous_triple_string = llvm::sys::getDefaultTargetTriple();
    }


    bool link_c_runtime_library(std::string_view _obj_path, std::string_view _binary_path)
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
