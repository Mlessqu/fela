
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>

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

struct CliOptions
{
    std::string_view file_path_{};
    bool dump_ast_{false};
    std::string_view output_file_{"a.out"};
};

void print_usage(std::string_view _program_name)
{
    std::cerr << "Usage: " << _program_name << " [options]\n"
              << "Options:\n"
              << "  " << CLI_FLAG_PATH_SHORT << ", " << CLI_FLAG_PATH_LONG << " <file>    Source file path\n"
              << "  " << CLI_FLAG_OUTPUT_SHORT << ", " << CLI_FLAG_OUTPUT << " <file>   Output file path (default: a.out)\n"
              << "      " << CLI_FLAG_AST_DUMP << "       Print AST after parsing\n";
}

std::optional<CliOptions> parse_args(int _argc, char** _argv)
{
    CliOptions options;

    for (int i = CLI_FIRST_ARG_INDEX; i < _argc; ++i)
    {
        const std::string_view arg{_argv[i]};

        if (arg == CLI_FLAG_PATH_SHORT || arg == CLI_FLAG_PATH_LONG)
        {
            if (++i >= _argc)
            {
                std::cerr << "Error: " << arg << " requires argument\n";
                return std::nullopt;
            }
            options.file_path_ = _argv[i];
        }
        else if (arg == CLI_FLAG_AST_DUMP|| arg == CLI_FLAG_AST_DUMP_SHORT)
        {
            options.dump_ast_ = true;
        }
        else if (arg == CLI_FLAG_OUTPUT || arg == CLI_FLAG_OUTPUT_SHORT)
        {
            if (++i >= _argc)
            {
                std::cerr << "Error: " << arg << " requires argument\n";
                return std::nullopt;
            }
            options.output_file_ = _argv[i];
        }
        else
        {
            std::cerr << "Unknown option: " << arg << "\n";
            return std::nullopt;
        }
    }

    if (options.file_path_.empty())
    {
        std::cerr << "Error: input file path required\n";
        return std::nullopt;
    }

    return options;
}

int main(int _argc, char** _argv)
{
    const auto cli = parse_args(_argc, _argv);
    if (!cli)
    {
        print_usage(_argv[0]);
        return EXIT_CODE_FAILURE;
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

    if (is_semantic_passed)
    {
        std::cout << "Semantic passed\n";
    }
    fela::CodeGen ir_representation;
    ir_representation.generate_node(program.get());
    if (!ir_representation.save_ir_to_file("mleko.ll"))
    {
        std::cout << "Error saving to file";
        return EXIT_CODE_FAILURE;
    }
    std::cout << "Generated ir file";
    fela::backend::emit_object_file(ir_representation.get_module(),"./");
    return EXIT_CODE_SUCCESS;
}
