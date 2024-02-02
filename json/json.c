#include "json.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "../string/string.h"

static JsonElement* parse_array(JsonParserState* state);
static JsonElement* parse_string(JsonParserState* state);
static JsonElement* parse_number(JsonParserState* state);
static JsonElement* parse_null(JsonParserState* state);
static JsonElement* parse_boolean(JsonParserState* state);
static JsonElement* parser_internal(JsonParserState* state);
static JsonElement* parse_object(JsonParserState* state);
static void json_serialize_internal(const JsonElement* element, String* str);
static bool json_find_in_object(const JsonElement* object, JsonPredicate predicate, void* user_data, JsonElement** found_element);
static bool json_find_in_array(const JsonElement* array, JsonPredicate predicate, void* user_data, JsonElement** found_element);
static void json_format_internal(const JsonElement* element, String* str, int indent);

static JsonError last_error = {0, ""};

static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        fmt_printf(" ");
    }
}

static void json_print_internal(const JsonElement* element, int indent) {
    if (!element) {
        fmt_printf("null");
        return;
    }

    switch (element->type) {
        case JSON_OBJECT:
            fmt_printf("{\n");
            MapIterator it = map_begin(element->value.object_val);
            MapIterator end = map_end(element->value.object_val);
            bool first = true;
            while (it.node != end.node) {
                if (!first) {
                    fmt_printf(",\n");
                }
                print_indent(indent + 2);
                fmt_printf("\"%s\": ", (char*)it.node->key);
                json_print_internal((JsonElement*)it.node->value, indent + 2);
                first = false;
                map_iterator_increment(&it);
            }
            if (!first) {
                fmt_printf("\n");
            }
            print_indent(indent - 2);
            fmt_printf("}");
            break;
        case JSON_ARRAY:
            fmt_printf("[\n");
            for (size_t i = 0; i < vector_size(element->value.array_val); ++i) {
                print_indent(indent + 2);
                json_print_internal(*(JsonElement**)vector_at(element->value.array_val, i), indent + 2);
                if (i < vector_size(element->value.array_val) - 1) {
                    fmt_printf(",");
                }
                fmt_printf("\n");
            }
            print_indent(indent - 2);
            fmt_printf("]");
            break;
        case JSON_STRING:
            fmt_printf("\"%s\"", element->value.string_val);
            break;
        case JSON_NUMBER:
            if (floor(element->value.number_val) == element->value.number_val) {
                fmt_printf("%d", (int)element->value.number_val);
            } 
            else {
                fmt_printf("%.2f", element->value.number_val);
            }
            break;
        case JSON_BOOL:
            fmt_printf(element->value.bool_val ? "true" : "false");
            break;
        case JSON_NULL:
            fmt_printf("null");
            break;
        default:
            fmt_printf("Unknown type");
    }
}

static char** split_query(const char* query, int* count) {
    static char* tokens[256]; // Simple example, real code should dynamically allocate
    int i = 0;
    char* tempQuery = string_strdup(query); // Duplicate query to avoid modifying the original string
    char* token = strtok(tempQuery, ".");
    
    while (token != NULL && i < 256) {
        tokens[i++] = string_strdup(token); // Duplicate tokens, remember to free them later
        token = strtok(NULL, ".");
    }
    
    *count = i;
    free(tempQuery); // Free the duplicated query
    return tokens;
}

void json_deallocate(JsonElement *element) {
    if (!element) return;

    switch (element->type) {
        case JSON_STRING:
            free(element->value.string_val);
            break;
        case JSON_ARRAY:
            for (size_t i = 0; i < vector_size(element->value.array_val); ++i) {
                JsonElement* e = *(JsonElement**)vector_at(element->value.array_val, i);
                json_deallocate(e);
            }
            vector_deallocate(element->value.array_val);
            break;
        case JSON_OBJECT:
            // Assuming the map_deallocate function properly deallocates each key-value pair
            map_deallocate(element->value.object_val);
            break;
        default:
            // Other types do not require special handling
            break;
    }
    free(element);
}

static bool json_find_in_object(const JsonElement* object, JsonPredicate predicate, void* user_data, JsonElement** found_element) {
    MapIterator it = map_begin(object->value.object_val);
    MapIterator end = map_end(object->value.object_val);

    while (it.node != end.node) {
        JsonElement* current_element = (JsonElement*)it.node->value;
        if (predicate(current_element, user_data)) {
            *found_element = current_element;
            return true;
        }
        map_iterator_increment(&it);
    }

    return false;
}

static bool json_find_in_array(const JsonElement* array, JsonPredicate predicate, void* user_data, JsonElement** found_element) {
    for (size_t i = 0; i < vector_size(array->value.array_val); ++i) {
        JsonElement* current_element = *(JsonElement**)vector_at(array->value.array_val, i);
        if (predicate(current_element, user_data)) {
            *found_element = current_element;
            return true;
        }
    }

    return false;
}

static void json_element_deallocator(void* data) {
    if (!data) 
        return;
    JsonElement* element = (JsonElement*)data;
    json_deallocate(element);
}

static int compare_strings_json(const KeyType a, const KeyType b) {
    return strcmp((const char*)a, (const char*)b);
}

static void string_deallocator_json(void *data) {
    free(data);
} 

static void next_token(JsonParserState* state) {
    while (isspace((unsigned char)state->input[state->position])) {
        state->position++;
    }

    if (state->input[state->position] == '\0') {
        state->current_token.type = JSON_TOKEN_EOF;
        return;
    }

    char currentChar = state->input[state->position];
    switch (currentChar){
        case '{':
            state->current_token.type = JSON_TOKEN_OBJECT_START;
            break;
        case '}':
            state->current_token.type = JSON_TOKEN_OBJECT_END;
            break;
        case '[':
            state->current_token.type = JSON_TOKEN_ARRAY_START;
            break;
        case ']':
            state->current_token.type = JSON_TOKEN_ARRAY_END;
            break;
        case '\"':
            state->current_token.type = JSON_TOKEN_STRING;
            break;
        case ':':
            state->current_token.type = JSON_TOKEN_COLON;
            break;
        case ',':
            state->current_token.type = JSON_TOKEN_COMMA;
            break;
        case '\0':
            state->current_token.type = JSON_TOKEN_EOF;
            break;
        case '-':
        case '+':
            state->current_token.type = JSON_TOKEN_NUMBER;
            break;
        default:
            if (isdigit((unsigned char)currentChar)) {
                state->current_token.type = JSON_TOKEN_NUMBER;
                break;
            }
            else if (strncmp(state->input + state->position, "true", 4) == 0) {
                state->current_token.type = JSON_TOKEN_BOOLEAN;
            }
            else if (strncmp(state->input + state->position, "false", 5) == 0) {
                state->current_token.type = JSON_TOKEN_BOOLEAN;
            }
            if (strncmp(state->input + state->position, "null", 4) == 0) {
                state->current_token.type = JSON_TOKEN_NULL;
            }
            else {
                // hanle error 
            }
            break;
    }
    state->position++;
}

