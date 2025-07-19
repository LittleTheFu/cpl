#include "virtualMachine.h"

VirtualMachine::VirtualMachine()
{
    memorySize_ = 1024;
    memory_.resize(memorySize_);
}
