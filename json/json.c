#include "json.h"
#include <stdlib.h>

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