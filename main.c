#include "json/json.h"
#include "fmt/fmt.h"

// Function to invert boolean values
JsonElement* invert_boolean(const JsonElement* element, void* user_data) {
    (void)user_data;
    if (element->type == JSON_BOOL) {
        JsonElement* newElement = json_create(JSON_BOOL);
        newElement->value.bool_val = !element->value.bool_val;
        return newElement;
    }
    return json_deep_copy(element); // Return a copy of the element if it's not a boolean
}

int main() {
    const char* jsonString = "[true, false, true, false]";
    JsonElement* jsonElement = json_parse(jsonString);

    if (jsonElement) {
        JsonElement* invertedArray = json_map(jsonElement, invert_boolean, NULL);
        if (invertedArray) {
            fmt_printf("Inverted boolean array:\n");
            json_print(invertedArray);
            json_deallocate(invertedArray);
        } 
        else {
            fmt_printf("Failed to map the JSON array.\n");
        }
        json_deallocate(jsonElement);
    } 
    else {
        fmt_printf("Failed to parse JSON string.\n");
    }
    return 0;
}