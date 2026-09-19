#pragma once

namespace fela
{
    enum class TokenType
    {
        semi,
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
        assign_op,
        //unary
        minus_sign_op,
        plus_sign_op,
        negation_op,
        //math
        add_op,
        minus_op,
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
    };
    class Lexer
    {
    };
} // fela
