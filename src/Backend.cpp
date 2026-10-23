#include "Backend.h++"

#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <fmt/format.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar/Reassociate.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>
#include <llvm/Transforms/Utils/Mem2Reg.h>

namespace fela::backend
{
    /* TODO:

    •
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
    namespace
    {
    }


    bool emit_object_file(llvm::Module* _module, std::string_view _obj_path)
    {
        if (llvm::verifyModule(*_module, &llvm::errs()))
        {
            return false;
        }

        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmParser();
        llvm::InitializeNativeTargetAsmPrinter();
        std::string the_famous_triple_string = llvm::sys::getDefaultTargetTriple();
        llvm::Triple triple(the_famous_triple_string);
        _module->setTargetTriple(triple);
        std::string err;
        const llvm::Target* target = llvm::TargetRegistry::lookupTarget(triple, err);
        if (!target) return false;
        llvm::TargetOptions options;
        llvm::TargetMachine* machine = target->createTargetMachine(triple, "generic", "", options, llvm::Reloc::PIC_);
        _module->setDataLayout(machine->createDataLayout());
        std::error_code err_code;
        llvm::raw_fd_ostream target_file(_obj_path, err_code, llvm::sys::fs::OF_None);
        if (err_code) return false;
        llvm::legacy::PassManager pass_manager;
        if (machine->addPassesToEmitFile(pass_manager, target_file, nullptr, llvm::CodeGenFileType::ObjectFile))
        {
            return false;
        }
        pass_manager.run(*_module);
        target_file.flush();
        return true;
    }


    void optimize_module(llvm::Module* _module, llvm::OptimizationLevel _optimization_level)
    {
        llvm::LoopAnalysisManager loop_analysis_manager;
        llvm::FunctionAnalysisManager function_analysis_manager;
        llvm::CGSCCAnalysisManager recursion_analysis_manager;
        llvm::ModuleAnalysisManager module_analysis_manager;

        //register in pass builder
        llvm::PassBuilder optimization_pass_builder;
        optimization_pass_builder.registerModuleAnalyses(module_analysis_manager);
        optimization_pass_builder.registerCGSCCAnalyses(recursion_analysis_manager);
        optimization_pass_builder.registerFunctionAnalyses(function_analysis_manager);
        optimization_pass_builder.registerLoopAnalyses(loop_analysis_manager);
        optimization_pass_builder.crossRegisterProxies(loop_analysis_manager,
                                                       function_analysis_manager,
                                                       recursion_analysis_manager,
                                                       module_analysis_manager);
        llvm::FunctionPassManager function_pass_manager;
        function_pass_manager.addPass(llvm::PromotePass());
        function_pass_manager.addPass(llvm::InstCombinePass());
        function_pass_manager.addPass(llvm::ReassociatePass());
        function_pass_manager.addPass(llvm::GVNPass());
        function_pass_manager.addPass(llvm::SimplifyCFGPass());

        for (llvm::Function& function : *_module)
        {
            if (!function.isDeclaration())
            {
                function_pass_manager.run(function,function_analysis_manager);
            }
        }

    }


    bool link_c_runtime_library(std::string_view _obj_path, std::string_view _binary_path)
    {
        std::string command = fmt::format("cc {} -o {}", _obj_path, _binary_path);
        auto cmd_result = std::system(command.c_str());
        if (cmd_result == 0)
        {
            return true;
        }
        return false;
    }
}
