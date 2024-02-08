
## Example 1 : initialize logger with `log_init`

```c
#include "log/log.h"

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
    if (!logger) {
        fmt_fprintf(stderr, "Failed to initialize logging system.\n");
        return -1;
    }

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
    if (!logger) {
        fmt_fprintf(stderr, "Failed to initialize logging system.\n");
        return -1;
    }

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
    if (!logger) {
        fmt_fprintf(stderr, "Failed to initialize logging system.\n");
        return -1;
    }

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
    if (!logger) {
        fmt_fprintf(stderr, "Failed to initialize logging system.\n");
        return -1;
    }

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
    if (!logger) {
        fmt_fprintf(stderr, "Failed to initialize logging system.\n");
        return -1;
    }

    log_message(logger, LOG_LEVEL_ERROR, "An unexpected error occurred.");

    // Flush logs to ensure the message is written out immediately
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
    if (!logger) {
        fmt_fprintf(stderr, "Failed to initialize logging system.\n");
        return -1;
    }

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
    if (!logger) {
        fmt_fprintf(stderr, "Failed to initialize logging system.\n");
        return -1;
    }

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
    if (!logger) {
        fmt_fprintf(stderr, "Failed to initialize logging system.\n");
        return -1;
    }

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
    if (!logger) {
        fmt_fprintf(stderr, "Failed to initialize logging system.\n");
        return -1;
    }

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
    if (!logger) {
        fmt_fprintf(stderr, "Failed to initialize logging system.\n");
        return -1;
    }

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
    if (!logger) {
        fmt_fprintf(stderr, "Failed to initialize logging system.\n");
        return -1;
    }

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