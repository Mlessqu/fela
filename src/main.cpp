#include <iostream>

#include"Lexer.h++"
#include "Parser.h++"
#include "SemanticChecker.h++"


int main(int _args, char** _arg_vals)
{
    fela::Lexer lexer;
    lexer.load_file("fela.txt");
    auto tokens = lexer.tokenize();
    fela::SemanticChecker sema;
    fela::Parser parser(tokens,sema);
    return 0;
}
