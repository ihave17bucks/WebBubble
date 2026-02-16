#include "json.hpp"
#include <sstream>
#include <stdexcept>
#include <cstring>

using namespace WebBubble;

// ===== C++ Implementation =====

std::string JSON::stringify() const
{
    std::ostringstream oss;

    switch (type)
    {
    case Type::Null:
        oss << "null";
        break;

    case Type::Bool:
        oss << (std::get<bool>(data) ? "true" : "false");
        break;

    case Type::Number:
        oss << std::get<double>(data);
        break;

    case Type::String:
        oss << "\"" << std::get<std::string>(data) << "\"";
        break;

    case Type::Object:
    {
        oss << "{";
        const auto &obj = std::get<Object>(data);
        bool first = true;
        for (const auto &[key, value] : obj)
        {
            if (!first)
                oss << ",";
            first = false;
            oss << "\"" << key << "\":" << value->stringify();
        }
        oss << "}";
        break;
    }

    case Type::Array:
    {
        oss << "[";
        const auto &arr = std::get<Array>(data);
        bool first = true;
        for (const auto &value : arr)
        {
            if (!first)
                oss << ",";
            first = false;
            oss << value->stringify();
        }
        oss << "]";
        break;
    }
    }

    return oss.str();
}

// Simple JSON parser (basic implementation)
std::shared_ptr<JSON> JSON::parse(const std::string &str)
{
    // This is a simplified parser - production would need a proper lexer/parser
    size_t pos = 0;

    // Skip whitespace
    while (pos < str.length() && std::isspace(str[pos]))
        pos++;

    if (pos >= str.length())
    {
        return std::make_shared<JSON>();
    }

    char first = str[pos];

    // Null
    if (str.substr(pos, 4) == "null")
    {
        return std::make_shared<JSON>();
    }

    // Boolean
    if (str.substr(pos, 4) == "true")
    {
        return std::make_shared<JSON>(true);
    }
    if (str.substr(pos, 5) == "false")
    {
        return std::make_shared<JSON>(false);
    }

    // Number
    if (std::isdigit(first) || first == '-')
    {
        double num = std::stod(str.substr(pos));
        return std::make_shared<JSON>(num);
    }

    // String
    if (first == '"')
    {
        size_t end = str.find('"', pos + 1);
        if (end != std::string::npos)
        {
            std::string s = str.substr(pos + 1, end - pos - 1);
            return std::make_shared<JSON>(s);
        }
    }

    // Object
    if (first == '{')
    {
        JSON::Object obj;
        return std::make_shared<JSON>(obj);
    }

    // Array
    if (first == '[')
    {
        JSON::Array arr;
        return std::make_shared<JSON>(arr);
    }

    return std::make_shared<JSON>();
}

// ===== C Interface =====

extern "C"
{

    JSONValue json_create_object()
    {
        return new std::shared_ptr<JSON>(std::make_shared<JSON>(JSON::Object()));
    }

    JSONValue json_create_array()
    {
        return new std::shared_ptr<JSON>(std::make_shared<JSON>(JSON::Array()));
    }

    JSONValue json_create_string(const char *str)
    {
        return new std::shared_ptr<JSON>(std::make_shared<JSON>(std::string(str)));
    }

    JSONValue json_create_number(double num)
    {
        return new std::shared_ptr<JSON>(std::make_shared<JSON>(num));
    }

    JSONValue json_create_bool(int value)
    {
        return new std::shared_ptr<JSON>(std::make_shared<JSON>(value != 0));
    }

    JSONValue json_create_null()
    {
        return new std::shared_ptr<JSON>(std::make_shared<JSON>());
    }

    void json_object_set(JSONValue obj, const char *key, JSONValue value)
    {
        auto obj_ptr = static_cast<std::shared_ptr<JSON> *>(obj);
        auto val_ptr = static_cast<std::shared_ptr<JSON> *>(value);

        if ((*obj_ptr)->type == JSON::Type::Object)
        {
            auto &object = std::get<JSON::Object>((*obj_ptr)->data);
            object[key] = *val_ptr;
        }
    }

    JSONValue json_object_get(JSONValue obj, const char *key)
    {
        auto obj_ptr = static_cast<std::shared_ptr<JSON> *>(obj);

        if ((*obj_ptr)->type == JSON::Type::Object)
        {
            auto &object = std::get<JSON::Object>((*obj_ptr)->data);
            auto it = object.find(key);
            if (it != object.end())
            {
                return new std::shared_ptr<JSON>(it->second);
            }
        }

        return nullptr;
    }

    void json_array_push(JSONValue arr, JSONValue value)
    {
        auto arr_ptr = static_cast<std::shared_ptr<JSON> *>(arr);
        auto val_ptr = static_cast<std::shared_ptr<JSON> *>(value);

        if ((*arr_ptr)->type == JSON::Type::Array)
        {
            auto &array = std::get<JSON::Array>((*arr_ptr)->data);
            array.push_back(*val_ptr);
        }
    }

    JSONValue json_array_get(JSONValue arr, int index)
    {
        auto arr_ptr = static_cast<std::shared_ptr<JSON> *>(arr);

        if ((*arr_ptr)->type == JSON::Type::Array)
        {
            auto &array = std::get<JSON::Array>((*arr_ptr)->data);
            if (index >= 0 && index < (int)array.size())
            {
                return new std::shared_ptr<JSON>(array[index]);
            }
        }

        return nullptr;
    }

    int json_array_length(JSONValue arr)
    {
        auto arr_ptr = static_cast<std::shared_ptr<JSON> *>(arr);

        if ((*arr_ptr)->type == JSON::Type::Array)
        {
            auto &array = std::get<JSON::Array>((*arr_ptr)->data);
            return array.size();
        }

        return 0;
    }

    JSONValue json_parse(const char *str)
    {
        try
        {
            auto parsed = JSON::parse(str);
            return new std::shared_ptr<JSON>(parsed);
        }
        catch (...)
        {
            return nullptr;
        }
    }

    char *json_stringify(JSONValue value)
    {
        auto val_ptr = static_cast<std::shared_ptr<JSON> *>(value);
        std::string result = (*val_ptr)->stringify();
        char *c_str = (char *)malloc(result.length() + 1);
        strcpy(c_str, result.c_str());
        return c_str;
    }

    void json_free(JSONValue value)
    {
        if (value)
        {
            delete static_cast<std::shared_ptr<JSON> *>(value);
        }
    }

} // extern "C"
