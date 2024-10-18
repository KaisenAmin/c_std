
# Logging Library in C 

**Author:** Amin Tahmasebi
**Release Date:** 2024
**License:** ISC License


## Overview

The logging system is designed to provide a flexible and configurable way to handle logging in C applications. It supports various features such as logging to files and console, setting log levels, filtering logs by keywords, custom formatting, rate limiting, log rotation, and more. This system is essential for tracking application behavior, debugging, and monitoring.

### Key Features:
- **Multiple Log Levels:** Control the verbosity of the logs (e.g., DEBUG, INFO, WARN, ERROR, FATAL).
- **Output Options:** Logs can be written to the console, files, or both.
- **Timestamping:** Add timestamps to log entries.
- **Keyword Filtering:** Display only logs containing specific keywords.
- **Custom Formatting:** Define how log messages are structured.
- **Rate Limiting:** Limit the number of log messages within a specified time frame.
- **Log Rotation:** Automatically archive and rotate log files when they reach a certain size.
- **Custom Filters:** Apply custom logic to filter which messages should be logged.
- **Suspend/Resume Logging:** Temporarily pause and resume logging.

## Function Explanations

### `Log* log_init()`
**Description:** Initializes the logging system. Allocates and configures a `Log` object with default settings.

**Returns:** A pointer to the initialized `Log` object, or `NULL` if initialization fails.

**Example:**
```c
Log* logger = log_init();
```

### `void log_message(Log* config, LogLevel level, const char* message, ...)`
**Description:** Logs a message at a specified log level. Supports variable arguments similar to `printf`.

**Parameters:**
- `config`: Pointer to the `Log` object.
- `level`: The severity level of the message (e.g., `LOG_LEVEL_DEBUG`).
- `message`: The format string for the message.
- `...`: Variable arguments to be formatted.

**Example:**
```c
log_message(logger, LOG_LEVEL_INFO, "This is an info message.");
```

### `void log_flush(Log* config)`
**Description:** Ensures that any buffered log messages are immediately written to their output destinations.

**Parameters:**
- `config`: Pointer to the `Log` object.

**Example:**
```c
log_flush(logger);
```

### `void log_suspend(Log* config)`
**Description:** Temporarily suspends logging. Messages sent to `log_message` will be ignored until logging is resumed.

**Parameters:**
- `config`: Pointer to the `Log` object.

**Example:**
```c
log_suspend(logger);
```

### `void log_resume(Log* config)`
**Description:** Resumes logging after it has been suspended.

**Parameters:**
- `config`: Pointer to the `Log` object.

**Example:**
```c
log_resume(logger);
```

### `void log_deallocate(Log* config)`
**Description:** Cleans up and deallocates the logging system, closing any open files and freeing memory.

**Parameters:**
- `config`: Pointer to the `Log` object.

**Example:**
```c
log_deallocate(logger);
```

### `bool log_set_output(Log* config, LogOutput output)`
**Description:** Sets the output destination for log messages (console, file, or both).

**Parameters:**
- `config`: Pointer to the `Log` object.
- `output`: The desired output destination (e.g., `LOG_OUTPUT_CONSOLE`).

**Returns:** `true` if successful, `false` otherwise.

**Example:**
```c
log_set_output(logger, LOG_OUTPUT_FILE);
```

### `bool log_enable_timestamp(Log* config, bool enable)`
**Description:** Enables or disables timestamping in log messages.

**Parameters:**
- `config`: Pointer to the `Log` object.
- `enable`: `true` to enable timestamps, `false` to disable.

**Returns:** `true` if successful, `false` otherwise.

**Example:**
```c
log_enable_timestamp(logger, true);
```

### `bool log_set_log_level(Log* config, LogLevel newLevel)`
**Description:** Sets the minimum log level. Only messages at or above this level will be logged.

**Parameters:**
- `config`: Pointer to the `Log` object.
- `newLevel`: The new log level (e.g., `LOG_LEVEL_WARN`).

**Returns:** `true` if successful, `false` otherwise.

