/*

 * Run it with `cmake --build build --target run`. It uses modules that need no
 * third-party library (vector, algorithm, string, json, uuid, fmt), so it
 * builds in every configuration. 
 */
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include "json/json.h"
#include "string/std_string.h"
#include "uuid/uuid.h"
#include "vector/vector.h"
#include <stdlib.h>


static int compare_ints(const void* a, const void* b) {
    int x = *(const int*)a;
    int y = *(const int*)b;

    return (x > y) - (x < y);
}


int main(void) {
    Vector* numbers = vector_create(sizeof(int));
    int values[] = {42, 7, 19, 3, 88, 25};
    
    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        vector_push_back(numbers, &values[i]);
    }
    algorithm_sort(vector_data(numbers), vector_size(numbers), sizeof(int), compare_ints);

    long sum = 0;
    fmt_printf("sorted:");
    for (size_t i = 0; i < vector_size(numbers); i++) {
        int v = *(int*)vector_at(numbers, i);
        sum += v;
        fmt_printf(" %d", v);
    }
    fmt_printf("\nsum: %ld\n", sum);
    vector_deallocate(numbers);

    String* greeting = string_create("hello, ");
    string_append(greeting, "c_std");
    char* upper = string_to_upper(greeting);

    fmt_printf("string: %s (%zu chars) -> %s\n", string_c_str(greeting), string_length(greeting), upper);

    free(upper);
    string_deallocate(greeting);

    JsonElement* doc = json_parse("{\"name\": \"c_std\", \"modules\": 50, \"tags\": [\"vector\", \"json\", \"uuid\"]}");
    if (doc) {
        JsonElement* tags = json_get_element(doc, "tags");
        
        fmt_printf("json: name=%s modules=%d tags=%zu\n", json_get_string(doc, "name", "?"),
                   json_get_int(doc, "modules", 0), json_array_size(tags));
        json_deallocate(doc);
    }

    char id[UUID_STR_LEN];
    uuid_to_string(uuid_v5(uuid_namespace_url(), "https://github.com/KaisenAmin/c_std"), id);
    fmt_printf("uuid v5: %s\n", id);

    return 0;
}
