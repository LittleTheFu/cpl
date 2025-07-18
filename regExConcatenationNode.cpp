#include "regExConcatenationNode.h"

regExConcatenationNode::regExConcatenationNode(RegExNode *left, RegExNode *right)
{
    left_ = left;
    right_ = right;
}

regExConcatenationNode::~regExConcatenationNode()
{
}

std::shared_ptr<NfaStateFragment> regExConcatenationNode::buildNfaStateFragment()
{
    auto leftFragment = left_->buildNfaStateFragment();
    auto rightFragment = right_->buildNfaStateFragment();

    leftFragment->getEndState()->addEpsilonTransition(rightFragment->getStartState());
    return std::make_shared<NfaStateFragment>(leftFragment->getStartState(), rightFragment->getEndState());
}
