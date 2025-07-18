#include "regExAlternationNode.h"

regExAlternationNode::regExAlternationNode(RegExNode *left, RegExNode *right)
{
    left_ = left;
    right_ = right;
}

regExAlternationNode::~regExAlternationNode()
{
}
