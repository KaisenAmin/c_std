/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Log
*/

#include <string.h>
#include <time.h> 
#include <stdlib.h>
#include "log.h"
#include "../string/std_string.h"


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

/**
 * @brief Initializes the logging system and returns a configured `Log` object.
 *
 * This function sets up the logging system by allocating memory for the `Log` configuration object
 * and initializing it with default settings. It attempts to open a log file named "log.txt" for 
 * writing. If the file cannot be opened, it defaults to console output. The function also 
 * initializes various logging options, including keyword filtering, log level visibility, and rate limiting.
 *
 * @return A pointer to the initialized `Log` object, or NULL if memory allocation fails.
 */
Log* log_init() {
    Log *config = (Log*) malloc(sizeof(Log));

    if (!config) {
        LOG_LOG("[log_init] Error: Memory allocation failed for config.");
        exit(-1);
    }
    
    config->level = LOG_LEVEL_DEBUG;
    config->output = LOG_OUTPUT_BOTH;
    config->file_writer = file_writer_open("log.txt", WRITE_TEXT);

    if (!config->file_writer) {
        LOG_LOG("[log_init] Error: Failed to open log file. Defaulting to console output.");
        config->output = LOG_OUTPUT_CONSOLE;
    }

    config->file_reader = NULL;
    config->enable_timestamp = false;
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
    
    LOG_LOG("[log_init] Info: Logging system initialized.");
    return config;
}

/**
 * @brief Sets the output destination for log messages.
 *
 * This function configures where the log messages will be output: to the console, a file, or both. 
 * If `LOG_OUTPUT_FILE` is selected but the log file cannot be opened, the function will 
 * automatically default to console output.
 *
 * @param config Pointer to the `Log` configuration object.
 * @param output The desired log output destination, specified as `LOG_OUTPUT_CONSOLE`, 
 * `LOG_OUTPUT_FILE`, or `LOG_OUTPUT_BOTH`.
 * 
 * @return `true` if the output was set successfully, or `false` if an error occurred.
 */
bool log_set_output(Log* config, LogOutput output) {
    if (!config) {
        LOG_LOG("[log_set_output] Error: Log configuration object is null and invalid.");
        return false;
    }
    
    config->output = output;
    if (output == LOG_OUTPUT_FILE && !config->file_writer) {
        // Attempt to open log file if not already open
        config->file_writer = file_writer_open("log.txt", WRITE_TEXT);
        if (!config->file_writer) {
            config->output = LOG_OUTPUT_CONSOLE;
            LOG_LOG("[log_set_output] Error: Failed to open log file, reverting to console output.");
            return false;
        }
        LOG_LOG("[log_set_output] Success: Log output set to file.");
    } 
    else {
        LOG_LOG("[log_set_output] Success: Log output set successfully.");
    }
    
    return true;
}

/**
 * @brief Enables or disables timestamps in log messages.
 *
 * This function controls whether timestamps are included in log messages. When enabled, 
 * each log message will be prefixed with the current date and time.
 *
 * @param config Pointer to the `Log` configuration object.
 * @param enable `true` to enable timestamps, `false` to disable them.
 * 
 * @return `true` if the operation was successful, or `false` if the configuration object is NULL.
 */
bool log_enable_timestamp(Log* config, bool enable) {
    if (!config) {
        LOG_LOG("[log_enable_timestamp] Error: Log configuration object is null.");
        return false;
    }
    config->enable_timestamp = enable;
    LOG_LOG("[log_enable_timestamp] Info: Timestamps are now %s.", enable ? "enabled" : "disabled");
    return true;
}

/**
 * @brief Logs a message at a specified log level.
 *
 * This function logs a message with a specified log level. The message will be formatted
 * according to the configuration in the `Log` object, including any enabled timestamp, 
 * keyword filtering, and custom formatting. If the log level is below the configured log level, 
 * or if the level is not visible due to dynamic visibility settings, the message will not be logged.
 * Additionally, this function respects rate-limiting settings, if configured.
 *
 * @param config Pointer to the `Log` configuration object.
 * @param level The log level of the message (e.g., `LOG_LEVEL_DEBUG`, `LOG_LEVEL_INFO`, etc.).
 * @param message The message to be logged, specified as a format string (similar to `printf`).
 * @param ... Additional arguments for the format string.
 */
