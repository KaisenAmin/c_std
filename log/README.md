
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

- **Purpose**:  
  This function initializes the logging system by setting up a `Log` configuration object with default settings, such as log levels, output options, and filtering. It also attempts to open a log file named "log.txt" for writing. If the file cannot be opened, it defaults to console output.

- **Return Value**:  
  - A pointer to the initialized `Log` object if successful.
  - `NULL`: If memory allocation fails.

- **Initialization**:  
  - Allocates memory for a `Log` object.
  - Sets default log level to `LOG_LEVEL_DEBUG`.
  - Tries to open the log file "log.txt" for writing. If it fails, it switches to console output.
  - Enables timestamps and log levels in log messages by default.
  - Initializes the keyword filter and makes all log levels visible initially.
  - If memory allocation for the log configuration object fails, the function logs the error and terminates the program.

- **Error Handling**:  
  - If memory allocation fails for the `Log` object, the function logs an error and calls `exit(-1)` to terminate.
  - If the log file cannot be opened, it logs an error and defaults to console output.

---

### `bool log_set_output(Log* config, LogOutput output)`

- **Purpose**:  
  This function sets the output destination for log messages, allowing logs to be sent to the console, a file, or both. If the log file cannot be opened when `LOG_OUTPUT_FILE` is specified, it defaults back to console output.

- **Parameters**:
  - `config`: Pointer to the `Log` configuration object.
  - `output`: The desired log output destination (`LOG_OUTPUT_CONSOLE`, `LOG_OUTPUT_FILE`, or `LOG_OUTPUT_BOTH`).

- **Return Value**:  
  - `true`: If the output was successfully set.
  - `false`: If an error occurred (e.g., the configuration object is `NULL`, or the log file could not be opened).

- **Functionality**:
  - It first checks if the `Log` configuration object is valid.
  - It sets the desired output destination.
  - If `LOG_OUTPUT_FILE` is selected but the log file is not already open, it attempts to open "log.txt" for writing.
  - If the file cannot be opened, the function logs an error and defaults to console output.

- **Error Handling**:  
  - Logs an error if the configuration object is `NULL`.
  - If the log file cannot be opened, it logs an error and reverts to console output.

---

### `bool log_enable_timestamp(Log* config, bool enable)`

- **Purpose**:  
  This function enables or disables timestamps in log messages. When enabled, each log message will include the current date and time.

- **Parameters**:
  - `config`: Pointer to the `Log` configuration object.
  - `enable`: `true` to enable timestamps, `false` to disable them.

- **Return Value**:  
  - `true`: If the operation was successful.
  - `false`: If the configuration object is `NULL`.

- **Functionality**:
  - It checks if the `Log` configuration object is valid.
  - It enables or disables timestamps based on the `enable` flag.
  - It logs a message indicating whether timestamps have been enabled or disabled.

- **Error Handling**:  
  - Logs an error if the configuration object is `NULL`.

---

### `void log_message(Log* config, LogLevel level, const char* message, ...)`

- **Purpose**:  
  This function logs a message with a specified log level. It respects the configuration in the `Log` object, including the log level, output settings (console or file), timestamp inclusion, keyword filtering, and rate-limiting. Messages that do not meet the configured criteria (e.g., are below the current log level or exceed the rate limit) will not be logged.

- **Parameters**:  
  - `config`: A pointer to the `Log` configuration object that controls logging behavior.
  - `level`: The log level of the message (e.g., `LOG_LEVEL_DEBUG`, `LOG_LEVEL_INFO`).
  - `message`: A format string for the message to log (similar to `printf`), followed by optional arguments for formatting.

- **Functionality**:
  - The function first checks if logging is suspended, the log level is lower than the configured level, or if logging for the current level is disabled, and exits early if necessary.
  - If rate limiting is enabled and the rate limit is reached for the current level, the message will be skipped.
  - The function constructs the message, applying the configured timestamp, formatting, and keyword filtering.
  - The formatted message is logged either to the console, to a file, or to both, depending on the configured output.

