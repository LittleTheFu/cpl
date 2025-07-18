#include "dfaState.h"

DFAState::DFAState(bool isEndState)
{
    isEndState_ = isEndState;
}

void DFAState::addTransition(char input, std::shared_ptr<DFAState> nextState)
{
    transitions_[input] = nextState;
}

std::shared_ptr<DFAState> DFAState::run(char input)
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

void DFAState::setEndState(bool isEndState)
{
    isEndState_ = isEndState;
}

bool DFAState::isEndState() const
{
    return isEndState_;
}
