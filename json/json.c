/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Json
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "json.h"
#include "../string/std_string.h"

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
        printf(" ");
    }
}

static void json_print_internal(const JsonElement* element, int indent) {
    if (!element) {
        printf("null");
        return;
    }

    switch (element->type) {
        case JSON_OBJECT:
            printf("{\n");
            MapIterator it = map_begin(element->value.object_val);
            MapIterator end = map_end(element->value.object_val);
            bool first = true;
            while (it.node != end.node) {
                if (!first) {
                    printf(",\n");
                }
                print_indent(indent + 2);
                printf("\"%s\": ", (char*)it.node->key);
                json_print_internal((JsonElement*)it.node->value, indent + 2);
                first = false;
                map_iterator_increment(&it);
            }
            if (!first) {
                printf("\n");
            }
            print_indent(indent - 2);
            printf("}");
            break;
        case JSON_ARRAY:
            printf("[\n");
            for (size_t i = 0; i < vector_size(element->value.array_val); ++i) {
                print_indent(indent + 2);
                json_print_internal(*(JsonElement**)vector_at(element->value.array_val, i), indent + 2);
                if (i < vector_size(element->value.array_val) - 1) {
                    printf(",");
                }
                printf("\n");
            }
            print_indent(indent - 2);
            printf("]");
            break;
        case JSON_STRING:
            printf("\"%s\"", element->value.string_val);
            break;
        case JSON_NUMBER:
            if (floor(element->value.number_val) == element->value.number_val) {
                printf("%d", (int)element->value.number_val);
            } 
            else {
                printf("%.2f", element->value.number_val);
            }
            break;
        case JSON_BOOL:
            printf(element->value.bool_val ? "true" : "false");
            break;
        case JSON_NULL:
            printf("null");
            break;
        default:
            printf("Unknown type");
    }
}

static char** split_query(const char* query, int* count) {
    static char* tokens[256]; // Simple example, real code should dynamically allocate
    int i = 0;

    if (!query) {
        JSON_LOG("[split_query] Error: Null query provided in split_query.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Null query provided in split_query.");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        return NULL;
    }

    char* tempQuery = string_strdup(query); // Duplicate query to avoid modifying the original string
    if (!tempQuery) {
        JSON_LOG("[split_query] Error: Memory allocation failed for temporary query in split_query.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in split_query.");
        last_error.code = JSON_ERROR_MEMORY;
        return NULL;
    }

    char* token = strtok(tempQuery, ".");
    while (token != NULL && i < 256) {
        tokens[i++] = string_strdup(token); 
        if (!tokens[i - 1]) {
            JSON_LOG("[split_query] Error: Memory allocation failed for token in split_query.");
            snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in split_query.");
            last_error.code = JSON_ERROR_MEMORY;
            free(tempQuery);
            return NULL;
        }
        token = strtok(NULL, ".");
    }

    *count = i;
    free(tempQuery); // Free the duplicated query
    return tokens;
}

/**
 * @brief Deallocates a JSON element and all of its associated resources.
 * 
 * @param element A pointer to the `JsonElement` to be deallocated. If the pointer is NULL, the function does nothing.
 * 
 * @details This function recursively deallocates the memory used by a JSON element, including any nested elements 
 *          in arrays or objects. It handles the following types:
 *          - `JSON_STRING`: Frees the string value.
 *          - `JSON_ARRAY`: Recursively deallocates each element in the array, then deallocates the array itself.
 *          - `JSON_OBJECT`: Deallocates the entire object map, including all key-value pairs.
 *          - For other types (`JSON_NULL`, `JSON_BOOL`, `JSON_NUMBER`), no special handling is needed.
 * 
 * @warning After calling this function, the `JsonElement` pointer should not be used again, as the memory it points to 
 *          has been freed. Using a freed pointer may result in undefined behavior.
 * 
 * @note It is assumed that `map_deallocate` and `vector_deallocate` correctly handle the deallocation of their contents.
 */
void json_deallocate(JsonElement *element) {
    if (!element) {
        JSON_LOG("[json_deallocate] Warning: Attempted to deallocate a NULL element in json_deallocate.");
        return;
    }

    switch (element->type) {
        case JSON_STRING:
            if (element->value.string_val) {
                free(element->value.string_val);
            } 
            else {
                JSON_LOG("[json_deallocate] Warning: Null string value found in JSON_STRING type during deallocation.");
            }
            break;

        case JSON_ARRAY:
            if (element->value.array_val) {
                for (size_t i = 0; i < vector_size(element->value.array_val); ++i) {
                    JsonElement* e = *(JsonElement**)vector_at(element->value.array_val, i);
                    json_deallocate(e);
                }
                vector_deallocate(element->value.array_val);
            } 
            else {
                JSON_LOG("[json_deallocate] Warning: Null array value found in JSON_ARRAY type during deallocation.");
            }
            break;

        case JSON_OBJECT:
            if (element->value.object_val) {
                map_deallocate(element->value.object_val);
            } 
            else {
                JSON_LOG("[json_deallocate] Warning: Null object value found in JSON_OBJECT type during deallocation.");
            }
            break;

        default:
            JSON_LOG("[json_deallocate] Notice: No special handling required for JSON type %d in json_deallocate.", element->type);
            break;
    }
    free(element);
}

/**
 * @brief Iterates through a JSON object to find an element that matches a given predicate.
 * @return True if a matching element is found, otherwise false.
 */
static bool json_find_in_object(const JsonElement* object, JsonPredicate predicate, void* user_data, JsonElement** found_element) {
    if (!object || object->type != JSON_OBJECT) {
        JSON_LOG("[json_find_in_object] Error: The provided element is NULL or not a JSON_OBJECT in json_find_in_object.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid element or type in json_find_in_object.");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        return false;
    }
    if (!predicate) {
        JSON_LOG("[json_find_in_object] Error: Predicate function is NULL in json_find_in_object.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Predicate function is NULL in json_find_in_object.");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        return false;
    }

    MapIterator it = map_begin(object->value.object_val);
    MapIterator end = map_end(object->value.object_val);

    while (it.node != end.node) {
        JsonElement* current_element = (JsonElement*)it.node->value;
        if (predicate(current_element, user_data)) {
            *found_element = current_element;
            JSON_LOG("[json_find_in_object] Info: Matching element found in json_find_in_object.");
            return true;
        }
        map_iterator_increment(&it);
    }

    JSON_LOG("[json_find_in_object] Info: No matching element found in json_find_in_object.");
    return false;
}

/**
 * @brief Iterates through a JSON array to find an element that matches a given predicate.
 * @return True if a matching element is found, otherwise false.
 */
static bool json_find_in_array(const JsonElement* array, JsonPredicate predicate, void* user_data, JsonElement** found_element) {
    if (!array || array->type != JSON_ARRAY) {
        JSON_LOG("[json_find_in_array] Error: The provided element is NULL or not a JSON_ARRAY in json_find_in_array.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid element or type in json_find_in_array.");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        return false;
    }
    if (!predicate) {
        JSON_LOG("[json_find_in_array] Error: Predicate function is NULL in json_find_in_array.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Predicate function is NULL in json_find_in_array.");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        return false;
    }

    for (size_t i = 0; i < vector_size(array->value.array_val); ++i) {
        JsonElement* current_element = *(JsonElement**)vector_at(array->value.array_val, i);
        if (predicate(current_element, user_data)) {
            *found_element = current_element;
            JSON_LOG("[json_find_in_array] Info: Matching element found in json_find_in_array at index %zu.", i);
            return true;
        }
    }

    JSON_LOG("[json_find_in_array] Info: No matching element found in json_find_in_array.");
    return false;
}

/**
 * @brief Deallocates a JSON element, including its nested elements if applicable.
 * @param data A pointer to the JSON element to be deallocated.
 */
static void json_element_deallocator(void* data) {
    if (!data) {
        JSON_LOG("[json_element_deallocator] Info: Tried to deallocate a NULL pointer in json_element_deallocator.");
        return;
    }
    JsonElement* element = (JsonElement*)data;
    JSON_LOG("[json_element_deallocator] Info: Deallocating JSON element of type %d in json_element_deallocator.", element->type);
    json_deallocate(element);
}

/**
 * @brief Compares two strings used as keys in a JSON object.
 * @return An integer less than, equal to, or greater than zero if `a` is found, respectively, to be less than, equal to, or greater than `b`.
 */
static int compare_strings_json(const KeyType a, const KeyType b) {
    if (!a || !b) {
        JSON_LOG("[compare_strings_json] Error: One or both of the strings are NULL in compare_strings_json.");
        return 0;
    }
    int result = strcmp((const char*)a, (const char*)b);
    JSON_LOG("[compare_strings_json] Info: Comparing strings '%s' and '%s' with result %d in compare_strings_json.", (const char*)a, (const char*)b, result);
    return result;
}

/**
 * @brief Deallocates memory used by a string.
 * @param data A pointer to the string to be deallocated.
 */
static void string_deallocator_json(void *data) {
    if (!data) {
        JSON_LOG("[string_deallocator_json] Info: Tried to deallocate a NULL string in string_deallocator_json.");
        return;
    }
    JSON_LOG("[string_deallocator_json] Info: Deallocating string '%s' in string_deallocator_json.", (char*)data);
    free(data);
}

/**
 * @brief Advances the JSON parser state to the next token in the input string.
 * 
 * This function skips over any whitespace and then identifies the next significant 
 * token in the JSON input, updating the parser state's current token accordingly.
 * 
 * @param state Pointer to the JsonParserState structure that holds the current parsing state.
 */
static void next_token(JsonParserState* state) {
    // Skip whitespace
    while (isspace((unsigned char)state->input[state->position])) {
        state->position++;
    }

    if (state->input[state->position] == '\0') {
        state->current_token.type = JSON_TOKEN_EOF;
        JSON_LOG("[next_token] Reached end of input (EOF) in next_token.");
        return;
    }

    char currentChar = state->input[state->position];
    switch (currentChar) {
        case '{':
            state->current_token.type = JSON_TOKEN_OBJECT_START;
            JSON_LOG("[next_token] Token: '{' (Object Start) detected in next_token at position %zu.", state->position);
            break;
        case '}':
            state->current_token.type = JSON_TOKEN_OBJECT_END;
            JSON_LOG("[next_token] Token: '}' (Object End) detected in next_token at position %zu.", state->position);
            break;
        case '[':
            state->current_token.type = JSON_TOKEN_ARRAY_START;
            JSON_LOG("[next_token] Token: '[' (Array Start) detected in next_token at position %zu.", state->position);
            break;
        case ']':
            state->current_token.type = JSON_TOKEN_ARRAY_END;
            JSON_LOG("[next_token] Token: ']' (Array End) detected in next_token at position %zu.", state->position);
            break;
        case '\"':
            state->current_token.type = JSON_TOKEN_STRING;
            JSON_LOG("[next_token] Token: '\"' (String) detected in next_token at position %zu.", state->position);
            break;
        case ':':
            state->current_token.type = JSON_TOKEN_COLON;
            JSON_LOG("[next_token] Token: ':' (Colon) detected in next_token at position %zu.", state->position);
            break;
        case ',':
            state->current_token.type = JSON_TOKEN_COMMA;
            JSON_LOG("[next_token] Token: ',' (Comma) detected in next_token at position %zu.", state->position);
            break;
        case '-':
        case '+':
            state->current_token.type = JSON_TOKEN_NUMBER;
            JSON_LOG("[next_token] Token: '%c' (Number) detected in next_token at position %zu.", currentChar, state->position);
            break;
        default:
            if (isdigit((unsigned char)currentChar)) {
                state->current_token.type = JSON_TOKEN_NUMBER;
                JSON_LOG("[next_token] Token: Number starting with '%c' detected in next_token at position %zu.", currentChar, state->position);
            } 
            else if (strncmp(state->input + state->position, "true", 4) == 0) {
                state->current_token.type = JSON_TOKEN_BOOLEAN;
                JSON_LOG("[next_token] Token: 'true' (Boolean) detected in next_token at position %zu.", state->position);
            } 
            else if (strncmp(state->input + state->position, "false", 5) == 0) {
                state->current_token.type = JSON_TOKEN_BOOLEAN;
                JSON_LOG("Token: 'false' (Boolean) detected in next_token at position %zu.", state->position);
            } 
            else if (strncmp(state->input + state->position, "null", 4) == 0) {
                state->current_token.type = JSON_TOKEN_NULL;
                JSON_LOG("[next_token] Token: 'null' detected in next_token at position %zu.", state->position);
            } 
            else {
                state->current_token.type = JSON_TOKEN_ERROR;
                JSON_LOG("[next_token] Error: Unexpected token '%c' detected in next_token at position %zu.", currentChar, state->position);
            }
            break;
    }
    
    state->position++;
}

