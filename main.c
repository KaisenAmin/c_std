#include "log/log.h"

int main() {
    Log* logger = log_init();
    if (!logger) {
        fmt_fprintf(stderr, "Error: Cannot create log object.\n");
        return -1;
    }

    log_message(logger, LOG_LEVEL_DEBUG, "Debugging application start.");
    log_message(logger, LOG_LEVEL_INFO, "Application running.");
    log_message(logger, LOG_LEVEL_WARN, "Low memory warning.");
    log_message(logger, LOG_LEVEL_ERROR, "Error opening file.");

    log_deallocate(logger);
    return 0;
}