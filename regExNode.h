#ifndef _REGEX_NODE_H_
#define _REGEX_NODE_H_

#include "nfaStateFragment.h"

class RegExNode
{
public:
    explicit RegExNode();
    virtual ~RegExNode();

    virtual std::shared_ptr<NfaStateFragment> buildNfaStateFragment() = 0;
};

#endif // _REGEX_NODE_H_