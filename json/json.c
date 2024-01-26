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

static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        fmt_printf(" ");
    }
}

void json_print_internal(const JsonElement* element, int indent) {
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
            print_indent(indent);
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
            print_indent(indent);
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

void json_element_deallocator(void* data) {
    if (!data) 
        return;
    JsonElement* element = (JsonElement*)data;
    json_deallocate(element);
}

int compare_strings_json(const KeyType a, const KeyType b) {
    return strcmp((const char*)a, (const char*)b);
}

void string_deallocator_json(void *data) {
    free(data);
} 

void next_token(JsonParserState* state) {
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
        snprintf(state->error.message, sizeof(state->error.message), "Expected start of array at position %zu", state->position);
        state->error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    JsonElement* array_element = json_create(JSON_ARRAY);
    if (!array_element) {
        snprintf(state->error.message, sizeof(state->error.message), "Memory allocation failed for array at position %zu", state->position);
        state->error.code = JSON_ERROR_MEMORY;
        return NULL;
    }
    next_token(state);

    while (state->current_token.type != JSON_TOKEN_ARRAY_END) {
        if (state->current_token.type == JSON_TOKEN_COMMA) {
            next_token(state);
        }
        else if (state->current_token.type == JSON_TOKEN_EOF) {
            snprintf(state->error.message, sizeof(state->error.message), "Unexpected end of input in array at position %zu", state->position);
            state->error.code = JSON_ERROR_SYNTAX;
            json_deallocate(array_element);
            return NULL;
        }
        JsonElement* element = parser_internal(state);
        if (!element) {
            // Propagate the error from the parser_internal
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
        snprintf(state->error.message, sizeof(state->error.message), "Expected string token at position %zu", state->position);
        state->error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }
    size_t start = state->position;

    while (state->input[state->position] != '\"' && state->input[state->position] != '\0') {
        // Handle escape sequences if needed
        if (state->input[state->position] == '\0') {
            state->error.code = JSON_ERROR_SYNTAX;
            snprintf(state->error.message, sizeof(state->error.message), "Unterminated string");
            return NULL;
        }
        state->position++;
    }

    if (state->input[state->position] == '\0') {
        snprintf(state->error.message, sizeof(state->error.message), "Unterminated string at position %zu", start);
        state->error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    size_t length = state->position - start;
    char* str_content = (char*)malloc(length + 1);
    if (!str_content) {
        snprintf(state->error.message, sizeof(state->error.message), "Memory allocation failed for string at position %zu", start);
        state->error.code = JSON_ERROR_MEMORY;
        return NULL;
    }

    strncpy(str_content, state->input + start, length);
    str_content[length] = '\0';
    state->position++; // Skip the closing quote

    JsonElement* element = json_create(JSON_STRING);
    if (!element) {
        snprintf(state->error.message, sizeof(state->error.message), "Failed to create JSON string element at position %zu", start);
        state->error.code = JSON_CREATION_FAILED;
        free(str_content);
        return NULL;
    }

    element->value.string_val = str_content;
    return element;
}

static JsonElement* parse_number(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_NUMBER) {
        snprintf(state->error.message, sizeof(state->error.message), "Expected number token at position %zu", state->position);
        state->error.code = JSON_ERROR_SYNTAX;
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
        snprintf(state->error.message, sizeof(state->error.message), "Memory allocation failed for number string at position %zu", start);
        state->error.code = JSON_ERROR_MEMORY;
        return NULL;
    }

    strncpy(number_str, state->input + start, length);
    number_str[length] = '\0';

    char* endptr;
    double number_double = strtod(number_str, &endptr);
    if (endptr == number_str) {
        snprintf(state->error.message, sizeof(state->error.message), "Invalid number format at position %zu", start);
        state->error.code = JSON_ERROR_SYNTAX;
        free(number_str);
        return NULL;
    }
    free(number_str);

    JsonElement* element = json_create(JSON_NUMBER);
    if (!element) {
        snprintf(state->error.message, sizeof(state->error.message), "Failed to create JSON number element at position %zu", start);
        state->error.code = JSON_CREATION_FAILED; // Ensure this error code is defined
        return NULL;
    }

    element->value.number_val = number_double;
    return element;
}

static JsonElement* parse_null(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_NULL) {
        snprintf(state->error.message, sizeof(state->error.message), "Expected 'null' token at position %zu", state->position);
        state->error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    state->position += 4; // // Skip past 'null'

    JsonElement* element = json_create(JSON_NULL);
    if (!element) {
        snprintf(state->error.message, sizeof(state->error.message), "Failed to create JSON null element at position %zu", state->position - 4);
        state->error.code = JSON_CREATION_FAILED; // Ensure this error code is defined
        return NULL;
    }
    return element;
}

static JsonElement* parse_boolean(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_BOOLEAN) {
        snprintf(state->error.message, sizeof(state->error.message),
                 "Expected boolean token at position %zu", state->position);
        state->error.code = JSON_ERROR_SYNTAX;
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
        snprintf(state->error.message, sizeof(state->error.message), "Invalid boolean format at position %zu", start);
        state->error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    JsonElement* element = json_create(JSON_BOOL);
    if (!element) {
        snprintf(state->error.message, sizeof(state->error.message), "Failed to create JSON boolean element at position %zu", start);
        state->error.code = JSON_CREATION_FAILED; // Ensure this error code is defined
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
            snprintf(state->error.message, sizeof(state->error.message), "Unexpected token");
            state->error.code = JSON_ERROR_UNEXPECTED_TOKEN;
            return NULL;
    }
}

static JsonElement* parse_object(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_OBJECT_START) {
        snprintf(state->error.message, sizeof(state->error.message), "Expected start of object at position %zu", state->position);
        state->error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    JsonElement* object_element = json_create(JSON_OBJECT);
    if (!object_element) {
        snprintf(state->error.message, sizeof(state->error.message), "Failed to create JSON object element at position %zu", state->position);
        state->error.code = JSON_CREATION_FAILED; // Ensure this error code is defined
        return NULL;
    }
    next_token(state);

    while (state->current_token.type != JSON_TOKEN_OBJECT_END) {
        if (state->current_token.type != JSON_TOKEN_STRING) {
            snprintf(state->error.message, sizeof(state->error.message), "Expected string as key in object at position %zu", state->position);
            state->error.code = JSON_ERROR_SYNTAX;
            json_deallocate(object_element);
            return NULL;
        }

        char* key = parse_string(state)->value.string_val;
        next_token(state);
         if (state->current_token.type != JSON_TOKEN_COLON) {
            snprintf(state->error.message, sizeof(state->error.message), "Expected colon after key in object at position %zu", state->position);
            state->error.code = JSON_ERROR_SYNTAX;
            free(key);
            json_deallocate(object_element);
            return NULL;
        }

        next_token(state);
        JsonElement* value = parser_internal(state);
        if (!value) {
            // Propagate the error from parser_internal
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
    state.input = string_strdup(json_str); // Duplicate the string

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
    bool start_object_flag = false;

    JsonElement* root = NULL;

    if (state.current_token.type == JSON_TOKEN_OBJECT_START) {
        root = parse_object(&state);
        start_object_flag = true;
    } 
    else if (state.current_token.type == JSON_TOKEN_ARRAY_START) {
        root = parse_array(&state);
    } 
    else {
        fmt_fprintf(stderr, "Error: Root element must be an object or array at position %zu.\n", state.position);
        free(state.input); // Free the duplicated string
        return NULL;
    }

    if (root == NULL) {
        fmt_fprintf(stderr, "Error while parsing JSON at position %zu: %s\n", state.position, state.error.message);
        free(state.input); // Free the duplicated string
        return NULL;
    }

    if (state.current_token.type != JSON_TOKEN_EOF) {
        if (!start_object_flag && state.input[state.position] != '\n') {
            fmt_fprintf(stderr, "Error: Unexpected data after root element at position %zu. is %c\n", state.position, state.input[state.position]);
            return NULL;
        }
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
    json_print_internal(element, 0);
    fmt_printf("\n"); // New line after complete JSON data is printed
}
