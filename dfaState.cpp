#include "dfaState.h"

DFAState::DFAState()
{
}

void DFAState::addTransition(const std::string &input, std::shared_ptr<DFAState> nextState)
{
    transitions_[input] = nextState;
}

std::shared_ptr<DFAState> DFAState::run(const std::string &input)
{
    auto it = transitions_.find(input);
    if (it == transitions_.end())
    {
        return nullptr;
    }

    auto nextState = it->second.lock();
    if (!nextState)
    {
        return nullptr;
    }
    return nextState;
}
