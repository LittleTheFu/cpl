#ifndef _REGEX_KLEENE_STAR_NODE_H_
#define _REGEX_KLEENE_STAR_NODE_H_

#include "regExNode.h"

class regExKleeneStarNode : public RegExNode
{
public:
    explicit regExKleeneStarNode(RegExNode* node);
    virtual ~regExKleeneStarNode();

    std::shared_ptr<NfaStateFragment> buildNfaStateFragment() override;

private:
    RegExNode* node_;
};


#endif // _REGEX_KLEENE_STAR_NODE_H_