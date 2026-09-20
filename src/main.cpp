#include <iostream>

#include"Lexer.h++"
int main(int _args, char** _arg_vals)
{
 fela::Lexer lexer;
    lexer.load_file("fela.txt");
    fela::Token current_token{.type_ = fela::TokenType::unknown,0,0,std::string_view{}};
    while (current_token.type_ != fela::TokenType::eof)
    {
        current_token = lexer.next_token();
        std::cout << current_token.payload_;
    }
    return 0;
}