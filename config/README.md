# README for ConfigFile Library

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
#include <stdio.h>

int main() 
{
    // Create configuration from file
    ConfigFile *config = config_create("sources/config.ini");

    if (!config) 
    {
        printf("Failed to load configuration.\n");
        return 1;
    }

    // Get a configuration value
    const char *log_level = config_get_value(config, "global", "log_level");
    if (log_level) 
        printf("Log Level: %s\n", log_level);
    else 
        printf("Failed to get log level.\n");

    // Set a configuration value
    config_set_value(config, "user_preferences", "theme", "light");
    
    // Retrieve updated configuration value
    const char *theme = config_get_value(config, "user_preferences", "theme");
    if (theme) 
        printf("Theme: %s\n", theme);
    else 
        printf("Failed to get theme.\n");
    
    // Save the modified configuration to a new file
    config_save(config, "sources/modified_config.ini");

    // Deallocate the configuration to free memory
    config_deallocate(config);

    return 0;
}
```

### Example 2: Using Comments and Checking for Sections/Keys

```c
#include "config/config.h"
#include <stdio.h>

int main() 
{
    ConfigFile *config = config_create("sources/config.ini");

    if (!config) 
    {
        printf("Failed to load configuration.\n");
        return 1;
    }
    config_set_comment(config, "global", "Global settings for the application"); // Set a comment for a section

    if (config_has_section(config, "user_preferences")) // Check if a section exists
        printf("Section 'user_preferences' exists.\n");
    
    if (config_has_key(config, "user_preferences", "theme"))  // Check if a key exists in a section
        printf("Key 'theme' exists in section 'user_preferences'.\n");
    

    config_save(config, "sources/config_with_comments.ini");
    config_deallocate(config);

    return 0;
}

```

### Example 3: Handling Encrypted Values

In this example, we'll demonstrate how to store and retrieve encrypted configuration values. This is particularly useful for sensitive data like passwords or API keys.

```c
#include "config/config.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    ConfigFile *config = config_create("sources/config.ini"); // Create configuration from file

    if (!config) 
    {
        printf("Failed to load configuration.\n");
        return 1;
    }

    const char *encryption_key = "secret_key"; // Set an encrypted value
    config_set_encrypted_value(config, "advanced", "api_key", "12345", encryption_key);

    // Retrieve and decrypt the value
    char *decrypted_api_key = config_get_encrypted_value(config, "advanced", "api_key", encryption_key);
    if (decrypted_api_key) 
    {
        printf("Decrypted API Key: %s\n", decrypted_api_key);
        free(decrypted_api_key); // Important to free the memory
    } 
    else 
        printf("Failed to decrypt API key.\n");

    config_save(config, "sources/encrypted_config.ini"); // Save the modified configuration
    config_deallocate(config);  // Deallocate the configuration

    return 0;
}

```

### Example 4: Iterating Through Configuration Entries

This example demonstrates how to use the `ConfigIterator` to iterate through all entries in the configuration file.

```c

#include "config/config.h"
#include <stdio.h>

int main() 
{
    ConfigFile *config = config_create("sources/config.ini"); // Create configuration from file

    if (!config) 
    {
        printf("Failed to load configuration.\n");
        return 1;
    }

    ConfigIterator iterator = config_get_iterator(config);  // Initialize the iterator
    const char *section, *key, *value;

    while (config_next_entry(&iterator, &section, &key, &value))  // Iterate through all entries
        printf("Section: %s, Key: %s, Value: %s\n", section, key, value);
    
    config_deallocate(config); // Deallocate the configuration

    return 0;
}

```
