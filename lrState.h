#ifndef _LR_STATE_H_
#define _LR_STATE_H_

#include "lrItem.h"
#include <set>

class LRState
{
public:
    LRState(const std::set<LRItem>& items);
    ~LRState() = default;

    bool isEmpty() const;
    std::set<LRItem> getReduceItems(const GrammarSymbol& nextSymbol) const;
    bool canShift() const;

    bool operator==(const LRState& other) const;
    bool operator!=(const LRState& other) const;
    bool operator<(const LRState& other) const;

    const std::set<LRItem>& getItems() const;

private:
    std::set<LRItem> items_;
};



#endif // _LR_STATE_H_