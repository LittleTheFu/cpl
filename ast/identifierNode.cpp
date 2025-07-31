#include "identifierNode.h"
#include <iostream>

IdentifierNode::IdentifierNode(const std::string &name)
{
    name_ = name;
}

void IdentifierNode::evaluate()
{
    std::cout << name_ << " ";
}

IROperand IdentifierNode::genIRCode(IRProgram &program)
{
    return IROperand::createIdentifier(name_);
}
