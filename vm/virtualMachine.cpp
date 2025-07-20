#include "virtualMachine.h"
#include "instruction.h"
#include <sstream>

VirtualMachine::VirtualMachine()
{
    memorySize_ = 1024;
    memory_.resize(memorySize_);

    registerSize_ = 16;
    register_.resize(registerSize_);

    stackBaseAddress_ = memorySize_ - 1;
    stackLimitAddress_ = stackBaseAddress_ - 10;
    stackPointer_ = stackBaseAddress_;

    programCounter_ = 0;

    zeroFlag_ = false;
    signFlag_ = false;

    isRunning_ = false;

    sourceCode_ = "MOV R0 0\n"  // R0 = 0 (用于循环累加)
                  "MOV R1 10\n" // R1 = 10 (循环次数，从10递减到0)
                  "MOV R2 2\n"  // R2 = 2 (循环每次递增值)
                  "MOV R3 0\n"  // R3 = 0 (一个辅助寄存器)
                  "MOV R4 0\n"  // R4 = 0 (用于测试不相等/大于)
                  "MOV R5 5\n"  // R5 = 5 (用于测试小于/大于)
                  "MOV R6 5\n"  // R6 = 5 (用于测试相等)

                  "MOV [0] 100\n" // Memory[0] = 100
                  "MOV [1] 200\n" // Memory[1] = 200

                  // --- Part 1: Basic Arithmetic and Flags Update ---
                  "ADD R0 R2\n" // R0 = R0 + R2 (0 + 2 = 2). ZF=F, SF=F
                  "SUB R1 R2\n" // R1 = R1 - R2 (10 - 2 = 8). ZF=F, SF=F
                  "HLT\n"
                  "MUL R0 R2\n" // R0 = R0 * R2 (2 * 2 = 4). ZF=F, SF=F
                  "DIV R0 2\n"  // R0 = R0 / 2 (4 / 2 = 2). ZF=F, SF=F

                  // --- Part 2: Stack Operations ---
                  "PUSH R0\n" // Push R0 (2)
                  "PUSH R1\n" // Push R1 (8)
                  "POP R7\n"  // R7 = 8
                  "POP R8\n"  // R8 = 2

                  // --- Part 3: JMP (Unconditional Jump) ---
                  "JMP skip_code_block\n" // 无条件跳过下一段
                  "ADD R9 999\n"          // R9 = 999 (should be skipped)

                  "skip_code_block:\n" // JMP 目标

                  // --- Part 4: Conditional Jumps (using CMP and various conditions) ---
                  // Test JMPZ / JMPNZ
                  "CMP R3 0\n"              // R3=0, 比较R3和0. 结果=0. ZF=T, SF=F
                  "JMPZ check_jmpnz_test\n" // 应该跳转
                  "ADD R9 1\n"              // (should be skipped if JMPZ works)

                  "check_jmpnz_test:\n"
                  "CMP R4 0\n"           // R4=0, 比较R4和0. 结果=0. ZF=T, SF=F
                  "JMPNZ failed_jmpnz\n" // 不应该跳转 (ZF=T)
                  "ADD R9 2\n"           // R9 = 2 (JMPNZ 失败，执行这里)
                  "JMP next_cmp_test\n"  // 跳过 failed_jmpnz
                  "failed_jmpnz:\n"      // 如果 JMPNZ 错误，会跳到这里
                  "MOV R9 9999\n"        // 标记错误

                  "next_cmp_test:\n"
                  // Test JMPL / JMPLE
                  "CMP R4 R5\n"       // R4=0, R5=5. 0 - 5 = -5. ZF=F, SF=T
                  "JMPL r4_less_r5\n" // 应该跳转 (SF=T)
                  "ADD R9 4\n"        // (should be skipped)

                  "r4_less_r5:\n"
                  "ADD R9 8\n" // R9 = 2 + 8 = 10 (JMPL 成功，执行这里)

                  "CMP R5 R4\n"        // R5=5, R4=0. 5 - 0 = 5. ZF=F, SF=F
                  "JMPL failed_jmpl\n" // 不应该跳转
                  "ADD R9 16\n"        // R9 = 10 + 16 = 26 (JMPL 失败，执行这里)
                  "JMP next_test_group\n"
                  "failed_jmpl:\n"
                  "MOV R9 9999\n" // 标记错误

                  "next_test_group:\n"
                  // Test JMPG / JMPGE
                  "CMP R5 R4\n"          // R5=5, R4=0. 5 - 0 = 5. ZF=F, SF=F
                  "JMPG r5_greater_r4\n" // 应该跳转
                  "ADD R9 32\n"          // (should be skipped)

                  "r5_greater_r4:\n"
                  "ADD R9 64\n" // R9 = 26 + 64 = 90 (JMPG 成功，执行这里)

                  "CMP R4 R5\n"        // R4=0, R5=5. 0 - 5 = -5. ZF=F, SF=T
                  "JMPG failed_jmpg\n" // 不应该跳转
                  "ADD R9 128\n"       // R9 = 90 + 128 = 218 (JMPG 失败，执行这里)
                  "JMP next_jmpge_test\n"
                  "failed_jmpg:\n"
                  "MOV R9 9999\n" // 标记错误

                  "next_jmpge_test:\n"
                  // Test JMPLE
                  "CMP R5 R6\n"              // R5=5, R6=5. 5 - 5 = 0. ZF=T, SF=F
                  "JMPLE r5_less_equal_r6\n" // 应该跳转 (ZF=T)
                  "ADD R9 256\n"             // (should be skipped)

                  "r5_less_equal_r6:\n"
                  "ADD R9 512\n" // R9 = 218 + 512 = 730 (JMPLE 成功，执行这里)

                  // Test JMPGE
                  "CMP R5 R6\n"                 // R5=5, R6=5. 5 - 5 = 0. ZF=T, SF=F
                  "JMPGE r5_greater_equal_r6\n" // 应该跳转 (ZF=T)
                  "ADD R9 1024\n"               // (should be skipped)

                  "r5_greater_equal_r6:\n"
                  "ADD R9 2048\n" // R9 = 730 + 2048 = 2778 (JMPGE 成功，执行这里)

                  // --- Part 5: A Simple Loop Test (counting down) ---
                  // R1 初始值是 10-2=8 (从上面的 SUB R1 R2 继承)
                  // 目标：R0 从 2 开始，R2 每次加到 R0。循环 R1 次。
                  // 循环结束后 R0 应该是 2 (初始) + 8 (R1) * 2 (R2) = 2 + 16 = 18
                  "MOV R0 2\n" // 重置 R0 为 2
                  "MOV R1 8\n" // 重置 R1 为 8 (循环次数)
                  "MOV R2 2\n" // R2 = 2 (增量)

                  "loop_test:\n"
                  "ADD R0 R2\n"       // R0 += R2
                  "SUB R1 1\n"        // R1 --
                  "CMP R1 0\n"        // 比较 R1 和 0
                  "JMPNZ loop_test\n" // 如果 R1 不为 0，继续循环

                  // Loop finished, R0 should be 18, R1 should be 0.

                  // --- Final Verification of Jumps & R9 ---
                  "MOV R9 100\n" // R9重置为100，用于后续验证
                  "CMP R3 0\n"   // R3=0. ZF=T, SF=F
                  "JMPZ confirm_jmpz\n"
                  "MOV R9 0\n" // Failed. If JMPZ didn't work.

                  "confirm_jmpz:\n"
                  "MOV R9 200\n" // R9=200. This confirms JMPZ worked.

                  "CMP R4 R5\n" // R4=0, R5=5. 0-5=-5. ZF=F, SF=T
                  "JMPL confirm_jmpl\n"
                  "MOV R9 0\n" // Failed. If JMPL didn't work.

                  "confirm_jmpl:\n"
                  "ADD R9 200\n" // R9=200+200=400. This confirms JMPL worked.

                  // Final check for JMPGE
                  "CMP R5 R6\n" // R5=5, R6=5. 5-5=0. ZF=T, SF=F
                  "JMPGE confirm_jmpge\n"
                  "MOV R9 0\n" // Failed. If JMPGE didn't work.

                  "confirm_jmpge:\n"
                  "ADD R9 200\n" // R9=400+200=600. This confirms JMPGE worked.

                  // Final check of JMPNZ
                  "MOV R10 1\n" // R10 = 1
                  "CMP R10 0\n" // 1 - 0 = 1. ZF=F, SF=F
                  "JMPNZ confirm_jmpnz\n"
                  "MOV R9 0\n" // This means JMPNZ failed
                  "confirm_jmpnz:\n"
                  "ADD R9 100\n" // R9 = 600 + 100 = 700. Confirms JMPNZ worked.

                  // Final check of JMPG
                  "MOV R10 10\n"  // R10 = 10
                  "MOV R11 5\n"   // R11 = 5
                  "CMP R10 R11\n" // 10 - 5 = 5. ZF=F, SF=F
                  "JMPG confirm_jmpg\n"
                  "MOV R9 0\n" // This means JMPG failed
                  "confirm_jmpg:\n"
                  "ADD R9 100\n" // R9 = 700 + 100 = 800. Confirms JMPG worked.

                  // Final check of JMPLE
                  "MOV R10 5\n"   // R10 = 5
                  "MOV R11 10\n"  // R11 = 10
                  "CMP R10 R11\n" // 5 - 10 = -5. ZF=F, SF=T
                  "JMPLE confirm_jmple\n"
                  "MOV R9 0\n" // This means JMPLE failed
                  "confirm_jmple:\n"
                  "ADD R9 100\n" // R9 = 800 + 100 = 900. Confirms JMPLE worked.

                  // --- Program End ---
                  "MOV R15 999\n"; // Just a final instruction to indicate end
}

