
#include <filesystem>
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>
#include <fmt/format.h>

#include "AstWalk.h++"
#include "Backend.h++"
#include "CodeGen.h++"
#include "Lexer.h++"
#include "Parser.h++"
#include "SemanticChecker.h++"

constexpr int EXIT_CODE_SUCCESS = 0;
constexpr int EXIT_CODE_FAILURE = 1;
constexpr int CLI_FIRST_ARG_INDEX = 1;

constexpr std::string_view CLI_FLAG_PATH_SHORT = "-p";
constexpr std::string_view CLI_FLAG_PATH_LONG = "--path";
constexpr std::string_view CLI_FLAG_AST_DUMP_SHORT = "-d";
constexpr std::string_view CLI_FLAG_AST_DUMP = "--ast-dump";
constexpr std::string_view CLI_FLAG_OUTPUT = "--out";
constexpr std::string_view CLI_FLAG_OUTPUT_SHORT = "-o";
constexpr std::string_view CLI_FLAG_COMPILE_ONLY = "-c";
constexpr std::string_view CLI_FLAG_EMIT_LLVM_SHORT = "-l";
constexpr std::string_view CLI_FLAG_EMIT_LLVM = "--emit-llvm";
constexpr std::string_view CLI_FLAG_HELP_SHORT = "-h";
constexpr std::string_view CLI_FLAG_HELP = "--help";

struct CliOptions
{
    std::string_view file_path_{};
    bool dump_ast_{false};
    bool compile_only_{false};
    bool emit_llvm_only_{false};
    bool show_help_{false};
    std::string output_name_{};
};

void print_usage(std::string_view _program_name)
{
    std::cerr << "Usage: " << _program_name << " [options] <source_file>\n"
              << "Options:\n"
              << "  <source_file>                     Input source file (or " << CLI_FLAG_PATH_SHORT << " <file>)\n"
              << "  " << CLI_FLAG_OUTPUT_SHORT << ", " << CLI_FLAG_OUTPUT << " <name>                Output base name (default: input file name)\n"
              << "  " << CLI_FLAG_COMPILE_ONLY << "                                Compile to object file only (do not link)\n"
              << "  " << CLI_FLAG_EMIT_LLVM_SHORT << ", " << CLI_FLAG_EMIT_LLVM << "                 Emit LLVM IR file only\n"
              << "  " << CLI_FLAG_AST_DUMP_SHORT << ", " << CLI_FLAG_AST_DUMP << "                    Print AST after parsing\n"
              << "  " << CLI_FLAG_HELP_SHORT << ", " << CLI_FLAG_HELP << "                       Display this help message\n";
}

std::optional<CliOptions> parse_args(int _argc, char** _argv)
{
    CliOptions options;

    for (int i = CLI_FIRST_ARG_INDEX; i < _argc; ++i)
    {
        const std::string_view arg{_argv[i]};

        if (arg == CLI_FLAG_HELP || arg == CLI_FLAG_HELP_SHORT)
        {
            options.show_help_ = true;
            return options;
        }
        else if (arg == CLI_FLAG_PATH_SHORT || arg == CLI_FLAG_PATH_LONG)
        {
            if (++i >= _argc)
            {
                std::cerr << "Error: " << arg << " requires argument\n";
                return std::nullopt;
            }
            options.file_path_ = _argv[i];
        }
        else if (arg == CLI_FLAG_AST_DUMP || arg == CLI_FLAG_AST_DUMP_SHORT)
        {
            options.dump_ast_ = true;
        }
        else if (arg == CLI_FLAG_COMPILE_ONLY)
        {
            options.compile_only_ = true;
        }
        else if (arg == CLI_FLAG_EMIT_LLVM || arg == CLI_FLAG_EMIT_LLVM_SHORT)
        {
            options.emit_llvm_only_ = true;
        }
        else if (arg == CLI_FLAG_OUTPUT || arg == CLI_FLAG_OUTPUT_SHORT)
        {
            if (++i >= _argc)
            {
                std::cerr << "Error: " << arg << " requires argument\n";
                return std::nullopt;
            }
            options.output_name_ = _argv[i];
        }
        else if (!arg.starts_with("-"))
        {
            if (!options.file_path_.empty())
            {
                std::cerr << "Error: multiple input files not supported ('" << options.file_path_ << "' and '" << arg << "')\n";
                return std::nullopt;
            }
            options.file_path_ = arg;
        }
        else
        {
            std::cerr << "Unknown option: " << arg << "\n";
            return std::nullopt;
        }
    }

    if (options.file_path_.empty())
    {
        std::cerr << "Error: input source file required\n";
        print_usage(_argv[0]);
        return std::nullopt;
    }

    if (options.output_name_.empty())
    {
        options.output_name_ = std::filesystem::path(options.file_path_).stem().string();
    }

    return options;
}

