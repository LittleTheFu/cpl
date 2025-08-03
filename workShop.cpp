#include "workShop.h"

WorkShop::WorkShop()
{
    vm_.loadProgram();
}

WorkShop& WorkShop::getInstance()
{
    static WorkShop instance;
    return instance;
}

std::vector<std::string> WorkShop::getVMSrc()
{
    return vm_.getSourceCode();
}

size_t WorkShop::getPC() const
{
    return vm_.getProgramCounter();
}

void WorkShop::step()
{
    vm_.step();
}

void WorkShop::resetProgram()
{
    vm_.resetProgram();
}

const std::vector<int> &WorkShop::getVMRegisters() const
{
    return vm_.getRegisters();
}

const std::vector<int> &WorkShop::getVMMemory() const
{
    return vm_.getMemory();
}

bool WorkShop::getVMZeroFlag() const
{
    return vm_.getZeroFlag();
}

bool WorkShop::getVMSignFlag() const
{
    return vm_.getSignFlag();
}
