#pragma once
#include <string>
#include <string_view>
#include <utility>

namespace fela
{
    constexpr unsigned int DEFAULT_START_LINE = 1;
    constexpr char SOURCE_NULL_TERMINATOR = '\0';

    enum class TokenType
    {
        //general
        eof,
        semi,
        coma,
        //literals
        integer_literal,
        boolean_literal,
        false_boolean,
        true_boolean,
        //types
        type_int,
        type_bool,
        type_void,
        //keywords
        if_keyword,
        else_keyword,
        while_keyword,
        return_keyword,
        //operators
        assign,
        minus,
        plus,
        negation_op,
        //math
        divide_op,
        multiply_op,
        //comparative op
        equal_op,
        not_equal_op,
        greater_op,
        smaller_op,
        //boolean op
        and_op,
        or_op,
        //grouping operators
        open_scope,
        close_scope,
        open_group,
        close_group,
        //identifier
        identifier,
        //comments
        comment_line,
        comment_block,
        unknown
    };
    struct Token
    {
        TokenType type_;
        unsigned int line_;
        unsigned int column_;
        std::string_view payload_;
    };
    //TODO: the idea is lexer loads whole thing at once into memory then we traverse with std::string_view
    //1. Load file
    //2. next_token() - advances pointer, reads next token, ignores whitespaces and comments etc.
    class Lexer
    {
    public:
        bool load_file(std::string _file_path);
        void load_source(std::string _source)
        {
            source_code_ = std::move(_source);
            source_code_.push_back(SOURCE_NULL_TERMINATOR);
            cursor_ = source_code_.data();
            line_start_ = cursor_;
            line_ = DEFAULT_START_LINE;
        }
        Token next_token();
    private:

        bool is_digit(char _c);
        bool is_letter(char _c);
        bool is_alpha_num(char _c);
        TokenType look_up_keyword_or_identifier(std::string_view _payload);
        char* cursor_=nullptr;
        const char* line_start_=nullptr;
        unsigned int line_=0;
        std::string source_code_;
    };
} // fela
