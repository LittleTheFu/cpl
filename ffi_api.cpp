#include "ffi_api.h" // This now defines EXPORT_API and declares functions with extern "C"
#include <iostream>
#include <vector>
#include <string>
#include <cstring> // For strlen, strcpy

// No need for #ifdef _WIN32 EXPORT_API ... here anymore, it's in the header.

// 辅助函数：安全地分配并复制 C 字符串
char* allocate_and_copy_c_string(const std::string& s) {
    char* c_str = new char[s.length() + 1];
    std::strcpy(c_str, s.c_str());
    return c_str;
}

// FFI 接口实现：生成汇编代码字符串列表并返回
// No EXPORT_API macro here directly, as it's applied via the header.
// No extern "C" {} wrapper here, as it's applied via the header.
StringArray* get_assembly_code_for_ui() {
    std::cout << "[C++] Generating assembly code lines for Dart UI..." << std::endl;

    // Here is your dummy data (replace with real data generation later)
    std::vector<std::string> assemblyCodeData = {
        "0000: MOV EAX, 0x12345678 (Generated)",
        "0001: ADD EBX, EAX",
        "0002: JMP L_LOOP",
        "0003: L_LOOP: (Generated)",
        "0004: CMP ECX, 0x0A",
        "0005: JL L_LOOP",
        "0006: PUSH EAX",
        "0007: CALL MyFunction",
        "0008: POP EAX",
        "0009: RET",
        "000A: NOP (Generated)",
        "000B: INT 0x80",
        "000C: XOR EDX, EDX",
        "000D: INC ESI",
        "000E: DEC EDI",
        "000F: LEA EBP, [ESP+0x10]",
        "0010: MOV BYTE PTR [ESI], AL",
        "0011: TEST EAX, EAX",
        "0012: JNZ L_CONTINUE",
        "0013: L_CONTINUE: (Generated)",
        "0014: SHL EAX, 2",
        "0015: SHR EBX, 1",
    };

    // 1. 在 C++ 堆上分配 StringArray 结构体
    StringArray* result = new StringArray();
    result->count = assemblyCodeData.size();

    // 2. 在 C++ 堆上分配 char* 数组，用于存储各个字符串的指针
    result->strings = new char*[result->count];

    // 3. 遍历 std::vector，为每个字符串分配内存并复制内容
    for (size_t i = 0; i < result->count; ++i) {
        result->strings[i] = allocate_and_copy_c_string(assemblyCodeData[i]);
    }

    std::cout << "[C++] Finished generating " << result->count << " lines. Returning StringArray*." << std::endl;
    return result;
}

// FFI 辅助函数实现：释放 get_assembly_code_for_ui 分配的内存
// No EXPORT_API macro here directly.
// No extern "C" {} wrapper here.
void free_string_array(StringArray* array) {
    if (array == nullptr) {
        return;
    }

    std::cout << "[C++] Freeing StringArray with " << array->count << " strings." << std::endl;

    // 1. 释放每个单独的字符串
    for (size_t i = 0; i < array->count; ++i) {
        if (array->strings[i] != nullptr) {
            delete[] array->strings[i]; // 使用 new char[] 分配的用 delete[] 释放
        }
    }

    // 2. 释放存储字符串指针的数组本身
    delete[] array->strings;

    // 3. 释放 StringArray 结构体
    delete array;

    std::cout << "[C++] StringArray freed successfully." << std::endl;
}