/**
 * @brief Parses a JSON array from the input string.
 * 
 * This function assumes that the current token is the start of a JSON array (`[`).
 * It continues parsing until the matching closing bracket (`]`) is found, or an error occurs.
 * 
 * @return JsonElement* Pointer to a newly created JsonElement representing the parsed array, 
 *         or NULL if an error occurs during parsing.
 */
static JsonElement* parse_array(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_ARRAY_START) {
        snprintf(last_error.message, sizeof(last_error.message), "Expected start of array at position %zu", state->position);
        last_error.code = JSON_ERROR_SYNTAX;
        JSON_LOG("[parse_array] Error: %s", last_error.message);
        return NULL;
    }

    JsonElement* array_element = json_create(JSON_ARRAY);
    if (!array_element) {
        snprintf(last_error.message, sizeof(last_error.message), "Memory allocation failed for array at position %zu", state->position);
        last_error.code = JSON_ERROR_MEMORY;
        JSON_LOG("[parse_array] Error: %s", last_error.message);
        return NULL;
    }
    next_token(state);

    while (state->current_token.type != JSON_TOKEN_ARRAY_END) {
        if (state->current_token.type == JSON_TOKEN_COMMA) {
             JSON_LOG("[parse_array] Found comma at position %zu, expecting next element", state->position);
            next_token(state);
        }
        else if (state->current_token.type == JSON_TOKEN_EOF) {
            snprintf(last_error.message, sizeof(last_error.message), "Unexpected end of input in array at position %zu", state->position);
            last_error.code = JSON_ERROR_SYNTAX;
            JSON_LOG("[parse_array] Error: %s", last_error.message);
            json_deallocate(array_element);
            return NULL;
        }

        JsonElement* element = parser_internal(state);
        if (!element) {
            snprintf(last_error.message, sizeof(last_error.message), "Failed to parse element in array at position %zu", state->position);
            last_error.code = JSON_ERROR_SYNTAX;
            JSON_LOG("[parse_array] Error: %s", last_error.message);
            json_deallocate(array_element);
            return NULL;
        }

        JSON_LOG("[parse_array] Adding element to array at position %zu", state->position);
        vector_push_back(array_element->value.array_val, &element);
        next_token(state);
    }
    JSON_LOG("[parse_array] Finished parsing array at position %zu", state->position);
    return array_element;
}

/**
 * @brief Parses a JSON string from the input string.
 * 
 * This function assumes that the current token is the start of a JSON string (`"`).
 * It reads characters until it finds the closing quote, handling escape sequences as necessary.
 * 
 * @return JsonElement* Pointer to a newly created JsonElement representing the parsed string, 
 *         or NULL if an error occurs during parsing.
 */
static JsonElement* parse_string(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_STRING) {
        snprintf(last_error.message, sizeof(last_error.message), "Expected string token at position %zu", state->position);
        last_error.code = JSON_ERROR_SYNTAX;
        JSON_LOG("[parse_string] %s", last_error.message);
        return NULL;
    }
    size_t start = state->position;

    while (state->input[state->position] != '\"' && state->input[state->position] != '\0') {
        // Handle escape sequences if needed
        if (state->input[state->position] == '\0') {
            last_error.code = JSON_ERROR_SYNTAX;
            snprintf(last_error.message, sizeof(last_error.message), "Unterminated string");
            JSON_LOG("[parse_string] %s", last_error.message);
            return NULL;
        }
        state->position++;
    }

    if (state->input[state->position] == '\0') {
        snprintf(last_error.message, sizeof(last_error.message), "Unterminated string at position %zu", start);
        JSON_LOG("[parse_string] %s", last_error.message);
        last_error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    size_t length = state->position - start;
    char* str_content = (char*)malloc(length + 1);
    if (!str_content) {
        snprintf(last_error.message, sizeof(last_error.message), "Memory allocation failed for string at position %zu", start);
        JSON_LOG("[parse_string] %s", last_error.message);
        last_error.code = JSON_ERROR_MEMORY;
        return NULL;
    }

    strncpy(str_content, state->input + start, length);
    str_content[length] = '\0';
    state->position++; // Skip the closing quote

    JsonElement* element = json_create(JSON_STRING);
    if (!element) {
        snprintf(last_error.message, sizeof(last_error.message), "Failed to create JSON string element at position %zu", start);
        JSON_LOG("[parse_string] %s", last_error.message);
        last_error.code = JSON_CREATION_FAILED;
        free(str_content);
        return NULL;
    }

    element->value.string_val = str_content;
    return element;
}

/**
 * @brief Parses a JSON number from the input string.
 * 
 * This function assumes that the current token is the start of a JSON number.
 * It reads characters that make up the number, handling integers, floating-point numbers, and scientific notation.
 * 
 * @return JsonElement* Pointer to a newly created JsonElement representing the parsed number, 
 *         or NULL if an error occurs during parsing.
 */
static JsonElement* parse_number(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_NUMBER) {
        snprintf(last_error.message, sizeof(last_error.message), "Expected number token at position %zu", state->position);
        JSON_LOG("[parse_string] %s", last_error.message);
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
        JSON_LOG("[parse_string] %s", last_error.message);
        last_error.code = JSON_ERROR_MEMORY;
        return NULL;
    }

    strncpy(number_str, state->input + start, length);
    number_str[length] = '\0';

    char* endptr;
    double number_double = strtod(number_str, &endptr);
    if (endptr == number_str) {
        snprintf(last_error.message, sizeof(last_error.message), "Invalid number format at position %zu", start);
        JSON_LOG("[parse_string] %s", last_error.message);
        last_error.code = JSON_ERROR_SYNTAX;
        free(number_str);
        return NULL;
    }
    free(number_str);

    JsonElement* element = json_create(JSON_NUMBER);
    if (!element) {
        snprintf(last_error.message, sizeof(last_error.message), "Failed to create JSON number element at position %zu", start);
        JSON_LOG("[parse_string] %s", last_error.message);
        last_error.code = JSON_CREATION_FAILED; 
        return NULL;
    }

    JSON_LOG("[parse_string] Finished parsing string");
    element->value.number_val = number_double;
    return element;
}

/**
 * @brief Parses a JSON null value from the input string.
 * 
 * This function assumes that the current token is the literal "null".
 * It advances the parser state past "null" and returns a JSON element representing the null value.
 * 
 * @return JsonElement* Pointer to a newly created JsonElement representing the null value, 
 *         or NULL if an error occurs during parsing.
 */
static JsonElement* parse_null(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_NULL) {
        snprintf(last_error.message, sizeof(last_error.message), "Expected 'null' token at position %zu", state->position);
        JSON_LOG("[parse_null] %s", last_error.message);
        last_error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    state->position += 4; // // Skip past 'null'

    JsonElement* element = json_create(JSON_NULL);
    if (!element) {
        snprintf(last_error.message, sizeof(last_error.message), "Failed to create JSON null element at position %zu", state->position - 4);
        JSON_LOG("[parse_null] %s", last_error.message);
        last_error.code = JSON_CREATION_FAILED; 
        return NULL;
    }
    return element;
}

/**
 * @brief Parses a JSON boolean value from the input string.
 * 
 * This function assumes that the current token is either "true" or "false".
 * It advances the parser state past the boolean value and returns a JSON element representing the boolean.
 * 
 * @return JsonElement* Pointer to a newly created JsonElement representing the boolean value, 
 *         or NULL if an error occurs during parsing.
 */
static JsonElement* parse_boolean(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_BOOLEAN) {
        snprintf(last_error.message, sizeof(last_error.message),
                 "Expected boolean token at position %zu", state->position);
        JSON_LOG("[parse_boolean] %s", last_error.message);
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
        JSON_LOG("[parse_boolean] %s", last_error.message);
        last_error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    JsonElement* element = json_create(JSON_BOOL);
    if (!element) {
        snprintf(last_error.message, sizeof(last_error.message), "Failed to create JSON boolean element at position %zu", start);
        JSON_LOG("[parse_boolean] %s", last_error.message);
        last_error.code = JSON_CREATION_FAILED; 
        return NULL;
    }

    JSON_LOG("[parse_boolean] Finished parse_boolean");
    element->type = JSON_BOOL;
    element->value.bool_val = boolean_value;
    return element;
}

/**
 * @brief Internal parser function to handle different JSON elements based on the current token.
 * 
 * This function is called to parse the current element in the JSON structure. 
 * It delegates to more specific parsing functions depending on the type of the token.
 * 
 * @return JsonElement* Pointer to a newly created JsonElement representing the parsed JSON value, 
 * or NULL if an error occurs during parsing.
 */
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
            JSON_LOG("[parse_internal] %s", last_error.message);
            last_error.code = JSON_ERROR_UNEXPECTED_TOKEN;
            return NULL;
    }
}

/**
 * @brief Parses a JSON object from the input string.
 * 
 * This function assumes that the current token is the start of a JSON object ('{'). 
 * It parses key-value pairs, handling nested objects and arrays as necessary.
 * 
 * @return JsonElement* Pointer to a newly created JsonElement representing the parsed object, 
 * or NULL if an error occurs during parsing.
 */
