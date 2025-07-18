#ifndef _NFA_STATE_FRAGMENT_H_
#define _NFA_STATE_FRAGMENT_H_

#include "nfaState.h"

struct NfaStateFragment
{
    std::shared_ptr<NfaState> startState_;
    std::shared_ptr<NfaState> endState_;
};

#endif // _NFA_STATE_FRAGMENT_H_