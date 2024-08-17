#include <string.h>
#include "config.h"
#include "../string/string.h"
#include "../fmt/fmt.h"
#include <stdlib.h>
#include <ctype.h>

static char *trim_whitespace(char *str) {
    char *end;
    // Trim leading space
    while (isspace((unsigned char)*str)) {  
        str++;
    }
    // All spaces?
    if (*str == 0) {  
        return str;
    }

    end = str + strlen(str) - 1;  // Trim trailing space
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    end[1] = '\0'; // Write new null terminator character

    return str;
}

static void xor_encrypt_decrypt(const char *input, char *output, char key, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        output[i] = input[i] ^ key;
    }
}

/**
 * @brief Creates a ConfigFile structure by reading the specified configuration file.
 *
 * This function opens a configuration file, reads its content, and parses it into sections and key-value pairs.
 * The configuration is stored in a ConfigFile structure, which can be used for further operations like getting or setting values.
 *
 * @param filename The name of the configuration file to be read.
 * @return A pointer to the ConfigFile structure containing the parsed configuration data.
 *         Returns NULL if the file could not be opened or memory allocation failed.
 */
ConfigFile *config_create(const char *filename) {
    FileReader* fr = file_reader_open(filename, READ_TEXT);
    if (!fr) {
        fmt_fprintf(stderr, "Error: Unable to open file '%s' for reading.\n", filename);
        exit(-1);
    }

    ConfigFile *config = malloc(sizeof(ConfigFile));
    if (!config) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for ConfigFile.\n");
        file_reader_close(fr);
        exit(-1);
    }

    config->sections = NULL;
    config->section_count = 0;
    config->default_section = NULL;
    config->filename = string_strdup(filename);

    char line[1024];
    ConfigSection *current_section = NULL;

    while (file_reader_read_line(line, sizeof(line), fr)){
        String* str = string_create(line);
        string_trim(str);
        const char *trimmed = string_c_str(str);

        char *writable_trimmed = string_strdup(trimmed);
        string_deallocate(str);

        ConfigEntry entry = {0};
        entry.isComment = false;
        entry.key = NULL;
        entry.value = NULL;

        if (writable_trimmed[0] == '#' || writable_trimmed[0] == ';') {
            entry.isComment = true;
            entry.value = string_strdup(writable_trimmed);
        }
        if (writable_trimmed[0] == '[') {
            current_section = malloc(sizeof(ConfigSection));
            size_t section_name_length = strlen(writable_trimmed) - 2; // Exclude '[' and ']'
            current_section->section_name = malloc(section_name_length + 1); // +1 for null terminator
            strncpy(current_section->section_name, writable_trimmed + 1, section_name_length); // Skip '['
            current_section->section_name[section_name_length] = '\0'; // Null terminate
            current_section->entries = NULL;
            current_section->entry_count = 0;

            config->sections = realloc(config->sections, (config->section_count + 1) * sizeof(ConfigSection *));
            config->sections[config->section_count++] = current_section;
        } 
        else if (current_section && strchr(writable_trimmed, '=')) {
            char *key = strtok(writable_trimmed, "=");
            char *value = strtok(NULL, "");
            entry.key = string_strdup(trim_whitespace(key));
            entry.value = string_strdup(trim_whitespace(value));
        }
        if (current_section && (entry.isComment || entry.key)) {
            current_section->entries = realloc(current_section->entries, (current_section->entry_count + 1) * sizeof(ConfigEntry));
            current_section->entries[current_section->entry_count++] = entry;
        }

        free(writable_trimmed);
    }

    file_reader_close(fr);
    return config;
}

/**
 * @brief Saves the configuration data to the specified file.
 *
 * This function writes the contents of the ConfigFile structure back to a file, preserving sections, key-value pairs, 
 * and comments. The configuration is saved in a format compatible with standard INI files.
 *
 * @param config Pointer to the ConfigFile structure containing the configuration data.
 * @param filename The name of the file where the configuration should be saved.
 */
