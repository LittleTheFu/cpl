#include "grammar.h"
#include <algorithm>

Grammar::Grammar(const std::vector<ProductionRule> &rules, const GrammarSymbol &startSymbol)
{
    rules_ = rules;
    startSymbol_ = startSymbol;

    for (const auto &rule : rules_)
    {
        nonTerminalSymbols_.insert(rule.getLeft());
        for (const auto &symbol : rule.getRight())
        {
            if (symbol.getType() == SymbolType::Terminal)
            {
                terminalSymbols_.insert(symbol);
            }
            else if (symbol.getType() == SymbolType::NonTerminal)
            {
                nonTerminalSymbols_.insert(symbol);
            }
        }
    }
}

const std::vector<ProductionRule> &Grammar::getRules() const
{
    return rules_;
}

const std::set<GrammarSymbol> &Grammar::getNonTerminalSymbols() const
{
    return nonTerminalSymbols_;
}

const std::set<GrammarSymbol> &Grammar::getTerminalSymbols() const
{
    return terminalSymbols_;
}

void Grammar::setNonTerminalSymbols(const std::set<GrammarSymbol> &nonTerminalSymbols)
{
    nonTerminalSymbols_ = nonTerminalSymbols;
}

void Grammar::setTerminalSymbols(const std::set<GrammarSymbol> &terminalSymbols)
{
    terminalSymbols_ = terminalSymbols;
}

std::string Grammar::toString() const
{
    std::string result;
    for (const auto &rule : rules_)
    {
        result += rule.toString() + "\n";
    }
    return result;
}

void Grammar::calculateFirstSets()
{
    for (const auto &symbol : terminalSymbols_)
    {
        firstSets_[symbol] = {symbol};
    }

    for (const auto &symbol : nonTerminalSymbols_)
    {
        firstSets_[symbol] = {};
    }

    for (const auto &rule : rules_)
    {
        const auto &left = rule.getLeft();
        const auto &right = rule.getRight();

        if (right.empty())
        {
            firstSets_[left].insert(GrammarSymbol::getEpsilonSymbol());
        }
    }

    bool changed = true;
    while (changed)
    {
        changed = false;
        for (const auto &rule : rules_)
        {
            const auto &left = rule.getLeft();
            const auto &right = rule.getRight();

            if (right.empty())
            {
                continue;
            }

            bool allCanDeriveEmpty = true;
            for (size_t i = 0; i < right.size(); i++)
            {
                const auto &first = firstSets_[right[i]];

                for (const auto &symbol : first)
                {
                    if (symbol != GrammarSymbol::getEpsilonSymbol())
                    {
                        if (firstSets_[left].insert(symbol).second)
                        {
                            changed = true;
                        }
                    }
                }

                if (first.count(GrammarSymbol::getEpsilonSymbol()) == 0)
                {
                    allCanDeriveEmpty = false;
                    break;
                }
            }

            if (allCanDeriveEmpty)
            {
                firstSets_[left].insert(GrammarSymbol::getEpsilonSymbol());
            }
        }
    }
}

void Grammar::calculateFollowSets()
{
    for (const auto &symbol : nonTerminalSymbols_)
    {
        followSets_[symbol] = {};
    }
    followSets_[startSymbol_].insert(GrammarSymbol::getEndSymbol());
    bool changed = true;

    while (changed)
    {
        for (const auto &rule : rules_)
        {
            const auto &left = rule.getLeft();
            const auto &right = rule.getRight();

            for (size_t i = 0; i < right.size(); i++)
            {
                const auto &symbol = right[i];
                if (symbol.getType() == SymbolType::NonTerminal)
                {
                    bool allCanDeriveEmpty = true;
                    std::vector<GrammarSymbol> symbols;

                    for (size_t j = i + 1; j < right.size(); j++)
                    {
                        symbols.push_back(right[j]);
                    }

                    std::set<GrammarSymbol> first = getFirstSet(symbols, allCanDeriveEmpty);
                    for (const auto &firstSymbol : first)
                    {
                        if (firstSymbol != GrammarSymbol::getEpsilonSymbol())
                        {
                            if (followSets_[symbol].insert(firstSymbol).second)
                            {
                                changed = true;
                            }
                        }
                    }

                    if (allCanDeriveEmpty)
                    {
                        for (const auto &followSymbol : followSets_[left])
                        {
                            if (followSets_[symbol].insert(followSymbol).second)
                            {
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
}

bool Grammar::canDeriveEmpty(const GrammarSymbol &symbol) const
{
    const auto &first = firstSets_[symbol];
    return first.count(GrammarSymbol::getEpsilonSymbol()) > 0;
}

bool Grammar::canDeriveEmpty(const std::vector<GrammarSymbol> &symbols) const
{
    if (symbols.empty())
    {
        return true;
    }

    for (const auto &symbol : symbols)
    {
        if (!canDeriveEmpty(symbol))
        {
            return false;
        }
    }

    return true;
}

std::set<GrammarSymbol> Grammar::getFirstSet(const std::vector<GrammarSymbol> &symbols, bool &allCanDeriveEmpty) const
{
    std::set<GrammarSymbol> firstSet = {};
    allCanDeriveEmpty = true;

    if (symbols.empty())
    {
        return firstSet;
    }

    for (const auto &symbol : symbols)
    {
        const auto &first = firstSets_.at(symbol);

        for (const auto &symbol : first)
        {
            firstSet.insert(symbol);
        }

        if (first.count(GrammarSymbol::getEpsilonSymbol()) == 0)
        {
            allCanDeriveEmpty = false;
            break;
        }
    }

    return firstSet;
}
