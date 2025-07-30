#include "binaryOpNode.h"
#include <iostream>

BinaryOpNode::BinaryOpNode(std::unique_ptr<AstNode> &&left, std::unique_ptr<AstNode> &&right, BinaryOpType op_type)
    : left_(std::move(left)),
      right_(std::move(right)),
      op_type_(op_type)
{
}

void BinaryOpNode::evaluate()
{
    std::cout << "(";
    left_->evaluate();
    std::cout << binaryOpTypeToString(op_type_) << " ";
    right_->evaluate();
    std::cout << ")";
}

std::string binaryOpTypeToString(BinaryOpType op_type)
{
    switch (op_type)
    {
    case BinaryOpType::ADD:
        return "+";
    case BinaryOpType::SUBTRACT:
        return "-";
    case BinaryOpType::MULTIPLY:
        return "*";
    case BinaryOpType::DIVIDE:
        return "/";
    default:
        return "";
    }
}
