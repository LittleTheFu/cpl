#include "nfaState.h"

NfaState::NfaState()
{

}

void NfaState::addTransition(const std::string &input, std::shared_ptr<NfaState> nextState)
{
    transitions_[input].insert(nextState);
}

std::vector<std::shared_ptr<NfaState>> NfaState::run(const std::string &input)
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
