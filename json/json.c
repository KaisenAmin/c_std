#include "json.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

void json_print_internal(JsonElement* element, int indent) {
    if (!element) {
        fmt_printf("NULL element encountered.\n");
        return;
    }

    fmt_printf("Printing JSON element of type: %d\n", element->type);
    switch (element->type) {
        case JSON_OBJECT: {
            fmt_printf("{\n");
            MapIterator it = map_begin(element->value.object_val);
            while (it.node != map_end(element->value.object_val).node) {
                print_indent(indent); fmt_printf("\"%s\": ", (char*)it.node->key);
                json_print_internal((JsonElement*)it.node->value, indent + 2);
                map_iterator_increment(&it);
                if (it.node != map_end(element->value.object_val).node) fmt_printf(",\n");
            }
            fmt_printf("\n");
            print_indent(indent - 2); fmt_printf("}");
            break;
        }
        case JSON_ARRAY: {
            printf("[\n");
            for (size_t i = 0; i < vector_size(element->value.array_val); ++i) {
                print_indent(indent);
                json_print_internal((JsonElement*)vector_at(element->value.array_val, i), indent + 2);
                if (i < vector_size(element->value.array_val) - 1) fmt_printf(",\n");
            }
            fmt_printf("\n");
            print_indent(indent - 2); fmt_printf("]");
            break;
        }
        case JSON_STRING:
            fmt_printf("\"%s\"", element->value.string_val);
            break;
        case JSON_NUMBER:
            fmt_printf("%g", element->value.number_val);
            break;
        case JSON_BOOL:
            fmt_printf(element->value.bool_val ? "true" : "false");
            break;
        case JSON_NULL:
            fmt_printf("null");
            break;
        default:
            fmt_printf("unknown");
    }
}

void json_deallocate(JsonElement *element) {
    free(element->value.string_val);
    vector_deallocate(element->value.array_val);
    map_deallocate(element->value.object_val);
}

void json_element_deallocator(void* data) {
    JsonElement* element = (JsonElement*)data;
    json_deallocate(element); // Implement this function based on your JsonElement structure
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

    fmt_printf("Current char: %c\n", state->input[state->position]);
    if (state->input[state->position] == '\0') {
        state->current_token.type = JSON_TOKEN_EOF;
        fmt_printf("End of input reached\n");
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
                // state->position += 3;
            }
            else if (strncmp(state->input + state->position, "false", 5) == 0) {
                state->current_token.type = JSON_TOKEN_BOOLEAN;
                // state->position += 4;
            }
            if (strncmp(state->input + state->position, "null", 4) == 0) {
                state->current_token.type = JSON_TOKEN_NULL;
                // state->position += 3;
            }
            // else {
            //     state->current_token.type = JSON_TOKEN_ERROR;
            // }
            break;
    }
    state->position++;
}

