#include "json/json.h"
#include "fmt/fmt.h"

// Function to double the number in a JSON element
JsonElement* double_number(const JsonElement* element, void* user_data) {
    (void)user_data;
    if (element->type == JSON_NUMBER) {
        JsonElement* newElement = json_create(JSON_NUMBER);
        newElement->value.number_val = element->value.number_val * 2;
        return newElement;
    }
    return json_deep_copy(element); // Return a copy of the element if it's not a number
}

int main() {
    const char* jsonString = "[1, 2, 3, 4, 5]";
    JsonElement* jsonElement = json_parse(jsonString);

    if (jsonElement) {
        JsonElement* doubledArray = json_map(jsonElement, double_number, NULL);
        if (doubledArray) {
            fmt_printf("Doubled numbers array:\n");
            json_print(doubledArray);
            json_deallocate(doubledArray);
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