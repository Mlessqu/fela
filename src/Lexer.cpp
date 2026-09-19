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
        if (!file.read(source_code_.data(), file_size)) return false;
        source_code_.push_back('\0');
        cursor_ = source_code_.data();
        line_start_ = cursor_;
        line_ = 1;

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


    TokenType Lexer::look_up_keyword_or_identifier(std::string_view _payload)
    {
        if (_payload == "true") return TokenType::true_boolean;
        if (_payload == "false") return TokenType::false_boolean;
        if (_payload == "int") return TokenType::type_int;
        if (_payload == "bool") return TokenType::type_bool;
        if (_payload == "void") return TokenType::type_void;
        if (_payload == "if") return TokenType::if_keyword;
        if (_payload == "else") return TokenType::else_keyword;
        if (_payload == "while") return TokenType::while_keyword;
        if (_payload == "return") return TokenType::return_keyword;
        return TokenType::identifier;
    }


    bool Lexer::is_alpha_num(char _c)
    {
        if (is_digit(_c) || is_letter(_c)) return true;
        return false;
    }


    Token Lexer::next_token()
    {
        //skip whitespaces

        while (*cursor_ == ' ' || *cursor_ == '\t' || *cursor_ == '\r' || *cursor_ == '\n')
        {
            if (*cursor_ == '\n')
            {
                line_++;
                line_start_ = cursor_ + 1;
            }
            cursor_++;
        }
        //skip comments
        //line comment
        if (*cursor_ == '/' && *(cursor_ + 1) == '/')
        {
            cursor_ += 2;
            while (*cursor_ != '\n' && *cursor_ != '\0') cursor_++;
            if (*cursor_ == '\n')
            {
                line_++;
                line_start_ = cursor_ + 1;
            }
            cursor_++;
        }
        if (*cursor_ == '/' && *(cursor_ + 1) == '*')
        {
            cursor_ += 2;
            while (*cursor_ != '\0')
            {
                if (*cursor_ == '\n')
                {
                    line_++;
                    line_start_ = cursor_ + 1;
                }
                if (*cursor_ == '*' && *(cursor_ + 1) == '/')
                {
                    cursor_ += 2;
                    break;
                }
                cursor_++;
            }
        }
        const char* tok_start = cursor_;
        unsigned int column = static_cast<unsigned int>(tok_start - line_start_) + 1;
        switch (*cursor_)
        {
        case '\0':
            {
                return Token{.type_ = TokenType::eof, line_, column, {tok_start, 0}};
            }
        case ';':
            {
                cursor_++;
                return Token{.type_ = TokenType::semi, line_, column, {tok_start, 1}};
            }
        case ',':
            {
                cursor_++;
                return Token{.type_ = TokenType::coma, line_, column, {tok_start, 1}};
            }
        case '=':
            {
                cursor_++;
                if (*cursor_ == '=')
                {
                    cursor_++;
                    return Token{.type_ = TokenType::equal_op, line_, column, {tok_start, 2}};
                }

                return Token{.type_ = TokenType::assign, line_, column, {tok_start, 1}};
            }
        case '+':
            {
                cursor_++;
                return Token{.type_ = TokenType::plus, line_, column, {tok_start, 1}};
            }
        case '-':
            {
                cursor_++;
                return Token{.type_ = TokenType::minus, line_, column, {tok_start, 1}};
            }
        case '!':
            {
                cursor_++;
                if (*cursor_ == '=')
                {
                    cursor_++;
                    return Token{.type_ = TokenType::not_equal_op, line_, column, {tok_start, 2}};
                }
                return Token{.type_ = TokenType::negation_op, line_, column, {tok_start, 1}};
            }
        case '/':
            {
                cursor_++;
                return Token{.type_ = TokenType::divide_op, line_, column, {tok_start, 1}};
            }
        case '*':
            {
                cursor_++;
                return Token{.type_ = TokenType::multiply_op, line_, column, {tok_start, 1}};
            }
        case '>':
            {
                cursor_++;
                return Token{.type_ = TokenType::greater_op, line_, column, {tok_start, 1}};
            }
        case '<':
            {
                cursor_++;
                return Token{.type_ = TokenType::smaller_op, line_, column, {tok_start, 1}};
            }
        case '(':
            {
                cursor_++;
                return Token{.type_ = TokenType::open_group, line_, column, {tok_start, 1}};
            }
        case ')':
            {
                cursor_++;
                return Token{.type_ = TokenType::close_group, line_, column, {tok_start, 1}};
            }
        case '{':
            {
                cursor_++;
                return Token{.type_ = TokenType::open_scope, line_, column, {tok_start, 1}};
            }
        case '}':
            {
                cursor_++;
                return Token{.type_ = TokenType::close_scope, line_, column, {tok_start, 1}};
            }
        case '&':
            {
                cursor_++;
                if (*cursor_ == '&')
                {
                    return Token{.type_ = TokenType::and_op, line_, column, {tok_start, 2}};
                }
                break;
            }
        case '|':
            {
                cursor_++;
                if (*cursor_ == '|')
                {
                    return Token{.type_ = TokenType::or_op, line_, column, {tok_start, 2}};
                }
                break;
            }
            cursor_++;
            return Token{.type_ = TokenType::unknown, line_, column, {tok_start, 1}};
        }


        //digits
        if (is_digit(*cursor_))
        {
            while (is_digit(*cursor_)) cursor_++;
            std::size_t len = cursor_ - tok_start;
            std::string_view payload{tok_start, len};
            Token token{.type_ = TokenType::integer_literal, line_, column, payload};
            return token;
        }
        if (is_letter(*cursor_))
        {
            while (is_alpha_num(*cursor_)) cursor_++;
            std::size_t len = cursor_ - tok_start;
            std::string_view payload{tok_start, len};
            TokenType token_type = look_up_keyword_or_identifier(payload);
            return Token{token_type, line_, column, payload};
        }
        //construct token, return token
        return Token{};
    }


    //implementation details here
} // fela
