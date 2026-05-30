/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Log
*/

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include "log.h"
#include "../string/std_string.h"



/**
 * @brief Returns the human-readable name of a log level.
 *
 * Maps a `LogLevel` to its uppercase string: `"DEBUG"`, `"INFO"`, `"WARN"`,
 * `"ERROR"` or `"FATAL"`. Any value outside the enum yields `"UNKNOWN"`.
 *
 * The returned pointer is a static string literal — do not free or modify it.
 * This is the inverse of `log_set_level_from_string`.
 *
 * @param level The log level to name.
 * @return A static, NUL-terminated level name.
 */
const char* log_level_to_string(LogLevel level) {
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
        return NULL;  // Project convention: return NULL on OOM, never exit().
    }

    config->level = LOG_LEVEL_DEBUG;
    config->output = LOG_OUTPUT_BOTH;
    config->file_writer = file_writer_open("log.txt", WRITE_TEXT);

    // Track the active log file path so rotation/archive can act on the
    // correct file even after log_set_file_path / log_redirect_output.
    strncpy(config->current_file_path, "log.txt", sizeof(config->current_file_path) - 1);
    config->current_file_path[sizeof(config->current_file_path) - 1] = '\0';

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

    // Default format: "<timestamp> [<level>] - <message>"
    // Use the same safe pattern as log_set_format.
    {
        const char *def = "%s [%s] - %s";
        strncpy(config->format, def, sizeof(config->format) - 1);
        config->format[sizeof(config->format) - 1] = '\0';
    }

    for (int i = 0; i <= LOG_LEVEL_FATAL; i++) {
        config->level_visibility[i] = true; // Initially, all log levels are visible
    }
    config->custom_filter = NULL;
    config->custom_filter_user_data = NULL;
    config->rate_limit_interval = 0; // Disabled by default.
    config->rate_limit_count = 0; // No limit by default.
    memset(config->log_counts, 0, sizeof(config->log_counts));
    memset(config->total_log_counts, 0, sizeof(config->total_log_counts));
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
    
    // Rate limiting: only active when both interval and count are non-zero.
    // Without this guard, an interval of 0 would reset the counters on every
    // call (since currentTime - last_reset_time >= 0 is always true).
    if (config->rate_limit_interval > 0 && config->rate_limit_count > 0) {
        time_t currentTime = time(NULL);
        if ((unsigned int)(currentTime - config->last_reset_time) >= config->rate_limit_interval) {
            // Reset rate limit counters
            memset(config->log_counts, 0, sizeof(config->log_counts));
            config->last_reset_time = currentTime;
        }
        if (config->log_counts[level] >= config->rate_limit_count) {
            LOG_LOG("[log_message] Info: Rate limit exceeded for this level, message skipped.");
            return;
        }
        config->log_counts[level]++;
    }

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

    // The message passed every gate/filter and is about to be emitted: count it.
    config->total_log_counts[level]++;

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
        config->keyword_filter[MAX_KEYWORD_LENGTH - 1] = '\0';
        config->is_keyword_filter_enabled = true;
        LOG_LOG("[log_update_keyword_filter] Info: Keyword filter updated to '%s'.", newKeyword);
    } 
    else if (newKeyword && strlen(newKeyword) == 0) {
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
    if (config->file_writer) {
        file_writer_close(config->file_writer);
        config->file_writer = NULL;
    }

    if (!newFilePath || strlen(newFilePath) == 0) {
        LOG_LOG("[log_set_file_path] Error: Invalid new file path.");
        return false;
    }

    config->file_writer = file_writer_open(newFilePath, WRITE_TEXT);
    if (!config->file_writer) {
        LOG_LOG("[log_set_file_path] Error: Failed to open new log file.");
        config->output = LOG_OUTPUT_CONSOLE; // Optionally, revert to console logging if file opening fails
        return false;
    }

    strncpy(config->current_file_path, newFilePath, sizeof(config->current_file_path) - 1);
    config->current_file_path[sizeof(config->current_file_path) - 1] = '\0';

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
    if ((config->output == LOG_OUTPUT_FILE || config->output == LOG_OUTPUT_BOTH) && config->file_writer) {
        fflush(config->file_writer->file_writer);
    }
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
    if (!config || !config->file_writer || !newLogPath) {
        LOG_LOG("[log_rotate] Error: Log configuration object is null or file_writer is not initialized.");
        return false;
    }

    long fileSize = file_writer_get_size(config->file_writer);
    if (fileSize < 0) {
        LOG_LOG("[log_rotate] Error: Failed to get log file size.");
        return false;
    }

    if ((size_t)fileSize >= maxSize) {
        char source_path[LOG_MAX_PATH];
        strncpy(source_path, config->current_file_path, sizeof(source_path) - 1);
        source_path[sizeof(source_path) - 1] = '\0';

        if (source_path[0] == '\0') {
            strncpy(source_path, "log.txt", sizeof(source_path) - 1);
            source_path[sizeof(source_path) - 1] = '\0';
        }

        file_writer_close(config->file_writer);
        config->file_writer = NULL;

        remove(newLogPath);
        if (rename(source_path, newLogPath) != 0) {
            LOG_LOG("[log_rotate] Error: Failed to rename log file.");
            config->file_writer = file_writer_open(source_path, WRITE_TEXT);
            return false;
        }

        config->file_writer = file_writer_open(source_path, WRITE_TEXT);
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

    LOG_LOG("[log_toggle_level_visibility] Info: Visibility for log level %s is now %s.", log_level_to_string(level), visible ? "enabled" : "disabled");

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
    if (config->file_writer) { 
        file_writer_close(config->file_writer);
        config->file_writer = NULL;
    }

    config->file_writer = file_writer_open(newFilePath, WRITE_TEXT);
    if (!config->file_writer) {
        LOG_LOG("[log_redirect_output] Error: Failed to open new log file.");
        return false;
    }

    // Track the active log file path so rotation/archive uses the right source.
    strncpy(config->current_file_path, newFilePath, sizeof(config->current_file_path) - 1);
    config->current_file_path[sizeof(config->current_file_path) - 1] = '\0';

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
    if (!config || !config->file_writer || maxSize == 0 || !archivePathFormat) {
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
        char source_path[LOG_MAX_PATH];
        strncpy(source_path, config->current_file_path, sizeof(source_path) - 1);
        source_path[sizeof(source_path) - 1] = '\0';

        if (source_path[0] == '\0') {
            strncpy(source_path, "log.txt", sizeof(source_path) - 1);
            source_path[sizeof(source_path) - 1] = '\0';
        }

        char archivePath[1024];
        time_t now = time(NULL);
        struct tm* tm_info = localtime(&now);

        strftime(archivePath, sizeof(archivePath), archivePathFormat, tm_info);
        file_writer_close(config->file_writer);

        config->file_writer = NULL;
        remove(archivePath);

        if (rename(source_path, archivePath) != 0) {
            LOG_LOG("[log_set_max_file_size] Error: Could not archive log file.");
            config->file_writer = file_writer_open(source_path, WRITE_TEXT);
            return false;
        }

        config->file_writer = file_writer_open(source_path, WRITE_TEXT);
        if (!config->file_writer) {
            LOG_LOG("[log_set_max_file_size] Error: Could not open new log file.");
            return false;
        }
        LOG_LOG("[log_set_max_file_size] Info: Log file archived and new log file started.");
    }

    return true;
}


/**
 * @brief Sets the minimum log level from a case-insensitive name.
 *
 * Accepts `"debug"`, `"info"`, `"warn"`, `"error"` or `"fatal"` (any letter
 * case). Handy for driving the level from an environment variable or a config
 * file. This is the inverse of `log_level_to_string`.
 *
 * @param config    The logger. Must not be NULL.
 * @param level_str The level name. Must not be NULL.
 * @return `true` if the name was recognised and the level updated; `false` on
 *         NULL arguments or an unrecognised name (the level is left unchanged).
 */
bool log_set_level_from_string(Log* config, const char* level_str) {
    if (!config || !level_str) {
        LOG_LOG("[log_set_level_from_string] Error: NULL config or level string.");
        return false;
    }

    static const struct { const char* name; LogLevel level; } table[] = {
        { "debug", LOG_LEVEL_DEBUG },
        { "info",  LOG_LEVEL_INFO  },
        { "warn",  LOG_LEVEL_WARN  },
        { "error", LOG_LEVEL_ERROR },
        { "fatal", LOG_LEVEL_FATAL },
    };

    for (size_t i = 0; i < sizeof(table) / sizeof(table[0]); ++i) {
        const char* b = table[i].name;   
        size_t k = 0;
        bool equal = true;
        for (;;) {
            int ca = tolower((unsigned char)level_str[k]);
            int cb = (unsigned char)b[k];

            if (ca != cb) { 
                equal = false; 
                break; 
            }
            if (ca == '\0') { 
                break; 
            }
            ++k;
        }
        if (equal) {
            config->level = table[i].level;
            LOG_LOG("[log_set_level_from_string] Info: Level set to %s.", log_level_to_string(config->level));
            return true;
        }
    }

    LOG_LOG("[log_set_level_from_string] Error: Unrecognised level '%s'.", level_str);
    return false;
}


/**
 * @brief Reports whether a message at @p level would currently be emitted.
 *
 * Checks the level-based gates that `log_message` applies before it does any
 * work: the logger must not be suspended, @p level must be at least the
 * configured minimum level, and that level's visibility must be enabled.
 * Content-dependent filters (keyword / custom callback) and rate limiting are
 * NOT considered, since they depend on the specific message or timing.
 *
 * Use it to skip building an expensive log message that would be dropped, e.g.
 * `if (log_is_level_enabled(lg, LOG_LEVEL_DEBUG)) { ... expensive ... }`.
 *
 * @param config The logger. May be NULL (returns false).
 * @param level  The level to test.
 * @return `true` if such a message would pass the level gates, else `false`.
 */
bool log_is_level_enabled(const Log* config, LogLevel level) {
    if (!config) {
        return false;
    }
    if ((int)level < 0 || (int)level > LOG_LEVEL_FATAL) {
        return false;
    }
    if (config->suspended) {
        return false;
    }
    if (level < config->level) {
        return false;
    }
    return config->level_visibility[level];
}


/**
 * @brief Configures rate limiting: at most @p max_logs_per_interval messages
 *        of each level per @p interval_seconds window.
 *
 * The logger already enforces a per-level rate limit internally; this is the
 * public setter for it. Counting is per level (e.g. INFO and ERROR have
 * independent budgets) and resets every window. Rate limiting is only active
 * when BOTH arguments are non-zero — passing `0, 0` disables it. Calling this
 * starts a fresh window so the new limit takes effect immediately.
 *
 * @param config                The logger. Must not be NULL.
 * @param max_logs_per_interval  Maximum messages per level per window (0 = off).
 * @param interval_seconds       Window length in seconds (0 = off).
 * @return `true` on success, `false` if @p config is NULL.
 */
bool log_set_rate_limit(Log* config, unsigned int max_logs_per_interval, unsigned int interval_seconds) {
    if (!config) {
        LOG_LOG("[log_set_rate_limit] Error: NULL config.");
        return false;
    }
    config->rate_limit_count = max_logs_per_interval;
    config->rate_limit_interval = interval_seconds;
    /* Start a fresh window so the new limit applies from now. */
    memset(config->log_counts, 0, sizeof(config->log_counts));
    config->last_reset_time = time(NULL);

    LOG_LOG("[log_set_rate_limit] Info: Rate limit set to %u message(s) per %u second(s).", max_logs_per_interval, interval_seconds);
    return true;
}


/**
 * @brief Returns the cumulative number of messages emitted at @p level.
 *
 * Counts only messages that actually passed every gate and filter and were
 * written to an output, since the logger was created. The count is monotonic
 * (it is never reset by rate-limit windows) and is maintained per level.
 *
 * @param config The logger. May be NULL (returns 0).
 * @param level  The level to query.
 * @return The number of emitted messages at @p level, or 0 for a NULL config
 *         or an out-of-range level.
 */
unsigned long log_get_message_count(const Log* config, LogLevel level) {
    if (!config) {
        return 0UL;
    }
    if ((int)level < 0 || (int)level > LOG_LEVEL_FATAL) {
        return 0UL;
    }
    return config->total_log_counts[level];
}

