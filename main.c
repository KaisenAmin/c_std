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

    // Expected structure
    const ConfigSection expected_structure[] = {
        {"global", NULL, 0, NULL},
        {"user_preferences", NULL, 0, NULL},
        {"network", NULL, 0, NULL}
    };

    // Validate the structure
    config_validate_structure(config, expected_structure, sizeof(expected_structure) / sizeof(ConfigSection));
    
    // Iterate through the expected structure and check for each section
    for (size_t i = 0; i < sizeof(expected_structure) / sizeof(ConfigSection); ++i) 
    {
        if (!config_has_section(config, expected_structure[i].section_name)) 
            printf("Section '%s' is missing in the configuration.\n", expected_structure[i].section_name);
    }

    config_deallocate(config);
    return 0;
}