static JsonElement* parse_object(JsonParserState* state) {
    if (state->current_token.type != JSON_TOKEN_OBJECT_START) {
        snprintf(last_error.message, sizeof(last_error.message), "Expected start of object at position %zu", state->position);
        JSON_LOG("[parse_object] %s", last_error.message);
        last_error.code = JSON_ERROR_SYNTAX;
        return NULL;
    }

    JsonElement* object_element = json_create(JSON_OBJECT);
    if (!object_element) {
        snprintf(last_error.message, sizeof(last_error.message), "Failed to create JSON object element at position %zu", state->position);
        JSON_LOG("[parse_object] %s", last_error.message);
        last_error.code = JSON_CREATION_FAILED; 
        return NULL;
    }
    next_token(state);

    while (state->current_token.type != JSON_TOKEN_OBJECT_END) {
        if (state->current_token.type != JSON_TOKEN_STRING) {
            snprintf(last_error.message, sizeof(last_error.message), "Expected string as key in object at position %zu", state->position);
            JSON_LOG("[parse_object] %s", last_error.message);
            last_error.code = JSON_ERROR_SYNTAX;
            json_deallocate(object_element);
            return NULL;
        }

        char* key = parse_string(state)->value.string_val;
        next_token(state);
         if (state->current_token.type != JSON_TOKEN_COLON) {
            snprintf(last_error.message, sizeof(last_error.message), "Expected colon after key in object at position %zu", state->position);
            JSON_LOG("[parse_object] %s", last_error.message);
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

/**
 * @brief Serializes a string value into a JSON-formatted string.
 * 
 * This function takes a string and appends it to a String object in JSON format, 
 * including the surrounding quotes.
 * 
 * @param value The string to be serialized.
 * @param str Pointer to the String structure where the serialized output is appended.
 */
static void serialize_string(const char* value, String* str) {
    string_append(str, "\"");
    string_append(str, value);
    string_append(str, "\"");
}

/**
 * @brief Serializes a JSON array into a JSON-formatted string.
 * 
 * This function converts a JSON array into its JSON string representation, 
 * including handling nested arrays or objects within the array.
 * 
 * @param element Pointer to the JsonElement representing the array to be serialized.
 * @param str Pointer to the String structure where the serialized output is appended.
 */
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

/**
 * @brief Serializes a JSON object into a JSON-formatted string.
 * 
 * This function converts a JSON object into its JSON string representation, 
 * including handling nested objects or arrays within the object.
 * 
 * @param element Pointer to the JsonElement representing the object to be serialized.
 * @param str Pointer to the String structure where the serialized output is appended.
 */
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

/**
 * @brief Internal function for serializing a JSON element into a JSON-formatted string.
 * 
 * This function serializes different types of JSON elements (objects, arrays, strings, numbers, booleans, and null) 
 * into a JSON-formatted string and appends the result to the provided `String` structure.
 * 
 * @param element Pointer to the JsonElement to be serialized.
 * @param str Pointer to the String structure where the serialized JSON will be appended.
 */
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

/**
 * @brief Appends a specified amount of indentation (spaces) to the JSON string.
 * 
 * This function adds spaces to the `String` structure to create indentation for pretty-printing JSON output.
 * 
 * @param str Pointer to the String structure where the indentation will be appended.
 * @param indent Number of spaces to append for indentation.
 */
static void append_indent(String* str, int indent) {
    for (int i = 0; i < indent; ++i) {
        string_append(str, " ");
    }
}

/**
 * @brief Formats a string value into a JSON-formatted string.
 * 
 * This function wraps the given string value in double quotes and appends it to the provided `String` structure.
 * 
 * @param value The string to be formatted.
 * @param str Pointer to the String structure where the formatted string will be appended.
 */
static void format_string(const char* value, String* str) {
    string_append(str, "\"");
    string_append(str, value);
    string_append(str, "\"");
}

/**
 * @brief Formats a JSON array into a pretty-printed JSON-formatted string.
 * 
 * This function converts a JSON array into its formatted JSON string representation, including proper indentation, 
 * and appends the result to the provided `String` structure.
 * 
 * @param element Pointer to the JsonElement representing the array to be formatted.
 * @param str Pointer to the String structure where the formatted JSON will be appended.
 * @param indent Current indentation level for pretty-printing.
 */
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

/**
 * @brief Formats a JSON object into a pretty-printed JSON-formatted string.
 * 
 * This function converts a JSON object into its formatted JSON string representation, including proper indentation, 
 * and appends the result to the provided `String` structure.
 * 
 * @param element Pointer to the JsonElement representing the object to be formatted.
 * @param str Pointer to the String structure where the formatted JSON will be appended.
 * @param indent Current indentation level for pretty-printing.
 */
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

/**
 * @brief Recursively formats a JSON element into a pretty-printed JSON-formatted string.
 * 
 * This function processes different types of JSON elements (objects, arrays, strings, numbers, booleans, and null),
 * converting them into their pretty-printed JSON string representation, including proper indentation. The formatted 
 * result is appended to the provided `String` structure.
 * 
 * @param element Pointer to the JsonElement to be formatted.
 * @param str Pointer to the String structure where the formatted JSON will be appended.
 * @param indent Current indentation level for pretty-printing.
 */
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
            JSON_LOG("[json_format_internal] %s", last_error.message);
            string_append(str, "unknown");
    }
}

/**
 * @brief Creates a new JSON element of the specified type.
 * 
 * @param type The type of JSON element to create. This can be one of the following:
 *             - `JSON_NULL`
 *             - `JSON_BOOL`
 *             - `JSON_NUMBER`
 *             - `JSON_STRING`
 *             - `JSON_ARRAY`
 *             - `JSON_OBJECT`
 * 
 * @return A pointer to the newly created `JsonElement`, or NULL if memory allocation fails or if an invalid type is provided.
 * 
 * @details This function allocates memory for a `JsonElement` and initializes it according to the specified type. 
 *          Depending on the type, it sets the appropriate initial value:
 *          - `JSON_NULL`: No value is needed.
 *          - `JSON_BOOL`: Initializes to `false`.
 *          - `JSON_NUMBER`: Initializes to `0.0`.
 *          - `JSON_STRING`: Initializes to `NULL`.
 *          - `JSON_ARRAY`: Allocates memory for a dynamic array to hold elements.
 *          - `JSON_OBJECT`: Allocates memory for a map to hold key-value pairs.
 * 
 * @warning The caller is responsible for deallocating the returned `JsonElement` using `json_deallocate` to avoid memory leaks.
 * 
 * @note If an error occurs during memory allocation, an error message is logged, and the function returns NULL.
 */
JsonElement* json_create(JsonType type) {
    JsonElement *element = (JsonElement*)malloc(sizeof(JsonElement));
    if (!element) {
        JSON_LOG("[json_create] Error: Memory allocation failed in json_create.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in json_create.");
        last_error.code = JSON_ERROR_MEMORY;
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
                JSON_LOG("[json_create] Error: Memory allocation failed for JSON array in json_create.");
                snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for JSON array in json_create.");
                last_error.code = JSON_ERROR_MEMORY;
                free(element);
                return NULL;
            }
            break;
        case JSON_OBJECT:
            element->value.object_val = map_create(compare_strings_json, string_deallocator_json, json_element_deallocator);
            if (!element->value.object_val) {
                JSON_LOG("[json_create] Error: Memory allocation failed for JSON object in json_create.");
                snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for JSON object in json_create.");
                last_error.code = JSON_ERROR_MEMORY;
                free(element);
                return NULL;
            }
            break;
        default:
            JSON_LOG("[json_create] Error: Invalid JSON type in json_create.");
            snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid JSON type in json_create.");
            last_error.code = JSON_ERROR_INVALID_VALUE;
            free(element);
            return NULL;
    }
    return element;
}

/**
 * @brief Parses a JSON string and returns the corresponding JSON element tree.
 * 
 * @param json_str The JSON string to be parsed.
 * @return A pointer to the root `JsonElement` of the parsed JSON structure, or NULL if parsing fails.
 * 
 * @details This function takes a JSON-formatted string and parses it into a `JsonElement` structure. It handles 
 * different types of JSON values, including objects, arrays, strings, numbers, booleans, and null values.
 * If an error occurs during parsing, an appropriate error message is logged, and NULL is returned.
 * 
 * @warning The caller is responsible for deallocating the returned `JsonElement` using `json_deallocate` to avoid memory leaks.
 */
JsonElement* json_parse(const char* json_str) {
    if (!json_str) {
        JSON_LOG("[json_parse] Error: JSON string is NULL and invalid in json_parse.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: JSON string is NULL and invalid in json_parse.");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        return NULL;
    }

    JsonParserState state;
    state.input = string_strdup(json_str);

    if (!state.input) {
        JSON_LOG("[json_parse] Error: Memory allocation failed for duplicating JSON string in json_parse.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for duplicating JSON string in json_parse.");
        last_error.code = JSON_ERROR_MEMORY;
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
            JSON_LOG("[json_parse] Error: Invalid JSON value at position %zu in json_parse.", state.position);
            snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid JSON value at position %zu in json_parse.", state.position);
            last_error.code = JSON_ERROR_SYNTAX;
            free(state.input);
            return NULL;
        }
    }

    if (root == NULL) {
        JSON_LOG("[json_parse] Error while parsing JSON at position %zu: %s", state.position, state.error.message);
        snprintf(last_error.message, sizeof(last_error.message), "Error while parsing JSON at position %zu: %s", state.position, state.error.message);
        last_error.code = JSON_ERROR_SYNTAX;
        free(state.input);
        return NULL;
    }

    next_token(&state);

    if (state.current_token.type != JSON_TOKEN_EOF) {
        JSON_LOG("[json_parse] Error: Unexpected data after root element at position %zu. Found character: '%c'.", state.position, state.input[state.position]);
        snprintf(last_error.message, sizeof(last_error.message), "Error: Unexpected data after root element at position %zu. Found character: '%c'.", state.position, state.input[state.position]);
        last_error.code = JSON_ERROR_UNEXPECTED_TOKEN;
        json_deallocate(root);
        free(state.input);
        return NULL;
    }

    free(state.input);
    return root;
}

/**
 * @brief Reads a JSON file and returns the corresponding JSON element tree.
 * 
 * @param filename The path to the JSON file to be read.
 * @return A pointer to the root `JsonElement` of the parsed JSON structure, or NULL if reading or parsing fails.
 * 
 * @details This function reads a JSON file from disk, parses its content into a `JsonElement` structure, and returns a 
 * pointer to the root element. It handles errors related to file operations and parsing, logging appropriate messages.
 * 
 * @warning The caller is responsible for deallocating the returned `JsonElement` using `json_deallocate` to avoid memory leaks.
 */
