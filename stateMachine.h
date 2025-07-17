#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

#include <memory>
#include <string>
#include "state.h"

class StateMachine
{
public:
    StateMachine(std::shared_ptr<DFAState> initialState);

    std::shared_ptr<DFAState> run(const std::string &input);
    
private:
    std::shared_ptr<DFAState> currentState_;
};

#endif // _STATE_MACHINE_H_