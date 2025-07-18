#include "regExKleeneStarNode.h"

regExKleeneStarNode::regExKleeneStarNode(RegExNode *node)
{
    node_ = node;
}

regExKleeneStarNode::~regExKleeneStarNode()
{
}

std::shared_ptr<NfaStateFragment> regExKleeneStarNode::buildNfaStateFragment()
{
    auto fragment = node_->buildNfaStateFragment();
    auto startState = std::make_shared<NFAState>();
    auto endState = std::make_shared<NFAState>();

    startState->addEpsilonTransition(fragment->getStartState());
    startState->addEpsilonTransition(endState);

    fragment->getEndState()->addEpsilonTransition(endState);
    fragment->getEndState()->addEpsilonTransition(fragment->getStartState());

    return std::make_shared<NfaStateFragment>(startState, endState);
}
