#include "log.h"
#include "../file_io/file_writer.h"
#include "../file_io/file_reader.h"
#include "../string/string.h"
#include <string.h>
#include <time.h> 
#include <stdlib.h>

static const char* log_level_to_string(LogLevel level) {
    switch (level) {
        case LOG_LEVEL_DEBUG: 
            return "DEBUG";
        case LOG_LEVEL_INFO: 
            return "INFO";
        case LOG_LEVEL_WARN: 
            return "WARN";
        case LOG_LEVEL_ERROR: 
            return "ERROR";
        case LOG_LEVEL_FATAL: 
            return "FATAL";
        default: return "UNKNOWN";
    }
}

Log* log_init() {
    Log *config = (Log*) malloc(sizeof(Log));

    if (!config) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Memory allocation failed for config in log_init.\n");
        #endif 
        return NULL;
    }
    
    config->level = LOG_LEVEL_DEBUG;
    config->output = LOG_OUTPUT_BOTH;
    config->file_writer = file_writer_open("log.txt", WRITE_TEXT);

    if (!config->file_writer) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Failed to open log file. Defaulting to console output in log_init.\n");
        #endif 
        config->output = LOG_OUTPUT_CONSOLE;
    }

    config->file_reader = NULL;
    config->enable_timestamp = true;
    config->enable_log_level = true;
    config->suspended = false;

    // Initialize keyword filtering
    config->keyword_filter[0] = '\0'; 
    config->is_keyword_filter_enabled = false; 
    strncpy(config->format, "%s [%s] - %s", sizeof(config->format));

    for (int i = 0; i <= LOG_LEVEL_FATAL; i++) {
        config->level_visibility[i] = true; // Initially, all log levels are visible
    }
    config->custom_filter = NULL;
    config->custom_filter_user_data = NULL;
    config->rate_limit_interval = 0; // Disabled by default.
    config->rate_limit_count = 0; // No limit by default.
    memset(config->log_counts, 0, sizeof(config->log_counts));
    config->last_reset_time = time(NULL);
    
    #ifdef LOG_ENABLE_LOGGING
        fmt_fprintf(stdout, "Info: Logging system initialized in log_init.\n");
    #endif 

    return config;
}

bool log_set_output(Log* config, LogOutput output) {
    if (!config) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: log object is null and invalid.\n");
        #endif 
        return false;
    }
    
    config->output = output;
    if (output == LOG_OUTPUT_FILE && !config->file_writer) {
        // Attempt to open log file if not already open
        config->file_writer = file_writer_open("log.txt", WRITE_TEXT);
        if (!config->file_writer) {
            config->output = LOG_OUTPUT_CONSOLE;
        }

        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Success: LogOutput set successfully.\n");
        #endif
        return true;
    }
    #ifdef LOG_ENABLE_LOGGING
        fmt_fprintf(stderr, "Error: LogOutput not set.\n");
    #endif
    return false;
}

bool log_enable_timestamp(Log* config, bool enable) {
    if (!config) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: log configuration object is null in log_enable_timestamp.\n");
        #endif
        return false;
    }
    config->enable_timestamp = enable;
    return true;
}

void log_message(Log* config, LogLevel level, const char* message, ...) {
    if (!config) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Log configuration is null.\n");
        #endif
        return;
    }
    if (config->suspended) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Logging is currently suspended.\n");
        #endif
        return;
    }
    if (level < config->level) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Current log level (%d) is higher than the message log level (%d); message not logged.\n", config->level, level);
        #endif
        return;
    }
    if (!config->level_visibility[level]) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Skip logging log level is currently not visible in log_message.\n");
        #endif 
        return; // Skip logging if this level is currently not visible
    }
    
    // Check for rate limiting
    time_t currentTime = time(NULL);
    if (currentTime - config->last_reset_time >= config->rate_limit_interval) {
        // Reset rate limit counters
        memset(config->log_counts, 0, sizeof(config->log_counts));
        config->last_reset_time = currentTime;
    }
    if (config->rate_limit_count > 0 && config->log_counts[level] >= config->rate_limit_count) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Rate limit exceeded for this level, skip logging in log_message.\n");
        #endif 
        return;
    }
    // Increment log count for this level
    config->log_counts[level]++;

    char formatted_message[1024];
    char timestamp[64] = "";
    char log_buffer[2048];
    const char* level_str = log_level_to_string(level);

    // Generate timestamp if enabled
    if (config->enable_timestamp) {
        time_t now = time(NULL);
        struct tm* tm_info = localtime(&now);
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
    }

    va_list args;
    va_start(args, message);
    vsnprintf(formatted_message, sizeof(formatted_message), message, args);
    va_end(args);

    // Keyword filtering
    if (config->is_keyword_filter_enabled && strstr(formatted_message, config->keyword_filter) == NULL) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Message does not contain the keyword filter; not logged.\n");
        #endif
        return;
    }

    // Apply custom format
    int written = snprintf(log_buffer, sizeof(log_buffer), config->format, timestamp, level_str, formatted_message);
    if (written < 0 || written >= (int)sizeof(log_buffer)) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Failed to format log message correctly.\n");
        #endif
        return;
    }
    else if(config->custom_filter && !config->custom_filter(level, formatted_message, config->custom_filter_user_data)) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: logging Skipped because of custom filter problem in in log_message.\n");
        #endif
        return;
    }

    // Log to the specified output
    if (config->output == LOG_OUTPUT_CONSOLE || config->output == LOG_OUTPUT_BOTH) {
        fmt_fprintf(stdout, "%s\n", log_buffer);
    }
    if ((config->output == LOG_OUTPUT_FILE || config->output == LOG_OUTPUT_BOTH) && config->file_writer) {
        fmt_fprintf(config->file_writer->file_writer, "%s\n", log_buffer);
    }
}

