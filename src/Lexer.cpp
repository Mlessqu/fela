#include "Lexer.h++"

namespace fela
{
    bool Lexer::load_file(std::string _file_path)
    {
        //check file size
        //preallocate string
        //load into std::string
        //append eof "\0"
        return true;
    }
    //3 cases
    // 2-char tokens == != && ||  // /*
    //
    //1 char token
    //variable_len tokens
    bool Lexer::is_digit(char _c)
    {
        const bool min_bound = _c >= '0';
        if (!min_bound) return false;
        const bool max_bound = _c <= '9';
        if (!max_bound) return false;
        return true;
    }


    bool Lexer::is_letter(char _c)
    {
        const bool small_let_min_bound = _c >= 'a';
        const bool small_let_max_bound = _c <= 'z';
        const bool capital_let_min_bound = _c >= 'A';
        const bool capital_let_max_bound = _c <= 'Z';
        const bool is_underscore = _c == '_';
        if (is_underscore)
        {
            return true;
        }else
        {
            if (small_let_min_bound && small_let_max_bound) return true;
            if (capital_let_min_bound && capital_let_max_bound) return true;
            return false;
        }
    }


    bool Lexer::is_alpha_num(char _c)
    {
        if (is_digit(_c)|| is_letter(_c)) return true;
        return false;
    }


    Token Lexer::next_token()
    {
    }



    char Lexer::advance()
    {
    }


    char Lexer::peek()
    {
    }


    char Lexer::peek_next()
    {
    }


    //implementation details here
} // fela