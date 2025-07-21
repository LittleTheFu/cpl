#ifndef _REG_EX_OPTIONAL_NODE_H_
#define _REG_EX_OPTIONAL_NODE_H_

#include "regExNode.h"
#include <memory>

class regExOptionalNode : public RegExNode
{
public:
    explicit regExOptionalNode(std::shared_ptr<RegExNode> node);
    virtual ~regExOptionalNode();

    std::shared_ptr<NfaStateFragment> buildNfaStateFragment() override;

private:
    std::shared_ptr<RegExNode> node_;
};

#endif // _REG_EX_OPTIONAL_NODE_H_