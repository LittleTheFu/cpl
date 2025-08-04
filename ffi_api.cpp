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

FfiResult create_ffi_result(int code, const std::string &message)
{
    FfiResult result;
    result.code = code;
    result.message = allocate_and_copy_c_string(message);
    return result;
}

EXPORT_API void free_ffi_result_message(char *message)
{
    if (message != nullptr)
    {
        delete[] message;
        // std::cout << "[C++] FfiResult message freed." << std::endl; // 可选的调试输出
    }
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


FfiResult load_program(const char* source, StringArray** out_program_array) {
    // 初始化输出指针为 nullptr，以防出错
    *out_program_array = nullptr;

    if (source == nullptr) {
        return create_ffi_result(-1, "Error: Source string cannot be null.");
    }

    try {
        // 调用 WorkShop 的 loadProgram，这里可能抛出异常
        std::vector<std::string> vec = WorkShop::getInstance().loadProgram(source);
        
        // 如果成功，将结果转换为 StringArray 并通过输出参数返回
        *out_program_array = convert_vector_to_string_array(vec);
        
        std::cout << "[C++ FFI] Program loaded successfully from source: " << source << std::endl;
        return create_ffi_result(0, "Program loaded successfully."); // 0 表示成功
    } catch (const std::exception& e) {
        // 捕获标准 C++ 异常，并将其封装到 FfiResult 中
        std::cerr << "[C++ FFI] Error loading program: " << e.what() << std::endl;
        return create_ffi_result(-1, "Failed to load program: " + std::string(e.what()));
    } catch (...) {
        // 捕获所有其他未知异常
        std::cerr << "[C++ FFI] Unknown error during program loading." << std::endl;
        return create_ffi_result(-2, "Failed to load program: An unknown error occurred.");
    }
}
