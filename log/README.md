
# Logging Library in C 

**Author:** Amin Tahmasebi
**Release Date:** 2024
**License:** ISC License


## Overview

The logging system is designed to provide a flexible and configurable way to handle logging in C applications. It supports various features such as logging to files and console, setting log levels, filtering logs by keywords, custom formatting, rate limiting, log rotation, and more. This system is essential for tracking application behavior, debugging, and monitoring.

### Key Features:
- **Multiple Log Levels:** Control the verbosity of the logs (e.g., DEBUG, INFO, WARN, ERROR, FATAL).
- **Thread-Safe:** Every logger owns an internal recursive mutex, so concurrent `log_message` calls from many threads never interleave (no torn lines) and the per-level counters stay exact. Timestamps use the reentrant `localtime_r`/`localtime_s`. Safe for production multi-threaded services out of the box.
- **Convenience Macros:** `log_debug`, `log_info`, `log_warn`, `log_error`, `log_fatal` — one-call shorthands for `log_message` at each level.
- **Output Options:** Logs can be written to the console, files, or both. ERROR/FATAL console output can be routed to **stderr** with `log_set_error_stream`.
- **Crash Durability:** `log_set_auto_flush` forces a flush after every emitted record so the most recent lines survive a crash.
- **Timestamping:** Add timestamps to log entries.
- **Keyword Filtering:** Display only logs containing specific keywords.
- **Custom Formatting:** Define how log messages are structured.
- **Rate Limiting:** Limit the number of log messages within a specified time frame. Only active when **both** `rate_limit_interval > 0` **and** `rate_limit_count > 0`.
- **Log Rotation:** Automatically archive and rotate log files when they reach a certain size. Operates on the currently-active log file (not a hardcoded path), so it works correctly after `log_set_file_path` / `log_redirect_output`.
- **Custom Filters:** Apply custom logic to filter which messages should be logged.
- **Suspend/Resume Logging:** Temporarily pause and resume logging.

## Function Explanations

### `Log* log_init(void)`
**Purpose**: Creates and initializes a `Log` configuration object with default settings (level=DEBUG, output=BOTH, timestamp=disabled, all levels visible, rate-limiting disabled).
**Parameters**: None.
**Return Value**: A pointer to the new `Log`, or `NULL` if allocation fails.
**Usage Case**: Call once at program startup before any logging to obtain a configuration handle; check the return value for `NULL` before proceeding.

---

### `void log_deallocate(Log* config)`
**Purpose**: Releases all resources held by the `Log` object: closes the file writer and file reader if open, then frees the struct.
**Parameters**:
- `config`: Pointer to the `Log` object to destroy.
**Return Value**: None.
**Usage Case**: Call when the application is done logging to avoid resource leaks; do not use `config` after this call.

---

### `void log_message(Log* config, LogLevel level, const char* message, ...)`
**Purpose**: Emits a formatted log message at the given level, respecting all active filters (suspend state, minimum level, visibility, keyword filter, custom filter, and rate limit).
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
- `level`: Severity level of the message (e.g., `LOG_LEVEL_DEBUG`, `LOG_LEVEL_INFO`).
- `message`: A `printf`-style format string followed by optional variadic arguments.
**Return Value**: None.
**Usage Case**: Use throughout application code to emit diagnostic and operational messages; the function silently skips messages that do not pass the configured checks.

---

### `void log_flush(Log* config)`
**Purpose**: Flushes any buffered log output to its destination(s) by calling `fflush` on the file writer and/or stdout.
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
**Return Value**: None.
**Usage Case**: Call after writing critical messages when you need them persisted to disk or visible on the terminal immediately, such as before a potential crash or long blocking operation.

---

### `void log_suspend(Log* config)`
**Purpose**: Temporarily pauses all log output so that messages sent while suspended are silently discarded.
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
**Return Value**: None.
**Usage Case**: Use to suppress noise during a known high-frequency or irrelevant code path without removing log calls, then re-enable with `log_resume`.

---

### `void log_resume(Log* config)`
**Purpose**: Re-enables log output after a call to `log_suspend`.
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
**Return Value**: None.
**Usage Case**: Call after `log_suspend` to restore normal logging once the suppressed code path has completed.

---