void VirtualMachine::loadProgram()
{
    clear();

    buildLabelMap();
    assembleInstructions();
}

void VirtualMachine::execute()
{
    isRunning_ = true;

    for (programCounter_ = 0; programCounter_ < instructions_.size(); programCounter_++)
    {
        if(!isRunning_)
            break;

        executeInstruction(instructions_[programCounter_]);

        std::cout << "PC: " << programCounter_ << " | " << instructions_[programCounter_] << std::endl;
        printStack();
        printRegister();
        printMemory();
        printFlags();
        std::cout << "------------------------" << std::endl;
    }
}

void VirtualMachine::clear()
{
    instructions_.clear();
    labelMap_.clear();

    // for (auto &m : memory_)
    // {
    //     m = 0;
    // }

    // for (auto &r : register_)
    // {
    //     r = 0;
    // }

    stackPointer_ = stackBaseAddress_;

    programCounter_ = 0;
}

void VirtualMachine::setRegister(size_t index, int value)
{
    register_.at(index) = value;
}

int VirtualMachine::getRegister(size_t index) const
{
    return register_.at(index);
}

void VirtualMachine::setMemory(size_t index, int value)
{
    memory_.at(index) = value;
}

int VirtualMachine::getMemory(size_t index) const
{
    return memory_.at(index);
}

