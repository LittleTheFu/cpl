#ifndef _EXPRESSION_NODE_H_
#define _EXPRESSION_NODE_H_

#include "astNode.h"

class ExpressionNode : public AstNode {
public:
    virtual ~ExpressionNode() = default;
};


#endif // _EXPRESSION_NODE_H_