### `bool log_set_output(Log* config, LogOutput output)`
**Purpose**: Sets the destination for log messages to console, file, or both, opening `log.txt` automatically if file output is requested and no file is currently open.
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
- `output`: Desired destination: `LOG_OUTPUT_CONSOLE`, `LOG_OUTPUT_FILE`, or `LOG_OUTPUT_BOTH`.
**Return Value**: `true` on success, `false` if `config` is `NULL` or the log file cannot be opened.
**Usage Case**: Switch log destinations at runtime, for example redirecting to file only in production and to console only during interactive debugging.

---

### `bool log_set_file_path(Log* config, const char* newFilePath)`
**Purpose**: Closes the currently open log file (if any) and opens a new one at `newFilePath`, falling back to console output if the new file cannot be opened.
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
- `newFilePath`: Path to the new log file; must not be `NULL` or empty.
**Return Value**: `true` on success, `false` if `config` is `NULL`, `newFilePath` is `NULL`/empty, or the new file cannot be opened.
**Usage Case**: Redirect log output to a different file at runtime, such as rolling to a date-stamped filename at midnight.

---

### `bool log_redirect_output(Log* config, const char* newFilePath)`
**Purpose**: Closes the current log file and redirects all future output to the file at `newFilePath` without restarting the application.
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
- `newFilePath`: Path to the new log file destination.
**Return Value**: `true` on success, `false` if `config` is `NULL`, `newFilePath` is invalid, or the new file cannot be opened.
**Usage Case**: Dynamically change the log file at runtime in long-running services where a restart is not acceptable.

---

### `bool log_rotate(Log* config, const char* newLogPath, size_t maxSize)`
**Purpose**: Checks whether the current log file exceeds `maxSize` bytes and, if so, renames it to `newLogPath` and opens a fresh log file at the original path.
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
- `newLogPath`: Destination path for the archived (rotated) log file.
- `maxSize`: Maximum allowed file size in bytes before rotation is triggered.
**Return Value**: `true` if rotation was performed or the file is still under the limit, `false` on any I/O error.
**Usage Case**: Call periodically (e.g., on each log write or via a timer) to cap log file size and archive older entries automatically.

---

### `bool log_set_max_file_size(Log* config, size_t maxSize, const char* archivePathFormat)`
**Purpose**: Sets a maximum file size and a `strftime`-style archive name pattern; if the current file exceeds `maxSize` on each call it is renamed using the pattern and a new file is opened.
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
- `maxSize`: Maximum size in bytes before the file is archived; must be greater than zero.
- `archivePathFormat`: A `strftime`-style format string for the archive filename (e.g., `"log_%Y%m%d_%H%M%S.txt"`).
**Return Value**: `true` on success, `false` if `config` or `config->file_writer` is `NULL`, `maxSize` is zero, or a file operation fails.
**Usage Case**: Enable automatic time-stamped log archiving so that log files never grow beyond a fixed size.

---

### `bool log_set_error_stream(Log* config, bool errors_to_stderr)`
**Purpose**: Routes high-severity console output (`LOG_LEVEL_ERROR` and `LOG_LEVEL_FATAL`) to **stderr** instead of stdout. Affects console output only (`LOG_OUTPUT_CONSOLE` / `LOG_OUTPUT_BOTH`); the file sink is unchanged. Thread-safe. Off by default (all console output goes to stdout).
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
- `errors_to_stderr`: `true` to send ERROR/FATAL to stderr, `false` to send all console output to stdout (the default).
**Return Value**: `true` on success, `false` if `config` is `NULL`.
**Usage Case**: In a production service, keep error/fatal lines visible and separately collectable when stdout is piped or captured (e.g. `./app >app.log 2>errors.log`).

---

### `bool log_set_auto_flush(Log* config, bool enable)`
**Purpose**: When enabled, flushes the destination stream (console and/or file) immediately after every record that is actually written, so the most recent lines survive a crash or `kill`. Trades throughput for durability. Thread-safe. Off by default.
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
- `enable`: `true` to flush after every emitted record, `false` to rely on normal stdio buffering (the default).
**Return Value**: `true` on success, `false` if `config` is `NULL`.
**Usage Case**: Enable on a low-volume audit/error log where losing the last few lines on a crash is unacceptable; leave off for high-volume logging and call `log_flush` at checkpoints instead.

---

