#include "regExCharNode.h"

RegExCharNode::RegExCharNode(char charactar)
{
    charactar_ = charactar;
}

RegExCharNode::~RegExCharNode()
{
}

std::shared_ptr<NfaStateFragment> RegExCharNode::buildNfaStateFragment()
{
    auto startState = std::make_shared<NFAState>();
    auto endState = std::make_shared<NFAState>();

    startState->addTransition(charactar_, endState);

    std::shared_ptr<NfaStateFragment> fragment = std::make_shared<NfaStateFragment>(startState, endState);
    
    fragment->addState(startState);
    fragment->addState(endState);

    return fragment;
}
