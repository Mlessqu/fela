#pragma once
#include <string>
#include <variant>
#include <vector>

namespace fela
{
    //need data structure where we keep info of symbols read by parser
    //then semantic checker can construct nodes from this data if valid of course

    enum class DataType
    {
        int_type,
        bool_type,
        void_type //<- for return types only not variables
    };
    struct VariableSymbol
    {
        std::string name_;
        DataType type_;
    };
    struct FunctionSymbol
    {
        std::string name_;
        DataType return_type_;
        std::vector<DataType> param_types_;
    };
    struct Symbol
    {
        std::variant<VariableSymbol, FunctionSymbol> symbol_signature_;
    };
    struct Scope
    {
        std::unordered_map<std::string, Symbol> symbols_;
    };
    struct ScopeStack
    {
        void push_scope();
        void pop_scope();
        bool insert_symbol(const std::string& _name, const Symbol& _symbol);
        const Symbol* lookup(const std::string& _name) const;
    private:
        std::vector<Scope> scopes_;
    };

} // fela
