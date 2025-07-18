#ifndef _NFA_STATE_FRAGMENT_H_
#define _NFA_STATE_FRAGMENT_H_

#include "nfaState.h"

class NfaStateFragment
{
public:
    NfaStateFragment(std::shared_ptr<NfaState> startState, std::shared_ptr<NfaState> endState);
    ~NfaStateFragment();

    std::shared_ptr<NfaState> getStartState() const;
    std::shared_ptr<NfaState> getEndState() const;

private:
    std::shared_ptr<NfaState> startState_;
    std::shared_ptr<NfaState> endState_;
};

#endif // _NFA_STATE_FRAGMENT_H_