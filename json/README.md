# JSON Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License

## Overview

The JSON library in C offers comprehensive tools for handling JSON data within C programs. This library allows for parsing JSON strings, manipulating JSON objects and arrays, and handling various JSON data types. It is designed to be both flexible and efficient, suitable for a range of applications that require JSON processing.

## Key Features

- **Versatile JSON Data Types:** Supports null, boolean, number, string, array, and object types.
- **Dynamic JSON Structures:** Enables creation and manipulation of JSON arrays and objects.
- **Flexible Parsing Options:** Customizable parsing behavior to accommodate different JSON formats.
- **Error Handling:** Robust error reporting mechanism for tracking and debugging issues in JSON operations.

## Function Descriptions

- `json_parse(const char *json_str)`: Parses a JSON string into a `JsonElement`.
- `json_read_from_file(const char *filename)`: Reads JSON data from a file and parses it into a `JsonElement`.
- `json_get_element(const JsonElement *element, const char *key_or_index)`: Retrieves an element from a JSON object or array by key or index.
- `json_query(const JsonElement *element, const char *query)`: Queries JSON data using a specific query language or path expression.
- `json_merge(const JsonElement *element1, const JsonElement *element2)`: Merges two JSON objects or arrays into one.
- `json_deep_copy(const JsonElement *element)`: Creates a deep copy of JSON data.
- `json_clone(const JsonElement *element)`: Create a shallow copy of a JSON element. This is different from a deep copy as it would copy the references for array and object types instead of duplicating all nested elements.
- `json_parse_with_options(const char *json_str, JsonParseOptions options)`: Parses a JSON string with specific parsing options.
- `json_find(const JsonElement *element, JsonPredicate predicate, void *user_data)`: Finds an element in a JSON object or array based on a predicate.
- `json_filter(const JsonElement *array, JsonPredicate predicate, void *user_data)`: Filters elements in a JSON array based on a predicate.
- `json_map(const JsonElement *array, JsonMapFunction map_func, void *user_data)`: Applies a function to each element in a JSON array, creating a new array.
- `json_serialize(const JsonElement *element)`: Converts a `JsonElement` back into a JSON string.
- `json_format(const JsonElement *element)`: Formats JSON data for pretty-printing or custom formatting.
- `json_write_to_file(const JsonElement *element, const char *filename)`: Writes JSON data to a file.
- `json_set_element(JsonElement *element, const char *key_or_index, JsonElement *new_element)`: Sets an element in a JSON object or array.
- `json_remove_element(JsonElement *element, const char *key_or_index)`: Removes an element from a JSON object or array.
- `json_validate(const JsonElement *element, const char *schema_json)`: Validates JSON data against a schema.
- `json_convert(const JsonElement *element, JsonType type)`: Converts between JSON elements and native data types.
- `json_reduce(const JsonElement *array, JsonReduceFunction reduce_func, void *initial_value, void *user_data)`: Applies a reducer function to a JSON array.
- `json_array_size(const JsonElement *array)`: Returns the size of a JSON array.
- `json_object_size(const JsonElement *object)`: Returns the number of key-value pairs in a JSON object.
- `json_type_of_element(const JsonElement *element)`: Returns the type of a given JSON element.
- `json_last_error()`: Retrieves the last error occurred during JSON operations.
- `json_print(const JsonElement* element)`: Prints the JSON element.
- `json_deallocate(JsonElement *element)`: Deallocates a JSON element and its contents.
- `json_compare(const JsonElement *element1, const JsonElement *element2)`: Compares two JSON elements.
- `json_to_string_array` : convert array element in json to string array.
- `json_add_to_array`: add objects or other kind of elements to array element in json.
- `json_add_to_object`: add any kind of elements to json objects.
- `json_query`: get element in json just by one query.
---

## Example 1: How to Read from a JSON File and Parse It `json_read_from_file` and `json_parse`
This example demonstrates how to read a JSON file and parse it using `json_read_from_file` and `json_parse`.

```c
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
```

## Example 2: Accessing Nested Elements in a JSON Structure with `json_get_element` and `json_parse`
This example shows how to access nested elements within a JSON structure. It retrieves the name of the second category from a JSON array.

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonFilePath = "./sources/json_example.json";
    JsonElement* jsonElement = json_read_from_file(jsonFilePath);

    if (jsonElement) {
        fmt_printf("Successfully parsed JSON file.\n");

        // Accessing a nested element in the JSON structure
        JsonElement* categories = json_get_element(jsonElement, "categories");
        if (categories && categories->type == JSON_ARRAY) {
            JsonElement* secondCategory = json_get_element(categories, "1");

            if (secondCategory && secondCategory->type == JSON_OBJECT) {
                JsonElement* categoryName = json_get_element(secondCategory, "name");

                if (categoryName && categoryName->type == JSON_STRING) {
                    fmt_printf("Second category name: %s\n", categoryName->value.string_val);
                }
            }
        }

        json_deallocate(jsonElement); // Deallocate the JSON structure
    } 
    else {
        fmt_printf("Failed to parse JSON file '%s'.\n", jsonFilePath);
    }
    return 0;
}
```

## Example 3: Retrieving a Specific Element in a Nested JSON Object with `json_get_element`

This example demonstrates how to access a nested element inside a JSON object and retrieve a specific field value.

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonFilePath = "./sources/json_example.json";
    JsonElement* jsonElement = json_read_from_file(jsonFilePath);

    if (jsonElement) {
        fmt_printf("Successfully parsed JSON file.\n");

        // Accessing a nested element in the JSON structure
        JsonElement* additionalInfo = json_get_element(jsonElement, "additional_info");
        if (additionalInfo && additionalInfo->type == JSON_OBJECT) {
            JsonElement* budget = json_get_element(additionalInfo, "budget");

            if (budget && budget->type == JSON_NUMBER) {
                fmt_printf("Budget: %.2f\n", budget->value.number_val);
            }
        }

        json_deallocate(jsonElement); // Deallocate the JSON structure
    } 
    else {
        fmt_printf("Failed to parse JSON file '%s'.\n", jsonFilePath);
    }
    return 0;
}
```

## Example 4: Getting the Size of the `categories` Array