static JsonElement* parse_array(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_ARRAY_START) {
        snprintf(last_error.message, sizeof(last_error.message), "Expected start of array at position %zu", state->position);
        last_error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    JsonElement* array_element = json_create(JSON_ARRAY);
    if (!array_element) {
        snprintf(last_error.message, sizeof(last_error.message), "Memory allocation failed for array at position %zu", state->position);
        last_error.code = JSON_ERROR_MEMORY;
        return NULL;
    }
    next_token(state);

    while (state->current_token.type != JSON_TOKEN_ARRAY_END) {
        if (state->current_token.type == JSON_TOKEN_COMMA) {
            next_token(state);
        }
        else if (state->current_token.type == JSON_TOKEN_EOF) {
            snprintf(last_error.message, sizeof(last_error.message), "Unexpected end of input in array at position %zu", state->position);
            last_error.code = JSON_ERROR_SYNTAX;
            json_deallocate(array_element);
            return NULL;
        }
        JsonElement* element = parser_internal(state);
        if (!element) {
            json_deallocate(array_element);
            return NULL;
        }

        vector_push_back(array_element->value.array_val, &element);
        next_token(state);
    }

    return array_element;
}

static JsonElement* parse_string(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_STRING) {
        snprintf(last_error.message, sizeof(last_error.message), "Expected string token at position %zu", state->position);
        last_error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }
    size_t start = state->position;

    while (state->input[state->position] != '\"' && state->input[state->position] != '\0') {
        // Handle escape sequences if needed
        if (state->input[state->position] == '\0') {
            last_error.code = JSON_ERROR_SYNTAX;
            snprintf(last_error.message, sizeof(last_error.message), "Unterminated string");
            return NULL;
        }
        state->position++;
    }

    if (state->input[state->position] == '\0') {
        snprintf(last_error.message, sizeof(last_error.message), "Unterminated string at position %zu", start);
        last_error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    size_t length = state->position - start;
    char* str_content = (char*)malloc(length + 1);
    if (!str_content) {
        snprintf(last_error.message, sizeof(last_error.message), "Memory allocation failed for string at position %zu", start);
        last_error.code = JSON_ERROR_MEMORY;
        return NULL;
    }

    strncpy(str_content, state->input + start, length);
    str_content[length] = '\0';
    state->position++; // Skip the closing quote

    JsonElement* element = json_create(JSON_STRING);
    if (!element) {
        snprintf(last_error.message, sizeof(last_error.message), "Failed to create JSON string element at position %zu", start);
        last_error.code = JSON_CREATION_FAILED;
        free(str_content);
        return NULL;
    }

    element->value.string_val = str_content;
    return element;
}

static JsonElement* parse_number(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_NUMBER) {
        snprintf(last_error.message, sizeof(last_error.message), "Expected number token at position %zu", state->position);
        last_error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    size_t start = state->position - 1;
    while (isdigit((unsigned char)state->input[state->position]) || 
            state->input[state->position] == '.' || 
            state->input[state->position] == 'e' || 
            state->input[state->position] == 'E' || 
            (state->position != start && (state->input[state->position] == '+' || state->input[state->position] == '-'))) {
        state->position++;
    }

    size_t length = state->position - start;
    char* number_str = (char*)malloc(length + 1);
    if (!number_str) {
        snprintf(last_error.message, sizeof(last_error.message), "Memory allocation failed for number string at position %zu", start);
        last_error.code = JSON_ERROR_MEMORY;
        return NULL;
    }

    strncpy(number_str, state->input + start, length);
    number_str[length] = '\0';

    char* endptr;
    double number_double = strtod(number_str, &endptr);
    if (endptr == number_str) {
        snprintf(last_error.message, sizeof(last_error.message), "Invalid number format at position %zu", start);
        last_error.code = JSON_ERROR_SYNTAX;
        free(number_str);
        return NULL;
    }
    free(number_str);

    JsonElement* element = json_create(JSON_NUMBER);
    if (!element) {
        snprintf(last_error.message, sizeof(last_error.message), "Failed to create JSON number element at position %zu", start);
        last_error.code = JSON_CREATION_FAILED; 
        return NULL;
    }

    element->value.number_val = number_double;
    return element;
}

static JsonElement* parse_null(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_NULL) {
        snprintf(last_error.message, sizeof(last_error.message), "Expected 'null' token at position %zu", state->position);
        last_error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    state->position += 4; // // Skip past 'null'

    JsonElement* element = json_create(JSON_NULL);
    if (!element) {
        snprintf(last_error.message, sizeof(last_error.message), "Failed to create JSON null element at position %zu", state->position - 4);
        last_error.code = JSON_CREATION_FAILED; 
        return NULL;
    }
    return element;
}

