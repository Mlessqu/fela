#pragma once
#include <string>
#include <string_view>

namespace fela
{
    enum class TokenType
    {
        //general
        semi,
        coma,
        eof,
        //literals
        integer_literal,
        boolean_literal,
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

    };
    struct Token
    {
        TokenType type_;
        unsigned int line_;
        unsigned int n_;
        std::string_view payload_;
    };
    //TODO: the idea is lexer loads whole thing at once into memory then we traverse with std::string_view
    //1. Load file
    //2. next_token() - advances pointer, reads next token, ignores whitespaces and comments etc.
    class Lexer
    {
    public:
        bool load_file(std::string _file_path);
        Token next_token();
    private:
        char advance(); //consume char
        char peek(); // look at current poinitng char without consuming
        char peek_next(); //look at current+ 1 char without consuming
        size_t pointer_=0;
        std::string source_code_;
    };
} // fela
