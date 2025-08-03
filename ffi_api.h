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

// 函数名称改为更能反映其作用的
EXPORT_API StringArray* get_hardcoded_vm_instructions();
EXPORT_API void free_string_array(StringArray* array);

#ifdef __cplusplus
}
#endif

#endif // FFI_API_H