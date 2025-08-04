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

typedef struct {
    int code;       // 0 for success, non-zero for error
    char* message;  // Error message string (allocated by C++, freed by Dart)
} FfiResult;

// 函数名称改为更能反映其作用的
EXPORT_API StringArray* get_hardcoded_vm_instructions();

EXPORT_API void free_string_array(StringArray* array);
EXPORT_API void free_int_array_data(int* data);
EXPORT_API void free_ffi_result_message(char* message);


EXPORT_API size_t get_vm_pc();
EXPORT_API void step_vm();
EXPORT_API void reset_vm_program();

EXPORT_API IntArray get_vm_all_registers();
EXPORT_API IntArray get_vm_all_memory();

EXPORT_API bool get_vm_zero_flag();
EXPORT_API bool get_vm_sign_flag();

EXPORT_API FfiResult load_program(const char* source, StringArray** out_program_array);


#ifdef __cplusplus
}
#endif

#endif // FFI_API_H