### Convenience macros: `log_debug` / `log_info` / `log_warn` / `log_error` / `log_fatal`
**Purpose**: Per-level shorthands for `log_message`, defined in `log.h`. `log_info(logger, "x=%d", x)` is exactly `log_message(logger, LOG_LEVEL_INFO, "x=%d", x)`.
**Parameters**: `(Log* config, const char* format, ...)` — a format string is required (at least one argument after `config`).
**Return Value**: None (same as `log_message`).
**Usage Case**: Cut boilerplate at call sites: `log_error(logger, "connect failed: %s", err)` instead of spelling out the level enum every time.

---

### Thread safety
Every `Log` created by `log_init` is **thread-safe**: an internal recursive mutex serializes `log_message` and all file-handle operations (`log_rotate`, `log_redirect_output`, `log_set_file_path`, `log_set_max_file_size`, `log_set_output`, `log_flush`) and protects the per-level counters. Concurrent logging from many threads produces interleave-free lines and exact counts. You can share one `Log*` across all threads; no external locking is required. (Timestamps use the reentrant `localtime_r` / `localtime_s`.)

---

### `bool log_set_log_level(Log* config, LogLevel newLevel)`
**Purpose**: Changes the minimum log level so that only messages at `newLevel` or higher are emitted.
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
- `newLevel`: New minimum level; must be one of `LOG_LEVEL_DEBUG`, `LOG_LEVEL_INFO`, `LOG_LEVEL_WARN`, `LOG_LEVEL_ERROR`, `LOG_LEVEL_FATAL`.
**Return Value**: `true` on success, `false` if `config` is `NULL` or the level is invalid.
**Usage Case**: Increase the minimum level in production to suppress debug/info noise, or lower it temporarily when diagnosing an issue.

---

### `bool log_set_format(Log* config, const char* format)`
**Purpose**: Sets a custom `printf`-style format string for log records containing three `%s` placeholders for timestamp, level, and message.
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
- `format`: Format string with three `%s` placeholders (e.g., `"%s [%s] - %s"`).
**Return Value**: `true` on success, `false` if `config` or `format` is `NULL`.
**Usage Case**: Tailor log record layout to match the expected input format of a log aggregation or parsing tool.

---

### `bool log_enable_timestamp(Log* config, bool enable)`
**Purpose**: Enables or disables prepending a `YYYY-MM-DD HH:MM:SS` timestamp to each log line.
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
- `enable`: `true` to enable timestamps, `false` to disable them.
**Return Value**: `true` on success, `false` if `config` is `NULL`.
**Usage Case**: Enable timestamps in file-based logs for post-mortem analysis, or disable them in console output where the terminal already shows time.

---

### `bool log_toggle_level_visibility(Log* config, LogLevel level, bool visible)`
**Purpose**: Shows or hides messages for a specific log level without changing the minimum level, allowing fine-grained suppression of individual levels.
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
- `level`: The log level whose visibility is being changed.
- `visible`: `true` to show messages at this level, `false` to hide them.
**Return Value**: `true` on success, `false` if `config` is `NULL` or `level` is out of range.
**Usage Case**: Suppress a single noisy level (e.g., hide `LOG_LEVEL_DEBUG` only) while continuing to emit all other levels.

---

### `bool log_set_verbose(Log* config, bool verbose)`
**Purpose**: Sets verbose mode: `true` lowers the minimum level to `LOG_LEVEL_DEBUG` (all messages visible); `false` raises it to `LOG_LEVEL_WARN` (only WARN, ERROR, FATAL visible).
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
- `verbose`: `true` to enable verbose output, `false` to restrict to warnings and above.
**Return Value**: `true` on success, `false` if `config` is `NULL`.
**Usage Case**: Toggle between a developer-friendly verbose mode and a quieter production mode with a single call.

---

### `bool log_enable_keyword_filter(Log* config, const char* keyword, bool enable)`
**Purpose**: Enables or disables keyword filtering so that only messages containing `keyword` as a substring are logged.
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
- `keyword`: Substring to match against; must not be `NULL` or empty when `enable` is `true`.
- `enable`: `true` to activate the filter, `false` to clear it (the `keyword` value is ignored when disabling).
**Return Value**: `true` on success, `false` if `config` is `NULL` or `keyword` is `NULL`/empty when enabling.
**Usage Case**: Focus log output on messages related to a specific component or event type during targeted debugging.