JsonElement * json_read_from_file(const char* filename) {
    if (!filename) {
        JSON_LOG("[json_read_from_file] Error: filename is NULL and invalid in json_read_from_file.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: filename is NULL and invalid in json_read_from_file.");
        last_error.code = JSON_ERROR_INVALID_KEY;
        return NULL;
    }

    FileReader *json = file_reader_open(filename, READ_TEXT);
    if (!json) {
        JSON_LOG("[json_read_from_file] Error: Failed to open file '%s' in json_read_from_file.", filename);
        snprintf(last_error.message, sizeof(last_error.message), "Error: Failed to open file '%s' in json_read_from_file.", filename);
        last_error.code = JSON_CREATION_FAILED;
        return NULL;
    }

    size_t json_file_size = file_reader_get_size(json);
    char* buffer = (char*) malloc(sizeof(char) * (json_file_size + 1));

    if (!buffer) {
        JSON_LOG("[json_read_from_file] Error: Memory allocation failed for buffer in json_read_from_file.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for buffer in json_read_from_file.");
        last_error.code = JSON_ERROR_MEMORY;
        file_reader_close(json);
        return NULL;
    }

    size_t size = file_reader_read(buffer, sizeof(char), json_file_size, json);
    if (size == 0) {
        JSON_LOG("[json_read_from_file] Error: Cannot read JSON file '%s' in json_read_from_file.", filename);
        snprintf(last_error.message, sizeof(last_error.message), "Error: Cannot read JSON file '%s' in json_read_from_file.", filename);
        last_error.code = JSON_ERROR_INVALID_VALUE;
        free(buffer);
        file_reader_close(json);
        return NULL;
    }

    buffer[size] = '\0';
    JsonElement* json_element = json_parse(buffer);

    if (!json_element) {
        JSON_LOG("[json_read_from_file] Error: Failed to parse JSON data from file '%s' in json_read_from_file.", filename);
        snprintf(last_error.message, sizeof(last_error.message), "Error: Failed to parse JSON data from file '%s' in json_read_from_file.", filename);
        last_error.code = JSON_ERROR_SYNTAX;
    }

    free(buffer);
    file_reader_close(json);
    return json_element;
}

/**
 * @brief Prints a JSON element and its children to the standard output in a formatted manner.
 * 
 * @param element The JSON element to be printed.
 * 
 * @details This function prints the provided `JsonElement` to the standard output, formatting it to be human-readable.
 * It handles different types of JSON values and ensures that the output is properly indented for readability.
 * 
 * @note If the input element is NULL, the function prints "null".
 */
void json_print(const JsonElement* element) {
    if (!element) {
        JSON_LOG("[json_print] Error: The provided JsonElement is NULL in json_print.");
        printf("null\n");
        return;
    }

    JSON_LOG("[json_print] Info: Printing JSON element in json_print.");
    json_print_internal(element, 2);
    printf("\n");
}

/**
 * @brief Retrieves a specific element from a JSON object or array by key or index.
 * 
 * @param element The JSON element, which must be an object or array, from which to retrieve the desired element.
 * @param key_or_index The key (for objects) or index (for arrays) of the element to retrieve.
 * 
 * @return A pointer to the retrieved `JsonElement`, or NULL if the key/index is invalid or if an error occurs.
 * 
 * @details This function accesses a JSON object by key or a JSON array by index, returning the corresponding element.
 * If the input is not an object or array, or if the key/index is invalid, the function logs an error and returns NULL.
 * 
 * @warning The returned element is part of the original structure; modifying it will affect the original JSON structure.
 */
JsonElement* json_get_element(const JsonElement *element, const char *key_or_index) {
    if (!element || !key_or_index) {
        JSON_LOG("[json_get_element] Error: Invalid argument(s) in json_get_element.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid argument(s) in json_get_element.");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        return NULL;
    }

    switch (element->type) {
        case JSON_OBJECT: {
            char* non_const_key = string_strdup(key_or_index);
            if (!non_const_key) {
                JSON_LOG("[json_get_element] Error: Memory allocation failed in json_get_element.");
                snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in json_get_element.");
                last_error.code = JSON_ERROR_MEMORY;
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
                JSON_LOG("[json_get_element] Error: Invalid index '%s' in json_get_element.", key_or_index);
                snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid index '%s' in json_get_element.", key_or_index);
                last_error.code = JSON_ERROR_INVALID_KEY;
                return NULL;
            }
            return *(JsonElement**)vector_at(element->value.array_val, (size_t)index);
        }

        default:
            JSON_LOG("[json_get_element] Error: Attempted to access non-object/non-array JSON element in json_get_element.");
            snprintf(last_error.message, sizeof(last_error.message), "Error: Attempted to access non-object/non-array JSON element in json_get_element.");
            last_error.code = JSON_ERROR_TYPE;
            return NULL;
    }
}

/**
 * @brief Returns the size of a JSON array.
 * 
 * @param array The JSON element representing an array.
 * @return The number of elements in the array, or 0 if the input is not a valid JSON array.
 * 
 * @details This function checks if the provided JSON element is an array. If it is, it returns the number of elements 
 * contained in the array. If the input is not an array or is NULL, an error is logged and 0 is returned.
 * 
 * @warning This function returns 0 if the input is NULL or if the input is not a JSON array.
 */
size_t json_array_size(const JsonElement *array) {
    if (!array) {
        JSON_LOG("[json_array_size] Error: The provided JsonElement is NULL in json_array_size.");
        return 0;
    }
    if (array->type != JSON_ARRAY) {
        JSON_LOG("[json_array_size] Error: The provided JsonElement is not an array in json_array_size.");
        return 0;
    }

    return vector_size(array->value.array_val);
}

/**
 * @brief Returns the size of a JSON object.
 * 
 * @param object The JSON element representing an object.
 * @return The number of key-value pairs in the object, or 0 if the input is not a valid JSON object.
 * 
 * @details This function checks if the provided JSON element is an object. If it is, it returns the number of key-value
 * pairs contained in the object. If the input is not an object or is NULL, an error is logged and 0 is returned.
 * 
 * @warning This function returns 0 if the input is NULL or if the input is not a JSON object.
 */
size_t json_object_size(const JsonElement* object) {
    if (!object) {
        JSON_LOG("[json_object_size] Error: The provided JsonElement is NULL in json_object_size.");
        return 0;
    }
    if (object->type != JSON_OBJECT) {
        JSON_LOG("[json_object_size] Error: The provided JsonElement is not an object in json_object_size.");
        return 0;
    }

    return map_size(object->value.object_val);
}

/**
 * @brief Creates a deep copy of a JSON element.
 * 
 * @param element The JSON element to be copied.
 * @return A new JSON element that is a deep copy of the input element, or NULL if an error occurs.
 * 
 * @details This function creates a deep copy of the given JSON element, recursively copying all nested elements in the case 
 * of arrays and objects. For primitive types (e.g., number, boolean), a direct copy of the value is made.
 * If memory allocation fails during the copy process, NULL is returned, and any partially created elements are deallocated.
 * 
 * @warning The caller is responsible for deallocating the returned JSON element using `json_deallocate` to prevent memory leaks.
 */
JsonElement* json_deep_copy(const JsonElement *element) {
    if (!element) {
        JSON_LOG("[json_deep_copy] Error: The provided JsonElement is NULL in json_deep_copy.");
        return NULL;
    }

    JsonElement *copy = json_create(element->type);
    if (!copy) {
        JSON_LOG("[json_deep_copy] Error: Memory allocation failed in json_deep_copy.");
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
                JSON_LOG("[json_deep_copy] Error: Memory allocation failed for string in json_deep_copy.");
                json_deallocate(copy);
                return NULL;
            }
            break;

        case JSON_ARRAY:
            for (size_t i = 0; i < vector_size(element->value.array_val); ++i) {
                JsonElement *item = json_deep_copy(*(JsonElement **)vector_at(element->value.array_val, i));
                if (!item) {
                    json_deallocate(copy); // Deallocate the partially copied array
                    JSON_LOG("[json_deep_copy] Error: Memory allocation failed while copying array element in json_deep_copy.");
                    return NULL;
                }
                if (!vector_push_back(copy->value.array_val, &item)) {
                    json_deallocate(item);
                    json_deallocate(copy);
                    JSON_LOG("[json_deep_copy] Error: Failed to push array element into new copy in json_deep_copy.");
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
                    JSON_LOG("[json_deep_copy] Error: Memory allocation failed for object key in json_deep_copy.");
                    json_deallocate(copy);
                    return NULL;
                }

                JsonElement *val_copy = json_deep_copy((JsonElement *)it.node->value);
                if (!val_copy) {
                    free(key);
                    json_deallocate(copy);
                    JSON_LOG("[json_deep_copy] Error: Memory allocation failed for object value in json_deep_copy.");
                    return NULL;
                }

                map_insert(copy->value.object_val, key, val_copy);
                map_iterator_increment(&it);
            }
            break;

        default:
            JSON_LOG("[json_deep_copy] Error: Unknown JSON type in json_deep_copy.");
            json_deallocate(copy);
            return NULL;
    }

    return copy;
}

/**
 * @brief Returns the type of a JSON element.
 * 
 * @param element The JSON element whose type is to be determined.
 * @return The type of the JSON element, or JSON_NULL if the input is NULL.
 * 
 * @details This function returns the type of the provided JSON element. If the input element is NULL, the function returns JSON_NULL.
 * This function does not distinguish between a JSON_NULL type and a NULL input.
 */
JsonType json_type_of_element(const JsonElement *element) {
    if (!element) {
        JSON_LOG("[json_type_of_element] Error: The provided JsonElement is NULL in json_type_of_element.");
        return JSON_NULL; // or another designated error value
    }
    return element->type;
}

/**
 * @brief Writes a JSON element to a file in serialized JSON format.
 * 
 * @param element The JSON element to serialize and write to a file.
 * @param filename The name of the file to which the JSON data will be written.
 * 
 * @return true if the JSON element was successfully written to the file; false otherwise.
 * 
 * @details This function serializes the given JSON element and writes the resulting string to a specified file.
 * If the file cannot be opened or if the serialization fails, the function returns false and logs an error.
 * 
 * @note The caller is responsible for ensuring that the `element` and `filename` parameters are valid.
 * 
 * @warning The function will overwrite the file if it already exists.
 */
bool json_write_to_file(const JsonElement *element, const char *filename) {
    if (!element || !filename) {
        JSON_LOG("[json_write_to_file] Error: Invalid argument(s) in json_write_to_file.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid argument(s) in json_write_to_file.");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        return false;
    }

    char *jsonString = json_serialize(element);
    if (!jsonString) {
        JSON_LOG("[json_write_to_file] Error: Failed to serialize JSON element in json_write_to_file.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Failed to serialize JSON element in json_write_to_file.");
        last_error.code = JSON_TRANSFORM_FAILED;
        return false;
    }

    FileWriter* writer = file_writer_open(filename, WRITE_TEXT);
    if (!writer || !file_writer_is_open(writer)) {
        JSON_LOG("[json_write_to_file] Error: Failed to open file '%s' for writing in json_write_to_file.", filename);
        snprintf(last_error.message, sizeof(last_error.message), "Error: Failed to open file '%s' for writing in json_write_to_file.", filename);
        last_error.code = JSON_CREATION_FAILED;
        free(jsonString);
        return false;
    }

    size_t written = file_writer_write(jsonString, sizeof(char), strlen(jsonString), writer);
    free(jsonString);
    if (written == 0) {
        JSON_LOG("[json_write_to_file] Error: Failed to write to file '%s' in json_write_to_file.", filename);
        snprintf(last_error.message, sizeof(last_error.message), "Error: Failed to write to file '%s' in json_write_to_file.", filename);
        last_error.code = JSON_ERROR_MEMORY;
        file_writer_close(writer);
        return false;
    }

    file_writer_close(writer);
    return true;
}

/**
 * @brief Serializes a JSON element into a JSON-formatted string.
 * 
 * @param element The JSON element to serialize.
 * @return A dynamically allocated string containing the serialized JSON. The caller is responsible for freeing this string.
 * 
 * @details This function converts a JSON element into its string representation in JSON format.
 * The returned string is a modifiable copy that must be freed by the caller to avoid memory leaks.
 * 
 * @warning The caller is responsible for freeing the returned string.
 */