static JsonElement* parse_boolean(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_BOOLEAN) {
        snprintf(last_error.message, sizeof(last_error.message),
                 "Expected boolean token at position %zu", state->position);
        last_error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    size_t start = state->position - 1;
    bool boolean_value;

    if (strncmp(state->input + start, "true", 4) == 0) {
        state->current_token.type = JSON_TOKEN_BOOLEAN;
        state->position += 3; // Skip past 'true'
        boolean_value = true;
    } 
    else if (strncmp(state->input + start, "false", 5) == 0) {
        state->current_token.type = JSON_TOKEN_BOOLEAN;
        state->position += 4; // Skip past 'false'
        boolean_value = false;
    } 
    else {
        snprintf(last_error.message, sizeof(last_error.message), "Invalid boolean format at position %zu", start);
        last_error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    JsonElement* element = json_create(JSON_BOOL);
    if (!element) {
        snprintf(last_error.message, sizeof(last_error.message), "Failed to create JSON boolean element at position %zu", start);
        last_error.code = JSON_CREATION_FAILED; 
        return NULL;
    }

    element->type = JSON_BOOL;
    element->value.bool_val = boolean_value;
    return element;
}

static JsonElement* parser_internal(JsonParserState* state) {
    switch (state->current_token.type) {
        case JSON_TOKEN_OBJECT_START:
            return parse_object(state);
        case JSON_TOKEN_ARRAY_START:
            return parse_array(state);
        case JSON_TOKEN_STRING:
            return parse_string(state);
        case JSON_TOKEN_NUMBER:
            return parse_number(state);
        case JSON_TOKEN_BOOLEAN:
            return parse_boolean(state);
        case JSON_TOKEN_NULL:
            return parse_null(state);
        default:
            snprintf(last_error.message, sizeof(last_error.message), "Unexpected token");
            last_error.code = JSON_ERROR_UNEXPECTED_TOKEN;
            return NULL;
    }
}

static JsonElement* parse_object(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_OBJECT_START) {
        snprintf(last_error.message, sizeof(last_error.message), "Expected start of object at position %zu", state->position);
        last_error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    JsonElement* object_element = json_create(JSON_OBJECT);
    if (!object_element) {
        snprintf(last_error.message, sizeof(last_error.message), "Failed to create JSON object element at position %zu", state->position);
        last_error.code = JSON_CREATION_FAILED; 
        return NULL;
    }
    next_token(state);

    while (state->current_token.type != JSON_TOKEN_OBJECT_END) {
        if (state->current_token.type != JSON_TOKEN_STRING) {
            snprintf(last_error.message, sizeof(last_error.message), "Expected string as key in object at position %zu", state->position);
            last_error.code = JSON_ERROR_SYNTAX;
            json_deallocate(object_element);
            return NULL;
        }

        char* key = parse_string(state)->value.string_val;
        next_token(state);
         if (state->current_token.type != JSON_TOKEN_COLON) {
            snprintf(last_error.message, sizeof(last_error.message), "Expected colon after key in object at position %zu", state->position);
            last_error.code = JSON_ERROR_SYNTAX;
            free(key);
            json_deallocate(object_element);
            return NULL;
        }

        next_token(state);
        JsonElement* value = parser_internal(state);
        if (!value) {
            free(key);
            json_deallocate(object_element);
            return NULL;
        }

        map_insert(object_element->value.object_val, key, value);
        next_token(state);

        if (state->current_token.type == JSON_TOKEN_COMMA) {
            next_token(state);
        }
    }
    return object_element;
}

static void serialize_string(const char* value, String* str) {
    string_append(str, "\"");
    string_append(str, value);
    string_append(str, "\"");
}

static void serialize_array(const JsonElement* element, String* str) {
    string_append(str, "[");
    for (size_t i = 0; i < vector_size(element->value.array_val); ++i) {
        json_serialize_internal(*(JsonElement**)vector_at(element->value.array_val, i), str);
        if (i < vector_size(element->value.array_val) - 1) {
            string_append(str, ", ");
        }
    }
    string_append(str, "]");
}

static void serialize_object(const JsonElement* element, String* str) {
    string_append(str, "{");
    MapIterator it = map_begin(element->value.object_val);
    MapIterator end = map_end(element->value.object_val);
    bool first = true;
    while (it.node != end.node) {
        if (!first) {
            string_append(str, ", ");
        }
        serialize_string((char*)it.node->key, str);
        string_append(str, ": ");
        json_serialize_internal((JsonElement*)it.node->value, str);
        first = false;
        map_iterator_increment(&it);
    }
    string_append(str, "}");
}

static void json_serialize_internal(const JsonElement* element, String* str) {
    if (!element) {
        string_append(str, "null");
        return;
    }

    switch (element->type) {
        case JSON_OBJECT:
            serialize_object(element, str);
            break;
        case JSON_ARRAY:
            serialize_array(element, str);
            break;
        case JSON_STRING:
            serialize_string(element->value.string_val, str);
            break;
        case JSON_NUMBER:
            {
                char buffer[64];
                snprintf(buffer, sizeof(buffer), "%g", element->value.number_val);
                string_append(str, buffer);
            }
            break;
        case JSON_BOOL:
            string_append(str, element->value.bool_val ? "true" : "false");
            break;
        case JSON_NULL:
            string_append(str, "null");
            break;
        default:
            string_append(str, "unknown");
    }
}

static void append_indent(String* str, int indent) {
    for (int i = 0; i < indent; ++i) {
        string_append(str, " ");
    }
}

static void format_string(const char* value, String* str) {
    string_append(str, "\"");
    string_append(str, value);
    string_append(str, "\"");
}

static void format_array(const JsonElement* element, String* str, int indent) {
    string_append(str, "[\n");
    for (size_t i = 0; i < vector_size(element->value.array_val); ++i) {
        append_indent(str, indent + 2);
        json_format_internal(*(JsonElement**)vector_at(element->value.array_val, i), str, indent + 2);
        if (i < vector_size(element->value.array_val) - 1) {
            string_append(str, ",");
        }
        string_append(str, "\n");
    }
    append_indent(str, indent);
    string_append(str, "]");
}

static void format_object(const JsonElement* element, String* str, int indent) {
    string_append(str, "{\n");
    MapIterator it = map_begin(element->value.object_val);
    MapIterator end = map_end(element->value.object_val);
    bool first = true;
    while (it.node != end.node) {
        if (!first) {
            string_append(str, ",\n");
        }
        append_indent(str, indent + 2);
        format_string((char*)it.node->key, str);
        string_append(str, ": ");
        json_format_internal((JsonElement*)it.node->value, str, indent + 2);
        first = false;
        map_iterator_increment(&it);
    }
    if (!first) {
        string_append(str, "\n");
    }
    append_indent(str, indent);
    string_append(str, "}");
}

static void json_format_internal(const JsonElement* element, String* str, int indent) {
    if (!element) {
        string_append(str, "null");
        return;
    }

    switch (element->type) {
        case JSON_OBJECT:
            format_object(element, str, indent);
            break;
        case JSON_ARRAY:
            format_array(element, str, indent);
            break;
        case JSON_STRING:
            format_string(element->value.string_val, str);
            break;
        case JSON_NUMBER:
            {
                char buffer[64];
                snprintf(buffer, sizeof(buffer), "%g", element->value.number_val);
                string_append(str, buffer);
            }
            break;
        case JSON_BOOL:
            string_append(str, element->value.bool_val ? "true" : "false");
            break;
        case JSON_NULL:
            string_append(str, "null");
            break;
        default:
            snprintf(last_error.message, sizeof(last_error.message), "Unknown JSON type");
            last_error.code = JSON_ERROR_UNEXPECTED_TOKEN;
            fmt_fprintf(stderr, "Error: Unknown JSON type in json_format_internal.\n");
            string_append(str, "unknown");
    }
}

JsonElement* json_create(JsonType type) {
    JsonElement *element = (JsonElement*)malloc(sizeof(JsonElement));
    if (!element) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in json_create.\n");
        return NULL;
    }

    element->type = type;
    switch (type) {
        case JSON_NULL:
            // No value needed for NULL type
            break;
        case JSON_BOOL:
            element->value.bool_val = false;
            break;
        case JSON_NUMBER:
            element->value.number_val = 0.0;
            break;
        case JSON_STRING:
            element->value.string_val = NULL;
            break;
        case JSON_ARRAY:
            element->value.array_val = vector_create(sizeof(JsonElement*));
            if (!element->value.array_val) {
                fmt_fprintf(stderr, "Error: Memory allocation failed for JSON array in json_create.\n");
                free(element);
                return NULL;
            }
            break;
        case JSON_OBJECT:
            element->value.object_val = map_create(compare_strings_json, string_deallocator_json, json_element_deallocator);
            if (!element->value.object_val) {
                fmt_fprintf(stderr, "Error: Memory allocation failed for JSON object in json_create.\n");
                free(element);
                return NULL;
            }
            break;
        default:
            fmt_fprintf(stderr, "Error: Invalid JSON type in json_create.\n");
            free(element);
            return NULL;
    }
    return element;
}

JsonElement* json_parse(const char* json_str) {
    if (!json_str) {
        fmt_fprintf(stderr, "Error: Json string is NULL and Invalid in json_parse.\n");
        return NULL;
    }

    JsonParserState state;
    state.input = string_strdup(json_str); 

    if (!state.input) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for duplicating JSON string in json_parse.\n");
        return NULL;
    }

    state.position = 0;
    state.input_len = string_length_cstr(json_str);
    state.error.code = JSON_ERROR_NONE;
    strcpy(state.error.message, "");

    // Move to the first token
    next_token(&state); 

    JsonElement* root = NULL;

    if (state.current_token.type == JSON_TOKEN_OBJECT_START) {
        root = parse_object(&state);

    } 
    else if (state.current_token.type == JSON_TOKEN_ARRAY_START) {
        root = parse_array(&state);
    }
    else {
        // Handling JSON as a single value (string, number, boolean, null)
        root = parser_internal(&state);
        if (root == NULL) {
            fmt_fprintf(stderr, "Error: Invalid JSON value at position %zu.\n", state.position);
            free(state.input);
            return NULL;
        }
    } 


    if (root == NULL) {
        fmt_fprintf(stderr, "Error while parsing JSON at position %zu: %s\n", state.position, state.error.message);
        free(state.input); 
        return NULL;
    }

    next_token(&state);
    
    if (state.current_token.type != JSON_TOKEN_EOF) {
        fmt_fprintf(stderr, "Error: Unexpected data after root element at position %zu. is %c\n", state.position, state.input[state.position]);
        json_deallocate(root);
        free(state.input);
        return NULL;
    }

    free(state.input);
    return root;
}

