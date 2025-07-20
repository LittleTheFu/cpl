#include "virtualMachine.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <stdexcept>

class VirtualMachineTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        vm = new VirtualMachine();
        vm->setSourceCode("");
    }

    void TearDown() override
    {
        delete vm;
        vm = nullptr;
    }

    void RunCode(const std::string &code)
    {
        vm->setSourceCode(code);
        vm->loadProgram();
        vm->execute();
    }

    void SetRegister(size_t reg_index, int value)
    {
        vm->setRegister(reg_index, value);
    }

    int GetRegister(size_t reg_index)
    {
        return vm->getRegister(reg_index);
    }

    void SetMemory(size_t addr, int value)
    {
        vm->setMemory(addr, value);
    }

    int GetMemory(size_t addr)
    {
        return vm->getMemory(addr);
    }

    bool GetZeroFlag() const
    {
        return vm->getZeroFlag();

    }

    bool GetSignFlag() const
    {
        return vm->getSignFlag();
    }

    VirtualMachine *vm;
};

TEST_F(VirtualMachineTest, MOV_RegisterToRegister)
{
    RunCode("MOV R0 10\nMOV R1 R0\n");
    EXPECT_EQ(GetRegister(0), 10);
    EXPECT_EQ(GetRegister(1), 10);
}

TEST_F(VirtualMachineTest, MOV_ImmediateToRegister)
{
    RunCode("MOV R0 123\n");
    EXPECT_EQ(GetRegister(0), 123);
}

TEST_F(VirtualMachineTest, MOV_RegisterToMemory)
{
    SetRegister(0, 500);
    RunCode("MOV [0] R0\n");
    EXPECT_EQ(GetMemory(0), 500);
}

TEST_F(VirtualMachineTest, MOV_MemoryToRegister)
{
    SetMemory(1, 789);
    RunCode("MOV R2 [1]\n");
    EXPECT_EQ(GetRegister(2), 789);
}

TEST_F(VirtualMachineTest, ADD_PositiveResult)
{
    SetRegister(0, 5);
    SetRegister(1, 3);
    RunCode("ADD R0 R1\n");
    EXPECT_EQ(GetRegister(0), 8);
    EXPECT_FALSE(GetZeroFlag());
    EXPECT_FALSE(GetSignFlag());
}

TEST_F(VirtualMachineTest, SUB_ZeroResult)
{
    SetRegister(0, 10);
    SetRegister(1, 10);
    RunCode("SUB R0 R1\n");
    EXPECT_EQ(GetRegister(0), 0);
    EXPECT_TRUE(GetZeroFlag());
    EXPECT_FALSE(GetSignFlag());
}

TEST_F(VirtualMachineTest, MUL_NegativeResult)
{
    SetRegister(0, 5);
    SetRegister(1, -2);
    RunCode("MUL R0 R1\n");
    EXPECT_EQ(GetRegister(0), -10);
    EXPECT_FALSE(GetZeroFlag());
    EXPECT_TRUE(GetSignFlag());
}

TEST_F(VirtualMachineTest, DIV_PositiveResult)
{
    SetRegister(0, 10);
    SetRegister(1, 3);
    RunCode("DIV R0 R1\n");
    EXPECT_EQ(GetRegister(0), 3);
    EXPECT_FALSE(GetZeroFlag());
    EXPECT_FALSE(GetSignFlag());
}

TEST_F(VirtualMachineTest, DIV_DivideByZeroThrowsError)
{
    SetRegister(0, 10);
    SetRegister(1, 0);
    EXPECT_THROW(RunCode("DIV R0 R1\n"), std::runtime_error);
}

TEST_F(VirtualMachineTest, PUSH_POP)
{
    SetRegister(0, 100);
    SetRegister(1, 200);
    RunCode("PUSH R0\nPUSH R1\nPOP R2\nPOP R3\n");
    EXPECT_EQ(GetRegister(2), 200);
    EXPECT_EQ(GetRegister(3), 100);
}

TEST_F(VirtualMachineTest, PUSH_StackOverflow)
{
    std::string code = "";
    for (int i = 0; i < 11; ++i)
    {
        code += "PUSH R0\n";
    }
    SetRegister(0, 1);
    EXPECT_THROW(RunCode(code), std::runtime_error);
}

TEST_F(VirtualMachineTest, POP_StackUnderflow)
{
    EXPECT_THROW(RunCode("POP R0\n"), std::runtime_error);
}