In this example, we parse your JSON string and then get the size of the `categories` array.

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonFilePath = "./sources/json_example.json";
    JsonElement* jsonElement = json_read_from_file(jsonFilePath);

    if (jsonElement) {
        JsonElement* categories = json_get_element(jsonElement, "categories");

        if (categories && categories->type == JSON_ARRAY) {
            size_t size = json_array_size(categories);
            fmt_printf("The 'categories' array has %zu elements.\n", size);
        } 
        else {
            fmt_printf("The 'categories' element is not an array.\n");
        }

        json_deallocate(jsonElement);
    } 
    else {
        fmt_printf("Failed to parse the JSON string.\n");
    }
    return 0;
}
```

## Example 5: Getting the Size of the `contributors` Array in `additional_info`

In this second example, we access the `additional_info` object and then find the size of the `contributors` array within it.

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonFilePath = "./sources/json_example.json";
    JsonElement* jsonElement = json_read_from_file(jsonFilePath);

    if (jsonElement) {
        JsonElement* additionalInfo = json_get_element(jsonElement, "additional_info");

        if (additionalInfo && additionalInfo->type == JSON_OBJECT) {
            JsonElement* contributors = json_get_element(additionalInfo, "contributors");

            if (contributors && contributors->type == JSON_ARRAY) {
                size_t size = json_array_size(contributors);
                fmt_printf("The 'contributors' array has %zu elements.\n", size);
            } 
            else {
                fmt_printf("The 'contributors' element is not an array.\n");
            }
        } 
        else {
            fmt_printf("The 'additional_info' element is not an object.\n");
        }
        json_deallocate(jsonElement);
    } 
    else {
        fmt_printf("Failed to parse the JSON string.\n");
    }

    return 0;
}

```

## Example 6 : Getting size of object in json with `json_object_size` 

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonFilePath = "./sources/json_example.json";
    JsonElement* jsonElement = json_read_from_file(jsonFilePath);

    if (jsonElement) {
        JsonElement* additionalInfo = json_get_element(jsonElement, "additional_info");

        if (additionalInfo && additionalInfo->type == JSON_OBJECT) {
            size_t size = json_object_size(additionalInfo);
            fmt_printf("The 'additional_info' object has %zu key-value pairs.\n", size);
        } 
        else {
            fmt_printf("The 'additional_info' element is not an object.\n");
        }
        json_deallocate(jsonElement);
    } 
    else {
        fmt_printf("Failed to parse the JSON string.\n");
    }
    return 0;
}

```

## Example 7 : getting the size of the `ratings` object in movie items 

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonFilePath = "./sources/json_example.json";
    JsonElement* jsonElement = json_read_from_file(jsonFilePath);

    if (jsonElement) {
        JsonElement* categories = json_get_element(jsonElement, "categories");
        if (categories && categories->type == JSON_ARRAY) {
            JsonElement* movies = json_get_element(categories, "1");
            
            if (movies && movies->type == JSON_OBJECT) {
                JsonElement* items = json_get_element(movies, "items");

                if (items && items->type == JSON_ARRAY) {
                    JsonElement* firstMovie = json_get_element(items, "0");

                    if (firstMovie && firstMovie->type == JSON_OBJECT) {
                        JsonElement* ratings = json_get_element(firstMovie, "ratings");

                        if (ratings && ratings->type == JSON_OBJECT) {
                            size_t size = json_object_size(ratings);
                            fmt_printf("The 'ratings' object of the first movie has %zu key-value pairs.\n", size);
                        } 
                        else {
                            fmt_printf("The 'ratings' element is not an object.\n");
                        }
                    }
                }
            }
        }
        json_deallocate(jsonElement);
    } 
    else {
        fmt_printf("Failed to parse the JSON string.\n");
    }

    return 0;
}
```

## Example 8 : Deep copying parsed json object with `json_deep_copy`

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonFilePath = "./sources/json_example.json";
    JsonElement* originalJsonElement = json_read_from_file(jsonFilePath);

    if (originalJsonElement) {
        fmt_printf("Successfully parsed JSON file.\n");

        // Deep copying the JSON element
        JsonElement* deepCopiedElement = json_deep_copy(originalJsonElement);
        if (deepCopiedElement) {
            fmt_printf("Deep copy created successfully.\n");

            JsonElement* categories = json_get_element(deepCopiedElement, "categories");
            if (categories && categories->type == JSON_ARRAY) {
                size_t arraySize = json_array_size(categories);
                fmt_printf("Size of 'categories' array in deep copy: %zu\n", arraySize);
            }

            JsonElement* additionalInfo = json_get_element(deepCopiedElement, "additional_info");
            if (additionalInfo && additionalInfo->type == JSON_OBJECT) {
                size_t objectSize = json_object_size(additionalInfo);
                fmt_printf("Size of 'additional_info' object in deep copy: %zu\n", objectSize);
            }

            json_deallocate(deepCopiedElement);
        } 
        else {
            fmt_printf("Failed to create a deep copy of the JSON element.\n");
        }
        json_deallocate(originalJsonElement);
    } 
    else {
        fmt_printf("Failed to parse JSON file '%s'.\n", jsonFilePath);
    }
    return 0;
}
```

## Example 9 : Using `json_type_of_element`` to Determine the Type of a JSON Element 

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    // Example JSON string
    const char* jsonString = "{\"name\": \"John Doe\", \"age\": 30, \"is_student\": false}";
    JsonElement* jsonElement = json_parse(jsonString);

    if (jsonElement) {
        fmt_printf("Successfully parsed JSON string.\n");

        JsonElement* nameElement = json_get_element(jsonElement, "name");
        if (nameElement) {
            JsonType nameType = json_type_of_element(nameElement);
            fmt_printf("Type of 'name' element: %s\n", nameType == JSON_STRING ? "String" : "Other");

            // Retrieve and check the type of the 'age' element
            JsonElement* ageElement = json_get_element(jsonElement, "age");
            if (ageElement) {
                JsonType ageType = json_type_of_element(ageElement);
                fmt_printf("Type of 'age' element: %s\n", ageType == JSON_NUMBER ? "Number" : "Other");
            }

            // Retrieve and check the type of the 'is_student' element
            JsonElement* studentElement = json_get_element(jsonElement, "is_student");
            if (studentElement) {
                JsonType studentType = json_type_of_element(studentElement);
                fmt_printf("Type of 'is_student' element: %s\n", studentType == JSON_BOOL ? "Boolean" : "Other");
            }
        } 
        else {
            fmt_printf("Element 'name' not found in JSON.\n");
        }
        json_deallocate(jsonElement);
    } 
    else {
        fmt_printf("Failed to parse JSON string.\n");
    }
    return 0;
}
```

## Example 10 : how to serialize and write json into the file with `json_write_file`

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonFilePath = "./sources/output_json_example.json";
    const char* jsonString = "{\"name\":\"John Doe\",\"age\":30}";

    JsonElement* jsonElement = json_parse(jsonString);
    if (!jsonElement) {
        fmt_printf("Failed to parse JSON string.\n");
        return 1;
    }

    if (json_write_to_file(jsonElement, jsonFilePath)) {
        fmt_printf("Successfully wrote JSON to file '%s'.\n", jsonFilePath);
    } 
    else {
        fmt_printf("Failed to write JSON to file '%s'.\n", jsonFilePath);
    }

    // Deallocate the JSON element
    json_deallocate(jsonElement);
    return 0;
}
```