void config_save(const ConfigFile *config, const char *filename) {
    FileWriter* fw = file_writer_open(filename, WRITE_TEXT);
    if (!fw) {
        fmt_fprintf(stderr, "Error: Unable to open file '%s' for writing.\n", filename);
        return;
    }

    for (size_t i = 0; i < config->section_count; i++) {
        ConfigSection *section = config->sections[i];
        
        file_writer_write_fmt(fw, "[%s]\n", section->section_name);

        for (size_t j = 0; j < section->entry_count; j++) {
            ConfigEntry *entry = &section->entries[j];
            if (entry->isComment) {
                file_writer_write_fmt(fw, "%s\n", entry->value);
            }
            else if (entry->key && entry->value) {
                file_writer_write_fmt(fw, "%s=%s\n", entry->key, entry->value);
            }
        }
        file_writer_write_fmt(fw, "\n");
    }
    file_writer_close(fw);
}

/**
 * @brief Retrieves the value for a given key in a specified section.
 *
 * This function searches the specified section in the configuration file for the given key.
 * If the key is found, the corresponding value is returned. If the section or key is not found, 
 * NULL is returned.
 *
 * @param config Pointer to the ConfigFile structure.
 * @param section The section name where the key is located.
 * @param key The key whose value is to be retrieved.
 * @return The value associated with the key, or NULL if the section or key is not found.
 */
const char *config_get_value(const ConfigFile *config, const char *section, const char *key) {
    if (!config) {
        fmt_fprintf(stderr, "Error: ConfigFile pointer is NULL in config_get_value.\n");
        return NULL;
    }
    if (!section) {
        fmt_fprintf(stderr, "Error: Section name is NULL in config_get_value.\n");
        return NULL;
    }
    if (!key) {
        fmt_fprintf(stderr, "Error: Key name is NULL in config_get_value.\n");
        return NULL;
    }

    for (size_t i = 0; i < config->section_count; ++i) {
        if (strcmp(config->sections[i]->section_name, section) == 0) {
            ConfigSection *sec = config->sections[i];
            for (size_t j = 0; j < sec->entry_count; ++j) {
                if (sec->entries[j].key && strcmp(sec->entries[j].key, key) == 0) { 
                    return sec->entries[j].value;
                }
            }
            
            break;
        }
    }
    return NULL;
}

/**
 * @brief Sets the value for a given key in a specified section.
 *
 * This function sets the value for the specified key in the given section. If the section or key
 * does not exist, they are created. If the key already exists, its value is updated.
 *
 * @param config Pointer to the ConfigFile structure.
 * @param section The section name where the key-value pair should be set.
 * @param key The key for which the value is to be set.
 * @param value The value to be set for the specified key.
 */
void config_set_value(ConfigFile *config, const char *section, const char *key, const char *value) {
    if (!config) {
        fmt_fprintf(stderr, "Error: ConfigFile pointer is NULL in config_set_value.\n");
        return;
    }
    if (!section) {
        fmt_fprintf(stderr, "Error: Section name is NULL in config_set_value.\n");
        return;
    }
    if (!key) {
        fmt_fprintf(stderr, "Error: Key name is NULL in config_set_value.\n");
        return;
    }
    if (!value) {
        fmt_fprintf(stderr, "Error: Value is NULL in config_set_value.\n");
        return;
    }

    // Find or create the section
    ConfigSection *sec = NULL;
    for (size_t i = 0; i < config->section_count; ++i) {
        if (strcmp(config->sections[i]->section_name, section) == 0) {
            sec = config->sections[i];
            break;
        }
    }

    if (!sec) {
        sec = malloc(sizeof(ConfigSection));
        if (!sec) {
            fmt_fprintf(stderr, "Error: Memory allocation failed for new section in config_set_value.\n");
            return;
        }
        sec->section_name = string_strdup(section);
        sec->entries = NULL;
        sec->entry_count = 0;

        config->sections = realloc(config->sections, (config->section_count + 1) * sizeof(ConfigSection *));
        if (!config->sections) {
            free(sec->section_name);
            free(sec);
            fmt_fprintf(stderr, "Error: Memory allocation failed for sections array in config_set_value.\n");
            return;
        }
        config->sections[config->section_count++] = sec;
    }

    // Update existing key or add new key-value pair
    for (size_t j = 0; j < sec->entry_count; ++j) {
        if (sec->entries[j].key && strcmp(sec->entries[j].key, key) == 0) {
            free(sec->entries[j].value);
            sec->entries[j].key = string_strdup(key);
            sec->entries[j].value = string_strdup(value);

            return;
        }
    }

    sec->entries = realloc(sec->entries, (sec->entry_count + 1) * sizeof(ConfigEntry));
    if (!sec->entries) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for new entry in config_set_value.\n");
        return;
    }

    sec->entries[sec->entry_count].key = string_strdup(key); // Allocate and set key
    sec->entries[sec->entry_count].value = string_strdup(value); // Allocate and set value
    sec->entry_count++;
}