JsonElement * json_read_from_file(const char* filename) {
    if (!filename) {
        fmt_fprintf(stderr, "Error: filename is Null and Invalid in json_read_from_file.\n");
        return NULL;
    }

    FileReader *json = file_reader_open(filename, READ_TEXT);
    if (!json) {
        fmt_fprintf(stderr, "Error: Failed to open file '%s' in json_read_from_file.\n", filename);
        return NULL;
    }

    size_t json_file_size = file_reader_get_size(json);
    char* buffer = (char*) malloc(sizeof(char) * json_file_size + 1);

    if (!buffer) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for buffer in json_read_from_file.\n");
        file_reader_close(json);
        return NULL;
    }   
    
    size_t size = file_reader_read(buffer, sizeof(char), json_file_size, json);
    if (size == 0) {
        fmt_fprintf(stderr, "Error: Cannot read JSON file '%s' in json_read_from_file.\n", filename);
        free(buffer);
        file_reader_close(json);
        return NULL;
    } 
    
    buffer[size] = '\0';
    JsonElement* json_element = json_parse(buffer);

    free(buffer);
    file_reader_close(json);
    return json_element;
}

void json_print(const JsonElement* element) {
    if (!element) {
        fmt_printf("null\n");
        return;
    }
    json_print_internal(element, 2);
    fmt_printf("\n");
}

JsonElement* json_get_element(const JsonElement *element, const char *key_or_index) {
    if (!element || !key_or_index) {
        fmt_fprintf(stderr, "Error: Invalid argument(s) in json_get_element.\n");
        return NULL;
    }

    switch (element->type) {
        case JSON_OBJECT: {
            char* non_const_key = string_strdup(key_or_index);
            if (!non_const_key) {
                fmt_fprintf(stderr, "Error: Memory allocation failed in json_get_element.\n");
                return NULL;
            }
            JsonElement* result = (JsonElement*)map_at(element->value.object_val, non_const_key);
            free(non_const_key);
            return result;
        }

        case JSON_ARRAY: {
            char *end;
            long index = strtol(key_or_index, &end, 10);
            if (end == key_or_index || *end != '\0' || index < 0 || (size_t)index >= vector_size(element->value.array_val)) {
                fmt_fprintf(stderr, "Error: Invalid index '%s' in json_get_element.\n", key_or_index);
                return NULL;
            }
            return *(JsonElement**)vector_at(element->value.array_val, (size_t)index);
        }

        default:
            fmt_fprintf(stderr, "Error: Attempted to access non-object/non-array JSON element in json_get_element.\n");
            return NULL;
    }
}

size_t json_array_size(const JsonElement *array) {
    if (!array) {
        fmt_fprintf(stderr, "Error: The provided JsonElement is NULL in json_array_size.\n");
        return 0;
    }

    if (array->type != JSON_ARRAY) {
        fmt_fprintf(stderr, "Error: The provided JsonElement is not an array in json_array_size.\n");
        return 0;
    }

    return vector_size(array->value.array_val);
}

size_t json_object_size(const JsonElement* object) {
    if (!object) {
        fmt_fprintf(stderr, "Error: The Provided JsonElement is NULL in json_object_size.\n");
        return 0;
    }
    if (object->type != JSON_OBJECT) {
        fmt_fprintf(stderr, "Error: The provided JsonElement is not object.\n");
        return 0;
    }

    return map_size(object->value.object_val);
}

JsonElement* json_deep_copy(const JsonElement *element) {
    if (!element) {
        return NULL;
    }

    JsonElement *copy = json_create(element->type);
    if (!copy) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in json_deep_copy.\n");
        return NULL;
    }

    MapIterator it, end;

    switch (element->type) {
        case JSON_NULL:
            // No additional data to copy for null type
            break;

        case JSON_BOOL:
            copy->value.bool_val = element->value.bool_val;
            break;

        case JSON_NUMBER:
            copy->value.number_val = element->value.number_val;
            break;

        case JSON_STRING:
            copy->value.string_val = string_strdup(element->value.string_val);
            if (!copy->value.string_val) {
                fmt_fprintf(stderr, "Error: Memory allocation failed for string in json_deep_copy.\n");
                json_deallocate(copy);
                return NULL;
            }
            break;

        case JSON_ARRAY:
            for (size_t i = 0; i < vector_size(element->value.array_val); ++i) {
                JsonElement *item = json_deep_copy(*(JsonElement **)vector_at(element->value.array_val, i));
                if (!item) {
                    json_deallocate(copy); // Deallocate the partially copied array
                    return NULL;
                }

                if (!vector_push_back(copy->value.array_val, &item)) {
                    json_deallocate(item);
                    json_deallocate(copy);
                    return NULL;
                }
            }
            break;

        case JSON_OBJECT:
            it = map_begin(element->value.object_val);
            end = map_end(element->value.object_val);
            while (it.node != end.node) {
                char *key = string_strdup((char *)it.node->key);
                if (!key) {
                    fmt_fprintf(stderr, "Error: Memory allocation failed for object key in json_deep_copy.\n");
                    json_deallocate(copy);
                    return NULL;
                }

                JsonElement *val_copy = json_deep_copy((JsonElement *)it.node->value);
                if (!val_copy) {
                    free(key);
                    json_deallocate(copy);
                    return NULL;
                }

                map_insert(copy->value.object_val, key, val_copy);
                map_iterator_increment(&it);
            }
            break;

        default:
            fmt_fprintf(stderr, "Error: Unknown JSON type in json_deep_copy.\n");
            json_deallocate(copy);
            return NULL;
    }

    return copy;
}

JsonType json_type_of_element(const JsonElement *element) {
    if (!element) {
        fmt_fprintf(stderr, "Error: The provided JsonElement is NULL in json_type_of_element.\n");
        return JSON_NULL; // or another designated error value
    }
    return element->type;
}

bool json_write_to_file(const JsonElement *element, const char *filename) {
    if (!element || !filename) {
        fmt_fprintf(stderr, "Error: Invalid argument(s) in json_write_to_file.\n");
        return false;
    }

    char *jsonString = json_serialize(element);
    if (!jsonString) {
        fmt_fprintf(stderr, "Error: Failed to serialize JSON element in json_write_to_file.\n");
        return false;
    }

    FileWriter* writer = file_writer_open(filename, WRITE_TEXT);
    if (!writer || !file_writer_is_open(writer)) {
        fmt_fprintf(stderr, "Error: Failed to open file '%s' for writing in json_write_to_file.\n", filename);
        free(jsonString);
        return false;
    }

    size_t written = file_writer_write(jsonString, sizeof(char), strlen(jsonString), writer);
    free(jsonString);
    if (written == 0) {
        fmt_fprintf(stderr, "Error: Failed to write to file '%s' in json_write_to_file.\n", filename);
        file_writer_close(writer);
        return false;
    }

    file_writer_close(writer);
    return true;
}

char* json_serialize(const JsonElement* element) {
    String* str = string_create("");
    json_serialize_internal(element, str);

    const char* temp = string_c_str(str);
    char* serialized = string_strdup(temp); // Use string_strdup to create a modifiable copy

    string_deallocate(str); // Deallocate the String object
    return serialized; // Return the duplicated string
}

