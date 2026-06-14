/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Config
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in config.c (file-level convention).
 *
 * INI-style configuration reader/writer with sections, comments,
 * typed getters, array values and an optional encryption hook.
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/* #define CONFIG_LOGGING_ENABLE */

#ifdef CONFIG_LOGGING_ENABLE
    #define CONFIG_LOG(fmt, ...) \
        do { fprintf(stderr, "[CONFIG LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define CONFIG_LOG(...) do { } while (0)
#endif



typedef struct ConfigEntry     ConfigEntry;
typedef struct ConfigSection   ConfigSection;
typedef struct ConfigFile      ConfigFile;
typedef struct ConfigIterator  ConfigIterator;

/* A single key=value pair (or a comment line). */
struct ConfigEntry {
    bool isComment;
    union {
        struct {
            char* key;
            char* value;
        };
        char* comment;
    };
};


/* A named section grouping a contiguous block of entries. */
struct ConfigSection {
    char*         section_name;
    ConfigEntry*  entries;
    size_t        entry_count;
    char*         comment;       /* trailing/leading section comment */
    size_t        entry_capacity;/* allocated slots in `entries` (>= entry_count).*/
};


/* The whole loaded configuration. */
struct ConfigFile {
    ConfigSection** sections;
    size_t          section_count;
    char*           default_section;
    char*           filename;
    void (*modification_callback)(const char* section, const char* key, const char* value);
    size_t          section_capacity; /* allocated slots in `sections` (>= section_count) */
};


/* Stateful iterator over every (section,key,value) triple. */
struct ConfigIterator {
    const ConfigFile* config;
    size_t            section_index;
    size_t            entry_index;
};


/* ------------------------------------------------------------------ */
/* Construction / destruction                                         */
/* ------------------------------------------------------------------ */

ConfigFile*       config_create                       (const char* filename);
ConfigFile*       config_create_empty                 (void);
void              config_deallocate                   (ConfigFile* config);
void              config_clear                        (ConfigFile* config);


/* ------------------------------------------------------------------ */
/* I/O                                                                */
/* ------------------------------------------------------------------ */

void              config_save                         (const ConfigFile* config, const char* filename);
void              config_reload                       (ConfigFile** config);
char*             config_to_string                    (const ConfigFile* config);
bool              config_load_string                  (ConfigFile* config, const char* data);


/* ------------------------------------------------------------------ */
/* Typed getters                                                      */
/* ------------------------------------------------------------------ */

const char*       config_get_value                    (const ConfigFile* config, const char* section, const char* key);
bool              config_get_bool                     (const ConfigFile* config, const char* section, const char* key, bool default_value);
int               config_get_int                      (const ConfigFile* config, const char* section, const char* key, int default_value);
double            config_get_double                   (const ConfigFile* config, const char* section, const char* key, double default_value);
char**            config_get_array                    (const ConfigFile* config, const char* section, const char* key, size_t* array_size);
char*             config_get_encrypted_value          (const ConfigFile* config, const char* section, const char* key, const char* encryption_key);


/* ------------------------------------------------------------------ */
/* Setters / mutation                                                 */
/* ------------------------------------------------------------------ */

void              config_set_value                    (ConfigFile* config, const char* section, const char* key, const char* value);
void              config_set_int                      (ConfigFile* config, const char* section, const char* key, int value);
void              config_set_double                   (ConfigFile* config, const char* section, const char* key, double value);
void              config_set_bool                     (ConfigFile* config, const char* section, const char* key, bool value);
void              config_set_array                    (ConfigFile* config, const char* section, const char* key, const char* const* array, size_t array_size);
void              config_set_encrypted_value          (ConfigFile* config, const char* section, const char* key, const char* value, const char* encryption_key);
void              config_set_comment                  (ConfigFile* config, const char* section, const char* comment);
void              config_remove_section               (ConfigFile* config, const char* section);
void              config_remove_key                   (ConfigFile* config, const char* section, const char* key);
void              config_merge                        (ConfigFile* dest, const ConfigFile* src);


/* ------------------------------------------------------------------ */
/* Introspection                                                      */
/* ------------------------------------------------------------------ */

bool              config_has_section                  (const ConfigFile* config, const char* section);
bool              config_has_key                      (const ConfigFile* config, const char* section, const char* key);
size_t            config_section_count                (const ConfigFile* config);
size_t            config_key_count                    (const ConfigFile* config, const char* section);
char**            config_list_sections                (const ConfigFile* config, size_t* out_count);
char**            config_list_keys                    (const ConfigFile* config, const char* section, size_t* out_count);


/* ------------------------------------------------------------------ */
/* Iteration                                                          */
/* ------------------------------------------------------------------ */

ConfigIterator    config_get_iterator                 (const ConfigFile* config);
bool              config_next_entry                   (ConfigIterator* iterator, const char** section, const char** key, const char** value);


/* ------------------------------------------------------------------ */
/* Validation / callbacks                                             */
/* ------------------------------------------------------------------ */

void              config_validate_structure           (const ConfigFile* config, const ConfigSection* expected_structure, size_t structure_size);
void              config_register_modification_callback(ConfigFile* config, void (*callback)(const char* section, const char* key, const char* value));


#ifdef __cplusplus
}
#endif

#endif 
