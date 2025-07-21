#ifndef _REG_EX_PLUS_NODE_H_
#define _REG_EX_PLUS_NODE_H_

#include "regExNode.h"
#include <memory>

class RegExPlusNode : public RegExNode
{
public:
    RegExPlusNode(std::shared_ptr<RegExNode> node);
    virtual ~RegExPlusNode();

    std::shared_ptr<NfaStateFragment> buildNfaStateFragment() override;

    std::shared_ptr<RegExNode> getNode() const { return node_; };

private:
    std::shared_ptr<RegExNode> node_;
};

#endif // _REG_EX_PLUS_NODE_H_