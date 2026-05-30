/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Log
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in log.c (file-level convention).
 *
 * Lightweight leveled logger with console+file output, timestamp /
 * level visibility toggles, keyword filtering, rate limiting and
 * size-based rotation.
 */

#ifndef LOG_H_
#define LOG_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "../file_io/file_writer.h"
#include "../file_io/file_reader.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ------------------------------------------------------------------ */
/* Limits                                                             */
/* ------------------------------------------------------------------ */

#define MAX_KEYWORD_LENGTH 512
#define LOG_MAX_PATH       1024


/* #define LOG_LOGGING_ENABLE */

#ifdef LOG_LOGGING_ENABLE
    #define LOG_LOG(fmt, ...) \
        do { fprintf(stderr, "[LOG LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define LOG_LOG(...) do { } while (0)
#endif



typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} LogLevel;


typedef enum {
    LOG_OUTPUT_CONSOLE,
    LOG_OUTPUT_FILE,
    LOG_OUTPUT_BOTH
} LogOutput;

/* Optional per-record filter callback. Return true to keep, false to
 * drop. `user_data` is the cookie passed to `log_set_custom_filter`. */
typedef bool (*LogFilterFunction)(LogLevel level, const char* message, void* user_data);

typedef struct {
    LogLevel           level;
    LogOutput          output;
    FileWriter*        file_writer;
    FileReader*        file_reader;
    bool               enable_timestamp;
    bool               enable_log_level;
    char               keyword_filter[MAX_KEYWORD_LENGTH];
    bool               is_keyword_filter_enabled;
    bool               suspended;
    char               format[256];
    bool               level_visibility[LOG_LEVEL_FATAL + 1];
    LogFilterFunction  custom_filter;
    void*              custom_filter_user_data;
    unsigned int       rate_limit_interval;            /* seconds                              */
    unsigned int       rate_limit_count;               /* max logs per interval                */
    unsigned int       log_counts[LOG_LEVEL_FATAL + 1];/* counts in the current interval       */
    unsigned long      total_log_counts[LOG_LEVEL_FATAL + 1];/* cumulative emitted, per level   */
    time_t             last_reset_time;                /* last interval reset                  */
    char               current_file_path[LOG_MAX_PATH];/* currently open log file              */
} Log;


/* ------------------------------------------------------------------ */
/* Construction / destruction                                         */
/* ------------------------------------------------------------------ */

Log*    log_init                          (void);
void    log_deallocate                    (Log* config);


/* ------------------------------------------------------------------ */
/* Emitting                                                           */
/* ------------------------------------------------------------------ */

void    log_message                       (Log* config, LogLevel level, const char* message, ...);
void    log_flush                         (Log* config);
void    log_suspend                       (Log* config);
void    log_resume                        (Log* config);


/* ------------------------------------------------------------------ */
/* Output / sink                                                      */
/* ------------------------------------------------------------------ */

bool    log_set_output                    (Log* config, LogOutput output);
bool    log_set_file_path                 (Log* config, const char* newFilePath);
bool    log_redirect_output               (Log* config, const char* newFilePath);
bool    log_rotate                        (Log* config, const char* newLogPath, size_t maxSize);
bool    log_set_max_file_size             (Log* config, size_t maxSize, const char* archivePathFormat);


/* ------------------------------------------------------------------ */
/* Formatting / level visibility                                      */
/* ------------------------------------------------------------------ */

bool    log_set_log_level                 (Log* config, LogLevel newLevel);
bool    log_set_format                    (Log* config, const char* format);
bool    log_enable_timestamp              (Log* config, bool enable);
bool    log_toggle_level_visibility       (Log* config, LogLevel level, bool visible);
bool    log_set_verbose                   (Log* config, bool verbose);


/* ------------------------------------------------------------------ */
/* Filtering                                                          */
/* ------------------------------------------------------------------ */

bool    log_enable_keyword_filter         (Log* config, const char* keyword, bool enable);
bool    log_update_keyword_filter         (Log* config, const char* newKeyword);
bool    log_set_custom_filter             (Log* config, LogFilterFunction filter, void* user_data);


/* ------------------------------------------------------------------ */
/* Introspection                                                      */
/* ------------------------------------------------------------------ */

const char*   log_level_to_string         (LogLevel level);
bool          log_set_level_from_string   (Log* config, const char* level_str);
bool          log_is_level_enabled        (const Log* config, LogLevel level);
bool          log_set_rate_limit          (Log* config, unsigned int max_logs_per_interval, unsigned int interval_seconds);
unsigned long log_get_message_count       (const Log* config, LogLevel level);


#ifdef __cplusplus
}
#endif

#endif 
