#include "regExPlusNode.h"

regExPlusNode::regExPlusNode(std::shared_ptr<RegExNode> node)
{
    node_ = node;
}

regExPlusNode::~regExPlusNode()
{
}

std::shared_ptr<NfaStateFragment> regExPlusNode::buildNfaStateFragment()
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
