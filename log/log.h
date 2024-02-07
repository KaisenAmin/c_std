#ifndef LOG_H_
#define LOG_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include "../fmt/fmt.h"

#define MAX_KEYWORD_LENGTH 512
#define LOG_ENABLE_LOGGING
// Log levels
typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} LogLevel;

// Log output options
typedef enum {
    LOG_OUTPUT_CONSOLE,
    LOG_OUTPUT_FILE,
    LOG_OUTPUT_BOTH
} LogOutput;

// Log configuration structure
typedef struct {
    LogLevel level;
    LogOutput output;
    FileWriter* file_writer;
    FileReader* file_reader;
    bool enable_timestamp;
    bool enable_log_level;
    char keyword_filter[MAX_KEYWORD_LENGTH]; 
    bool is_keyword_filter_enabled; 
} Log;

// Initialize the logging system
Log* log_init();

// Log a message at a specified level
void log_message(Log* config, LogLevel level, const char* message, ...);

// Set the log output
bool log_set_output(Log* config, LogOutput output);

// Enable or disable timestamps in log messages
bool log_enable_timestamp(Log* config, bool enable);

// Clean up the logging system
void log_deallocate(Log* config);

// set log leve 
bool log_set_log_level(Log* config, LogLevel newLevel);

// Enable filtering of log messages by keywords
bool log_enable_keyword_filter(Log* config, const char* keyword, bool enable);


#endif // LOG_H_
