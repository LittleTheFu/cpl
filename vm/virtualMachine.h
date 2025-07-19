#ifndef _VIRTUAL_MACHINE_H_
#define _VIRTUAL_MACHINE_H_

#include <vector>
#include <string>
#include "instruction.h"

class VirtualMachine
{
public:
    VirtualMachine();
    ~VirtualMachine() = default;

    void loadProgram();
    void execute();

private:
    void executeInstruction(const Instruction &instruction);

    void executeNOP(const Instruction &instruction);
    void executeADD(const Instruction &instruction);
    void executePUSH(const Instruction &instruction);
    void executePOP(const Instruction &instruction);

    int getOperandValue(const Operand &operand);
    void setOperandValue(Operand &operand, int value);

private:
    std::string sourceCode_;

    std::vector<int> memory_;
    size_t memorySize_;

    std::vector<int> register_;
    size_t registerSize_;

    std::vector<Instruction> instructions_;
    size_t programCounter_;
};

#endif // _VIRTUAL_MACHINE_H_