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
EXPORT_API StringArray* get_hardcoded_vm_instructions() {
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
EXPORT_API void free_string_array(StringArray* array) {
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