TEST_F(VirtualMachineTest, CMP_Equal)
{
    SetRegister(0, 5);
    SetRegister(1, 5);
    RunCode("CMP R0 R1\n");
    EXPECT_TRUE(GetZeroFlag());
    EXPECT_FALSE(GetSignFlag());
}

TEST_F(VirtualMachineTest, CMP_LessThan)
{
    SetRegister(0, 3);
    SetRegister(1, 5);
    RunCode("CMP R0 R1\n");
    EXPECT_FALSE(GetZeroFlag());
    EXPECT_TRUE(GetSignFlag());
}

TEST_F(VirtualMachineTest, CMP_GreaterThan)
{
    SetRegister(0, 7);
    SetRegister(1, 5);
    RunCode("CMP R0 R1\n");
    EXPECT_FALSE(GetZeroFlag());
    EXPECT_FALSE(GetSignFlag());
}

TEST_F(VirtualMachineTest, JMP_Unconditional)
{
    RunCode("MOV R0 0\nJMP target_label\nMOV R0 999\ntarget_label:\nMOV R0 100\n");
    EXPECT_EQ(GetRegister(0), 100);
}

TEST_F(VirtualMachineTest, JMPZ)
{
    RunCode("MOV R0 10\n"
            "CMP R0 10\n"
            "JMPZ target_jmpz\n"
            "MOV R0 0\n"
            "target_jmpz:\n"
            "MOV R0 1\n");
    EXPECT_EQ(GetRegister(0), 1);

    RunCode("MOV R0 10\n"
            "CMP R0 5\n"
            "JMPZ target_jmpz2\n"
            "MOV R0 0\n"
            "JMP end_jmpz2_test\n"
            "target_jmpz2:\n"
            "MOV R0 999\n"
            "end_jmpz2_test:\n");
    EXPECT_EQ(GetRegister(0), 0);
}

TEST_F(VirtualMachineTest, JMPNZ)
{
    RunCode("MOV R0 10\n"
            "CMP R0 5\n"
            "JMPNZ target_jmpnz\n"
            "MOV R0 0\n"
            "target_jmpnz:\n"
            "MOV R0 1\n");
    EXPECT_EQ(GetRegister(0), 1);

    RunCode("MOV R0 10\n"
            "CMP R0 10\n"
            "JMPNZ target_jmpnz2\n"
            "MOV R0 0\n"
            "JMP end_jmpnz2_test\n"
            "target_jmpnz2:\n"
            "MOV R0 999\n"
            "end_jmpnz2_test:\n");
    EXPECT_EQ(GetRegister(0), 0);
}

TEST_F(VirtualMachineTest, JMPL)
{
    RunCode("MOV R0 5\n"
            "MOV R1 10\n"
            "CMP R0 R1\n"
            "JMPL target_jmpl\n"
            "MOV R0 0\n"
            "target_jmpl:\n"
            "MOV R0 1\n");
    EXPECT_EQ(GetRegister(0), 1);

    RunCode("MOV R0 5\n"
            "MOV R1 5\n"
            "CMP R0 R1\n"
            "JMPL target_jmpl2\n"
            "MOV R0 0\n"
            "JMP end_jmpl2_test\n"
            "target_jmpl2:\n"
            "MOV R0 999\n"
            "end_jmpl2_test:\n");
    EXPECT_EQ(GetRegister(0), 0);

    RunCode("MOV R0 10\n"
            "MOV R1 5\n"
            "CMP R0 R1\n"
            "JMPL target_jmpl3\n"
            "MOV R0 0\n"
            "JMP end_jmpl3_test\n"
            "target_jmpl3:\n"
            "MOV R0 999\n"
            "end_jmpl3_test:\n");
    EXPECT_EQ(GetRegister(0), 0);
}


TEST_F(VirtualMachineTest, JMPLE)
{
    RunCode("MOV R0 5\n"
            "MOV R1 10\n"
            "CMP R0 R1\n"
            "JMPLE target_jmple\n"
            "MOV R0 0\n"
            "target_jmple:\n"
            "MOV R0 1\n");
    EXPECT_EQ(GetRegister(0), 1);

    RunCode("MOV R0 5\n"
            "MOV R1 5\n"
            "CMP R0 R1\n"
            "JMPLE target_jmple2\n"
            "MOV R0 0\n"
            "target_jmple2:\n"
            "MOV R0 1\n");
    EXPECT_EQ(GetRegister(0), 1);

    RunCode("MOV R0 10\n"
            "MOV R1 5\n"
            "CMP R0 R1\n"
            "JMPLE target_jmple3\n"
            "MOV R0 0\n"
            "JMP end_jmple3_test\n"
            "target_jmple3:\n"
            "MOV R0 999\n"
            "end_jmple3_test:\n");
    EXPECT_EQ(GetRegister(0), 0);
}

