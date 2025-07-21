#include "regExOptionalNode.h"

RegExOptionalNode::RegExOptionalNode(std::shared_ptr<RegExNode> node)
{
    node_ = node;
}

RegExOptionalNode::~RegExOptionalNode()
{
}

std::shared_ptr<NfaStateFragment> RegExOptionalNode::buildNfaStateFragment()
{
    auto fragment = node_->buildNfaStateFragment();
    auto startState = std::make_shared<NFAState>();
    auto endState = std::make_shared<NFAState>();

    startState->addEpsilonTransition(fragment->getStartState());
    startState->addEpsilonTransition(endState);

    fragment->getEndState()->addEpsilonTransition(endState);

    std::shared_ptr<NfaStateFragment> optionalFragment = std::make_shared<NfaStateFragment>(startState, endState);
    optionalFragment->addState(startState);
    optionalFragment->addState(endState);

    optionalFragment->merge(fragment);

    return optionalFragment;
}