## Example 11 : Parsing and serializing json with `json_serialize`

```c
#include "json/json.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    const char* jsonString = 
        "{"
        "  \"name\": \"John Doe\","
        "  \"age\": 30,"
        "  \"is_student\": false,"
        "  \"scores\": [95, 85, 75],"
        "  \"address\": {"
        "    \"street\": \"123 Main St\","
        "    \"city\": \"Anytown\""
        "  }"
        "}";

    JsonElement* jsonElement = json_parse(jsonString);
    if (!jsonElement) {
        fmt_printf("Failed to parse JSON string.\n");
        return 1;
    }

    // Serialize the JSON element back into a string
    char* serializedJson = json_serialize(jsonElement);
    if (!serializedJson) {
        fmt_printf("Failed to serialize JSON element.\n");
        json_deallocate(jsonElement);
        return 1;
    }

    fmt_printf("Serialized JSON:\n%s\n", serializedJson);

    free(serializedJson); 
    json_deallocate(jsonElement); 
    return 0;
}
```

## Example 12 : how to compare json with `json_compare`

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonString1 = "{\"name\": \"John Doe\", \"age\": 30}";
    const char* jsonString2 = "{\"name\": \"John Doe\", \"age\": 30}";
    const char* jsonString3 = "{\"name\": \"Jane Doe\", \"age\": 25}";

    JsonElement* jsonElement1 = json_parse(jsonString1);
    JsonElement* jsonElement2 = json_parse(jsonString2);
    JsonElement* jsonElement3 = json_parse(jsonString3);

    if (!jsonElement1 || !jsonElement2 || !jsonElement3) {
        fmt_printf("Failed to parse one or more JSON strings.\n");

        json_deallocate(jsonElement1);
        json_deallocate(jsonElement2);
        json_deallocate(jsonElement3);
        return -1;
    }

    // Compare the first two JSON elements
    bool areEqual1and2 = json_compare(jsonElement1, jsonElement2);
    fmt_printf("JSON 1 and JSON 2 are %s.\n", areEqual1and2 ? "equal" : "not equal");

    // Compare the first and third JSON elements
    bool areEqual1and3 = json_compare(jsonElement1, jsonElement3);
    fmt_printf("JSON 1 and JSON 3 are %s.\n", areEqual1and3 ? "equal" : "not equal");

    json_deallocate(jsonElement1);
    json_deallocate(jsonElement2);
    json_deallocate(jsonElement3);

    return 0;
}
```

## Example 13 : how to set element in objects with `json_set_element`

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonFilePath = "./sources/json_example.json";
    const char* outputFilePath = "./sources/json_modified.json";
    JsonElement* jsonElement = json_read_from_file(jsonFilePath);

    // Create a new element for the updated budget
    JsonElement* newBudgetElement = json_create(JSON_NUMBER);
    newBudgetElement->value.number_val = 600000.0;  // New budget value

    // Set the new element in the "additional_info" object
    JsonElement* additionalInfo = json_get_element(jsonElement, "additional_info");
    if (additionalInfo && additionalInfo->type == JSON_OBJECT) {
        if (!json_set_element(additionalInfo, "budget", newBudgetElement)) {
            fmt_printf("Failed to set new element in JSON.\n");
            json_deallocate(jsonElement);
            return 1;
        }
    } 
    else {
        fmt_printf("Failed to find 'additional_info' object in JSON.\n");
        json_deallocate(jsonElement);
        return 1;
    }

    // Write the modified JSON data to a new file
    if (json_write_to_file(jsonElement, outputFilePath)) {
        fmt_printf("Successfully wrote modified JSON to file '%s'.\n", outputFilePath);
    } 
    else {
        fmt_printf("Failed to write JSON to file '%s'.\n", outputFilePath);
    }

    json_deallocate(jsonElement);
    return 0;
}
```

## Example 14 : how to set or modify an element within an array in json with `json_set_element`

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonFilePath = "./sources/json_example.json";
    const char* outputFilePath = "./sources/json_modified.json";
    JsonElement* jsonElement = json_read_from_file(jsonFilePath);

    JsonElement* categories = json_get_element(jsonElement, "categories");
    if (categories && categories->type == JSON_ARRAY) {
        // Get the first category (Books)
        JsonElement* booksCategory = json_get_element(categories, "0");
        if (booksCategory && booksCategory->type == JSON_OBJECT) {
            // Navigate to the "items" array in the Books category
            JsonElement* items = json_get_element(booksCategory, "items");
            if (items && items->type == JSON_ARRAY) {
                // Create a new element for the updated availability
                JsonElement* newAvailability = json_create(JSON_BOOL);
                newAvailability->value.bool_val = false; // Updated availability value

                JsonElement* firstBook = json_get_element(items, "0"); // Get the first book
                if (firstBook && firstBook->type == JSON_OBJECT) {
                    // Set the new element in the first book object
                    if (!json_set_element(firstBook, "available", newAvailability)) {
                        fmt_printf("Failed to set new element in JSON.\n");
                        json_deallocate(jsonElement);
                        return 1;
                    }
                } 
                else {
                    fmt_printf("Failed to find first book in JSON.\n");
                    json_deallocate(jsonElement);
                    return 1;
                }
            } 
            else {
                fmt_printf("Failed to find 'items' array in JSON.\n");
                json_deallocate(jsonElement);
                return 1;
            }
        } 
        else {
            fmt_printf("Failed to find 'Books' category in JSON.\n");
            json_deallocate(jsonElement);
            return 1;
        }
    } 
    else {
        fmt_printf("Failed to find 'categories' array in JSON.\n");
        json_deallocate(jsonElement);
        return 1;
    }

    // Write the modified JSON data to a new file
    if (json_write_to_file(jsonElement, outputFilePath)) {
        fmt_printf("Successfully wrote modified JSON to file '%s'.\n", outputFilePath);
    } 
    else {
        fmt_printf("Failed to write JSON to file '%s'.\n", outputFilePath);
    }

    json_deallocate(jsonElement);
    return 0;
}
```

## Example 15 : bench mark read and parse json file with python 

`c_std JSON Library Time: 0.00192643 seconds`

```c
#include "json/json.h"
#include "fmt/fmt.h"
#include <time.h>

