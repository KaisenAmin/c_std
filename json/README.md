# JSON Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2023  
**License:** ISC License

## Overview

The JSON library in C offers comprehensive tools for handling JSON data within C programs. This library allows for parsing JSON strings, manipulating JSON objects and arrays, and handling various JSON data types. It is designed to be both flexible and efficient, suitable for a range of applications that require JSON processing.

## Key Features

- **Versatile JSON Data Types:** Supports null, boolean, number, string, array, and object types.
- **Dynamic JSON Structures:** Enables creation and manipulation of JSON arrays and objects.
- **Flexible Parsing Options:** Customizable parsing behavior to accommodate different JSON formats.
- **Error Handling:** Robust error reporting mechanism for tracking and debugging issues in JSON operations.

## Function Descriptions

- `json_parse(const char *json_str)`: Parses a JSON string into a `JsonElement`.
- `json_read_from_file(const char *filename)`: Reads JSON data from a file and parses it into a `JsonElement`.
- `json_get_element(const JsonElement *element, const char *key_or_index)`: Retrieves an element from a JSON object or array by key or index.
- `json_query(const JsonElement *element, const char *query)`: Queries JSON data using a specific query language or path expression.
- `json_merge(const JsonElement *element1, const JsonElement *element2)`: Merges two JSON objects or arrays into one.
- `json_deep_copy(const JsonElement *element)`: Creates a deep copy of JSON data.
- `json_parse_with_options(const char *json_str, JsonParseOptions options)`: Parses a JSON string with specific parsing options.
- `json_find(const JsonElement *element, JsonPredicate predicate, void *user_data)`: Finds an element in a JSON object or array based on a predicate.
- `json_filter(const JsonElement *array, JsonPredicate predicate, void *user_data)`: Filters elements in a JSON array based on a predicate.
- `json_map(const JsonElement *array, JsonMapFunction map_func, void *user_data)`: Applies a function to each element in a JSON array, creating a new array.
- `json_serialize(const JsonElement *element)`: Converts a `JsonElement` back into a JSON string.
- `json_format(const JsonElement *element)`: Formats JSON data for pretty-printing or custom formatting.
- `json_to_pretty_string(const JsonElement *element, int indent_width)`: Returns a pretty-printed JSON string.
- `json_write_to_file(const JsonElement *element, const char *filename)`: Writes JSON data to a file.
- `json_set_element(JsonElement *element, const char *key_or_index, JsonElement *new_element)`: Sets an element in a JSON object or array.
- `json_remove_element(JsonElement *element, const char *key_or_index)`: Removes an element from a JSON object or array.
- `json_validate(const JsonElement *element, const char *schema_json)`: Validates JSON data against a schema.
- `json_convert(const JsonElement *element, JsonType type)`: Converts between JSON elements and native data types.
- `json_reduce(const JsonElement *array, JsonReduceFunction reduce_func, void *initial_value, void *user_data)`: Applies a reducer function to a JSON array.
- `json_array_size(const JsonElement *array)`: Returns the size of a JSON array.
- `json_object_size(const JsonElement *object)`: Returns the number of key-value pairs in a JSON object.
- `json_iterator_begin(const JsonElement *element)`: Begins iteration over JSON objects and arrays.
- `json_iterator_end(const JsonElement *element)`: Ends iteration over JSON objects and arrays.
- `json_type_of_element(const JsonElement *element)`: Returns the type of a given JSON element.
- `json_last_error()`: Retrieves the last error occurred during JSON operations.
- `json_print(const JsonElement* element)`: Prints the JSON element.
- `json_deallocate(JsonElement *element)`: Deallocates a JSON element and its contents.
- `json_compare(const JsonElement *element1, const JsonElement *element2)`: Compares two JSON elements.

---

## Example 1: How to Read from a JSON File and Parse It `json_read_from_file` and `json_parse`
This example demonstrates how to read a JSON file and parse it using `json_read_from_file` and `json_parse`.

```c
#include "fmt/fmt.h"
#include "json/json.h"

int main() {
    const char* jsonFilePath = "./sources/json_example.json";
    JsonElement* jsonElement = json_read_from_file(jsonFilePath);
    
    if (jsonElement) {
        fmt_printf("Successfully parsed JSON file.\n");
        json_print(jsonElement); // Print the parsed JSON structure
        json_deallocate(jsonElement); // Deallocate the JSON structure
    } 
    else {
        fmt_printf("Failed to parse JSON file '%s'.\n", jsonFilePath);
    }
    
    return 0;
}
```

## Example 2: Accessing Nested Elements in a JSON Structure with `json_get_element` and `json_parse`
This example shows how to access nested elements within a JSON structure. It retrieves the name of the second category from a JSON array.

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonFilePath = "./sources/json_example.json";
    JsonElement* jsonElement = json_read_from_file(jsonFilePath);

    if (jsonElement) {
        fmt_printf("Successfully parsed JSON file.\n");

        // Accessing a nested element in the JSON structure
        JsonElement* categories = json_get_element(jsonElement, "categories");
        if (categories && categories->type == JSON_ARRAY) {
            JsonElement* secondCategory = json_get_element(categories, "1");

            if (secondCategory && secondCategory->type == JSON_OBJECT) {
                JsonElement* categoryName = json_get_element(secondCategory, "name");

                if (categoryName && categoryName->type == JSON_STRING) {
                    fmt_printf("Second category name: %s\n", categoryName->value.string_val);
                }
            }
        }

        json_deallocate(jsonElement); // Deallocate the JSON structure
    } 
    else {
        fmt_printf("Failed to parse JSON file '%s'.\n", jsonFilePath);
    }
    return 0;
}
```

## Example 3: Retrieving a Specific Element in a Nested JSON Object with `json_get_element`

This example demonstrates how to access a nested element inside a JSON object and retrieve a specific field value.

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonFilePath = "./sources/json_example.json";
    JsonElement* jsonElement = json_read_from_file(jsonFilePath);

    if (jsonElement) {
        fmt_printf("Successfully parsed JSON file.\n");

        // Accessing a nested element in the JSON structure
        JsonElement* additionalInfo = json_get_element(jsonElement, "additional_info");
        if (additionalInfo && additionalInfo->type == JSON_OBJECT) {
            JsonElement* budget = json_get_element(additionalInfo, "budget");

            if (budget && budget->type == JSON_NUMBER) {
                fmt_printf("Budget: %.2f\n", budget->value.number_val);
            }
        }

        json_deallocate(jsonElement); // Deallocate the JSON structure
    } 
    else {
        fmt_printf("Failed to parse JSON file '%s'.\n", jsonFilePath);
    }
    return 0;
}
```
