#include "regEx.h"
#include "alphaBet.h"
#include <queue>

// RegEx::RegEx(const std::string &regex)
// {
//     regex_ = regex;
// }

RegEx::RegEx(std::shared_ptr<RegExNode> root)
{
    root_ = root;

    nfaStateFragment_ = root_->buildNfaStateFragment();
}

RegEx::~RegEx()
{
}

void RegEx::buildDFA()
{
    std::set<std::shared_ptr<NFAState>, CompareNfaStateSharedPtr> startStates = epsilonClosure({nfaStateFragment_->getStartState()});

    std::queue<std::set<std::shared_ptr<NFAState>, CompareNfaStateSharedPtr>> queue;
    queue.push(startStates);

    dfaStateMap_[startStates] = std::make_shared<DFAState>(isContainEndState(startStates));
    startState_ = dfaStateMap_[startStates];

    while(!queue.empty())
    {
        auto currentStates = queue.front();
        queue.pop();

        std::shared_ptr<DFAState> currentDfaState = dfaStateMap_[currentStates];

        for(const auto &charactor : AlphaBet::instance().getCharactors())
        {
            std::set<std::shared_ptr<NFAState>, CompareNfaStateSharedPtr> nextAllStates;
            for(const auto &state : currentStates)
            {
                std::set<std::shared_ptr<NFAState>, CompareNfaStateSharedPtr> nextStates = state->run(charactor);
                nextAllStates.insert(nextStates.begin(), nextStates.end());
            }

            nextAllStates = epsilonClosure(nextAllStates);

            if(nextAllStates.empty())
            {
                continue;
            }

            if(dfaStateMap_.find(nextAllStates) == dfaStateMap_.end())
            {
                queue.push(nextAllStates);
                dfaStateMap_[nextAllStates] = std::make_shared<DFAState>(isContainEndState(nextAllStates));
            }

            currentDfaState->addTransition(charactor, dfaStateMap_[nextAllStates]);
        }
    }
}

 
std::set<std::shared_ptr<NFAState>, CompareNfaStateSharedPtr> RegEx::epsilonClosure(const std::set<std::shared_ptr<NFAState>, CompareNfaStateSharedPtr> &states)
{
    std::set<std::shared_ptr<NFAState>, CompareNfaStateSharedPtr> result = states;
    std::queue<std::shared_ptr<NFAState>> queue;
  
    for(const auto &state : states)
    {
        queue.push(state);
    }

    while (!queue.empty())
    {
        auto state = queue.front();
        queue.pop();

        for (auto &epsilonState : state->getEpsilonTransitions())
        {
            if (auto lockedState = epsilonState.lock())
            {
                if(result.insert(lockedState).second)
                {
                    queue.push(lockedState);
                }
            }
        }
    }

    return result;
}

bool RegEx::isContainEndState(const std::set<std::shared_ptr<NFAState>, CompareNfaStateSharedPtr> &states)
{
    for (const auto &state : states)
    {
        if (state == nfaStateFragment_->getEndState())
        {
            return true;
        }
    }

    return false;
}