/**
 * @brief Removes an entire section from the configuration.
 *
 * This function removes the specified section from the configuration file. All keys and values within
 * the section are deleted. If the section is not found, a warning is printed.
 *
 * @param config Pointer to the ConfigFile structure.
 * @param section The name of the section to be removed.
 */
void config_remove_section(ConfigFile *config, const char *section) {
    if (!config || !section) {
        fmt_fprintf(stderr, "Error: Invalid arguments provided to config_remove_section.\n");
        return;
    }

    bool section_found = false;
    for (size_t i = 0; i < config->section_count; ++i) {
        if (strcmp(config->sections[i]->section_name, section) == 0) {
            section_found = true;
            free(config->sections[i]->section_name); // Free the section
            for (size_t j = 0; j < config->sections[i]->entry_count; ++j) {
                free(config->sections[i]->entries[j].key);
                free(config->sections[i]->entries[j].value);
            }

            free(config->sections[i]->entries);
            free(config->sections[i]);
            
            // Shift remaining sections
            for (size_t k = i; k < config->section_count - 1; ++k) { 
                config->sections[k] = config->sections[k + 1];
            }
            config->section_count--;
            config->sections = realloc(config->sections, config->section_count * sizeof(ConfigSection *)); // Resize the sections array
            
            return;
        }
    }
    if (!section_found) {
        fmt_fprintf(stderr, "Warning: Section '%s' not found in config_remove_section.\n", section);
    }
}

/**
 * @brief Removes a specific key-value pair from a section in the configuration.
 *
 * This function removes the specified key-value pair from the given section in the configuration. 
 * If the key is found, it is deleted, and the remaining entries in the section are adjusted accordingly.
 *
 * @param config Pointer to the ConfigFile structure.
 * @param section The section name from which the key-value pair should be removed.
 * @param key The key to be removed from the section.
 */
void config_remove_key(ConfigFile *config, const char *section, const char *key) {
    if (!config || !section || !key) {
        fmt_fprintf(stderr, "Error: Invalid arguments provided to config_remove_key.\n");
        return;
    }

    bool key_found = false;
    for (size_t i = 0; i < config->section_count; ++i) {
        if (strcmp(config->sections[i]->section_name, section) == 0) {
            ConfigSection *sec = config->sections[i];

            for (size_t j = 0; j < sec->entry_count; ++j) {
                if (strcmp(sec->entries[j].key, key) == 0) {
                    key_found = true;
                    free(sec->entries[j].key);
                    free(sec->entries[j].value);

                    for (size_t k = j; k < sec->entry_count - 1; ++k) { 
                        sec->entries[k] = sec->entries[k + 1];
                    }
                    sec->entry_count--;
                    sec->entries = realloc(sec->entries, sec->entry_count * sizeof(ConfigEntry));
                    return;
                }
            }
            break;
        }
    }
    if (!key_found) {
        fmt_fprintf(stderr, "Warning: Key '%s' in section '%s' not found in config_remove_key.\n", key, section);
    }
}

/**
 * @brief Frees all memory associated with the configuration structure.
 *
 * This function deallocates all memory used by the ConfigFile structure, including sections, keys, values, and comments.
 * After calling this function, the ConfigFile structure should no longer be used.
 *
 * @param config Pointer to the ConfigFile structure to be deallocated.
 */
void config_deallocate(ConfigFile *config) {
    if (!config) {
        fmt_fprintf(stderr, "Error: ConfigFile pointer is NULL in config_deallocate.\n");
        return;
    }

    for (size_t i = 0; i < config->section_count; ++i) {
        free(config->sections[i]->section_name);
        for (size_t j = 0; j < config->sections[i]->entry_count; ++j) {
            free(config->sections[i]->entries[j].key);
            free(config->sections[i]->entries[j].value);
        }

        free(config->sections[i]->entries);
        free(config->sections[i]);
    }
    free(config->sections);
    free(config->default_section);
    free(config);
}