**Example:**
```c
log_set_log_level(logger, LOG_LEVEL_WARN);
```

### `bool log_enable_keyword_filter(Log* config, const char* keyword, bool enable)`
**Description:** Enables or disables filtering of log messages based on a keyword. Only messages containing the keyword will be logged when enabled.

**Parameters:**
- `config`: Pointer to the `Log` object.
- `keyword`: The keyword to filter by.
- `enable`: `true` to enable filtering, `false` to disable.

**Returns:** `true` if successful, `false` otherwise.

**Example:**
```c
log_enable_keyword_filter(logger, "error", true);
```

### `bool log_update_keyword_filter(Log* config, const char* newKeyword)`
**Description:** Updates the keyword used for filtering log messages.

**Parameters:**
- `config`: Pointer to the `Log` object.
- `newKeyword`: The new keyword to filter by.

**Returns:** `true` if successful, `false` otherwise.

**Example:**
```c
log_update_keyword_filter(logger, "warning");
```

### `bool log_set_file_path(Log* config, const char* newFilePath)`
**Description:** Changes the file path where log messages are written.

**Parameters:**
- `config`: Pointer to the `Log` object.
- `newFilePath`: The new file path for the log file.

**Returns:** `true` if successful, `false` otherwise.

**Example:**
```c
log_set_file_path(logger, "new_log.txt");
```

### `bool log_rotate(Log* config, const char* newLogPath, size_t maxSize)`
**Description:** Rotates the log file if it exceeds the specified size. The current log file is archived, and a new log file is started.

**Parameters:**
- `config`: Pointer to the `Log` object.
- `newLogPath`: Path to the archive file.
- `maxSize`: The maximum size (in bytes) for the log file.

**Returns:** `true` if successful, `false` otherwise.

**Example:**
```c
log_rotate(logger, "old_log.txt", 1048576); // Rotate if file size exceeds 1 MB
```

### `bool log_set_format(Log* config, const char* format)`
**Description:** Sets a custom format for log messages.

**Parameters:**
- `config`: Pointer to the `Log` object.
- `format`: The format string for log messages (e.g., `[%s] [%s] - %s`).

**Returns:** `true` if successful, `false` otherwise.

**Example:**
```c
log_set_format(logger, "[%Y-%m-%d %H:%M:%S] [%s] %s");
```

### `bool log_toggle_level_visibility(Log* config, LogLevel level, bool visible)`
**Description:** Toggles the visibility of specific log levels, allowing them to be shown or hidden dynamically.

**Parameters:**
- `config`: Pointer to the `Log` object.
- `level`: The log level to toggle visibility for.
- `visible`: `true` to show messages of this level, `false` to hide them.

**Returns:** `true` if successful, `false` otherwise.

**Example:**
```c
log_toggle_level_visibility(logger, LOG_LEVEL_DEBUG, false);
```

### `bool log_redirect_output(Log* config, const char* newFilePath)`
**Description:** Redirects log output to a different file dynamically.

**Parameters:**
- `config`: Pointer to the `Log` object.
- `newFilePath`: The new file path for logging.

**Returns:** `true` if successful, `false` otherwise.

**Example:**
```c
log_redirect_output(logger, "new_log_file.txt");
```

### `bool log_set_verbose(Log* config, bool verbose)`
**Description:** Enables or disables verbose logging. When enabled, all log levels are visible.

**Parameters:**
- `config`: Pointer to the `Log` object.
- `verbose`: `true` to enable verbose logging, `false` to disable.

**Returns:** `true` if successful, `false` otherwise.

**Example:**
```c
log_set_verbose(logger, true);
```

### `bool log_set_custom_filter(Log* config, LogFilterFunction filter, void* user_data)`
**Description:** Sets a custom filter function to apply additional logic when deciding whether to log a message.

**Parameters:**
- `config`: Pointer to the `Log` object.
- `filter`: The custom filter function.
- `user_data`: User data to pass to the filter function.

**Returns:** `true` if successful, `false` otherwise.

