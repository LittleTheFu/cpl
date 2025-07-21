#include "regExConcatenationNode.h"

RegExConcatenationNode::RegExConcatenationNode(std::shared_ptr<RegExNode> left, std::shared_ptr<RegExNode> right)
{
    left_ = left;
    right_ = right;
}

RegExConcatenationNode::~RegExConcatenationNode()
{
}

std::shared_ptr<NfaStateFragment> RegExConcatenationNode::buildNfaStateFragment()
{
    auto leftFragment = left_->buildNfaStateFragment();
    auto rightFragment = right_->buildNfaStateFragment();

    leftFragment->getEndState()->addEpsilonTransition(rightFragment->getStartState());

    std::shared_ptr<NfaStateFragment> concatenationFragment = std::make_shared<NfaStateFragment>(leftFragment->getStartState(), rightFragment->getEndState());
    concatenationFragment->merge(leftFragment);
    concatenationFragment->merge(rightFragment);
    
    return concatenationFragment;
}