/**
 * @brief Checks if a specific section exists in the configuration.
 *
 * This function checks whether a given section exists in the configuration file.
 *
 * @param config Pointer to the ConfigFile structure.
 * @param section The name of the section to check for.
 * @return True if the section exists, false otherwise.
 */
bool config_has_section(const ConfigFile *config, const char *section) {
    if (!config || !section) {
        fmt_fprintf(stderr, "Error: Invalid arguments provided to config_has_section.\n");
        return false;
    }

    for (size_t i = 0; i < config->section_count; ++i) {
        if (strcmp(config->sections[i]->section_name, section) == 0) { 
            return true;
        }
    }
    return false;
}

/**
 * @brief Checks if a specific key exists within a section.
 *
 * This function checks whether a given key exists in the specified section of the configuration file.
 *
 * @param config Pointer to the ConfigFile structure.
 * @param section The name of the section to check within.
 * @param key The name of the key to check for.
 * 
 * @return True if the key exists within the section, false otherwise.
 */
bool config_has_key(const ConfigFile *config, const char *section, const char *key) {
    if (!config || !section || !key) {
        fmt_fprintf(stderr, "Error: Invalid arguments provided to config_has_key.\n");
        return false;
    }

    for (size_t i = 0; i < config->section_count; ++i) {
        if (strcmp(config->sections[i]->section_name, section) == 0) {
            ConfigSection *sec = config->sections[i];
            for (size_t j = 0; j < sec->entry_count; ++j) {
                if (strcmp(config->sections[i]->section_name, section) == 0) {
                    ConfigSection *sec = config->sections[i];
                    for (size_t j = 0; j < sec->entry_count; ++j) {   
                        if (sec->entries[j].key && strcmp(sec->entries[j].key, key) == 0) { 
                            return true;
                        }
                    }
                    break;
                }
            }
            break;
        }
    }
    return false;
}

/**
 * @brief Retrieves an integer value for a given key in a specified section, or a default value if the key is not found.
 *
 * This function attempts to retrieve and convert the value associated with the given key in the specified section
 * to an integer. If the key is not found or the value cannot be converted to an integer, the specified default value is returned.
 *
 * @param config Pointer to the ConfigFile structure.
 * @param section The section name where the key is located.
 * @param key The key whose value will be retrieved and converted to an integer.
 * @param default_value The default integer value to return if the key is not found or the value is invalid.
 * 
 * @return The integer value associated with the key, or the default value if the key is not found or invalid.
 */
int config_get_int(const ConfigFile *config, const char *section, const char *key, int default_value) {
    if (!config || !section || !key) {
        fmt_fprintf(stderr, "Error: Invalid arguments provided to config_get_int.\n");
        return default_value;
    }

    const char *value = config_get_value(config, section, key);
    if (value) {
        char *end;
        long int_val = strtol(value, &end, 10);

        if (end != value && *end == '\0') {
            return (int)int_val; // Return the parsed integer
        }
    }
    return default_value; // Return the default value if not found or not an integer
}

/**
 * @brief Retrieves a double value for a given key in a specified section, or a default value if the key is not found.
 *
 * This function attempts to retrieve and convert the value associated with the given key in the specified section
 * to a double. If the key is not found or the value cannot be converted to a double, the specified default value is returned.
 *
 * @param config Pointer to the ConfigFile structure.
 * @param section The section name where the key is located.
 * @param key The key whose value will be retrieved and converted to a double.
 * @param default_value The default double value to return if the key is not found or the value is invalid.
 * 
 * @return The double value associated with the key, or the default value if the key is not found or invalid.
 */
double config_get_double(const ConfigFile *config, const char *section, const char *key, double default_value) {
    if (!config || !section || !key) {
        fmt_fprintf(stderr, "Error: Invalid arguments provided to config_get_double.\n");
        return default_value;
    }

    const char *value = config_get_value(config, section, key);
    if (value) {
        char *end;
        double double_val = strtod(value, &end);

        if (end != value && *end == '\0') { 
            return double_val; // Return the parsed double
        }
    }
    return default_value; // Return the default value if not found or not a double
}