void log_message(Log* config, LogLevel level, const char* message, ...) {
    if (!config) {
        LOG_LOG("[log_message] Error: Log configuration is null.");
        return;
    }
    if (config->suspended) {
        LOG_LOG("[log_message] Info: Logging is currently suspended.");
        return;
    }
    if (level < config->level) {
        LOG_LOG("[log_message] Info: Current log level (%d) is higher than the message log level (%d); message not logged.", config->level, level);
        return;
    }
    if (!config->level_visibility[level]) {
        LOG_LOG("[log_message] Info: Log level is currently not visible.");
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
        LOG_LOG("[log_message] Info: Rate limit exceeded for this level, message skipped.");
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
        LOG_LOG("[log_message] Info: Message does not contain the keyword filter; not logged.");
        return;
    }

    // Apply custom format
    int written = snprintf(log_buffer, sizeof(log_buffer), config->format, timestamp, level_str, formatted_message);
    if (written < 0 || written >= (int)sizeof(log_buffer)) {
        LOG_LOG("[log_message] Error: Failed to format log message correctly.");
        return;
    }
    else if(config->custom_filter && !config->custom_filter(level, formatted_message, config->custom_filter_user_data)) {
        LOG_LOG("[log_message] Info: Logging skipped due to custom filter.");
        return;
    }

    // Log to the specified output
    if (config->output == LOG_OUTPUT_CONSOLE || config->output == LOG_OUTPUT_BOTH) {
        fprintf(stdout, "%s\n", log_buffer);
    }
    if ((config->output == LOG_OUTPUT_FILE || config->output == LOG_OUTPUT_BOTH) && config->file_writer) {
        fprintf(config->file_writer->file_writer, "%s\n", log_buffer);
    }
}

/**
 * @brief Deallocates the log configuration and associated resources.
 *
 * This function frees all resources associated with the `Log` object, including any open
 * file writer or file reader. It is important to call this function to prevent memory leaks 
 * when the logging system is no longer needed.
 *
 * @param config Pointer to the `Log` configuration object to be deallocated.
 */
void log_deallocate(Log* config) {
    if (!config) { 
        LOG_LOG("[log_deallocate] Error: Log configuration is null.");
        return; 
    }
    if (config->file_writer) {
        file_writer_close(config->file_writer);
        config->file_writer = NULL; // Avoid dangling pointer
        LOG_LOG("[log_deallocate] Success: file_writer of Log is now free.");
    }
    if (config->file_reader) {
        file_reader_close(config->file_reader);
        config->file_reader = NULL;
        LOG_LOG("[log_deallocate] Success: file_reader of Log is now free.");
    }

    free(config);
    LOG_LOG("[log_deallocate] Success: Log object is now free.");
}


/**
 * @brief Sets the logging level for the logger.
 *
 * This function updates the logging level for the logger, allowing control over which
 * messages are logged. Only messages with a log level greater than or equal to the specified 
 * level will be logged.
 *
 * @param config Pointer to the `Log` configuration object.
 * @param newLevel The new logging level to be set. Must be one of `LOG_LEVEL_DEBUG`, 
 * `LOG_LEVEL_INFO`, `LOG_LEVEL_WARN`, `LOG_LEVEL_ERROR`, or `LOG_LEVEL_FATAL`.
 *
 * @return `true` if the log level was successfully updated, `false` if the `config` is `NULL` or `newLevel` is invalid.
 */
bool log_set_log_level(Log* config, LogLevel newLevel) {
    if (!config) {
        LOG_LOG("[log_set_log_level] Error: Log configuration object is null and invalid.");
        return false;
    }
    if (newLevel < LOG_LEVEL_DEBUG || newLevel > LOG_LEVEL_FATAL) {
        LOG_LOG("[log_set_log_level] Error: Invalid log level specified.");
        return false;
    }
    config->level = newLevel;

    LOG_LOG("[log_set_log_level] Info: Log Level changed to %s.", log_level_to_string(config->level));
    return true;
}

