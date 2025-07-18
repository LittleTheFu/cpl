#ifndef _NFA_STATE_H_
#define _NFA_STATE_H_

#include <memory>
#include <unordered_map>
#include <string>
#include <set>

class NfaState;

struct CompareWeakNfaStatePtr
{
    bool operator()(const std::weak_ptr<NfaState> &a, const std::weak_ptr<NfaState> &b) const
    {
        return a.owner_before(b);
    }
};


class NfaState
{
public:
    NfaState();

    void addTransition(char input, std::shared_ptr<NfaState> nextState);

    void addEpsilonTransition(std::shared_ptr<NfaState> nextState);
    const std::set<std::weak_ptr<NfaState>, CompareWeakNfaStatePtr>& getEpsilonTransitions() const;

    std::vector<std::shared_ptr<NfaState>> run(char input);

private:
    std::unordered_map<char, std::set<std::weak_ptr<NfaState>, CompareWeakNfaStatePtr>> transitions_;
    std::set<std::weak_ptr<NfaState>, CompareWeakNfaStatePtr> epsilonTransitions_;
};

#endif // _NFA_STATE_H_