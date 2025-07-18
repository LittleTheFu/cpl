#include "dfaStateMachine.h"

DFAStateMachine::DFAStateMachine(std::shared_ptr<DFAState> initialState)
{
    currentState_ = initialState;
}

std::shared_ptr<DFAState> DFAStateMachine::run(char input)
{
    if (!currentState_)
    {
        return nullptr;
    }

    currentState_ = currentState_->run(input);
    return currentState_;
}


