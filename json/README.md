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


### `void json_deallocate(JsonElement *element)`

- **Purpose**: Deallocates a `JsonElement` and all of its associated resources.
- **Parameters**:
  - `element`: A pointer to the `JsonElement` to be deallocated. If `NULL`, the function does nothing.
- **Details**:
  - **`JSON_STRING`**: Frees the memory associated with the string value.
  - **`JSON_ARRAY`**: Recursively deallocates all elements within the array, then deallocates the array itself.
  - **`JSON_OBJECT`**: Deallocates the entire object, including all key-value pairs in the object map.
  - **Other Types** (`JSON_NULL`, `JSON_BOOL`, `JSON_NUMBER`): No special handling is needed; the element is freed.
- **Return**: No return value (`void`), but frees the memory associated with the `JsonElement`.

---

### `JsonElement* json_create(JsonType type)`

- **Purpose**: Creates a new JSON element of the specified type.
- **Parameters**:
  - `type`: The type of JSON element to create. This can be one of the following:
    - `JSON_NULL`
    - `JSON_BOOL`
    - `JSON_NUMBER`
    - `JSON_STRING`
    - `JSON_ARRAY`
    - `JSON_OBJECT`
- **Details**:
  - Allocates memory for a `JsonElement` and initializes it based on the provided type:
    - **`JSON_NULL`**: No value is needed.
    - **`JSON_BOOL`**: Initializes to `false`.
    - **`JSON_NUMBER`**: Initializes to `0.0`.
    - **`JSON_STRING`**: Initializes to `NULL`.
    - **`JSON_ARRAY`**: Allocates memory for a dynamic array to hold elements.
    - **`JSON_OBJECT`**: Allocates memory for a map to hold key-value pairs.
- **Return**: A pointer to the newly created `JsonElement`, or `NULL` if memory allocation fails or if an invalid type is provided.
- **Warnings**: 
  - If an error occurs during memory allocation, the function logs an error and returns `NULL`.
  - The caller is responsible for deallocating the returned `JsonElement` using `json_deallocate` to avoid memory leaks.

---

### `JsonElement* json_parse(const char* json_str)`

- **Purpose**: Parses a JSON string and returns the corresponding JSON element tree.
- **Parameters**:
  - `json_str`: The JSON-formatted string to be parsed.
- **Details**:
  - This function parses the given JSON string into a `JsonElement` structure, handling various JSON types such as:
    - **Objects**
    - **Arrays**
    - **Strings**
    - **Numbers**
    - **Booleans**
    - **Null values**
  - If the parsing encounters an error, the function logs the error and returns `NULL`.
- **Return**: A pointer to the root `JsonElement` of the parsed structure, or `NULL` if parsing fails.
- **Warnings**:
  - The caller is responsible for deallocating the returned `JsonElement` using `json_deallocate` to avoid memory leaks.
  - If the input string contains invalid JSON or if an unexpected token is encountered, the function will return `NULL` and log the corresponding error message.

---

### `JsonElement * json_read_from_file(const char* filename)`

- **Purpose**: Reads a JSON file from disk and parses it into a JSON element tree.
- **Parameters**:
  - `filename`: Path to the JSON file to be read.
- **Details**:
  - This function opens the specified JSON file, reads its content, and parses it into a `JsonElement` structure.
  - It handles errors related to file operations (e.g., file not found, read errors) and JSON parsing.
  - If any error occurs (such as failure to open the file, memory allocation failure, or invalid JSON), the function logs the error and returns `NULL`.
- **Return**: A pointer to the root `JsonElement` of the parsed JSON structure, or `NULL` if reading or parsing fails.
- **Warnings**:
  - The caller is responsible for deallocating the returned `JsonElement` using `json_deallocate` to avoid memory leaks.
  - Ensure the file content is in valid JSON format; otherwise, parsing will fail.

---

### `void json_print(const JsonElement* element)`

- **Purpose**: Prints a JSON element and its children in a formatted and human-readable manner.
- **Parameters**:
  - `element`: The JSON element to be printed.
- **Details**:
  - This function formats and prints the `JsonElement` and its children to standard output.
  - It handles all JSON types, including objects, arrays, strings, numbers, booleans, and null.
  - Proper indentation is applied for better readability.
- **Return**: No return value, but the JSON element is printed to the standard output.
- **Note**: If the input `element` is `NULL`, it prints `"null"`.

