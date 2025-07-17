#ifndef _STATE_H_
#define _STATE_H_

#include <memory>
#include <unordered_map>
#include <string>

class DFAState
{
public:
    DFAState();

    void addTransition(const std::string &input, std::shared_ptr<DFAState> nextState);
    std::shared_ptr<DFAState> run(const std::string &input);

private:
    std::unordered_map<std::string, std::weak_ptr<DFAState>> transitions_;
};


#endif // _STATE_H_