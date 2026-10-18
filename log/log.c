/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Log
*/

/* POSIX / GNU declarations (strdup, clock_gettime, localtime_r, ...) must be
   requested before the first #include, so that strict -std=c17 builds
   (CMAKE_C_EXTENSIONS=OFF) see them as well. */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include "log.h"
#include "../string/std_string.h"
#include "../concurrent/concurrent.h"


/* ------------------------------------------------------------------ */
/* Internal helpers — distinct `logp_` prefix from the public `log_`. */
/* ------------------------------------------------------------------ */

/* Lock / unlock the logger's internal recursive mutex. Both are no-ops when
 * the logger has no lock (mutex allocation failed at init), so the logger
 * degrades to the historical single-threaded behaviour instead of crashing. */
static void logp_lock(Log* config) {
    if (config && config->_lock) {
        mutex_lock((Mutex*)config->_lock);
    }
}
static void logp_unlock(Log* config) {
    if (config && config->_lock) {
        mutex_unlock((Mutex*)config->_lock);
    }
}

/* Moves src over dst, replacing dst atomically when it exists. Rotation used
 * to remove() the previous archive first and, if the rename then failed,
 * reopen the live log with "w" -- losing both files. */
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include "../encoding/encoding.h"
static bool logp_replace_file(const char* src, const char* dst) {
    wchar_t* wsrc = encoding_utf8_to_wchar(src);
    wchar_t* wdst = encoding_utf8_to_wchar(dst);
    bool ok = wsrc && wdst && MoveFileExW(wsrc, wdst, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED) != 0;
    free(wsrc);
    free(wdst);
    return ok;
}
static bool logp_path_exists(const char* path) {
    wchar_t* wpath = encoding_utf8_to_wchar(path);
    bool exists = wpath && GetFileAttributesW(wpath) != INVALID_FILE_ATTRIBUTES;
    free(wpath);
    return exists;
}
#else
#include <stdio.h>
#include <sys/stat.h>
static bool logp_replace_file(const char* src, const char* dst) {
    return rename(src, dst) == 0;   /* POSIX rename replaces dst atomically */
}
static bool logp_path_exists(const char* path) {
    struct stat st;
    return stat(path, &st) == 0;
}
#endif

/* The format is handed to snprintf with exactly three string arguments
 * (timestamp, level, message), so only "%s" and "%%" may appear, and at most
 * three "%s". Anything else (e.g. "%d" or "%n") would read the wrong
 * argument types: undefined behavior driven by the format string. */
static bool logp_format_is_safe(const char* format) {
    int strings = 0;
    for (const char* p = format; *p; ++p) {
        if (*p != '%') {
            continue;
        }
        ++p;
        if (*p == '%') {
            continue;
        }
        if (*p != 's' || ++strings > 3) {
            return false;
        }
    }
    return true;
}

/* Path of the live log file ("log.txt" when none was recorded). */
static void logp_live_path(const Log* config, char* out, size_t cap) {
    const char* src = config->current_file_path[0] ? config->current_file_path : "log.txt";
    size_t n = strlen(src);
    if (n >= cap) {
        n = cap - 1;
    }
    memcpy(out, src, n);
    out[n] = '\0';
}

/* Archives the live log file under strftime(archivePathFormat, now) and starts
 * an empty one at the same path. An archive name that already exists gets a
 * ".1", ".2", ... suffix, so rotations within the same second keep every
 * archive. On failure the live file is reopened for appending (nothing is
 * lost). Caller holds the lock and has checked config->file_writer. */
static bool logp_archive_current(Log* config, const char* archivePathFormat) {
    char source_path[LOG_MAX_PATH];
    logp_live_path(config, source_path, sizeof(source_path));

    char archivePath[LOG_MAX_PATH];
    time_t now = time(NULL);
    struct tm tm_buf;                          /* reentrant localtime */
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm_buf, &now);
#else
    localtime_r(&now, &tm_buf);
