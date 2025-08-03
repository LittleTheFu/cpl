#include "ffi_api.h"
#include "virtualMachine.h" // 包含 VirtualMachine 头文件
#include <iostream>
#include <vector>
#include <string>
#include <cstring> // For strlen, strcpy
#include <memory>
#include <stdexcept>
#include "workShop.h"

// 辅助函数：安全地分配并复制 C 字符串
char* allocate_and_copy_c_string(const std::string& s) {
    char* c_str = new char[s.length() + 1];
    std::strcpy(c_str, s.c_str());
    return c_str;
}

// 辅助函数：用于将 std::vector<std::string> 转换为 StringArray*
StringArray* convert_vector_to_string_array(const std::vector<std::string>& vec) {
    StringArray* result = new StringArray();
    result->count = vec.size();
    result->strings = new char*[result->count];
    for (size_t i = 0; i < result->count; ++i) {
        result->strings[i] = allocate_and_copy_c_string(vec[i]);
    }
    return result;
}

// 获取 VM 内部硬编码指令的 FFI 函数
StringArray* get_hardcoded_vm_instructions() {
    std::vector<std::string> instructions_str;
    try {
        instructions_str = WorkShop::getInstance().getVMSrc(); // 获取指令的字符串表示
        std::cout << "[C++ FFI] Successfully retrieved " << instructions_str.size() << " hardcoded VM instructions." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[C++ FFI] Error getting hardcoded VM instructions: " << e.what() << std::endl;
        return convert_vector_to_string_array({"Error: " + std::string(e.what())});
    } catch (...) {
        std::cerr << "[C++ FFI] Unknown error getting hardcoded VM instructions." << std::endl;
        return convert_vector_to_string_array({"Error: Unknown error."});
    }
    return convert_vector_to_string_array(instructions_str);
}

// 释放内存的函数（不变）
void free_string_array(StringArray* array) {
    if (array == nullptr) {
        return;
    }
    std::cout << "[C++] Freeing StringArray with " << array->count << " strings." << std::endl;
    for (size_t i = 0; i < array->count; ++i) {
        if (array->strings[i] != nullptr) {
            delete[] array->strings[i];
        }
    }
    delete[] array->strings;
    delete array;
    std::cout << "[C++] StringArray freed successfully." << std::endl;
}

void free_int_array_data(int* data)
{
    if (data == nullptr) {
        return;
    }
    
    delete[] data;
    std::cout << "[C++] IntArray data freed successfully." << std::endl;
}

size_t get_vm_pc()
{
    return WorkShop::getInstance().getPC();
}

void step_vm()
{
    WorkShop::getInstance().step();
}

void reset_vm_program()
{
    WorkShop::getInstance().resetProgram();
}

IntArray get_vm_all_registers()
{
    const std::vector<int>& vec = WorkShop::getInstance().getVMRegisters();
    IntArray array;
    array.data = new int[vec.size()];
    std::memcpy(array.data, vec.data(), vec.size() * sizeof(int));
    array.count = vec.size();

    return array;
}

EXPORT_API IntArray get_vm_all_memory()
{
    const std::vector<int>& vec = WorkShop::getInstance().getVMMemory();
    IntArray array;
    array.data = new int[vec.size()];
    std::memcpy(array.data, vec.data(), vec.size() * sizeof(int));
    array.count = vec.size();

    return array;
}

bool get_vm_zero_flag()
{
    return WorkShop::getInstance().getVMZeroFlag();
}

bool get_vm_sign_flag()
{
    return WorkShop::getInstance().getVMSignFlag();
}
