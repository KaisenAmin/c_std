#include "config/config.h"
#include "fmt/fmt.h"

int main() {
    ConfigFile *config = config_create("C:\\Users\\asus\\OneDrive\\Desktop\\project\\c_std\\sources\\config.ini");
    config_set_comment(config, "global", "Global settings for the application"); // Set a comment for a section

    // Check if a section exists
    if (config_has_section(config, "user_preferences")) { 
        fmt_printf("Section 'user_preferences' exists.\n");
    }
    // Check if a key exists in a section
    if (config_has_key(config, "user_preferences", "theme")) {
        fmt_printf("Key 'theme' exists in section 'user_preferences'.\n");
    }

    config_save(config, "C:\\Users\\asus\\OneDrive\\Desktop\\project\\c_std\\sources\\config_with_comments.ini");
    config_deallocate(config);
    return 0;
}