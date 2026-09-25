#include "SymbolTable.h++"

namespace fela
{
    //implementation details here
    void ScopeStack::push_scope()
    {
        scopes_.emplace_back();
    }


    void ScopeStack::pop_scope()
    {
        if (!scopes_.empty())
        {
            scopes_.pop_back();
        }
    }


    bool ScopeStack::insert_symbol(const std::string& _name, const Symbol& _symbol)
    {
        if (scopes_.empty()) return false;
        auto& current_scope = scopes_.back().symbols_;
        if (current_scope.contains(_name))
        {
            return false;
        }
        current_scope.emplace(_name,_symbol);
        return true;
    }


    bool ScopeStack::insert_var_symbol(std::string_view _name, DataType _type)
    {
        if (!lookup(std::string{_name}))
        {
            //TODO: error msg
            //symbol already exists!
            return false;
        }
        if (_type == DataType::void_type)
        {
            //TODO: error msg
            //var cannot be of type void!
            return false;
        }
        VariableSymbol var_symbol{std::string{_name},_type};
        if (!insert_symbol(var_symbol.name_,{var_symbol}))
        {
            return false;
        }
        return true;
    }


    bool ScopeStack::insert_func_symbol(std::string_view _name, DataType _type, const std::vector<VariableSymbol>& _params)
    {
        if (!lookup(std::string{_name}))
        {
            return false;
        }
        FunctionSymbol function_symbol(std::string{_name}, _type, _params);
        if (!insert_symbol(function_symbol.name_,{function_symbol}))
        {
            return false;
        }
        return true;
    }


    const Symbol* ScopeStack::lookup(const std::string& _name) const
    {
        //go over vector from r begin to r end,
        for (auto itr = this->scopes_.rbegin(); itr != scopes_.rend(); itr++) //outer
        {
            auto map_itr = itr->symbols_.find(_name);
            if (map_itr != itr->symbols_.end())
            {
                return &map_itr->second;
            }
        }
        return nullptr;
    }
} // fela
