#include "stateMachine.h"

StateMachine::StateMachine(std::shared_ptr<DFAState> initialState)
{
    currentState_ = initialState;
}

std::shared_ptr<DFAState> StateMachine::run(const std::string &input)
{
    if (!currentState_)
    {
        return nullptr;
    }

    currentState_ = currentState_->run(input);
    return currentState_;
}


