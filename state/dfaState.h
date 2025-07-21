#ifndef _STATE_H_
#define _STATE_H_

#include <memory>
#include <unordered_map>
#include <string>

class DFAState
{
public:
    DFAState(bool isEndState = false);
    ~DFAState() = default;

    void addTransition(char input, std::shared_ptr<DFAState> nextState);
    std::shared_ptr<DFAState> run(char input);

    void setEndState(bool isEndState);
    bool isEndState() const;

private:
    static int genId();

private:
    std::unordered_map<char, std::weak_ptr<DFAState>> transitions_;
    bool isEndState_;

    int id_;
};


#endif // _STATE_H_