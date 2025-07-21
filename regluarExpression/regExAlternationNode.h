#ifndef _REGEX_ALTERNATION_NODE_H_
#define _REGEX_ALTERNATION_NODE_H_

#include "regExNode.h"
#include <memory>

class regExAlternationNode : public RegExNode
{
public:
    explicit regExAlternationNode(std::shared_ptr<RegExNode> left, std::shared_ptr<RegExNode> right);
    virtual ~regExAlternationNode();

    std::shared_ptr<NfaStateFragment> buildNfaStateFragment() override;

private:
    std::shared_ptr<RegExNode> left_;
    std::shared_ptr<RegExNode> right_;
};


#endif // _REGEX_ALTERNATION_NODE_H_