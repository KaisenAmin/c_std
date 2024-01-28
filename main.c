#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    JsonElement* jsonElement = json_read_from_file("./sources/json_example.json");

    JsonElement* versionElement = json_get_element(jsonElement, "version");
    if (versionElement) {
        JsonElement* versionStrElement = (JsonElement*)json_convert(versionElement, JSON_STRING);
        json_set_element(jsonElement, "version", versionStrElement);
    }

    // Convert unicode_sample string to an array and update JSON
    JsonElement* unicodeElement = json_get_element(jsonElement, "unicode_sample");
    if (unicodeElement) {
        JsonElement* unicodeArray = (JsonElement*)json_convert(unicodeElement, JSON_ARRAY);
        json_set_element(jsonElement, "unicode_sample", unicodeArray);
    }

    // Convert supports_unicode boolean to an object and update JSON
    JsonElement* supportsUnicodeElement = json_get_element(jsonElement, "supports_unicode");
    if (supportsUnicodeElement) {
        JsonElement* supportsUnicodeObj = (JsonElement*)json_convert(supportsUnicodeElement, JSON_OBJECT);
        json_set_element(jsonElement, "supports_unicode", supportsUnicodeObj);
    }

    // Convert additional_info object to an array and update JSON
    JsonElement* additionalInfoElement = json_get_element(jsonElement, "additional_info");
    if (additionalInfoElement) {
        JsonElement* additionalInfoArray = (JsonElement*)json_convert(additionalInfoElement, JSON_ARRAY);
        json_set_element(jsonElement, "additional_info", additionalInfoArray);
    }
    
    if (!json_write_to_file(jsonElement, "./sources/converted_json.json")) {
        fmt_printf("Failed to write converted JSON to file.\n");
    } 
    else {
        fmt_printf("Converted JSON successfully written to './sources/converted_json.json'.\n");
    }

    json_deallocate(jsonElement);
    return 0;
}
