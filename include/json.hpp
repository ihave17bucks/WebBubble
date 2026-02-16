#ifndef JSON_HPP
#define JSON_HPP

#ifdef __cplusplus
extern "C" {
#endif

// C interface for JSON functionality
typedef void* JSONValue;

// JSON creation
JSONValue json_create_object();
JSONValue json_create_array();
JSONValue json_create_string(const char* str);
JSONValue json_create_number(double num);
JSONValue json_create_bool(int value);
JSONValue json_create_null();

// JSON object operations
void json_object_set(JSONValue obj, const char* key, JSONValue value);
JSONValue json_object_get(JSONValue obj, const char* key);

// JSON array operations
void json_array_push(JSONValue arr, JSONValue value);
JSONValue json_array_get(JSONValue arr, int index);
int json_array_length(JSONValue arr);

// JSON parsing and serialization
JSONValue json_parse(const char* str);
char* json_stringify(JSONValue value);

// Cleanup
void json_free(JSONValue value);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
// C++ implementation details (hidden from C)
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <variant>

namespace WebBubble {
    class JSON {
    public:
        using Object = std::map<std::string, std::shared_ptr<JSON>>;
        using Array = std::vector<std::shared_ptr<JSON>>;
        using Value = std::variant<
            std::nullptr_t,
            bool,
            double,
            std::string,
            Object,
            Array
        >;
        
        enum class Type {
            Null, Bool, Number, String, Object, Array
        };
        
        Value data;
        Type type;
        
        JSON() : data(nullptr), type(Type::Null) {}
        explicit JSON(bool b) : data(b), type(Type::Bool) {}
        explicit JSON(double n) : data(n), type(Type::Number) {}
        explicit JSON(const std::string& s) : data(s), type(Type::String) {}
        explicit JSON(const Object& o) : data(o), type(Type::Object) {}
        explicit JSON(const Array& a) : data(a), type(Type::Array) {}
        
        static std::shared_ptr<JSON> parse(const std::string& str);
        std::string stringify() const;
    };
}
#endif

#endif