/**
 * @brief Enables or disables keyword-based filtering for log messages.
 *
 * This function allows for filtering log messages based on a specific keyword. When enabled, 
 * only messages containing the specified keyword will be logged. If the keyword is `NULL` or an empty 
 * string, the operation fails.
 *
 * @param config Pointer to the `Log` configuration object.
 * @param keyword The keyword to filter log messages. Must not be `NULL` or empty when enabling filtering.
 * @param enable `true` to enable keyword filtering, `false` to disable it.
 *
 * @return `true` if keyword filtering was successfully enabled or disabled,
 * `false` if the `config` is `NULL`, or the `keyword` is invalid when enabling filtering.
 */
bool log_enable_keyword_filter(Log* config, const char* keyword, bool enable) {
    if (!config) {
        LOG_LOG("[log_enable_keyword_filter] Error: Log configuration object is null.");
        return false;
    }
    if (enable) {
        if (keyword == NULL || strlen(keyword) == 0) {
            LOG_LOG("[log_enable_keyword_filter] Error: Invalid keyword specified.");
            return false;
        }
        
        strncpy(config->keyword_filter, keyword, sizeof(config->keyword_filter) - 1);
        config->keyword_filter[sizeof(config->keyword_filter) - 1] = '\0'; // Ensure null termination
        config->is_keyword_filter_enabled = true;

        LOG_LOG("[log_enable_keyword_filter] Info: Keyword filtering enabled for '%s'.", keyword);
    } 
    else {
        config->is_keyword_filter_enabled = false;
        LOG_LOG("[log_enable_keyword_filter] Info: Keyword filtering disabled.");
    }

    return true;
}


/**
 * @brief Updates the keyword used for filtering log messages.
 *
 * This function updates the keyword used for filtering log messages. If the new keyword is 
 * an empty string, keyword filtering will be disabled. The new keyword must be shorter than the 
 * maximum allowed keyword length.
 *
 * @param config Pointer to the `Log` configuration object.
 * @param newKeyword The new keyword to filter log messages. If an empty string is provided, keyword filtering is disabled.
 *
 * @return `true` if the keyword filter was successfully updated or disabled, `false` if the `config` is `NULL`, or the `newKeyword` is invalid.
 */
bool log_update_keyword_filter(Log* config, const char* newKeyword) {
    if (!config) {
        LOG_LOG("[log_update_keyword_filter] Error: Log configuration object is null.");
        return false;
    }
    if (newKeyword && strlen(newKeyword) > 0 && strlen(newKeyword) < MAX_KEYWORD_LENGTH) {
        strncpy(config->keyword_filter, newKeyword, MAX_KEYWORD_LENGTH);
        config->keyword_filter[MAX_KEYWORD_LENGTH - 1] = '\0'; // Ensure null-termination
        config->is_keyword_filter_enabled = true;
        LOG_LOG("[log_update_keyword_filter] Info: Keyword filter updated to '%s'.", newKeyword);
    } 
    else if (newKeyword && strlen(newKeyword) == 0) {
        // Disable keyword filtering if an empty string is passed
        config->is_keyword_filter_enabled = false;
        config->keyword_filter[0] = '\0';
        LOG_LOG("[log_update_keyword_filter] Info: Keyword filter disabled.");
    } 
    else {
        LOG_LOG("[log_update_keyword_filter] Error: Invalid keyword length.");
        return false;
    }

    return true;
}

/**
 * @brief Sets the file path for the log output.
 *
 * This function changes the log file to the specified path. It closes the current log file 
 * (if open) and attempts to open a new log file at the specified path.
 *
 * @param config Pointer to the `Log` configuration object.
 * @param newFilePath The path to the new log file. If `newFilePath` is `NULL` or an empty string, the operation fails.
 *
 * @return `true` if the log file path was successfully updated, `false` if the `config` is `NULL`, `newFilePath` is invalid, or if the new file could not be opened.
 */
