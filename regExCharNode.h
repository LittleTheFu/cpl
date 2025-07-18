#ifndef _REGEX_CHAR_NODE_H_
#define _REGEX_CHAR_NODE_H_

#include "regExNode.h"


class RegExCharNode : public RegExNode
{
public:
    RegExCharNode(char charactor);
    virtual ~RegExCharNode();

private:
    char charactor_;
};


#endif // _REGEX_CHAR_NODE_H_