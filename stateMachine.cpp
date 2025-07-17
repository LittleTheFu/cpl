#include "stateMachine.h"

StateMachine::StateMachine(std::shared_ptr<State> initialState)
{
    currentState_ = initialState;
}

std::shared_ptr<State> StateMachine::run(const std::string &input)
{
    if (!currentState_)
    {
        return nullptr;
    }

    currentState_ = currentState_->run(input);
    return currentState_;
}


