#ifndef _STACK_ITEM_H_
#define _STACK_ITEM_H_

#include "astNode.h"
#include "grammarSymbol.h"
#include <variant>
#include "token.h"
#include <memory>

struct StackItem
{
    int stateId;
    GrammarSymbol symbol;

    std::variant<std::unique_ptr<AstNode>, Token> value;

    StackItem(int stateId, GrammarSymbol symbol, Token token)
        : stateId(stateId), symbol(symbol), value(token)
    {
    }

    StackItem(int stateId, GrammarSymbol symbol, std::unique_ptr<AstNode>&& node)
        : stateId(stateId), symbol(symbol), value(std::move(node))
    {
    }

    StackItem(int stateId, GrammarSymbol symbol)
        : stateId(stateId), symbol(symbol), value()
    {
    }
};

#endif 