bool json_compare(const JsonElement *element1, const JsonElement *element2) {
    if (element1 == element2) {
        return true;
    }
    if (element1 == NULL || element2 == NULL) {
        return false;
    }
    if (element1->type != element2->type) {
        return false;
    }

    switch (element1->type) {
        case JSON_NULL:
            return true;
        case JSON_BOOL:
            return element1->value.bool_val == element2->value.bool_val;
        case JSON_NUMBER:
            return element1->value.number_val == element2->value.number_val;
        case JSON_STRING:
            return strcmp(element1->value.string_val, element2->value.string_val) == 0;
        case JSON_ARRAY:
            if (vector_size(element1->value.array_val) != vector_size(element2->value.array_val)) {
                return false;
            }
            for (size_t i = 0; i < vector_size(element1->value.array_val); ++i) {
                JsonElement* item1 = *(JsonElement**)vector_at(element1->value.array_val, i);
                JsonElement* item2 = *(JsonElement**)vector_at(element2->value.array_val, i);
                if (!json_compare(item1, item2)) {
                    return false;
                }
            }
            return true;
        case JSON_OBJECT:
            if (map_size(element1->value.object_val) != map_size(element2->value.object_val)) {
                return false;
            }
            MapIterator it1 = map_begin(element1->value.object_val);
            MapIterator end1 = map_end(element1->value.object_val);
            while (it1.node != end1.node) {
                const char* key = (char*)it1.node->key;
                JsonElement* val1 = (JsonElement*)it1.node->value;
                JsonElement* val2 = (JsonElement*)map_at(element2->value.object_val, (KeyType)key);
                if (!val2 || !json_compare(val1, val2)) {
                    return false;
                }
                map_iterator_increment(&it1);
            }
            return true;
        default:
            fmt_fprintf(stderr, "Error: Unknown JSON type in json_compare.\n");
            return false;
    }
}

bool json_set_element(JsonElement *element, const char *key_or_index, JsonElement *new_element) {
    if (!element || !key_or_index || !new_element) {
        fmt_fprintf(stderr, "Error: Invalid argument(s) in json_set_element.\n");
        return false;
    }

    switch (element->type) {
        case JSON_OBJECT: {
            // For JSON_OBJECT, key_or_index is the key as a string
            char* non_const_key = string_strdup(key_or_index);
            if (!non_const_key) {
                fmt_fprintf(stderr, "Error: Memory allocation failed in json_set_element.\n");
                return false;
            }
            map_insert(element->value.object_val, non_const_key, new_element);
            return true;
        }
        case JSON_ARRAY: {
            // For JSON_ARRAY, key_or_index is the index as a string
            char *end;
            long index = strtol(key_or_index, &end, 10);
            if (end == key_or_index || *end != '\0' || index < 0 || (size_t)index >= vector_size(element->value.array_val)) {
                fmt_fprintf(stderr, "Error: Invalid index '%s' in json_set_element.\n", key_or_index);
                return false;
            }
            // Replace the existing element at the specified index
            JsonElement **array_element = vector_at(element->value.array_val, (size_t)index);
            json_deallocate(*array_element); 
            *array_element = new_element;
            return true;
        }
        default:
            fmt_fprintf(stderr, "Error: Element is not an object or an array in json_set_element.\n");
            return false;
    }
}

bool json_remove_element(JsonElement *element, const char *key_or_index) {
    if (!element || !key_or_index) {
        fmt_fprintf(stderr, "Error: Invalid argument(s) in json_remove_element.\n");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid argument(s) in json_remove_element.\n");
        last_error.code = JSON_ERROR_NONE;
        return false;
    }

    switch (element->type) {
        // Removing an element from a JSON object
        case JSON_OBJECT:{
                char *non_const_key = string_strdup(key_or_index); 
                if (!non_const_key) {
                    fmt_fprintf(stderr, "Error: Memory allocation failed in json_remove_element.\n");
                    snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in json_remove_element.\n");
                    last_error.code = JSON_ERROR_MEMORY;
                    return false;
                }
                bool result = map_erase(element->value.object_val, non_const_key); // Erase the element with the given key
                free(non_const_key); 
                
                return result; // Return the result of the erase operation
            }
            // Removing an element from a JSON array
        case JSON_ARRAY: {
                char *end;
                long index = strtol(key_or_index, &end, 10);
                if (end == key_or_index || *end != '\0' || index < 0 || (size_t)index >= vector_size(element->value.array_val)) {
                    fmt_fprintf(stderr, "Error: Invalid index '%s' in json_remove_element.\n", key_or_index);
                    return false;
                }

                JsonElement **array_element = vector_at(element->value.array_val, (size_t)index); // Get the element at the specified index
                json_deallocate(*array_element); 
                vector_erase(element->value.array_val, (size_t)index, 1); // Erase one element from the vector at the specified index
                
                return true;
            }

        default:
            fmt_fprintf(stderr, "Error: Element is not an object or an array in json_remove_element.\n");
            snprintf(last_error.message, sizeof(last_error.message), "Error: Element is not an object or an array in json_remove_element.\n");
            last_error.code = JSON_ERROR_UNEXPECTED_TOKEN;
            return false;
    }
}

JsonElement* json_find(const JsonElement *element, JsonPredicate predicate, void *user_data) {
    if (!element || !predicate) {
        fmt_fprintf(stderr, "Error: Invalid argument(s) in json_find.\n");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid argument(s) in json_find.\n");
        last_error.code = JSON_ERROR_NONE;
        return NULL;
    }

    JsonElement* found_element = NULL;

    switch (element->type) {
        case JSON_OBJECT:
            if (json_find_in_object(element, predicate, user_data, &found_element)) {
                return found_element;
            }
            break;
        case JSON_ARRAY:
            if (json_find_in_array(element, predicate, user_data, &found_element)) {
                return found_element;
            }
            break;
        default:
            fmt_fprintf(stderr, "Error: Element is not an object or an array in json_find.\n");
            snprintf(last_error.message, sizeof(last_error.message), "Error: Element is not an object or an array in json_find.\n");
            last_error.code = JSON_ERROR_UNEXPECTED_TOKEN;
            break;
    }

    return NULL; // No matching element found
}

JsonError json_last_error() {
    return last_error;
}

JsonElement* json_merge(const JsonElement *element1, const JsonElement *element2) {
    if (!element1 || !element2 || element1->type != JSON_OBJECT || element2->type != JSON_OBJECT) {
        fmt_fprintf(stderr, "Error: Both elements must be JSON objects in json_merge.\n");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Both elements must be JSON objects in json_merge.\n");
        last_error.code = JSON_ERROR_NONE;
        return NULL;
    }

    JsonElement* merged = json_create(JSON_OBJECT);
    if (!merged) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in json_merge.\n");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in json_merge.\n");
        last_error.code = JSON_ERROR_MEMORY;
        return NULL;
    }

    // Insert all elements from the first object into the merged object
    MapIterator it = map_begin(element1->value.object_val);
    MapIterator end = map_end(element1->value.object_val);
    while (it.node != end.node) {
        char* key = string_strdup((char*)it.node->key);
        if (!key) {
            fmt_fprintf(stderr, "Error: Memory allocation failed for key in json_merge.\n");
            snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for key in json_merge.\n");
            last_error.code = JSON_ERROR_MEMORY;
            json_deallocate(merged);
            return NULL;
        }

        JsonElement* value_copy = json_deep_copy((JsonElement*)it.node->value);
        if (!value_copy) {
            free(key);
            json_deallocate(merged);
            return NULL;
        }

        map_insert(merged->value.object_val, key, value_copy);
        map_iterator_increment(&it);
    }

    // Merge elements from the second object
    it = map_begin(element2->value.object_val);
    end = map_end(element2->value.object_val);
    while (it.node != end.node) {
        char* key = string_strdup((char*)it.node->key);
        if (!key) {
            fmt_fprintf(stderr, "Error: Memory allocation failed for key in json_merge (second object).\n");
            snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for key in json_merge (second object).\n");
            last_error.code = JSON_ERROR_MEMORY;
            json_deallocate(merged);
            return NULL;
        }

        JsonElement* value_copy = json_deep_copy((JsonElement*)it.node->value);
        if (!value_copy) {
            free(key);
            json_deallocate(merged);
            return NULL;
        }
        map_insert(merged->value.object_val, key, value_copy);
        map_iterator_increment(&it);
    }

    return merged;
}

