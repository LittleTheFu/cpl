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
    explicit Grammar(std::vector<ProductionRule>&& rules, const GrammarSymbol& startSymbol);
    ~Grammar() = default;

    const ProductionRule &getArgumentedRule() const;
    const std::vector<ProductionRule>& getRules() const;
    const GrammarSymbol& getEndSymbol() const;

    const std::set<GrammarSymbol>& getNonTerminalSymbols() const;
    const std::set<GrammarSymbol>& getTerminalSymbols() const;
    const std::set<ProductionRule> getProductionRules(const GrammarSymbol& symbol) const;

    void setNonTerminalSymbols(const std::set<GrammarSymbol>& nonTerminalSymbols);
    void setTerminalSymbols(const std::set<GrammarSymbol>& terminalSymbols);

    std::string toString() const;

    using FirstSet = std::set<GrammarSymbol>;
    using FollowSet = std::set<GrammarSymbol>;

    const std::map<GrammarSymbol, FirstSet>& getCalculatedFirstSets() const;
    const std::map<GrammarSymbol, FollowSet>& getCalculatedFollowSets() const; 



    void calculateFirstSets();
    void calculateFollowSets();

    bool updateFollowSetFromRule(const ProductionRule& rule);

    bool canDeriveEmpty(const GrammarSymbol& symbol) const;
    bool canDeriveEmpty(const std::vector<GrammarSymbol>& symbols) const;

    std::set<GrammarSymbol> getFirstSet(const std::vector<GrammarSymbol> &symbols, bool &allCanDeriveEmpty) const;

private:
    std::vector<ProductionRule> rules_;
    GrammarSymbol startSymbol_;
    GrammarSymbol endSymbol_;

    GrammarSymbol argumentedStartSymbol_;
    ProductionRule argumentedRule_;

    std::set<GrammarSymbol> nonTerminalSymbols_;
    std::set<GrammarSymbol> terminalSymbols_;

    //-------------------------------------------------------
    std::map<GrammarSymbol, FirstSet> firstSets_;
    std::map<GrammarSymbol, FollowSet> followSets_;
};



#endif // _GRAMMAR_H_