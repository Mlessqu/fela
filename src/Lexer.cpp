#include "Lexer.h++"

#include <filesystem>
#include <fstream>

namespace fela
{
    bool Lexer::load_file(std::string _file_path)
    {
        std::ifstream file(_file_path, std::ios::binary);
        if (!file.is_open()) return false;
        auto file_size = std::filesystem::file_size(_file_path);
        source_code_.resize(file_size);
        if (!file.read(source_code_.data(),file_size)) return false;
        source_code_.push_back('\0');
        cursor_= source_code_.data();
        line_start_ = cursor_;
        line_=1;

        return true;
    }


    //
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
        }

        if (small_let_min_bound && small_let_max_bound) return true;
        if (capital_let_min_bound && capital_let_max_bound) return true;
        return false;
    }


    bool Lexer::is_alpha_num(char _c)
    {
        if (is_digit(_c) || is_letter(_c)) return true;
        return false;
    }


    Token Lexer::next_token()
    {
        Token token;
        //skip whitespaces and comments
        //TODO: skip comments
        while (*cursor_ == ' ' || *cursor_ == '\t' || *cursor_== '\r' || *cursor_ == '\n')
        {
            if (*cursor_ == '\n') line_++;
            cursor_++;
        }

        const char* tok_start = cursor_;
        switch(*cursor_)
        {
        case ';':
            {
                cursor_++;
                return Token{ .type_ = TokenType::semi,line_, 0, ""};
            }
        }


        return Token{};
    }


    //implementation details here
} // fela
