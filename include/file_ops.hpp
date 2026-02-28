// WebBubble File Operations Module (C++)
// Read, write, and manipulate files

#ifndef FILE_OPS_HPP
#define FILE_OPS_HPP

extern "C" {

// Read entire file into string
const char* file_read(const char* path);

// Write string to file
int file_write(const char* path, const char* content);

// Append string to file
int file_append(const char* path, const char* content);

// Check if file exists
int file_exists(const char* path);

// Delete file
int file_delete(const char* path);

// Get file size in bytes
long file_size(const char* path);

// List files in directory (returns JSON array)
const char* file_list_dir(const char* path);

// Create directory
int file_mkdir(const char* path);

// Copy file
int file_copy(const char* src, const char* dst);

// Move/rename file
int file_move(const char* src, const char* dst);

// Get file extension
const char* file_extension(const char* path);

// Free string returned by file operations
void file_free_string(const char* str);

}

#endif // FILE_OPS_HPP
