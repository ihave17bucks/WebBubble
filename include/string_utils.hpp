#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#ifdef __cplusplus
extern "C" {
#endif

// C interface for string utilities
char* string_format(const char* format, ...);
char* string_join(const char** strings, int count, const char* separator);
char** string_split(const char* str, const char* delimiter, int* out_count);
char* string_trim(const char* str);
char* string_replace(const char* str, const char* find, const char* replace);
int string_starts_with(const char* str, const char* prefix);
int string_ends_with(const char* str, const char* suffix);
void string_free(char* str);
void string_array_free(char** arr, int count);

#ifdef __cplusplus
}
#endif

#endif
