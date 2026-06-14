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

```bash
gcc -std=c17 -O2 -Wall -Wextra -o main ./main.c ./config/config.c ./file_io/file_reader.c ./file_io/file_writer.c ./fmt/fmt.c ./string/std_string.c ./encoding/encoding.c -I. -lm
```

## Usage

To use the config library in your project, include the `config.h` header file in your source code.

## Function Descriptions

### `ConfigFile *config_create(const char *filename)`
**Purpose**: Creates a new `ConfigFile` structure by loading configuration data from the specified file.
**Parameters**:
  - `filename`: Path to the INI file to be loaded.
**Return Value**: A pointer to the `ConfigFile` structure.
**Usage Case**: Use at program startup to load an INI file into memory before any get/set operations.

---

### `ConfigFile *config_create_empty(void)`
**Purpose**: Creates a new, empty `ConfigFile` in memory with no backing file. The counterpart to `config_create`, which requires an existing file on disk.
**Parameters**: None.
**Return Value**: A pointer to a blank `ConfigFile` (free with `config_deallocate`), or `NULL` on allocation failure.
**Usage Case**: Build a configuration from scratch with the setters / `config_load_string`, then write it with `config_save`. The `filename` field is `NULL`, so `config_reload` does not apply until the config has been saved to a path and re-opened with `config_create`.

---

### `const char *config_get_value(const ConfigFile *config, const char *section, const char *key)`
**Purpose**: Retrieves the value associated with the specified key in the given section.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section name where the key is located.
  - `key`: The key whose value is to be retrieved.
**Return Value**: The value associated with the key, or `NULL` if the key does not exist.
**Usage Case**: Use to read any string value from a loaded configuration by section and key name.

---

### `void config_set_value(ConfigFile *config, const char *section, const char *key, const char *value)`
**Purpose**: Sets or updates the value for a specific key in the given section.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section where the key is located.
  - `key`: The key whose value is to be set or updated.
  - `value`: The new value to be assigned to the key.
**Return Value**: None.
**Usage Case**: Use to programmatically add or update a key-value pair before saving the configuration.

---

### `void config_set_int(ConfigFile *config, const char *section, const char *key, int value)`
**Purpose**: Sets an integer value for a key (typed counterpart to `config_get_int`). Formats `value` with `%d` and stores it via `config_set_value`, creating the section/key if needed.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section where the key is located.
  - `key`: The key whose value is to be set.
  - `value`: The integer value to store.
**Return Value**: None.
**Usage Case**: Store a numeric setting without formatting the string yourself. `NULL` config/section/key are rejected by `config_set_value`.

---

### `void config_set_double(ConfigFile *config, const char *section, const char *key, double value)`
**Purpose**: Sets a double value for a key (typed counterpart to `config_get_double`). Formats `value` with `%.15g` — enough precision that ordinary decimal values round-trip cleanly through `config_get_double` without exposing binary-representation noise.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section where the key is located.
  - `key`: The key whose value is to be set.
  - `value`: The double value to store.
**Return Value**: None.
**Usage Case**: Store a floating-point setting (ratio, threshold, timeout) that reads back faithfully with `config_get_double`.

---

### `void config_set_bool(ConfigFile *config, const char *section, const char *key, bool value)`
**Purpose**: Sets a boolean value for a key (typed counterpart to `config_get_bool`). Stores the canonical literal `"true"` or `"false"`.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section where the key is located.
  - `key`: The key whose value is to be set.
  - `value`: The boolean value to store.
**Return Value**: None.
**Usage Case**: Store a flag that `config_get_bool` recognizes on read-back.

---

### `void config_save(const ConfigFile *config, const char *filename)`
**Purpose**: Saves the current configuration to the specified file.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `filename`: Path to the file where the configuration will be saved.
**Return Value**: None.
**Usage Case**: Use after modifying configuration values to persist changes to disk.

---

