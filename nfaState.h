#ifndef _NFA_STATE_H_
#define _NFA_STATE_H_

#include <memory>
#include <unordered_map>
#include <string>
#include <set>

class NfaState
{
public:
    NfaState();

    void addTransition(const std::string &input, std::shared_ptr<NfaState> nextState);
    std::vector<std::shared_ptr<NfaState>> run(const std::string &input);

private:
    std::unordered_map<std::string, std::set<std::weak_ptr<NfaState>>> transitions_;
};

#endif // _NFA_STATE_H_