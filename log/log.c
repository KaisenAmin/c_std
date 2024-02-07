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

    if (level < config->level) {
        #ifdef LOG_ENABLE_LOGGING
            fmt_fprintf(stderr, "Error: Current log level (%d) is higher than the message log level (%d); message not logged.\n", config->level, level);
        #endif
        return;
    }

    va_list args;
    char formatted_message[1024]; // Use a fixed size buffer for simplicity
    char log_buffer[2048]; // Larger buffer to accommodate timestamps, log level, and message

    // Generate timestamp if enabled
    char timestamp[64] = ""; // Buffer to hold timestamp
    if (config->enable_timestamp) {
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info); // Customize this format as needed
    }

    // Prepare log message with vsnprintf to prevent buffer overflows
    va_start(args, message);
    vsnprintf(formatted_message, sizeof(formatted_message), message, args);
    va_end(args);

    // Construct the final log message with optional log level and timestamp
    const char* level_str = log_level_to_string(level);
    snprintf(log_buffer, sizeof(log_buffer), "%s [%s] %s", timestamp, level_str, formatted_message);

    // Actual logging
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

    // Finally, free the log config itself
    free(config);

    #ifdef LOG_ENABLE_LOGGING
        fmt_fprintf(stderr, "Success: Log Object now is free in log_deallocate.\n");
    #endif
}
