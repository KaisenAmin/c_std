# Config Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2023  
**License:** ISC License  

## Overview

The ConfigFile library is a versatile and easy-to-use solution for handling configuration files. It allows developers to read, modify, and save configurations in the standard INI file format. The library supports sections, key-value pairs, and comments, making it ideal for managing configuration data in various C applications.

## Features

- **Read Configuration**: Load configuration data from INI files with support for sections, keys, and comments.
- **Modify Configuration**: Programmatically change configuration settings.
- **Save Configuration**: Write updated configuration back to a file.
- **Memory Management**: Automatic handling of memory allocation and deallocation for configuration data.
- **Encrypted Values**: Support for storing and retrieving encrypted values.
- **Dynamic Configuration Management**: Add and remove sections and keys dynamically.
- **Iteration Support**: Iterate through configuration entries using an iterator.

## Compilation

To compile the Config library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./config/config.c 
g++ -std=c++14 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.cpp

```

## Usage

To use the config library in your project, include the `config.h` header file in your source code.

## Note
in these examples i rewrite cpp example in Bitset code 

```c
#include "config/config.h"
```

## Function Descriptions

### 1. `ConfigFile *config_create(const char *filename);`
Creates a new `ConfigFile` structure by loading configuration data from the specified file.

- **Parameters**:
  - `filename`: Path to the INI file to be loaded.
- **Returns**: 
  - A pointer to the `ConfigFile` structure.
- **Usage**:
  ```c
  ConfigFile *config = config_create("config.ini");
  ```

### 2. `const char *config_get_value(const ConfigFile *config, const char *section, const char *key);`
Retrieves the value associated with the specified key in the given section.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section name where the key is located.
  - `key`: The key whose value is to be retrieved.
- **Returns**:
  - The value associated with the key, or `NULL` if the key does not exist.
- **Usage**:
  ```c
  const char *value = config_get_value(config, "global", "log_level");
  ```

### 3. `void config_set_value(ConfigFile *config, const char *section, const char *key, const char *value);`
Sets or updates the value for a specific key in the given section.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section where the key is located.
  - `key`: The key whose value is to be set or updated.
  - `value`: The new value to be assigned to the key.
- **Usage**:
  ```c
  config_set_value(config, "global", "log_level", "debug");
  ```

### 4. `void config_save(const ConfigFile *config, const char *filename);`
Saves the current configuration to the specified file.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `filename`: Path to the file where the configuration will be saved.
- **Usage**:
  ```c
  config_save(config, "modified_config.ini");
  ```

### 5. `void config_remove_section(ConfigFile *config, const char *section);`
Removes an entire section from the configuration.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section to be removed.
- **Usage**:
  ```c
  config_remove_section(config, "deprecated_section");
  ```

### 6. `void config_remove_key(ConfigFile *config, const char *section, const char *key);`
Removes a specific key-value pair from a section in the configuration.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section where the key is located.
  - `key`: The key to be removed.
- **Usage**:
  ```c
  config_remove_key(config, "global", "old_key");
  ```

### 7. `void config_deallocate(ConfigFile *config);`
Frees all memory associated with the configuration structure.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure to be deallocated.
- **Usage**:
  ```c
  config_deallocate(config);
  ```

### 8. `bool config_has_section(const ConfigFile *config, const char *section);`
Checks if a specific section exists in the configuration.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section name to check for existence.
- **Returns**:
  - `true` if the section exists, `false` otherwise.
- **Usage**:
  ```c
  if (config_has_section(config, "user_preferences")) {
      // Section exists
  }
  ```

### 9. `bool config_has_key(const ConfigFile *config, const char *section, const char *key);`
Checks if a specific key exists within a section.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section name where the key should be located.
  - `key`: The key to check for existence.
- **Returns**:
  - `true` if the key exists, `false` otherwise.
- **Usage**:
  ```c
  if (config_has_key(config, "user_preferences", "theme")) {
      // Key exists
  }
  ```

### 10. `int config_get_int(const ConfigFile *config, const char *section, const char *key, int default_value);`
Retrieves an integer value for a specific key, or returns a default value if the key is not found.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section name where the key is located.
  - `key`: The key whose integer value is to be retrieved.
  - `default_value`: The default value to return if the key does not exist.
- **Returns**:
  - The integer value associated with the key, or the default value if not found.
- **Usage**:
  ```c
  int log_level = config_get_int(config, "global", "log_level", 0);
  ```

### 11. `double config_get_double(const ConfigFile *config, const char *section, const char *key, double default_value);`
Retrieves a double value for a specific key, or returns a default value if the key is not found.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section name where the key is located.
  - `key`: The key whose double value is to be retrieved.
  - `default_value`: The default value to return if the key does not exist.
- **Returns**:
  - The double value associated with the key, or the default value if not found.
- **Usage**:
  ```c
  double threshold = config_get_double(config, "settings", "threshold", 0.5);
  ```

### 12. `bool config_get_bool(const ConfigFile *config, const char *section, const char *key, bool default_value);`
Retrieves a boolean value for a specific key, or returns a default value if the key is not found.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section name where the key is located.
  - `key`: The key whose boolean value is to be retrieved.
  - `default_value`: The default value to return if the key does not exist.
- **Returns**:
  - The boolean value associated with the key, or the default value if not found.
- **Usage**:
  ```c
  bool enabled = config_get_bool(config, "global", "enabled", false);
  ```

### 13. `char **config_get_array(const ConfigFile *config, const char *section, const char *key, size_t *array_size);`
Retrieves an array of strings for a given key in a section.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section name where the key is located.
  - `key`: The key whose value is an array of strings.
  - `array_size`: Pointer to a size_t variable where the size of the array will be stored.
- **Returns**:
  - A dynamically allocated array of strings. The caller is responsible for freeing the memory.
- **Usage**:
  ```c
  size_t array_size;
  char **ips = config_get_array(config, "network", "server_ips", &array_size);
  ```

### 14. `void config_set_array(ConfigFile *config, const char *section, const char *key, const char *const *array, size_t array_size);`
Sets an array of strings for a given key in a section.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section where the key is located.
  - `key`: The key whose value will be set as an array of strings.
  - `array`: The array of strings to be set.
  - `array_size`: The size of the array.
- **Usage**:
  ```c
  const char *ips[] = {"192.168.1.1

", "192.168.1.2"};
  config_set_array(config, "network", "server_ips", ips, 2);
  ```

### 15. `char *config_get_encrypted_value(const ConfigFile *config, const char *section, const char *key, const char *encryption_key);`
Retrieves an encrypted value for a specific key in a section, and decrypts it using the provided encryption key.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section name where the key is located.
  - `key`: The key whose value is encrypted.
  - `encryption_key`: The key to decrypt the value.
- **Returns**:
  - The decrypted value as a string. The caller is responsible for freeing the memory.
- **Usage**:
  ```c
  char *api_key = config_get_encrypted_value(config, "advanced", "api_key", "secret_key");
  ```

### 16. `void config_set_encrypted_value(ConfigFile *config, const char *section, const char *key, const char *value, const char *encryption_key);`
Sets an encrypted value for a specific key in a section.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section where the key is located.
  - `key`: The key whose value will be encrypted.
  - `value`: The value to be encrypted and set.
  - `encryption_key`: The key to encrypt the value.
- **Usage**:
  ```c
  config_set_encrypted_value(config, "advanced", "api_key", "12345", "secret_key");
  ```

### 17. `void config_set_comment(ConfigFile *config, const char *section, const char *comment);`
Adds a comment to a specific section in the configuration.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section name to which the comment will be added.
  - `comment`: The comment text.
- **Usage**:
  ```c
  config_set_comment(config, "global", "Global settings for the application");
  ```

### 18. `ConfigIterator config_get_iterator(const ConfigFile *config);`
Initializes an iterator for traversing the configuration entries.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
- **Returns**:
  - A `ConfigIterator` structure initialized to the beginning of the configuration.
- **Usage**:
  ```c
  ConfigIterator it = config_get_iterator(config);
  ```

### 19. `bool config_next_entry(ConfigIterator *iterator, const char **section, const char **key, const char **value);`
Iterates to the next entry in the configuration, returning false if at the end.

- **Parameters**:
  - `iterator`: Pointer to the `ConfigIterator` structure.
  - `section`: Pointer to a string that will be set to the current section name.
  - `key`: Pointer to a string that will be set to the current key.
  - `value`: Pointer to a string that will be set to the current value.
- **Returns**:
  - `true` if there are more entries to iterate over, `false` otherwise.
- **Usage**:
  ```c
  const char *section, *key, *value;
  while (config_next_entry(&it, &section, &key, &value)) {
      fmt_printf("Section: %s, Key: %s, Value: %s\n", section, key, value);
  }
  ```

### 20. `void config_reload(ConfigFile **config_ptr);`
Reloads the configuration from the file, updating the in-memory representation.

- **Parameters**:
  - `config_ptr`: Double pointer to the `ConfigFile` structure to be reloaded.
- **Usage**:
  ```c
  config_reload(&config);
  ```

### 21. `void config_register_modification_callback(ConfigFile *config, void (*callback)(const char *section, const char *key, const char *value));`
Registers a callback function to be called upon modifications to the configuration.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `callback`: The callback function to be registered.
- **Usage**:
  ```c
  config_register_modification_callback(config, my_callback_function);
  ```

### 22. `void config_validate_structure(const ConfigFile *config, const ConfigSection *expected_structure, size_t structure_size);`
Validates the structure of the configuration against an expected template.

- **Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `expected_structure`: Pointer to an array of `ConfigSection` structures that define the expected structure.
  - `structure_size`: The number of elements in the `expected_structure` array.
- **Usage**:
  ```c
  const ConfigSection expected_structure[] = {{"global", NULL, 0, NULL}, {"user_preferences", NULL, 0, NULL}};
  config_validate_structure(config, expected_structure, sizeof(expected_structure) / sizeof(ConfigSection));
  ```

## Examples

## Example 1 Usage

The following example demonstrates how to use the ConfigFile library to read, modify, and save configuration settings:

```c
#include "config/config.h"
#include "fmt/fmt.h"

int main() {
    ConfigFile *config = config_create("sources/config.ini");
    if (!config) {
        fmt_printf("Failed to load configuration.\n");
        return 1;
    }
    // Get a configuration value
    const char *log_level = config_get_value(config, "global", "log_level");
    if (log_level) {
        fmt_printf("Log Level: %s\n", log_level);
    }
    else {
        fmt_printf("Failed to get log level.\n");
    }
    // Set a configuration value
    config_set_value(config, "user_preferences", "theme", "light");
    
    // Retrieve updated configuration value
    const char *theme = config_get_value(config, "user_preferences", "theme");
    if (theme) { 
        fmt_printf("Theme: %s\n", theme);
    }
    else { 
        fmt_printf("Failed to get theme.\n");
    }
    
    // Save the modified configuration to a new file
    config_save(config, "sources/modified_config.ini");
    // Deallocate the configuration to free memory
    config_deallocate(config);
    return 0;
}
```

## Example 2: Using Comments and Checking for Sections/Keys

```c
#include "config/config.h"
#include "fmt/fmt.h"

int main() {
    ConfigFile *config = config_create("sources/config.ini");
    config_set_comment(config, "global", "Global settings for the application"); // Set a comment for a section

    // Check if a section exists
    if (config_has_section(config, "user_preferences")) { 
        fmt_printf("Section 'user_preferences' exists.\n");
    }
    // Check if a key exists in a section
    if (config_has_key(config, "user_preferences", "theme")) {
        fmt_printf("Key 'theme' exists in section 'user_preferences'.\n");
    }

    config_save(config, "sources/config_with_comments.ini");
    config_deallocate(config);
    return 0;
}
```

## Example 3: Handling Encrypted Values

In this example, we'll demonstrate how to store and retrieve encrypted configuration values. This is particularly useful for sensitive data like passwords or API keys.

```c
#include "config/config.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    ConfigFile *config = config_create("sources/config.ini"); // Create configuration from file
    const char *encryption_key = "secret_key"; // Set an encrypted value
    config_set_encrypted_value(config, "advanced", "api_key", "12345", encryption_key);

    // Retrieve and decrypt the value
    char *decrypted_api_key = config_get_encrypted_value(config, "advanced", "api_key", encryption_key);
    if (decrypted_api_key) {
        fmt_printf("Decrypted API Key: %s\n", decrypted_api_key);
        free(decrypted_api_key); // Important to free the memory
    } 
    else {
        fmt_printf("Failed to decrypt API key.\n");
    }

    config_save(config, "sources/encrypted_config.ini"); // Save the modified configuration
    config_deallocate(config);  // Deallocate the configuration
    return 0;
}
```

## Example 4: Iterating Through Configuration Entries

This example demonstrates how to use the `ConfigIterator` to iterate through all entries in the configuration file.

```c
#include "config/config.h"
#include "fmt/fmt.h"