- **Return Value**:  
  - This function does not return a value. If an error occurs (e.g., configuration object is `NULL`, or rate limit is exceeded), the function logs appropriate messages internally.

- **Error Handling**:  
  - Logs errors if the configuration object is `NULL`, if the message exceeds the rate limit, or if there are formatting issues.

---

### `void log_deallocate(Log* config)`

- **Purpose**:  
  This function releases all resources associated with the `Log` object, including closing any open file writer or file reader, and freeing the memory allocated for the `Log` configuration.

- **Parameters**:  
  - `config`: A pointer to the `Log` configuration object to be deallocated.

- **Functionality**:
  - The function checks if the `config` object is `NULL`. If not, it closes any open file writer and file reader, sets their pointers to `NULL`, and frees the `Log` object.
  - It logs a message indicating that the resources have been successfully deallocated.

- **Return Value**:  
  - This function does not return a value. It performs cleanup and logging as needed.

- **Error Handling**:  
  - Logs an error if the `config` object is `NULL`.

---

### `bool log_set_log_level(Log* config, LogLevel newLevel)`

- **Purpose**:  
  This function updates the logging level of the logger, controlling which messages are logged. Only messages with a log level greater than or equal to the specified level will be logged.

- **Parameters**:
  - `config`: A pointer to the `Log` configuration object.
  - `newLevel`: The new logging level to be set. It must be one of `LOG_LEVEL_DEBUG`, `LOG_LEVEL_INFO`, `LOG_LEVEL_WARN`, `LOG_LEVEL_ERROR`, or `LOG_LEVEL_FATAL`.

- **Return Value**:  
  - `true`: If the log level was successfully updated.
  - `false`: If the `config` is `NULL` or if `newLevel` is outside the valid range.

- **Functionality**:
  - The function first validates the `Log` object and checks that the specified `newLevel` is valid.
  - It updates the log level in the `Log` configuration and logs a message indicating the log level change.

- **Error Handling**:  
  - Logs an error if the `Log` configuration object is `NULL` or if the specified log level is invalid (outside the range of valid levels).

---

### `bool log_enable_keyword_filter(Log* config, const char* keyword, bool enable)`

- **Purpose**:  
  This function enables or disables keyword-based filtering for log messages. When enabled, only log messages that contain the specified keyword will be logged.

- **Parameters**:  
  - `config`: A pointer to the `Log` configuration object.
  - `keyword`: The keyword to filter log messages. Must not be `NULL` or empty when enabling filtering.
  - `enable`: A boolean flag to enable (`true`) or disable (`false`) keyword filtering.

- **Return Value**:  
  - `true`: If keyword filtering was successfully enabled or disabled.
  - `false`: If the `config` is `NULL`, or if the `keyword` is invalid when enabling filtering.

- **Functionality**:  
  - The function checks if the `config` is valid. If keyword filtering is enabled, it verifies that the `keyword` is valid (i.e., non-`NULL` and non-empty). It then copies the keyword into the configuration and sets the filter flag.
  - If keyword filtering is being disabled, the function clears the keyword and resets the filter flag.

- **Error Handling**:  
  - Logs errors if the `config` is `NULL` or the keyword is invalid.

---

### `bool log_update_keyword_filter(Log* config, const char* newKeyword)`

- **Purpose**:  
  This function updates the keyword used for filtering log messages. If the new keyword is an empty string, keyword filtering is disabled.

- **Parameters**:  
  - `config`: A pointer to the `Log` configuration object.
  - `newKeyword`: The new keyword to filter log messages. If an empty string is provided, keyword filtering will be disabled.

- **Return Value**:  
  - `true`: If the keyword filter was successfully updated or disabled.
  - `false`: If the `config` is `NULL`, the `newKeyword` is invalid, or the length exceeds the maximum allowed keyword length.

