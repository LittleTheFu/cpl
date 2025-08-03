#ifndef FFI_API_H
#define FFI_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h> // For size_t

// Define EXPORT_API based on the compiler/platform
#ifdef _WIN32
#define EXPORT_API __declspec(dllexport)
#else
// For GCC/Clang on Linux/macOS, use visibility("default") for shared libraries
#define EXPORT_API __attribute__((visibility("default")))
#endif

// Define a C 结构体，用于在 C++ 和 Dart 之间传递字符串数组
// Dart FFI 需要一个与 C 兼容的结构体布局
typedef struct {
    char** strings; // 指向 C 风格字符串指针数组的指针
    size_t count;   // 数组中字符串的数量
} StringArray;

// 函数声明：从 C++ 端获取汇编代码字符串列表
// 返回一个指向 StringArray 结构体的指针。
// 注意：这个结构体及其内部的字符串都是在 C++ 堆上动态分配的，
// Dart 端收到后必须调用相应的释放函数。
EXPORT_API StringArray* get_assembly_code_for_ui();

// 辅助函数：释放 C++ 端为 get_assembly_code_for_ui 分配的内存
// Dart 端调用这个函数来释放 StringArray 及其包含的所有字符串。
EXPORT_API void free_string_array(StringArray* array);

#ifdef __cplusplus
}
#endif

#endif // FFI_API_H