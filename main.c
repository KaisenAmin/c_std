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