---

### `bool log_update_keyword_filter(Log* config, const char* newKeyword)`
**Purpose**: Replaces the current keyword filter with `newKeyword`; passing an empty string disables filtering.
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
- `newKeyword`: New keyword substring to filter on; pass an empty string to disable the filter.
**Return Value**: `true` on success, `false` if `config` is `NULL`, `newKeyword` is `NULL`, or the keyword length exceeds `MAX_KEYWORD_LENGTH`.
**Usage Case**: Switch the active keyword filter at runtime without disabling and re-enabling it manually.

---

### `bool log_set_custom_filter(Log* config, LogFilterFunction filter, void* user_data)`
**Purpose**: Installs a user-supplied callback (`bool filter(LogLevel, const char* message, void* user_data)`) that is invoked for each message after all other checks pass; returning `false` discards the message.
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
- `filter`: Callback function pointer; pass `NULL` to remove the current custom filter.
- `user_data`: Arbitrary pointer forwarded to the callback on every invocation.
**Return Value**: `true` on success, `false` if `config` is `NULL`.
**Usage Case**: Apply application-specific logic (e.g., routing ERROR messages to an alerting system) that cannot be expressed with the built-in filters.

---

### `const char* log_level_to_string(LogLevel level)`
**Purpose**: Returns the human-readable name of a log level — `"DEBUG"`, `"INFO"`, `"WARN"`, `"ERROR"` or `"FATAL"`. Any value outside the enum yields `"UNKNOWN"`. This is the inverse of `log_set_level_from_string`.
**Parameters**:
- `level`: The log level to name.
**Return Value**: A pointer to a static string literal — **do not free or modify it**.
**Usage Case**: Print or serialise a level (e.g. in a status line, metrics label, or your own custom output sink).

---

### `bool log_set_level_from_string(Log* config, const char* level_str)`
**Purpose**: Sets the minimum log level from a case-insensitive name (`"debug"`, `"info"`, `"warn"`, `"error"`, `"fatal"`). Ideal for driving the level from an environment variable or config file.
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
- `level_str`: The level name (any letter case).
**Return Value**: `true` if the name was recognised and the level updated; `false` on a `NULL` argument or an unrecognised name (the level is then left unchanged).
**Usage Case**: `log_set_level_from_string(logger, getenv("LOG_LEVEL"))` to make verbosity configurable without recompiling.

---

### `bool log_is_level_enabled(const Log* config, LogLevel level)`
**Purpose**: Reports whether a message at `level` would currently be emitted, considering the level-based gates `log_message` applies first: not suspended, `level >= ` the configured minimum, and that level's visibility enabled. (Content-dependent keyword/custom filters and rate limiting are not considered, since they depend on the specific message or timing.)
**Parameters**:
- `config`: Pointer to the `Log` configuration object (may be `NULL` → returns `false`).
- `level`: The level to test.
**Return Value**: `true` if such a message would pass the level gates, otherwise `false`.
**Usage Case**: Guard expensive message construction — `if (log_is_level_enabled(lg, LOG_LEVEL_DEBUG)) { /* build costly debug string */ }`.

---

### `bool log_set_rate_limit(Log* config, unsigned int max_logs_per_interval, unsigned int interval_seconds)`
**Purpose**: Configures rate limiting: at most `max_logs_per_interval` messages **of each level** per `interval_seconds` window. This is the public setter for the logger's built-in per-level rate limiter. Rate limiting is active only when both values are non-zero — passing `0, 0` disables it. Calling this starts a fresh window so the new limit takes effect immediately.
**Parameters**:
- `config`: Pointer to the `Log` configuration object.
- `max_logs_per_interval`: Maximum messages per level per window (`0` = off).
- `interval_seconds`: Window length in seconds (`0` = off).
**Return Value**: `true` on success, `false` if `config` is `NULL`.
**Usage Case**: Protect log sinks (and disk) from runaway loops or noisy error storms in production.

---

