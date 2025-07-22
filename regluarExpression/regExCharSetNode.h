#ifndef _REGEX_CHARSET_NODE_H_
#define _REGEX_CHARSET_NODE_H_

#include "regExNode.h"
#include <string>
#include <set>

class RegExCharSetNode : public RegExNode
{
public:
    explicit RegExCharSetNode(const std::set<char> &charSet, bool isNegated);
    virtual ~RegExCharSetNode();

    std::shared_ptr<NfaStateFragment> buildNfaStateFragment() override;

    std::set<char> getCharSet() const { return charSet_; }
    bool isNegated() const { return isNegated_; }

private:
    std::set<char> charSet_;
    bool isNegated_;
};

#endif // _REGEX_CHARSET_NODE_H_