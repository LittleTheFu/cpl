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

size_t ProductionRule::getRightSize() const
{
    return right_.size();
}

bool ProductionRule::operator==(const ProductionRule &other) const
{
    return left_ == other.left_ && right_ == other.right_;
}

bool ProductionRule::operator!=(const ProductionRule &other) const
{
    return !(*this == other);
}

bool ProductionRule::operator<(const ProductionRule &other) const
{
    if (left_ != other.left_)
    {
        return left_ < other.left_;
    }
    return right_ < other.right_;
}
