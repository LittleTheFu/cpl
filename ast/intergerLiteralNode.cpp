#include "intergerLiteralNode.h"
#include <iostream>

IntegerLiteralNode::IntegerLiteralNode(int value)
{
    value_ = value;
}

void IntegerLiteralNode::evaluate()
{
    std::cout << value_ << " ";
}

IROperand IntegerLiteralNode::genIRCode(IRProgram &program)
{
    return IROperand::createInt(value_);
}
