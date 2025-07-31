#include "grammar.h"
#include <algorithm>
#include <stdexcept>
#include "stackItem.h"
#include "predefineSymbol.h"

Grammar::Grammar(std::vector<ProductionRule> &&rules, const GrammarSymbol &startSymbol)
    : rules_(std::move(rules)),
      startSymbol_(startSymbol),
      endSymbol_(PredefineSymbol::SYMBOL_END),
      argumentedStartSymbol_("S'", SymbolType::NonTerminal),
      argumentedRule_(argumentedStartSymbol_,
                      {startSymbol_},
                      [](std::vector<StackItem> &&stackItems) -> std::unique_ptr<AstNode>
                      {
                          if (stackItems.empty())
                          {
                              throw std::runtime_error("Augmented rule: no start symbol AST found.");
                          }

                          try
                          {
                              return std::move(std::get<std::unique_ptr<AstNode>>(stackItems[0].value));
                          }
                          catch (const std::bad_variant_access &e)
                          {
                              throw std::runtime_error("Augmented rule: Expected ASTNode pointer for start symbol, but got wrong type.");
                          }
                      })
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
            else if (symbol.getType() == SymbolType::End)
            {
                // terminalSymbols_.insert(symbol);
            }
            else
            {
                throw std::runtime_error("Unknown symbol type");
            }
        }
    }

    allSymbols_ = nonTerminalSymbols_;
    allSymbols_.insert(terminalSymbols_.begin(), terminalSymbols_.end());

    // nonTerminalSymbols_.insert(argumentedStartSymbol_);
    // terminalSymbols_.insert(endSymbol_);
}

const ProductionRule &Grammar::getArgumentedRule() const
{
    return argumentedRule_;
}

const std::vector<ProductionRule> &Grammar::getRules() const
{
    return rules_;
}

size_t Grammar::getRuleId(const ProductionRule &rule) const
{
    auto it = std::find(rules_.begin(), rules_.end(), rule);
    if (it == rules_.end())
    {
        throw std::runtime_error("Rule not found");
    }

    return it - rules_.begin();
}

const ProductionRule &Grammar::getRuleById(size_t id) const
{
    if (id >= rules_.size())
    {
        throw std::runtime_error("Rule id out of range");
    }

    return rules_[id];
}

const GrammarSymbol &Grammar::getArgumentedStartSymbol() const
{
    return argumentedStartSymbol_;
}

const GrammarSymbol &Grammar::getEndSymbol() const
{
    return endSymbol_;
}

const std::set<GrammarSymbol> &Grammar::getNonTerminalSymbols() const
{
    return nonTerminalSymbols_;
}

const std::set<GrammarSymbol> &Grammar::getTerminalSymbols() const
{
    return terminalSymbols_;
}

const std::set<GrammarSymbol> &Grammar::getAllSymbols() const
{
    return allSymbols_;
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

    firstSets_[endSymbol_] = {endSymbol_};

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

bool Grammar::updateFollowSetFromRule(const ProductionRule &rule)
{
    bool ruleChanged = false;

    const auto &left = rule.getLeft();
    const auto &right = rule.getRight();

    if (right.empty())
    {
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
