#ifndef _PRODUCTION_RULE_H_
#define _PRODUCTION_RULE_H_

#include "grammarSymbol.h"
#include <vector>

class ProductionRule
{
public:
    explicit ProductionRule(const GrammarSymbol& left, const std::vector<GrammarSymbol>& right);
    ~ProductionRule() = default;

    std::string toString() const;

    GrammarSymbol getLeft() const;
    const std::vector<GrammarSymbol>& getRight() const;

private:
    GrammarSymbol left_;
    std::vector<GrammarSymbol> right_;
};

#endif // _PRODUCTION_RULE_H_