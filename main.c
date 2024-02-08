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
