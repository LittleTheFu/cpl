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
