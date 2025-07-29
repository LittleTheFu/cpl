#include "lrParserGenerator.h"
#include <queue>
#include <stdexcept>

LRParserGenerator::LRParserGenerator(const Grammar &grammar)
    : grammar_(grammar)
{
    buildDFA();
}

std::set<LRItem> LRParserGenerator::calculateClosure(const std::set<LRItem> &items)
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

    return LRState(calculateClosure(gotoCoreItems));
}

void LRParserGenerator::buildDFA()
{
    LRState startState = LRState(calculateClosure({LRItem(grammar_.getArgumentedRule(), 0, grammar_.getEndSymbol())}));
    int startStateId = getNextId();
    dfa_.at(startState) = startStateId;

    std::queue<LRState> q;
    q.push(startState);
    while (!q.empty())
    {
        LRState state = q.front();
        int stateId = dfa_.at(state);
        q.pop();

        for (const auto &symbol : grammar_.getAllSymbols())
        {
            if (symbol == grammar_.getArgumentedStartSymbol())
            {
                continue;
            }
            LRState nextState = calculateNextState(state, symbol);
            if (nextState.isEmpty())
            {
                continue;
            }
            if (dfa_.find(nextState) == dfa_.end())
            {
                dfa_.at(nextState) = getNextId();
                q.push(nextState);
            }

            if (symbol.getType() == SymbolType::NonTerminal)
            {
                gotoTable_[stateId][symbol] = dfa_.at(nextState);
            }
        }
    }

    for (const auto &pair : dfa_)
    {
        LRState state = pair.first;
        int stateId = pair.second;
        for (const auto &symbol : grammar_.getTerminalSymbols())
        {
            LRState nextState = calculateNextState(state, symbol);
            if (nextState.isEmpty())
            {
                continue;
            }

            actionTable_[stateId][symbol] = Action{ActionType::Shift, dfa_.at(nextState), 0};
        }
    }

    std::set<GrammarSymbol> allLookaheadSymbols = grammar_.getTerminalSymbols();
    allLookaheadSymbols.insert(grammar_.getEndSymbol());

    for (const auto &pair : dfa_)
    {
        const LRState &state = pair.first;
        int stateId = pair.second;
        for (const auto &symbol : allLookaheadSymbols)
        {
            std::set<LRItem> reduceItems;
            for (const auto &item : state.getItems())
            {
                if (item.isReduceItem(symbol))
                {
                    reduceItems.insert(item);
                }
            }

            if (reduceItems.empty())
            {
                continue;
            }

            if (reduceItems.size() > 1)
            {
                std::string errorMsg;
                for (const auto &item : reduceItems)
                {
                    errorMsg += item.toString() + "\n";
                }
                throw std::runtime_error("reduce conflict : " + errorMsg);
            }

            const LRItem &item = *reduceItems.begin();
            std::map<GrammarSymbol, Action> &actionMap = actionTable_[stateId];
            if (actionMap.count(symbol) == 0)
            {
                if (symbol.getType() == SymbolType::Terminal)
                {
                    actionMap[symbol] = Action{ActionType::Reduce, 0, grammar_.getRuleId(item.getRule())};
                }
                else if (symbol.getType() == SymbolType::End)
                {
                    if (item.getRule() == grammar_.getArgumentedRule())
                    {
                        actionMap[symbol] = Action{ActionType::Accept, 0, 0};
                    }
                    else
                    {
                        throw std::runtime_error("end symbol is not valid");
                    }
                }
                else
                {
                    throw std::runtime_error("symbol type is not valid");
                }
            }
            else
            {
                if (actionMap[symbol].type == ActionType::Shift)
                {
                    throw std::runtime_error("shift conflict with shift");
                }
                else if (actionMap[symbol].type == ActionType::Reduce)
                {
                    size_t id = actionMap[symbol].productionRuleId;
                    throw std::runtime_error("reduce conflict : " + std::to_string(id));
                }
            }
        }
    }
}

int LRParserGenerator::getNextId()
{
    static int id = 0;
    return id++;
}

const std::map<int, std::map<GrammarSymbol, int>> &LRParserGenerator::getGotoTable() const
{
    return gotoTable_;
}

const std::map<int, std::map<GrammarSymbol, Action>> &LRParserGenerator::getActionTable() const
{
    return actionTable_;
}

const std::map<LRState, int>& LRParserGenerator::getDFA() const
{
    return dfa_;
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