bool VirtualMachine::getZeroFlag() const
{
    return zeroFlag_;
}

bool VirtualMachine::getSignFlag() const
{
    return signFlag_;
}

void VirtualMachine::setSourceCode(const std::string &sourceCode)
{
    sourceCode_ = sourceCode;
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
    case OpCode::JMPZ:
        executeJMPZ(instruction);
        break;
    case OpCode::JMPL:
        executeJMPL(instruction);
        break;
    case OpCode::JMPGE:
        executeJMPGE(instruction);
        break;
    case OpCode::JMPNZ:
        executeJMPNZ(instruction);
        break;
    case OpCode::JMPG:
        executeJMPG(instruction);
        break;
    case OpCode::JMPLE:
        executeJMPLE(instruction);
        break;
    case OpCode::HLT:
        executeHLT(instruction);
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

void VirtualMachine::executeJMPZ(const Instruction &instruction)
{
    if (zeroFlag_)
    {
        executeJMP(instruction);
    }
}

void VirtualMachine::executeJMPNZ(const Instruction &instruction)
{
    if (!zeroFlag_)
    {
        executeJMP(instruction);
    }
}

void VirtualMachine::executeJMPL(const Instruction &instruction)
{
    if (signFlag_)
    {
        executeJMP(instruction);
    }
}

void VirtualMachine::executeJMPLE(const Instruction &instruction)
{
    if (signFlag_ || zeroFlag_)
    {
        executeJMP(instruction);
    }
}

void VirtualMachine::executeJMPG(const Instruction &instruction)
{
    if (!signFlag_ && !zeroFlag_)
    {
        executeJMP(instruction);
    }
}

void VirtualMachine::executeJMPGE(const Instruction &instruction)
{
    if (!signFlag_)
    {
        executeJMP(instruction);
    }
}

void VirtualMachine::executeHLT(const Instruction &instruction)
{
    isRunning_ = false;
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

void VirtualMachine::printMemory(size_t start, size_t end) const
{
    std::cout << "Memory: ";
    for (size_t i = start; i < end; i++)
    {
        std::cout << memory_[i] << " ";
    }
    std::cout << std::endl;
}
