#include <stdio.h>
#include <stdlib.h>
#include "json.hpp"
#include "string_utils.hpp"

void demo_json()
{
    printf("=== JSON Demo (C++ module called from C) ===\n\n");

    // Create JSON object
    JSONValue user = json_create_object();
    json_object_set(user, "name", json_create_string("Alice"));
    json_object_set(user, "age", json_create_number(28));
    json_object_set(user, "active", json_create_bool(1));

    // Stringify
    char *json_str = json_stringify(user);
    printf("JSON Object: %s\n", json_str);
    free(json_str);

    // Create JSON array
    JSONValue numbers = json_create_array();
    json_array_push(numbers, json_create_number(1));
    json_array_push(numbers, json_create_number(2));
    json_array_push(numbers, json_create_number(3));

    json_str = json_stringify(numbers);
    printf("JSON Array: %s\n", json_str);
    free(json_str);

    // Cleanup
    json_free(user);
    json_free(numbers);

    printf("\n");
}

void demo_string_utils()
{
    printf("=== String Utils Demo (C++ module called from C) ===\n\n");

    // String formatting
    char *formatted = string_format("Hello, %s! You are %d years old.", "Bob", 30);
    printf("Formatted: %s\n", formatted);
    string_free(formatted);

    // String joining
    const char *words[] = {"WebBubble", "is", "awesome"};
    char *joined = string_join(words, 3, " ");
    printf("Joined: %s\n", joined);
    string_free(joined);

    // String splitting
    int count;
    char **parts = string_split("apple,banana,cherry", ",", &count);
    printf("Split into %d parts: ", count);
    for (int i = 0; i < count; i++)
    {
        printf("%s%s", parts[i], i < count - 1 ? ", " : "\n");
    }
    string_array_free(parts, count);

    // String trimming
    char *trimmed = string_trim("  hello world  ");
    printf("Trimmed: '%s'\n", trimmed);
    string_free(trimmed);

    // String replacement
    char *replaced = string_replace("I love C", "C", "C++");
    printf("Replaced: %s\n", replaced);
    string_free(replaced);

    // String checks
    printf("Starts with 'Web': %d\n", string_starts_with("WebBubble", "Web"));
    printf("Ends with 'ble': %d\n", string_ends_with("WebBubble", "ble"));

    printf("\n");
}

int main()
{
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║  WebBubble Hybrid C/C++ Demo 🫧       ║\n");
    printf("║  Core: C  |  Extensions: C++          ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("\n");

    demo_json();
    demo_string_utils();

    printf("✅ All demos completed successfully!\n");
    printf("💡 C code is calling C++ functions seamlessly!\n\n");

    return 0;
}
