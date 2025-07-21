#include "regExPlusNode.h"

RegExPlusNode::RegExPlusNode(std::shared_ptr<RegExNode> node)
{
    node_ = node;
}

RegExPlusNode::~RegExPlusNode()
{
}

std::shared_ptr<NfaStateFragment> RegExPlusNode::buildNfaStateFragment()
{
    auto fragment = node_->buildNfaStateFragment();
    auto startState = std::make_shared<NFAState>();
    auto endState = std::make_shared<NFAState>();

    startState->addEpsilonTransition(fragment->getStartState());

    fragment->getEndState()->addEpsilonTransition(fragment->getStartState());
    fragment->getEndState()->addEpsilonTransition(endState);

    std::shared_ptr<NfaStateFragment> plusFragment = std::make_shared<NfaStateFragment>(startState, endState);
    plusFragment->addState(startState);
    plusFragment->addState(endState);

    plusFragment->merge(fragment);

    return plusFragment;
}