/**
 * @brief Retrieves a boolean value for a given key in a specified section, or a default value if the key is not found.
 *
 * This function attempts to retrieve the value associated with the given key in the specified section
 * and interpret it as a boolean. Accepted values for true are "true", "yes", and "1". Accepted values for false are "false", "no", and "0".
 * If the key is not found or the value does not match any recognized boolean value, the specified default value is returned.
 *
 * @param config Pointer to the ConfigFile structure.
 * @param section The section name where the key is located.
 * @param key The key whose value will be retrieved and interpreted as a boolean.
 * @param default_value The default boolean value to return if the key is not found or the value is invalid.
 * 
 * @return The boolean value associated with the key, or the default value if the key is not found or invalid.
 */
bool config_get_bool(const ConfigFile *config, const char *section, const char *key, bool default_value) {
    if (!config || !section || !key) {
        fmt_fprintf(stderr, "Error: Invalid arguments provided to config_get_bool.\n");
        return default_value;
    }

    const char *value = config_get_value(config, section, key);

    if (value) {
        if (strcasecmp(value, "true") == 0 || strcasecmp(value, "yes") == 0 || strcmp(value, "1") == 0) { 
            return true;
        }
        else if (strcasecmp(value, "false") == 0 || strcasecmp(value, "no") == 0 || strcmp(value, "0") == 0) { 
            return false;
        }
    }
    return default_value; // Return the default value if not found or not a recognized boolean
}

/**
 * @brief Adds a comment to a specific section in the configuration.
 *
 * This function allows you to add a comment to a specified section in the configuration. 
 * The comment will be stored with the section and can be saved when the configuration is written to a file.
 *
 * @param config Pointer to the ConfigFile structure.
 * @param section The section name where the comment will be added.
 * @param comment The comment to be added to the section.
 */
void config_set_comment(ConfigFile *config, const char *section, const char *comment) {
    if (!config || !section || !comment) {
        fmt_fprintf(stderr, "Error: Invalid arguments provided to config_set_comment.\n");
        return;
    }
    for (size_t i = 0; i < config->section_count; ++i) {
        ConfigSection *sec = config->sections[i];
        if (strcmp(config->sections[i]->section_name, section) == 0) { 
            sec->comment = string_strdup(comment); // Allocate memory for and store the new comment 
        }
        else {
            sec->comment = NULL;
        }
    }
    fmt_fprintf(stderr, "Warning: Section '%s' not found in config_set_comment.\n", section);
}

/**
 * @brief Initializes an iterator for traversing the configuration entries.
 *
 * This function initializes a ConfigIterator for traversing the entries in the configuration file. 
 * The iterator is used to sequentially access each key-value pair in each section of the configuration.
 *
 * @param config Pointer to the ConfigFile structure.
 * @return A ConfigIterator initialized to the first entry in the configuration.
 */
ConfigIterator config_get_iterator(const ConfigFile *config) {
    ConfigIterator iterator = {0};

    if (!config) {
        fmt_fprintf(stderr, "Error: ConfigFile is NULL in config_get_iterator.\n");
        return iterator; // Return an empty iterator
    }
    iterator.config = config;
    iterator.section_index = 0;
    iterator.entry_index = 0;
    
    return iterator;
}

/**
 * @brief Iterates to the next entry in the configuration.
 *
 * This function advances the iterator to the next entry (key-value pair) in the configuration. 
 * It returns false if there are no more entries to iterate over, indicating the end of the configuration.
 *
 * @param iterator Pointer to the ConfigIterator structure.
 * @param section Pointer to a string where the section name will be stored.
 * @param key Pointer to a string where the key name will be stored.
 * @param value Pointer to a string where the value will be stored.
 * 
 * @return True if the next entry was successfully retrieved, false if the end of the configuration is reached.
 */
bool config_next_entry(ConfigIterator *iterator, const char **section, const char **key, const char **value) {
    if (!iterator || !iterator->config || iterator->section_index >= iterator->config->section_count) {
        fmt_fprintf(stderr, "Error: Invalid iterator or config in config_next_entry.\n");
        return false;
    }

    while (iterator->section_index < iterator->config->section_count) {
        ConfigSection *sec = iterator->config->sections[iterator->section_index];

        if (iterator->entry_index < sec->entry_count) {
            *section = sec->section_name;
            *key = sec->entries[iterator->entry_index].key;
            *value = sec->entries[iterator->entry_index].value;
            iterator->entry_index++;

            return true;
        } 
        else {
            iterator->section_index++;
            iterator->entry_index = 0;
        }
    }
    return false;
}

