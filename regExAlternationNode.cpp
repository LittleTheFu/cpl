#include "regExAlternationNode.h"

regExAlternationNode::regExAlternationNode(RegExNode *left, RegExNode *right)
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

    auto startState = std::make_shared<NfaState>();
    auto endState = std::make_shared<NfaState>();

    startState->addEpsilonTransition(leftFragment->getStartState());
    startState->addEpsilonTransition(rightFragment->getStartState());
    
    leftFragment->getEndState()->addEpsilonTransition(endState);
    rightFragment->getEndState()->addEpsilonTransition(endState);

    return std::make_shared<NfaStateFragment>(startState, endState);
}
