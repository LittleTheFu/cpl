#include "regConcatenationNode.h"

regConcatenationNode::regConcatenationNode(RegExNode *left, RegExNode *right)
{
    left_ = left;
    right_ = right;
}

regConcatenationNode::~regConcatenationNode()
{
}
