#ifndef _NFA_STATE_FRAGMENT_H_
#define _NFA_STATE_FRAGMENT_H_

#include "nfaState.h"

class NfaStateFragment
{
public:
    NfaStateFragment(std::shared_ptr<NFAState> startState, std::shared_ptr<NFAState> endState);
    ~NfaStateFragment();

    std::shared_ptr<NFAState> getStartState() const;
    std::shared_ptr<NFAState> getEndState() const;

    void addState(std::shared_ptr<NFAState> state);
    void merge(std::shared_ptr<NfaStateFragment> otherFragment);

private:
    std::shared_ptr<NFAState> startState_;
    std::shared_ptr<NFAState> endState_;

    std::vector<std::shared_ptr<NFAState>> states_;
};

#endif // _NFA_STATE_FRAGMENT_H_