### `void config_remove_section(ConfigFile *config, const char *section)`
**Purpose**: Removes an entire section from the configuration.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section to be removed.
**Return Value**: None.
**Usage Case**: Use to drop an obsolete or unwanted section from the in-memory configuration before saving.

---

### `void config_remove_key(ConfigFile *config, const char *section, const char *key)`
**Purpose**: Removes a specific key-value pair from a section in the configuration.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section where the key is located.
  - `key`: The key to be removed.
**Return Value**: None.
**Usage Case**: Use to delete a single setting from a section without affecting the rest of the section.

---

### `void config_deallocate(ConfigFile *config)`
**Purpose**: Frees all memory associated with the configuration structure.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure to be deallocated.
**Return Value**: None.
**Usage Case**: Use when the configuration is no longer needed to release all heap memory it holds.

---

### `bool config_has_section(const ConfigFile *config, const char *section)`
**Purpose**: Checks if a specific section exists in the configuration.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section name to check for existence.
**Return Value**: `true` if the section exists, `false` otherwise.
**Usage Case**: Use to guard against missing sections before attempting to read keys from them.

---

### `bool config_has_key(const ConfigFile *config, const char *section, const char *key)`
**Purpose**: Checks if a specific key exists within a section.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section name where the key should be located.
  - `key`: The key to check for existence.
**Return Value**: `true` if the key exists, `false` otherwise.
**Usage Case**: Use before calling `config_get_value` when a key may be legitimately absent.

---

### `int config_get_int(const ConfigFile *config, const char *section, const char *key, int default_value)`
**Purpose**: Retrieves an integer value for a specific key, or returns a default value if the key is not found.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section name where the key is located.
  - `key`: The key whose integer value is to be retrieved.
  - `default_value`: The default value to return if the key does not exist.
**Return Value**: The integer value associated with the key, or the default value if not found.
**Usage Case**: Use to read numeric settings such as port numbers or retry counts without manual `atoi` calls.

---

### `double config_get_double(const ConfigFile *config, const char *section, const char *key, double default_value)`
**Purpose**: Retrieves a double value for a specific key, or returns a default value if the key is not found.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section name where the key is located.
  - `key`: The key whose double value is to be retrieved.
  - `default_value`: The default value to return if the key does not exist.
**Return Value**: The double value associated with the key, or the default value if not found.
**Usage Case**: Use to read floating-point settings such as ratios or font sizes without manual `strtod` calls.

---

### `bool config_get_bool(const ConfigFile *config, const char *section, const char *key, bool default_value)`
**Purpose**: Retrieves a boolean value for a specific key, or returns a default value if the key is not found.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section name where the key is located.
  - `key`: The key whose boolean value is to be retrieved.
  - `default_value`: The default value to return if the key does not exist.
**Return Value**: The boolean value associated with the key, or the default value if not found.
**Usage Case**: Use to read on/off flags such as feature toggles or logging switches.

---

### `char **config_get_array(const ConfigFile *config, const char *section, const char *key, size_t *array_size)`
**Purpose**: Retrieves an array of strings for a given key in a section.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section name where the key is located.
  - `key`: The key whose value is an array of strings.
  - `array_size`: Pointer to a size_t variable where the size of the array will be stored.
**Return Value**: A dynamically allocated array of strings. The caller is responsible for freeing the memory.
**Usage Case**: Use to read comma-separated lists (e.g., allowed IPs or tags) as individual strings.

---

### `void config_set_array(ConfigFile *config, const char *section, const char *key, const char *const *array, size_t array_size)`
**Purpose**: Sets an array of strings for a given key in a section.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section where the key is located.
  - `key`: The key whose value will be set as an array of strings.
  - `array`: The array of strings to be set.
  - `array_size`: The size of the array.
**Return Value**: None.
**Usage Case**: Use to store a list of values under a single key, serialized as a comma-separated string.

---

