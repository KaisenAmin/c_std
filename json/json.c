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
        case '0' ... '9':
            state->current_token.type = JSON_TOKEN_NUMBER;
            break;
        default:
            if (strncmp(state->input + state->position, "true", 4) == 0) {
                state->current_token.type = JSON_TOKEN_BOOLEAN;
                state->position += 3;
            }
            else if (strncmp(state->input + state->position, "false", 5) == 0) {
                state->current_token.type = JSON_TOKEN_BOOLEAN;
                state->position += 4;
            }
            else if (strncmp(state->input + state->position, "null", 4) == 0) {
                state->current_token.type = JSON_TOKEN_NULL;
                state->position += 3;
            }
            else {
                state->current_token.type = JSON_TOKEN_ERROR;
            }
            break;
    }
    state->position++;
}

static JsonElement* parse_array(JsonParserState* state) {
    // should be implement
}

static JsonElement* parse_string(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_STRING) {
        fmt_fprintf(stderr, "Error: Expected string token in parse_string.\n");
        return NULL;
    }

    // Skip the opening quote, which next_token already identified
    size_t start = state->position;
    while (state->input[state->position] != '\"' || (state->input[state->position] == '\"' && state->input[state->position - 1] == '\\')) {
        if (state->input[state->position] == '\0') {
            // End of input reached before closing quote - syntax error
            state->current_token.type = JSON_TOKEN_ERROR;
            snprintf(state->error.message, sizeof(state->error.message), "Unterminated string");
                state->error.code = JSON_ERROR_SYNTAX;
                return;
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
    fmt_printf("%s\n", str_content);
    state->position++;

    // Create a new JsonElement for the string
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
    fmt_printf("%f\n", number_double);
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
    // should be implement
}

static JsonElement* parse_boolean(JsonParserState* state) {
    // should be implement
}

static JsonElement* parser_internal(JsonParserState* state) {
    // should be implement
}

static JsonElement* parse_object(JsonParserState* state) {
    Map *objec = map_create(compare_strings_json, string_deallocator_json, json_element_deallocator);

    next_token(state);

    while (state->current_token.type != JSON_TOKEN_OBJECT_END) {
        if (state->current_token.type == JSON_TOKEN_STRING) {
            // 
        }
    }
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
    state.input = string_strdup(json_str + 1);
    if (!state.input) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in json_parse.\n");
        return NULL;
    }
    state.position = 0;
    state.input_len = string_c_str(json_str);
    state.error.code = JSON_ERROR_NONE;
    strcpy(state.error.message, "");

    JsonElement* root = NULL;
    while (state.position < state.input_len && state.current_token.type != JSON_TOKEN_ERROR) {
        next_token(&state);

        switch(state.current_token.type) {
            // case JSON_TOKEN_ARRAY_START:
            //     root = parse_array(&state);
            //     break;
            // case JSON_TOKEN_OBJECT_START:
            //     root = parse_object(&state);
            //     break;
            case JSON_TOKEN_BOOLEAN:
                root = parse_boolean(&state);
                break;
            case JSON_TOKEN_NUMBER:
                root = parse_number(&state);
                break;
            case JSON_TOKEN_STRING:
                root = parse_string(&state);
                break;
            case JSON_TOKEN_COLON:
                break;
            case JSON_TOKEN_COMMA:
                break;
            // case JSON_TOKEN_NULL:
            //     root = parse_null(&state);
            //     break;

            default:
                case JSON_TOKEN_ERROR:
                    fmt_fprintf(stderr, "Error: Unexpected token at root level in json_parse.\n");
                    state.error.code = JSON_ERROR_UNEXPECTED_TOKEN;
                    strcpy(state.error.message, "Unexpected token at root level");
                    free(state.input);
                    return NULL;
        }
    }
    return parser_internal(&state);
}

JsonElement * json_read_from_file(const char* filename) {
    if (!filename) {
        fmt_fprintf(stderr, "Error: filename is Null and Invalid in json_read_from_file.\n");
        return NULL;
    }

    FileReader *json = file_reader_open(filename, READ_TEXT);
    size_t json_file_size = file_reader_get_size(json);
    char* buffer = (char*) malloc(sizeof(char) * json_file_size);

    if (!buffer) {
        fmt_fprintf(stderr, "Error: Memory allocation faild for buffer in json_read_from_file.\n");
        file_reader_close(json);
        return NULL;
    }   
    
    size_t size = file_reader_read(buffer, sizeof(char), json_file_size, json);
    if (size > 0) {
        JsonElement* json_element = json_parse(buffer);

        free(buffer);
        file_reader_close(json);
        return json_element;
    } 
    
    fmt_fprintf(stderr, "Error: Can not Read Json file correct in json_read_from_file.\n");
    return NULL;
}