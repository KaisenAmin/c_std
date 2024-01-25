#include "fmt/fmt.h"
#include "json/json.h"

int main() {
    JsonElement* jsonElement = json_read_from_file("./sources/json_example.json");
    
    if (jsonElement) {
        json_print(jsonElement);
        json_deallocate(jsonElement); // Don't forget to deallocate
    } 
    else {
        fmt_printf("Failed to parse JSON file.\n");
    }
    
    return 0;
}