static JsonElement* parse_array(JsonParserState* state) {
    fmt_printf("Entering parse_array\n");
    if (state->current_token.type != JSON_TOKEN_ARRAY_START) {
        snprintf(state->error.message, sizeof(state->error.message), "Expected start of array");
        state->error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    JsonElement* array_element = json_create(JSON_ARRAY);
    if (!array_element) {
        return NULL;
    }

    // array_element->value.array_val = vector_create(sizeof(JsonElement));
    // if (!array_element->value.array_val) {
    //     json_deallocate(array_element);
    //     return NULL;
    // }

    next_token(state); // Move to the first element or the end of the array

    while (state->current_token.type != JSON_TOKEN_ARRAY_END) {
        if (state->current_token.type == JSON_TOKEN_COMMA) {
            next_token(state); // Move past the comma
            continue;
        }
        
        JsonElement* elem = parser_internal(state);
        // if (!elem) {
            // // Properly deallocate all elements and the array itself before returning
            // for (size_t i = 0; i < vector_size(array_element->value.array_val); ++i) {
            //     JsonElement* e = (JsonElement*)vector_at(array_element->value.array_val, i);
            //     json_deallocate(e);
            // }
            // json_deallocate(array_element);
        //     return NULL;
        // }

        next_token(state);
        // next_token(state);
        // if (state->current_token.type == JSON_TOKEN_COMMA) {
        //     next_token(state); // Move past the comma
        // }
        // vector_push_back(array_element->value.array_val, elem);

        // JsonElement* value = parser_internal(state);
        // if (!value) {

        //     return NULL;
        // }

        // map_insert(object_element->value.object_val, key, value);

        
        // if (state->current_token.type == JSON_TOKEN_COMMA) {
        //     next_token(state); // Move past the comma
        // }
        // if (state->current_token.type == JSON_TOKEN_STRING) {
        //     elem = parse_string(state);
        //     // vector_push_back(array_element->value.array_val, elem);
        //     next_token(state);
        //     // next_token(state);
        // }
        // if (state->current_token.type == JSON_TOKEN_COMMA) {
        //     next_token(state); // Move past the comma to the next element or the end of the array
        // }
        
         // else if (state->current_token.type != JSON_TOKEN_ARRAY_END) {
        //     // If it's not a comma and not the end of the array, there's a syntax error
        //     snprintf(state->error.message, sizeof(state->error.message), "Expected comma or end of array");
        //     state->error.code = JSON_ERROR_SYNTAX;
        //     json_deallocate(array_element);
        //     return NULL;
        // }
    }

    next_token(state); // Move past the end of the array
    fmt_printf("After Parse Array\n");
    return array_element;
}

static JsonElement* parse_string(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_STRING) {
        fmt_fprintf(stderr, "Error: Expected string token in parse_string.\n");
        return NULL;
    }

    // state->position++; // Skip the opening quote
    size_t start = state->position;

    while (state->input[state->position] != '\"') {
        // Handle escape sequences if needed
        if (state->input[state->position] == '\0') {
            state->error.code = JSON_ERROR_SYNTAX;
            snprintf(state->error.message, sizeof(state->error.message), "Unterminated string");
            return NULL;
        }
        state->position++;
    }

    size_t length = state->position - start;
    char* str_content = (char*)malloc(length + 1);
    if (!str_content) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in parse_string.\n");
        return NULL;
    }
    strncpy(str_content, state->input + start, length);
    str_content[length] = '\0';
    fmt_printf("Str -> %s\n", str_content);
    state->position++; // Skip the closing quote

    JsonElement* element = json_create(JSON_STRING);
    if (!element) {
        free(str_content);
        return NULL;
    }
    element->value.string_val = str_content;
    return element;
}


static JsonElement* parse_number(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_NUMBER) {
        fmt_fprintf(stderr, "Error: Expected Number token in parse_number.\n");
        state->error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    size_t start = state->position - 1;
    while (isdigit((unsigned char)state->input[state->position]) || state->input[state->position] == '.' || state->input[state->position] == 'e' || 
        state->input[state->position] == 'E' || (state->position != start && (state->input[state->position] == '+' || state->input[state->position] == '-'))) {
        state->position++;
    }

    size_t length = state->position - start;
    char* number_str = (char*)malloc(length + 1);
    
    if (!number_str) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in parse_number.\n");
        return NULL;
    }

    strncpy(number_str, state->input + start, length);
    number_str[length] = '\0';
    fmt_printf("Number %s\n", number_str);

    double number_double = atof(number_str);
    free(number_str);

    JsonElement* element = json_create(JSON_NUMBER);
    if (!element) {
        fmt_fprintf(stderr, "Error: failed in json element creation in parse_number.\n");
        return NULL;
    }
    element->value.number_val = number_double;
    return element;
}