---

### `JsonElement* json_get_element(const JsonElement *element, const char *key_or_index)`

- **Purpose**: Retrieves a specific element from a JSON object or array by key or index.
- **Parameters**:
  - `element`: The JSON element (must be an object or array).
  - `key_or_index`: The key (for objects) or index (for arrays) of the element to retrieve.
- **Details**:
  - This function accesses a JSON object by key or a JSON array by index.
  - If the input is not an object or array, or if the key or index is invalid, it logs an error and returns `NULL`.
- **Return**: A pointer to the retrieved `JsonElement`, or `NULL` if the key/index is invalid or an error occurs.
- **Warning**: The returned element is part of the original structure; modifying it will affect the original JSON structure.

---

### `size_t json_array_size(const JsonElement *array)`

- **Purpose**: Returns the number of elements in a JSON array.
- **Parameters**:
  - `array`: The JSON element representing an array.
- **Return**: The number of elements in the array, or `0` if the input is not a valid JSON array.
- **Details**:
  - This function checks if the provided JSON element is an array and returns its size.
  - If the input is not a valid JSON array or is `NULL`, it logs an error and returns `0`.

---

### `size_t json_object_size(const JsonElement* object)`

- **Purpose**: Returns the number of key-value pairs in a JSON object.
- **Parameters**:
  - `object`: The JSON element representing an object.
- **Return**: The number of key-value pairs in the object, or `0` if the input is not a valid JSON object.
- **Details**:
  - This function checks if the provided JSON element is an object and returns its size.
  - If the input is not a valid JSON object or is `NULL`, it logs an error and returns `0`.

---

### `JsonElement* json_deep_copy(const JsonElement *element)`

- **Purpose**: Creates a deep copy of a JSON element, recursively copying nested elements.
- **Parameters**:
  - `element`: The JSON element to be copied.
- **Return**: A pointer to the newly created deep copy of the JSON element, or `NULL` if memory allocation fails.
- **Details**:
  - The function makes a deep copy of the provided JSON element, including its nested elements in the case of arrays and objects.
  - If memory allocation fails at any point, it logs an error, deallocates any partially copied data, and returns `NULL`.
- **Warning**: The caller is responsible for deallocating the returned JSON element using `json_deallocate` to prevent memory leaks.

---

### `JsonType json_type_of_element(const JsonElement *element)`

- **Purpose**: Returns the type of a JSON element.
- **Parameters**:
  - `element`: The JSON element whose type is to be determined.
- **Return**: The type of the JSON element, or `JSON_NULL` if the input is `NULL`.
- **Details**:
  - This function returns the type of the provided JSON element. If the input is `NULL`, the function returns `JSON_NULL`.

---

### `bool json_write_to_file(const JsonElement *element, const char *filename)`

- **Purpose**: Writes a JSON element to a file in serialized JSON format.
- **Parameters**:
  - `element`: The JSON element to serialize and write to a file.
  - `filename`: The name of the file to which the JSON data will be written.
- **Return**: `true` if the JSON element was successfully written to the file; `false` otherwise.
- **Details**:
  - This function serializes a JSON element and writes it to a specified file.
  - If the file cannot be opened or the serialization fails, an error is logged, and `false` is returned.
- **Warning**: The function will overwrite the file if it already exists.

---

### `char* json_serialize(const JsonElement* element)`

- **Purpose**: Serializes a JSON element into a JSON-formatted string.
- **Parameters**:
  - `element`: The JSON element to serialize.
- **Return**: A dynamically allocated string containing the serialized JSON, or `NULL` if an error occurs.
- **Details**:
  - The function converts a JSON element into its string representation in JSON format.
  - The caller is responsible for freeing the returned string to prevent memory leaks.
- **Warning**: The returned string must be freed by the caller.

---

### `bool json_compare(const JsonElement *element1, const JsonElement *element2)`

- **Purpose**: Compares two JSON elements for equality.
- **Parameters**:
  - `element1`, `element2`: The two JSON elements to compare.
- **Return**: `true` if the elements are equal; `false` otherwise.
- **Details**:
  - The function performs a deep comparison, comparing nested elements (arrays, objects) recursively.
  - Elements of different types are considered unequal. Two `NULL` elements are considered equal.

---

### `bool json_set_element(JsonElement *element, const char *key_or_index, JsonElement *new_element)`

