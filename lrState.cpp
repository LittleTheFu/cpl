#include "lrState.h"

LRState::LRState(const std::set<LRItem> &items)
{
    items_ = items;
}

bool LRState::isEmpty() const
{
    return items_.empty();
}

std::set<LRItem> LRState::getReduceItems(const GrammarSymbol &nextSymbol) const
{
    std::set<LRItem> result;
    for (const auto &item : items_)
    {
        if (item.isReduceItem(nextSymbol))
        {
            result.insert(item);
        }
    }

    return result;
}


bool LRState::canShift() const
{
    for (const auto &item : items_)
    {
        if (item.isDotAtEnd())
        {
            return false;
        }
    }

    return true;
}

bool LRState::operator==(const LRState &other) const
{
    return items_ == other.items_;
}

bool LRState::operator!=(const LRState &other) const
{
    return !(*this == other);
}

bool LRState::operator<(const LRState &other) const
{
    return items_ < other.items_;
}

const std::set<LRItem>& LRState::getItems() const
{
    return items_;
}
