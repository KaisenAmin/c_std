/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Config
*/

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stddef.h>
#include <stdbool.h>

typedef struct ConfigEntry ConfigEntry;
typedef struct ConfigSection ConfigSection;
typedef struct ConfigFile ConfigFile;
typedef struct ConfigIterator ConfigIterator;

// Represents a key-value pair in a configuration
typedef struct ConfigEntry {
    bool isComment;
    union {
        struct {
            char *key;
            char *value;
        };
        char *comment;
    };
} ConfigEntry;

// Represents a section in a configuration file
typedef struct ConfigSection {
    char *section_name;
    ConfigEntry *entries;
    size_t entry_count;
    char *comment; // New field to store the comment
} ConfigSection;

// Represents the overall configuration file
struct ConfigFile {
    ConfigSection **sections;
    size_t section_count;
    char *default_section; // Default section name (optional)
    char *filename; // Filename from which the config was loaded
    void (*modification_callback)(const char *section, const char *key, const char *value); // New field
};

// Iterator for configuration entries
struct ConfigIterator {
    const ConfigFile *config;
    size_t section_index;
    size_t entry_index;
};

// Function declarations
ConfigFile *config_create(const char *filename);

const char *config_get_value(const ConfigFile *config, const char *section, const char *key);
char **config_get_array(const ConfigFile *config, const char *section, const char *key, size_t *array_size);
char *config_get_encrypted_value(const ConfigFile *config, const char *section, const char *key, const char *encryption_key);

void config_set_value(ConfigFile *config, const char *section, const char *key, const char *value);
void config_remove_section(ConfigFile *config, const char *section);
void config_remove_key(ConfigFile *config, const char *section, const char *key);
void config_deallocate(ConfigFile *config);
void config_save(const ConfigFile *config, const char *filename);
void config_reload(ConfigFile **config); // Reload configuration from file
void config_register_modification_callback(ConfigFile *config, void (*callback)(const char *section, const char *key, const char *value));
void config_validate_structure(const ConfigFile *config, const ConfigSection *expected_structure, size_t structure_size);
void config_set_comment(ConfigFile *config, const char *section, const char *comment); // Add a comment to a section
void config_set_encrypted_value(ConfigFile *config, const char *section, const char *key, const char *value, const char *encryption_key);
void config_set_array(ConfigFile *config, const char *section, const char *key, const char *const *array, size_t array_size);

bool config_next_entry(ConfigIterator *iterator, const char **section, const char **key, const char **value); // Iterate over entries
bool config_get_bool(const ConfigFile *config, const char *section, const char *key, bool default_value);
bool config_has_section(const ConfigFile *config, const char *section);
bool config_has_key(const ConfigFile *config, const char *section, const char *key);

int config_get_int(const ConfigFile *config, const char *section, const char *key, int default_value);
double config_get_double(const ConfigFile *config, const char *section, const char *key, double default_value);
ConfigIterator config_get_iterator(const ConfigFile *config); // Get an iterator for the config file

#endif // CONFIG_H_
