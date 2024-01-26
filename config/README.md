# Config Library in C

**Author:** amin tahmasebi
**Release Date:** 2023
**License:** ISC License

## Overview

The ConfigFile library provides a flexible and easy-to-use solution for handling configuration files in C. It allows reading, modifying, and saving configurations in a standard INI file format. The library supports sections, key-value pairs, and comments, enabling comprehensive management of configuration data.

## Features

- **Read Configuration**: Load configuration data from INI files with support for sections, keys, and comments.
- **Modify Configuration**: Programmatically change configuration settings.
- **Save Configuration**: Write updated configuration back to a file.
- **Memory Management**: Automatic handling of memory allocation and deallocation for configuration data.

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