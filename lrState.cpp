#include "lrState.h"

LRState::LRState(const std::set<LRItem> &items)
{
    items_ = items;
}

const std::set<LRItem>& LRState::getItems() const
{
    return items_;
}
