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

void NfaStateFragment::addState(std::shared_ptr<NFAState> state)
{
    if(state)
    {
        states_.push_back(state);
    }
}

void NfaStateFragment::merge(std::shared_ptr<NfaStateFragment> otherFragment)
{
    if(otherFragment)
    {
        states_.insert(states_.end(), otherFragment->states_.begin(), otherFragment->states_.end());
    }
}

