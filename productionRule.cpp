#include "productionRule.h"

ProductionRule::ProductionRule(const GrammarSymbol &left, const std::vector<GrammarSymbol> &right)
    : left_(left), right_(right)
{
}

std::string ProductionRule::toString() const
{
    std::string result = left_.getName() + " -> ";

    if (right_.empty())
    {
        result += "ε";
    }
    else
    {
        for (const auto& symbol : right_)
        {
            result += symbol.getName() + " ";
        }
    }

    return result;
}

GrammarSymbol ProductionRule::getLeft() const
{
    return left_;
}

const std::vector<GrammarSymbol>& ProductionRule::getRight() const
{
    return right_;
}