char** json_to_string_array(const JsonElement *array, size_t *length) {
    if (!array || array->type != JSON_ARRAY || !length) {
        fmt_fprintf(stderr, "Error: Invalid input in json_to_string_array.\n");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid input in json_to_string_array.\n");
        last_error.code = JSON_ERROR_NONE;
        return NULL;
    }

    *length = vector_size(array->value.array_val); 
    if (*length == 0) {
        fmt_fprintf(stderr, "Error: Size of String array element in json is zero in json_to_string_array\n");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Size of String array element in json is zero in json_to_string_array\n");
        last_error.code = JSON_ERROR_NONE;
        return NULL;
    }

    char **string_array = malloc(*length * sizeof(char*));
    if (!string_array) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in json_to_string_array.\n");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in json_to_string_array.\n");
        last_error.code = JSON_ERROR_MEMORY;
        *length = 0;
        return NULL;
    }

    for (size_t i = 0; i < *length; ++i) {
        JsonElement *element = *(JsonElement**)vector_at(array->value.array_val, i);
        if (!element || element->type != JSON_STRING) {
            // If any element is not a string, deallocate and return NULL
            fmt_fprintf(stderr, "Error: Non-string element found in json_to_string_array at index %zu.\n", i);
            while (i > 0) {
                free(string_array[--i]);
            }
            free(string_array);
            *length = 0;
            return NULL;
        }
        string_array[i] = string_strdup(element->value.string_val);
    }

    return string_array;
}

void* json_convert(const JsonElement *element, JsonType type) {
    if (!element) {
        snprintf(last_error.message, sizeof(last_error.message), "Error: The provided JsonElement is NULL in json_convert.\n");
        last_error.code = JSON_ERROR_NONE;
        fmt_fprintf(stderr, last_error.message);
        return NULL;
    }

    switch (type) {
        case JSON_STRING:
            if (element->type == JSON_NUMBER) {
                JsonElement *stringElement = json_create(JSON_STRING);
                if (!stringElement) {
                    snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for JSON_STRING element in json_convert.\n");
                    last_error.code = JSON_ERROR_MEMORY;
                    fmt_fprintf(stderr, last_error.message);
                    return NULL;
                }
                char *str = (char*)malloc(64 * sizeof(char));
                if (!str) {
                    snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for number to string buffer in json_convert.\n");
                    last_error.code = JSON_ERROR_MEMORY;
                    fmt_fprintf(stderr, last_error.message);
                    json_deallocate(stringElement);
                    return NULL;
                }
                snprintf(str, 64, "%g", element->value.number_val);
                stringElement->value.string_val = str;
                return stringElement;
            } 
            else if (element->type == JSON_BOOL) {
                JsonElement *stringElement = json_create(JSON_STRING);
                if (!stringElement) {
                    snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for JSON_BOOL to JSON_STRING conversion in json_convert.\n");
                    last_error.code = JSON_ERROR_MEMORY;
                    fmt_fprintf(stderr, last_error.message);
                    return NULL;
                }
                if (!stringElement->value.string_val) {
                    snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for string duplication in json_convert.\n");
                    last_error.code = JSON_ERROR_MEMORY;
                    fmt_fprintf(stderr, last_error.message);
                    json_deallocate(stringElement);
                    return NULL;
                }
                stringElement->value.string_val = string_strdup(element->value.bool_val ? "true" : "false");
                return stringElement;
            }
            fmt_fprintf(stderr, "Error: Conversion to string is not supported for the given type in json_convert.\n");
            snprintf(last_error.message, sizeof(last_error.message), "Error: Conversion to string is not supported for the given type in json_convert.\n");
            last_error.code = JSON_ERROR_UNEXPECTED_TOKEN;
            return NULL;
        case JSON_NUMBER:
            if (element->type == JSON_STRING) {
                double *num = (double*)malloc(sizeof(double));
                if (!num) {
                    snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for string to number conversion in json_convert.\n");
                    last_error.code = JSON_ERROR_MEMORY;
                    fmt_fprintf(stderr, last_error.message);
                    return NULL;
                }
                *num = strtod(element->value.string_val, NULL);
                return num;
            }
            snprintf(last_error.message, sizeof(last_error.message), "Error: Conversion to number is not supported for the given type in json_convert.\n");
            last_error.code = JSON_ERROR_UNEXPECTED_TOKEN;
            fmt_fprintf(stderr, last_error.message);
            return NULL;
        case JSON_BOOL:
            if (element->type == JSON_NUMBER) {
                bool *bool_val = (bool*)malloc(sizeof(bool));
                if (!bool_val) {
                    snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for number to boolean conversion in json_convert.\n");
                    last_error.code = JSON_ERROR_MEMORY;
                    fmt_fprintf(stderr, last_error.message);
                    return NULL;
                }
                *bool_val = (element->value.number_val != 0); // Non-zero numbers are true
                return bool_val;
            }
            snprintf(last_error.message, sizeof(last_error.message), "Error: Conversion to boolean is not supported for the given type in json_convert.\n");
            last_error.code = JSON_ERROR_UNEXPECTED_TOKEN;
            fmt_fprintf(stderr, last_error.message);
            return NULL;
        case JSON_ARRAY: {
            JsonElement *array = json_create(JSON_ARRAY);
            if (!array) {
                snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in json_convert for array.\n");
                last_error.code = JSON_ERROR_MEMORY;
                fmt_fprintf(stderr, last_error.message);
                return NULL;
            }

            switch (element->type) {
                case JSON_STRING: {
                    size_t i = 0;
                    size_t len = strlen(element->value.string_val);
                    while (i < len) {
                        size_t char_len = string_utf8_char_len(element->value.string_val[i]);
                        if (char_len == 0 || i + char_len > len) { // Invalid UTF-8
                            json_deallocate(array);
                            snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid UTF-8 in json_convert.\n");
                            last_error.code = JSON_ERROR_SYNTAX;
                            fmt_fprintf(stderr, last_error.message);
                            return NULL;
                        }
                        char *utf8_char = (char*)malloc((char_len + 1) * sizeof(char));
                        if (!utf8_char) {
                            json_deallocate(array);
                            snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in json_convert for UTF-8 character.\n");
                            last_error.code = JSON_ERROR_MEMORY;
                            fmt_fprintf(stderr, last_error.message);
                            return NULL;
                        }

                        strncpy(utf8_char, element->value.string_val + i, char_len);
                        utf8_char[char_len] = '\0';
                        JsonElement *charElement = json_create(JSON_STRING);
                        charElement->value.string_val = utf8_char;
                        vector_push_back(array->value.array_val, &charElement);

                        i += char_len;
                    }
                    break;
                }
                case JSON_NUMBER: {
                    JsonElement *numElement = json_create(JSON_NUMBER);
                    numElement->value.number_val = element->value.number_val;
                    vector_push_back(array->value.array_val, &numElement);
                    break;
                }

                case JSON_BOOL: {
                    JsonElement *boolElement = json_create(JSON_BOOL);
                    boolElement->value.bool_val = element->value.bool_val;
                    vector_push_back(array->value.array_val, &boolElement);
                    break;
                }

                case JSON_NULL: {
                    JsonElement *nullElement = json_create(JSON_NULL);
                    vector_push_back(array->value.array_val, &nullElement);
                    break;
                }

                case JSON_OBJECT: {
                    JsonElement *objElement = json_deep_copy(element); // Assuming json_deep_copy exists
                    vector_push_back(array->value.array_val, &objElement);
                    break;
                }

                case JSON_ARRAY: // Handle nested arrays
                default: { // Treat any other types as a nested array
                    JsonElement *arrElement = json_deep_copy(element); // Assuming json_deep_copy exists
                    vector_push_back(array->value.array_val, &arrElement);
                    break;
                }
            }
            return array;
            break;
        }
        case JSON_OBJECT: {
            JsonElement *object = json_create(JSON_OBJECT);
            if (!object) {
                snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in json_convert for object.\n");
                last_error.code = JSON_ERROR_MEMORY;
                fmt_fprintf(stderr, last_error.message);
                return NULL;
            }

            char *key = string_strdup("value");  // Common key for all conversions
            JsonElement *newElement = NULL;

            switch (element->type) {
                case JSON_NUMBER:
                    newElement = json_create(JSON_NUMBER);
                    newElement->value.number_val = element->value.number_val;
                    break;
                case JSON_STRING:
                    newElement = json_create(JSON_STRING);
                    newElement->value.string_val = string_strdup(element->value.string_val);
                    break;
                case JSON_BOOL:
                    newElement = json_create(JSON_BOOL);
                    newElement->value.bool_val = element->value.bool_val;
                    break;
                case JSON_NULL:
                    newElement = json_create(JSON_NULL);
                    break;
                default:
                    snprintf(last_error.message, sizeof(last_error.message), "Error: Unsupported element type for object conversion in json_convert.\n");
                    last_error.code = JSON_CREATION_FAILED;
                    fmt_fprintf(stderr, last_error.message);
                    json_deallocate(object);
                    free(key); 
                    return NULL;
            }

            if (!newElement) {
                snprintf(last_error.message, sizeof(last_error.message), "Error: Failed to create new element in json_convert.\n");
                last_error.code = JSON_CREATION_FAILED;
                fmt_fprintf(stderr, last_error.message);
                json_deallocate(object);
                free(key); 
                return NULL;
            }

            map_insert(object->value.object_val, key, newElement);
            return object;
        }
        break;
        default:
            snprintf(last_error.message, sizeof(last_error.message), "Error: Unsupported conversion type in json_convert.\n");
            last_error.code = JSON_ERROR_UNEXPECTED_TOKEN; // Or any other suitable error code
            fmt_fprintf(stderr, last_error.message);
            return NULL;

    }
    return NULL;
}

