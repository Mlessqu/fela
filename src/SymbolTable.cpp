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