static JsonElement* parse_null(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_NULL) {
        fmt_fprintf(stderr, "ErrorL Expected Null token in parse_null");
        state->error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }
    size_t start = state->position - 1;

    if (strncmp(state->input + start, "null", 4) == 0) {
                state->current_token.type = JSON_TOKEN_NULL;
                state->position += 3;
    }

    fmt_printf("Null\n");
    JsonElement* element = json_create(JSON_NULL);
    if (!element) {
        fmt_fprintf(stderr, "Error: failed in json element creation in parse_number.\n");
        return NULL;
    }
    return element;
}

static JsonElement* parse_boolean(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_BOOLEAN) {
        fmt_fprintf(stderr, "Error: Expected Boolean token in parse_boolean");
        state->error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    size_t start = state->position - 1;
    if (strncmp(state->input + start, "true", 4) == 0) {
        state->current_token.type = JSON_TOKEN_BOOLEAN;
        state->position += 3;
    }
    else if (strncmp(state->input + start, "false", 5) == 0) {
        state->current_token.type = JSON_TOKEN_BOOLEAN;
        state->position += 4;
    }


    size_t length = state->position - start; 
    char *boolean_str = (char*) malloc(length);
    
    if (!boolean_str) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in parse_boolean.\n");
        return NULL;
    } 
    strncpy(boolean_str, state->input + start, length);
    boolean_str[length] = '\0';
    fmt_printf("Boolean %s\n", boolean_str);
    bool boolean_value = string_to_bool_from_cstr(boolean_str);

    JsonElement* element = json_create(JSON_BOOL);
    if (!element) {
        fmt_fprintf(stderr, "Error: faile to create json bool value in parse_boolean.\n");
        return NULL;
    }

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
    fmt_printf("Entering parse_object\n");

    if (state->current_token.type != JSON_TOKEN_OBJECT_START) {
        snprintf(state->error.message, sizeof(state->error.message), "Expected start of object");
        state->error.code = JSON_ERROR_SYNTAX;
        fmt_fprintf(stderr, "Error: %s\n", state->error.message);
        return NULL;
    }

    JsonElement* object_element = json_create(JSON_OBJECT);
    if (!object_element) {
        return NULL;
    }

    // object_element->value.object_val = map_create(compare_strings_json, string_deallocator_json, json_element_deallocator);
    // if (!object_element->value.object_val) {
    //     json_deallocate(object_element);
    //     return NULL;
    // }

    next_token(state);
    while (state->current_token.type != JSON_TOKEN_OBJECT_END && state->current_token.type != JSON_TOKEN_EOF) {
        if (state->current_token.type != JSON_TOKEN_STRING) {
            snprintf(state->error.message, sizeof(state->error.message), "Expected string as key in object");
            state->error.code = JSON_ERROR_SYNTAX;
            json_deallocate(object_element);
            return NULL;
        }


        // char* key = parse_string(state)->value.string_val;
        // fmt_printf("Key %s\n", key);
        // next_token(state);

        // if (state->current_token.type != JSON_TOKEN_COLON) {
        //     snprintf(state->error.message, sizeof(state->error.message), "Expected colon after key in object");
        //     state->error.code = JSON_ERROR_SYNTAX;
        //     json_deallocate(object_element);
        //     return NULL;
        // }

        // next_token(state);
        JsonElement* value = parser_internal(state);
        // if (!value) {
        //     free(key);
        //     json_deallocate(object_element);
        //     return NULL;
        // }

        // map_insert(object_element->value.object_val, key, value);

        next_token(state);
        if (state->current_token.type == JSON_TOKEN_COMMA) {
            next_token(state); // Move past the comma
        }
    }

    if (state->current_token.type != JSON_TOKEN_OBJECT_END) {
        snprintf(state->error.message, sizeof(state->error.message), "Expected end of object");
        state->error.code = JSON_ERROR_SYNTAX;
        json_deallocate(object_element);
        return NULL;
    }

    next_token(state); // Move past the object end
    fmt_printf("Exit object parser.\n");
    return object_element;
}