JsonElement* json_map(const JsonElement* array, JsonMapFunction map_func, void* user_data) {
    if (!array || array->type != JSON_ARRAY || !map_func) {
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid argument(s) in json_map. The input must be a JSON array and map_func must be provided.\n");
        last_error.code = JSON_ERROR_NONE;
        fmt_fprintf(stderr, last_error.message);
        return NULL;
    }

    // Create a new JSON array to hold the result
    JsonElement* resultArray = json_create(JSON_ARRAY);
    if (!resultArray) {
        snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for the result array in json_map.\n");
        last_error.code = JSON_CREATION_FAILED;
        fmt_fprintf(stderr, last_error.message);
        return NULL;
    }

    for (size_t i = 0; i < vector_size(array->value.array_val); ++i) {
        JsonElement* currentElement = *(JsonElement**)vector_at(array->value.array_val, i);
        
        // Apply the transformation function to the current element
        JsonElement* transformedElement = map_func(currentElement, user_data);

        // Check if the transformation was successful
        if (!transformedElement) {
            snprintf(last_error.message, sizeof(last_error.message), "Error: Transformation failed for an element at index %zu in json_map.\n", i);
            last_error.code = JSON_TRANSFORM_FAILED;
            // Deallocate previously transformed elements and the result array
            for (size_t j = 0; j < i; ++j) {
                JsonElement* previousElement = *(JsonElement**)vector_at(resultArray->value.array_val, j);
                json_deallocate(previousElement);
            }
            vector_deallocate(resultArray->value.array_val);
            free(resultArray);
            return NULL;
        }
        vector_push_back(resultArray->value.array_val, &transformedElement);
    }
    return resultArray;
}

JsonElement* json_filter(const JsonElement *array, JsonPredicate predicate, void *user_data) {
    if (!array || array->type != JSON_ARRAY || !predicate) {
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid argument(s) in json_filter. The input must be a JSON array and predicate must be provided.\n");
        last_error.code = JSON_ERROR_NONE;
        fmt_fprintf(stderr, last_error.message);
        return NULL;
    }

    JsonElement* resultArray = json_create(JSON_ARRAY);
    if (!resultArray) {
        snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for the result array in json_filter.\n");
        last_error.code = JSON_CREATION_FAILED;
        fmt_fprintf(stderr, last_error.message);
        return NULL;
    }

    for (size_t i = 0; i < vector_size(array->value.array_val); ++i) {
        JsonElement* currentElement = *(JsonElement**)vector_at(array->value.array_val, i);

        // Apply the predicate function to the current element
        if (predicate(currentElement, user_data)) {
            // If predicate returns true, add the element to the result array
            JsonElement* elementCopy = json_deep_copy(currentElement);
            if (!elementCopy) {
                snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed during deep copy in json_filter.\n");
                last_error.code = JSON_ERROR_MEMORY;
                // Handle memory allocation failure
                fmt_fprintf(stderr, last_error.message);
                // Deallocate previously added elements and the result array
                for (size_t j = 0; j < vector_size(resultArray->value.array_val); ++j) {
                    JsonElement* previousElement = *(JsonElement**)vector_at(resultArray->value.array_val, j);
                    json_deallocate(previousElement);
                }
                vector_deallocate(resultArray->value.array_val);
                free(resultArray);
                return NULL;
            }
            vector_push_back(resultArray->value.array_val, &elementCopy);
        }
    }
    return resultArray;
}

void* json_reduce(const JsonElement *array, JsonReduceFunction reduce_func, void *initial_value, void *user_data) {
    if (!array || array->type != JSON_ARRAY || !reduce_func) {
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid argument(s) in json_reduce.\n");
        last_error.code = JSON_ERROR_NONE;
        fmt_fprintf(stderr, last_error.message);
        return NULL;
    }

    void* accumulator = initial_value;

    for (size_t i = 0; i < vector_size(array->value.array_val); ++i) {
        JsonElement* currentElement = *(JsonElement**)vector_at(array->value.array_val, i);
        accumulator = reduce_func(currentElement, accumulator, user_data);
    }

    return accumulator;
}

char* json_format(const JsonElement *element) {
    if (!element) {
        snprintf(last_error.message, sizeof(last_error.message), "Error: The provided JsonElement is NULL in json_format.\n");
        last_error.code = JSON_ERROR_NONE;
        fmt_fprintf(stderr, last_error.message);
        return NULL;
    }

    String* str = string_create("");
    json_format_internal(element, str, 0);

    const char* temp = string_c_str(str);
    char* formatted = string_strdup(temp); // Use string_strdup to create a modifiable copy

    string_deallocate(str); // Deallocate the String object
    return formatted; // Return the duplicated string
}

