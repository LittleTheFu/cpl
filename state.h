#ifndef _STATE_H_
#define _STATE_H_

#include <memory>
#include <unordered_map>
#include <string>

class State
{
public:
    State();

    void addTransition(const std::string &input, std::shared_ptr<State> nextState);
    std::shared_ptr<State> run(const std::string &input);

private:
    std::unordered_map<std::string, std::weak_ptr<State>> states_;
};


#endif // _STATE_H_