### `char *config_get_encrypted_value(const ConfigFile *config, const char *section, const char *key, const char *encryption_key)`
**Purpose**: Retrieves an encrypted value for a specific key in a section, and decrypts it using the provided encryption key.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section name where the key is located.
  - `key`: The key whose value is encrypted.
  - `encryption_key`: The key to decrypt the value.
**Return Value**: The decrypted value as a string. The caller is responsible for freeing the memory.
**Usage Case**: Use to securely retrieve sensitive settings such as passwords or API tokens stored encrypted in the INI file.

---

### `void config_set_encrypted_value(ConfigFile *config, const char *section, const char *key, const char *value, const char *encryption_key)`
**Purpose**: Sets an encrypted value for a specific key in a section.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section where the key is located.
  - `key`: The key whose value will be encrypted.
  - `value`: The value to be encrypted and set.
  - `encryption_key`: The key to encrypt the value.
**Return Value**: None.
**Usage Case**: Use to store sensitive data in the configuration file in an encrypted form.

---

### `void config_set_comment(ConfigFile *config, const char *section, const char *comment)`
**Purpose**: Adds a comment to a specific section in the configuration.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `section`: The section name to which the comment will be added.
  - `comment`: The comment text.
**Return Value**: None.
**Usage Case**: Use to annotate sections with human-readable notes that are preserved when the file is saved.

---

### `ConfigIterator config_get_iterator(const ConfigFile *config)`
**Purpose**: Initializes an iterator for traversing the configuration entries.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
**Return Value**: A `ConfigIterator` structure initialized to the beginning of the configuration.
**Usage Case**: Use to start a full traversal of all key-value pairs across every section.

---

### `bool config_next_entry(ConfigIterator *iterator, const char **section, const char **key, const char **value)`
**Purpose**: Iterates to the next entry in the configuration, returning false if at the end.
**Parameters**:
  - `iterator`: Pointer to the `ConfigIterator` structure.
  - `section`: Pointer to a string that will be set to the current section name.
  - `key`: Pointer to a string that will be set to the current key.
  - `value`: Pointer to a string that will be set to the current value.
**Return Value**: `true` if there are more entries to iterate over, `false` otherwise.
**Usage Case**: Use in a `while` loop to visit every key-value pair without knowing the section names in advance.

---

### `void config_reload(ConfigFile **config_ptr)`
**Purpose**: Reloads the configuration from the file, updating the in-memory representation.
**Parameters**:
  - `config_ptr`: Double pointer to the `ConfigFile` structure to be reloaded.
**Return Value**: None.
**Usage Case**: Use after an external process may have changed the INI file on disk, to refresh the in-memory state.

---

### `void config_register_modification_callback(ConfigFile *config, void (*callback)(const char *section, const char *key, const char *value))`
**Purpose**: Registers a callback function to be called upon modifications to the configuration.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `callback`: The callback function to be registered.
**Return Value**: None.
**Usage Case**: Use to trigger side effects (e.g., logging or live reloading) whenever a key is modified via `config_set_value`.

---

### `void config_validate_structure(const ConfigFile *config, const ConfigSection *expected_structure, size_t structure_size)`
**Purpose**: Validates the structure of the configuration against an expected template.
**Parameters**:
  - `config`: Pointer to the `ConfigFile` structure.
  - `expected_structure`: Pointer to an array of `ConfigSection` structures that define the expected structure.
  - `structure_size`: The number of elements in the `expected_structure` array.
**Return Value**: None.
**Usage Case**: Use at startup to assert that all required sections are present and emit diagnostics for any that are missing.

---

### Extensions

The next seven functions fill common gaps the original API leaves out: counting sections / keys, listing them by name, merging two configs, wiping everything in one shot, and serializing to a heap string without going through disk.

---

### `size_t config_section_count(const ConfigFile *config)`
**Purpose**: Number of sections currently in the config.
**Parameters**:
  - `config`: Source config. May be `NULL`.