JsonElement* json_clone(const JsonElement *element) {
    if (!element) {
        snprintf(last_error.message, sizeof(last_error.message), "Error: NULL input to json_clone.");
        fmt_fprintf(stderr, last_error.message);
        last_error.code = JSON_ERROR_NONE;
        return NULL;
    }

    JsonElement *clonedElement = (JsonElement*)malloc(sizeof(JsonElement));
    if (!clonedElement) {
        snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in json_clone.");
        fmt_fprintf(stderr, last_error.message);
        last_error.code = JSON_ERROR_MEMORY;
        return NULL;
    }

    clonedElement->type = element->type;

    switch (element->type) {
        case JSON_NULL:
        case JSON_BOOL:
        case JSON_NUMBER:
            clonedElement->value = element->value;
            break;
        case JSON_STRING:
            clonedElement->value.string_val = string_strdup(element->value.string_val);
            if (!clonedElement->value.string_val) {
                free(clonedElement);
                snprintf(last_error.message, sizeof(last_error.message), "Error: String duplication failed in json_clone.");
                fmt_fprintf(stderr, last_error.message);
                last_error.code = JSON_ERROR_MEMORY;
                return NULL;
            }
            break;
        case JSON_ARRAY:
        case JSON_OBJECT:
            // Shallow copy; just copy the reference
            clonedElement->value = element->value;
            break;
        default:
            free(clonedElement);
            snprintf(last_error.message, sizeof(last_error.message), "Error: Unknown type in json_clone.");
            return NULL;
    }

    return clonedElement;
}

char** json_get_keys(const JsonElement *object, size_t *num_keys) {
    if (!object || object->type != JSON_OBJECT || !num_keys) {
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid input in json_get_keys.\n");
        last_error.code = JSON_ERROR_NONE;
        fmt_fprintf(stderr, last_error.message);
        return NULL;
    }

    *num_keys = map_size(object->value.object_val);
    char** keys = (char**)malloc(*num_keys * sizeof(char*));
    if (!keys) {
        snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in json_get_keys.\n");
        last_error.code = JSON_ERROR_MEMORY;
        fmt_fprintf(stderr, last_error.message);
        return NULL;
    }

    size_t i = 0;
    MapIterator it = map_begin(object->value.object_val);
    MapIterator end = map_end(object->value.object_val);
    while (it.node != end.node) {
        keys[i] = string_strdup((char*)it.node->key);
        if (!keys[i]) {
            snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for key duplication in json_get_keys.\n");
            last_error.code = JSON_ERROR_MEMORY;
            fmt_fprintf(stderr, last_error.message);
            // Deallocate previously allocated keys
            while (i > 0) {
                free(keys[--i]);
            }
            free(keys);
            return NULL;
        }
        i++;
        map_iterator_increment(&it);
    }

    return keys;
}

bool json_add_to_array(JsonElement* element1, JsonElement* element2) {
    if (!element1) {
        snprintf(last_error.message, sizeof(last_error.message), "Error: element1 is NULL and invalid in json_add_to_array.\n");
        last_error.code = JSON_ERROR_NONE;
        fmt_fprintf(stderr, last_error.message);
        return false;
    }
    if (!element2) {
        snprintf(last_error.message, sizeof(last_error.message), "Error: element2 is NULL and invalid in json_add_to_array.\n");
        last_error.code = JSON_ERROR_NONE;
        fmt_fprintf(stderr, last_error.message);
        return false;
    }

    if (vector_push_back(element1->value.array_val, &element2)) {
        return true;
    }
    return false;
}

bool json_add_to_object(JsonElement* object, const char* key, JsonElement* value) {
    if (!object || object->type != JSON_OBJECT) {
        snprintf(last_error.message, sizeof(last_error.message), "Error: Target is not a JSON object in json_add_to_object.\n");
        last_error.code = JSON_ERROR_TYPE;
        fmt_fprintf(stderr, last_error.message);
        return false;
    }
    if (!key) {
        snprintf(last_error.message, sizeof(last_error.message), "Error: Key is NULL in json_add_to_object.\n");
        last_error.code = JSON_ERROR_INVALID_KEY;
        fmt_fprintf(stderr, last_error.message);
        return false;
    }
    if (!value) {
        snprintf(last_error.message, sizeof(last_error.message), "Error: Value is NULL in json_add_to_object.\n");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        fmt_fprintf(stderr, last_error.message);
        return false;
    }

    // Duplicate the key to ensure it is managed independently
    char* duplicatedKey = string_strdup(key);
    if (!duplicatedKey) {
        snprintf(last_error.message, sizeof(last_error.message), "Error: Failed to duplicate key in json_add_to_object.\n");
        last_error.code = JSON_ERROR_MEMORY;
        fmt_fprintf(stderr, last_error.message);
        return false;
    }

    // Check if the key already exists in the object
    JsonElement* existingValue = (JsonElement*)map_at(object->value.object_val, duplicatedKey);
    if (existingValue) {
        // Deallocate the existing value associated with the key
        json_deallocate(existingValue);

        snprintf(last_error.message, sizeof(last_error.message), "Warning: Key already exists and its value will be replaced in json_add_to_object.\n");
        fmt_fprintf(stderr, last_error.message);
    }

    // Replace or insert the key-value pair in the map
    bool insertResult = map_insert(object->value.object_val, duplicatedKey, value);
    if (!insertResult) {
        free(duplicatedKey); 
        if (!existingValue) {
            // Only deallocate the new value if it wasn't replacing an existing value
            json_deallocate(value);
        }
        snprintf(last_error.message, sizeof(last_error.message), "Error: Failed to insert or replace key-value pair in json_add_to_object.\n");
        last_error.code = JSON_ERROR_INSERTION_FAILED;
        fmt_fprintf(stderr, last_error.message);
        return false;
    }

    return true;
}

JsonElement* json_query(const JsonElement *element, const char *query) {
    if (!element || !query) {
        fprintf(stderr, "Invalid argument(s) to json_query.\n");
        return NULL;
    }

    int count;
    char** tokens = split_query(query, &count);
    JsonElement* currentElement = (JsonElement*)element;

    for (int i = 0; i < count; ++i) {
        char* token = tokens[i];

        // Handle array access
        char* bracketPos = strchr(token, '[');
        if (bracketPos) {
            *bracketPos = '\0'; // End the string at the bracket to isolate the key
            char* indexStr = bracketPos + 1;
            char* endBracket = strchr(indexStr, ']');
            if (!endBracket) {
                fprintf(stderr, "Invalid query format. Missing closing bracket.\n");
                break; // Break early on syntax error
            }
            *endBracket = '\0';

            currentElement = json_get_element(currentElement, token); // Get the array or object
            if (json_type_of_element(currentElement) == JSON_ARRAY) {
                currentElement = json_get_element(currentElement, indexStr); // Get the element at index
            } 
            else {
                fprintf(stderr, "Query error: Element is not an array where expected.\n");
                currentElement = NULL;
                break;
            }
        } 
        else {
            currentElement = json_get_element(currentElement, token);
        }

        if (!currentElement) {
            fprintf(stderr, "Query error: Element not found for token %s.\n", token);
            break;
        }
    }

    for (int i = 0; i < count; ++i) {
        free(tokens[i]); // Free the duplicated tokens
    }
    return currentElement;
}
