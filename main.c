#include "json/json.h"
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    JsonElement* root = json_create(JSON_OBJECT);

    // Adding a string element
    JsonElement* name = json_create(JSON_STRING);
    name->value.string_val = string_strdup("John Doe");
    json_set_element(root, "name", name);

    // Adding a number element
    JsonElement* age = json_create(JSON_NUMBER);
    age->value.number_val = 30;
    json_set_element(root, "age", age);

    // Adding a boolean element
    JsonElement* isStudent = json_create(JSON_BOOL);
    isStudent->value.bool_val = true;
    json_set_element(root, "isStudent", isStudent);

    // Adding an array element
    JsonElement* hobbies = json_create(JSON_ARRAY);
    JsonElement* hobby1 = json_create(JSON_STRING);
    hobby1->value.string_val = string_strdup("Reading");
    vector_push_back(hobbies->value.array_val, &hobby1);

    JsonElement* hobby2 = json_create(JSON_STRING);
    hobby2->value.string_val = string_strdup("Hiking");
    vector_push_back(hobbies->value.array_val, &hobby2);
    json_set_element(root, "hobbies", hobbies);

    // Clone the root JSON element
    JsonElement* clonedRoot = json_clone(root);

    if (clonedRoot) {
        JsonElement* clonedAge = json_get_element(clonedRoot, "age");
        if (clonedAge && clonedAge->type == JSON_NUMBER) {
            clonedAge->value.number_val = 35;
        }
        // Adding a new hobby to the cloned JSON
        JsonElement* newHobby = json_create(JSON_STRING);
        newHobby->value.string_val = string_strdup("Gaming");
        vector_push_back(hobbies->value.array_val, &newHobby); // This affects both root and clonedRoot due to shallow copy
    }

    fmt_printf("Original JSON:\n");
    json_print(root);

    fmt_printf("\nCloned and modified JSON:\n");
    json_print(clonedRoot);

    json_deallocate(root);
    json_deallocate(clonedRoot);

    return 0;
}