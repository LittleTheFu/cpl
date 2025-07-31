#include "irProgram.h"

IRProgram::IRProgram()
{
    temp_var_count_ = 0;
}

void IRProgram::addInstruction(IRInstruction &&instruction)
{
    instructions_.push_back(std::move(instruction));
}

int IRProgram::getTempVarCount()
{
    return temp_var_count_++;
}

const std::vector<IRInstruction> &IRProgram::getInstructions() const
{
    return instructions_;
}

void IRProgram::print() const
{
    for (const auto& instruction : instructions_)
    {
        instruction.print();
    }
}
