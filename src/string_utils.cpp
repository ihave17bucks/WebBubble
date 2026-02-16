#include "string_utils.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <cstdarg>
#include <algorithm>

extern "C"
{

    char *string_format(const char *format, ...)
    {
        va_list args;
        va_start(args, format);

        // Get required size
        va_list args_copy;
        va_copy(args_copy, args);
        int size = vsnprintf(nullptr, 0, format, args_copy);
        va_end(args_copy);

        if (size < 0)
        {
            va_end(args);
            return nullptr;
        }

        // Allocate and format
        char *buffer = (char *)malloc(size + 1);
        vsnprintf(buffer, size + 1, format, args);
        va_end(args);

        return buffer;
    }

    char *string_join(const char **strings, int count, const char *separator)
    {
        std::ostringstream oss;
        for (int i = 0; i < count; i++)
        {
            if (i > 0)
                oss << separator;
            oss << strings[i];
        }

        std::string result = oss.str();
        char *c_str = (char *)malloc(result.length() + 1);
        strcpy(c_str, result.c_str());
        return c_str;
    }

    char **string_split(const char *str, const char *delimiter, int *out_count)
    {
        std::string s(str);
        std::string delim(delimiter);
        std::vector<std::string> tokens;

        size_t pos = 0;
        while ((pos = s.find(delim)) != std::string::npos)
        {
            tokens.push_back(s.substr(0, pos));
            s.erase(0, pos + delim.length());
        }
        tokens.push_back(s);

        *out_count = tokens.size();
        char **result = (char **)malloc(sizeof(char *) * tokens.size());

        for (size_t i = 0; i < tokens.size(); i++)
        {
            result[i] = (char *)malloc(tokens[i].length() + 1);
            strcpy(result[i], tokens[i].c_str());
        }

        return result;
    }

    char *string_trim(const char *str)
    {
        std::string s(str);

        // Trim left
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
                                        { return !std::isspace(ch); }));

        // Trim right
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                             { return !std::isspace(ch); })
                    .base(),
                s.end());

        char *result = (char *)malloc(s.length() + 1);
        strcpy(result, s.c_str());
        return result;
    }

    char *string_replace(const char *str, const char *find, const char *replace)
    {
        std::string s(str);
        std::string f(find);
        std::string r(replace);

        size_t pos = 0;
        while ((pos = s.find(f, pos)) != std::string::npos)
        {
            s.replace(pos, f.length(), r);
            pos += r.length();
        }

        char *result = (char *)malloc(s.length() + 1);
        strcpy(result, s.c_str());
        return result;
    }

    int string_starts_with(const char *str, const char *prefix)
    {
        std::string s(str);
        std::string p(prefix);
        return s.find(p) == 0 ? 1 : 0;
    }

    int string_ends_with(const char *str, const char *suffix)
    {
        std::string s(str);
        std::string suf(suffix);
        if (s.length() < suf.length())
            return 0;
        return s.compare(s.length() - suf.length(), suf.length(), suf) == 0 ? 1 : 0;
    }

    void string_free(char *str)
    {
        if (str)
            free(str);
    }

    void string_array_free(char **arr, int count)
    {
        if (!arr)
            return;
        for (int i = 0; i < count; i++)
        {
            free(arr[i]);
        }
        free(arr);
    }

} // extern "C"
