#ifndef _REG_EX_OPTIONAL_NODE_H_
#define _REG_EX_OPTIONAL_NODE_H_

#include "regExNode.h"
#include <memory>

class RegExOptionalNode : public RegExNode
{
public:
    explicit RegExOptionalNode(std::shared_ptr<RegExNode> node);
    virtual ~RegExOptionalNode();

    std::shared_ptr<NfaStateFragment> buildNfaStateFragment() override;

private:
    std::shared_ptr<RegExNode> node_;
};

#endif // _REG_EX_OPTIONAL_NODE_H_