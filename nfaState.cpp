#include "nfaState.h"

NfaState::NfaState()
{

}

void NfaState::addTransition(char input, std::shared_ptr<NfaState> nextState)
{
    transitions_[input].insert(nextState);
}

void NfaState::addEpsilonTransition(std::shared_ptr<NfaState> nextState)
{
    epsilonTransitions_.insert(nextState);
}

const std::set<std::weak_ptr<NfaState>, CompareWeakNfaStatePtr> &NfaState::getEpsilonTransitions() const
{
    return epsilonTransitions_;
}

std::vector<std::shared_ptr<NfaState>> NfaState::run(char input)
{
    auto it = transitions_.find(input);
    if (it == transitions_.end())
    {
        return {};
    }
    
    std::vector<std::shared_ptr<NfaState>> result;
    for (auto &state : it->second)
    {
        if (auto lockedState = state.lock())
        {
            result.push_back(lockedState);
        }
    }

    return result;
}