**Example:**
```c
bool custom_filter(LogLevel level, const char* message, void* user_data) {
    return strstr(message, "important") != NULL;
}

log_set_custom_filter(logger, custom_filter, NULL);
```

### `bool log_set_max_file_size(Log* config, size_t maxSize, const char* archivePathFormat)`
**Description:** Sets the maximum log file size. When the file exceeds this size, it is archived, and a new file is started.

**Parameters:**
- `config

`: Pointer to the `Log` object.
- `maxSize`: The maximum log file size in bytes.
- `archivePathFormat`: The format string for the archive file path, which can include timestamps.

**Returns:** `true` if successful, `false` otherwise.

**Example:**
```c
log_set_max_file_size(logger, 5 * 1024 * 1024, "log_archive_%Y-%m-%d.txt"); // 5 MB
```

## Example 1 : initialize logger with `log_init`

```c
#include "log/log.h"
#include <stdlib.h>

int main() {
    Log* config = log_init();
    
    if (!config) {
        fmt_fprintf(stderr, "Error: Can not Create log object.\n");
        exit(-1);
    }

    return 0;
}
```

## Example 2 : different log level 

```c
#include "log/log.h"

int main() {
    Log* logger = log_init();
    if (!logger) {
        fmt_fprintf(stderr, "Failed to initialize logging system.\n");
        return -1;
    }

    log_message(logger, LOG_LEVEL_DEBUG, "This is a debug message - might not be displayed.");
    log_message(logger, LOG_LEVEL_INFO, "This is an info message.");
    log_message(logger, LOG_LEVEL_WARN, "This is a warning message.");
    log_message(logger, LOG_LEVEL_ERROR, "This is an error message.");

    // Clean up and deallocate the logging system
    log_deallocate(logger);
    return 0;
}
```

## Example 3 : Logging with Timestamps Enabled 

```c
#include "log/log.h"

int main() {
    Log* logger = log_init();
    if (!logger) {
        fmt_fprintf(stderr, "Error: Cannot create log object.\n");
        return -1;
    }

    // Enable timestamps in log messages
    log_enable_timestamp(logger, true);

    log_message(logger, LOG_LEVEL_INFO, "Application started with timestamp.");
    log_message(logger, LOG_LEVEL_WARN, "Low disk space warning logged with timestamp.");
    log_message(logger, LOG_LEVEL_ERROR, "File not found error logged with timestamp.");

    log_deallocate(logger);
    return 0;
}
```

## Example 4 : set log level with `log_set_log_level`

```c
#include "log/log.h"

int main() {
    Log* logger = log_init();

    // Initially set the log level to INFO
    log_set_log_level(logger, LOG_LEVEL_INFO);
    log_message(logger, LOG_LEVEL_INFO, "This is an info message. Debug messages will not be shown.");

    // Change log level to DEBUG to see more detailed logging
    log_set_log_level(logger, LOG_LEVEL_DEBUG);
    log_message(logger, LOG_LEVEL_DEBUG, "Debug level set. This debug message is now visible.");

    log_deallocate(logger);
    return 0;
}
```

## Example 5 : filter keyword in logging with `log_enable_keyword_filter`

```c
#include "log/log.h"

int main() {
    Log* logger = log_init();

    // Enable keyword filtering to only show logs containing "error"
    log_enable_keyword_filter(logger, "error", true);
    log_message(logger, LOG_LEVEL_DEBUG, "This debug message does not contain the keyword.");
    log_message(logger, LOG_LEVEL_ERROR, "An error occurred.");

    // Disable keyword filtering to show all logs again
    log_enable_keyword_filter(logger, NULL, false);
    log_message(logger, LOG_LEVEL_INFO, "Info level log, keyword filter disabled.");

    log_deallocate(logger);
    return 0;
}
```

## Example 6 : update keyword filter with `log_update_keyword_filter`