**Return Value**: Section count, or `0` if `config` is `NULL`.
**Usage Case**: Use to quickly check whether any sections were loaded after `config_create`, or to size an iteration buffer.

---

### `size_t config_key_count(const ConfigFile *config, const char *section)`
**Purpose**: Number of `key=value` entries in a section (comments excluded).
**Parameters**:
  - `config`: Source config. May be `NULL`.
  - `section`: Section name. May be `NULL`.
**Return Value**: The number of real key-value entries, or `0` if the section is missing or arguments are `NULL`.
**Usage Case**: Use to validate that a required section is non-empty, or to size a buffer before calling `config_list_keys`.

---

### `char** config_list_sections(const ConfigFile *config, size_t *out_count)`
**Purpose**: Snapshot every section name into a freshly-allocated, `NULL`-terminated array.
**Parameters**:
  - `config`: Source config. May be `NULL`.
  - `out_count`: Receives the number of names (excluding the trailing `NULL`). May be `NULL`.
**Return Value**: A `char**` array; the caller frees **every string** AND the array with `free()`. `NULL` on bad input / OOM.
**Usage Case**: Use to enumerate all sections for display, diff, or to drive a generic validation loop.

---

### `char** config_list_keys(const ConfigFile *config, const char *section, size_t *out_count)`
**Purpose**: Snapshot every key in a section. Comments are skipped.
**Parameters**:
  - `config`: Source config. May be `NULL`.
  - `section`: Section name. May be `NULL`.
  - `out_count`: Receives the key count.
**Return Value**: A `NULL`-terminated `char**` array (caller frees each + the array), or `NULL` on bad input / unknown section.
**Usage Case**: Use to iterate all key names in a section without hard-coding them, e.g. to dump a section or to check completeness.

---

### `void config_merge(ConfigFile *dest, const ConfigFile *src)`
**Purpose**: Merge `src` into `dest`. Keys in `src` override matching keys in `dest`; new keys are added; comments and per-section comments from `src` are NOT copied (value merge only).
**Parameters**:
  - `dest`: Destination config (modified). Must NOT be `NULL`.
  - `src`: Source config (untouched). May be `NULL` (no-op).
**Return Value**: None.
**Usage Case**: Use to layer an override config on top of a base config — keys present in `src` take precedence; keys only in `dest` are preserved.

---

### `void config_clear(ConfigFile *config)`
**Purpose**: Drop every section and entry, keeping the `ConfigFile` shell alive. After this returns the file path, modification callback, and default section name are preserved (so `config_save` and `config_reload` still know where to write/read).
**Parameters**:
  - `config`: Config to wipe. May be `NULL` (no-op).
**Return Value**: None.
**Usage Case**: Use to reset a configuration to an empty state without destroying the `ConfigFile` shell, so the filename and callback are retained for subsequent `config_save` or `config_reload` calls.

---

### `char* config_to_string(const ConfigFile *config)`
**Purpose**: Serialize the configuration to a heap-allocated INI-format string. Symmetric counterpart to `config_save` — useful for tests, embedding output in a network response, etc.
**Parameters**:
  - `config`: Source config. May be `NULL`.
**Return Value**: Newly-allocated, NUL-terminated string (caller frees with `free()`), or `NULL` on bad input / OOM. For an empty configuration the result is the empty string `""`.
**Usage Case**: Use to embed the configuration in a network response, write it to a log, or compare in a test without touching the filesystem.

---

### `bool config_load_string(ConfigFile *config, const char *data)`
**Purpose**: Parse INI text from an in-memory string into an existing config — the inverse of `config_to_string`. Reads `data` as INI-format text and appends its sections, keys and comments. Lines split on `\n` (a trailing `\r` from CRLF is tolerated) with **no line-length limit**, unlike the file path which reads in 1024-byte chunks. Parsing rules match `config_create`: `#`/`;` lines become comments (inside a section), `[name]` opens a section (a malformed `[name` is skipped), `key = value` splits on the first `=` with both sides trimmed (the value may itself contain `=`).
**Parameters**:
  - `config`: Target config (e.g. from `config_create_empty`). Must not be `NULL`.
  - `data`: NUL-terminated INI text. Must not be `NULL`.
