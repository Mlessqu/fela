
#include"Lexer.h++"
#include "Parser.h++"
#include "SemanticChecker.h++"


int main(int _args, char** _arg_vals)
{
    fela::Lexer lexer;
    lexer.load_from_file("fela.txt");
    auto tokens = lexer.tokenize();
    fela::Parser parser(tokens);
    auto program = parser.parse_program();
    fela::SemanticChecker sema;
    sema.check(program.get());
    return 0;
}
