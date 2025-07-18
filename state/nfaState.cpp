#include "nfaState.h"

NFAState::NFAState()
{

}

void NFAState::addTransition(char input, std::shared_ptr<NFAState> nextState)
{
    transitions_[input].insert(nextState);
}

void NFAState::addEpsilonTransition(std::shared_ptr<NFAState> nextState)
{
    epsilonTransitions_.insert(nextState);
}

const std::set<std::weak_ptr<NFAState>, CompareWeakNfaStatePtr> &NFAState::getEpsilonTransitions() const
{
    return epsilonTransitions_;
}

std::set<std::shared_ptr<NFAState>, CompareNfaStateSharedPtr> NFAState::run(char input) const
{
    auto it = transitions_.find(input);
    if (it == transitions_.end())
    {
        return {};
    }
    
    std::set<std::shared_ptr<NFAState>, CompareNfaStateSharedPtr> result;
    for (auto &state : it->second)
    {
        if (auto lockedState = state.lock())
        {
            result.insert(lockedState);
        }
    }

    return result;
}
