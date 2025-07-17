#include "state.h"

State::State()
{
}

void State::addTransition(const std::string &input, std::shared_ptr<State> nextState)
{
    transitions_[input] = nextState;
}

std::shared_ptr<State> State::run(const std::string &input)
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
