/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Config
*/

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "config.h"
#include "../string/std_string.h"
#include "../fmt/fmt.h"


static char *trim_whitespace(char *str) {
    if (!str) {
        CONFIG_LOG("[trime_whitespace] Error: NULL string provided to trim_whitespace.\n");
        return NULL;
    }
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
    if (!input || !output) {
        CONFIG_LOG("[xor_encrypt_decrypt] Error: NULL input or output provided to xor_encrypt_decrypt.\n");
        return;
    }
    
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
    CONFIG_LOG("[config_create] Creating configuration from file '%s'.", filename);

    FileReader* fr = file_reader_open(filename, READ_TEXT);
    if (!fr) {
        CONFIG_LOG("[config_create] Error: Unable to open file '%s' for reading.", filename);
        exit(-1);
    }

    ConfigFile *config = malloc(sizeof(ConfigFile));
    if (!config) {
        CONFIG_LOG("[config_create] Error: Memory allocation failed for ConfigFile.");
        file_reader_close(fr);
        exit(-1);
    }

    config->sections = NULL;
    config->section_count = 0;
    config->default_section = NULL;
    config->filename = string_strdup(filename);
    CONFIG_LOG("[config_create] Initialized ConfigFile structure.");

    char line[1024];
    ConfigSection *current_section = NULL;

    while (file_reader_read_line(line, sizeof(line), fr)) {
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
            CONFIG_LOG("[config_create] Comment found: %s", entry.value);
        }
        else if (writable_trimmed[0] == '[') {
            current_section = malloc(sizeof(ConfigSection));
            if (!current_section) {
                CONFIG_LOG("[config_create] Error: Memory allocation failed for ConfigSection.");
                free(writable_trimmed);
                file_reader_close(fr);
                exit(-1);
            }
            
            size_t section_name_length = strlen(writable_trimmed) - 2; // Exclude '[' and ']'
            current_section->section_name = malloc(section_name_length + 1); // +1 for null terminator
            strncpy(current_section->section_name, writable_trimmed + 1, section_name_length); // Skip '['
            current_section->section_name[section_name_length] = '\0'; // Null terminate
            current_section->entries = NULL;
            current_section->entry_count = 0;

            config->sections = realloc(config->sections, (config->section_count + 1) * sizeof(ConfigSection *));
            config->sections[config->section_count++] = current_section;
            CONFIG_LOG("[config_create] New section added: [%s]", current_section->section_name);
        } 
        else if (current_section && strchr(writable_trimmed, '=')) {
            char *key = strtok(writable_trimmed, "=");
            char *value = strtok(NULL, "");
            entry.key = string_strdup(trim_whitespace(key));
            entry.value = string_strdup(trim_whitespace(value));
            CONFIG_LOG("[config_create] Key-Value pair found: %s = %s", entry.key, entry.value);
        }

        if (current_section && (entry.isComment || entry.key)) {
            current_section->entries = realloc(current_section->entries, (current_section->entry_count + 1) * sizeof(ConfigEntry));
            current_section->entries[current_section->entry_count++] = entry;
        }

        free(writable_trimmed);
    }

    CONFIG_LOG("[config_create] Configuration successfully loaded from '%s'.", filename);
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
    CONFIG_LOG("[config_save] Saving configuration to file '%s'.", filename);

    FileWriter* fw = file_writer_open(filename, WRITE_TEXT);
    if (!fw) {
        CONFIG_LOG("[config_save] Error: Unable to open file '%s' for writing.", filename);
        return;
    }

    for (size_t i = 0; i < config->section_count; i++) {
        ConfigSection *section = config->sections[i];
        
        CONFIG_LOG("[config_save] Writing section [%s].", section->section_name);
        file_writer_write_fmt(fw, "[%s]\n", section->section_name);

        for (size_t j = 0; j < section->entry_count; j++) {
            ConfigEntry *entry = &section->entries[j];
            if (entry->isComment) {
                CONFIG_LOG("[config_save] Writing comment: %s", entry->value);
                file_writer_write_fmt(fw, "%s\n", entry->value);
            }
            else if (entry->key && entry->value) {
                CONFIG_LOG("[config_save] Writing key-value pair: %s=%s", entry->key, entry->value);
                file_writer_write_fmt(fw, "%s=%s\n", entry->key, entry->value);
            }
        }
        file_writer_write_fmt(fw, "\n");
    }

    CONFIG_LOG("[config_save] Configuration saved successfully to '%s'.", filename);
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
    CONFIG_LOG("[config_get_value] Retrieving value for section '%s', key '%s'.", section, key);

    if (!config) {
        CONFIG_LOG("[config_get_value] Error: ConfigFile pointer is NULL.");
        return NULL;
    }
    if (!section) {
        CONFIG_LOG("[config_get_value] Error: Section name is NULL.");
        return NULL;
    }
    if (!key) {
        CONFIG_LOG("[config_get_value] Error: Key name is NULL.");
        return NULL;
    }

    for (size_t i = 0; i < config->section_count; ++i) {
        if (strcmp(config->sections[i]->section_name, section) == 0) {
            ConfigSection *sec = config->sections[i];
            for (size_t j = 0; j < sec->entry_count; ++j) {
                if (sec->entries[j].key && strcmp(sec->entries[j].key, key) == 0) {
                    CONFIG_LOG("[config_get_value] Found value for key '%s': %s", key, sec->entries[j].value);
                    return sec->entries[j].value;
                }
            }
            CONFIG_LOG("[config_get_value] Key '%s' not found in section '%s'.", key, section);
            break;
        }
    }

    CONFIG_LOG("[config_get_value] Section '%s' not found.", section);
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
    CONFIG_LOG("[config_set_value] Setting value for section '%s', key '%s'.", section, key);

    if (!config) {
        CONFIG_LOG("[config_set_value] Error: ConfigFile pointer is NULL.");
        return;
    }
    if (!section) {
        CONFIG_LOG("[config_set_value] Error: Section name is NULL.");
        return;
    }
    if (!key) {
        CONFIG_LOG("[config_set_value] Error: Key name is NULL.");
        return;
    }
    if (!value) {
        CONFIG_LOG("[config_set_value] Error: Value is NULL.");
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
        CONFIG_LOG("[config_set_value] Section '%s' not found, creating a new section.", section);
        sec = malloc(sizeof(ConfigSection));
        if (!sec) {
            CONFIG_LOG("[config_set_value] Error: Memory allocation failed for new section.");
            return;
        }
        sec->section_name = string_strdup(section);
        sec->entries = NULL;
        sec->entry_count = 0;

        config->sections = realloc(config->sections, (config->section_count + 1) * sizeof(ConfigSection *));
        if (!config->sections) {
            free(sec->section_name);
            free(sec);
            CONFIG_LOG("[config_set_value] Error: Memory allocation failed for sections array.");
            return;
        }
        config->sections[config->section_count++] = sec;
        CONFIG_LOG("[config_set_value] New section '%s' created.", section);
    }

    for (size_t j = 0; j < sec->entry_count; ++j) {
        if (sec->entries[j].key && strcmp(sec->entries[j].key, key) == 0) {
            CONFIG_LOG("[config_set_value] Updating existing key '%s' in section '%s'.", key, section);
            free(sec->entries[j].value);
            sec->entries[j].value = string_strdup(value);
            CONFIG_LOG("[config_set_value] Key '%s' updated with value '%s'.", key, value);
            return;
        }
    }

    // Add a new entry
    sec->entries = realloc(sec->entries, (sec->entry_count + 1) * sizeof(ConfigEntry));
    if (!sec->entries) {
        CONFIG_LOG("[config_set_value] Error: Memory allocation failed for new entry.");
        return;
    }

    sec->entries[sec->entry_count].key = string_strdup(key); // Allocate and set key
    sec->entries[sec->entry_count].value = string_strdup(value); // Allocate and set value
    sec->entry_count++;
    
    CONFIG_LOG("[config_set_value] New key '%s' with value '%s' added to section '%s'.", key, value, section);
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
    CONFIG_LOG("[config_remove_section] Attempting to remove section '%s'.", section);

    if (!config || !section) {
        CONFIG_LOG("[config_remove_section] Error: Invalid arguments provided.");
        return;
    }

    bool section_found = false;
    for (size_t i = 0; i < config->section_count; ++i) {
        if (strcmp(config->sections[i]->section_name, section) == 0) {
            CONFIG_LOG("[config_remove_section] Section '%s' found, removing it.", section);
            section_found = true;
            free(config->sections[i]->section_name);

            for (size_t j = 0; j < config->sections[i]->entry_count; ++j) {
                free(config->sections[i]->entries[j].key);
                free(config->sections[i]->entries[j].value);
            }

            free(config->sections[i]->entries);
            free(config->sections[i]);

            // Shift the remaining sections up
            for (size_t k = i; k < config->section_count - 1; ++k) {
                config->sections[k] = config->sections[k + 1];
            }

            config->section_count--;
            config->sections = realloc(config->sections, config->section_count * sizeof(ConfigSection *));
            CONFIG_LOG("[config_remove_section] Section '%s' successfully removed.", section);

            return;
        }
    }

    if (!section_found) {
        CONFIG_LOG("[config_remove_section] Warning: Section '%s' not found.", section);
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
    CONFIG_LOG("[config_remove_key] Attempting to remove key '%s' from section '%s'.", key, section);

    if (!config || !section || !key) {
        CONFIG_LOG("[config_remove_key] Error: Invalid arguments provided.");
        return;
    }

    bool key_found = false;
    for (size_t i = 0; i < config->section_count; ++i) {
        if (strcmp(config->sections[i]->section_name, section) == 0) {
            ConfigSection *sec = config->sections[i];

            for (size_t j = 0; j < sec->entry_count; ++j) {
                if (strcmp(sec->entries[j].key, key) == 0) {
                    CONFIG_LOG("[config_remove_key] Key '%s' found in section '%s', removing it.", key, section);
                    key_found = true;

                    free(sec->entries[j].key);
                    free(sec->entries[j].value);

                    // Shift the remaining entries up
                    for (size_t k = j; k < sec->entry_count - 1; ++k) {
                        sec->entries[k] = sec->entries[k + 1];
                    }
                    sec->entry_count--;
                    sec->entries = realloc(sec->entries, sec->entry_count * sizeof(ConfigEntry));

                    CONFIG_LOG("[config_remove_key] Key '%s' successfully removed from section '%s'.", key, section);
                    return;
                }
            }
            break;
        }
    }

    if (!key_found) {
        CONFIG_LOG("[config_remove_key] Warning: Key '%s' in section '%s' not found.", key, section);
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
    CONFIG_LOG("[config_deallocate] Deallocating configuration file.");

    if (!config) {
        CONFIG_LOG("[config_deallocate] Error: ConfigFile pointer is NULL.");
        return;
    }

    for (size_t i = 0; i < config->section_count; ++i) {
        CONFIG_LOG("[config_deallocate] Deallocating section '%s'.", config->sections[i]->section_name);

        free(config->sections[i]->section_name);
        for (size_t j = 0; j < config->sections[i]->entry_count; ++j) {
            CONFIG_LOG("[config_deallocate] Deallocating key-value pair: '%s'='%s'.",
                       config->sections[i]->entries[j].key,
                       config->sections[i]->entries[j].value);

            free(config->sections[i]->entries[j].key);
            free(config->sections[i]->entries[j].value);
        }

        free(config->sections[i]->entries);
        free(config->sections[i]);
    }

    CONFIG_LOG("[config_deallocate] Deallocating sections array.");
    free(config->sections);

    if (config->default_section) {
        CONFIG_LOG("[config_deallocate] Deallocating default section name '%s'.", config->default_section);
    }
    free(config->default_section);

    CONFIG_LOG("[config_deallocate] Deallocating ConfigFile structure.");
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
    CONFIG_LOG("[config_has_section] Checking for section '%s'.", section);
    if (!config || !section) {
        CONFIG_LOG("[config_has_section] Error: Invalid arguments provided.");
        return false;
    }

    for (size_t i = 0; i < config->section_count; ++i) {
        if (strcmp(config->sections[i]->section_name, section) == 0) {
            CONFIG_LOG("[config_has_section] Section '%s' found.", section);
            return true;
        }
    }

    CONFIG_LOG("[config_has_section] Section '%s' not found.", section);
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
    CONFIG_LOG("[config_has_key] Checking for key '%s' in section '%s'.", key, section);
    if (!config || !section || !key) {
        CONFIG_LOG("[config_has_key] Error: Invalid arguments provided.");
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
                            CONFIG_LOG("[config_has_key] Found key '%s' in section '%s'.", key, section);
                            return true;
                        }
                    }
                    break;
                }
            }
            break;
        }
    }
    CONFIG_LOG("[config_has_key] Key '%s' not found in section '%s'.", key, section);
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
    CONFIG_LOG("[config_get_int] Retrieving integer value for key='%s' in section='%s'.", key, section);
    if (!config || !section || !key) {
        CONFIG_LOG("[config_get_int] Error: Invalid arguments provided.");
        return default_value;
    }

    const char *value = config_get_value(config, section, key);
    if (value) {
        CONFIG_LOG("[config_get_int] Found value='%s' for key='%s'.", value, key);

        char *end;
        long int_val = strtol(value, &end, 10);

        if (end != value && *end == '\0') {
            CONFIG_LOG("[config_get_int] Successfully parsed integer value '%ld'.", int_val);
            return (int)int_val; 
        }

        CONFIG_LOG("[config_get_int] Value '%s' is not a valid integer.", value);
    } 
    else {
        CONFIG_LOG("[config_get_int] Key='%s' not found, returning default value '%d'.", key, default_value);
    }

    return default_value; 
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
    CONFIG_LOG("[config_get_double] Retrieving double value for key='%s' in section='%s'.", key, section);

    if (!config || !section || !key) {
        CONFIG_LOG("[config_get_double] Error: Invalid arguments provided.");
        return default_value;
    }

    const char *value = config_get_value(config, section, key);
    if (value) {
        CONFIG_LOG("[config_get_double] Found value='%s' for key='%s'.", value, key);

        char *end;
        double double_val = strtod(value, &end);

        if (end != value && *end == '\0') {
            CONFIG_LOG("[config_get_double] Successfully parsed double value '%f'.", double_val);
            return double_val; 
        }

        CONFIG_LOG("[config_get_double] Value '%s' is not a valid double.", value);
    } 
    else {
        CONFIG_LOG("[config_get_double] Key='%s' not found, returning default value '%f'.", key, default_value);
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
    CONFIG_LOG("[config_get_bool] Retrieving boolean value for section='%s', key='%s'.", section, key);
    if (!config || !section || !key) {
        CONFIG_LOG("[config_get_bool] Error: Invalid arguments provided.");
        return default_value;
    }

    const char *value = config_get_value(config, section, key);

    if (value) {
        CONFIG_LOG("[config_get_bool] Found value='%s' for key='%s'.", value, key);

        if (strcasecmp(value, "true") == 0 || strcasecmp(value, "yes") == 0 || strcmp(value, "1") == 0) { 
            CONFIG_LOG("[config_get_bool] Interpreted value as 'true'.");
            return true;
        }
        else if (strcasecmp(value, "false") == 0 || strcasecmp(value, "no") == 0 || strcmp(value, "0") == 0) { 
            CONFIG_LOG("[config_get_bool] Interpreted value as 'false'.");
            return false;
        }
    }

    CONFIG_LOG("[config_get_bool] Returning default value '%d'.", default_value);
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
    CONFIG_LOG("[config_set_comment] Setting comment for section='%s'.", section);
    if (!config || !section || !comment) {
        CONFIG_LOG("[config_set_comment] Error: Invalid arguments provided.");
        return;
    }

    for (size_t i = 0; i < config->section_count; ++i) {
        ConfigSection *sec = config->sections[i];
        if (strcmp(config->sections[i]->section_name, section) == 0) {
            CONFIG_LOG("[config_set_comment] Found section '%s', setting comment.", section);
            sec->comment = string_strdup(comment); 
            return; 
        }
    }

    CONFIG_LOG("[config_set_comment] Warning: Section '%s' not found.", section);
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
    CONFIG_LOG("[config_get_iterator] Initializing iterator for configuration.");
    ConfigIterator iterator = {0};

    if (!config) {
        CONFIG_LOG("[config_get_iterator] Error: ConfigFile is NULL.");
        return iterator; 
    }

    iterator.config = config;
    iterator.section_index = 0;
    iterator.entry_index = 0;

    CONFIG_LOG("[config_get_iterator] Iterator initialized successfully.");
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
    CONFIG_LOG("[config_next_entry] Retrieving next entry in configuration.");

    if (!iterator || !iterator->config || iterator->section_index >= iterator->config->section_count) {
        CONFIG_LOG("[config_next_entry] Error: Invalid iterator or configuration.");
        return false;
    }

    while (iterator->section_index < iterator->config->section_count) {
        ConfigSection *sec = iterator->config->sections[iterator->section_index];

        if (iterator->entry_index < sec->entry_count) {
            *section = sec->section_name;
            *key = sec->entries[iterator->entry_index].key;
            *value = sec->entries[iterator->entry_index].value;
            CONFIG_LOG("[config_next_entry] Found entry: Section='%s', Key='%s', Value='%s'.", *section, *key, *value);
            iterator->entry_index++;

            return true;
        } 
        else {
            iterator->section_index++;
            iterator->entry_index = 0;
        }
    }

    CONFIG_LOG("[config_next_entry] No more entries to retrieve.");
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
    CONFIG_LOG("[config_reload] Reloading configuration from file.");

    if (!config_ptr || !(*config_ptr) || !(*config_ptr)->filename) {
        CONFIG_LOG("[config_reload] Error: Invalid configuration pointer or missing filename.");
        return;
    }

    CONFIG_LOG("[config_reload] Reloading from file: '%s'.", (*config_ptr)->filename);

    ConfigFile *new_config = config_create((*config_ptr)->filename);
    if (!new_config) {
        CONFIG_LOG("[config_reload] Error: Failed to reload configuration from file '%s'.", (*config_ptr)->filename);
        return;
    }

    CONFIG_LOG("[config_reload] Successfully reloaded configuration from '%s'.", (*config_ptr)->filename);
    config_deallocate(*config_ptr); 
    *config_ptr = new_config; 

    CONFIG_LOG("[config_reload] Configuration reloaded successfully.");
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
    CONFIG_LOG("[config_register_modification_callback] Registering modification callback.");
    if (!config) {
        CONFIG_LOG("[config_register_modification_callback] Error: ConfigFile is NULL.");
        return;
    }

    config->modification_callback = callback;
    CONFIG_LOG("[config_register_modification_callback] Modification callback registered successfully.");
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
    CONFIG_LOG("[config_validate_structure] Validating configuration structure.");

    if (!config || !expected_structure) {
        CONFIG_LOG("[config_validate_structure] Error: Invalid arguments provided.");
        return;
    }

    for (size_t i = 0; i < structure_size; ++i) {
        const ConfigSection *expected_sec = &expected_structure[i];
        bool section_found = false;

        CONFIG_LOG("[config_validate_structure] Validating section: '%s'.", expected_sec->section_name);
        for (size_t j = 0; j < config->section_count; ++j) {
            if (strcmp(config->sections[j]->section_name, expected_sec->section_name) == 0) {
                section_found = true;
                CONFIG_LOG("[config_validate_structure] Section '%s' found.", expected_sec->section_name);
                // Additional validation for keys and values can be added here
                break;
            }
        }

        if (!section_found) {
            CONFIG_LOG("[config_validate_structure] Warning: Section '%s' is missing in the configuration.", expected_sec->section_name);
        }
    }
    CONFIG_LOG("[config_validate_structure] Configuration structure validation completed.");
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
    CONFIG_LOG("[config_get_array] Retrieving array for key '%s' in section '%s'.", key, section);

    if (!config || !section || !key || !array_size) {
        CONFIG_LOG("[config_get_array] Error: Invalid arguments provided.");
        *array_size = 0;
        return NULL;
    }

    const char *value = config_get_value(config, section, key);
    if (!value) {
        CONFIG_LOG("[config_get_array] Warning: Value for key '%s' in section '%s' not found.", key, section);
        *array_size = 0;
        return NULL;
    }

    *array_size = 1;  // Count the number of elements
    for (const char *p = value; *p; ++p) {
        if (*p == ',') (*array_size)++;
    }

    CONFIG_LOG("[config_get_array] Found %zu elements for key '%s'.", *array_size, key);

    char **array = malloc(*array_size * sizeof(char *)); 
    if (!array) {
        CONFIG_LOG("[config_get_array] Error: Memory allocation failed.");
        *array_size = 0;
        return NULL;
    }

    char *value_copy = string_strdup(value);
    char *token = strtok(value_copy, ", ");
    size_t idx = 0;

    while (token) {
        array[idx++] = string_strdup(token);
        CONFIG_LOG("[config_get_array] Added element '%s' to the array.", token);
        token = strtok(NULL, ",");
    }
    free(value_copy);

    CONFIG_LOG("[config_get_array] Array retrieval completed for key '%s' in section '%s'.", key, section);
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
    CONFIG_LOG("[config_set_array] Setting array for key '%s' in section '%s'.", key, section);

    if (!config || !section || !key || !array || array_size == 0) {
        CONFIG_LOG("[config_set_array] Error: Invalid arguments provided.");
        return;
    }

    size_t total_length = strlen(key) + 2; // +2 for '=' and '\0'
    for (size_t i = 0; i < array_size; i++) {
        total_length += strlen(array[i]) + ((i < array_size - 1) ? 1 : 0); // +1 for comma, if not the last element
    }

    char *combined = malloc(total_length);
    if (!combined) {
        CONFIG_LOG("[config_set_array] Error: Memory allocation failed.");
        return;
    }

    char *ptr = combined;
    ptr += sprintf(ptr, "%s=", key);
    for (size_t i = 0; i < array_size; i++) {
        ptr += sprintf(ptr, "%s%s", array[i], (i < array_size - 1) ? ", " : "");
    }

    CONFIG_LOG("[config_set_array] Combined array value for key '%s': %s", key, combined);
    config_set_value(config, section, key, combined);
    
    free(combined);
    CONFIG_LOG("[config_set_array] Array set successfully for key '%s' in section '%s'.", key, section);
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
    CONFIG_LOG("[config_get_encrypted_value] Retrieving encrypted value for key '%s' in section '%s'.", key, section);

    if (!config || !section || !key || !encryption_key) {
        CONFIG_LOG("[config_get_encrypted_value] Error: Invalid arguments provided.");
        return NULL;
    }

    const char *encrypted_value = config_get_value(config, section, key);
    if (!encrypted_value) {
        CONFIG_LOG("[config_get_encrypted_value] Error: Unable to find encrypted value for key '%s' in section '%s'.", key, section);
        return NULL;
    }

    size_t value_size = strlen(encrypted_value);
    char *decrypted_value = malloc(value_size + 1);
    if (!decrypted_value) {
        CONFIG_LOG("[config_get_encrypted_value] Error: Memory allocation failed.");
        return NULL;
    }

    CONFIG_LOG("[config_get_encrypted_value] Decrypting value for key '%s'.", key);
    xor_encrypt_decrypt(encrypted_value, decrypted_value, encryption_key[0], value_size);
    decrypted_value[value_size] = '\0';

    CONFIG_LOG("[config_get_encrypted_value] Decryption successful for key '%s'.", key);
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
    CONFIG_LOG("[config_set_encrypted_value] Setting encrypted value for key '%s' in section '%s'.", key, section);
    
    if (!config || !section || !key || !value || !encryption_key) {
        CONFIG_LOG("[config_set_encrypted_value] Error: Invalid arguments provided.");
        return;
    }
    
    size_t value_size = strlen(value);
    char *encrypted_value = malloc(value_size + 1);
    if (!encrypted_value) {
        CONFIG_LOG("[config_set_encrypted_value] Error: Memory allocation failed.");
        return;
    }

    CONFIG_LOG("[config_set_encrypted_value] Encrypting value.");
    xor_encrypt_decrypt(value, encrypted_value, encryption_key[0], value_size);
    encrypted_value[value_size] = '\0';

    CONFIG_LOG("[config_set_encrypted_value] Encrypted value set for key '%s' in section '%s'.", key, section);
    config_set_value(config, section, key, encrypted_value);
    
    free(encrypted_value);
    CONFIG_LOG("[config_set_encrypted_value] Encryption process completed and memory freed.");
}