JsonElement* json_create(JsonType type) {
    JsonElement *element = (JsonElement*)malloc(sizeof(JsonElement));
    if (element == NULL) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in json_create.\n");
        return NULL;
    }

    element->type = type;
    switch (type) {
        case JSON_NULL:
            // No additional initialization needed for NULL type
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
            element->value.array_val = NULL;
            break;
        case JSON_OBJECT:
            element->value.object_val = NULL;
            break;
        default:
            fmt_fprintf(stderr, "Error: Unknown type, free allocated memory and return NULL in json_create.\n");
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
    state.input = string_strdup(json_str);  // Why '+ 1'? It skips the first character.
    fmt_printf("Parsing JSON: %s\n", state.input);

    if (!state.input) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in json_parse.\n");
        return NULL;
    }
    state.position = 0;
    state.input_len = string_length_cstr(json_str);
    state.error.code = JSON_ERROR_NONE;
    strcpy(state.error.message, "");

    JsonElement* root = NULL;
    while (state.position < state.input_len && state.current_token.type != JSON_TOKEN_ERROR) {
        next_token(&state);
        fmt_printf("Token type: %d at position %zu\n", state.current_token.type, state.position);

        switch(state.current_token.type) {
            case JSON_TOKEN_ARRAY_START:
                fmt_printf("Found array start.\n");
                root = parse_array(&state);
                break;
            case JSON_TOKEN_OBJECT_START:
                fmt_printf("Found object start.\n");
                root = parse_object(&state);
                break;
            case JSON_TOKEN_BOOLEAN:
                fmt_printf("Found boolean.\n");
                root = parse_boolean(&state);
                break;
            case JSON_TOKEN_NUMBER:
                fmt_printf("Found number.\n");
                root = parse_number(&state);
                break;
            case JSON_TOKEN_STRING:
                fmt_printf("Found string.\n");
                root = parse_string(&state);
                break;
            case JSON_TOKEN_COLON:
                fmt_printf("Found colon.\n");
                break;
            case JSON_TOKEN_COMMA:
                fmt_printf("Found comma.\n");
                break;
            case JSON_TOKEN_NULL:
                fmt_printf("Found null.\n");
                root = parse_null(&state);
                break;
            default:
                next_token(&state); 
                if (state.current_token.type == JSON_TOKEN_EOF || state.current_token.type == JSON_TOKEN_OBJECT_END) {
                    fmt_printf("Finish");
                    return NULL;
                }
                
                fmt_fprintf(stderr, "Error: Unexpected token at position %zu\n", state.position);
                state.error.code = JSON_ERROR_UNEXPECTED_TOKEN;
                strcpy(state.error.message, "Unexpected token at root level");
                free(state.input);
                return NULL;
        }
    }
    if (root == NULL) {
        fmt_printf("Parsed JSON is NULL.\n");
    } 
    else {
        fmt_printf("Successfully parsed JSON.\n");
    }

    return root;
}

JsonElement * json_read_from_file(const char* filename) {
    if (!filename) {
        fmt_fprintf(stderr, "Error: filename is Null and Invalid in json_read_from_file.\n");
        return NULL;
    }

    FileReader *json = file_reader_open(filename, READ_TEXT);
    size_t json_file_size = file_reader_get_size(json);
    char* buffer = (char*) malloc(sizeof(char) * json_file_size + 1);

    if (!buffer) {
        fmt_fprintf(stderr, "Error: Memory allocation faild for buffer in json_read_from_file.\n");
        file_reader_close(json);
        return NULL;
    }   
    
    size_t size = file_reader_read(buffer, sizeof(char), json_file_size, json);
    if (size > 0) {
        buffer[size] = '\0';
        JsonElement* json_element = json_parse(buffer);

        free(buffer);
        file_reader_close(json);
        return json_element;
    } 
    
    fmt_fprintf(stderr, "Error: Can not Read Json file correct in json_read_from_file.\n");
    return NULL;
}

void json_print(JsonElement* element) {
    json_print_internal(element, 0);
}