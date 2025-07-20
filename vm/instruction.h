#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include <string>
#include <iostream>
#include <map>

enum class OpCode
{
    NOP,
    MOV,
    PUSH,
    POP,

    ADD,
    SUB,
    MUL,
    DIV,

    CMP,

    JMP,
    JMPZ,
    JMPNZ,
    JMPL,
    JMPLE,
    JMPG,
    JMPGE,

    CALL,
    RET,
    HLT,

    UNKNOWN,
};

std::ostream & operator<<(std::ostream & os, const OpCode & opCode);
size_t getOpCodeParameterCount(OpCode opCode);

enum class OperandType
{
    REGISTER,
    MEMORY,
    IMMEDIATE,
    LABEL,

    UNKNOWN,
};
std::ostream & operator<<(std::ostream & os, const OperandType & operandType);

struct Operand
{
    OperandType type;
    int value;
};
std::ostream & operator<<(std::ostream & os, const Operand & operand);

class Instruction
{
public:
    Instruction();
    ~Instruction() = default;

    void readInstruction(const std::string &line, const std::map<std::string, size_t>& labelMap);
    bool isLabelDefinition(const std::string& line, std::string& outLabelName);
    bool isPureLabelLine(const std::string& trimmedLine);

    friend std::ostream &operator<<(std::ostream &os, const Instruction &instruction);

    void setOpCode(OpCode opCode) { opCode_ = opCode; }

    OpCode getOpCode() const { return opCode_; }
    Operand getOperandFirst() const { return operandFirst_; }
    Operand getOperandSecond() const { return operandSecond_; }

private:
    std::string trim(const std::string &str);
    OpCode stringToOpCode(const std::string &str);
    Operand stringToOperand(const std::string &str, const std::map<std::string, size_t>& labelMap);

private:
    OpCode opCode_;

    Operand operandFirst_;
    Operand operandSecond_;
};

#endif // _INSTRUCTION_H_