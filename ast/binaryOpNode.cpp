#include "binaryOpNode.h"

BinaryOpNode::BinaryOpNode(std::unique_ptr<ExpressionNode> &&left, std::unique_ptr<ExpressionNode> &&right, BinaryOpType op_type)
    : left_(std::move(left)),
      right_(std::move(right)),
      op_type_(op_type)
{
}

void BinaryOpNode::evaluate()
{
    left_->evaluate();
    right_->evaluate();
}
