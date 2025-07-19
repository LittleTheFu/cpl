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

    zeroFlag_ = false;
    signFlag_ = false;

    sourceCode_ = "MOV R0 0\n"  // Instruction 0: R0 = 0
                  "MOV R1 10\n" // Instruction 1: R1 = 10
                  "MOV R2 20\n" // Instruction 2: R2 = 20
                  "MOV R3 5\n"  // Instruction 3: R3 = 5

                  "MOV [0] 100\n" // Instruction 4: Memory[0] = 100
                  "MOV [1] 200\n" // Instruction 5: Memory[1] = 200

                  "label_arith:\n" // Instruction 6 (pure label)
                  "ADD R1 R2\n"    // Instruction 7: R1 = R1 + R2 (10 + 20 = 30)
                  "SUB R2 R3\n"    // Instruction 8: R2 = R2 - R3 (20 - 5 = 15)
                  "MUL R0 R3\n"    // Instruction 9: R0 = R0 * R3 (0 * 5 = 0)

                  "label_stack:\n" // Instruction 10 (pure label)
                  "PUSH R1\n"      // Instruction 11: Push R1 (30) to stack
                  "PUSH R0\n"      // Instruction 12: Push R0 (0) to stack

                  "MOV R4 [0]\n" // Instruction 13: R4 = Memory[0] (100)
                  "PUSH R4\n"    // Instruction 14: Push R4 (100) to stack

                  "POP R5\n" // Instruction 15: Pop from stack into R5 (R5 = 100)
                  "POP R6\n" // Instruction 16: Pop from stack into R6 (R6 = 0)
                  "POP R7\n" // Instruction 17: Pop from stack into R7 (R7 = 30)

                  "JMP end_of_test\n" // Instruction 18: Jump to 'end_of_test'

                  "skipped_section:\n" // Instruction 19 (pure label)
                  "DIV R0 2\n"         // Instruction 20: This instruction will be skipped
                  "ADD R8 100\n"       // Instruction 21: This instruction will be skipped

                  "end_of_test:\n" // Instruction 22 (pure label)
                  "DIV R5 2\n"     // Instruction 23: R5 = R5 / 2 (100 / 2 = 50)
                  "MOV [0] R5\n";  // Instruction 24: Memory[0] = R5 (Memory[0] = 50)
}

void VirtualMachine::loadProgram()
{
    buildLabelMap();
    assembleInstructions();
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
    case OpCode::CMP:
        executeCMP(instruction);
        break;
    case OpCode::JMP:
        executeJMP(instruction);
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

    updateFlags(result);
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

    updateFlags(result);
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

    updateFlags(result);
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

    if (value2 == 0)
    {
        throw std::runtime_error("Error divide by zero");
    }

    int result = value1 / value2;
    setOperandValue(operand1, result);

    updateFlags(result);
}

void VirtualMachine::executeCMP(const Instruction &instruction)
{
    Operand operand1 = instruction.getOperandFirst();
    Operand operand2 = instruction.getOperandSecond();

    int value1 = getOperandValue(operand1);
    int value2 = getOperandValue(operand2);

    updateFlags(value1 - value2);
}

void VirtualMachine::executeJMP(const Instruction &instruction)
{
    Operand operand = instruction.getOperandFirst();
    int value = getOperandValue(operand);
    programCounter_ = value;
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

void VirtualMachine::buildLabelMap()
{
    std::stringstream ss(sourceCode_);
    std::string line;

    size_t lineNumber = 0;

    while (std::getline(ss, line))
    {
        Instruction instruction;
        std::string labelName;
        if (instruction.isLabelDefinition(line, labelName))
        {
            // ok,i'm lazy...so label name cannot be start with 'R' or '['
            // which stands for register or memory
            char firstCharacter = labelName[0];
            if (firstCharacter == 'R')
            {
                throw std::runtime_error("label cannot be start with R which stands for register");
            }

            if (firstCharacter == '[')
            {
                throw std::runtime_error("label cannot be start with [ which stands for memory");
            }

            if (labelMap_.count(labelName))
            {
                throw std::runtime_error("Error label duplicate");
            }

            labelMap_[labelName] = lineNumber;
        }

        lineNumber++;
    }
}

void VirtualMachine::assembleInstructions()
{
    std::stringstream ss(sourceCode_);
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(ss, line))
    {
        Instruction instruction;

        std::string labelName;
        if (instruction.isLabelDefinition(line, labelName))
        {
            instruction.setOpCode(OpCode::NOP);
        }
        else
        {
            instruction.readInstruction(line, labelMap_);
        }

        instructions_.push_back(instruction);
        std::cout << lineNumber << " " << instruction << std::endl;

        lineNumber++;
    }
}

void VirtualMachine::updateFlags(int value)
{
    zeroFlag_ = (value == 0);
    signFlag_ = (value < 0);
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

void VirtualMachine::printFlags() const
{
    std::cout << "Flags: ";
    std::cout << "Zero: " << zeroFlag_ << " ";
    std::cout << "Sign: " << signFlag_ << " ";
    std::cout << std::endl;
}
