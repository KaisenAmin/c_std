#ifndef JSON_H_
#define JSON_H_

#include "../fmt/fmt.h"
#include "../vector/vector.h"
#include "../map/map.h"

// JSON data types
typedef enum {
    JSON_NULL,
    JSON_BOOL,
    JSON_STRING,
    JSON_NUMBER,
    JSON_ARRAY,
    JSON_OBJECT
} JsonType;

// JSON value union
typedef union {
    bool bool_val;
    char *string_val;
    double number_val;
    Vector *array_val;
    Map *object_val;
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
    // Strict mode (e.g., no trailing commas)
    // Overrides all other options
    bool strict;
    // Allow comments in JSON (// and /* */)
    bool allow_comments; 
    // Allow trailing commas like in JavaScript
    bool allow_trailing_comma; 
    // Allow number extensions (e.g. 0xFF, 0b1010, 0o777)
    bool allow_number_extensions; 
    // Allow single-quoted strings
    bool allow_single_quoted_strings; 
    // Allow unquoted keys in objects
    bool allow_unquoted_keys;
    // Allow C-style escape sequences in strings (true by default)
    bool allow_string_c_style_escapes; // (does not include \uXXXX and \0 escapes)
    // Allow UTF-8 in strings (false by default)
    bool allow_utf8;
} JsonParseOptions;

typedef enum {
    JSON_ERROR_NONE,
    JSON_ERROR_OUT_OF_MEMORY,
    JSON_ERROR_OUT_OF_RANGE,
    JSON_ERROR_NON_ENABLED_EXTENSION,
    JSON_ERROR_UNFINISHED_COMMENT,
    JSON_ERROR_UNFINISHED_STRING,
    JSON_ERROR_UNIMPLEMENTED_STRING,
    JSON_ERROR_INVALID_NUMBER,
    JSON_ERROR_UNFINISHED_ARRAY,
    JSON_ERROR_UNFINISHED_OBJECT,
    JSON_ERROR_UNFINISHED_OBJECT_KEY,
    JSON_ERROR_NO_PARSERS_FOUND,
    JSON_ERROR_FINISHED_EARLY,
    JSON_ERROR_FILE_NOT_FOUND,
    JSON_ERROR_IO_ERROR,
    JSON_ERROR_INVALID_QUERY,
    JSON_ERROR_INVALID_TYPE,
    JSON_ERROR_INVALID_ARGUMENT
} JsonErrorCode;

// Error handling
typedef struct {
    JsonErrorCode code;
    char message[256];
} JsonError;

typedef enum {
    JSON_PARSER_STATE_OK, // Parsing is ok
    JSON_PARSER_STATE_CONTINUE, // No error, but continue parsing
    JSON_PARSER_STATE_ERROR // Error
} JsonParserStateResult;

// Parser state
typedef struct {
    const char* input;
    size_t input_len;
    size_t position;
    JsonParseOptions options;
} JsonParserState;

// Create and copy
JsonElement *json_create(JsonType type);
void json_deallocate(JsonElement *element);
JsonElement* json_deep_copy(const JsonElement *element);

// Parse
JsonElement* json_parse(const char *json_str);
JsonElement* json_read_from_file(const char *filename);
JsonElement* json_parse_with_options(const char *json_str, JsonParseOptions options);
JsonElement* json_read_from_file_with_options(const char *filename, JsonParseOptions options);

// Get
JsonElement* json_get_element(const JsonElement *element, const char *key_or_index);
JsonElement *json_get_array_element(const JsonElement *array, size_t index);
JsonElement *json_get_object_element(const JsonElement *object, const char *key);
JsonElement* json_query(const JsonElement *element, const char *query);

size_t json_array_size(const JsonElement *array);
size_t json_object_size(const JsonElement *object);

// Error handling
JsonError json_last_error();

JsonElement* json_find(const JsonElement *element, JsonPredicate predicate, void *user_data);
JsonElement* json_merge(const JsonElement *element1, const JsonElement *element2);
JsonElement* json_filter(const JsonElement *array, JsonPredicate predicate, void *user_data);
/*
JsonElement* json_map(const JsonElement *array, JsonMapFunction map_func, void *user_data);

char* json_serialize(const JsonElement *element);
char* json_format(const JsonElement *element);
char** json_to_string_array(const JsonElement *array, size_t *length);

bool json_write_to_file(const JsonElement *element, const char *filename);
bool json_set_element(JsonElement *element, const char *key_or_index, JsonElement *new_element);
bool json_remove_element(JsonElement *element, const char *key_or_index);
bool json_validate(const JsonElement *element, const char *schema_json);
bool json_compare(const JsonElement *element1, const JsonElement *element2);

void* json_convert(const JsonElement *element, JsonType type);
void* json_reduce(const JsonElement *array, JsonReduceFunction reduce_func, void *initial_value, void *user_data);
*/
void json_print(JsonElement *element, int ident);

JsonType json_typeof(const JsonElement *element);



#endif 