int main(int _argc, char** _argv)
{
    const auto cli = parse_args(_argc, _argv);
    if (!cli)
    {
        return EXIT_CODE_FAILURE;
    }
    if (cli->show_help_)
    {
        print_usage(_argv[0]);
        return EXIT_CODE_SUCCESS;
    }

    fela::Lexer lexer;
    if (!lexer.load_from_file(std::string(cli->file_path_)))
    {
        std::cerr << "Failed to open file: " << cli->file_path_ << "\n";
        return EXIT_CODE_FAILURE;
    }

    auto tokens = lexer.tokenize();
    fela::Parser parser(tokens);
    auto program = parser.parse_program();

    for (const auto& err : parser.errors())
    {
        std::cout << err;
    }

    if (!program)
    {
        std::cerr << "Failed to parse program\n";
        return EXIT_CODE_FAILURE;
    }
    std::cout << "Program parsed\n";
    if (cli->dump_ast_)
    {
        std::cout << fela::ast_dump_node(program.get());
    }

    fela::SemanticChecker sema;
    const bool is_semantic_passed = sema.check(program.get());

    for (const auto& err : sema.errors())
    {
        std::cout << err;
    }

    if (!is_semantic_passed)
    {
        return EXIT_CODE_FAILURE;
    }

    std::cout << "Semantic passed\n";

    fela::CodeGen ir_representation;
    ir_representation.generate_node(program.get());
    const std::string raw_ir_path = fmt::format("{}_raw.ll", cli->output_name_);
    const std::string ir_path = fmt::format("{}.ll", cli->output_name_);
    const std::string obj_path = fmt::format("{}.o", cli->output_name_);
    const std::string binary_path = cli->output_name_;

    if (!ir_representation.save_ir_to_file(raw_ir_path))
    {
        std::cerr << "Error saving raw IR to file: " << raw_ir_path << "\n";
        return EXIT_CODE_FAILURE;
    }
    std::cout << "Generated raw IR file: " << raw_ir_path << "\n";

    fela::backend::optimize_module(ir_representation.get_module());

    if (!ir_representation.save_ir_to_file(ir_path))
    {
        std::cerr << "Error saving optimized IR to file: " << ir_path << "\n";
        return EXIT_CODE_FAILURE;
    }
    std::cout << "Generated optimized IR file: " << ir_path << "\n";

    if (cli->emit_llvm_only_)
    {
        return EXIT_CODE_SUCCESS;
    }

    if (!fela::backend::emit_object_file(ir_representation.get_module(), obj_path))
    {
        std::cerr << "Error emitting object file: " << obj_path << "\n";
        return EXIT_CODE_FAILURE;
    }
    std::cout << "Generated object file: " << obj_path << "\n";

    if (cli->compile_only_)
    {
        return EXIT_CODE_SUCCESS;
    }

    if (!fela::backend::link_c_runtime_library(obj_path, binary_path))
    {
        std::cerr << "Error linking binary: " << binary_path << "\n";
        return EXIT_CODE_FAILURE;
    }
    std::cout << "Generated binary: " << binary_path << "\n";

    return EXIT_CODE_SUCCESS;
}