**Return Value**: `true` on success (including parsing zero entries); `false` on `NULL` input or an allocation failure mid-parse (entries parsed before the failure remain).
**Usage Case**: Load configuration that arrives as a string — from a network response, a database column, an embedded resource, or a secrets manager — without staging it through a temp file. Rows are appended, so repeated calls layer fragments.

---


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
    

    config_save(config, "sources/modified_config.ini");
    config_deallocate(config);
    
    return 0;
}
```
**Result**
```
Log Level: دیباگ
Theme: light
```

---

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
**Result**
```
Section 'user_preferences' exists.
Key 'theme' exists in section 'user_preferences'.
```

---

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
        free(decrypted_api_key); 
    } 
    else {
        fmt_printf("Failed to decrypt API key.\n");
    }

    config_save(config, "sources/encrypted_config.ini"); 
    config_deallocate(config);  

    return 0;
}
```
**Result**
```
Decrypted API Key: 12345
```

---

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
    
    config_deallocate(config); 
    return 0;
}
```
**Result**
```
Section: global, Key: enable_logging, Value: true
Section: global, Key: log_level, Value: دیباگ
Section: global, Key: max_connections, Value: 100
Section: global, Key: type, Value: wifi
Section: global, Key: connected, Value: true
Section: user_preferences, Key: theme, Value: تاریک
Section: user_preferences, Key: font_size, Value: 14
Section: user_preferences, Key: auto_save_interval, Value: 5
Section: user_preferences, Key: timezone, Value: GMT+5
Section: database, Key: host, Value: هاست داخلی
Section: database, Key: port, Value: 3306
Section: database, Key: username, Value: dbuser
Section: database, Key: password, Value: dbpass
Section: database, Key: database_name, Value: testdb
Section: paths, Key: log_directory, Value: /var/log/myapp
Section: paths, Key: temp_directory, Value: /tmp/myapp
Section: paths, Key: backup_directory, Value: /var/backup/myapp
Section: service_endpoints, Key: auth_service, Value: https://auth.example.com
Section: service_endpoints, Key: data_service, Value: https://data.example.com
Section: service_endpoints, Key: metrics_service, Value: https://metrics.example.com
Section: advanced, Key: complexity, Value: high
Section: advanced, Key: experimental_features, Value: yes
Section: advanced, Key: retry_attempts, Value: 5
Section: advanced, Key: timeout_seconds, Value: 30
```

---

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
    config_set_array(config, "network", "server_ips", server_ips, 3); 

    // Retrieve the array of values
    size_t array_size;
    char **retrieved_ips = config_get_array(config, "network", "server_ips", &array_size); // get from buffer 

    if (retrieved_ips) {
        fmt_printf("Server IPs:\n");
        for (size_t i = 0; i < array_size; ++i) {
            fmt_printf("- %s\n", retrieved_ips[i]);
            free(retrieved_ips[i]); 
        }
        free(retrieved_ips); 
    }

    config_save(config, "sources/modified_config.ini");
    config_deallocate(config);

    return 0;
}
```
**Result**
```
Server IPs:
- 192.168.1.1
- 192.168.1.2
- 192.168.1.3
```

---

## Example 6: Validating Configuration Structure
This example shows how to validate the configuration structure against an expected template, ensuring that required sections are present.