int main() {
    struct timespec start, end;
    long double time_elapsed;
    const char* jsonFilePath = "./sources/users_1k.json";

    clock_gettime(CLOCK_MONOTONIC, &start);

    JsonElement* jsonElement = json_read_from_file(jsonFilePath);

    clock_gettime(CLOCK_MONOTONIC, &end);

    time_elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e10;
    fmt_printf("c_std JSON Library Time: %.8Lf seconds\n", time_elapsed);

    json_deallocate(jsonElement);
    return 0;
}
```

*Python Code*

`Python JSON Library Time: 0.00509940 seconds`

```python
import json
import time

json_file_path = r"C:\Users\Science\Desktop\projects\C\c_std\sources\users_1k.json"

start = time.time()

with open(json_file_path, 'r') as file:
    data = json.load(file)

end = time.time()
print(f"Python JSON Library Time: {end - start:.8f} seconds")
```

## Example 16 : Removing an element from json object with `json_remove_element`

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonString = "{\"name\": \"John Doe\", \"age\": 30, \"email\": \"johndoe@example.com\"}";
    JsonElement* jsonElement = json_parse(jsonString);

    if (jsonElement) {
        // Remove the 'age' element from the object
        if (json_remove_element(jsonElement, "age")) {
            fmt_printf("Age removed successfully.\n");
        } 
        else {
            fmt_printf("Failed to remove 'age' from JSON object.\n");
        }

        json_print(jsonElement);
        json_deallocate(jsonElement);
    } 
    else {
        fmt_printf("Failed to parse JSON string.\n");
    }
    return 0;
}
```

## Example 17 : Removing an element from json Array with `json_remove_element`

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonString = "[1, 2, 3, 4, 5]";
    JsonElement* jsonElement = json_parse(jsonString);

    if (jsonElement) {
        // Remove the element at index 1 (the second element)
        if (json_remove_element(jsonElement, "1")) {
            fmt_printf("Element at index 1 removed successfully.\n");
        } 
        else {
            fmt_printf("Failed to remove element at index 1 from JSON array.\n");
        }

        json_print(jsonElement);
        json_deallocate(jsonElement);

    } 
    else {
        fmt_printf("Failed to parse JSON string.\n");
    }
    return 0;
}
```

## Example 18: Finding an Element in a JSON Array with `json_find`

```c
#include "json/json.h"
#include "fmt/fmt.h"

// Predicate function to find the first number greater than a threshold
bool is_number_greater(const JsonElement* element, void* user_data) {
    double threshold = *(double*)user_data;
    return element->type == JSON_NUMBER && element->value.number_val > threshold;
}

int main() {
    const char* jsonString = "[1, 20, 3, 40, 5]";
    JsonElement* jsonElement = json_parse(jsonString);

    if (jsonElement) {
        double threshold = 10.0;
        JsonElement* foundElement = json_find(jsonElement, is_number_greater, &threshold);

        if (foundElement) {
            fmt_printf("Found element: %g\n", foundElement->value.number_val);
        } 
        else {
            fmt_printf("No element found greater than %g.\n", threshold);
        }

        json_deallocate(jsonElement);
    } 
    else {
        fmt_printf("Failed to parse JSON string.\n");
    }
    return 0;
}
```

## Example 19: Finding an Element in a JSON Object with `json_find`

```c
#include "json/json.h"
#include "fmt/fmt.h"
#include <string.h>

// Predicate function to find a string element matching a keyword
bool is_string_match(const JsonElement* element, void* user_data) {
    const char* keyword = (const char*)user_data;
    return element->type == JSON_STRING && strcmp(element->value.string_val, keyword) == 0;
}

int main() {
    const char* jsonString = "{\"name\": \"John\", \"age\": 30, \"city\": \"New York\"}";
    JsonElement* jsonElement = json_parse(jsonString);

    if (jsonElement) {
        const char* keyword = "New York";
        JsonElement* foundElement = json_find(jsonElement, is_string_match, (void*)keyword);

        if (foundElement) {
            fmt_printf("Found matching string: %s\n", foundElement->value.string_val);
        } 
        else {
            fmt_printf("No string element found matching '%s'.\n", keyword);
        }

        json_deallocate(jsonElement);
    } 
    else {
        fmt_printf("Failed to parse JSON string.\n");
    }
    return 0;
}
```

## Example 20: Finding a Boolean Element in a JSON Object with `json_find`


```c
#include "json/json.h"
#include "fmt/fmt.h"

// Predicate function to find a boolean element with a specific value
bool is_boolean_true(const JsonElement* element, void* user_data) {
    (void)user_data; // Unused parameter
    return element->type == JSON_BOOL && element->value.bool_val == true;
}

int main() {
    const char* jsonString = "{\"name\": \"John\", \"age\": 30, \"is_student\": true, \"employed\": false}";
    JsonElement* jsonElement = json_parse(jsonString);

    if (jsonElement) {
        JsonElement* foundElement = json_find(jsonElement, is_boolean_true, NULL);

        if (foundElement) {
            fmt_printf("Found boolean element with value 'true'.\n");
        } 
        else {
            fmt_printf("No boolean element with value 'true' found.\n");
        }

        json_deallocate(jsonElement);
    } 
    else {
        fmt_printf("Failed to parse JSON string.\n");
    }
    return 0;
}
```

## Example 21: Manipulating and Serializing a JSON Object

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonString = "{\"name\": \"Alice\", \"age\": 25}";
    JsonElement* jsonElement = json_parse(jsonString);

    if (jsonElement) {
        // Adding a new element
        JsonElement* newElement = json_create(JSON_STRING);
        newElement->value.string_val = string_strdup("Engineer");
        json_set_element(jsonElement, "profession", newElement);

        // Modifying an existing element
        JsonElement* ageElement = json_get_element(jsonElement, "age");
        if (ageElement && ageElement->type == JSON_NUMBER) {
            ageElement->value.number_val = 26; // Updating Alice's age
        }

        // Removing an element
        json_remove_element(jsonElement, "name");

        // Serializing the modified JSON object
        char* serializedJson = json_serialize(jsonElement);
        if (serializedJson) {
            fmt_printf("Serialized JSON:\n%s\n", serializedJson);
            free(serializedJson);
        } 
        else {
            fmt_printf("Failed to serialize JSON element.\n");
        }

        json_deallocate(jsonElement);
    } 
    else {
        fmt_printf("Failed to parse JSON string.\n");
    }
    return 0;
}
```