- **Purpose**: Sets a value in a JSON object or array at a specified key or index.
- **Parameters**:
  - `element`: The JSON object or array in which to set the value.
  - `key_or_index`: The key (for objects) or index (for arrays) where the value should be set.
  - `new_element`: The new JSON element to set at the specified key or index.
- **Return**: `true` if the value was successfully set; `false` otherwise.
- **Details**:
  - For objects, the key is used to identify where to set the value. For arrays, the index is used.
  - Replaces existing elements in arrays and adds or updates key-value pairs in objects.

---

### `bool json_remove_element(JsonElement *element, const char *key_or_index)`

- **Purpose**: Removes an element from a JSON object or array by key or index.
- **Parameters**:
  - `element`: The JSON object or array from which to remove an element.
  - `key_or_index`: The key (for objects) or index (for arrays) identifying the element to remove.
- **Return**: `true` if the element was successfully removed; `false` otherwise.
- **Details**: 
  - For objects, the key is used to locate and remove the key-value pair.
  - For arrays, the index is used to remove the element at the specified position.

---

### `JsonElement* json_find(const JsonElement *element, JsonPredicate predicate, void *user_data)`

- **Purpose**: Finds an element within a JSON object or array that matches a given predicate.
- **Parameters**:
  - `element`: The JSON object or array to search.
  - `predicate`: Function pointer to the predicate used to test elements.
  - `user_data`: Additional data passed to the predicate function.
- **Return**: Pointer to the first matching JSON element, or `NULL` if no match is found.
- **Details**: 
  - Searches through the elements of a JSON object or array, returning the first element that satisfies the predicate.

---

### `JsonError json_last_error()`

- **Purpose**: Retrieves the last error that occurred during JSON operations.
- **Parameters**: None.
- **Return**: A `JsonError` struct containing the error code and message of the last error.
- **Details**: 
  - Provides access to the last error that occurred in any JSON operation. 

---

### `JsonElement* json_merge(const JsonElement *element1, const JsonElement *element2)`

- **Purpose**: Merges two JSON objects into a new JSON object.
- **Parameters**:
  - `element1`, `element2`: The JSON objects to merge.
- **Return**: A new JSON object containing the merged contents, or `NULL` if an error occurs.
- **Details**: 
  - The function creates a new JSON object and merges key-value pairs from both input objects. If a key exists in both objects, the value from the second object is used.

---

### `char** json_to_string_array(const JsonElement *array, size_t *length)`

- **Purpose**: Converts a JSON array of strings into a C-style array of strings.
- **Parameters**:
  - `array`: The JSON array to convert. Must contain only `JSON_STRING` elements.
  - `length`: Pointer to a variable where the number of strings will be stored.
- **Return**: A C-style array of strings, or `NULL` if an error occurs.
- **Details**: 
  - Extracts strings from the JSON array and returns them in a newly allocated array.
  - If the array contains non-string elements or is invalid, `NULL` is returned.

---

### `void* json_convert(const JsonElement *element, JsonType type)`

- **Purpose**: Converts a JSON element to a specified type (e.g., `JSON_STRING`, `JSON_NUMBER`, `JSON_BOOL`, `JSON_ARRAY`, `JSON_OBJECT`).
- **Parameters**:
  - `element`: The JSON element to convert.
  - `type`: The target type for the conversion.
- **Return**: A new `JsonElement` of the specified type, or `NULL` if the conversion fails.
- **Details**: 
  - Handles conversion between JSON types (e.g., number to string, string to number, boolean to string).
  - Returns `NULL` if the conversion is not supported or if memory allocation fails.
  - The caller is responsible for deallocating the returned element.

---

### `JsonElement* json_map(const JsonElement* array, JsonMapFunction map_func, void* user_data)`

- **Purpose**: Applies a transformation function to each element of a JSON array and returns a new JSON array with the transformed elements.
- **Parameters**:
  - `array`: The JSON array to transform. Must be of type `JSON_ARRAY`.
  - `map_func`: A function that takes a JSON element and user data as arguments and returns a transformed JSON element.
  - `user_data`: Optional user data to pass to the mapping function. Can be `NULL`.
- **Return**: A new JSON array containing the transformed elements, or `NULL` if an error occurs.
- **Details**: 
  - If the transformation fails for any element, the operation stops, and the entire result array is deallocated.

---

### `JsonElement* json_filter(const JsonElement *array, JsonPredicate predicate, void *user_data)`

