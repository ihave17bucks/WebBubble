// WebBubble File Operations Implementation (C++)

#include "file_ops.hpp"
#include <fstream>
#include <filesystem>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>

namespace fs = std::filesystem;

extern "C" {

const char* file_read(const char* path) {
    if (!path) return nullptr;  // Null check added
    
    try {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) return nullptr;
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        std::string content = buffer.str();
        char* result = new char[content.length() + 1];
        std::strcpy(result, content.c_str());
        
        return result;
    } catch (...) {
        return nullptr;
    }
}

int file_write(const char* path, const char* content) {
    if (!path || !content) return 0;  // Null checks added
    
    try {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) return 0;
        
        file << content;
        return 1;
    } catch (...) {
        return 0;
    }
}

int file_append(const char* path, const char* content) {
    try {
        std::ofstream file(path, std::ios::binary | std::ios::app);
        if (!file.is_open()) return 0;
        
        file << content;
        return 1;
    } catch (...) {
        return 0;
    }
}

int file_exists(const char* path) {
    return fs::exists(path) ? 1 : 0;
}

int file_delete(const char* path) {
    try {
        return fs::remove(path) ? 1 : 0;
    } catch (...) {
        return 0;
    }
}

long file_size(const char* path) {
    try {
        return static_cast<long>(fs::file_size(path));
    } catch (...) {
        return -1;
    }
}

const char* file_list_dir(const char* path) {
    try {
        std::vector<std::string> files;
        
        for (const auto& entry : fs::directory_iterator(path)) {
            files.push_back(entry.path().filename().string());
        }
        
        // Create JSON array
        std::stringstream json;
        json << "[";
        for (size_t i = 0; i < files.size(); i++) {
            json << "\"" << files[i] << "\"";
            if (i < files.size() - 1) json << ",";
        }
        json << "]";
        
        std::string result_str = json.str();
        char* result = new char[result_str.length() + 1];
        std::strcpy(result, result_str.c_str());
        
        return result;
    } catch (...) {
        return nullptr;
    }
}

int file_mkdir(const char* path) {
    try {
        return fs::create_directories(path) ? 1 : 0;
    } catch (...) {
        return 0;
    }
}

int file_copy(const char* src, const char* dst) {
    try {
        fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
        return 1;
    } catch (...) {
        return 0;
    }
}

int file_move(const char* src, const char* dst) {
    try {
        fs::rename(src, dst);
        return 1;
    } catch (...) {
        return 0;
    }
}

const char* file_extension(const char* path) {
    if (!path) return nullptr;  // Null check added
    
    try {
        fs::path p(path);
        std::string ext = p.extension().string();
        
        if (ext.empty()) return nullptr;
        
        char* result = new char[ext.length() + 1];
        std::strcpy(result, ext.c_str());
        
        return result;
    } catch (...) {
        return nullptr;
    }
}

void file_free_string(const char* str) {
    if (str) {
        delete[] str;
    }
}

}