```c
#include "log/log.h"

int main() {
    Log* logger = log_init();

    log_message(logger, LOG_LEVEL_INFO, "Initial info message.");
    log_message(logger, LOG_LEVEL_ERROR, "Initial error message.");

    // Update keyword filter to only log messages containing "error"
    log_update_keyword_filter(logger, "error");
    log_message(logger, LOG_LEVEL_INFO, "This info message does not contain the keyword.");
    log_message(logger, LOG_LEVEL_ERROR, "An error occurred after update.");

    // Disable keyword filtering
    log_update_keyword_filter(logger, "");
    log_message(logger, LOG_LEVEL_INFO, "Info level log, keyword filter disabled.");

    log_deallocate(logger);
    return 0;
}
```

## Example 7 : set file path with `log_set_file_path`

```c
#include "log/log.h"

int main() {
    Log* logger = log_init();

    log_message(logger, LOG_LEVEL_INFO, "Logging to the initial file.");

    // Change the log file path
    if (log_set_file_path(logger, "new_log.txt")) {
        log_message(logger, LOG_LEVEL_INFO, "Logging to the new file.");
    } 
    else {
        log_message(logger, LOG_LEVEL_ERROR, "Failed to switch to the new log file.");
    }

    log_deallocate(logger);
    return 0;
}
```

## Example 8 : This function will ensure that any buffered log messages are immediately written to their output destinations. `log_flush`

```c
#include "log/log.h"

int main() {
    Log* logger = log_init();

    log_message(logger, LOG_LEVEL_ERROR, "An unexpected error occurred.");
    log_flush(logger); 

    log_deallocate(logger);
    return 0;
}
```

## Example 9 : log rotate 

```c
#include "log/log.h"

int main() {
    Log* logger = log_init();

    log_message(logger, LOG_LEVEL_INFO, "This is an informational message.");

    // Attempt to rotate the log if it exceeds 5 byte
    if (!log_rotate(logger, "old_log.txt", 5)) {
        fmt_fprintf(stderr, "Log rotation failed.\n");
    }

    log_deallocate(logger);
    return 0;
}
```

## Example 10 : suspend and resume log with `log_suspend` and `log_resume`

```c
#include "log/log.h"

int main() {
    Log* logger = log_init();

    // Suspend logging temporarily
    log_suspend(logger);
    log_message(logger, LOG_LEVEL_INFO, "This message will not be logged.");

    // Resume logging
    log_resume(logger);
    log_message(logger, LOG_LEVEL_INFO, "Logging has been resumed.");

    log_deallocate(logger);
    return 0;
}
```

## Example 11 : set custome format for log message with `log_set_format`

```c
#include "log/log.h"

int main() {
    Log* logger = log_init();

    // Set a custom format for log messages
    log_set_format(logger, "[%s] [%s] - %s"); // Format: [Timestamp] [LogLevel] - Message
    log_message(logger, LOG_LEVEL_INFO, "Custom formatted log message.");

    log_deallocate(logger);
    return 0;
}
```

## Example 12 : show and hide message base on log_level with `log_toggle_level_visibility`

```c
#include "log/log.h"

int main() {
    Log* logger = log_init();

    log_message(logger, LOG_LEVEL_DEBUG, "This is a debug message.");
    log_message(logger, LOG_LEVEL_INFO, "This is an info message.");
    log_message(logger, LOG_LEVEL_WARN, "This is a warning message.");
    log_message(logger, LOG_LEVEL_ERROR, "This is an error message.");

    //toggle visibility to hide DEBUG and INFO messages
    log_toggle_level_visibility(logger, LOG_LEVEL_DEBUG, false);
    log_toggle_level_visibility(logger, LOG_LEVEL_INFO, false);

    // After toggling, DEBUG and INFO messages will not be logged.
    log_message(logger, LOG_LEVEL_DEBUG, "This DEBUG message will not be shown.");
    log_message(logger, LOG_LEVEL_INFO, "This INFO message will not be shown.");
    log_message(logger, LOG_LEVEL_WARN, "This WARNING message is still visible.");
    log_message(logger, LOG_LEVEL_ERROR, "This ERROR message is still visible.");

    //toggle visibility to show DEBUG and INFO messages again
    log_toggle_level_visibility(logger, LOG_LEVEL_DEBUG, true);
    log_toggle_level_visibility(logger, LOG_LEVEL_INFO, true);

    // DEBUG and INFO messages are visible again.
    log_message(logger, LOG_LEVEL_DEBUG, "DEBUG messages are visible again.");
    log_message(logger, LOG_LEVEL_INFO, "INFO messages are visible again.");

    log_deallocate(logger);
    return 0;
}
```