- **Functionality**:  
  - The function first checks if the `config` is valid. If the new keyword is valid (non-empty and within the maximum length), the function updates the keyword filter.
  - If an empty string is passed, keyword filtering is disabled, and the filter flag is reset.

- **Error Handling**:  
  - Logs an error if the `config` is `NULL`, the new keyword is invalid, or the keyword length exceeds the allowed limit.

---

### `bool log_set_file_path(Log* config, const char* newFilePath)`

- **Purpose**:  
  This function changes the log file path, closing the current file (if open) and attempting to open a new log file at the specified path.

- **Parameters**:  
  - `config`: A pointer to the `Log` configuration object.
  - `newFilePath`: The path to the new log file. If `newFilePath` is `NULL` or an empty string, the operation will fail.

- **Return Value**:  
  - `true`: If the log file path was successfully updated.
  - `false`: If the `config` is `NULL`, `newFilePath` is invalid, or the new file cannot be opened.

- **Functionality**:  
  - The function closes any currently open log file and attempts to open a new one at the specified path. If the new file cannot be opened, it logs an error and optionally reverts to console output.

- **Error Handling**:  
  - Logs errors if the `config` is `NULL`, the new file path is invalid, or the new log file cannot be opened.

---

### `bool log_rotate(Log* config, const char* newLogPath, size_t maxSize)`

- **Purpose**:  
  This function manages log file rotation when the current log file reaches a specified size. It renames or moves the existing log file to a new location, then starts writing to a fresh log file. This helps control log file size and manage disk space.

- **Parameters**:  
  - `config`: A pointer to the `Log` configuration object.
  - `newLogPath`: The new path to move or rename the current log file when it is rotated.
  - `maxSize`: The maximum allowed size of the log file (in bytes). When the log file exceeds this size, rotation occurs.

- **Return Value**:  
  - `true`: If the log file was successfully rotated.
  - `false`: If there was an error (e.g., `config` is `NULL`, file rotation fails, or the file cannot be opened after rotation).

- **Error Handling**:  
  If the log file size cannot be obtained or the file cannot be renamed, the function logs appropriate errors and returns `false`.

---

### `void log_suspend(Log* config)`

- **Purpose**:  
  This function temporarily suspends logging. No log messages will be recorded until logging is resumed using `log_resume`.

- **Parameters**:  
  - `config`: A pointer to the `Log` configuration object.

- **Return Value**:  
  - None (void function).

- **Error Handling**:  
  Logs an error if `config` is `NULL`.

---

### `void log_resume(Log* config)`

- **Purpose**:  
  This function resumes logging after it has been suspended. Once resumed, log messages will start being recorded again.

- **Parameters**:  
  - `config`: A pointer to the `Log` configuration object.

- **Return Value**:  
  - None (void function).

- **Error Handling**:  
  Logs an error if `config` is `NULL`.

---

### `bool log_set_format(Log* config, const char* format)`

- **Purpose**:  
  This function allows setting a custom format for log messages. The format string should follow `printf`-like formatting rules and should include placeholders for timestamp, log level, and message text.

- **Parameters**:  
  - `config`: A pointer to the `Log` configuration object.
  - `format`: A `printf`-like format string to be used for log messages.

- **Return Value**:  
  - `true`: If the format was successfully set.
  - `false`: If `config` or `format` is `NULL`.

- **Error Handling**:  
  Logs an error if `config` or `format` is invalid.

---

### `bool log_toggle_level_visibility(Log* config, LogLevel level, bool visible)`

- **Purpose**:  
  This function enables or disables the visibility of a specific log level dynamically. It allows you to show or hide log messages for a specific level (e.g., disable `DEBUG` messages while keeping `INFO` and `ERROR` messages visible).

- **Parameters**:  
  - `config`: A pointer to the `Log` configuration object.
  - `level`: The log level to toggle visibility for (e.g., `LOG_LEVEL_DEBUG`, `LOG_LEVEL_INFO`).
  - `visible`: A boolean flag to enable (`true`) or disable (`false`) visibility for the specified log level.

