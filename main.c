#include "fmt/fmt.h"
#include "json/json.h"

int main() {
    const char* jsonFilePath = "./sources/json_example.json";
    JsonElement* jsonElement = json_read_from_file(jsonFilePath);
    
    if (jsonElement) {
        fmt_printf("Successfully parsed JSON file.\n");
        json_print(jsonElement); // Print the parsed JSON structure
        json_deallocate(jsonElement); // Deallocate the JSON structure
    } 
    else {
        fmt_printf("Failed to parse JSON file '%s'.\n", jsonFilePath);
    }
    
    return 0;
}