## Example 13 : redirect the log output to an other file with `log_redirect_output`

```c
#include "log/log.h"

int main() {
    Log* logger = log_init();

    log_message(logger, LOG_LEVEL_INFO, "Logging to the initial file.");

    // Redirect log output to a new file
    if (log_redirect_output(logger, "new_log_file.txt")) {
        log_message(logger, LOG_LEVEL_INFO, "Logging to a new file after redirection.");
    } 
    else {
        log_message(logger, LOG_LEVEL_ERROR, "Failed to redirect log output.");
    }

    log_deallocate(logger);
    return 0;
}
```

## Example 14 : set or unset verbosity with `log_set_verbose`

```c
#include "log/log.h"

int main() {
    Log* logger = log_init();

    log_set_verbose(logger, true);

    log_message(logger, LOG_LEVEL_DEBUG, "This is a verbose debug message.");
    log_message(logger, LOG_LEVEL_INFO, "This is a verbose info message.");
    log_message(logger, LOG_LEVEL_WARN, "This is a warning message.");
    log_message(logger, LOG_LEVEL_ERROR, "This is an error message.");

    // Change verbosity dynamically
    log_set_verbose(logger, false); 

    log_message(logger, LOG_LEVEL_DEBUG, "This debug message will not be shown when verbose is off.");
    log_message(logger, LOG_LEVEL_INFO, "This info message will not be shown when verbose is off.");
    log_message(logger, LOG_LEVEL_WARN, "This warning message is still visible.");
    log_message(logger, LOG_LEVEL_ERROR, "This error message is still visible.");

    log_deallocate(logger);
    return 0;
}
```

## Example 15 : set custome filter on logger `log_set_custom_filter

```c
#include "log/log.h"
#include <string.h>

bool custom_log_filter(LogLevel level, const char* message, void* user_data) {
    (void)level;
    const char* required_substring = (const char*)user_data;
    return strstr(message, required_substring) != NULL;
}

int main() {
    Log* logger = log_init();

    const char* filter_criteria = "important";
    log_set_custom_filter(logger, custom_log_filter, (void*)filter_criteria);

    log_message(logger, LOG_LEVEL_INFO, "This is an important info message.");
    log_message(logger, LOG_LEVEL_INFO, "This is a regular info message, will be filtered out.");

    log_deallocate(logger);
    return 0;
}
```

## Example 16 : set maximum log file size with  `log_set_max_file_size`

```c
#include "log/log.h"


int main() {
    Log* logger = log_init();

    // Set maximum log file size to 5 MB and specify archive naming pattern
    log_set_max_file_size(logger, 5 * 1024 * 1024, "log_archive_%Y-%m-%d_%H-%M-%S.txt");

    for (int i = 0; i < 1000; i++) {
        log_message(logger, LOG_LEVEL_INFO, "Log message number %d", i);
    }

    log_deallocate(logger);
    return 0;
}
```

## Example 17 : rate limiting features 

```c
#include "log/log.h"
#include "time/std_time.h"

int main() {
    Log* logger = log_init();

    // Configure rate limiting: Allow up to 5 messages per log level every 10 seconds
    logger->rate_limit_interval = 10; // seconds
    logger->rate_limit_count = 5;

    // Generate log messages at a rate that exceeds the limit
    for (int i = 0; i < 20; i++) {
        log_message(logger, LOG_LEVEL_INFO, "This is info message %d", i);
        log_message(logger, LOG_LEVEL_DEBUG, "This is debug message %d", i);
        if (i % 2 == 0) {
            log_message(logger, LOG_LEVEL_WARN, "This is a warning message %d", i);
        }
        time_sleep(1);
    }

    log_deallocate(logger);
    return 0;
}
```