- **Return Value**:  
  - `true`: If the visibility for the specified log level was successfully toggled.
  - `false`: If `config` is `NULL` or the specified log level is invalid.

- **Error Handling**:  
  Logs an error if `config` is `NULL` or if the specified log level is out of bounds.

---

### `bool log_redirect_output(Log* config, const char* newFilePath)`

- **Purpose**:  
  This function allows dynamically changing the log file during runtime without restarting the application. It closes the current log file (if open) and redirects the log output to the new file specified by `newFilePath`.

- **Parameters**:  
  - `config`: A pointer to the `Log` configuration object.
  - `newFilePath`: The path to the new log file where future log messages will be written.

- **Return Value**:  
  - `true`: If the log output was successfully redirected to the new file.
  - `false`: If `config` is `NULL`, the `newFilePath` is invalid, or an error occurs while opening the new file.

- **Error Handling**:  
  Logs an error if the `config` is `NULL`, the `newFilePath` is invalid, or the new log file cannot be opened.

---

### `bool log_set_verbose(Log* config, bool verbose)`

- **Purpose**:  
  This function enables or disables verbose logging. When verbose logging is enabled, all log levels (including `DEBUG` and `INFO`) are logged. When disabled, only higher-severity log levels (`WARN`, `ERROR`, `FATAL`) are logged.

- **Parameters**:  
  - `config`: A pointer to the `Log` configuration object.
  - `verbose`: A boolean value. `true` to enable verbose logging; `false` to disable verbose logging.

- **Return Value**:  
  - `true`: If the verbosity setting was successfully applied.
  - `false`: If `config` is `NULL`.

- **Error Handling**:  
  Logs an error if `config` is `NULL`.

---

### `bool log_set_custom_filter(Log* config, LogFilterFunction filter, void* user_data)`

- **Purpose**:  
  This function sets a custom filter for log messages. The filter function, provided by the user, will be called for each log message. If the filter returns `true`, the message will be logged; if `false`, it will be ignored.

- **Parameters**:  
  - `config`: A pointer to the `Log` configuration object.
  - `filter`: A function pointer to the custom filter function.
  - `user_data`: A pointer to user-defined data to be passed to the filter function.

- **Return Value**:  
  - `true`: If the custom filter was set successfully.
  - `false`: If `config` is `NULL`.

- **Error Handling**:  
  Logs an error if `config` is `NULL`.

---

### `bool log_set_max_file_size(Log* config, size_t maxSize, const char* archivePathFormat)`

- **Purpose**:  
  This function sets a maximum log file size. If the current log file exceeds the specified size, it is archived (renamed or moved), and a new log file is created. The archived log file is renamed using the format specified by `archivePathFormat`.

- **Parameters**:  
  - `config`: A pointer to the `Log` configuration object.
  - `maxSize`: The maximum size (in bytes) allowed for the log file before rotation.
  - `archivePathFormat`: A format string used to generate the name for the archived log file (e.g., `log-%Y%m%d-%H%M%S.txt` for time-based naming).

- **Return Value**:  
  - `true`: If the maximum file size was set and log rotation was performed as needed.
  - `false`: If `config` is `NULL`, `config->file_writer` is `NULL`, `maxSize` is zero, or there was an error during file operations.

- **Error Handling**:  
  Logs an error if the file size cannot be retrieved, the log file cannot be archived, or the new log file cannot be created.

---

## Example 1 : initialize logger with `log_init`

```c
#include "log/log.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Log* config = log_init();
    
    if (!config) {
        fmt_fprintf(stderr, "Error: Can not Create log object.\n");
        exit(-1);
    }
    else {
      fmt_println("Successfully Initialize Log");
    }

    return 0;
}
```
**Result**
```
Successfully Initialize Log
```

---

## Example 2 : different log level 

