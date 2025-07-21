#ifndef _REGEX_KLEENE_STAR_NODE_H_
#define _REGEX_KLEENE_STAR_NODE_H_

#include "regExNode.h"
#include <memory>

class RegExKleeneStarNode : public RegExNode
{
public:
    explicit RegExKleeneStarNode(std::shared_ptr<RegExNode> node);
    virtual ~RegExKleeneStarNode();

    std::shared_ptr<NfaStateFragment> buildNfaStateFragment() override;

private:
    std::shared_ptr<RegExNode> node_;
};


#endif // _REGEX_KLEENE_STAR_NODE_H_