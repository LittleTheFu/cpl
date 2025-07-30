#ifndef _BINARY_OP_NODE_H_
#define _BINARY_OP_NODE_H_

#include "expressionNode.h"
#include "astNode.h"
#include <memory>

enum class BinaryOpType {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
};

class BinaryOpNode : public ExpressionNode {
public:
    BinaryOpNode(std::unique_ptr<ExpressionNode>&& left, std::unique_ptr<ExpressionNode>&& right, BinaryOpType op_type);
    virtual ~BinaryOpNode() override = default;
    void evaluate() override;
private:
    std::unique_ptr<ExpressionNode> left_;
    std::unique_ptr<ExpressionNode> right_;
    BinaryOpType op_type_;
};

#endif // _BINARY_OP_NODE_H_