bool log_set_file_path(Log* config, const char* newFilePath) {
    if (!config) {
        LOG_LOG("[log_set_file_path] Error: Log configuration object is null.");
        return false;
    }

    // Close the current file writer if it's open
    if (config->file_writer) {
        file_writer_close(config->file_writer);
        config->file_writer = NULL;
    }

    // If the new file path is NULL or an empty string, stop here
    if (!newFilePath || strlen(newFilePath) == 0) {
        LOG_LOG("[log_set_file_path] Error: Invalid new file path.");
        return false;
    }

    // Attempt to open the new log file
    config->file_writer = file_writer_open(newFilePath, WRITE_TEXT);
    if (!config->file_writer) {
        LOG_LOG("[log_set_file_path] Error: Failed to open new log file.");
        config->output = LOG_OUTPUT_CONSOLE; // Optionally, revert to console logging if file opening fails
        return false;
    }

    LOG_LOG("[log_set_file_path] Info: Log file path updated to '%s'.", newFilePath);
    return true;
}

/**
 * @brief Flushes the log output buffer.
 *
 * This function ensures that any buffered log messages are immediately written 
 * to their respective output destinations (console, file, or both).
 *
 * @param config Pointer to the `Log` configuration object.
 */
void log_flush(Log* config) {
    if (!config) {
        LOG_LOG("[log_flush] Error: Log configuration object is null.");
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

    LOG_LOG("[log_flush] Info: Log buffer flushed.");
}

/**
 * @brief Rotates the log file when it reaches a specified maximum size.
 *
 * This function helps manage disk space by archiving the current log file and starting 
 * a new one once the specified size limit is reached. The old log file is renamed or moved 
 * according to the `newLogPath` parameter.
 *
 * @param config Pointer to the `Log` configuration object.
 * @param newLogPath The path to rename or move the current log file when rotation occurs.
 * @param maxSize The maximum allowed size of the log file in bytes. When the file reaches this size, rotation occurs.
 *
 * @return `true` if the log was successfully rotated, `false` if the `config` is `NULL`, the `file_writer` is not initialized, 
 * or if there was an error during the rotation process.
 */
bool log_rotate(Log* config, const char* newLogPath, size_t maxSize) {
    if (!config || !config->file_writer) {
        LOG_LOG("[log_rotate] Error: Log configuration object is null or file_writer is not initialized.");
        return false;
    }

    // Get the current size of the log file
    long fileSize = file_writer_get_size(config->file_writer);
    if (fileSize < 0) {
        LOG_LOG("[log_rotate] Error: Failed to get log file size.");
        return false;
    }

    // Rotate the log if it exceeds the specified max size
    if ((size_t)fileSize >= maxSize) {
        // Close the current log file
        file_writer_close(config->file_writer);
        config->file_writer = NULL;

        // Optionally, rename or move the current log file before opening a new one
        if (rename("log.txt", newLogPath) != 0) {
            LOG_LOG("[log_rotate] Error: Failed to rename log file.");
            return false;
        }

        config->file_writer = file_writer_open("log.txt", WRITE_TEXT);
        if (!config->file_writer) {
            LOG_LOG("[log_rotate] Error: Failed to open new log file.");
            return false;
        }
        LOG_LOG("[log_rotate] Info: Log rotated successfully.");
    }

    return true;
}


/**
 * @brief Suspends logging temporarily.
 *
 * This function disables logging, so no messages will be logged until logging is resumed.
 *
 * @param config Pointer to the `Log` configuration object.
 */
void log_suspend(Log* config) {
    if (!config) {
        LOG_LOG("[log_suspend] Error: Log configuration object is null.");
        return;
    }
    config->suspended = true;
    LOG_LOG("[log_suspend] Info: Logging suspended.");
}

/**
 * @brief Resumes logging after it has been suspended.
 *
 * This function re-enables logging if it was previously suspended, allowing log messages to be recorded again.
 *
 * @param config Pointer to the `Log` configuration object.
 */
void log_resume(Log* config) {
    if (!config) {
        LOG_LOG("[log_resume] Error: Log configuration object is null.");
        return;
    }
    config->suspended = false;
    LOG_LOG("[log_resume] Info: Logging resumed.");
}

/**
 * @brief Sets a custom format for log messages.
 *
 * This function allows setting a custom format for log messages. The format string should follow
 * standard `printf` formatting rules. The format string should include placeholders for timestamp, 
 * log level, and the message text.
 *
 * Example format: `[%s] [%s] - %s` (corresponds to `[Timestamp] [LogLevel] - Message`)
 *
 * @param config Pointer to the `Log` configuration object.
 * @param format The format string to be used for log messages.
 *
 * @return `true` if the format was successfully set, `false` if the `config` or `format` parameter is `NULL`.
 */
bool log_set_format(Log* config, const char* format) {
    if (!config || !format) {
        LOG_LOG("[log_set_format] Error: Invalid arguments.");
        return false;
    }
    
    strncpy(config->format, format, sizeof(config->format) - 1);
    config->format[sizeof(config->format) - 1] = '\0'; 
    
    LOG_LOG("[log_set_format] Info: Log format updated.");
    return true;
}

/**
 * @brief Toggles the visibility of specific log levels.
 *
 * This function enables or disables logging for a specific log level dynamically.
 * For instance, you might want to temporarily disable `DEBUG` logs without affecting 
 * the visibility of `INFO`, `WARN`, or `ERROR` logs.
 *
 * @param config Pointer to the `Log` configuration object.
 * @param level The log level to toggle visibility for. This should be one of the values defined in `LogLevel`.
 * @param visible `true` to enable visibility for the specified log level, `false` to disable visibility for that log level.
 *
 * @return `true` if the visibility was successfully toggled, `false` if the `config` parameter is `NULL` or if an invalid log level is specified.
 */
bool log_toggle_level_visibility(Log* config, LogLevel level, bool visible) {
    if (!config) {
        LOG_LOG("[log_toggle_level_visibility] Error: Log configuration object is null.");
        return false;
    }
    if (level < 0 || level > LOG_LEVEL_FATAL) {
        LOG_LOG("[log_toggle_level_visibility] Error: Invalid log level specified.");
        return false;
    }

    config->level_visibility[level] = visible;

    LOG_LOG("[log_toggle_level_visibility] Info: Visibility for log level %s is now %s.",
            log_level_to_string(level), visible ? "enabled" : "disabled");

    return true;
}

/**
 * @brief Redirects log output to a new file without restarting the application.
 *
 * This function allows dynamically changing the log file during runtime, which is useful 
 * for long-running applications where you may want to switch log files based on time, 
 * events, or other criteria without losing log messages or needing to restart the application.
 *
 * @param config Pointer to the `Log` configuration object.
 * @param newFilePath The path to the new log file where future log messages should be written. 
 * This path should be a valid file path, and the file will be created if it does not exist.
 *
 * @return `true` if the log output was successfully redirected to the new file, `false` if the `config` 
 * parameter is `NULL`, `newFilePath` is invalid, or if an error occurs while opening the new file.
 */
bool log_redirect_output(Log* config, const char* newFilePath) {
    if (!config) {
        LOG_LOG("[log_redirect_output] Error: Log configuration object is null.");
        return false;
    }
    if (!newFilePath || strlen(newFilePath) == 0) {
        LOG_LOG("[log_redirect_output] Error: Invalid new file path.");
        return false;
    }
    if (config->file_writer) { // Close the current log file if it's open
        file_writer_close(config->file_writer);
        config->file_writer = NULL;
    }

    // Open the new log file
    config->file_writer = file_writer_open(newFilePath, WRITE_TEXT);
    if (!config->file_writer) {
        LOG_LOG("[log_redirect_output] Error: Failed to open new log file.");
        return false;
    }

    LOG_LOG("[log_redirect_output] Info: Log output redirected to '%s'.", newFilePath);
    return true;
}

/**
 * @brief Enables or disables verbose logging.
 *
 * When verbose logging is enabled, all log levels, including `DEBUG` and `INFO`, are logged. 
 * When verbose logging is disabled, only higher-severity log levels, such as `WARN`, `ERROR`, and `FATAL`, are logged.
 *
 * @param config Pointer to the `Log` configuration object.
 * @param verbose `true` to enable verbose logging (enables `DEBUG` and `INFO` logs), `false` to disable verbose logging (disables `DEBUG` and `INFO` logs).
 *
 * @return `true` if the verbosity was successfully set, `false` if the `config` parameter is `NULL`.
 */
bool log_set_verbose(Log* config, bool verbose) {
    if (!config) {
        LOG_LOG("[log_set_verbose] Error: Log configuration object is null.");
        return false;
    }

    // Enable or disable verbose logging
    config->level_visibility[LOG_LEVEL_DEBUG] = verbose;
    config->level_visibility[LOG_LEVEL_INFO] = verbose;
    LOG_LOG("[log_set_verbose] Info: Verbose logging is now %s.", verbose ? "enabled" : "disabled");

    return true;
}

/**
 * @brief Sets a custom filter function for log messages.
 *
 * This function allows the user to set a custom filter that determines whether 
 * a log message should be recorded based on custom logic. The filter function 
 * is provided by the user and will be called for each log message.
 *
 * @param config Pointer to the `Log` configuration object.
 * @param filter A function pointer to the custom filter function. The filter function should return `true` if the message should be logged, 
 * and `false` if it should be ignored.
 * @param user_data A pointer to user-defined data that will be passed to the filter function each time it is called.
 *
 * @return `true` if the custom filter was set successfully, `false` if the `config` parameter is `NULL`.
 */
bool log_set_custom_filter(Log* config, LogFilterFunction filter, void* user_data) {
    if (!config) {
        LOG_LOG("[log_set_custom_filter] Error: Log configuration object is null.");
        return false;
    }

    config->custom_filter = filter;
    config->custom_filter_user_data = user_data;

    LOG_LOG("[log_set_custom_filter] Info: Custom log filter set successfully.");

    return true;
}

/**
 * @brief Sets a maximum log file size and automatically rotates the log file when this size is exceeded.
 *
 * This function configures the logging system to monitor the size of the current log file. 
 * If the log file exceeds the specified size, the current log file is archived, and a new 
 * log file is started. The archived log file is renamed using the specified format.
 *
 * @param config Pointer to the `Log` configuration object.
 * @param maxSize The maximum size (in bytes) that the log file is allowed to reach before being rotated.
 * @param archivePathFormat A format string used to generate the name of the archived log file. 
 * The format string can include time-based formatting specifiers.
 *
 * @return `true` if the maximum file size was set successfully and log rotation was performed as needed, 
 * `false` if `config` is `NULL`, `config->file_writer` is `NULL`, `maxSize` is zero, or if any error occurs during file operations.
 */
bool log_set_max_file_size(Log* config, size_t maxSize, const char* archivePathFormat) {
    if (!config || !config->file_writer || maxSize == 0) {
        LOG_LOG("[log_set_max_file_size] Error: Invalid parameters.");
        return false;
    }

    // Check current log file size
    long fileSize = file_writer_get_size(config->file_writer);
    if (fileSize < 0) {
        LOG_LOG("[log_set_max_file_size] Error: Could not retrieve file size.");
        return false;
    }

    if ((size_t)fileSize >= maxSize) {
        char archivePath[1024];
        time_t now = time(NULL);
        struct tm* tm_info = localtime(&now);
        strftime(archivePath, sizeof(archivePath), archivePathFormat, tm_info);
        file_writer_close(config->file_writer);
        
        // Archive current log file
        if (rename("log.txt", archivePath) != 0) {
            LOG_LOG("[log_set_max_file_size] Error: Could not archive log file.");
            return false;
        }

        // Open a new log file
        config->file_writer = file_writer_open("log.txt", WRITE_TEXT);
        if (!config->file_writer) {
            LOG_LOG("[log_set_max_file_size] Error: Could not open new log file.");
            return false;
        }
        LOG_LOG("[log_set_max_file_size] Info: Log file archived and new log file started.");
    }

    return true;
}

