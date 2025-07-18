#include "regExCharNode.h"

RegExCharNode::RegExCharNode(char charactor)
{
    charactor_ = charactor;
}

RegExCharNode::~RegExCharNode()
{
}

std::shared_ptr<NfaStateFragment> RegExCharNode::buildNfaStateFragment()
{
    auto startState = std::make_shared<NFAState>();
    auto endState = std::make_shared<NFAState>();

    startState->addTransition(charactor_, endState);

    return std::make_shared<NfaStateFragment>(startState, endState);
}
