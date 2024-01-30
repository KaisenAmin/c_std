#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonString = "\"Hello, world!\"";  // JSON string
    JsonElement* jsonElement = json_parse(jsonString);

    if (jsonElement && jsonElement->type == JSON_STRING) {
        fmt_printf("Parsed JSON string: %s\n", jsonElement->value.string_val);
    } 
    else {
        fmt_printf("Failed to parse JSON string or JSON is not a string.\n");
    }

    json_deallocate(jsonElement);
    return 0;
}
