#include "irInstruction.h"
#include <iostream>

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

void IRInstruction::print() const
{
    std::cout << opCodeToString(opCode_) << " ";
    for (const auto& operand : operands_)
    {
        std::cout << operand.toString() << " ";
    }
    std::cout << std::endl;
}

std::string opCodeToString(IROpCode opCode)
{
    switch (opCode)
    {
    case IROpCode::NOP:
        return "NOP";
    case IROpCode::MOV:
        return "MOV";
    case IROpCode::ADD:
        return "ADD";
    case IROpCode::SUB:
        return "SUB";
    case IROpCode::MUL:
        return "MUL";
    case IROpCode::DIV:
        return "DIV";
    default:
        return "UNKNOWN";
    }
}
