#ifndef __LR_ITEM_H__
#define __LR_ITEM_H__

#include "grammarSymbol.h"
#include "productionRule.h"
#include <optional>
#include <vector>

class LRItem
{
public:
    LRItem(const ProductionRule& rule, size_t dotPosition, const GrammarSymbol& lookAheadSymbol);
    ~LRItem() = default;

    std::string toString() const;
    bool isDotAtEnd() const;
    bool isReduceItem(const GrammarSymbol& nextSymbol) const;
    
    ProductionRule getRule() const;
    size_t getDotPosition() const;
    GrammarSymbol getLookAheadSymbol() const;
    std::optional<GrammarSymbol> getSymbolAfterDot() const;
    std::vector<GrammarSymbol> getBetaLookaheadSequence() const;

    bool operator==(const LRItem& other) const;
    bool operator!=(const LRItem& other) const;
    bool operator<(const LRItem& other) const;

private:
    ProductionRule rule_;
    size_t dotPosition_;
    GrammarSymbol lookAheadSymbol_;
};

#endif // __LR_ITEM_H__