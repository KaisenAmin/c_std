#include "log/log.h"


int main() {
    Log* logger = log_init();
    if (!logger) {
        fmt_fprintf(stderr, "Failed to initialize logging system.\n");
        return -1;
    }

    // Set maximum log file size to 5 MB and specify archive naming pattern
    log_set_max_file_size(logger, 5 * 1024 * 1024, "log_archive_%Y-%m-%d_%H-%M-%S.txt");

    for (int i = 0; i < 1000; i++) {
        log_message(logger, LOG_LEVEL_INFO, "Log message number %d", i);
    }

    log_deallocate(logger);
    return 0;
}


