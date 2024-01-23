#include "json.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../string/string.h"

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
    const char* input;
    size_t input_len;
    size_t position;
    JsonToken current_token;
    JsonError error;
} JsonParserState;

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
            state->position++; // Skip the opening quote

            while (state->input[state->position] != '\"' || 
                (state->input[state->position] == '\"' && state->input[state->position - 1] == '\\')) {
                if (state->input[state->position] == '\0') {
                    // End of input reached before closing quote - syntax error
                    state->current_token.type = JSON_TOKEN_ERROR;
                    snprintf(state->error.message, sizeof(state->error.message), "Unterminated string");
                    state->error.code = JSON_ERROR_SYNTAX;
                    return;
                }
                state->position++;
            }
            // Move past the closing quote
            state->position++;
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
            size_t start = state->position;
            while (isdigit((unsigned char)state->input[state->position]) || 
                state->input[state->position] == '.' || 
                state->input[state->position] == 'e' || 
                state->input[state->position] == 'E' || 
                (state->position != start && (state->input[state->position] == '+' || state->input[state->position] == '-'))) {
                state->position++;
            }
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


static JsonElement* parse_object(JsonParserState* state) {
    // should be implement
}

static JsonElement* parse_array(JsonParserState* state) {
    // should be implement
}

static JsonElement* parse_string(JsonParserState* state) {
    // should be implement
}

static JsonElement* parse_number(JsonParserState* state) {
    // should be implement
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
    state.input = string_strdup(json_str);
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
            case JSON_TOKEN_ARRAY_START:
                root = parse_array(&state);
                break;
            case JSON_TOKEN_OBJECT_START:
                root = parse_object(&state);
                break;
            case JSON_TOKEN_BOOLEAN:
                root = parse_boolean(&state);
                break;
            case JSON_TOKEN_NUMBER:
                root = parse_number(&state);
                break;
            case JSON_TOKEN_STRING:
                root = parse_string(&state);
                break;
            case JSON_TOKEN_NULL:
                root = parse_null(&state);
                break;

            default:
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