#ifndef JSON_H_
#define JSON_H_

#include "../fmt/fmt.h"
#include "../vector/vector.h"
#include "../map/map.h"

#define JSON_ERROR_NONE 0
#define JSON_ERROR_SYNTAX 1
#define JSON_ERROR_UNEXPECTED_TOKEN 2
#define JSON_ERROR_MEMORY 3
#define JSON_CREATION_FAILED 4
#define JSON_TRANSFORM_FAILED 5
#define JSON_ERROR_INSERTION_FAILED 6
#define JSON_ERROR_INVALID_VALUE 7
#define JSON_ERROR_INVALID_KEY 8
#define JSON_ERROR_TYPE 9 

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
    int a;
    // Define parsing options here (e.g., strict mode, allow comments)
} JsonParseOptions;

// Error handling
typedef struct {
    int code;
    char message[256];
} JsonError;

typedef enum {
    JSON_TOKEN_OBJECT_START,
    JSON_TOKEN_OBJECT_END,
    JSON_TOKEN_ARRAY_START,
    JSON_TOKEN_ARRAY_END,
    JSON_TOKEN_STRING,
    JSON_TOKEN_NUMBER,
    JSON_TOKEN_BOOLEAN,
    JSON_TOKEN_NULL,
    JSON_TOKEN_COLON,
    JSON_TOKEN_COMMA,
    JSON_TOKEN_EOF,
    JSON_TOKEN_ERROR,
    
} JsonTokenType;

typedef struct {
    JsonTokenType type;
    char* value; // for strings, numbers
} JsonToken;

// Parser state
typedef struct {
    char* input;
    size_t input_len;
    size_t position;
    JsonToken current_token;
    JsonError error;
} JsonParserState;

JsonElement* json_parse(const char *json_str);
JsonElement* json_read_from_file(const char *filename);
JsonElement* json_get_element(const JsonElement *element, const char *key_or_index);
JsonElement* json_query(const JsonElement *element, const char *query);
JsonElement* json_merge(const JsonElement *element1, const JsonElement *element2);
JsonElement* json_deep_copy(const JsonElement *element);
JsonElement* json_parse_with_options(const char *json_str, JsonParseOptions options);
JsonElement* json_find(const JsonElement *element, JsonPredicate predicate, void *user_data);
JsonElement* json_filter(const JsonElement *array, JsonPredicate predicate, void *user_data);
JsonElement* json_map(const JsonElement *array, JsonMapFunction map_func, void *user_data);
JsonElement* json_create(JsonType type);
JsonElement* json_clone(const JsonElement *element);

char* json_serialize(const JsonElement *element);
char* json_format(const JsonElement *element);
char* json_generate_schema(const JsonElement* element);

char** json_to_string_array(const JsonElement *array, size_t *length);
char** json_get_keys(const JsonElement *object, size_t *num_keys);

bool json_write_to_file(const JsonElement *element, const char *filename);
bool json_set_element(JsonElement *element, const char *key_or_index, JsonElement *new_element);
bool json_remove_element(JsonElement *element, const char *key_or_index);
bool json_validate(const JsonElement *element, const char *schema_json);
bool json_compare(const JsonElement *element1, const JsonElement *element2);
bool json_add_to_array(JsonElement* element1, JsonElement* element2);
bool json_add_to_object(JsonElement* object, const char* key, JsonElement* value);

void* json_convert(const JsonElement *element, JsonType type);
void* json_reduce(const JsonElement *array, JsonReduceFunction reduce_func, void *initial_value, void *user_data);
size_t json_array_size(const JsonElement *array);
size_t json_object_size(const JsonElement *object);

JsonType json_type_of_element(const JsonElement *element);
JsonError json_last_error();

void json_print(const JsonElement* element);
void json_deallocate(JsonElement *element);

#endif 
