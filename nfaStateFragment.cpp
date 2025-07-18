#include "nfaStateFragment.h"

NfaStateFragment::NfaStateFragment(std::shared_ptr<NfaState> startState, std::shared_ptr<NfaState> endState)
{
    startState_ = startState;
    endState_ = endState;
}

NfaStateFragment::~NfaStateFragment()
{
}

std::shared_ptr<NfaState> NfaStateFragment::getStartState() const
{
    return startState_;
}

std::shared_ptr<NfaState> NfaStateFragment::getEndState() const
{
    return endState_;
}