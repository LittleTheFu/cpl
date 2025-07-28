#include "grammar.h"
#include <algorithm>

Grammar::Grammar(std::vector<ProductionRule>&& rules, const GrammarSymbol &startSymbol)
    : rules_(std::move(rules)),
      startSymbol_(startSymbol),
      argumentedStartSymbol_("S'", SymbolType::NonTerminal),
      argumentedRule_(argumentedStartSymbol_, {startSymbol_})
{
    rules_.push_back(argumentedRule_);
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

const ProductionRule &Grammar::getArgumentedRule() const
{
    return argumentedRule_;
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

const std::set<ProductionRule> Grammar::getProductionRules(const GrammarSymbol &symbol) const
{
    std::set<ProductionRule> result;
    for (const auto &rule : rules_)
    {
        if (rule.getLeft() == symbol)
        {
            result.insert(rule);
        }
    }
    
    return result;
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

const std::map<GrammarSymbol, Grammar::FirstSet> &Grammar::getCalculatedFirstSets() const
{
    return firstSets_;
}

const std::map<GrammarSymbol, Grammar::FollowSet> &Grammar::getCalculatedFollowSets() const
{
    return followSets_;
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
        std::map<GrammarSymbol, FirstSet> prevFirstSets = firstSets_;
        
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
                const auto &first = prevFirstSets[right[i]];

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

bool Grammar::updateFollowSetFromRule(const ProductionRule& rule)
{
    bool ruleChanged = false;

    const auto &left = rule.getLeft();
    const auto &right = rule.getRight();

    if (right.empty()) {
        return false;
    }

    for (size_t i = 0; i < right.size(); i++)
    {
        const auto &currentSymbolInRight = right[i];

        if (currentSymbolInRight.getType() == SymbolType::NonTerminal)
        {
            std::vector<GrammarSymbol> betaSymbols;
            for (size_t j = i + 1; j < right.size(); j++)
            {
                betaSymbols.push_back(right[j]);
            }

            bool betaCanDeriveEmpty = false;
            std::set<GrammarSymbol> firstOfBeta = getFirstSet(betaSymbols, betaCanDeriveEmpty);

            for (const auto &firstSymbol : firstOfBeta)
            {
                if (firstSymbol != GrammarSymbol::getEpsilonSymbol())
                {
                    if (followSets_[currentSymbolInRight].insert(firstSymbol).second)
                    {
                        ruleChanged = true;
                    }
                }
            }

            if (betaCanDeriveEmpty)
            {
                for (const auto &followSymbolFromLeft : followSets_[left])
                {
                    if (followSets_[currentSymbolInRight].insert(followSymbolFromLeft).second)
                    {
                        ruleChanged = true;
                    }
                }
            }
        }
    }
    return ruleChanged;
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
        changed = false;

        for (const auto &rule : rules_)
        {
            if (updateFollowSetFromRule(rule))
            {
                changed = true;
            }
        }
    }
}

bool Grammar::canDeriveEmpty(const GrammarSymbol &symbol) const
{
    auto &first = firstSets_.at(symbol);
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