### `unsigned long log_get_message_count(const Log* config, LogLevel level)`
**Purpose**: Returns the cumulative number of messages that were actually emitted at `level` since the logger was created. Only messages that passed every gate and filter are counted; the count is monotonic (never reset by rate-limit windows).
**Parameters**:
- `config`: Pointer to the `Log` configuration object (may be `NULL` → returns `0`).
- `level`: The level to query.
**Return Value**: The number of emitted messages at `level`, or `0` for a `NULL` config or an out-of-range level.
**Usage Case**: Lightweight metrics/health checks — e.g. export per-level counters, or assert in tests that exactly N errors were logged.

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

    log_deallocate(config);
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

## Example 15 : set custom filter on logger `log_set_custom_filter`

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

## Example 18 : name a log level with `log_level_to_string`

```c
#include "log/log.h"
#include "fmt/fmt.h"

int main() {
    for (LogLevel lvl = LOG_LEVEL_DEBUG; lvl <= LOG_LEVEL_FATAL; ++lvl) {
        fmt_printf("%d -> %s\n", (int)lvl, log_level_to_string(lvl));
    }
    return 0;
}
```
**Result**
```
0 -> DEBUG
1 -> INFO
2 -> WARN
3 -> ERROR
4 -> FATAL
```

---

## Example 19 : set the level from a string with `log_set_level_from_string`

```c
#include "log/log.h"
#include "fmt/fmt.h"

int main() {
    Log* logger = log_init();
    log_set_output(logger, LOG_OUTPUT_CONSOLE);

    // Drive the threshold from a string, e.g. an env var like LOG_LEVEL=warn
    if (log_set_level_from_string(logger, "warn")) {
        fmt_printf("Level set to %s\n", log_level_to_string(logger->level));
    }

    log_message(logger, LOG_LEVEL_INFO,  "info is below threshold, dropped");
    log_message(logger, LOG_LEVEL_ERROR, "error is shown");

    log_deallocate(logger);
    return 0;
}
```
**Result**
```
Level set to WARN
 [ERROR] - error is shown
```

---

## Example 20 : skip expensive work with `log_is_level_enabled`

```c
#include "log/log.h"
#include "fmt/fmt.h"

int main() {
    Log* logger = log_init();
    log_set_output(logger, LOG_OUTPUT_CONSOLE);
    log_set_log_level(logger, LOG_LEVEL_WARN);

    if (log_is_level_enabled(logger, LOG_LEVEL_DEBUG)) {
        log_message(logger, LOG_LEVEL_DEBUG, "expensive debug: %d", 42);
    } else {
        fmt_printf("DEBUG disabled -> skipped building the message\n");
    }

    if (log_is_level_enabled(logger, LOG_LEVEL_ERROR)) {
        log_message(logger, LOG_LEVEL_ERROR, "error path active");
    }

    log_deallocate(logger);
    return 0;
}
```
**Result**
```
DEBUG disabled -> skipped building the message
 [ERROR] - error path active
```

---

## Example 21 : cap log volume with `log_set_rate_limit`

```c
#include "log/log.h"
#include "fmt/fmt.h"

int main() {
    Log* logger = log_init();
    log_set_output(logger, LOG_OUTPUT_CONSOLE);

    // At most 3 messages per level per 60-second window.
    log_set_rate_limit(logger, 3, 60);

    for (int i = 0; i < 10; ++i) {
        log_message(logger, LOG_LEVEL_INFO, "attempt %d", i);
    }

    log_deallocate(logger);
    return 0;
}
```
**Result**
```
 [INFO] - attempt 0
 [INFO] - attempt 1
 [INFO] - attempt 2
```

---

## Example 22 : count emitted messages with `log_get_message_count`

```c
#include "log/log.h"
#include "fmt/fmt.h"

int main() {
    Log* logger = log_init();
    log_set_output(logger, LOG_OUTPUT_CONSOLE);
    log_set_log_level(logger, LOG_LEVEL_WARN);   // DEBUG/INFO are dropped

    log_message(logger, LOG_LEVEL_DEBUG, "dropped");
    log_message(logger, LOG_LEVEL_WARN,  "first warning");
    log_message(logger, LOG_LEVEL_WARN,  "second warning");
    log_message(logger, LOG_LEVEL_ERROR, "an error");

    fmt_printf("WARN logged : %lu\n", log_get_message_count(logger, LOG_LEVEL_WARN));
    fmt_printf("ERROR logged: %lu\n", log_get_message_count(logger, LOG_LEVEL_ERROR));
    fmt_printf("DEBUG logged: %lu\n", log_get_message_count(logger, LOG_LEVEL_DEBUG));

    log_deallocate(logger);
    return 0;
}
```
**Result**
```
 [WARN] - first warning
 [WARN] - second warning
 [ERROR] - an error
WARN logged : 2
ERROR logged: 1
DEBUG logged: 0
```

