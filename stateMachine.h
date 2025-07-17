#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

#include <memory>
#include <string>
#include "state.h"

class StateMachine
{
public:
    StateMachine(std::shared_ptr<State> initialState);

    std::shared_ptr<State> run(const std::string &input);
    
private:
    std::shared_ptr<State> currentState_;
};

#endif // _STATE_MACHINE_H_