#include "nfaStateFragment.h"

NfaStateFragment::NfaStateFragment(std::shared_ptr<NFAState> startState, std::shared_ptr<NFAState> endState)
{
    startState_ = startState;
    endState_ = endState;
}

NfaStateFragment::~NfaStateFragment()
{
}

std::shared_ptr<NFAState> NfaStateFragment::getStartState() const
{
    return startState_;
}

std::shared_ptr<NFAState> NfaStateFragment::getEndState() const
{
    return endState_;
}