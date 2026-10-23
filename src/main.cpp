
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
constexpr std::string_view CLI_FLAG_OUTPUT = "--out";
constexpr std::string_view CLI_FLAG_OUTPUT_SHORT = "-o";
constexpr std::string_view CLI_FLAG_DUMP_ALL_SHORT = "-d";
constexpr std::string_view CLI_FLAG_DUMP_ALL = "--dump-all";
constexpr std::string_view CLI_FLAG_HELP_SHORT = "-h";
constexpr std::string_view CLI_FLAG_HELP = "--help";

constexpr std::string_view CLI_FLAG_AST_DUMP = "--ast-dump";
constexpr std::string_view CLI_FLAG_EMIT_LLVM = "--emit-llvm";
constexpr std::string_view CLI_FLAG_COMPILE_ONLY = "--compile-only";
constexpr std::string_view CLI_FLAG_BINARY = "--binary";

struct CliOptions
{
    std::string_view file_path_{};
    bool dump_ast_{false};
    bool emit_llvm_{false};
    bool emit_obj_{false};
    bool emit_binary_{false};
    bool show_help_{false};
    std::string output_name_{};
};

void print_usage(std::string_view _program_name)
{
    std::cerr << "Usage: " << _program_name << " " << CLI_FLAG_PATH_SHORT << " <source_file> [options]\n\n"
              << "General Options (non-chaining):\n"
              << "  " << CLI_FLAG_PATH_SHORT << ", " << CLI_FLAG_PATH_LONG << " <file>                 Input source file (required)\n"
              << "  " << CLI_FLAG_OUTPUT_SHORT << ", " << CLI_FLAG_OUTPUT << " <name>                Output base name (default: input file stem)\n"
              << "  " << CLI_FLAG_DUMP_ALL_SHORT << ", " << CLI_FLAG_DUMP_ALL << "                 Dump all outputs (-a, -l, -c, -b)\n"
              << "  " << CLI_FLAG_HELP_SHORT << ", " << CLI_FLAG_HELP << "                       Display this help message\n\n"
              << "Emission Toggles (chainable, e.g. -clb, default: -b):\n"
              << "  -a, " << CLI_FLAG_AST_DUMP << "                    Print AST after parsing\n"
              << "  -l, " << CLI_FLAG_EMIT_LLVM << "                 Emit LLVM IR files (.ll, raw and optimized)\n"
              << "  -c, " << CLI_FLAG_COMPILE_ONLY << "                Emit object file (.o)\n"
              << "  -b, " << CLI_FLAG_BINARY << "                      Emit linked binary executable\n";
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
            if (!options.file_path_.empty())
            {
                std::cerr << "Error: multiple input files not supported ('" << options.file_path_ << "' and '" << _argv[i] << "')\n";
                return std::nullopt;
            }
            options.file_path_ = _argv[i];
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
        else if (arg == CLI_FLAG_DUMP_ALL || arg == CLI_FLAG_DUMP_ALL_SHORT)
        {
            options.dump_ast_ = true;
            options.emit_llvm_ = true;
            options.emit_obj_ = true;
            options.emit_binary_ = true;
        }
        else if (arg == CLI_FLAG_AST_DUMP)
        {
            options.dump_ast_ = true;
        }
        else if (arg == CLI_FLAG_EMIT_LLVM)
        {
            options.emit_llvm_ = true;
        }
        else if (arg == CLI_FLAG_COMPILE_ONLY)
        {
            options.emit_obj_ = true;
        }
        else if (arg == CLI_FLAG_BINARY)
        {
            options.emit_binary_ = true;
        }
        else if (arg.starts_with("-") && !arg.starts_with("--"))
        {
            if (arg.size() == 1)
            {
                std::cerr << "Unknown option: -\n";
                return std::nullopt;
            }

            if (arg.find('d') != std::string_view::npos && arg.size() > 2)
            {
                std::cerr << "Error: -d cannot be chained with other options\n";
                return std::nullopt;
            }

            for (size_t j = 1; j < arg.size(); ++j)
            {
                const char ch = arg[j];
                switch (ch)
                {
                case 'h':
                    options.show_help_ = true;
                    return options;
                case 'a':
                    options.dump_ast_ = true;
                    break;
                case 'l':
                    options.emit_llvm_ = true;
                    break;
                case 'c':
                    options.emit_obj_ = true;
                    break;
                case 'b':
                    options.emit_binary_ = true;
                    break;
                case 'p':
                case 'o':
                    std::cerr << "Error: -" << ch << " requires argument and cannot be chained\n";
                    return std::nullopt;
                default:
                    std::cerr << "Unknown option: -" << ch << "\n";
                    return std::nullopt;
                }
            }
        }
        else if (!arg.starts_with("-"))
        {
            std::cerr << "Error: positional argument '" << arg << "' not supported, use "
                      << CLI_FLAG_PATH_SHORT << " or " << CLI_FLAG_PATH_LONG << " <file>\n";
            return std::nullopt;
        }
        else
        {
            std::cerr << "Unknown option: " << arg << "\n";
            return std::nullopt;
        }
    }

    if (options.file_path_.empty())
    {
        std::cerr << "Error: input source file required (" << CLI_FLAG_PATH_SHORT << ", " << CLI_FLAG_PATH_LONG << " <file>)\n";
        print_usage(_argv[0]);
        return std::nullopt;
    }

    if (options.output_name_.empty())
    {
        options.output_name_ = std::filesystem::path(options.file_path_).stem().string();
    }

    const bool any_emission_flag = options.dump_ast_ || options.emit_llvm_ || options.emit_obj_ || options.emit_binary_;
    if (!any_emission_flag)
    {
        options.emit_binary_ = true;
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

    if (!cli->emit_llvm_ && !cli->emit_obj_ && !cli->emit_binary_)
    {
        return EXIT_CODE_SUCCESS;
    }

    fela::CodeGen ir_representation;
    ir_representation.generate_node(program.get());
    const std::string raw_ir_path = fmt::format("{}_raw.ll", cli->output_name_);
    const std::string ir_path = fmt::format("{}.ll", cli->output_name_);
    const std::string obj_path = fmt::format("{}.o", cli->output_name_);
    const std::string binary_path = cli->output_name_;

    if (cli->emit_llvm_)
    {
        if (!ir_representation.save_ir_to_file(raw_ir_path))
        {
            std::cerr << "Error saving raw IR to file: " << raw_ir_path << "\n";
            return EXIT_CODE_FAILURE;
        }
        std::cout << "Generated raw IR file: " << raw_ir_path << "\n";
    }

    fela::backend::optimize_module(ir_representation.get_module());

    if (cli->emit_llvm_)
    {
        if (!ir_representation.save_ir_to_file(ir_path))
        {
            std::cerr << "Error saving optimized IR to file: " << ir_path << "\n";
            return EXIT_CODE_FAILURE;
        }
        std::cout << "Generated optimized IR file: " << ir_path << "\n";
    }

    if (!cli->emit_obj_ && !cli->emit_binary_)
    {
        return EXIT_CODE_SUCCESS;
    }

    if (!fela::backend::emit_object_file(ir_representation.get_module(), obj_path))
    {
        std::cerr << "Error emitting object file: " << obj_path << "\n";
        return EXIT_CODE_FAILURE;
    }
    if (cli->emit_obj_)
    {
        std::cout << "Generated object file: " << obj_path << "\n";
    }

    if (!cli->emit_binary_)
    {
        return EXIT_CODE_SUCCESS;
    }

    if (!fela::backend::link_c_runtime_library(obj_path, binary_path))
    {
        std::cerr << "Error linking binary: " << binary_path << "\n";
        return EXIT_CODE_FAILURE;
    }
    std::cout << "Generated binary: " << binary_path << "\n";

    if (!cli->emit_obj_)
    {
        std::filesystem::remove(obj_path);
    }

    return EXIT_CODE_SUCCESS;
}
