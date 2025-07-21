#include "regExAlternationNode.h"

regExAlternationNode::regExAlternationNode(std::shared_ptr<RegExNode> left, std::shared_ptr<RegExNode> right)
{
    left_ = left;
    right_ = right;
}

regExAlternationNode::~regExAlternationNode()
{
}

std::shared_ptr<NfaStateFragment> regExAlternationNode::buildNfaStateFragment()
{
    auto leftFragment = left_->buildNfaStateFragment();
    auto rightFragment = right_->buildNfaStateFragment();

    auto startState = std::make_shared<NFAState>();
    auto endState = std::make_shared<NFAState>();

    startState->addEpsilonTransition(leftFragment->getStartState());
    startState->addEpsilonTransition(rightFragment->getStartState());

    leftFragment->getEndState()->addEpsilonTransition(endState);
    rightFragment->getEndState()->addEpsilonTransition(endState);

    std::shared_ptr<NfaStateFragment> fragment = std::make_shared<NfaStateFragment>(startState, endState);
    fragment->addState(startState);
    fragment->addState(endState);

    fragment->merge(leftFragment);
    fragment->merge(rightFragment);

    return fragment;
}