```c
#include "config/config.h"
#include "fmt/fmt.h"

int main() {
    ConfigFile *config = config_create("sources/config.ini");

    // Expected structure
    const ConfigSection expected_structure[] = {
        {(char*)"global", NULL, 0, NULL},
        {(char*)"user_preferences", NULL, 0, NULL},
        {(char*)"network", NULL, 0, NULL}
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
**Result**
```
Section 'network' is missing in the configuration.
```

---

## Example 7 : Dynamic Configuration Management 

This example demonstrates how to dynamically manage a configuration by adding and removing sections and keys, and reloading the configuration from the file.

```c
#include "config/config.h"
#include "fmt/fmt.h"

int main() {
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
**Result**
```
Adding new section and keys...
Saving to dynamic_config.ini...
Save operation completed.
Removing key 'another_key'...
Saving to dynamic_config_modified.ini...
Save operation completed.
Reloading configuration...
Key 'another_key' successfully removed.
```

---

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
    fmt_printf("Configs Merged Successfully\n");

    config_deallocate(config1);
    config_deallocate(config2);

    return 0;
}
```
**Result**
```
Configs Merged Successfully
```

---

## Example 9: Typed Getters — `config_get_int`, `config_get_double`, `config_get_bool`

This example demonstrates how to retrieve typed values from a configuration file.

```c
#include "config/config.h"
#include "fmt/fmt.h"

int main() {
    ConfigFile *config = config_create("sources/config.ini");

    int port = config_get_int(config, "database", "port", 0);
    double font_size = config_get_double(config, "user_preferences", "font_size", 0.0);
    bool logging = config_get_bool(config, "global", "enable_logging", false);

    fmt_printf("Database port: %d\n", port);
    fmt_printf("Font size: %.1f\n", font_size);
    fmt_printf("Logging enabled: %s\n", logging ? "true" : "false");

    config_deallocate(config);
    return 0;
}
```

**Result**
```
Database port: 3306
Font size: 14.0
Logging enabled: true
```

---

### Example 10 — `config_section_count`

```c
#include "config/config.h"
#include "fmt/fmt.h"

int main(void) {
    ConfigFile* c = config_create("sources/settings.ini");
    fmt_printf("loaded %zu section(s)\n", config_section_count(c));

    config_deallocate(c);
    return 0;
}
```

**Result** (for an INI with three `[section]` blocks):
```
loaded 3 section(s)
```

---

### Example 11 — `config_key_count`

```c
#include "config/config.h"
#include "fmt/fmt.h"

int main(void) {
    ConfigFile* c = config_create("sources/settings.ini");

    fmt_printf("server keys: %zu\n", config_key_count(c, "server"));
    fmt_printf("features keys: %zu\n", config_key_count(c, "features"));
    fmt_printf("missing keys: %zu\n", config_key_count(c, "no-such"));

    config_deallocate(c);
    return 0;
}
```

**Result**
```
server keys: 4
features keys: 2
missing keys: 0
```

---

### Example 12 — `config_list_sections`

```c
#include "config/config.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    ConfigFile* c = config_create("sources/settings.ini");
    size_t n = 0;
    char** secs = config_list_sections(c, &n);

    for (size_t i = 0; i < n; ++i) {
        fmt_printf("[%zu] %s\n", i, secs[i]);
        free(secs[i]);          /* caller frees each string */
    }

    free(secs);                 /* and the array itself */
    config_deallocate(c);

    return 0;
}
```

**Result**
```
[0] server
[1] features
[2] empty
```

---

### Example 13 — `config_list_keys`

```c
#include "config/config.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    ConfigFile* c = config_create("sources/settings.ini");
    size_t n = 0;
    char** keys = config_list_keys(c, "server", &n);

    fmt_printf("server has %zu keys:\n", n);
    for (size_t i = 0; i < n; ++i) {
        fmt_printf("  %s = %s\n", keys[i], config_get_value(c, "server", keys[i]));
        free(keys[i]);
    }

    free(keys);
    config_deallocate(c);

    return 0;
}
```

**Result**
```
server has 4 keys:
  host = localhost
  port = 8080
  tls = true
  ratio = 0.75
