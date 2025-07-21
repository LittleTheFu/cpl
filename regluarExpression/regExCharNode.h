#ifndef _REGEX_CHAR_NODE_H_
#define _REGEX_CHAR_NODE_H_

#include "regExNode.h"


class RegExCharNode : public RegExNode
{
public:
    explicit RegExCharNode(char charactar);
    virtual ~RegExCharNode();

    std::shared_ptr<NfaStateFragment> buildNfaStateFragment() override;

private:
    char charactar_;
};


#endif // _REGEX_CHAR_NODE_H_