char* json_serialize(const JsonElement* element) {
    if (!element) {
        JSON_LOG("[json_serialize] Error: The provided JsonElement is NULL in json_serialize.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: The provided JsonElement is NULL in json_serialize.");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        return NULL;
    }

    String* str = string_create("");
    if (!str) {
        JSON_LOG("[json_serialize] Error: Memory allocation failed for string creation in json_serialize.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for string creation in json_serialize.");
        last_error.code = JSON_ERROR_MEMORY;
        return NULL;
    }

    json_serialize_internal(element, str);

    const char* temp = string_c_str(str);
    char* serialized = string_strdup(temp); // Create a modifiable copy

    if (!serialized) {
        JSON_LOG("[json_serialize] Error: Memory allocation failed for string duplication in json_serialize.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for string duplication in json_serialize.");
        last_error.code = JSON_ERROR_MEMORY;
        string_deallocate(str);
        return NULL;
    }

    string_deallocate(str); 
    return serialized; 
}

/**
 * @brief Compares two JSON elements for equality.
 * 
 * @param element1 The first JSON element to compare.
 * @param element2 The second JSON element to compare.
 * 
 * @return true if the JSON elements are equal; false otherwise.
 * 
 * @details This function compares two JSON elements, considering their type and content. The comparison is deep, meaning
 * that nested elements (arrays, objects) are also compared recursively.
 * 
 * @note JSON elements of different types are considered unequal. Two NULL elements are considered equal.
 */
bool json_compare(const JsonElement *element1, const JsonElement *element2) {
    if (element1 == element2) {
        JSON_LOG("[json_compare] Elements are identical.");
        return true;
    }
    if (element1 == NULL || element2 == NULL) {
        JSON_LOG("[json_compare] One or both elements are NULL.");
        return false;
    }
    if (element1->type != element2->type) {
        JSON_LOG("[json_compare] Element types differ: %d vs %d.", element1->type, element2->type);
        return false;
    }

    switch (element1->type) {
        case JSON_NULL:
            JSON_LOG("[json_compare] Comparing JSON_NULL types.");
            return true;
        case JSON_BOOL:
            JSON_LOG("[json_compare] Comparing JSON_BOOL values.");
            return element1->value.bool_val == element2->value.bool_val;
        case JSON_NUMBER:
            JSON_LOG("[json_compare] Comparing JSON_NUMBER values: %f vs %f.", element1->value.number_val, element2->value.number_val);
            return element1->value.number_val == element2->value.number_val;
        case JSON_STRING:
            JSON_LOG("[json_compare] Comparing JSON_STRING values: \"%s\" vs \"%s\".", element1->value.string_val, element2->value.string_val);
            return strcmp(element1->value.string_val, element2->value.string_val) == 0;
        case JSON_ARRAY:
            JSON_LOG("[json_compare] Comparing JSON_ARRAY sizes: %zu vs %zu.", vector_size(element1->value.array_val), vector_size(element2->value.array_val));
            if (vector_size(element1->value.array_val) != vector_size(element2->value.array_val)) {
                return false;
            }
            for (size_t i = 0; i < vector_size(element1->value.array_val); ++i) {
                JsonElement* item1 = *(JsonElement**)vector_at(element1->value.array_val, i);
                JsonElement* item2 = *(JsonElement**)vector_at(element2->value.array_val, i);
                if (!json_compare(item1, item2)) {
                    JSON_LOG("[json_compare] Array elements at index %zu differ.", i);
                    return false;
                }
            }
            return true;
        case JSON_OBJECT:
            JSON_LOG("[json_compare] Comparing JSON_OBJECT sizes: %zu vs %zu.", map_size(element1->value.object_val), map_size(element2->value.object_val));
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
                    JSON_LOG("[json_compare] Object values for key \"%s\" differ.", key);
                    return false;
                }
                map_iterator_increment(&it1);
            }
            return true;
        default:
            JSON_LOG("[json_compare] Error: Unknown JSON type %d.", element1->type);
            fprintf(stderr, "Error: Unknown JSON type in json_compare.\n");
            return false;
    }
}

/**
 * @brief Sets a value in a JSON object or array at a specified key or index.
 * 
 * @param element The JSON object or array in which to set the value.
 * @param key_or_index The key (for objects) or index (for arrays) where the value should be set.
 * @param new_element The new JSON element to set at the specified key or index.
 * 
 * @return true if the value was successfully set; false otherwise.
 * 
 * @details This function updates a JSON object or array by setting the value at the specified key or index.
 * If the element is an object, the key is used to identify the location to set the value. 
 * If the element is an array, the index is used.
 * 
 * @warning The function assumes that `element`, `key_or_index`, and `new_element` are valid and non-NULL.
 * 
 * @note For arrays, the function replaces the existing element at the specified index.
 *       For objects, the function adds or updates the key-value pair.
 */
bool json_set_element(JsonElement *element, const char *key_or_index, JsonElement *new_element) {
    if (!element || !key_or_index || !new_element) {
        JSON_LOG("[json_set_element] Error: Invalid argument(s). Element, key/index, or new element is NULL.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid argument(s) in json_set_element.\n");
        last_error.code = JSON_ERROR_NONE;
        return false;
    }

    switch (element->type) {
        case JSON_OBJECT: {
            // For JSON_OBJECT, key_or_index is the key as a string
            char* non_const_key = string_strdup(key_or_index);
            if (!non_const_key) {
                JSON_LOG("[json_set_element] Error: Memory allocation failed for key duplication.");
                snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in json_set_element.\n");
                last_error.code = JSON_ERROR_MEMORY;
                return false;
            }

            map_insert(element->value.object_val, non_const_key, new_element);
            JSON_LOG("[json_set_element] Key '%s' inserted into JSON object.", key_or_index);
            return true;
        }
        case JSON_ARRAY: {
            // For JSON_ARRAY, key_or_index is the index as a string
            char *end;
            long index = strtol(key_or_index, &end, 10);
            if (end == key_or_index || *end != '\0' || index < 0 || (size_t)index >= vector_size(element->value.array_val)) {
                JSON_LOG("[json_set_element] Error: Invalid index '%s'.", key_or_index);
                snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid index '%s' in json_set_element.\n", key_or_index);
                last_error.code = JSON_ERROR_INVALID_VALUE;
                return false;
            }

            // Replace the existing element at the specified index
            JsonElement **array_element = vector_at(element->value.array_val, (size_t)index);
            JSON_LOG("[json_set_element] Replacing element at index %ld in JSON array.", index);
            json_deallocate(*array_element); // Deallocate the old element
            *array_element = new_element;
            return true;
        }
        default:
            JSON_LOG("[json_set_element] Error: Element is not an object or an array.");
            snprintf(last_error.message, sizeof(last_error.message), "Error: Element is not an object or an array in json_set_element.\n");
            last_error.code = JSON_ERROR_UNEXPECTED_TOKEN;
            return false;
    }
}

/**
 * @brief Removes an element from a JSON object or array by key or index.
 * 
 * @param element The JSON object or array from which to remove an element. Must be of type JSON_OBJECT or JSON_ARRAY.
 * @param key_or_index The key (for objects) or index (for arrays) identifying the element to remove.
 * 
 * @return true if the element was successfully removed; false if an error occurred.
 * 
 * @details This function removes an element from a JSON object or array based on the provided key or index.
 * If the element is a JSON object, the key is used to locate and remove the associated key-value pair.
 * If the element is a JSON array, the index is used to locate and remove the element at that position.
 * The function returns false if the element is not an object or array, or if the key or index is invalid.
 * 
 * @warning The caller is responsible for ensuring the validity of the key or index.
 * 
 * @note If the element is not an object or array, or if an error occurs, the function returns false and sets an error message.
 */
bool json_remove_element(JsonElement *element, const char *key_or_index) {
    if (!element || !key_or_index) {
        JSON_LOG("[json_remove_element] Error: Invalid argument(s). Element or key/index is NULL.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid argument(s) in json_remove_element.\n");
        last_error.code = JSON_ERROR_NONE;
        return false;
    }

    switch (element->type) {
        // Removing an element from a JSON object
        case JSON_OBJECT: {
            char *non_const_key = string_strdup(key_or_index);
            if (!non_const_key) {
                JSON_LOG("[json_remove_element] Error: Memory allocation failed for key duplication.");
                snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in json_remove_element.\n");
                last_error.code = JSON_ERROR_MEMORY;
                return false;
            }
            bool result = map_erase(element->value.object_val, non_const_key); // Erase the element with the given key
            JSON_LOG("[json_remove_element] Removing key '%s' from JSON object. Result: %s", non_const_key, result ? "Success" : "Failure");
            free(non_const_key);
            
            return result;
        }
        // Removing an element from a JSON array
        case JSON_ARRAY: {
            char *end;
            long index = strtol(key_or_index, &end, 10);
            if (end == key_or_index || *end != '\0' || index < 0 || (size_t)index >= vector_size(element->value.array_val)) {
                JSON_LOG("[json_remove_element] Error: Invalid array index '%s'.", key_or_index);
                snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid index '%s' in json_remove_element.\n", key_or_index);
                last_error.code = JSON_ERROR_INVALID_VALUE;
                return false;
            }

            JsonElement **array_element = vector_at(element->value.array_val, (size_t)index);
            JSON_LOG("[json_remove_element] Removing element at index %ld from JSON array.", index);
            json_deallocate(*array_element);
            vector_erase(element->value.array_val, (size_t)index, 1); // Erase one element from the vector at the specified index

            return true;
        }

        default:
            JSON_LOG("[json_remove_element] Error: Element is neither an object nor an array.");
            snprintf(last_error.message, sizeof(last_error.message), "Error: Element is not an object or an array in json_remove_element.\n");
            last_error.code = JSON_ERROR_UNEXPECTED_TOKEN;
            return false;
    }
}

/**
 * @brief Finds an element within a JSON object or array that matches a given predicate.
 * 
 * @param element The JSON object or array to search. Must be of type JSON_OBJECT or JSON_ARRAY.
 * @param predicate A function pointer to the predicate function used to test elements.
 * @param user_data Additional data passed to the predicate function.
 * 
 * @return A pointer to the first matching JSON element, or NULL if no match is found or an error occurs.
 * 
 * @details This function searches through a JSON object or array and returns the first element that satisfies
 * the predicate function. The search stops as soon as a match is found. The predicate function must
 * return true for a match and false otherwise.
 * 
 * @warning The caller is responsible for ensuring that the predicate function is correctly implemented.
 * 
 * @note If the element is not an object or array, or if an error occurs, the function returns NULL and sets an error message.
 */
