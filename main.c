#include "config/config.h"
#include <stdio.h>

int main() 
{
    ConfigFile *config1 = config_create("sources/config.ini");
    ConfigFile *config2 = config_create("sources/dynamic_config.ini");

    if (!config1 || !config2) 
    {
        printf("Failed to load configurations.\n");
        if (config1) config_deallocate(config1);
        if (config2) config_deallocate(config2);
        return 1;
    }

    // Merge configuration from config2 into config1
    ConfigIterator it = config_get_iterator(config2);
    const char *section, *key, *value;

    while (config_next_entry(&it, &section, &key, &value)) 
    {
        if (!config_has_key(config1, section, key)) 
            config_set_value(config1, section, key, value); // If the key does not exist in config1, add it
    }

    config_save(config1, "sources/merged_config.ini");

    config_deallocate(config1);
    config_deallocate(config2);
    
    return 0;
}