- **Purpose**: Filters elements of a JSON array based on a predicate function and returns a new JSON array with elements that satisfy the predicate.
- **Parameters**:
  - `array`: The JSON array to filter. Must be of type `JSON_ARRAY`.
  - `predicate`: A function that takes a JSON element and user data as arguments and returns `true` if the element should be included in the result.
  - `user_data`: Optional user data to pass to the predicate function. Can be `NULL`.
- **Return**: A new JSON array containing the elements that satisfy the predicate, or `NULL` if an error occurs.
- **Details**: 
  - If an error occurs during the operation, the partially constructed result array is deallocated, and `NULL` is returned.

---

### `void* json_reduce(const JsonElement *array, JsonReduceFunction reduce_func, void *initial_value, void *user_data)`

- **Purpose**: Applies a reduction function across all elements of a JSON array, accumulating the result.
- **Parameters**:
  - `array`: The JSON array to reduce. Must be of type `JSON_ARRAY`.
  - `reduce_func`: The function to apply to each element and the accumulator. Takes a JSON element, the current accumulator, and user data as arguments.
  - `initial_value`: The initial value for the accumulator.
  - `user_data`: Optional user data to pass to the reduction function.
- **Return**: The final accumulated value, or `NULL` if an error occurs.
- **Details**: 
  - The function iterates over the elements, applying the reduction function and updating the accumulator. If any error occurs, it logs the issue and returns `NULL`.

---

### `char* json_format(const JsonElement *element)`

- **Purpose**: Formats a JSON element into a human-readable string.
- **Parameters**:
  - `element`: The JSON element to format.
- **Return**: A dynamically allocated string containing the formatted JSON, or `NULL` if an error occurs.
- **Details**:
  - The returned string is pretty-printed with indentation for readability. The caller is responsible for freeing the string.

---

### `JsonElement* json_clone(const JsonElement *element)`

- **Purpose**: Creates a shallow copy of a JSON element.
- **Parameters**:
  - `element`: The JSON element to clone.
- **Return**: A pointer to the newly cloned JSON element, or `NULL` if an error occurs.
- **Details**: 
  - For JSON objects and arrays, the copy references the same underlying data as the original. For strings, a new copy is allocated. The caller is responsible for deallocating the cloned element.

---

### `char** json_get_keys(const JsonElement *object, size_t *num_keys)`

- **Purpose**: Retrieves all the keys from a JSON object.
- **Parameters**:
  - `object`: The JSON object from which to retrieve the keys.
  - `num_keys`: A pointer to a `size_t` variable where the number of keys will be stored.
- **Return**: A dynamically allocated array of strings containing the keys, or `NULL` if an error occurs.
- **Details**: 
  - Allocates memory for both the array of keys and each individual key. The caller is responsible for freeing the allocated memory.

---

### `bool json_add_to_array(JsonElement* element1, JsonElement* element2)`

- **Purpose**: Adds an element to a JSON array.
- **Parameters**:
  - `element1`: The JSON array to which the element will be added.
  - `element2`: The JSON element to add to the array.
- **Return**: `true` if the element was successfully added, `false` otherwise.
- **Details**: 
  - Appends the provided element to the end of the JSON array. If the operation fails, it returns `false` and sets an error code.

---

### `bool json_add_to_object(JsonElement* object, const char* key, JsonElement* value)`

- **Purpose**: Adds a key-value pair to a JSON object.
- **Parameters**:
  - `object`: The JSON object to which the key-value pair will be added.
  - `key`: The key as a string.
  - `value`: The value to associate with the key.
- **Return**: `true` if the key-value pair was successfully added or replaced, `false` otherwise.
- **Details**: 
  - If the key already exists, its value is replaced. The key is duplicated to ensure independent management in the object. Any existing value is deallocated.

---

### `JsonElement* json_query(const JsonElement *element, const char *query)`

- **Purpose**: Queries a JSON element using a dot-separated path.
- **Parameters**:
  - `element`: The JSON element to query.
  - `query`: The query string, using dot notation to specify the path to the desired element.
- **Return**: The JSON element found at the specified path, or `NULL` if the element could not be found or if an error occurs.
- **Details**: 
  - The query can include array indexing (e.g., `"array[0].key"`). The function returns the corresponding JSON element or `NULL` if a key or index is missing.


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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

## License

This project is open-source and available under [ISC License].