void log_deallocate(Log* config) {
    if (!config) { 
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Log configuration is null in log_deallocate.\n");
        #endif
        return; 
    }

    // Close the file writer if it's open
    if (config->file_writer) {
        file_writer_close(config->file_writer);
        config->file_writer = NULL; // Avoid dangling pointer
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Success: file_writer of Log now is free in log_deallocate.\n");
        #endif
    }

    // if you had allocated memory for `file_reader`
    if (config->file_reader) {
        file_reader_close(config->file_reader);
        config->file_reader = NULL;
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Success: file_reader of Log now is free in log_deallocate.\n");
        #endif
    }

    free(config);

    #ifdef LOG_ENABLE_LOGGING
        fmt_fprintf(stderr, "Success: Log Object now is free in log_deallocate.\n");
    #endif
}

bool log_set_log_level(Log* config, LogLevel newLevel) {
    if (!config) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Log configuration object is null and invalid in log_set_log_level.\n");
        #endif 
        return false;
    }
    if (newLevel < LOG_LEVEL_DEBUG || newLevel > LOG_LEVEL_FATAL) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Invalid log level specified in log_set_log_level.\n");
        #endif 
        return false;
    }

    config->level = newLevel;

    #ifdef LOG_ENABLE_LOGGING
        fmt_fprintf(stdout, "Info: Log Level chaged to %s in log_set_log_level.\n", log_level_to_string(config->level));
    #endif 
    return true;
}

bool log_enable_keyword_filter(Log* config, const char* keyword, bool enable) {
    if (!config) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Log configuration object is null in log_enable_keyword_filter.\n");
        #endif
        return false;
    }
    if (enable) {
        if (keyword == NULL || strlen(keyword) == 0) {
            #ifdef LOG_ENABLE_LOGGING
                fmt_fprintf(stderr, "Error: Invalid keyword specified in log_enable_keyword_filter.\n");
            #endif
            return false;
        }
        
        strncpy(config->keyword_filter, keyword, sizeof(config->keyword_filter) - 1);
        config->keyword_filter[sizeof(config->keyword_filter) - 1] = '\0'; // Ensure null termination
        config->is_keyword_filter_enabled = true;
    } 
    else {
        config->is_keyword_filter_enabled = false;
    }

    #ifdef LOG_ENABLE_LOGGING
        if (enable) {
            fmt_fprintf(stdout, "Info: Keyword filtering enabled for '%s' in log_enable_keyword_filter.\n", keyword);
        } 
        else {
            fmt_fprintf(stdout, "Info: Keyword filtering disabled in log_enable_keyword_filter.\n");
        }
    #endif
    
    return true;
}

bool log_update_keyword_filter(Log* config, const char* newKeyword) {
    if (!config) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Log configuration object is null in log_update_keyword_filter.\n");
        #endif
        return false;
    }
    if (newKeyword && strlen(newKeyword) > 0 && strlen(newKeyword) < MAX_KEYWORD_LENGTH) {
        strncpy(config->keyword_filter, newKeyword, MAX_KEYWORD_LENGTH);
        config->keyword_filter[MAX_KEYWORD_LENGTH - 1] = '\0'; // Ensure null-termination
        config->is_keyword_filter_enabled = true;
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stdout, "Info: Keyword filter updated to '%s' in log_update_keyword_filter.\n", newKeyword);
        #endif
    } 
    else if (newKeyword && strlen(newKeyword) == 0) {
        // Disable keyword filtering if an empty string is passed
        config->is_keyword_filter_enabled = false;
        config->keyword_filter[0] = '\0';
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stdout, "Info: Keyword filter disabled in log_update_keyword_filter.\n");
        #endif
    } 
    else {
        // Handle invalid keyword length
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Invalid keyword length in log_update_keyword_filter.\n");
        #endif
        return false;
    }

    return true;
}

