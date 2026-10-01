#pragma once
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace fela
{
    //need data structure where we keep info of symbols read by parser
    //then semantic checker can construct nodes from this data if valid of course

    enum class DataType
    {
        unresolved_type,
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
        std::vector<VariableSymbol> param_types_;
    };
    struct Symbol
    {
        std::variant<VariableSymbol, FunctionSymbol> symbol_signature_;
    };
    struct Scope
    {
        std::unordered_map<std::string, Symbol> symbols_;
    };
    constexpr std::string_view data_type_to_string_view(DataType _type)
    {
        switch (_type)
        {
        case DataType::int_type: return "int";
        case DataType::bool_type: return "bool";
        case DataType::void_type: return "void";
        case DataType::unresolved_type: return "undefined";
        }
        return "unknown type, consider updating converter function in symbol table.h";
    }
    struct ScopeStack
    {
        void push_scope();
        void pop_scope();
        [[nodiscard]] bool is_stack_empty() const;
        [[nodiscard]] bool insert_symbol(const std::string& _name, const Symbol& _symbol);
        bool insert_var_symbol(std::string_view _name, DataType _type);
        bool insert_func_symbol(std::string_view _name, DataType _type, const std::vector<VariableSymbol>& _params);
        const Symbol* lookup(const std::string& _name) const;
    private:

        std::vector<Scope> scopes_;
    };

} // fela
