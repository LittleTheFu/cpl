#include "instruction.h"
#include <sstream>

Instruction::Instruction()
{
}

void Instruction::readInstruction(const std::string &line, const std::map<std::string, size_t> &labelMap)
{
    std::string lineTrim = trim(line);

    std::stringstream ss(lineTrim);
    std::string opCodeStr;
    ss >> opCodeStr;
    opCode_ = stringToOpCode(opCodeStr);

    std::string operandStr;
    ss >> operandStr;
    operandFirst_ = stringToOperand(operandStr, labelMap);
    ss >> operandStr;
    operandSecond_ = stringToOperand(operandStr, labelMap);
}

std::string Instruction::trim(const std::string &str)
{
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first)
    {
        return "";
    }

    size_t last = str.find_last_not_of(" \t\n\r");

    return str.substr(first, (last - first + 1));
}

OpCode Instruction::stringToOpCode(const std::string &str)
{
    if (str == "NOP")
    {
        return OpCode::NOP;
    }
    else if (str == "MOV")
    {
        return OpCode::MOV;
    }
    else if (str == "PUSH")
    {
        return OpCode::PUSH;
    }
    else if (str == "POP")
    {
        return OpCode::POP;
    }
    else if (str == "ADD")
    {
        return OpCode::ADD;
    }
    else if (str == "SUB")
    {
        return OpCode::SUB;
    }
    else if (str == "MUL")
    {
        return OpCode::MUL;
    }
    else if (str == "DIV")
    {
        return OpCode::DIV;
    }
    else if (str == "JMP")
    {
        return OpCode::JMP;
    }
    else if (str == "JMPZ")
    {
        return OpCode::JMPZ;
    }
    else if (str == "JMPNZ")
    {
        return OpCode::JMPNZ;
    }
    else if (str == "CALL")
    {
        return OpCode::CALL;
    }
    else if (str == "RET")
    {
        return OpCode::RET;
    }
    else if (str == "HLT")
    {
        return OpCode::HLT;
    }

    throw std::runtime_error("Error parsing opCode '" + str + "'");

    return OpCode::NOP;
}

Operand Instruction::stringToOperand(const std::string &str, const std::map<std::string, size_t>& labelMap)
{
    Operand operand;

    //handle JMP label first
    for(auto& label : labelMap)
    {
        if (str == label.first)
        {
            operand.type = OperandType::IMMEDIATE;
            operand.value = static_cast<int>(label.second);
            return operand;
        }
    }

    if (str[0] == 'R')
    {
        operand.type = OperandType::REGISTER;
        operand.value = std::stoi(str.substr(1));
    }
    else if (str[0] == '[') // Memory
    {
        operand.type = OperandType::MEMORY;
        operand.value = std::stoi(str.substr(1, str.size() - 2));
    }
    else
    {
        operand.type = OperandType::IMMEDIATE;
        operand.value = std::stoi(str);
    }

    return operand;
}

std::ostream &operator<<(std::ostream &os, const OpCode &opCode)
{
    switch (opCode)
    {
    case OpCode::NOP:
        os << "NOP";
        break;
    case OpCode::MOV:
        os << "MOV";
        break;
    case OpCode::PUSH:
        os << "PUSH";
        break;
    case OpCode::POP:
        os << "POP";
        break;
    case OpCode::ADD:
        os << "ADD";
        break;
    case OpCode::SUB:
        os << "SUB";
        break;
    case OpCode::MUL:
        os << "MUL";
        break;
    case OpCode::DIV:
        os << "DIV";
        break;
    case OpCode::JMP:
        os << "JMP";
        break;
    case OpCode::JMPZ:
        os << "JMPZ";
        break;
    case OpCode::JMPNZ:
        os << "JMPNZ";
        break;
    case OpCode::CALL:
        os << "CALL";
        break;
    case OpCode::RET:
        os << "RET";
        break;
    case OpCode::HLT:
        os << "HLT";
        break;
    case OpCode::UNKNOWN:
        os << "UNKNOWN_OP";
        break;
    default:
        os << "UNKNOWN_OP";
        break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const OperandType &operandType)
{
    switch (operandType)
    {
    case OperandType::REGISTER:
        os << "REGISTER";
        break;
    case OperandType::MEMORY:
        os << "MEMORY";
        break;
    case OperandType::IMMEDIATE:
        os << "IMMEDIATE";
        break;
    case OperandType::LABEL:
        os << "LABEL";
        break;
    case OperandType::UNKNOWN:
        os << "UNKNOWN";
        break;
    default:
        os << "<UNHANDLED_OPERAND_TYPE>";
        break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const Operand &operand)
{
    switch (operand.type)
    {
    case OperandType::REGISTER:
        os << "R" << operand.value;
        break;
    case OperandType::MEMORY:
        os << "[" << operand.value << "]";
        break;
    case OperandType::IMMEDIATE:
        os << "#" << operand.value;
        break;
    default:
        os << "<UNKNOWN_OPERAND>";
        break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const Instruction &instruction)
{
    os << instruction.opCode_;

    if(instruction.opCode_ == OpCode::NOP)
    {
        return os;
    }

    if (instruction.operandFirst_.type != OperandType::UNKNOWN)
    {
        os << " " << instruction.operandFirst_;
    }
    else if (instruction.operandFirst_.type == OperandType::UNKNOWN)
    {
        os << " <INVALID_OP1>";
    }

    if(instruction.opCode_ == OpCode::JMP || instruction.opCode_ == OpCode::PUSH || instruction.opCode_ == OpCode::POP)
    {
        return os;
    }

    if (instruction.operandSecond_.type != OperandType::UNKNOWN)
    {
        os << ", " << instruction.operandSecond_;
    }
    else if (instruction.operandSecond_.type == OperandType::UNKNOWN)
    {
        os << ", <INVALID_OP2>";
    }

    return os;
}

bool Instruction::isLabelDefinition(const std::string &line, std::string &outLabelName)
{
    if (line.empty())
    {
        return false;
    }

    size_t colonPos = line.find(':');
    if (colonPos != std::string::npos)
    {
        outLabelName = trim(line.substr(0, colonPos));
        if (outLabelName.empty())
        {
            throw std::runtime_error("Assembler Error: Label name cannot be empty for line: '" + line + "'");
        }
        return true;
    }
    return false;
}

bool Instruction::isPureLabelLine(const std::string &trimmedLine)
{
    size_t colonPos = trimmedLine.find(':');
    if (colonPos != std::string::npos)
    {
        std::string remainingLine = trim(trimmedLine.substr(colonPos + 1));
        return remainingLine.empty();
    }
    return false;
}
