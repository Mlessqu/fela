#pragma once
#include <string>
#include <variant>
#include <vector>

namespace fela
{
    //need data structure where we keep info of symbols read by parser
    //then semantic checker can construct nodes from this data if valid of course

    enum class SymbolType
    {
        variable,
        function
    };
    enum class DataType
    {
        int_type,
        bool_type,
        void_type //<- for return types only not variables
    };
    struct VariableSymbol
    {
        DataType type_;
    };
    struct FunctionSymbol
    {
        DataType return_type_;
        std::vector<DataType> param_types_;
    };
    struct Symbol
    {
        std::string identifier_;
        SymbolType symbol_type_;
        std::variant<VariableSymbol,FunctionSymbol> symbol_signature_;
    };

} // fela
