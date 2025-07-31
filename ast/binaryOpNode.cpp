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

IROperand BinaryOpNode::genIRCode(IRProgram &program)
{
    IROperand left = left_->genIRCode(program);
    IROperand right = right_->genIRCode(program);
    IROperand result = IROperand::createTemporary(program.getTempVarCount());

    IROpCode ir_op_type = getIRType(op_type_);

    program.addInstruction(IRInstruction(ir_op_type, {result, left, right}));
    return result;
}

IROpCode BinaryOpNode::getIRType(BinaryOpType op_type) const
{
    switch (op_type)
    {
    case BinaryOpType::ADD:
        return IROpCode::ADD;
    case BinaryOpType::SUBTRACT:
        return IROpCode::SUB;
    case BinaryOpType::MULTIPLY:
        return IROpCode::MUL;
    case BinaryOpType::DIVIDE:
        return IROpCode::DIV;
    default:
        return IROpCode::UNKNOWN;
    }
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
