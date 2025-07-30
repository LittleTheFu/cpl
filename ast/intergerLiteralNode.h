#ifndef _INTEGER_LITERAL_NODE_H_
#define _INTEGER_LITERAL_NODE_H_

#include "expressionNode.h"

class IntegerLiteralNode : public ExpressionNode {
public:
    IntegerLiteralNode(int value);
    virtual ~IntegerLiteralNode() override = default;
    void evaluate() override;
private:
    int value_;
};

#endif // _INTEGER_LITERAL_NODE_H_
