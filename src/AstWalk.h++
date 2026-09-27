#pragma once
#include <string>

namespace fela
{
    struct AstBase;

    std::string ast_dump_node(AstBase* _node);

} // fela
