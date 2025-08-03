#ifndef FFI_API_H
#define FFI_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h> // For size_t

#ifdef _WIN32
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __attribute__((visibility("default")))
#endif

typedef struct {
    char** strings;
    size_t count;
} StringArray;

typedef struct {
    int* data;
    size_t count;
} IntArray;

// 函数名称改为更能反映其作用的
EXPORT_API StringArray* get_hardcoded_vm_instructions();
EXPORT_API void free_string_array(StringArray* array);
EXPORT_API void free_int_array_data(int* data);
EXPORT_API size_t get_vm_pc();
EXPORT_API void step_vm();
EXPORT_API void reset_vm_program();
EXPORT_API IntArray get_vm_all_registers();
EXPORT_API bool get_vm_zero_flag();
EXPORT_API bool get_vm_sign_flag();


#ifdef __cplusplus
}
#endif

#endif // FFI_API_H