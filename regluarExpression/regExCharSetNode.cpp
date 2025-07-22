#include "regExCharSetNode.h"
#include "alphaBet.h"

RegExCharSetNode::RegExCharSetNode(const std::set<char> &charSet, bool isNegated)
{
    charSet_ = charSet;
    isNegated_ = isNegated;
}

RegExCharSetNode::~RegExCharSetNode()
{
}

std::shared_ptr<NfaStateFragment> RegExCharSetNode::buildNfaStateFragment()
{
    std::shared_ptr<NFAState> startState = std::make_shared<NFAState>();
    std::shared_ptr<NFAState> endState = std::make_shared<NFAState>();

    std::set<char> charSetToMatch;
    std::set<char> alphaBetSet = AlphaBet::instance().getcharactars();
    if(isNegated_)
    {
        for(char character : alphaBetSet)
        {
            if(charSet_.find(character) == charSet_.end())
            {
                charSetToMatch.insert(character);
            }
        }
    }
    else
    {
        charSetToMatch = charSet_;
    }

    for(char character : charSetToMatch)
    {
        startState->addTransition(character, endState);
    }

    std::shared_ptr<NfaStateFragment> fragment = std::make_shared<NfaStateFragment>(startState, endState);
    fragment->addState(startState);
    fragment->addState(endState);

    return fragment;
}
