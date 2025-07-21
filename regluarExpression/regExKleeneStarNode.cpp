#include "regExKleeneStarNode.h"

RegExKleeneStarNode::RegExKleeneStarNode(std::shared_ptr<RegExNode> node)
{
    node_ = node;
}

RegExKleeneStarNode::~RegExKleeneStarNode()
{
}

std::shared_ptr<NfaStateFragment> RegExKleeneStarNode::buildNfaStateFragment()
{
    auto fragment = node_->buildNfaStateFragment();
    auto startState = std::make_shared<NFAState>();
    auto endState = std::make_shared<NFAState>();

    startState->addEpsilonTransition(fragment->getStartState());
    startState->addEpsilonTransition(endState);

    fragment->getEndState()->addEpsilonTransition(endState);
    fragment->getEndState()->addEpsilonTransition(fragment->getStartState());

    std::shared_ptr<NfaStateFragment> kleeneFragment = std::make_shared<NfaStateFragment>(startState, endState);
    kleeneFragment->addState(startState);
    kleeneFragment->addState(endState);

    kleeneFragment->merge(fragment);

    return kleeneFragment;
}