/**
 * @brief Reloads the configuration from the file, updating the in-memory representation.
 *
 * This function reloads the configuration from the original file, discarding any changes that have been made in memory.
 * The in-memory configuration is replaced with the newly loaded data.
 *
 * @param config_ptr Pointer to the pointer of the ConfigFile structure. The structure will be updated with the new data.
 */
void config_reload(ConfigFile **config_ptr) {
    if (!config_ptr || !(*config_ptr) || !(*config_ptr)->filename) {
        fmt_fprintf(stderr, "Error: Invalid arguments provided to config_reload.\n");
        return;
    }

    ConfigFile *new_config = config_create((*config_ptr)->filename);
    if (!new_config) {
        fmt_fprintf(stderr, "Error: Failed to reload configuration from file '%s'.\n", (*config_ptr)->filename);
        return;
    }

    config_deallocate(*config_ptr); // Free the old configuration
    *config_ptr = new_config; // Update to the new configuration
}

/**
 * @brief Registers a callback function to be called whenever a modification is made to the configuration.
 *
 * This function allows you to register a callback that will be triggered whenever a key-value pair
 * in the configuration is modified. The callback will receive the section, key, and value of the modification.
 *
 * @param config Pointer to the ConfigFile structure.
 * @param callback Function pointer to the callback function that will handle the modification events.
 */
void config_register_modification_callback(ConfigFile *config, void (*callback)(const char *section, const char *key, const char *value)) {
    if (!config) {
        fmt_fprintf(stderr, "Error: ConfigFile is NULL in config_register_modification_callback.\n");
        return;
    }
    config->modification_callback = callback;
}


/**
 * @brief Validates the structure of the configuration against an expected template.
 *
 * This function checks the configuration to ensure it conforms to a specified structure template.
 * It validates the presence of required sections and optionally their keys. If any required section
 * is missing, a warning is printed.
 *
 * @param config Pointer to the ConfigFile structure.
 * @param expected_structure Pointer to an array of ConfigSection structures representing the expected configuration structure.
 * @param structure_size The number of elements in the expected_structure array.
 */
void config_validate_structure(const ConfigFile *config, const ConfigSection *expected_structure, size_t structure_size) {
    if (!config || !expected_structure) {
        fmt_fprintf(stderr, "Error: Invalid arguments provided to config_validate_structure.\n");
        return;
    }

    for (size_t i = 0; i < structure_size; ++i) {
        const ConfigSection *expected_sec = &expected_structure[i];
        bool section_found = false;

        for (size_t j = 0; j < config->section_count; ++j) {
            if (strcmp(config->sections[j]->section_name, expected_sec->section_name) == 0) {
                section_found = true;  // Further validation can be added here to check for specific keys and values
                break;
            }
        }

        if (!section_found) {
            fmt_printf("Section '%s' is missing in the configuration.\n", expected_sec->section_name);
        }
    }
}

/**
 * @brief Retrieves an array of strings for a given key in a specified section.
 *
 * This function retrieves the value associated with the given key in the specified section,
 * splits the value by commas into an array of strings, and returns the array.
 *
 * @param config Pointer to the ConfigFile structure.
 * @param section The section name where the key is located.
 * @param key The key whose value is to be retrieved and split into an array.
 * @param array_size Pointer to a size_t variable where the number of elements in the array will be stored.
 * 
 * @return A pointer to an array of strings. The caller is responsible for freeing this memory. 
 * Returns NULL if the key does not exist or if there is an error.
 */
char **config_get_array(const ConfigFile *config, const char *section, const char *key, size_t *array_size) {
    if (!config || !section || !key || !array_size) {
        fmt_fprintf(stderr, "Error: Invalid arguments provided to config_get_array.\n");
        *array_size = 0;
        return NULL;
    }

    const char *value = config_get_value(config, section, key);
    if (!value) {
        *array_size = 0;
        return NULL;
    }

    *array_size = 1;  // Count the number of elements
    for (const char *p = value; *p; ++p) 
        if (*p == ',') (*array_size)++;
    
    char **array = malloc(*array_size * sizeof(char *)); // Allocate array of string pointers
    if (!array) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in config_get_array.\n");
        *array_size = 0;
        return NULL;
    }

    // Split the string into array
    char *value_copy = string_strdup(value);
    char *token = strtok(value_copy, ", ");
    size_t idx = 0;

    while (token) 
    {
        array[idx++] = string_strdup(token);
        token = strtok(NULL, ",");
    }
    free(value_copy);

    return array;
}

