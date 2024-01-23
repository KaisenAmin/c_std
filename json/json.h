#ifndef JSON_H_
#define JSON_H_

#include "../fmt/fmt.h"

#define JSON_ERROR_NONE 0
#define JSON_ERROR_SYNTAX 1
#define JSON_ERROR_UNEXPECTED_TOKEN 2

// JSON data types
typedef enum {
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} JsonType;

// JSON value union
typedef union {
    bool bool_val;
    double number_val;
    char *string_val;
    struct JsonArray *array_val;
    struct JsonObject *object_val;
} JsonValue;

// JSON element struct
typedef struct {
    JsonType type;
    JsonValue value;
} JsonElement;

// Forward declarations for JSON array and object
typedef struct JsonArray JsonArray; // Struct representing a JSON array, possibly using a vector or list for storing elements.
typedef struct JsonObject JsonObject; // Struct representing a JSON object, possibly using a map or hash table for storing key-value pairs.

// Function pointer types for operations
typedef bool (*JsonPredicate)(const JsonElement*, void*);
typedef JsonElement* (*JsonMapFunction)(const JsonElement*, void*);
typedef void* (*JsonReduceFunction)(const JsonElement*, void*, void*);

// JSON parsing options (placeholder for extensibility)
typedef struct {
    int a;
    // Define parsing options here (e.g., strict mode, allow comments)
} JsonParseOptions;

// Iterator structure for JSON elements
typedef struct {
    const JsonElement *current_element; // Pointer to the current JSON element
    size_t index;                       // Current index in the array or object
    const JsonElement *parent;          // Pointer to the parent JSON element (array or object)
} JsonIterator;

// Error handling
typedef struct {
    int code;
    char message[256];
} JsonError;

// Function to parse a JSON string into a `JsonElement`.
JsonElement* json_parse(const char *json_str);

// Function to read JSON data from a file using your `file_io` library.
JsonElement* json_read_from_file(const char *filename);

// Function to retrieve an element from a JSON object or array by key or index.
JsonElement* json_get_element(const JsonElement *element, const char *key_or_index);

// Function to query JSON data using a query language or path expression.
JsonElement* json_query(const JsonElement *element, const char *query);

// Function to merge two JSON objects or arrays.
JsonElement* json_merge(const JsonElement *element1, const JsonElement *element2);

// Function to create a deep copy of JSON data.
JsonElement* json_deep_copy(const JsonElement *element);

// Function to create a new JSON element with optional memory pool support.
JsonElement* json_create(JsonType type);

// Parses a JSON string with specific parsing options.q
JsonElement* json_parse_with_options(const char *json_str, JsonParseOptions options);

// Finds an element in a JSON object or array based on a predicate.
JsonElement* json_find(const JsonElement *element, JsonPredicate predicate, void *user_data);

// Filters elements in a JSON array based on a predicate.
JsonElement* json_filter(const JsonElement *array, JsonPredicate predicate, void *user_data);

// Applies a function to each element in a JSON array, creating a new array.
JsonElement* json_map(const JsonElement *array, JsonMapFunction map_func, void *user_data);
JsonElement* json_iterator_value(const JsonIterator *iterator);

// Function to convert a `JsonElement` or derived type (`JsonObject`, `JsonArray`) back into a JSON string.
char* json_serialize(const JsonElement *element);
// Function to format JSON data using `fmt` library for pretty-printing or custom formatting.
char* json_format(const JsonElement *element);

// Returns a pretty-printed JSON string.
char* json_to_pretty_string(const JsonElement *element, int indent_width);

bool json_iterator_next(JsonIterator *iterator);

// Function to write JSON data to a file using `file_io`
bool json_write_to_file(const JsonElement *element, const char *filename);

// Function to set an element in a JSON object or array.
bool json_set_element(JsonElement *element, const char *key_or_index, JsonElement *new_element);

// Function to remove an element from a JSON object or array.
bool json_remove_element(JsonElement *element, const char *key_or_index);
// Function to validate JSON data against a schema.
bool json_validate(const JsonElement *element, const char *schema_json);

// Functions to convert between JSON elements and native data types.
void* json_convert(const JsonElement *element, JsonType type);

// Applies a reducer function to a JSON array, resulting in a single value.
void* json_reduce(const JsonElement *array, JsonReduceFunction reduce_func, void *initial_value, void *user_data);

// Returns the size of a JSON array.
size_t json_array_size(const JsonElement *array);

// Returns the number of key-value pairs in a JSON object.
size_t json_object_size(const JsonElement *object);

// Struct and functions for iterating over JSON objects and arrays.
JsonIterator json_iterator_begin(const JsonElement *element);
JsonIterator json_iterator_end(const JsonElement *element);

// Returns the type of a given JSON element.
JsonType json_type_of_element(const JsonElement *element);
// Function to retrieve the last error occurred during JSON operations.
JsonError json_last_error();

// Function to deallocate a JSON element and its contents.
void json_deallocate(JsonElement *element);
//Function to compare two JSON elements.
int json_compare(const JsonElement *element1, const JsonElement *element2);

#endif 
