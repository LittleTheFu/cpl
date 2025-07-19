#include "virtualMachine.h"
#include "instruction.h"
#include <sstream>

VirtualMachine::VirtualMachine()
{
    memorySize_ = 1024;
    memory_.resize(memorySize_);

    registerSize_ = 8;
    register_.resize(registerSize_);

    stackBaseAddress_ = memorySize_ - 1;
    stackLimitAddress_ = stackBaseAddress_ - 10;
    stackPointer_ = stackBaseAddress_;

    programCounter_ = 0;

    sourceCode_ = "MOV R0 1\n"
                  "MOV R1 10\n"
                  "MOV R2 20\n"
                  "ADD R1 R2\n"
                  "MUL R0 R1\n"
                  "DIV R0 2\n";
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
        executeInstruction(instructions_[programCounter_]);
    }
}

void VirtualMachine::executeInstruction(const Instruction &instruction)
{
    switch (instruction.getOpCode())
    {
    case OpCode::NOP:
        executeNOP(instruction);
        break;
    case OpCode::MOV:
        executeMOV(instruction);
        break;
    case OpCode::ADD:
        executeADD(instruction);
        break;
    case OpCode::SUB:
        executeSUB(instruction);
        break;
    case OpCode::MUL:
        executeMUL(instruction);
        break;
    case OpCode::DIV:
        executeDIV(instruction);
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

void VirtualMachine::executeMOV(const Instruction &instruction)
{
    Operand operand1 = instruction.getOperandFirst();
    Operand operand2 = instruction.getOperandSecond();

    if (operand1.type != OperandType::REGISTER && operand1.type != OperandType::MEMORY)
    {
        throw std::runtime_error("Error operand type");
    }

    int value = getOperandValue(operand2);
    setOperandValue(operand1, value);

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

void VirtualMachine::executeSUB(const Instruction &instruction)
{
    Operand operand1 = instruction.getOperandFirst();
    Operand operand2 = instruction.getOperandSecond();

    if (operand1.type != OperandType::REGISTER && operand1.type != OperandType::MEMORY)
    {
        throw std::runtime_error("Error operand type");
    }

    int value1 = getOperandValue(operand1);
    int value2 = getOperandValue(operand2);
    int result = value1 - value2;
    setOperandValue(operand1, result);
}

void VirtualMachine::executeMUL(const Instruction &instruction)
{
    Operand operand1 = instruction.getOperandFirst();
    Operand operand2 = instruction.getOperandSecond();

    if (operand1.type != OperandType::REGISTER && operand1.type != OperandType::MEMORY)
    {
        throw std::runtime_error("Error operand type");
    }

    int value1 = getOperandValue(operand1);
    int value2 = getOperandValue(operand2);
    int result = value1 * value2;
    setOperandValue(operand1, result);
}

void VirtualMachine::executeDIV(const Instruction &instruction)
{
    Operand operand1 = instruction.getOperandFirst();
    Operand operand2 = instruction.getOperandSecond();

    if (operand1.type != OperandType::REGISTER && operand1.type != OperandType::MEMORY)
    {
        throw std::runtime_error("Error operand type");
    }

    int value1 = getOperandValue(operand1);
    int value2 = getOperandValue(operand2);

    if(value2 == 0)
    {
        throw std::runtime_error("Error divide by zero");
    }

    int result = value1 / value2;
    setOperandValue(operand1, result);
}

void VirtualMachine::executePUSH(const Instruction &instruction)
{
    Operand operand = instruction.getOperandFirst();
    int value = getOperandValue(operand);

    if (stackPointer_ > stackLimitAddress_)
    {
        memory_[stackPointer_] = value;
        stackPointer_--;
    }
    else
    {
        throw std::runtime_error("Error stack overflow");
    }
}

void VirtualMachine::executePOP(const Instruction &instruction)
{
    Operand operand = instruction.getOperandFirst();

    if (stackPointer_ < stackBaseAddress_)
    {
        stackPointer_++;
        setOperandValue(operand, memory_[stackPointer_]);
    }
    else
    {
        throw std::runtime_error("Error stack underflow");
    }
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
}

void VirtualMachine::printStack() const
{
    std::cout << "Stack: ";
    for (size_t i = stackPointer_; i < stackBaseAddress_; i++)
    {
        std::cout << memory_[i + 1] << " ";
    }
    std::cout << std::endl;
}

void VirtualMachine::printRegister() const
{
    std::cout << "Register: ";
    for (size_t i = 0; i < registerSize_; i++)
    {
        std::cout << register_[i] << " ";
    }
    std::cout << std::endl;
}
