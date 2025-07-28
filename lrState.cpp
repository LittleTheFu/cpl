#include "lrState.h"

LRState::LRState(const std::set<LRItem> &items)
{
    items_ = items;
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
