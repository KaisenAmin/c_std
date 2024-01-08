/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Config
*/
#include "config.h"
#include "../string/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char* my_strdup(const char* s) 
{
    if (s == NULL) 
        return NULL;
        
    char* new_str = malloc(strlen(s) + 1);
    if (new_str) 
        strcpy(new_str, s);

    return new_str;
}

static char *trim_whitespace(char *str) 
{
    char *end;

    while (isspace((unsigned char)*str))  // Trim leading space
        str++;

    if (*str == 0)  // All spaces?
        return str;

    end = str + strlen(str) - 1;  // Trim trailing space
    while (end > str && isspace((unsigned char)*end)) 
        end--;
        
    end[1] = '\0'; // Write new null terminator character

    return str;
}

static void xor_encrypt_decrypt(const char *input, char *output, char key, size_t size) 
{
    for (size_t i = 0; i < size; ++i) 
        output[i] = input[i] ^ key;
}

ConfigFile *config_create(const char *filename) 
{
    FILE *file = fopen(filename, "r");
    if (!file) 
    {
        perror("Error opening file");
        return NULL;
    }

    ConfigFile *config = malloc(sizeof(ConfigFile));
    if (!config) 
    {
        perror("Error allocating memory for ConfigFile");
        fclose(file);
        return NULL;
    }

    config->sections = NULL;
    config->section_count = 0;
    config->default_section = NULL;
    config->filename = my_strdup(filename);

    char line[1024];
    ConfigSection *current_section = NULL;

    while (fgets(line, sizeof(line), file)) 
    {
        String* str = string_create(line);
        string_trim(str);
        const char *trimmed = string_c_str(str);

        char *writable_trimmed = my_strdup(trimmed);
        string_deallocate(str);

        ConfigEntry entry = {0};
        entry.isComment = false;
        entry.key = NULL;
        entry.value = NULL;

        if (writable_trimmed[0] == '#' || writable_trimmed[0] == ';') 
        {
            entry.isComment = true;
            entry.value = my_strdup(writable_trimmed);
        }
        if (writable_trimmed[0] == '[') 
        {
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
        else if (current_section && strchr(writable_trimmed, '=')) 
        {
            char *key = strtok(writable_trimmed, "=");
            char *value = strtok(NULL, "");
            entry.key = my_strdup(trim_whitespace(key));
            entry.value = my_strdup(trim_whitespace(value));
        }

        if (current_section && (entry.isComment || entry.key)) 
        {
            current_section->entries = realloc(current_section->entries, (current_section->entry_count + 1) * sizeof(ConfigEntry));
            current_section->entries[current_section->entry_count++] = entry;
        }

        free(writable_trimmed);
    }

    fclose(file);
    return config;
}

// Saves the current state of the configuration structure to a file
void config_save(const ConfigFile *config, const char *filename) 
{
    FILE *file = fopen(filename, "w");
    if (!file) 
    {
        perror("Error opening file for writing");
        return;
    }

    for (size_t i = 0; i < config->section_count; i++) 
    {
        ConfigSection *section = config->sections[i];
        
        fprintf(file, "[%s]\n", section->section_name); // Write the section name

        // Write all entries within the section
        for (size_t j = 0; j < section->entry_count; j++) 
        {
            ConfigEntry *entry = &section->entries[j];
            if (entry->isComment) 
                fprintf(file, "%s\n", entry->value);
            else 
            {
                if (entry->key && entry->value) // Ensure both key and value are written
                    fprintf(file, "%s=%s\n", entry->key, entry->value);
            }
        }

        fprintf(file, "\n"); // New line after each section for better readability
    }

    fclose(file);
}


// Retrieves the value for a given key in a specified section
const char *config_get_value(const ConfigFile *config, const char *section, const char *key) 
{
    if (!config || !section || !key) 
        return NULL;

    for (size_t i = 0; i < config->section_count; ++i) 
    {
        if (strcmp(config->sections[i]->section_name, section) == 0) 
        {
            ConfigSection *sec = config->sections[i];
            for (size_t j = 0; j < sec->entry_count; ++j) 
            {
                if (sec->entries[j].key && strcmp(sec->entries[j].key, key) == 0) 
                    return sec->entries[j].value;
            }
            
            break;
        }
    }
    return NULL;
}


// Sets the value for a given key in a specified section
void config_set_value(ConfigFile *config, const char *section, const char *key, const char *value) 
{
    if (!config || !section || !key || !value) 
        return;

    // Find or create the section
    ConfigSection *sec = NULL;
    for (size_t i = 0; i < config->section_count; ++i) 
    {
        if (strcmp(config->sections[i]->section_name, section) == 0) 
        {
            sec = config->sections[i];
            break;
        }
    }

    if (!sec) 
    {
        sec = malloc(sizeof(ConfigSection));
        if (!sec)
            return; // Memory allocation failed
        sec->section_name = my_strdup(section);
        sec->entries = NULL;
        sec->entry_count = 0;

        config->sections = realloc(config->sections, (config->section_count + 1) * sizeof(ConfigSection *));
        if (!config->sections) 
        {
            free(sec->section_name);
            free(sec);
            return; // Memory allocation failed
        }
        config->sections[config->section_count++] = sec;
    }

    // Update existing key or add new key-value pair
    for (size_t j = 0; j < sec->entry_count; ++j) 
    {
        if (sec->entries[j].key && strcmp(sec->entries[j].key, key) == 0) 
        {
            free(sec->entries[j].value);
            sec->entries[j].key = my_strdup(key);
            sec->entries[j].value = my_strdup(value);

            return;
        }
    }

    sec->entries = realloc(sec->entries, (sec->entry_count + 1) * sizeof(ConfigEntry));
    if (!sec->entries) 
    {
        perror("Failed to allocate memory for new entry");
        return; // Handle allocation error
    }

    sec->entries[sec->entry_count].key = my_strdup(key); // Allocate and set key
    sec->entries[sec->entry_count].value = my_strdup(value); // Allocate and set value
    sec->entry_count++;
}


// Removes an entire section from the configuration
void config_remove_section(ConfigFile *config, const char *section) 
{
    if (!config || !section) 
        return;

    for (size_t i = 0; i < config->section_count; ++i) {
        if (strcmp(config->sections[i]->section_name, section) == 0) 
        {
            free(config->sections[i]->section_name); // Free the section
            for (size_t j = 0; j < config->sections[i]->entry_count; ++j) 
            {
                free(config->sections[i]->entries[j].key);
                free(config->sections[i]->entries[j].value);
            }

            free(config->sections[i]->entries);
            free(config->sections[i]);
            
            for (size_t k = i; k < config->section_count - 1; ++k) // Shift remaining sections
                config->sections[k] = config->sections[k + 1];

            config->section_count--;
            config->sections = realloc(config->sections, config->section_count * sizeof(ConfigSection *)); // Resize the sections array
            
            return;
        }
    }
}

// Removes a specific key-value pair from a section in the configuration
void config_remove_key(ConfigFile *config, const char *section, const char *key) 
{
    if (!config || !section || !key) 
    {
        printf("Invalid input provided.\n");
        return;
    }

    for (size_t i = 0; i < config->section_count; ++i) 
    {
        if (strcmp(config->sections[i]->section_name, section) == 0) 
        {
            ConfigSection *sec = config->sections[i];

            for (size_t j = 0; j < sec->entry_count; ++j) 
            {
                if (strcmp(sec->entries[j].key, key) == 0) 
                {
                    free(sec->entries[j].key);
                    free(sec->entries[j].value);

                    for (size_t k = j; k < sec->entry_count - 1; ++k) 
                        sec->entries[k] = sec->entries[k + 1];

                    sec->entry_count--;
                    sec->entries = realloc(sec->entries, sec->entry_count * sizeof(ConfigEntry));
                   
                    return;
                }
            }
            break;
        }
    }
}


// Frees all memory associated with the configuration structure
void config_deallocate(ConfigFile *config) 
{
    if (!config) 
        return;

    for (size_t i = 0; i < config->section_count; ++i) 
    {
        free(config->sections[i]->section_name);
        for (size_t j = 0; j < config->sections[i]->entry_count; ++j) 
        {
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


// Checks if a specific section exists in the configuration
bool config_has_section(const ConfigFile *config, const char *section) 
{
    if (!config || !section) 
        return false;

    for (size_t i = 0; i < config->section_count; ++i) 
    {
        if (strcmp(config->sections[i]->section_name, section) == 0) 
            return true;
    }
    return false;
}

// Checks if a specific key exists within a section
bool config_has_key(const ConfigFile *config, const char *section, const char *key) 
{
    if (!config || !section || !key) 
        return false;

    for (size_t i = 0; i < config->section_count; ++i) 
    {
        if (strcmp(config->sections[i]->section_name, section) == 0) 
        {
            ConfigSection *sec = config->sections[i];
            for (size_t j = 0; j < sec->entry_count; ++j) 
            {
                if (strcmp(config->sections[i]->section_name, section) == 0) 
                {
                    ConfigSection *sec = config->sections[i];
                    for (size_t j = 0; j < sec->entry_count; ++j) 
                    {   
                        if (sec->entries[j].key && strcmp(sec->entries[j].key, key) == 0) 
                            return true;
                    }
                    break;
                }
            }
            break;
        }
    }
    return false;
}

// Retrieves an integer value for a given key, or a default if key is not found
int config_get_int(const ConfigFile *config, const char *section, const char *key, int default_value) 
{
    const char *value = config_get_value(config, section, key);
    if (value) 
    {
        char *end;
        long int_val = strtol(value, &end, 10);

        if (end != value && *end == '\0') 
            return (int)int_val; // Return the parsed integer
    }
    return default_value; // Return the default value if not found or not an integer
}

// Retrieves a double value for a given key, or a default if key is not found
double config_get_double(const ConfigFile *config, const char *section, const char *key, double default_value) 
{
    const char *value = config_get_value(config, section, key);
    if (value) 
    {
        char *end;
        double double_val = strtod(value, &end);

        if (end != value && *end == '\0') 
            return double_val; // Return the parsed double
    }
    return default_value; // Return the default value if not found or not a double
}

// Retrieves a boolean value for a given key, or a default if key is not found
bool config_get_bool(const ConfigFile *config, const char *section, const char *key, bool default_value) 
{
    const char *value = config_get_value(config, section, key);

    if (value) 
    {
        if (strcasecmp(value, "true") == 0 || strcasecmp(value, "yes") == 0 || strcmp(value, "1") == 0) 
            return true;
        else if (strcasecmp(value, "false") == 0 || strcasecmp(value, "no") == 0 || strcmp(value, "0") == 0) 
            return false;
    }
    return default_value; // Return the default value if not found or not a recognized boolean
}

// Adds a comment to a specific section in the configuration
void config_set_comment(ConfigFile *config, const char *section, const char *comment) 
{
    if (!config || !section || !comment) 
        return;

    for (size_t i = 0; i < config->section_count; ++i) 
    {
        ConfigSection *sec = config->sections[i];
        if (strcmp(config->sections[i]->section_name, section) == 0) 
            sec->comment = my_strdup(comment); // Allocate memory for and store the new comment 
        else
            sec->comment = NULL;
    }
}

// Initializes an iterator for traversing the configuration entries
ConfigIterator config_get_iterator(const ConfigFile *config) 
{
    ConfigIterator iterator = {0};

    if (config) 
    {
        iterator.config = config;
        iterator.section_index = 0;
        iterator.entry_index = 0;
    }

    return iterator;
}

// Iterates to the next entry in the configuration, returning false if at the end
bool config_next_entry(ConfigIterator *iterator, const char **section, const char **key, const char **value) 
{
    if (!iterator || !iterator->config || iterator->section_index >= iterator->config->section_count) 
        return false;

    while (iterator->section_index < iterator->config->section_count) 
    {
        ConfigSection *sec = iterator->config->sections[iterator->section_index];

        if (iterator->entry_index < sec->entry_count) 
        {
            *section = sec->section_name;
            *key = sec->entries[iterator->entry_index].key;
            *value = sec->entries[iterator->entry_index].value;
            iterator->entry_index++;

            return true;
        } 
        else 
        {
            iterator->section_index++;
            iterator->entry_index = 0;
        }
    }

    return false;
}

// Reloads the configuration from the file, updating the in-memory representation
void config_reload(ConfigFile **config_ptr) 
{
    if (!config_ptr || !(*config_ptr) || !(*config_ptr)->filename) 
        return;

    ConfigFile *new_config = config_create((*config_ptr)->filename);
    if (new_config) 
    {
        config_deallocate(*config_ptr); // Free the old configuration
        *config_ptr = new_config; // Point to the new configuration
    }
}

// Registers a callback function to be called upon modifications to the configuration
void config_register_modification_callback(ConfigFile *config, void (*callback)(const char *section, const char *key, const char *value)) 
{
    if (config) 
        config->modification_callback = callback;
}

// Validates the structure of the configuration against an expected template
void config_validate_structure(const ConfigFile *config, const ConfigSection *expected_structure, size_t structure_size) 
{
    if (!config || !expected_structure) 
        return; // Invalid input

    for (size_t i = 0; i < structure_size; ++i) 
    {
        const ConfigSection *expected_sec = &expected_structure[i];
        bool section_found = false;

        for (size_t j = 0; j < config->section_count; ++j) 
        {
            if (strcmp(config->sections[j]->section_name, expected_sec->section_name) == 0) 
            {
                section_found = true;  // Further validation can be added here to check for specific keys and values
                break;
            }
        }

        if (!section_found) 
            printf("Section '%s' is missing in the configuration.\n", expected_sec->section_name);
    }
}

// Retrieves an array of strings for a given key in a section
char **config_get_array(const ConfigFile *config, const char *section, const char *key, size_t *array_size) 
{
    const char *value = config_get_value(config, section, key);

    if (!value) 
    {
        *array_size = 0;
        return NULL;
    }

    *array_size = 1;  // Count the number of elements
    for (const char *p = value; *p; ++p) 
        if (*p == ',') (*array_size)++;
    
    char **array = malloc(*array_size * sizeof(char *)); // Allocate array of string pointers
    if (!array) 
    {
        *array_size = 0;
        return NULL;
    }

    // Split the string into array
    char *value_copy = my_strdup(value);
    char *token = strtok(value_copy, ", ");
    size_t idx = 0;

    while (token) 
    {
        array[idx++] = my_strdup(token);
        token = strtok(NULL, ",");
    }
    free(value_copy);

    return array;
}

// Sets an array of strings for a given key in a section
void config_set_array(ConfigFile *config, const char *section, const char *key, const char *const *array, size_t array_size) {
    if (!config || !section || !key || !array || array_size == 0) 
        return;

    size_t total_length = strlen(key) + 2; // +2 for '=' and '\0'
    for (size_t i = 0; i < array_size; i++) 
        total_length += strlen(array[i]) + ((i < array_size - 1) ? 1 : 0); // +1 for comma, if not the last element
    
    char *combined = malloc(total_length);
    if (!combined) 
        return; // Memory allocation failed

    char *ptr = combined;
    ptr += sprintf(ptr, "%s=", key);
    for (size_t i = 0; i < array_size; i++) 
        ptr += sprintf(ptr, "%s%s", array[i], (i < array_size - 1) ? ", " : "");

    config_set_value(config, section, key, combined);
    free(combined);
}

// Retrieves an encrypted value for a given key in a section
char *config_get_encrypted_value(const ConfigFile *config, const char *section, const char *key, const char *encryption_key) 
{
    const char *encrypted_value = config_get_value(config, section, key);
    if (!encrypted_value) 
        return NULL;

    size_t value_size = strlen(encrypted_value);
    char *decrypted_value = malloc(value_size + 1);
    if (!decrypted_value) 
        return NULL;

    xor_encrypt_decrypt(encrypted_value, decrypted_value, encryption_key[0], value_size);
    decrypted_value[value_size] = '\0';

    return decrypted_value;
}

// Sets an encrypted value for a given key in a section
void config_set_encrypted_value(ConfigFile *config, const char *section, const char *key, const char *value, const char *encryption_key)
{
    if (!config || !section || !key || !value) 
        return;
    
    size_t value_size = strlen(value);
    char *encrypted_value = malloc(value_size + 1);
    if (!encrypted_value) 
        return;

    xor_encrypt_decrypt(value, encrypted_value, encryption_key[0], value_size);
    encrypted_value[value_size] = '\0';

    config_set_value(config, section, key, encrypted_value);
    free(encrypted_value);
}