```

---

### Example 14 — `config_merge`

Layer overrides on top of defaults:

```c
#include "config/config.h"
#include "fmt/fmt.h"

int main(void) {
    ConfigFile* base    = config_create("sources/defaults.ini");
    ConfigFile* prefer  = config_create("sources/override.ini");

    config_merge(base, prefer);  /* prefer's values win on conflict */

    fmt_printf("host = %s\n", config_get_value(base, "server", "host"));
    fmt_printf("port = %s\n", config_get_value(base, "server", "port"));

    config_deallocate(base);
    config_deallocate(prefer);

    return 0;
}
```

**Result** (if `sources/defaults.ini` has `host=local port=80` and `sources/override.ini` has `host=remote`):
```
host = remote
port = 80
```

---

### Example 15 — `config_clear`

```c
#include "config/config.h"
#include "fmt/fmt.h"

int main(void) {
    ConfigFile* c = config_create("sources/settings.ini");
    fmt_printf("before clear: %zu sections\n", config_section_count(c));
    config_clear(c);
    fmt_printf("after clear:  %zu sections\n", config_section_count(c));
    /* Shell still usable for fresh writes. */
    config_set_value(c, "new", "k", "v");
    fmt_printf("after fresh write: %zu sections\n", config_section_count(c));

    config_deallocate(c);
    return 0;
}
```

**Result**
```
before clear: 3 sections
after clear:  0 sections
after fresh write: 1 sections
```

---

### Example 16 — `config_to_string`

In-memory serialize without touching disk:

```c
#include "config/config.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    ConfigFile* c = config_create("sources/settings.ini");
    char* s = config_to_string(c);
    fmt_printf("--- serialized ---\n%s--- end ---\n", s);

    free(s);
    config_deallocate(c);
    
    return 0;
}
```

**Result**
```
--- serialized ---
[server]
host=localhost
port=8080
tls=true
ratio=0.75

[features]
tags=alpha, beta, gamma
enabled=true

[empty]

--- end ---
```

---

## Example 17 — In-memory config: `config_create_empty`, typed setters, and `config_load_string`

Build a configuration without any file on disk, serialize it, then parse it straight back from the string. `config_create_empty` gives a blank config; the typed setters (`config_set_int`/`config_set_double`/`config_set_bool`) store values without manual formatting; `config_load_string` is the inverse of `config_to_string`.

```c
#include "config/config.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    /* Build a configuration entirely in memory — no file on disk needed. */
    ConfigFile* cfg = config_create_empty();

    config_set_value(cfg, "server", "host", "localhost");
    config_set_int(cfg, "server", "port", 8080);
    config_set_bool(cfg, "server", "tls",  true);
    config_set_double(cfg, "limits", "cpu_ratio", 0.75);

    /* Serialize it to an INI string. */
    char* text = config_to_string(cfg);
    fmt_printf("Serialized config:\n%s", text);

    /* Parse that string back into a fresh config (the inverse of to_string). */
    ConfigFile* loaded = config_create_empty();
    config_load_string(loaded, text);
    free(text);

    /* Read the values back with the typed getters. */
    fmt_printf("Parsed back:\n");
    fmt_printf("  host = %s\n", config_get_value(loaded, "server", "host"));
    fmt_printf("  port = %d\n", config_get_int(loaded, "server", "port", 0));
    fmt_printf("  tls  = %s\n", config_get_bool(loaded, "server", "tls", false) ? "on" : "off");
    fmt_printf("  cpu  = %.2f\n", config_get_double(loaded, "limits", "cpu_ratio", 0.0));

    config_deallocate(cfg);
    config_deallocate(loaded);
    return 0;
}
```

**Result**
```
Serialized config:
[server]
host=localhost
port=8080
tls=true

[limits]
cpu_ratio=0.75

Parsed back:
  host = localhost
  port = 8080
  tls  = on
  cpu  = 0.75
```

---

## License

This project is open-source and available under [ISC License].