#endif
    if (strftime(archivePath, sizeof(archivePath), archivePathFormat, &tm_buf) == 0) {
        return false;   /* empty or too long a name */
    }
    if (logp_path_exists(archivePath)) {
        char numbered[LOG_MAX_PATH + 16];
        bool found = false;
        for (unsigned n = 1; n < 10000 && !found; ++n) {
            snprintf(numbered, sizeof(numbered), "%s.%u", archivePath, n);
            found = strlen(numbered) < sizeof(archivePath) && !logp_path_exists(numbered);
        }
        if (!found) {
            return false;
        }
        strcpy(archivePath, numbered);
    }

    file_writer_close(config->file_writer);
    config->file_writer = NULL;

    /* Same safe sequence as log_rotate: atomic replace, append on failure. */
    if (!logp_replace_file(source_path, archivePath)) {
        config->file_writer = file_writer_append(source_path, WRITE_TEXT);
        return false;
    }
    config->file_writer = file_writer_open(source_path, WRITE_TEXT);
    return config->file_writer != NULL;
}

/* Switches the file sink to `path`, all or nothing: the new file is opened
 * before the old one is closed, so a failed open leaves the current file, its
 * path and the output setting untouched. (The old file used to be closed
 * first: a NULL path or a failed open silently stopped file logging, and the
 * stale current_file_path later made log_set_output reopen the old name.)
 * Caller holds the lock and has validated `path`. */
static bool logp_switch_file(Log* config, const char* path) {
    if (config->file_writer) {
        fflush(config->file_writer->file_writer);   /* before a reopen of the same path truncates it */
    }
    FileWriter* fresh = file_writer_open(path, WRITE_TEXT);
    if (!fresh) {
        return false;
    }
    if (config->file_writer) {
        file_writer_close(config->file_writer);
    }
    config->file_writer = fresh;
    strncpy(config->current_file_path, path, sizeof(config->current_file_path) - 1);
    config->current_file_path[sizeof(config->current_file_path) - 1] = '\0';
    return true;
}

/* A usable log file path: not NULL, not empty, fits current_file_path. */
static bool logp_path_ok(const char* path) {
    return path && path[0] != '\0' && strlen(path) < LOG_MAX_PATH;
}

/* Thread-safe local timestamp ("YYYY-MM-DD HH:MM:SS"). Uses the reentrant
 * localtime_r / localtime_s rather than localtime(), whose shared static
 * `struct tm` is a data race under concurrent logging. */
static void logp_format_timestamp(char* out, size_t cap) {
    time_t now = time(NULL);
    struct tm tm_buf;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm_buf, &now);
#else
    localtime_r(&now, &tm_buf);
