#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include <string>
#include <iostream>

enum class OpCode
{
    NOP,
    PUSH,
    POP,
    ADD,
    SUB,
    MUL,
    DIV,
    JMP,
    JMPZ,
    JMPNZ,
    CALL,
    RET,
    HLT,

    UNKNOWN,
};

std::ostream & operator<<(std::ostream & os, const OpCode & opCode);

enum class OperandType
{
    REGISTER,
    MEMORY,
    IMMEDIATE,

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

    void readInstruction(const std::string &line);

    friend std::ostream &operator<<(std::ostream &os, const Instruction &instruction);

    OpCode getOpCode() const { return opCode_; }
    Operand getOperandFirst() const { return operandFirst_; }
    Operand getOperandSecond() const { return operandSecond_; }

private:
    std::string trim(const std::string &str);
    OpCode stringToOpCode(const std::string &str);
    Operand stringToOperand(const std::string &str);

private:
    OpCode opCode_;

    Operand operandFirst_;
    Operand operandSecond_;
};

#endif // _INSTRUCTION_H_