bool log_set_file_path(Log* config, const char* newFilePath) {
    if (!config) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Log configuration object is null in log_set_file_path.\n");
        #endif
        return false;
    }

    // Close the current file writer if it's open
    if (config->file_writer) {
        file_writer_close(config->file_writer);
        config->file_writer = NULL;
    }

    // If the new file path is NULL or an empty string, stop here
    if (!newFilePath || strlen(newFilePath) == 0) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Invalid new file path in log_set_file_path.\n");
        #endif
        return false;
    }

    // Attempt to open the new log file
    config->file_writer = file_writer_open(newFilePath, WRITE_TEXT);
    if (!config->file_writer) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Failed to open new log file in log_set_file_path.\n");
        #endif
        // Optionally, revert to console logging if file opening fails
        config->output = LOG_OUTPUT_CONSOLE;
        return false;
    }

    #ifdef LOG_ENABLE_LOGGING
        fmt_fprintf(stdout, "Info: Log file path updated to '%s' in log_set_file_path.\n", newFilePath);
    #endif

    return true;
}

void log_flush(Log* config) {
    if (!config) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Log configuration object is null in log_flush.\n");
        #endif
        return;
    }

    // Flush the log file if logging to a file
    if ((config->output == LOG_OUTPUT_FILE || config->output == LOG_OUTPUT_BOTH) && config->file_writer) {
        fflush(config->file_writer->file_writer);
    }

    // For console output, stdout is typically used
    if (config->output == LOG_OUTPUT_CONSOLE || config->output == LOG_OUTPUT_BOTH) {
        fflush(stdout);
    }

    #ifdef LOG_ENABLE_LOGGING
        fmt_fprintf(stdout, "Info: Log buffer flushed in log_flush.\n");
    #endif
}
/*
log_rotate help manage disk space by archiving old logs and starting fresh ones after a log file 
reaches a certain size or after a certain period.
This prevents log files from growing indefinitely and consuming too much disk space
*/
bool log_rotate(Log* config, const char* newLogPath, size_t maxSize) {
    if (!config || !config->file_writer) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Log configuration object is null or file_writer is not initialized in log_rotate.\n");
        #endif
        return false;
    }

    // Get the current size of the log file
    long fileSize = file_writer_get_size(config->file_writer);
    if (fileSize < 0) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Failed to get log file size in log_rotate.\n");
        #endif
        return false;
    }

    // Rotate the log if it exceeds the specified max size
    if ((size_t)fileSize >= maxSize) {
        // Close the current log file
        file_writer_close(config->file_writer);
        config->file_writer = NULL;

        // Optionally, rename or move the current log file before opening a new one
        if (rename("log.txt", newLogPath) != 0) {
            #ifdef LOG_ENABLE_LOGGING
                fmt_fprintf(stderr, "Error: Failed to rename log file in log_rotate.\n");
            #endif
            return false;
        }

        // Open a new log file
        config->file_writer = file_writer_open("log.txt", WRITE_TEXT);
        if (!config->file_writer) {
            #ifdef LOG_ENABLE_LOGGING
                fmt_fprintf(stderr, "Error: Failed to open new log file in log_rotate.\n");
            #endif
            return false;
        }

        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stdout, "Info: Log rotated successfully in log_rotate.\n");
        #endif
    }

    return true;
}

void log_suspend(Log* config) {
    if (!config) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Log configuration object is null in log_suspend.\n");
        #endif
        return;
    }
    config->suspended = true;
    #ifdef LOG_ENABLE_LOGGING
        fmt_fprintf(stdout, "Info: Logging suspended.\n");
    #endif
}

void log_resume(Log* config) {
    if (!config) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Log configuration object is null in log_resume.\n");
        #endif
        return;
    }
    config->suspended = false;
    #ifdef LOG_ENABLE_LOGGING
        fmt_fprintf(stdout, "Info: Logging resumed.\n");
    #endif
}

bool log_set_format(Log* config, const char* format) {
    if (!config || !format) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Invalid arguments to log_set_format.\n");
        #endif
        return false;
    }
    
    strncpy(config->format, format, sizeof(config->format) - 1);
    config->format[sizeof(config->format) - 1] = '\0'; // Ensure null termination
    
    #ifdef LOG_ENABLE_LOGGING
        fmt_fprintf(stdout, "Info: Log format updated.\n");
    #endif
    return true;
}

