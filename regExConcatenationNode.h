#ifndef _REGEX_CONCATENATION_NODE_H_
#define _REGEX_CONCATENATION_NODE_H_

#include "regExNode.h"

class regExConcatenationNode : public RegExNode
{
public:
    explicit regExConcatenationNode(RegExNode* left, RegExNode* right);
    virtual ~regExConcatenationNode();

    std::shared_ptr<NfaStateFragment> buildNfaStateFragment() override;

private:
    RegExNode* left_;
    RegExNode* right_;
};

#endif // _REGEX_CONCATENATION_NODE_H_