/**
 * @brief Sets an array of strings for a given key in a specified section.
 *
 * This function combines an array of strings into a single comma-separated string and stores it as the value
 * for the given key in the specified section.
 *
 * @param config Pointer to the ConfigFile structure.
 * @param section The section name where the key is located.
 * @param key The key whose value will be set to the combined array string.
 * @param array Pointer to an array of strings to be combined and stored.
 * @param array_size The number of elements in the array.
 */
void config_set_array(ConfigFile *config, const char *section, const char *key, const char *const *array, size_t array_size) {
    if (!config || !section || !key || !array || array_size == 0) {
        fmt_fprintf(stderr, "Error: Invalid arguments provided to config_set_array.\n");
        return;
    }

    size_t total_length = strlen(key) + 2; // +2 for '=' and '\0'
    for (size_t i = 0; i < array_size; i++) 
        total_length += strlen(array[i]) + ((i < array_size - 1) ? 1 : 0); // +1 for comma, if not the last element
    
    char *combined = malloc(total_length);
    if (!combined) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in config_set_array.\n");
        return;
    }

    char *ptr = combined;
    ptr += sprintf(ptr, "%s=", key);
    for (size_t i = 0; i < array_size; i++) 
        ptr += sprintf(ptr, "%s%s", array[i], (i < array_size - 1) ? ", " : "");

    config_set_value(config, section, key, combined);
    free(combined);
}

/**
 * @brief Retrieves an encrypted value for a given key in a section and decrypts it using the provided encryption key.
 *
 * This function looks up the specified key within the specified section of the configuration file.
 * The stored value is expected to be encrypted using a simple XOR encryption, and it will be decrypted
 * using the provided encryption key.
 *
 * @param config Pointer to the ConfigFile structure.
 * @param section The section name where the key is located.
 * @param key The key whose encrypted value is to be retrieved.
 * @param encryption_key The key used to decrypt the value.
 * 
 * @return A pointer to the decrypted value as a dynamically allocated string. The caller is responsible for freeing this memory.
 * Returns NULL if the key does not exist or if there is an error in the process.
 */
char *config_get_encrypted_value(const ConfigFile *config, const char *section, const char *key, const char *encryption_key) {
    if (!config || !section || !key || !encryption_key) {
        fmt_fprintf(stderr, "Error: Invalid arguments provided to config_get_encrypted_value.\n");
        return NULL;
    }

    const char *encrypted_value = config_get_value(config, section, key);
    if (!encrypted_value) {
        fmt_fprintf(stderr, "Error: Unable to find encrypted value for key '%s' in section '%s'.\n", key, section);
        return NULL;
    }

    size_t value_size = strlen(encrypted_value);
    char *decrypted_value = malloc(value_size + 1);
    if (!decrypted_value) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in config_get_encrypted_value.\n");
        return NULL;
    }

    xor_encrypt_decrypt(encrypted_value, decrypted_value, encryption_key[0], value_size);
    decrypted_value[value_size] = '\0';

    return decrypted_value;
}

/**
 * @brief Sets an encrypted value for a given key in a section.
 *
 * This function encrypts the provided value using a simple XOR encryption with the provided encryption key
 * and then stores the encrypted value in the configuration file under the specified section and key.
 *
 * @param config Pointer to the ConfigFile structure.
 * @param section The section name where the key is located.
 * @param key The key whose value will be encrypted and stored.
 * @param value The value to be encrypted and stored.
 * @param encryption_key The key used to encrypt the value.
 */
void config_set_encrypted_value(ConfigFile *config, const char *section, const char *key, const char *value, const char *encryption_key) {
    if (!config || !section || !key || !value || !encryption_key) {
        fmt_fprintf(stderr, "Error: Invalid arguments provided to config_set_encrypted_value.\n");
        return;
    }
    
    size_t value_size = strlen(value);
    char *encrypted_value = malloc(value_size + 1);
    if (!encrypted_value) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in config_set_encrypted_value.\n");
        return;
    }

    xor_encrypt_decrypt(value, encrypted_value, encryption_key[0], value_size);
    encrypted_value[value_size] = '\0';

    config_set_value(config, section, key, encrypted_value);
    free(encrypted_value);
}