#include "instruction.h"
#include <sstream>

Instruction::Instruction()
{
}

void Instruction::readInstruction(const std::string &line)
{
    std::string lineTrim = trim(line);

    std::stringstream ss(lineTrim);
    std::string opCodeStr;
    ss >> opCodeStr;
    opCode_ = stringToOpCode(opCodeStr);

    std::string operandStr;
    ss >> operandStr;
    operandFirst_ = stringToOperand(operandStr);
    ss >> operandStr;
    operandSecond_ = stringToOperand(operandStr);
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

Operand Instruction::stringToOperand(const std::string &str)
{
    Operand operand;
    if (str[0] == 'R')
    {
        operand.type = OperandType::REGISTER;
        operand.value = std::stoi(str.substr(1));
    }
    else if (str[0] == '[') //Memory
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