## Example 22 : how to get last_error of json failed parse with `json_last_error`

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    // Example of an invalid JSON string
    const char* invalidJsonString = "{invalid json}";
    JsonElement* jsonElement = json_parse(invalidJsonString);

    if (!jsonElement) {
        JsonError error = json_last_error();
        fmt_printf("Failed to parse JSON: Error Code %d, Message: %s\n", error.code, error.message);
    } 
    else {
        // Parsing succeeded (unexpected in this case)
        fmt_printf("JSON parsed successfully, which is unexpected.\n");
        json_deallocate(jsonElement);
    }
    return 0;
}
```

## Example 23 : how to merge 2 json file and the write into file `json_merge`

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    JsonElement* json1 = json_read_from_file("./sources/json_modified.json");
    if (!json1) {
        fmt_printf("Failed to read or parse json1.json\n");
        return 1;
    }

    // Read the second JSON object from file
    JsonElement* json2 = json_read_from_file("./sources/test_json.json");
    if (!json2) {
        fmt_printf("Failed to read or parse json2.json\n");
        json_deallocate(json1);
        return 1;
    }

    // Merge json1 and json2
    JsonElement* mergedJson = json_merge(json1, json2);
    if (!mergedJson) {
        fmt_printf("Failed to merge JSON objects\n");
        json_deallocate(json1);
        json_deallocate(json2);
        return 1;
    }

    fmt_printf("Merged JSON:\n");
    json_print(mergedJson);

    if (json_write_to_file(mergedJson, "./sources/merged_json.json")) {
        fmt_printf("Successfully write merged json into file\n");
    }
    else {
        fmt_printf("Can not write merged json int file\n");
    }

    json_deallocate(json1);
    json_deallocate(json2);
    json_deallocate(mergedJson);

    return 0;
}
```

## Example 24 : how to convert array element in json to string_array with `json_to_string_array`

```c
#include "json/json.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    const char* jsonFilePath = "./sources/json_example.json";
    JsonElement* jsonElement = json_read_from_file(jsonFilePath);

    if (jsonElement) {
        JsonElement* additionalInfo = json_get_element(jsonElement, "additional_info");

        if (additionalInfo && additionalInfo->type == JSON_OBJECT) {
            JsonElement* contributorsArray = json_get_element(additionalInfo, "contributors");

            if (contributorsArray && contributorsArray->type == JSON_ARRAY) {
                size_t length = 0;
                char** contributors = json_to_string_array(contributorsArray, &length);

                if (contributors) {
                    fmt_printf("Contributors:\n");
                    for (size_t i = 0; i < length; ++i) {
                        fmt_printf("  [%zu] %s\n", i, contributors[i]);
                        free(contributors[i]); // Free each individual string
                    }
                    free(contributors); // Free the array itself
                } 
                else {
                    fmt_printf("Failed to convert JSON array to string array.\n");
                }
            }
            else {
                fmt_printf("Contributors is not an Array\n");
            }
        } 
        else {
            fmt_printf("The 'contributors' element is not an array or not found.\n");
        }
        json_deallocate(jsonElement);
    } 
    else {
        fmt_printf("Failed to parse JSON file '%s'.\n", jsonFilePath);
    }

    return 0;
}
```

## Example 25 : how to convert type of json element to other types with `json_convert`

```c
#include "json/json.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    JsonElement* jsonElement = json_read_from_file("./sources/json_example.json");

    // Example: Convert the 'version' element to a string
    JsonElement* versionElement = json_get_element(jsonElement, "version");
    if (versionElement) {
        char* versionStr = (char*)json_convert(versionElement, JSON_STRING);
        if (versionStr) {
            fmt_printf("Version as string: %s\n", versionStr);
            free(versionStr);
        } 
        else {
            fmt_printf("Failed to convert 'version' to string.\n");
        }
    }

    // Example: Convert the 'supports_unicode' element to a string
    JsonElement* unicodeElement = json_get_element(jsonElement, "supports_unicode");
    if (unicodeElement) {
        char* unicodeStr = (char*)json_convert(unicodeElement, JSON_STRING);
        if (unicodeStr) {
            fmt_printf("Supports Unicode as string: %s\n", unicodeStr);
            free(unicodeStr);
        } 
        else {
            fmt_printf("Failed to convert 'supports_unicode' to string.\n");
        }
    }

    // Example: Convert the 'budget' in 'additional_info' to a string
    JsonElement* additionalInfo = json_get_element(jsonElement, "additional_info");
    if (additionalInfo && additionalInfo->type == JSON_OBJECT) {
        JsonElement* budgetElement = json_get_element(additionalInfo, "budget");
        if (budgetElement) {
            char* budgetStr = (char*)json_convert(budgetElement, JSON_STRING);
            if (budgetStr) {
                fmt_printf("Budget as string: %s\n", budgetStr);
                free(budgetStr);
            } 
            else {
                fmt_printf("Failed to convert 'budget' to string.\n");
            }
        }
    }

    json_deallocate(jsonElement);
    return 0;
}
```

## Example 26 : read json file and convert data then write to file with `json_convert` and `json_write_to_file`

```c
#include "json/json.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    JsonElement* jsonElement = json_read_from_file("./sources/json_example.json");

    // Convert the 'version' element to a string and update the JSON structure
    JsonElement* versionElement = json_get_element(jsonElement, "version");
    if (versionElement) {
        char* versionStr = (char*)json_convert(versionElement, JSON_STRING);
        if (versionStr) {
            JsonElement* newVersionElement = json_create(JSON_STRING);
            newVersionElement->value.string_val = versionStr;
            json_set_element(jsonElement, "version", newVersionElement);
        }
    }

    // Convert the 'budget' in 'additional_info' to a string and update the JSON structure
    JsonElement* additionalInfo = json_get_element(jsonElement, "additional_info");
    if (additionalInfo && additionalInfo->type == JSON_OBJECT) {
        JsonElement* budgetElement = json_get_element(additionalInfo, "budget");
        if (budgetElement) {
            char* budgetStr = (char*)json_convert(budgetElement, JSON_STRING);
            if (budgetStr) {
                JsonElement* newBudgetElement = json_create(JSON_STRING);
                newBudgetElement->value.string_val = budgetStr;
                json_set_element(additionalInfo, "budget", newBudgetElement);
            }
        }
    }

    if (!json_write_to_file(jsonElement, "./sources/modified_json.json")) {
        fmt_printf("Failed to write modified JSON to file.\n");
    }
    else {
        fmt_printf("Modified JSON successfully written to './sources/modified_json.json'.\n");
    }

    json_deallocate(jsonElement);
    return 0;
}

```

## Example 27 : convert any kind of data types to an other data with `json_convert`

```c
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
```

## Example 28: Doubling Numbers in a JSON Array

```c
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
```

## Example 29: Converting Numbers to Strings in a JSON Array

