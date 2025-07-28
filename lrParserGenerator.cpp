#include "lrParserGenerator.h"
#include <queue>
#include <stdexcept>

LRParserGenerator::LRParserGenerator(const Grammar &grammar)
    : grammar_(grammar)
{
}

std::set<LRItem> LRParserGenerator::caculateClosure(const std::set<LRItem> &items)
{
    std::set<LRItem> closure = items;
    std::queue<LRItem> q;

    for (const auto &item : items)
    {
        q.push(item);
    }

    while (!q.empty())
    {
        LRItem item = q.front();
        q.pop();

        std::optional<GrammarSymbol> symbolAfterDot = item.getSymbolAfterDot();
        if (symbolAfterDot.has_value())
        {
            GrammarSymbol symbol = symbolAfterDot.value();
            bool canDeriveEmpty = false;
            std::set<GrammarSymbol> lookAheadSymbols = grammar_.getFirstSet(item.getBetaLookaheadSequence(), canDeriveEmpty);
            if (symbol.getType() == SymbolType::NonTerminal)
            {
                std::set<ProductionRule> productionRules = grammar_.getProductionRules(symbol);
                for (const auto &productionRule : productionRules)
                {
                    for (const auto &lookAheadSymbol : lookAheadSymbols)
                    {
                        SymbolType type = lookAheadSymbol.getType();
                        if (type == SymbolType::Terminal || type == SymbolType::End)
                        {
                            LRItem newItem = LRItem(productionRule, 0, lookAheadSymbol);
                            if (closure.insert(newItem).second)
                            {
                                q.push(newItem);
                            }
                        }
                    }
                }
            }
        }
    }

    return closure;
}

LRState LRParserGenerator::calculateNextState(const LRState &state, const GrammarSymbol &inputSymbol)
{
    std::set<LRItem> gotoCoreItems;

    for (const auto &lrItem : state.getItems())
    {
        std::optional<GrammarSymbol> symbolAfterDot = lrItem.getSymbolAfterDot();
        if (symbolAfterDot.has_value())
        {
            GrammarSymbol symbol = symbolAfterDot.value();
            if (symbol == inputSymbol)
            {
                LRItem newItem = LRItem(lrItem.getRule(), lrItem.getDotPosition() + 1, lrItem.getLookAheadSymbol());
                gotoCoreItems.insert(newItem);
            }
        }
    }

    return LRState(caculateClosure(gotoCoreItems));
}

void LRParserGenerator::buildDFA()
{
    LRState startState = LRState(caculateClosure({LRItem(grammar_.getArgumentedRule(), 0, grammar_.getEndSymbol())}));
    int startStateId = getNextId();
    dfa_[startState] = startStateId;

    std::queue<LRState> q;
    q.push(startState);
    while (!q.empty())
    {
        LRState state = q.front();
        int stateId = dfa_[state];
        q.pop();

        for (const auto &symbol : grammar_.getAllSymbols())
        {
            if (symbol == grammar_.getArgumentedStartSymbol())
            {
                continue;
            }
            LRState gotoState = calculateNextState(state, symbol);
            if (gotoState.isEmpty())
            {
                continue;
            }
            if (dfa_.find(gotoState) == dfa_.end())
            {
                dfa_[gotoState] = getNextId();
                q.push(gotoState);
            }

            if (symbol.getType() == SymbolType::NonTerminal)
            {
                gotoTable_[stateId][symbol] = dfa_[gotoState];
            }
        }
    }
}

int LRParserGenerator::getNextId()
{
    static int id = 0;
    return id++;
}

// std::optional<LRState> LRParserGenerator::getState(int id) const
// {
//     for (const auto &pair : dfa_)
//     {
//         if (pair.second == id)
//         {
//             return pair.first;
//         }
//     }

//     return std::nullopt;
// }
