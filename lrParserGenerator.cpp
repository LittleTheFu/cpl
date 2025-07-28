#include "lrParserGenerator.h"
#include <queue>

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

std::set<LRItem> LRParserGenerator::calculateGoto(const LRState &state, const GrammarSymbol &inputSymbol)
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

    return caculateClosure(gotoCoreItems);
}