int main() {
    ConfigFile *config = config_create("sources/config.ini"); // Create configuration from file
    ConfigIterator iterator = config_get_iterator(config);  // Initialize the iterator
    const char *section, *key, *value;

    // Iterate through all entries
    while (config_next_entry(&iterator, &section, &key, &value)) {
        fmt_printf("Section: %s, Key: %s, Value: %s\n", section, key, value);
    }
    
    config_deallocate(config); // Deallocate the configuration
    return 0;
}
```

## Example 5: Handling Arrays of Values
This example demonstrates how to store and retrieve an array of strings for a given key in a configuration section.

```c
#include "config/config.h"
#include "fmt/fmt.h"
#include <stdlib.h>


int main() {
    ConfigFile *config = config_create("sources/config.ini");

    // Define an array of values to store
    const char *server_ips[] = {"192.168.1.1", "192.168.1.2", "192.168.1.3"};
    config_set_array(config, "network", "server_ips", server_ips, 3); // set on buffer 

    // Retrieve the array of values
    size_t array_size;
    char **retrieved_ips = config_get_array(config, "network", "server_ips", &array_size); // get from buffer 

    if (retrieved_ips) {
        fmt_printf("Server IPs:\n");
        for (size_t i = 0; i < array_size; ++i) {
            fmt_printf("- %s\n", retrieved_ips[i]);
            free(retrieved_ips[i]); // Free each string in the array
        }
        free(retrieved_ips); // Free the array itself
    }

    config_save(config, "sources/modified_config.ini");
    config_deallocate(config);
    return 0;
}
```

## Example 6: Validating Configuration Structure
This example shows how to validate the configuration structure against an expected template, ensuring that required sections are present.

```c
#include "config/config.h"
#include "fmt/fmt.h"