```c
#include "json/json.h"
#include "fmt/fmt.h"
#include <stdlib.h>

// Function to convert a number to a string
JsonElement* number_to_string(const JsonElement* element, void* user_data) {
    (void)user_data;
    if (element->type == JSON_NUMBER) {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%g", element->value.number_val);
        JsonElement* stringElement = json_create(JSON_STRING);
        stringElement->value.string_val = string_strdup(buffer);
        return stringElement;
    }
    return json_deep_copy(element); // Return a copy of the element if it's not a number
}

int main() {
    const char* jsonString = "[1, 2, 3.5, 4, 5]";
    JsonElement* jsonElement = json_parse(jsonString);

    if (jsonElement) {
        JsonElement* stringArray = json_map(jsonElement, number_to_string, NULL);
        if (stringArray) {
            fmt_printf("Numbers converted to strings:\n");
            json_print(stringArray);
            json_deallocate(stringArray);
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
```

## Example 30: Changing Boolean Values in a JSON Array

```c
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
```

## Example 31 : how to filter element in json file and then write to file `json_filter`

```c
#include "json/json.h"
#include "fmt/fmt.h"

// Predicate function to check if a book is available
bool is_book_available(const JsonElement* element, void* user_data) {
    (void)user_data; // Explicitly mark user_data as unused
    if (element->type == JSON_OBJECT) {
        JsonElement* available = json_get_element(element, "available");
        return available && available->type == JSON_BOOL && available->value.bool_val;
    }
    return false; // Not a JSON object or not an available book
}

int main() {
    JsonElement* jsonElement = json_read_from_file("./sources/json_example.json");

    if (jsonElement) {
        JsonElement* categories = json_get_element(jsonElement, "categories");
        if (categories && categories->type == JSON_ARRAY) {
            JsonElement* booksCategory = json_get_element(categories, "0");
            if (booksCategory && booksCategory->type == JSON_OBJECT) {
                JsonElement* items = json_get_element(booksCategory, "items");

                if (items && items->type == JSON_ARRAY) {
                    JsonElement* availableBooks = json_filter(items, is_book_available, NULL);
                    if (availableBooks) {
                        fmt_printf("Available books:\n");
                        json_print(availableBooks);

                        json_set_element(booksCategory, "items", availableBooks);

                        // Write the updated JSON element to a file
                        if (json_write_to_file(jsonElement, "./sources/filtered_books.json")) {
                            fmt_printf("Filtered JSON successfully written to './sources/filtered_books.json'.\n");
                        } 
                        else {
                            fmt_printf("Failed to write filtered JSON to file.\n");
                        }
                    } 
                    else {
                        fmt_printf("Failed to filter available books.\n");
                    }
                } 
                else {
                    fmt_printf("Items in 'Books' category are not an array or not found.\n");
                }
            } 
            else {
                fmt_printf("Books category not found.\n");
            }
        } 
        else {
            fmt_printf("'categories' element is not an array or not found.\n");
        }
        json_deallocate(jsonElement);
    } 
    else {
        fmt_printf("Failed to parse JSON file './sources/json_example.json'.\n");
    }
    return 0;
}
```

## Example 32 : how to sum up the numbers with arrays element in json using `json_reduce`

```c
#include "json/json.h"
#include "fmt/fmt.h"

// Reduction function to sum numbers
void* sum_numbers(const JsonElement* element, void* accumulator, void* user_data) {
    (void)user_data;
    if (element->type == JSON_NUMBER) {
        double* sum = (double*)accumulator;
        *sum += element->value.number_val;
    }
    return accumulator;
}

int main() {
    // Assume jsonElement is initialized by parsing the JSON data
    JsonElement* jsonElement = json_read_from_file("./sources/json_example.json");

    if (jsonElement) {
        // Access the 'additional_info' object
        JsonElement* additionalInfo = json_get_element(jsonElement, "additional_info");
        if (additionalInfo && additionalInfo->type == JSON_OBJECT) {
            // Access the 'numbers' array within 'additional_info'
            JsonElement* numbersArray = json_get_element(additionalInfo, "numbers");
            if (numbersArray && numbersArray->type == JSON_ARRAY) {
                double initial_value = 0.0;
                double* sum = (double*)json_reduce(numbersArray, sum_numbers, &initial_value, NULL);
                if (sum) {
                    fmt_printf("Sum of numbers in the array: %f\n", *sum);
                } 
                else {
                    fmt_printf("Failed to calculate sum.\n");
                }
            } 
            else {
                fmt_printf("'numbers' array not found or not an array.\n");
            }
        } 
        else {
            fmt_printf("'additional_info' object not found or not an object.\n");
        }

        json_deallocate(jsonElement);
    } 
    else {
        fmt_printf("Failed to parse JSON file.\n");
    }

    return 0;
}
```

## Example 33 : concatenates names with `json_reduce`

```c
#include "json/json.h"
#include "fmt/fmt.h"
#include "string/string.h"
#include <string.h>
#include <stdlib.h>


// Reduction function to concatenate strings
void* concatenate_strings(const JsonElement* element, void* accumulator, void* user_data) {
    (void)user_data;
    if (element->type == JSON_STRING) {
        char** combined = (char**)accumulator;
        if (*combined == NULL) {
            // Allocate memory for the first time
            *combined = string_strdup(element->value.string_val);
        } 
        else {
            // Calculate new size and reallocate memory
            size_t new_size = strlen(*combined) + strlen(element->value.string_val) + 2; // +2 for comma and null terminator
            char* new_str = (char*)realloc(*combined, new_size);

            if (new_str) {
                strcat(new_str, ",");
                strcat(new_str, element->value.string_val);
                *combined = new_str;
            }
        }
    }
    return accumulator;
}

int main() {
    JsonElement* jsonElement = json_read_from_file("./sources/json_example.json");

    JsonElement *addinfo = json_get_element(jsonElement, "additional_info");
    if (addinfo && addinfo->type == JSON_OBJECT) {
        JsonElement* contributorsArray = json_get_element(addinfo, "contributors");
        if (contributorsArray && contributorsArray->type == JSON_ARRAY) {
            char* concatenatedNames = NULL;
            json_reduce(contributorsArray, concatenate_strings, &concatenatedNames, NULL);

            if (concatenatedNames) {
                fmt_printf("Concatenated names: %s\n", concatenatedNames);
                free(concatenatedNames);
            } 
            else {
                fmt_printf("Failed to concatenate names.\n");
            }
        } 
        else {
            fmt_printf("Contributors array not found or not an array.\n");
        }
        json_deallocate(jsonElement);
    } 
    else {
        fmt_printf("Failed to parse JSON file.\n");
    }

    return 0;
}
```

## Example 34 : how to format data with `json_format`

