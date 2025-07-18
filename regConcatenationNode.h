#ifndef _REGEX_CONCATENATION_NODE_H_
#define _REGEX_CONCATENATION_NODE_H_

#include "regExNode.h"

class regConcatenationNode : public RegExNode
{
public:
    explicit regConcatenationNode(RegExNode* left, RegExNode* right);
    virtual ~regConcatenationNode();

private:
    RegExNode* left_;
    RegExNode* right_;
};

#endif // _REGEX_CONCATENATION_NODE_H_