TEST_F(VirtualMachineTest, JMPG)
{
    RunCode("MOV R0 10\n"
            "MOV R1 5\n"
            "CMP R0 R1\n"
            "JMPG target_jmpg\n"
            "MOV R0 0\n"
            "target_jmpg:\n"
            "MOV R0 1\n");
    EXPECT_EQ(GetRegister(0), 1);

    RunCode("MOV R0 5\n"
            "MOV R1 10\n"
            "CMP R0 R1\n"
            "JMPG target_jmpg2\n"
            "MOV R0 0\n"
            "JMP end_jmpg2_test\n"
            "target_jmpg2:\n"
            "MOV R0 999\n"
            "end_jmpg2_test:\n");
    EXPECT_EQ(GetRegister(0), 0);

    RunCode("MOV R0 5\n"
            "MOV R1 5\n"
            "CMP R0 R1\n"
            "JMPG target_jmpg3\n"
            "MOV R0 0\n"
            "JMP end_jmpg3_test\n"
            "target_jmpg3:\n"
            "MOV R0 999\n"
            "end_jmpg3_test:\n");
    EXPECT_EQ(GetRegister(0), 0);
}

TEST_F(VirtualMachineTest, JMPGE)
{
    RunCode("MOV R0 10\n"
            "MOV R1 5\n"
            "CMP R0 R1\n"
            "JMPGE target_jmpge\n"
            "MOV R0 0\n"
            "target_jmpge:\n"
            "MOV R0 1\n");
    EXPECT_EQ(GetRegister(0), 1);

    RunCode("MOV R0 5\n"
            "MOV R1 5\n"
            "CMP R0 R1\n"
            "JMPGE target_jmpge2\n"
            "MOV R0 0\n"
            "target_jmpge2:\n"
            "MOV R0 1\n");
    EXPECT_EQ(GetRegister(0), 1);

    RunCode("MOV R0 5\n"
            "MOV R1 10\n"
            "CMP R0 R1\n"
            "JMPGE target_jmpge3\n"
            "MOV R0 0\n"
            "JMP end_jmpge3_test\n"
            "target_jmpge3:\n"
            "MOV R0 999\n"
            "end_jmpge3_test:\n");
    EXPECT_EQ(GetRegister(0), 0);
}

TEST_F(VirtualMachineTest, LoopCountingDown)
{
    std::string loop_code =
        "MOV R0 2\n"
        "MOV R1 8\n"
        "MOV R2 2\n"
        "loop_test:\n"
        "ADD R0 R2\n"
        "SUB R1 1\n"
        "CMP R1 0\n"
        "JMPNZ loop_test\n";

    RunCode(loop_code);
    EXPECT_EQ(GetRegister(0), 18);
    EXPECT_EQ(GetRegister(1), 0);
}

TEST_F(VirtualMachineTest, MemoryAccess)
{
    RunCode("MOV [5] 12345\nMOV R0 [5]\n");
    EXPECT_EQ(GetMemory(5), 12345);
    EXPECT_EQ(GetRegister(0), 12345);
}

TEST_F(VirtualMachineTest, Label_InvalidName_R)
{
    std::string invalid_code = "R_invalid_label:\nMOV R0 0\n";
    vm->setSourceCode(invalid_code);
    EXPECT_THROW(vm->loadProgram(), std::runtime_error);
}

TEST_F(VirtualMachineTest, Label_InvalidName_Memory)
{
    std::string invalid_code = "[invalid_label]:\nMOV R0 0\n";
    vm->setSourceCode(invalid_code);
    EXPECT_THROW(vm->loadProgram(), std::runtime_error);
}

TEST_F(VirtualMachineTest, NOP_DoesNothing)
{
    SetRegister(0, 5);
    RunCode("NOP\nMOV R1 10\nNOP\n");
    EXPECT_EQ(GetRegister(0), 5);
    EXPECT_EQ(GetRegister(1), 10);
}