```c
#include "json/json.h"
#include "fmt/fmt.h"
#include "string/string.h"
#include <stdlib.h>

int main() {
    JsonElement* jsonObject = json_create(JSON_OBJECT);
    JsonElement* nameElement = json_create(JSON_STRING);

    nameElement->value.string_val = string_strdup("John Doe");
    json_set_element(jsonObject, "name", nameElement);

    JsonElement* ageElement = json_create(JSON_NUMBER);
    ageElement->value.number_val = 30;
    json_set_element(jsonObject, "age", ageElement);

    JsonElement* isStudentElement = json_create(JSON_BOOL);
    isStudentElement->value.bool_val = true;
    json_set_element(jsonObject, "isStudent", isStudentElement);

    char* formattedJson = json_format(jsonObject);
    if (formattedJson) {
        fmt_printf("Formatted JSON:\n%s\n", formattedJson);
        free(formattedJson);
    } 
    else {
        fmt_printf("Failed to format JSON.\n");
    }

    json_deallocate(jsonObject);
    return 0;
}
```

## Example 35 : how to create a nested JSON structure with various data types, including arrays and objects.format this with `json_format`

```c
#include "json/json.h"
#include "fmt/fmt.h"
#include "string/string.h"
#include <stdlib.h>

int main() {
    JsonElement* jsonObject = json_create(JSON_OBJECT);
    JsonElement* nameElement = json_create(JSON_STRING);

    nameElement->value.string_val = string_strdup("John Doe");
    json_set_element(jsonObject, "name", nameElement);

    // Add a number element
    JsonElement* ageElement = json_create(JSON_NUMBER);
    ageElement->value.number_val = 30;
    json_set_element(jsonObject, "age", ageElement);

    // Add a boolean element
    JsonElement* isStudentElement = json_create(JSON_BOOL);
    isStudentElement->value.bool_val = true;
    json_set_element(jsonObject, "isStudent", isStudentElement);

    // Add an array element
    JsonElement* hobbiesArray = json_create(JSON_ARRAY);
    JsonElement* hobby1 = json_create(JSON_STRING);
    hobby1->value.string_val = string_strdup("Reading");
    vector_push_back(hobbiesArray->value.array_val, &hobby1);
    JsonElement* hobby2 = json_create(JSON_STRING);
    hobby2->value.string_val = string_strdup("Hiking");
    vector_push_back(hobbiesArray->value.array_val, &hobby2);
    json_set_element(jsonObject, "hobbies", hobbiesArray);

    // Add a nested object
    JsonElement* addressObject = json_create(JSON_OBJECT);
    JsonElement* streetElement = json_create(JSON_STRING);

    streetElement->value.string_val = string_strdup("123 Main St");
    json_set_element(addressObject, "street", streetElement);

    JsonElement* cityElement = json_create(JSON_STRING);
    cityElement->value.string_val = string_strdup("Anytown");
    json_set_element(addressObject, "city", cityElement);
    json_set_element(jsonObject, "address", addressObject);

    char* formattedJson = json_format(jsonObject);
    if (formattedJson) {
        fmt_printf("Formatted JSON:\n%s\n", formattedJson);

        const char* filename = "./sources/output_json.json";
        if (json_write_to_file(jsonObject, filename)) {
            fmt_printf("JSON successfully written to '%s'.\n", filename);
        } 
        else {
            fmt_printf("Failed to write JSON to file '%s'.\n", filename);
        }

        free(formattedJson);
    } 
    else {
        fmt_printf("Failed to format JSON.\n");
    }

    json_deallocate(jsonObject);
    return 0;
}
```

## Example 36 : Parsing a Single Number as json 

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonString = "42";
    JsonElement* jsonElement = json_parse(jsonString);

    if (jsonElement) {
        if (jsonElement->type == JSON_NUMBER) {
            fmt_printf("Parsed JSON number: %f\n", jsonElement->value.number_val);
        } 
        else {
            fmt_printf("Parsed JSON element is not a number.\n");
        }
        json_deallocate(jsonElement);
    } 
    else {
        fmt_printf("Failed to parse JSON string.\n");
    }

    return 0;
}
```

## Example 37 : Parsing a Single String value as json 

```c
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
```

## Example 38 : create shallow copy of original json object with `json_clone`

```c
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
```

## Example 39 : Reading JSON from a File and Getting its Keys with `json_get_keys`

```c
#include "json/json.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    const char* jsonFilePath = "./sources/json_example.json";
    JsonElement* jsonElement = json_read_from_file(jsonFilePath);

    if (jsonElement && jsonElement->type == JSON_OBJECT) {
        size_t num_keys;
        char** keys = json_get_keys(jsonElement, &num_keys);

        if (keys) {
            fmt_printf("Keys in JSON object:\n");
            for (size_t i = 0; i < num_keys; ++i) {
                fmt_printf("  Key %zu: %s\n", i + 1, keys[i]);
                free(keys[i]); 
            }
            free(keys);
        } 
        else {
            fmt_printf("Failed to get keys from JSON object.\n");
        }
        json_deallocate(jsonElement);
    } 
    else {
        fmt_printf("Failed to parse JSON file '%s'.\n", jsonFilePath);
    }

    return 0;
}
```

## Example 40 : Using a JSON String and Getting its Keys with `json_get_keys`

```c
#include "json/json.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    const char* jsonString = "{\"name\": \"John Doe\", \"age\": 30, \"city\": \"New York\"}";
    JsonElement* jsonElement = json_parse(jsonString);

    if (jsonElement && jsonElement->type == JSON_OBJECT) {
        size_t num_keys;
        char** keys = json_get_keys(jsonElement, &num_keys);

        if (keys) {
            fmt_printf("Keys in JSON object:\n");
            for (size_t i = 0; i < num_keys; ++i) {
                fmt_printf("  Key %zu: %s\n", i + 1, keys[i]);
                free(keys[i]);
            }
            free(keys); 
        } 
        else {
            fmt_printf("Failed to get keys from JSON object.\n");
        }
        json_deallocate(jsonElement);
    } 
    else {
        fmt_printf("Failed to parse JSON string.\n");
    }

    return 0;
}
```

## Example 41 : how to add items to array element in json with `json_add_to_array` 

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    JsonElement* root = json_read_from_file("./sources/json_example.json");

    JsonElement* categories = json_get_element(root, "categories");
    JsonElement* moviesCategory = json_get_element(categories, "1"); // Direct access if you know the position
    JsonElement* moviesItems = json_get_element(moviesCategory, "items");

    JsonElement* newMovie = json_create(JSON_OBJECT);

    // Set properties on the new movie object
    JsonElement* title = json_create(JSON_STRING);
    title->value.string_val = "JSON's Adventure";
    json_set_element(newMovie, "title", title);

    JsonElement* director = json_create(JSON_STRING);
    director->value.string_val = "Chris Coder";
    json_set_element(newMovie, "director", director);

    JsonElement* releasedYear = json_create(JSON_NUMBER);
    releasedYear->value.number_val = 2024;
    json_set_element(newMovie, "released_year", releasedYear);

    // Create and set ratings object
    JsonElement* ratings = json_create(JSON_OBJECT);
    JsonElement* imdbRating = json_create(JSON_NUMBER);
    imdbRating->value.number_val = 8.5;
    json_set_element(ratings, "IMDB", imdbRating);

    JsonElement* rtRating = json_create(JSON_STRING);
    rtRating->value.string_val = "92%";
    json_set_element(ratings, "Rotten Tomatoes", rtRating);
    json_set_element(newMovie, "ratings", ratings);
    
    if (json_add_to_array(moviesItems, newMovie)) {
        fmt_printf("add successfully to array\n");
    } 
    else {
        fmt_printf("failed in adding to array\n");
    }

    if (json_write_to_file(root, "./sources/modified_json_example.json")) {
        fmt_printf("Write to file successfully.\n");
    } 
    else {
        fmt_printf("Failed to write to file.\n");
    }

    json_deallocate(root);
    return 0;
}
```

