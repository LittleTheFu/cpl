#ifndef _GRAMMAR_H_
#define _GRAMMAR_H_

#include <vector>
#include "productionRule.h"
#include "grammarSymbol.h"
#include <set>
#include <map>

class Grammar
{
public:
    explicit Grammar(const std::vector<ProductionRule>& rules, const GrammarSymbol& startSymbol);
    ~Grammar() = default;

    const std::vector<ProductionRule>& getRules() const;

    const std::set<GrammarSymbol>& getNonTerminalSymbols() const;
    const std::set<GrammarSymbol>& getTerminalSymbols() const;

    void setNonTerminalSymbols(const std::set<GrammarSymbol>& nonTerminalSymbols);
    void setTerminalSymbols(const std::set<GrammarSymbol>& terminalSymbols);

    std::string toString() const;

    using FirstSet = std::set<GrammarSymbol>;
    using FollowSet = std::set<GrammarSymbol>;

    void calculateFirstSets();
    void calculateFollowSets();

    bool canDeriveEmpty(const GrammarSymbol& symbol) const;
    bool canDeriveEmpty(const std::vector<GrammarSymbol>& symbols) const;

private:
    std::vector<ProductionRule> rules_;
    GrammarSymbol startSymbol_;

    std::set<GrammarSymbol> nonTerminalSymbols_;
    std::set<GrammarSymbol> terminalSymbols_;

    //-------------------------------------------------------
    std::map<GrammarSymbol, FirstSet> firstSets_;
    std::map<GrammarSymbol, FollowSet> followSets_;
};



#endif // _GRAMMAR_H_