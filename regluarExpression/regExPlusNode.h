#ifndef _REG_EX_PLUS_NODE_H_
#define _REG_EX_PLUS_NODE_H_

#include "regExNode.h"
#include <memory>

class regExPlusNode : public RegExNode
{
public:
    regExPlusNode(std::shared_ptr<RegExNode> node);
    virtual ~regExPlusNode();

    std::shared_ptr<NfaStateFragment> buildNfaStateFragment() override;

private:
    std::shared_ptr<RegExNode> node_;
};

#endif // _REG_EX_PLUS_NODE_H_