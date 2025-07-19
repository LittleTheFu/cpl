#include "virtualMachine.h"
#include "instruction.h"
#include <sstream>

VirtualMachine::VirtualMachine()
{
    memorySize_ = 1024;
    memory_.resize(memorySize_);

    registerSize_ = 8;
    register_.resize(registerSize_);

    programCounter_ = 0;

    sourceCode_ = "ADD R1 R2\n"
                  "PUSH R1\n"
                  "POP R2\n";
}

void VirtualMachine::loadProgram()
{
    std::stringstream ss(sourceCode_);
    std::string line;
    while (std::getline(ss, line))
    {
        Instruction instruction;
        instruction.readInstruction(line);

        instructions_.push_back(instruction);

        std::cout << instruction << std::endl;
    }
}

void VirtualMachine::execute()
{
    for (programCounter_ = 0; programCounter_ < instructions_.size(); programCounter_++)
    {
        // instruction.execute();
    }
}

void VirtualMachine::executeInstruction(const Instruction &instruction)
{
    switch (instruction.getOpCode())
    {
    case OpCode::NOP:
        executeNOP(instruction);
        break;
    case OpCode::ADD:
        executeADD(instruction);
        break;
    case OpCode::PUSH:
        executePUSH(instruction);
        break;
    case OpCode::POP:
        executePOP(instruction);
        break;
    default:
        break;
    }
}

void VirtualMachine::executeNOP(const Instruction &instruction)
{
}

void VirtualMachine::executeADD(const Instruction &instruction)
{
    Operand operand1 = instruction.getOperandFirst();
    Operand operand2 = instruction.getOperandSecond();

    if (operand1.type != OperandType::REGISTER && operand1.type != OperandType::MEMORY)
    {
        throw std::runtime_error("Error operand type");
    }

    int value1 = getOperandValue(operand1);
    int value2 = getOperandValue(operand2);
    int result = value1 + value2;
    setOperandValue(operand1, result);
}

void VirtualMachine::executePUSH(const Instruction &instruction)
{
}

void VirtualMachine::executePOP(const Instruction &instruction)
{
}

int VirtualMachine::getOperandValue(const Operand &operand)
{
    if (operand.type == OperandType::REGISTER)
    {
        return register_[operand.value];
    }
    else if (operand.type == OperandType::MEMORY)
    {
        return memory_[operand.value];
    }
    else if (operand.type == OperandType::IMMEDIATE)
    {
        return operand.value;
    }

    throw std::runtime_error("Error operand type");

    return 0;
}

void VirtualMachine::setOperandValue(Operand &operand, int value)
{
    if (operand.type == OperandType::REGISTER)
    {
        register_[operand.value] = value;
    }
    else if (operand.type == OperandType::MEMORY)
    {
        memory_[operand.value] = value;
    }
    else if (operand.type == OperandType::IMMEDIATE)
    {
        throw std::runtime_error("Error operand type");
        // operand.value = value;
    }

    throw std::runtime_error("Error operand type");
}