JsonElement* json_find(const JsonElement *element, JsonPredicate predicate, void *user_data) {
    if (!element || !predicate) {
        JSON_LOG("[json_find] Error: Invalid argument(s). Element or predicate is NULL.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid argument(s) in json_find.\n");
        last_error.code = JSON_ERROR_NONE;
        return NULL;
    }

    JsonElement* found_element = NULL;

    switch (element->type) {
        case JSON_OBJECT:
            if (json_find_in_object(element, predicate, user_data, &found_element)) {
                JSON_LOG("[json_find] Found matching element in JSON object.");
                return found_element;
            }
            break;
        case JSON_ARRAY:
            if (json_find_in_array(element, predicate, user_data, &found_element)) {
                JSON_LOG("[json_find] Found matching element in JSON array.");
                return found_element;
            }
            break;
        default:
            JSON_LOG("[json_find] Error: Element is not an object or array.");
            snprintf(last_error.message, sizeof(last_error.message), "Error: Element is not an object or an array in json_find.\n");
            last_error.code = JSON_ERROR_UNEXPECTED_TOKEN;
            return NULL;
    }

    JSON_LOG("[json_find] No matching element found.");
    return NULL; // No matching element found
}

/**
 * @brief Retrieves the last error that occurred during JSON operations.
 * 
 * @return A JsonError struct containing the error code and message of the last error.
 * 
 * @details This function provides access to the last error that occurred during any JSON operation.
 * The error code can be used to identify the type of error, and the error message provides
 * additional details about the error.
 * 
 * @note The error state is global and shared across all JSON operations. It is reset with each new error.
 */
JsonError json_last_error() {
    JSON_LOG("[json_last_error] : Function start point");
    return last_error;
}

/**
 * @brief Merges two JSON objects into a new JSON object.
 * 
 * @param element1 The first JSON object to merge. Must be of type JSON_OBJECT.
 * @param element2 The second JSON object to merge. Must be of type JSON_OBJECT.
 * @return A new JSON object containing the merged contents of element1 and element2, or NULL if an error occurs.
 * 
 * @details This function creates a new JSON object and merges the key-value pairs from both input objects into it.
 * If a key exists in both objects, the value from the second object is used. Memory allocation failures
 * during the merge process will cause the function to return NULL, and an appropriate error message will be
 * set in the global `last_error`.
 * 
 * @warning The caller is responsible for deallocating the returned JSON object using `json_deallocate`.
 * @note Both input elements must be JSON objects. If they are not, the function returns NULL and sets an error message.
 */
JsonElement* json_merge(const JsonElement *element1, const JsonElement *element2) {
    if (!element1 || !element2 || element1->type != JSON_OBJECT || element2->type != JSON_OBJECT) {
        JSON_LOG("[json_merge] Error: Both elements must be JSON objects.\n");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Both elements must be JSON objects in json_merge.\n");
        last_error.code = JSON_ERROR_NONE;
        return NULL;
    }

    JsonElement* merged = json_create(JSON_OBJECT);
    if (!merged) {
        JSON_LOG("[json_merge] Error: Memory allocation failed for the merged object.\n");
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
            JSON_LOG("[json_merge] Error: Memory allocation failed for key in the first object.\n");
            snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for key in json_merge.\n");
            last_error.code = JSON_ERROR_MEMORY;
            json_deallocate(merged);
            return NULL;
        }

        JsonElement* value_copy = json_deep_copy((JsonElement*)it.node->value);
        if (!value_copy) {
            JSON_LOG("[json_merge] Error: Memory allocation failed while copying value from the first object.\n");
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
            JSON_LOG("[json_merge] Error: Memory allocation failed for key in the second object.\n");
            snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for key in json_merge (second object).\n");
            last_error.code = JSON_ERROR_MEMORY;
            json_deallocate(merged);
            return NULL;
        }

        JsonElement* value_copy = json_deep_copy((JsonElement*)it.node->value);
        if (!value_copy) {
            JSON_LOG("[json_merge] Error: Memory allocation failed while copying value from the second object.\n");
            free(key);
            json_deallocate(merged);
            return NULL;
        }
        map_insert(merged->value.object_val, key, value_copy);
        map_iterator_increment(&it);
    }

    JSON_LOG("[json_merge] Successfully merged two JSON objects.\n");
    return merged;
}

/**
 * @brief Converts a JSON array of strings into a C-style array of strings.
 * 
 * @param array The JSON array to convert. Must be of type JSON_ARRAY and contain only JSON_STRING elements.
 * @param length A pointer to a size_t variable where the number of strings will be stored.
 * 
 * @return A C-style array of strings, or NULL if an error occurs.
 * 
 * @details This function extracts the strings from a JSON array and returns them in a newly allocated array.
 * If the JSON array contains any non-string elements, the function returns NULL and sets an error message.
 * The `length` parameter will be set to the number of strings in the returned array.
 * 
 * @warning The caller is responsible for deallocating the returned array and each string within it.
 * @note If the input array is NULL, not of type JSON_ARRAY, or contains non-string elements, the function returns NULL.
 */
