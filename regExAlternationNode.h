#ifndef _REGEX_ALTERNATION_NODE_H_
#define _REGEX_ALTERNATION_NODE_H_

#include "regExNode.h"

class regExAlternationNode : public RegExNode
{
public:
    regExAlternationNode(RegExNode* left, RegExNode* right);
    virtual ~regExAlternationNode();

private:
    RegExNode* left_;
    RegExNode* right_;
};


#endif // _REGEX_ALTERNATION_NODE_H_