int main() {
    ConfigFile *config = config_create("sources/config.ini");

    // Expected structure
    const ConfigSection expected_structure[] = {
        {"global", NULL, 0, NULL},
        {"user_preferences", NULL, 0, NULL},
        {"network", NULL, 0, NULL}
    };

    // Validate the structure
    config_validate_structure(config, expected_structure, sizeof(expected_structure) / sizeof(ConfigSection));
    
    // Iterate through the expected structure and check for each section
    for (size_t i = 0; i < sizeof(expected_structure) / sizeof(ConfigSection); ++i) {
        if (!config_has_section(config, expected_structure[i].section_name)) {
            fmt_printf("Section '%s' is missing in the configuration.\n", expected_structure[i].section_name);
        }
    }

    config_deallocate(config);
    return 0;
}
```

## Example 7 : Dynamic Configuration Management 

This example demonstrates how to dynamically manage a configuration by adding and removing sections and keys, and reloading the configuration from the file.

```c
#include "config/config.h"
#include "fmt/fmt.h"

int main() 
{
    ConfigFile *config = config_create("sources/config.ini");

    fmt_printf("Adding new section and keys...\n");
    config_set_value(config, "new_section", "new_key", "new_value");
    config_set_value(config, "new_section", "another_key", "another_value");

    fmt_printf("Saving to dynamic_config.ini...\n");
    config_save(config, "sources/dynamic_config.ini");
    fmt_printf("Save operation completed.\n");

    fmt_printf("Removing key 'another_key'...\n");
    config_remove_key(config, "new_section", "another_key");

    fmt_printf("Saving to dynamic_config_modified.ini...\n");
    config_save(config, "sources/dynamic_config_modified.ini");
    fmt_printf("Save operation completed.\n");

    fmt_printf("Reloading configuration...\n");
    config_reload(&config);

    if (!config_has_key(config, "new_section", "another_key")) {
        fmt_printf("Key 'another_key' successfully removed.\n");
    }
    else {
        fmt_printf("Key 'another_key' still exists.\n");
    }

    config_deallocate(config);
    return 0;
}
```

## Example 8 : Handling Multiple Configuration Files 

This example shows how to work with multiple configuration files, such as loading different configurations based on certain conditions and merging them

```c
#include "config/config.h"
#include "fmt/fmt.h"

int main() {
    ConfigFile *config1 = config_create("sources/config.ini");
    ConfigFile *config2 = config_create("sources/dynamic_config.ini");

    if (!config1 || !config2) {
        fmt_printf("Failed to load configurations.\n");
        if (config1) {
            config_deallocate(config1);
        }
        if (config2) {
            config_deallocate(config2);
        }
        return 1;
    }

    // Merge configuration from config2 into config1
    ConfigIterator it = config_get_iterator(config2);
    const char *section, *key, *value;

    while (config_next_entry(&it, &section, &key, &value)) {
        if (!config_has_key(config1, section, key)) {
            config_set_value(config1, section, key, value); // If the key does not exist in config1, add it
        }
    }
    config_save(config1, "sources/merged_config.ini");

    config_deallocate(config1);
    config_deallocate(config2);
    return 0;
}
```