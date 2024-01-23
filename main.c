#include "fmt/fmt.h"
#include "json/json.h"

int main() {
    JsonElement* jsonElement = json_read_from_file("./sources/json_example.json");
    (void)jsonElement;
    
    return 0;
}