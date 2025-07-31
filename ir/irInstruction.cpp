#include "irInstruction.h"

IRInstruction::IRInstruction(IROpCode opCode, std::vector<IROperand> &&operands)
    : opCode_(opCode), operands_(std::move(operands))
{
}

IROpCode IRInstruction::getOpCode() const
{
    return opCode_;
}

IROperand IRInstruction::getOperand(size_t index) const
{
    return operands_.at(index);
}
