#ifndef _VIRTUAL_MACHINE_H_
#define _VIRTUAL_MACHINE_H_

#include <vector>
#include <map>
#include <string>
#include "instruction.h"

class VirtualMachine
{
public:
    VirtualMachine();
    ~VirtualMachine() = default;

    void loadProgram();
    void execute();

    void clear();

    void setRegister(size_t index, int value);
    int getRegister(size_t index) const;

    void setMemory(size_t index, int value);
    int getMemory(size_t index) const;

    bool getZeroFlag() const;
    bool getSignFlag() const;

    void setSourceCode(const std::string &sourceCode);

private:
    void executeInstruction(const Instruction &instruction);

    void executeNOP(const Instruction &instruction);
    void executeMOV(const Instruction &instruction);
    void executePUSH(const Instruction &instruction);
    void executePOP(const Instruction &instruction);

    void executeADD(const Instruction &instruction);
    void executeSUB(const Instruction &instruction);
    void executeMUL(const Instruction &instruction);
    void executeDIV(const Instruction &instruction);

    void executeCMP(const Instruction &instruction);

    void executeJMP(const Instruction &instruction);
    void executeJMPZ(const Instruction &instruction);
    void executeJMPNZ(const Instruction &instruction);
    void executeJMPL(const Instruction &instruction);
    void executeJMPLE(const Instruction &instruction);
    void executeJMPG(const Instruction &instruction);
    void executeJMPGE(const Instruction &instruction);

    void executeHLT(const Instruction &instruction);

    int getOperandValue(const Operand &operand);
    void setOperandValue(Operand &operand, int value);

    void buildLabelMap();
    void assembleInstructions();

    void updateFlags(int value);

public:
    void printStack() const;
    void printRegister() const;
    void printFlags() const;
    void printMemory(size_t start = 0, size_t end = 6) const;

private:
    std::string sourceCode_;
    std::map<std::string, size_t> labelMap_;

    std::vector<int> memory_;
    size_t memorySize_;

    std::vector<int> register_;
    size_t registerSize_;

    std::vector<Instruction> instructions_;
    size_t programCounter_;

    size_t stackPointer_;
    size_t stackBaseAddress_;
    size_t stackLimitAddress_;

    bool zeroFlag_;
    bool signFlag_;

    bool isRunning_;
};

#endif // _VIRTUAL_MACHINE_H_