---

## Example 23 : per-level convenience macros `log_debug` / `log_info` / `log_warn` / `log_error` / `log_fatal`

```c
#include "log/log.h"

int main() {
    Log* logger = log_init();
    log_set_output(logger, LOG_OUTPUT_CONSOLE);

    int port = 8080;
    log_debug(logger, "starting up on port %d", port);
    log_info (logger, "service ready");
    log_warn (logger, "cache hit-rate low: %d%%", 42);
    log_error(logger, "db connection failed: %s", "timeout");
    log_fatal(logger, "unrecoverable - shutting down");

    log_deallocate(logger);
    return 0;
}
```
**Result**
```
 [DEBUG] - starting up on port 8080
 [INFO] - service ready
 [WARN] - cache hit-rate low: 42%
 [ERROR] - db connection failed: timeout
 [FATAL] - unrecoverable - shutting down
```

---

## Example 24 : route errors to stderr + crash-durable flushing with `log_set_error_stream` and `log_set_auto_flush`

```c
#include "log/log.h"

int main() {
    Log* logger = log_init();
    log_set_output(logger, LOG_OUTPUT_CONSOLE);

    /* Route ERROR/FATAL to stderr; flush every record so nothing is lost on a crash. */
    log_set_error_stream(logger, true);
    log_set_auto_flush(logger, true);

    log_info (logger, "startup complete");                     /* -> stdout */
    log_warn (logger, "config file missing, using defaults");  /* -> stdout */
    log_error(logger, "payment gateway unreachable");          /* -> stderr */
    log_fatal(logger, "out of memory");                        /* -> stderr */

    log_deallocate(logger);
    return 0;
}
```
**Result** — run as `./app >out.txt 2>err.txt`, the two streams separate cleanly:
```
# out.txt (stdout)
 [INFO] - startup complete
 [WARN] - config file missing, using defaults

# err.txt (stderr)
 [ERROR] - payment gateway unreachable
 [FATAL] - out of memory
```
> **Why it matters:** in production you usually want operational chatter on stdout and real problems on stderr, so an alerting pipe (`2>`) only sees the errors. `auto_flush` guarantees the last line is on disk even if the process is killed immediately after.

---

## Example 25 : thread-safe concurrent logging (one `Log*` shared across threads)

```c
#include "log/log.h"
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

#define THREADS 4
#define PER     3

typedef struct { Log* logger; int id; } WorkerArg;

static int worker(void* arg) {
    WorkerArg* w = (WorkerArg*)arg;
    for (int i = 0; i < PER; ++i) {
        log_info(w->logger, "thread %d message %d", w->id, i);
    }
    return 0;
}

int main() {
    Log* logger = log_init();
    log_set_output(logger, LOG_OUTPUT_CONSOLE);

    /* One logger shared by every thread — no external locking required. */
    Thread    t[THREADS];
    WorkerArg args[THREADS];
    for (int i = 0; i < THREADS; ++i) {
        args[i].logger = logger;
        args[i].id     = i;
        thread_create(&t[i], worker, &args[i]);
    }
    for (int i = 0; i < THREADS; ++i) {
        thread_join(t[i], NULL);
    }

    /* Exact even under concurrency: 4 threads x 3 = 12. */
    fmt_printf("total INFO logged: %lu\n", log_get_message_count(logger, LOG_LEVEL_INFO));

    log_deallocate(logger);
    return 0;
}
```
**Result** (the 12 log lines interleave in a non-deterministic order, but every line is intact — never torn or merged — and the final count is always exact)
```
 [INFO] - thread 0 message 0
 [INFO] - thread 0 message 1
 ...
 [INFO] - thread 2 message 2
total INFO logged: 12
```
> **Why it matters:** the internal mutex makes `log_message` and the file-handle operations atomic, so you can hand the same `Log*` to every worker thread without wrapping calls in your own lock. Verified race-free under Helgrind.

---

## License

This project is open-source and available under [ISC License].