## Example 42 : how to add object to json as new element with `json_add_to_object`

```c
#include "json/json.h"
#include "fmt/fmt.h"
#include "string/string.h"

int main() {
    JsonElement* root = json_read_from_file("./sources/json_example.json");
    JsonElement* jsonObject = json_create(JSON_OBJECT);

    JsonElement* titleElement = json_create(JSON_STRING);
    titleElement->value.string_val = string_strdup("Introduction to JSON");
    
    // Add the string element to the object with the key "title"
    if (!json_add_to_object(jsonObject, "title", titleElement)) {
        fprintf(stderr, "Failed to add title to JSON object.\n");
        // Assume json_deallocate properly deallocates the entire JSON structure
        json_deallocate(jsonObject);
        return 1;
    }

    // Create a new number element
    JsonElement* yearElement = json_create(JSON_NUMBER);
    yearElement->value.number_val = 2024;
    
    // Add the number element to the object with the key "year"
    if (!json_add_to_object(jsonObject, "year", yearElement)) {
        fprintf(stderr, "Failed to add year to JSON object.\n");
        json_deallocate(jsonObject);
        return 1;
    }

    // Create a nested JSON object for the author
    JsonElement* authorObject = json_create(JSON_OBJECT);
    JsonElement* nameElement = json_create(JSON_STRING);
    nameElement->value.string_val = string_strdup("John Doe");
    json_add_to_object(authorObject, "name", nameElement);

    // Add the nested object to the main JSON object with the key "author"
    if (!json_add_to_object(jsonObject, "author", authorObject)) {
        fprintf(stderr, "Failed to add author to JSON object.\n");
        json_deallocate(jsonObject);
        return 1;
    }

    // add to root object 
    if (!json_add_to_object(root, "additional_info", jsonObject)) {
        fmt_printf("Can not add new object to root");
    }

    json_print(root);

    json_deallocate(root);
    return 0;
}
```

## Example 42 : quering over the json with `json_query`

```c
#include "json/json.h"
#include "fmt/fmt.h"
#include "string/string.h"

int main() {
    JsonElement* root = json_read_from_file("./sources/json_example.json");

    // Query for the title of the first book
    JsonElement* bookTitleElement = json_query(root, "categories[0].items[0].title");
    if (bookTitleElement && bookTitleElement->type == JSON_STRING) {
        fmt_printf("Title of the first book: %s\n", bookTitleElement->value.string_val);
    } 
    else {
        fmt_printf("Failed to query the title of the first book.\n");
    }

    // Query for the IMDB rating of "The JSON Saga"
    JsonElement* imdbRatingElement = json_query(root, "categories[1].items[0].ratings.IMDB");
    if (imdbRatingElement && imdbRatingElement->type == JSON_NUMBER) {
        fmt_printf("IMDB rating of 'The JSON Saga': %.1f\n", imdbRatingElement->value.number_val);
    } 
    else {
        fmt_printf("Failed to query the IMDB rating of 'The JSON Saga'.\n");
    }

    json_deallocate(root);
    return 0;
}
```

## Example 43 : set query over json string with `json_query`

```c
#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonString = 
        "{"
        "  \"technology\": {"
        "    \"products\": ["
        "      {"
        "        \"category\": \"Laptops\","
        "        \"items\": ["
        "          {"
        "            \"name\": \"Laptop A\","
        "            \"brand\": \"BrandOne\","
        "            \"price\": 1200,"
        "            \"ratings\": {"
        "              \"tech_site\": 9.1,"
        "              \"user_reviews\": 8.5"
        "            }"
        "          },"
        "          {"
        "            \"name\": \"Laptop B\","
        "            \"brand\": \"BrandTwo\","
        "            \"price\": 1500,"
        "            \"ratings\": {"
        "              \"tech_site\": 9.3,"
        "              \"user_reviews\": 9.0"
        "            }"
        "          }"
        "        ]"
        "      },"
        "      {"
        "        \"category\": \"Smartphones\","
        "        \"items\": ["
        "          {"
        "            \"name\": \"Smartphone A\","
        "            \"brand\": \"BrandThree\","
        "            \"price\": 700,"
        "            \"ratings\": {"
        "              \"tech_site\": 8.5,"
        "              \"user_reviews\": 8.8"
        "            }"
        "          },"
        "          {"
        "            \"name\": \"Smartphone B\","
        "            \"brand\": \"BrandFour\","
        "            \"price\": 950,"
        "            \"ratings\": {"
        "              \"tech_site\": 9.0,"
        "              \"user_reviews\": 9.2"
        "            }"
        "          }"
        "        ]"
        "      }"
        "    ]"
        "  }"
        "}";

    JsonElement* root = json_parse(jsonString);
    JsonElement* laptopAPriceElement = json_query(root, "technology.products[0].items[0].price");

    if (laptopAPriceElement && laptopAPriceElement->type == JSON_NUMBER) {
        fmt_printf("Price of Laptop A: %.2f\n", laptopAPriceElement->value.number_val);
    } 
    else {
        fmt_printf("Failed to query the price of Laptop A.\n");
    }

    JsonElement* smartphoneBRatingElement = json_query(root, "technology.products[1].items[1].ratings.tech_site");
    if (smartphoneBRatingElement && smartphoneBRatingElement->type == JSON_NUMBER) {
        fmt_printf("Tech site rating of 'Smartphone B': %.1f\n", smartphoneBRatingElement->value.number_val);
    } 
    else {
        fmt_printf("Failed to query the tech site rating of 'Smartphone B'.\n");
    }

    json_deallocate(root);
    return 0;
}
```