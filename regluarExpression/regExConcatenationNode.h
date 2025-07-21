#ifndef _REGEX_CONCATENATION_NODE_H_
#define _REGEX_CONCATENATION_NODE_H_

#include "regExNode.h"
#include <memory>


class RegExConcatenationNode : public RegExNode
{
public:
    explicit RegExConcatenationNode(std::shared_ptr<RegExNode> left, std::shared_ptr<RegExNode> right);
    virtual ~RegExConcatenationNode();

    std::shared_ptr<NfaStateFragment> buildNfaStateFragment() override;

private:
    std::shared_ptr<RegExNode> left_;
    std::shared_ptr<RegExNode> right_;
};

#endif // _REGEX_CONCATENATION_NODE_H_