#endif
    strftime(out, cap, "%Y-%m-%d %H:%M:%S", &tm_buf);
}



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
    /* Append, like Python's FileHandler: opening with "w" silently emptied an
       existing ./log.txt (from an earlier run or an unrelated program). */
    config->file_writer = file_writer_append("log.txt", WRITE_TEXT);

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

    // Production-default behaviour, opt-in:
    config->errors_to_stderr = false;  // all console output to stdout (legacy default)
    config->auto_flush       = false;  // rely on stdio buffering by default
    config->max_file_size    = 0;      // no automatic rotation until log_set_max_file_size
    config->archive_path_format[0] = '\0';

    // Internal recursive mutex for thread safety. If allocation/init fails the
    // logger still works (single-threaded); locking helpers become no-ops.
    config->_lock = malloc(sizeof(Mutex));
    if (config->_lock) {
        if (mutex_init((Mutex*)config->_lock, MUTEX_RECURSIVE) != THREAD_SUCCESS) {
            free(config->_lock);
            config->_lock = NULL;
            LOG_LOG("[log_init] Warning: mutex init failed; logger is not thread-safe.");
        }
    }

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
    
    logp_lock(config);
    config->output = output;
    if (output == LOG_OUTPUT_FILE && !config->file_writer) {
        // Attempt to open log file if not already open (appending, see log_init)
        config->file_writer = file_writer_append(config->current_file_path[0] ? config->current_file_path : "log.txt", WRITE_TEXT);
        if (!config->file_writer) {
            config->output = LOG_OUTPUT_CONSOLE;
            logp_unlock(config);
            LOG_LOG("[log_set_output] Error: Failed to open log file, reverting to console output.");
            return false;
        }
        LOG_LOG("[log_set_output] Success: Log output set to file.");
    }
    else {
        LOG_LOG("[log_set_output] Success: Log output set successfully.");
    }
    logp_unlock(config);

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
    logp_lock(config);
    config->enable_timestamp = enable;
    logp_unlock(config);

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
    if ((int)level < 0 || (int)level > LOG_LEVEL_FATAL || !message) {
        /* level indexes the per-level arrays below */
        LOG_LOG("[log_message] Error: Invalid log level %d or NULL message.", (int)level);
        return;
    }

    // Compose the caller's message up-front (local buffer; the va_args must be
    // consumed on this thread). All shared-state work below is mutex-guarded.
    char formatted_message[1024];
    va_list args;
    va_start(args, message);
    vsnprintf(formatted_message, sizeof(formatted_message), message, args);
    va_end(args);

    logp_lock(config);

    /* The gates read settings that other threads may change concurrently, so
       they are evaluated under the lock as well. */
    if (config->suspended) {
        LOG_LOG("[log_message] Info: Logging is currently suspended.");
        logp_unlock(config);
        return;
    }
    if (level < config->level) {
        LOG_LOG("[log_message] Info: Current log level (%d) is higher than the message log level (%d); message not logged.", config->level, level);
        logp_unlock(config);
        return;
    }
    if (!config->level_visibility[level]) {
        LOG_LOG("[log_message] Info: Log level is currently not visible.");
        logp_unlock(config);
        return; // Skip logging if this level is currently not visible
    }

    // Keyword filtering
    if (config->is_keyword_filter_enabled && strstr(formatted_message, config->keyword_filter) == NULL) {
        LOG_LOG("[log_message] Info: Message does not contain the keyword filter; not logged.");
        logp_unlock(config);
        return;
    }
    if (config->custom_filter && !config->custom_filter(level, formatted_message, config->custom_filter_user_data)) {
        LOG_LOG("[log_message] Info: Logging skipped due to custom filter.");
        logp_unlock(config);
        return;
    }

    char timestamp[64] = "";
    if (config->enable_timestamp) {
        logp_format_timestamp(timestamp, sizeof(timestamp));   // reentrant localtime
    }

    const char* level_str = log_level_to_string(level);
    char log_buffer[2048];

    // Apply custom format
    int written = snprintf(log_buffer, sizeof(log_buffer), config->format, timestamp, level_str, formatted_message);
    if (written < 0 || written >= (int)sizeof(log_buffer)) {
        LOG_LOG("[log_message] Error: Failed to format log message correctly.");
        logp_unlock(config);
        return;
    }

    // Rate limiting: only active when both interval and count are non-zero.
    // Without this guard, an interval of 0 would reset the counters on every
    // call (since currentTime - last_reset_time >= 0 is always true). It runs
    // after the keyword and custom filters: messages they drop used to use up
    // the quota, so the matching ones were then suppressed.
    if (config->rate_limit_interval > 0 && config->rate_limit_count > 0) {
        time_t currentTime = time(NULL);
        if ((unsigned int)(currentTime - config->last_reset_time) >= config->rate_limit_interval) {
            // Reset rate limit counters
            memset(config->log_counts, 0, sizeof(config->log_counts));
            config->last_reset_time = currentTime;
        }
        if (config->log_counts[level] >= config->rate_limit_count) {
            LOG_LOG("[log_message] Info: Rate limit exceeded for this level, message skipped.");
            logp_unlock(config);
            return;
        }
        config->log_counts[level]++;
    }

    // The message passed every gate/filter and is about to be emitted: count it.
    config->total_log_counts[level]++;

    if (config->output == LOG_OUTPUT_CONSOLE || config->output == LOG_OUTPUT_BOTH) {
        FILE* console = (config->errors_to_stderr && level >= LOG_LEVEL_ERROR) ? stderr : stdout;
        fprintf(console, "%s\n", log_buffer);
        if (config->auto_flush) {
            fflush(console);
        }
    }
    if ((config->output == LOG_OUTPUT_FILE || config->output == LOG_OUTPUT_BOTH) && config->file_writer) {
        fprintf(config->file_writer->file_writer, "%s\n", log_buffer);
        if (config->auto_flush) {
            fflush(config->file_writer->file_writer);
        }
        /* Automatic rotation set up by log_set_max_file_size. */
        if (config->max_file_size > 0) {
            long size = ftell(config->file_writer->file_writer);
            if (size >= 0 && (size_t)size >= config->max_file_size &&
                !logp_archive_current(config, config->archive_path_format)) {
                LOG_LOG("[log_message] Error: automatic rotation failed.");
            }
        }
    }

    logp_unlock(config);
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
    if (config->_lock) {
        mutex_destroy((Mutex*)config->_lock);
        free(config->_lock);
        config->_lock = NULL;
        LOG_LOG("[log_deallocate] Success: logger mutex destroyed.");
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
    logp_lock(config);
    config->level = newLevel;
    logp_unlock(config);

    LOG_LOG("[log_set_log_level] Info: Log Level changed to %s.", log_level_to_string(newLevel));
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
        
        logp_lock(config);
        strncpy(config->keyword_filter, keyword, sizeof(config->keyword_filter) - 1);
        config->keyword_filter[sizeof(config->keyword_filter) - 1] = '\0'; // Ensure null termination
        config->is_keyword_filter_enabled = true;
        logp_unlock(config);

        LOG_LOG("[log_enable_keyword_filter] Info: Keyword filtering enabled for '%s'.", keyword);
    } 
    else {
        logp_lock(config);
        config->is_keyword_filter_enabled = false;
        logp_unlock(config);
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
        logp_lock(config);
        strncpy(config->keyword_filter, newKeyword, MAX_KEYWORD_LENGTH);
        config->keyword_filter[MAX_KEYWORD_LENGTH - 1] = '\0';
        config->is_keyword_filter_enabled = true;
        logp_unlock(config);
        LOG_LOG("[log_update_keyword_filter] Info: Keyword filter updated to '%s'.", newKeyword);
    }
    else if (newKeyword && strlen(newKeyword) == 0) {
        logp_lock(config);
        config->is_keyword_filter_enabled = false;
        config->keyword_filter[0] = '\0';
        logp_unlock(config);
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
 * This function changes the log file to the specified path. The new file is opened first and
 * the current one is closed only when that worked: on any failure the current file, its path
 * and the output setting are left as they were.
 *
 * @param config Pointer to the `Log` configuration object.
 * @param newFilePath The path to the new log file. If `newFilePath` is `NULL`, empty or not
 * shorter than `LOG_MAX_PATH`, the operation fails.
 *
 * @return `true` if the log file path was successfully updated, `false` if the `config` is `NULL`, `newFilePath` is invalid, or if the new file could not be opened.
 */
bool log_set_file_path(Log* config, const char* newFilePath) {
    if (!config) {
        LOG_LOG("[log_set_file_path] Error: Log configuration object is null.");
        return false;
    }
    if (!logp_path_ok(newFilePath)) {
        LOG_LOG("[log_set_file_path] Error: Invalid new file path.");
        return false;
    }

    logp_lock(config);
    if (!logp_switch_file(config, newFilePath)) {
        logp_unlock(config);
        LOG_LOG("[log_set_file_path] Error: Failed to open new log file.");
        return false;
    }
    logp_unlock(config);

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
    logp_lock(config);
    if ((config->output == LOG_OUTPUT_FILE || config->output == LOG_OUTPUT_BOTH) && config->file_writer) {
        fflush(config->file_writer->file_writer);
    }
    if (config->output == LOG_OUTPUT_CONSOLE || config->output == LOG_OUTPUT_BOTH) {
        fflush(stdout);
    }
    logp_unlock(config);

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
    if (!config || !newLogPath) {
        LOG_LOG("[log_rotate] Error: Log configuration object or path is null.");
        return false;
    }

    logp_lock(config);
    if (!config->file_writer) {
        logp_unlock(config);
        LOG_LOG("[log_rotate] Error: file_writer is not initialized.");
        return false;
    }
    size_t fileSize = file_writer_get_size(config->file_writer);   /* 0 on error */

    if (fileSize >= maxSize) {
        char source_path[LOG_MAX_PATH];
        strncpy(source_path, config->current_file_path, sizeof(source_path) - 1);
        source_path[sizeof(source_path) - 1] = '\0';

        if (source_path[0] == '\0') {
            strncpy(source_path, "log.txt", sizeof(source_path) - 1);
            source_path[sizeof(source_path) - 1] = '\0';
        }

        file_writer_close(config->file_writer);
        config->file_writer = NULL;

        /* Replace the archive atomically (no remove() first) and, if that
           fails, reopen the live log for APPENDING: "w" truncated it. */
        if (!logp_replace_file(source_path, newLogPath)) {
            LOG_LOG("[log_rotate] Error: Failed to rename log file.");
            config->file_writer = file_writer_append(source_path, WRITE_TEXT);
            logp_unlock(config);
            return false;
        }

        config->file_writer = file_writer_open(source_path, WRITE_TEXT);
        if (!config->file_writer) {
            logp_unlock(config);
            LOG_LOG("[log_rotate] Error: Failed to open new log file.");
            return false;
        }
        LOG_LOG("[log_rotate] Info: Log rotated successfully.");
    }

    logp_unlock(config);
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
    logp_lock(config);
    config->suspended = true;
    logp_unlock(config);
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
    logp_lock(config);
    config->suspended = false;
    logp_unlock(config);
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
    
    if (!logp_format_is_safe(format)) {
        LOG_LOG("[log_set_format] Error: Only %%s (at most three) and %%%% are allowed in the format.");
        return false;
    }

    /* Under the lock: log_message formats with config->format, and a copy
       racing with that read produced a torn format string (crash). */
    logp_lock(config);
    strncpy(config->format, format, sizeof(config->format) - 1);
    config->format[sizeof(config->format) - 1] = '\0';
    logp_unlock(config);

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

    logp_lock(config);
    config->level_visibility[level] = visible;
    logp_unlock(config);

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
 * parameter is `NULL`, `newFilePath` is invalid, or if an error occurs while opening the new file
 * (the current file then stays in use: it is closed only after the new one opened).
 */
bool log_redirect_output(Log* config, const char* newFilePath) {
    if (!config) {
        LOG_LOG("[log_redirect_output] Error: Log configuration object is null.");
        return false;
    }
    if (!logp_path_ok(newFilePath)) {
        LOG_LOG("[log_redirect_output] Error: Invalid new file path.");
        return false;
    }
    logp_lock(config);
    // Also tracks the active log file path so rotation/archive uses the right source.
    if (!logp_switch_file(config, newFilePath)) {
        logp_unlock(config);
        LOG_LOG("[log_redirect_output] Error: Failed to open new log file.");
        return false;
    }
    logp_unlock(config);

    LOG_LOG("[log_redirect_output] Info: Log output redirected to '%s'.", newFilePath);
    return true;
}


/**
 * @brief Enables or disables verbose logging.
 *
 * Verbose mode switches the visibility of `DEBUG` and `INFO` on (`true`) or off (`false`),
 * like two log_toggle_level_visibility calls. The minimum level set by log_set_log_level is
 * not changed and still applies: after log_set_log_level(ERROR), DEBUG stays hidden even
 * in verbose mode.
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

    logp_lock(config);
    config->level_visibility[LOG_LEVEL_DEBUG] = verbose;
    config->level_visibility[LOG_LEVEL_INFO] = verbose;
    logp_unlock(config);
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

    /* Both fields change together under the lock, so a concurrent
       log_message never calls one filter with another filter's user_data. */
    logp_lock(config);
    config->custom_filter = filter;
    config->custom_filter_user_data = user_data;
    logp_unlock(config);

    LOG_LOG("[log_set_custom_filter] Info: Custom log filter set successfully.");

    return true;
}


/**
 * @brief Sets a maximum log file size and automatically rotates the log file when this size is exceeded.
 *
 * The setting is kept: after every record written to the file, log_message checks the size
 * and, once it reaches @p maxSize, archives the file under the strftime() name made from
 * @p archivePathFormat and starts an empty one at the same path. (It used to be a one-shot
 * check at the time of the call.) An archive name that already exists gets a ".1", ".2", ...
 * suffix. The current file is checked right away as well.
 *
 * @param config Pointer to the `Log` configuration object.
 * @param maxSize The maximum size (in bytes) that the log file is allowed to reach before being
 * rotated; 0 turns automatic rotation off (@p archivePathFormat may then be NULL).
 * @param archivePathFormat A format string used to generate the name of the archived log file.
 * The format string can include time-based formatting specifiers.
 *
 * @return `true` if the setting was stored (and the immediate check, when a file is open, did not
 * fail), `false` if `config` is `NULL`, the format is NULL, empty or too long while `maxSize` is
 * not 0, or the immediate rotation failed.
 */
bool log_set_max_file_size(Log* config, size_t maxSize, const char* archivePathFormat) {
    if (!config) {
        LOG_LOG("[log_set_max_file_size] Error: Invalid parameters.");
        return false;
    }
    if (maxSize == 0) {
        logp_lock(config);
        config->max_file_size = 0;   /* automatic rotation off */
        logp_unlock(config);
        return true;
    }
    if (!archivePathFormat || archivePathFormat[0] == '\0' || strlen(archivePathFormat) >= LOG_MAX_PATH) {
        LOG_LOG("[log_set_max_file_size] Error: Invalid archive path format.");
        return false;
    }

    logp_lock(config);
    config->max_file_size = maxSize;
    strcpy(config->archive_path_format, archivePathFormat);

    bool ok = true;
    if (config->file_writer) {
        fflush(config->file_writer->file_writer);
        size_t fileSize = file_writer_get_size(config->file_writer);
        if (fileSize >= maxSize) {
            ok = logp_archive_current(config, config->archive_path_format);
            if (!ok) {
                LOG_LOG("[log_set_max_file_size] Error: Could not archive log file.");
            }
        }
    }
    logp_unlock(config);
    return ok;
}


/**
 * @brief Routes ERROR/FATAL console output to stderr instead of stdout.
 *
 * By default every console line goes to stdout. In production it is usually
 * preferable to send high-severity records (`LOG_LEVEL_ERROR` and above) to
 * stderr, so they stay visible when stdout is piped or captured and can be
 * collected separately. This affects console output only (`LOG_OUTPUT_CONSOLE`
 * / `LOG_OUTPUT_BOTH`); the file sink is unchanged. Thread-safe.
 *
 * @param config           The logger. Must not be NULL.
 * @param errors_to_stderr `true` to send ERROR/FATAL to stderr, `false` to send
 *                         all console output to stdout (the default).
 * @return `true` on success, `false` if `config` is NULL.
 */
bool log_set_error_stream(Log* config, bool errors_to_stderr) {
    if (!config) {
        LOG_LOG("[log_set_error_stream] Error: Log configuration object is null.");
        return false;
    }
    logp_lock(config);
    config->errors_to_stderr = errors_to_stderr;
    logp_unlock(config);
    LOG_LOG("[log_set_error_stream] Info: ERROR/FATAL now go to %s.", errors_to_stderr ? "stderr" : "stdout");
    return true;
}


/**
 * @brief Enables or disables flushing after every emitted record.
 *
 * When enabled, the destination stream (console and/or file) is flushed
 * immediately after each message that is actually written, so the most recent
 * lines survive a crash or `kill`. This trades throughput for durability; leave
 * it off for high-volume logging and call `log_flush` at checkpoints instead.
 * Thread-safe.
 *
 * @param config The logger. Must not be NULL.
 * @param enable `true` to flush after every emitted record, `false` to rely on
 *               normal stdio buffering (the default).
 * @return `true` on success, `false` if `config` is NULL.
 */
bool log_set_auto_flush(Log* config, bool enable) {
    if (!config) {
        LOG_LOG("[log_set_auto_flush] Error: Log configuration object is null.");
        return false;
    }
    logp_lock(config);
    config->auto_flush = enable;
    logp_unlock(config);
    LOG_LOG("[log_set_auto_flush] Info: Auto-flush is now %s.", enable ? "enabled" : "disabled");
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
            logp_lock(config);
            config->level = table[i].level;
            logp_unlock(config);
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
    logp_lock((Log*)config);   /* internal mutex: const-cast as in log_get_message_count */
    bool enabled = !config->suspended && level >= config->level && config->level_visibility[level];
    logp_unlock((Log*)config);
    return enabled;
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
    logp_lock(config);
    config->rate_limit_count = max_logs_per_interval;
    config->rate_limit_interval = interval_seconds;
    /* Start a fresh window so the new limit applies from now. */
    memset(config->log_counts, 0, sizeof(config->log_counts));
    config->last_reset_time = time(NULL);
    logp_unlock(config);

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
    /* Lock so the read can't tear against a concurrent increment in
     * log_message. The mutex is an internal detail, hence the const-cast. */
    logp_lock((Log*)config);
    unsigned long count = config->total_log_counts[level];
    logp_unlock((Log*)config);
    return count;
}