/*
This function would enable or disable logging for specific log levels dynamically. For instance, 
you might want to temporarily disable DEBUG logs without affecting the visibility of INFO, WARN, or ERROR logs
*/
bool log_toggle_level_visibility(Log* config, LogLevel level, bool visible) {
    if (!config) {
        #ifdef LOG_ENABLE_LOGGING
        fmt_fprintf(stderr, "Error: Log configuration object is null in log_toggle_level_visibility.\n");
        #endif
        return false;
    }

    if (level < 0 || level > LOG_LEVEL_FATAL) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Invalid log level specified in log_toggle_level_visibility.\n");
        #endif
        return false;
    }

    config->level_visibility[level] = visible;

    #ifdef LOG_ENABLE_LOGGING
    fmt_fprintf(stdout, "Info: Visibility for log level %s is now %s.\n",
                log_level_to_string(level), visible ? "enabled" : "disabled");
    #endif

    return true;
}

/*
this function redirect log output dynamically to a different file without restarting the application. 
This can be particularly useful for long-running applications where you want to change the log file based on time
or based on a specific event without losing log messages or needing to restart the application
*/
bool log_redirect_output(Log* config, const char* newFilePath) {
    if (!config) {
        #ifdef LOG_ENABLE_LOGGING
        fmt_fprintf(stderr, "Error: Log configuration object is null in log_redirect_output.\n");
        #endif
        return false;
    }
    if (!newFilePath || strlen(newFilePath) == 0) {
        #ifdef LOG_ENABLE_LOGGING
        fmt_fprintf(stderr, "Error: Invalid new file path in log_redirect_output.\n");
        #endif
        return false;
    }

    // Close the current log file if it's open
    if (config->file_writer) {
        file_writer_close(config->file_writer);
        config->file_writer = NULL;
    }

    // Open the new log file
    config->file_writer = file_writer_open(newFilePath, WRITE_TEXT);
    if (!config->file_writer) {
        #ifdef LOG_ENABLE_LOGGING
        fmt_fprintf(stderr, "Error: Failed to open new log file in log_redirect_output.\n");
        #endif
        return false;
    }

    #ifdef LOG_ENABLE_LOGGING
    fmt_fprintf(stdout, "Info: Log output redirected to '%s'.\n", newFilePath);
    #endif

    return true;
}

/*
This function enables or disables verbose logging dynamically. 
When verbose logging is enabled, all log levels are enabled (including DEBUG and INFO). 
When it is disabled, it could limit the logging to higher severities (WARN, ERROR, FATAL).
*/
bool log_set_verbose(Log* config, bool verbose) {
    if (!config) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Log configuration object is null in log_set_verbose.\n");
        #endif
        return false;
    }

    // Enable or disable verbose logging
    config->level_visibility[LOG_LEVEL_DEBUG] = verbose;
    config->level_visibility[LOG_LEVEL_INFO] = verbose;

    #ifdef LOG_ENABLE_LOGGING
        fmt_fprintf(stdout, "Info: Verbose logging is now %s.\n", verbose ? "enabled" : "disabled");
    #endif

    return true;
}

bool log_set_custom_filter(Log* config, LogFilterFunction filter, void* user_data) {
    if (!config) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Log configuration object is null in log_set_custom_filter.\n");
        #endif
        return false;
    }

    config->custom_filter = filter;
    config->custom_filter_user_data = user_data;

    #ifdef LOG_ENABLE_LOGGING
        fmt_fprintf(stdout, "Info: Custom log filter set successfully.\n");
    #endif

    return true;
}

bool log_set_max_file_size(Log* config, size_t maxSize, const char* archivePathFormat) {
    if (!config || !config->file_writer || maxSize == 0) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Invalid parameters in log_set_max_file_size.\n");
        #endif
        return false;
    }

    // Check current log file size
    long fileSize = file_writer_get_size(config->file_writer);
    if (fileSize < 0) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Could not retrieve file size in log_set_max_file_size.\n");
        #endif
        return false;
    }

    if ((size_t)fileSize >= maxSize) {
        char archivePath[1024];
        time_t now = time(NULL);
        struct tm* tm_info = localtime(&now);
        strftime(archivePath, sizeof(archivePath), archivePathFormat, tm_info);

        // Close current log file
        file_writer_close(config->file_writer);
        
        // Archive current log file
        if (rename("log.txt", archivePath) != 0) {
            #ifdef LOG_ENABLE_LOGGING
                fmt_fprintf(stderr, "Error: Could not archive log file in log_set_max_file_size.\n");
            #endif
            return false;
        }

        // Open a new log file
        config->file_writer = file_writer_open("log.txt", WRITE_TEXT);
        if (!config->file_writer) {
            #ifdef LOG_ENABLE_LOGGING
                fmt_fprintf(stderr, "Error: Could not open new log file in log_set_max_file_size.\n");
            #endif
            return false;
        }

        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stdout, "Info: Log file archived and new log file started.\n");
        #endif
    }

    return true;
}
