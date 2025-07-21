#ifndef _NFA_STATE_H_
#define _NFA_STATE_H_

#include <memory>
#include <unordered_map>
#include <string>
#include <set>

class NFAState;

struct CompareWeakNfaStatePtr
{
    bool operator()(const std::weak_ptr<NFAState> &a, const std::weak_ptr<NFAState> &b) const
    {
        return a.owner_before(b);
    }
};

struct CompareNfaStateSharedPtr
{
    bool operator()(const std::shared_ptr<NFAState> &a, const std::shared_ptr<NFAState> &b) const
    {
        return a.owner_before(b);
    }
};


class NFAState
{
public:
    NFAState();

    void addTransition(char input, std::shared_ptr<NFAState> nextState);

    void addEpsilonTransition(std::shared_ptr<NFAState> nextState);
    const std::set<std::weak_ptr<NFAState>, CompareWeakNfaStatePtr>& getEpsilonTransitions() const;

    std::set<std::shared_ptr<NFAState>, CompareNfaStateSharedPtr> run(char input) const;

private:
    static int genId();

private:
    std::unordered_map<char, std::set<std::weak_ptr<NFAState>, CompareWeakNfaStatePtr>> transitions_;
    std::set<std::weak_ptr<NFAState>, CompareWeakNfaStatePtr> epsilonTransitions_;

    int id_;
};

#endif // _NFA_STATE_H_