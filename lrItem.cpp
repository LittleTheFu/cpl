#include "lrItem.h"

LRItem::LRItem(const ProductionRule &rule, size_t dotPosition, const GrammarSymbol& lookAheadSymbol)
    : rule_(rule), dotPosition_(dotPosition), lookAheadSymbol_(lookAheadSymbol)
{
}

std::string LRItem::toString() const
{
    return rule_.toString() + " . " + std::to_string(dotPosition_);
}

bool LRItem::isDotAtEnd() const
{
    return dotPosition_ >= rule_.getRight().size();
}

bool LRItem::isReduceItem(const GrammarSymbol &nextSymbol) const
{
    if(!isDotAtEnd())
    {
        return false;
    }

    return lookAheadSymbol_ == nextSymbol;
}

ProductionRule LRItem::getRule() const
{
    return rule_;
}

size_t LRItem::getDotPosition() const
{
    return dotPosition_;
}

GrammarSymbol LRItem::getLookAheadSymbol() const
{
    return lookAheadSymbol_;
}

bool LRItem::operator==(const LRItem &other) const
{
    return rule_ == other.rule_ && dotPosition_ == other.dotPosition_;
}

bool LRItem::operator!=(const LRItem &other) const
{
    return !(*this == other);
}

bool LRItem::operator<(const LRItem &other) const
{
    if (rule_ != other.rule_)
    {
        return rule_ < other.rule_;
    }
    return dotPosition_ < other.dotPosition_;
}

std::optional<GrammarSymbol> LRItem::getSymbolAfterDot() const
{
    if (dotPosition_ >= rule_.getRight().size())
    {
        return std::nullopt;
    }

    return std::optional<GrammarSymbol>(rule_.getRight().at(dotPosition_));
}

std::vector<GrammarSymbol> LRItem::getBetaLookaheadSequence() const
{
    std::vector<GrammarSymbol> remainingSymbols;

    remainingSymbols.insert(remainingSymbols.end(), rule_.getRight().begin() + dotPosition_ + 1, rule_.getRight().end());
    remainingSymbols.push_back(lookAheadSymbol_);

    return remainingSymbols;
}