char** json_to_string_array(const JsonElement *array, size_t *length) {
    if (!array || array->type != JSON_ARRAY || !length) {
        JSON_LOG("[json_to_string_array] Error: Invalid input in json_to_string_array.\n");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid input in json_to_string_array.\n");
        last_error.code = JSON_ERROR_NONE;
        return NULL;
    }

    *length = vector_size(array->value.array_val); 
    if (*length == 0) {
        JSON_LOG("[json_to_string_array] Error: Size of String array element in json is zero in json_to_string_array.\n");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Size of String array element in json is zero in json_to_string_array.\n");
        last_error.code = JSON_ERROR_NONE;
        return NULL;
    }

    char **string_array = malloc(*length * sizeof(char*));
    if (!string_array) {
        JSON_LOG("[json_to_string_array] Error: Memory allocation failed in json_to_string_array.\n");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in json_to_string_array.\n");
        last_error.code = JSON_ERROR_MEMORY;
        *length = 0;
        return NULL;
    }

    for (size_t i = 0; i < *length; ++i) {
        JsonElement *element = *(JsonElement**)vector_at(array->value.array_val, i);
        if (!element || element->type != JSON_STRING) {
            JSON_LOG("[json_to_string_array] Error: Non-string element found in json_to_string_array at index %zu.\n", i);
            snprintf(last_error.message, sizeof(last_error.message), "Error: Non-string element found in json_to_string_array at index %zu.\n", i);
            // If any element is not a string, deallocate and return NULL
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

/**
 * @brief Converts a JSON element to a specified JSON type.
 * 
 * @param element The JSON element to be converted. Must not be NULL.
 * @param type The target JSON type for the conversion (e.g., JSON_STRING, JSON_NUMBER, JSON_BOOL, JSON_ARRAY, JSON_OBJECT).
 * @return A pointer to the newly converted JSON element, or NULL if the conversion fails.
 * 
 * @details This function attempts to convert the provided JSON element to the specified type. If the conversion is not supported, or if memory allocation fails,
 *          the function returns NULL and sets an appropriate error message in the global `last_error`.
 * 
 * Conversion details:
 * - JSON_STRING: Converts a number or boolean to a string representation.
 * - JSON_NUMBER: Converts a string containing a valid number to a double.
 * - JSON_BOOL: Converts a non-zero number to true, and zero to false.
 * - JSON_ARRAY: Converts various types (string, number, boolean, null, object, array) into a single-element array, or splits a string into an array of UTF-8 characters.
 * - JSON_OBJECT: Wraps the element into a JSON object with the key "value".
 * 
 * If the conversion is unsupported or fails, the function returns NULL and sets the `last_error` code.
 * 
 * @warning The caller is responsible for deallocating the returned JSON element using `json_deallocate`.
 */
void* json_convert(const JsonElement *element, JsonType type) {
    if (!element) {
        JSON_LOG("[json_convert] Error: The provided JsonElement is NULL.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: The provided JsonElement is NULL in json_convert.\n");
        last_error.code = JSON_ERROR_INVALID_VALUE;  
        return NULL;
    }

    switch (type) {
        case JSON_STRING:
            if (element->type == JSON_NUMBER) {
                JsonElement *stringElement = json_create(JSON_STRING);
                if (!stringElement) {
                    JSON_LOG("[json_convert] Error: Memory allocation failed for JSON_STRING element.");
                    snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for JSON_STRING element in json_convert.\n");
                    last_error.code = JSON_ERROR_MEMORY;  
                    return NULL;
                }

                char *str = (char*)malloc(64 * sizeof(char));
                if (!str) {
                    JSON_LOG("[json_convert] Error: Memory allocation failed for number to string buffer.");
                    snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for number to string buffer in json_convert.\n");
                    last_error.code = JSON_ERROR_MEMORY;  
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
                    JSON_LOG("[json_convert] Error: Memory allocation failed for JSON_BOOL to JSON_STRING conversion.");
                    snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for JSON_BOOL to JSON_STRING conversion in json_convert.\n");
                    last_error.code = JSON_ERROR_MEMORY;
                    return NULL;
                }
                if (!stringElement->value.string_val) {
                    JSON_LOG("[json_convert] Error: Memory allocation failed for string duplication.");
                    snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for string duplication in json_convert.\n");
                    last_error.code = JSON_ERROR_MEMORY;
                    json_deallocate(stringElement);
                    return NULL;
                }

                stringElement->value.string_val = string_strdup(element->value.bool_val ? "true" : "false");
                return stringElement;
            }
            JSON_LOG("[json_convert] Error: Conversion to string is not supported for the given type.");
            snprintf(last_error.message, sizeof(last_error.message), "Error: Conversion to string is not supported for the given type in json_convert.\n");
            last_error.code = JSON_ERROR_UNEXPECTED_TOKEN;
            return NULL;
        case JSON_NUMBER:
            if (element->type == JSON_STRING) {
                double *num = (double*)malloc(sizeof(double));
                if (!num) {
                    snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for string to number conversion in json_convert.\n");
                    last_error.code = JSON_ERROR_MEMORY;
                    JSON_LOG("[json_convert] %s", last_error.message);  
                    return NULL;
                }
                *num = strtod(element->value.string_val, NULL);
                return num;
            }
            snprintf(last_error.message, sizeof(last_error.message), "Error: Conversion to number is not supported for the given type in json_convert.\n");
            last_error.code = JSON_ERROR_UNEXPECTED_TOKEN;
            JSON_LOG("[json_convert] %s", last_error.message);
            return NULL;
        case JSON_BOOL:
            if (element->type == JSON_NUMBER) {
                bool *bool_val = (bool*)malloc(sizeof(bool));
                if (!bool_val) {
                    snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for number to boolean conversion in json_convert.\n");
                    last_error.code = JSON_ERROR_MEMORY;
                    JSON_LOG("[json_convert] %s", last_error.message);  
                    return NULL;
                }
                *bool_val = (element->value.number_val != 0); // Non-zero numbers are true
                return bool_val;
            }
            snprintf(last_error.message, sizeof(last_error.message), "Error: Conversion to boolean is not supported for the given type in json_convert.\n");
            last_error.code = JSON_ERROR_UNEXPECTED_TOKEN;
            JSON_LOG("[json_convert] %s", last_error.message); 
            return NULL;
        case JSON_ARRAY: {
            JsonElement *array = json_create(JSON_ARRAY);
            if (!array) {
                snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in json_convert for array.\n");
                last_error.code = JSON_ERROR_MEMORY;
                JSON_LOG("[json_convert] %s", last_error.message);  
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
                            JSON_LOG("[json_convert] %s", last_error.message); 
                            return NULL;
                        }

                        char *utf8_char = (char*)malloc((char_len + 1) * sizeof(char));
                        if (!utf8_char) {
                            json_deallocate(array);
                            snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in json_convert for UTF-8 character.\n");
                            last_error.code = JSON_ERROR_MEMORY;
                            JSON_LOG("[json_convert] %s", last_error.message);  
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
                    JsonElement *objElement = json_deep_copy(element); 
                    vector_push_back(array->value.array_val, &objElement);
                    break;
                }

                case JSON_ARRAY: // Handle nested arrays
                default: { // Treat any other types as a nested array
                    JsonElement *arrElement = json_deep_copy(element); 
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
                JSON_LOG("[json_convert] %s", last_error.message);  
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
                    JSON_LOG("[json_convert] %s", last_error.message);  
                    json_deallocate(object);
                    free(key); 
                    return NULL;
            }

            if (!newElement) {
                snprintf(last_error.message, sizeof(last_error.message), "Error: Failed to create new element in json_convert.\n");
                last_error.code = JSON_CREATION_FAILED;
                JSON_LOG("[json_convert] %s", last_error.message);  
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
            last_error.code = JSON_ERROR_UNEXPECTED_TOKEN; 
            JSON_LOG("[json_convert] %s", last_error.message);  
            return NULL;

    }
    return NULL;
}

/**
 * @brief Applies a transformation function to each element of a JSON array and returns a new JSON array with the transformed elements.
 * 
 * @param array The JSON array to map over. Must be of type JSON_ARRAY.
 * @param map_func The function to apply to each element. This function takes a JSON element and user data as arguments and returns a transformed JSON element.
 * @param user_data Optional user data to pass to the mapping function. Can be NULL.
 * 
 * @return A new JSON array containing the transformed elements, or NULL if an error occurs.
 * 
 * @details This function creates a new JSON array where each element is the result of applying the `map_func` to the corresponding element in the input array. 
 * If the transformation function returns NULL for any element, the entire operation fails, and any allocated memory for the result array is freed.
 * The caller is responsible for deallocating the returned JSON array.
 */
JsonElement* json_map(const JsonElement* array, JsonMapFunction map_func, void* user_data) {
    JSON_LOG("[json_map] Starting map operation on array: %p", (void*)array);

    if (!array || array->type != JSON_ARRAY) {
        JSON_LOG("[json_map] Error: Invalid input array. Expected JSON_ARRAY type.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid input in json_map. The input must be a JSON array.\n");
        last_error.code = JSON_ERROR_TYPE;
        return NULL;
    }
    if (!map_func) {
        JSON_LOG("[json_map] Error: map_func is NULL.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: map_func is NULL in json_map.\n");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        return NULL;
    }

    // Create a new JSON array to hold the result
    JsonElement* resultArray = json_create(JSON_ARRAY);
    if (!resultArray) {
        JSON_LOG("[json_map] Error: Memory allocation failed for result array.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for the result array in json_map.\n");
        last_error.code = JSON_CREATION_FAILED;
        return NULL;
    }

    JSON_LOG("[json_map] Mapping elements in array...");
    for (size_t i = 0; i < vector_size(array->value.array_val); ++i) {
        JsonElement* currentElement = *(JsonElement**)vector_at(array->value.array_val, i);
        JSON_LOG("[json_map] Applying map function on element at index %zu.", i);

        // Apply the transformation function to the current element
        JsonElement* transformedElement = map_func(currentElement, user_data);

        // Check if the transformation was successful
        if (!transformedElement) {
            JSON_LOG("[json_map] Error: Transformation failed for element at index %zu.", i);
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
        JSON_LOG("[json_map] Successfully mapped element at index %zu.", i);
    }

    JSON_LOG("[json_map] Map operation completed. Returning result array.");
    last_error.code = JSON_ERROR_NONE;
    return resultArray;
}

/**
 * @brief Filters elements of a JSON array based on a predicate function and returns a new JSON array with elements that satisfy the predicate.
 * 
 * @param array The JSON array to filter. Must be of type JSON_ARRAY.
 * @param predicate The function to evaluate each element. This function takes a JSON element and user data as arguments and returns true if the element should be included in the result.
 * @param user_data Optional user data to pass to the predicate function. Can be NULL.
 * 
 * @return A new JSON array containing the elements that satisfy the predicate, or NULL if an error occurs.
 * 
 * @details This function creates a new JSON array containing only the elements for which the `predicate` function returns true. 
 * If an error occurs during the operation, such as a memory allocation failure, the function returns NULL and any partially constructed result array is deallocated.
 * The caller is responsible for deallocating the returned JSON array.
 */
JsonElement* json_filter(const JsonElement *array, JsonPredicate predicate, void *user_data) {
    JSON_LOG("[json_filter] Starting filter operation on array: %p", (void*)array);

    if (!array || array->type != JSON_ARRAY) {
        JSON_LOG("[json_filter] Error: Invalid array input. Expected JSON_ARRAY type.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid array input in json_filter. The input must be a JSON array.\n");
        last_error.code = JSON_ERROR_TYPE;
        return NULL;
    }
    if (!predicate) {
        JSON_LOG("[json_filter] Error: Predicate function is NULL.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Predicate function is NULL in json_filter.\n");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        return NULL;
    }

    JsonElement* resultArray = json_create(JSON_ARRAY);
    if (!resultArray) {
        JSON_LOG("[json_filter] Error: Memory allocation failed for the result array.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for the result array in json_filter.\n");
        last_error.code = JSON_CREATION_FAILED;
        return NULL;
    }

    JSON_LOG("[json_filter] Filtering elements in array...");
    for (size_t i = 0; i < vector_size(array->value.array_val); ++i) {
        JsonElement* currentElement = *(JsonElement**)vector_at(array->value.array_val, i);
        JSON_LOG("[json_filter] Applying predicate on element at index %zu.", i);

        if (predicate(currentElement, user_data)) {
            // If the predicate returns true, copy and add the element to the result array
            JSON_LOG("[json_filter] Predicate matched. Copying element at index %zu.", i);
            JsonElement* elementCopy = json_deep_copy(currentElement);
            if (!elementCopy) {
                JSON_LOG("[json_filter] Error: Memory allocation failed during deep copy at index %zu.", i);
                snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed during deep copy in json_filter.\n");
                last_error.code = JSON_ERROR_MEMORY;
                
                // Deallocate the already allocated elements in the result array
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

    JSON_LOG("[json_filter] Filter operation completed. Returning result array.");
    last_error.code = JSON_ERROR_NONE;
    return resultArray;
}

/**
 * @brief Applies a reduction function across all elements of a JSON array.
 * 
 * @param array The JSON array to reduce. Must be of type JSON_ARRAY.
 * @param reduce_func The function to apply to each element and the accumulator. This function takes a JSON element, the current accumulator, and user data as arguments.
 * @param initial_value The initial value for the accumulator. Can be NULL if the reduction function can handle NULL as an initial value.
 * @param user_data Optional user data to pass to the reduction function. Can be NULL.
 * 
 * @return The final accumulated value after applying the reduction function, or NULL if an error occurs.
 * 
 * @details This function iterates over each element in the array, applying the reduction function and accumulating the result. 
 * If the input array is not of type JSON_ARRAY or if any argument is invalid, the function returns NULL and sets an error code.
 */
void* json_reduce(const JsonElement *array, JsonReduceFunction reduce_func, void *initial_value, void *user_data) {
    JSON_LOG("[json_reduce] Starting reduction on array: %p", (void*)array);

    if (!array || array->type != JSON_ARRAY) {
        JSON_LOG("[json_reduce] Error: Invalid array input. Expected JSON_ARRAY type.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid array input in json_reduce.");
        last_error.code = JSON_ERROR_TYPE;
        return NULL;
    }

    if (!reduce_func) {
        JSON_LOG("[json_reduce] Error: reduce_func is NULL.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: reduce_func is NULL in json_reduce.");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        return NULL;
    }

    // Initialize the accumulator with the provided initial value
    void* accumulator = initial_value;
    JSON_LOG("[json_reduce] Initial accumulator value: %p", (void*)accumulator);

    // Iterate over the array elements and apply the reduce function
    for (size_t i = 0; i < vector_size(array->value.array_val); ++i) {
        JsonElement* currentElement = *(JsonElement**)vector_at(array->value.array_val, i);
        JSON_LOG("[json_reduce] Applying reduce_func on element at index %zu.", i);

        accumulator = reduce_func(currentElement, accumulator, user_data);
        JSON_LOG("[json_reduce] Updated accumulator value: %p", (void*)accumulator);
    }

    JSON_LOG("[json_reduce] Reduction completed successfully.");
    last_error.code = JSON_ERROR_NONE;
    return accumulator;
}


/**
 * @brief Formats a JSON element into a human-readable string.
 * 
 * @param element The JSON element to format.
 * @return A dynamically allocated string containing the formatted JSON, or NULL if an error occurs.
 * 
 * @details This function serializes the JSON element into a pretty-printed string with indentation for readability.
 * The caller is responsible for freeing the allocated string. If the input element is NULL, the function returns NULL and sets an error code.
 */
char* json_format(const JsonElement *element) {
    JSON_LOG("[json_format] Formatting JSON element: %p", (void*)element);

    if (!element) {
        JSON_LOG("[json_format] Error: The provided JsonElement is NULL.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: The provided JsonElement is NULL in json_format.");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        return NULL;
    }

    // Create a string to store the formatted output
    String* str = string_create("");
    json_format_internal(element, str, 0);

    const char* temp = string_c_str(str);
    char* formatted = string_strdup(temp); // Create a modifiable copy of the formatted string

    if (!formatted) {
        JSON_LOG("[json_format] Error: Memory allocation failed for formatted string.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for formatted string in json_format.");
        last_error.code = JSON_ERROR_MEMORY;
        string_deallocate(str);
        return NULL;
    }

    JSON_LOG("[json_format] Successfully formatted JSON element.");
    last_error.code = JSON_ERROR_NONE;
    
    // Clean up and return the formatted string
    string_deallocate(str); // Deallocate the String object
    return formatted;
}

/**
 * @brief Creates a shallow copy of a JSON element.
 * 
 * @param element The JSON element to clone.
 * 
 * @return A pointer to the newly cloned JSON element, or NULL if an error occurs.
 * 
 * @details This function creates a shallow copy of the given JSON element. For JSON objects and arrays, the copy will reference the same underlying data as the original.
 * For strings, a new string is allocated and copied. The caller is responsible for freeing the cloned element.
 * If the input element is NULL or memory allocation fails, the function returns NULL and sets an error code.
 */
JsonElement* json_clone(const JsonElement *element) {
    JSON_LOG("[json_clone] Cloning JSON element: %p", (void*)element);

    if (!element) {
        JSON_LOG("[json_clone] Error: NULL input provided.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: NULL input to json_clone.");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        return NULL;
    }

    JsonElement *clonedElement = (JsonElement*)malloc(sizeof(JsonElement));
    if (!clonedElement) {
        JSON_LOG("[json_clone] Error: Memory allocation failed.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in json_clone.");
        last_error.code = JSON_ERROR_MEMORY;
        return NULL;
    }

    clonedElement->type = element->type;
    JSON_LOG("[json_clone] Cloning element of type: %d", element->type);

    switch (element->type) {
        case JSON_NULL:
        case JSON_BOOL:
        case JSON_NUMBER:
            clonedElement->value = element->value;
            break;
        case JSON_STRING:
            clonedElement->value.string_val = string_strdup(element->value.string_val);
            if (!clonedElement->value.string_val) {
                JSON_LOG("[json_clone] Error: String duplication failed.");
                free(clonedElement);
                snprintf(last_error.message, sizeof(last_error.message), "Error: String duplication failed in json_clone.");
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
            JSON_LOG("[json_clone] Error: Unknown type encountered.");
            free(clonedElement);
            snprintf(last_error.message, sizeof(last_error.message), "Error: Unknown type in json_clone.");
            last_error.code = JSON_ERROR_TYPE;
            return NULL;
    }

    JSON_LOG("[json_clone] Successfully cloned the element.");
    last_error.code = JSON_ERROR_NONE;
    return clonedElement;
}

/**
 * @brief Retrieves all the keys from a JSON object.
 * 
 * @param object The JSON object from which to retrieve the keys. Must be of type JSON_OBJECT.
 * @param num_keys Pointer to a size_t variable where the number of keys will be stored.
 * 
 * @return A dynamically allocated array of strings containing the keys, or NULL if an error occurs.
 * 
 * @details This function allocates memory for the array of keys and for each key string. 
 * The caller is responsible for freeing the allocated memory. If the input JSON element is not an object 
 * or if memory allocation fails, the function returns NULL and sets an error code.
 */
char** json_get_keys(const JsonElement *object, size_t *num_keys) {
    JSON_LOG("[json_get_keys] Retrieving keys from JSON object: %p", (void*)object);

    if (!object || object->type != JSON_OBJECT || !num_keys) {
        JSON_LOG("[json_get_keys] Error: Invalid input. Object: %p, Type: %d, num_keys: %p", (void*)object, (object ? (int)object->type : (int)-1), (void*)num_keys);
        snprintf(last_error.message, sizeof(last_error.message), "Error: Invalid input in json_get_keys.");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        return NULL;
    }

    *num_keys = map_size(object->value.object_val);
    JSON_LOG("[json_get_keys] Number of keys: %zu", *num_keys);

    char** keys = (char**)malloc(*num_keys * sizeof(char*));
    if (!keys) {
        JSON_LOG("[json_get_keys] Error: Memory allocation failed.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed in json_get_keys.");
        last_error.code = JSON_ERROR_MEMORY;
        return NULL;
    }

    size_t i = 0;
    MapIterator it = map_begin(object->value.object_val);
    MapIterator end = map_end(object->value.object_val);

    while (it.node != end.node) {
        keys[i] = string_strdup((char*)it.node->key);
        if (!keys[i]) {
            JSON_LOG("[json_get_keys] Error: Memory allocation failed for key duplication.");
            snprintf(last_error.message, sizeof(last_error.message), "Error: Memory allocation failed for key duplication in json_get_keys.");
            last_error.code = JSON_ERROR_MEMORY;

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

    JSON_LOG("[json_get_keys] Successfully retrieved keys.");
    last_error.code = JSON_ERROR_NONE;
    return keys;
}

/**
 * @brief Adds an element to a JSON array.
 * 
 * @param element1 The JSON array to which the element will be added. Must be of type JSON_ARRAY.
 * @param element2 The JSON element to add to the array. Must not be NULL.
 * 
 * @return true if the element was successfully added, false otherwise.
 * 
 * @details This function appends the provided element to the end of the JSON array. 
 * If the operation fails (e.g., due to a memory allocation error), the function returns false and sets an error code.
 * The function assumes that `element1` is a valid JSON array and `element2` is a valid JSON element.
 */
bool json_add_to_array(JsonElement* element1, JsonElement* element2) {
    JSON_LOG("[json_add_to_array] Attempting to add element to array: %p", (void*)element1);

    if (!element1) {
        JSON_LOG("[json_add_to_array] Error: element1 is NULL.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: element1 is NULL and invalid.");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        return false;
    }
    if (!element2) {
        JSON_LOG("[json_add_to_array] Error: element2 is NULL.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: element2 is NULL and invalid.");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        return false;
    }

    if (vector_push_back(element1->value.array_val, &element2)) {
        JSON_LOG("[json_add_to_array] Successfully added element to array.");
        last_error.code = JSON_ERROR_NONE;
        snprintf(last_error.message, sizeof(last_error.message), "Successfully added element to array.");
        return true;
    }

    JSON_LOG("[json_add_to_array] Error: Failed to add element to array.");
    snprintf(last_error.message, sizeof(last_error.message), "Error: Failed to add element to array.");
    last_error.code = JSON_ERROR_INSERTION_FAILED;
    return false;
}


/**
 * @brief Adds a key-value pair to a JSON object.
 * 
 * @param object The JSON object to which the key-value pair will be added. Must be of type JSON_OBJECT.
 * @param key The key as a string. Must not be NULL.
 * @param value The value to associate with the key. Must not be NULL.
 * @return true if the key-value pair was successfully added or replaced, false otherwise.
 * 
 * @details If the key already exists in the object, the existing value is deallocated and replaced with the new value. 
 * The key is duplicated to ensure it is managed independently within the JSON object. If any error occurs, it is reported, and false is returned.
 */
bool json_add_to_object(JsonElement* object, const char* key, JsonElement* value) {
    JSON_LOG("[json_add_to_object] Attempting to add key: %s to object: %p", key, (void*)object);

    if (!object || object->type != JSON_OBJECT) {
        JSON_LOG("[json_add_to_object] Error: Target is not a JSON object. Object: %p, Type: %d", (void*)object, (object ? (int)object->type : (int)-1));
        snprintf(last_error.message, sizeof(last_error.message), "Error: Target is not a JSON object.");
        last_error.code = JSON_ERROR_TYPE;
        return false;
    }
    if (!key) {
        JSON_LOG("[json_add_to_object] Error: Key is NULL.");
        snprintf(last_error.message, sizeof(last_error.message), "Error: Key is NULL.");
        last_error.code = JSON_ERROR_INVALID_KEY;
        return false;
    }
    if (!value) {
        JSON_LOG("[json_add_to_object] Error: Value is NULL for key: %s.", key);
        snprintf(last_error.message, sizeof(last_error.message), "Error: Value is NULL.");
        last_error.code = JSON_ERROR_INVALID_VALUE;
        return false;
    }

    // Duplicate the key to ensure it is managed independently
    char* duplicatedKey = string_strdup(key);
    if (!duplicatedKey) {
        JSON_LOG("[json_add_to_object] Error: Failed to duplicate key: %s.", key);
        snprintf(last_error.message, sizeof(last_error.message), "Error: Failed to duplicate key.");
        last_error.code = JSON_ERROR_MEMORY;
        return false;
    }

    // Check if the key already exists in the object
    JSON_LOG("[json_add_to_object] Checking if key: %s already exists in the object.", key);
    JsonElement* existingValue = (JsonElement*)map_at(object->value.object_val, duplicatedKey);
    if (existingValue) {
        // Deallocate the existing value associated with the key
        JSON_LOG("[json_add_to_object] Key: %s already exists, replacing its value.", key);
        json_deallocate(existingValue);
        snprintf(last_error.message, sizeof(last_error.message), "Warning: Key already exists and its value will be replaced.");
        last_error.code = JSON_ERROR_NONE; // It's not technically an error, so no error code change.
    }

    // Replace or insert the key-value pair in the map
    JSON_LOG("[json_add_to_object] Inserting key-value pair into the object.");
    bool insertResult = map_insert(object->value.object_val, duplicatedKey, value);
    if (!insertResult) {
        free(duplicatedKey); 
        if (!existingValue) {
            // Only deallocate the new value if it wasn't replacing an existing value
            json_deallocate(value);
        }
        JSON_LOG("[json_add_to_object] Error: Failed to insert or replace key-value pair for key: %s.", key);
        snprintf(last_error.message, sizeof(last_error.message), "Error: Failed to insert or replace key-value pair.");
        last_error.code = JSON_ERROR_INSERTION_FAILED;
        return false;
    }

    JSON_LOG("[json_add_to_object] Successfully added key: %s to the object.", key);
    last_error.code = JSON_ERROR_NONE;
    snprintf(last_error.message, sizeof(last_error.message), "Successfully added key-value pair.");
    return true;
}

/**
 * @brief Queries a JSON element using a dot-separated path.
 * 
 * @param element The JSON element to query. Must not be NULL.
 * @param query The query string, using dot notation to specify the path to the desired element.
 * 
 * @return The JSON element found at the specified path, or NULL if the element could not be found or if an error occurs.
 * 
 * @details The query string can include array indexing using square brackets, e.g., "array[0].key". 
 * The function traverses the JSON structure according to the query string and returns the corresponding JSON element.
 * If any error occurs during the traversal (e.g., missing keys or indices), the function returns NULL.
 */
JsonElement* json_query(const JsonElement *element, const char *query) {
    JSON_LOG("[json_query] Starting query: %s", query);
    
    if (!element || !query) {
        JSON_LOG("[json_query] Error: Invalid argument(s). Element: %p, Query: %s", (void*)element, query ? query : "NULL");
        return NULL;
    }

    int count;
    char** tokens = split_query(query, &count);
    if (!tokens) {
        JSON_LOG("[json_query] Error: Failed to split query.");
        return NULL;
    }

    JsonElement* currentElement = (JsonElement*)element;
    for (int i = 0; i < count; ++i) {
        char* token = tokens[i];
        JSON_LOG("[json_query] Processing token: %s", token);

        // Handle array access
        char* bracketPos = strchr(token, '[');
        if (bracketPos) {
            *bracketPos = '\0'; // End the string at the bracket to isolate the key
            char* indexStr = bracketPos + 1;
            char* endBracket = strchr(indexStr, ']');
            if (!endBracket) {
                JSON_LOG("[json_query] Error: Invalid query format, missing closing bracket.");
                break; // Break early on syntax error
            }
            *endBracket = '\0';

            currentElement = json_get_element(currentElement, token); // Get the array or object
            if (!currentElement) {
                JSON_LOG("[json_query] Error: Element not found for key: %s", token);
                break;
            }

            JSON_LOG("[json_query] Accessing array element at index: %s", indexStr);
            if (json_type_of_element(currentElement) == JSON_ARRAY) {
                currentElement = json_get_element(currentElement, indexStr); // Get the element at index
                if (!currentElement) {
                    JSON_LOG("[json_query] Error: Element not found at index %s.", indexStr);
                    break;
                }
            } 
            else {
                JSON_LOG("[json_query] Error: Expected an array but found a different type.");
                currentElement = NULL;
                break;
            }
        } 
        else {
            currentElement = json_get_element(currentElement, token);
            if (!currentElement) {
                JSON_LOG("[json_query] Error: Element not found for token: %s", token);
                break;
            }
        }
    }

    for (int i = 0; i < count; ++i) {
        free(tokens[i]); // Free the duplicated tokens
    }

    JSON_LOG("[json_query] Query completed. Returning element: %p", (void*)currentElement);
    return currentElement;
}
