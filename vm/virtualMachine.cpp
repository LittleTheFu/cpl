#include "virtualMachine.h"
#include "instruction.h"
#include <sstream>

VirtualMachine::VirtualMachine()
{
    memorySize_ = 1024;
    memory_.resize(memorySize_);

    registerSize_ = 8;
    register_.resize(registerSize_);

    sourceCode_ = "ADD R1 R2\n"
                   "PUSH R1\n"
                   "POP R2\n";
}

void VirtualMachine::run()
{
    std::stringstream ss(sourceCode_);
    std::string line;
    while (std::getline(ss, line))
    {
        Instruction instruction;
        instruction.readInstruction(line);

        std::cout << instruction << std::endl;
    }
}
