#ifndef _REGEX_ALTERNATION_NODE_H_
#define _REGEX_ALTERNATION_NODE_H_

#include "regExNode.h"
#include <memory>

class RegExAlternationNode : public RegExNode
{
public:
    explicit RegExAlternationNode(std::shared_ptr<RegExNode> left, std::shared_ptr<RegExNode> right);
    virtual ~RegExAlternationNode();

    std::shared_ptr<NfaStateFragment> buildNfaStateFragment() override;

    std::shared_ptr<RegExNode> getLeft() const { return left_; };
    std::shared_ptr<RegExNode> getRight() const { return right_; };

private:
    std::shared_ptr<RegExNode> left_;
    std::shared_ptr<RegExNode> right_;
};


#endif // _REGEX_ALTERNATION_NODE_H_