```c
#include "log/log.h"
#include "fmt/fmt.h"

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
**Result**
```
[DEBUG] - This is a debug message - might not be displayed.
[INFO] - This is an info message.
[WARN] - This is a warning message.
[ERROR] - This is an error message.
```

---

## Example 3 : Logging with Timestamps Enabled 

```c
#include "log/log.h"
#include "fmt/fmt.h"

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
**Result**
```
2024-11-28 00:08:40 [INFO] - Application started with timestamp.
2024-11-28 00:08:40 [WARN] - Low disk space warning logged with timestamp.
2024-11-28 00:08:40 [ERROR] - File not found error logged with timestamp.
```

---

## Example 4 : set log level with `log_set_log_level`

```c
#include "log/log.h"
#include "fmt/fmt.h"

int main() {
    Log* logger = log_init();

    // Initially set the log level to INFO
    log_set_log_level(logger, LOG_LEVEL_INFO);
    log_message(logger, LOG_LEVEL_WARN, "This is an info message. Debug messages will not be shown.");

    // Change log level to DEBUG to see more detailed logging
    log_set_log_level(logger, LOG_LEVEL_DEBUG);
    log_message(logger, LOG_LEVEL_DEBUG, "Debug level set. This debug message is now visible.");

    log_deallocate(logger);
    return 0;
}
```
**Result**
```
 [WARN] - This is an info message. Debug messages will not be shown.
 [DEBUG] - Debug level set. This debug message is now visible.
```

---

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
**Result**
```
 [ERROR] - An error occurred.
 [INFO] - Info level log, keyword filter disabled.
```

---

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
**Result**
```
 [INFO] - Initial info message.
 [ERROR] - Initial error message.
 [ERROR] - An error occurred after update.
 [INFO] - Info level log, keyword filter disabled.
```

---

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
**Result**
```
 [INFO] - Logging to the initial file.
 [INFO] - Logging to the new file.
```

---

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
**Result**
```
 [ERROR] - An unexpected error occurred.
```

---

## Example 9 : log rotate 

```c
#include "log/log.h"
#include "fmt/fmt.h"

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
**Result**
```
 [INFO] - This is an informational message.
```

---

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
**Result**
```
 [INFO] - Logging has been resumed.
```

---

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
**Result**
```
[] [INFO] - Custom formatted log message.
```

---

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
**Result**
```
 [DEBUG] - This is a debug message.
 [INFO] - This is an info message.
 [WARN] - This is a warning message.
 [ERROR] - This is an error message.
 [WARN] - This WARNING message is still visible.
 [ERROR] - This ERROR message is still visible.
 [DEBUG] - DEBUG messages are visible again.
 [INFO] - INFO messages are visible again.
```

---

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
**Result**
```
 [INFO] - Logging to the initial file.
 [INFO] - Logging to a new file after redirection.
```

---

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
**Result**
```
 [DEBUG] - This is a verbose debug message.
 [INFO] - This is a verbose info message.
 [WARN] - This is a warning message.
 [ERROR] - This is an error message.
 [WARN] - This warning message is still visible.
 [ERROR] - This error message is still visible.
```

---

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
**Result**
```
[INFO] - This is an important info message.
```

---

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
**Result**
```
 [INFO] - Log message number 927
 [INFO] - Log message number 928
 [INFO] - Log message number 929
 [INFO] - Log message number 930
 [INFO] - Log message number 931
```

---

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
**Result**
```
 [INFO] - This is info message 0
 [DEBUG] - This is debug message 0
 [WARN] - This is a warning message 0
 [INFO] - This is info message 1
 [DEBUG] - This is debug message 1
 [INFO] - This is info message 2
 [DEBUG] - This is debug message 2
 [WARN] - This is a warning message 2
 [INFO] - This is info message 3
 [DEBUG] - This is debug message 3
 [INFO] - This is info message 4
 [DEBUG] - This is debug message 4
 [WARN] - This is a warning message 4
```

---

## License

This project is open-source and available under [ISC License].