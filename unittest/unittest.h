#ifndef UNITTEST_H_
#define UNITTEST_H_

#if defined(_WIN32)
#include <windows.h>
#include <io.h>
#define F_OK 0
#define access _access

#elif defined(__MACH__) && defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_time.h>
#elif defined(_POSIX_VERSION)
#include <sys/time.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>
#include <float.h>
#include "../regex/std_regex.h"


#ifdef __cplusplus
extern "C" {
#endif

#define MAX_STUBS 100
#define MAX_ARGS 10

// track memory allocations
typedef struct MemoryRecord {
    void* address;
    size_t size;
    const char* file;
    int line;
    struct MemoryRecord* next;
} MemoryRecord;

// Mocking Structure
typedef struct MockCall {
    char* function_name;
    int call_count;
    size_t arg_count;
    char* actual_args[MAX_ARGS];
    int index; 
    struct MockCall* next;
} MockCall;

typedef struct MockRegistry {
    MockCall* head;
} MockRegistry;

// Stubbing
typedef struct {
    const char* function_name;
    const char* arg;  
    void* return_value;
} Stub;

extern size_t total_memory_allocated;

void add_memory_record(void* address, size_t size, const char* file, int line);
void remove_memory_record(void* address);
void print_memory_leaks();
void unittest_free(void* ptr);
void unittest_set_timeout(double timeout_ms);
void unittest_verify_mock_call(const char* function_name, size_t arg_count, void** expected_args, int call_index);
void unittest_verify_mock_call_count(const char* function_name, int expected_count);
void unittest_register_mock_call(const char* function_name, size_t arg_count, void** args);
void unittest_register_stub(const char* function_name, const char* query, void* return_value);
void unittest_free_generated_data(void* data);
void unittest_display_progress(int completed, int total);

void* unittest_malloc(size_t size, const char* file, int line);
void* unittest_get_stub_return_value(const char* function_name, const char* query);

double unittest_timer_real(void);
double unittest_timer_cpu(void);
double unittest_generate_random_double(double min, double max);
double* unittest_generate_edge_case_doubles(size_t* size);

size_t unittest_get_memory_usage(void);

MockCall* unittest_get_mock_call(const char* function_name);
MockCall* unittest_get_mock_call_by_index(const char* function_name, int call_index);

int unittest_generate_random_int(int min, int max);
int* unittest_generate_edge_case_integers(size_t* size);

float unittest_generate_random_float(float min, float max); 
float* unittest_generate_edge_case_floats(size_t* size);

char** unittest_generate_edge_case_strings(size_t* size);
char* unittest_generate_random_string(size_t length);

extern void (*unittest_setup)(void);
extern void (*unittest_teardown)(void);

#define UNITTEST_MALLOC(size) unittest_malloc(size, __FILE__, __LINE__)
#define UNITTEST_FREE(ptr) unittest_free(ptr)

/* Configuration Constants */
#define UNITTEST_MESSAGE_LEN 1024
#define UNITTEST_EPSILON 1E-12
#define UNITTEST_EPSILON_FLOAT 1E-6

extern int unittest_run;
extern int unittest_assert;
extern int unittest_fail;
extern int unittest_status;
extern int unittest_skipped;
extern double unittest_timeout_ms;

// Timers
extern double unittest_real_timer;
extern double unittest_proc_timer;
extern int unittest_summary_only; 

extern char unittest_last_message[UNITTEST_MESSAGE_LEN];
extern int unittest_retry_count;

extern void* unittest_param;

#define UNITTEST_CASE(method_name) static void method_name(void)
#define UNITTEST_SUITE(suite_name) static void suite_name(void)
#define UNITTEST_RETRY_COUNT(count) do { unittest_retry_count = count; } while (0)
#define UNITTEST_SAFE_BLOCK(...) do { __VA_ARGS__ } while (0)
#define UNITTEST_EXIT_CODE (unittest_fail > 0 ? 1 : 0)
#define UNITTEST_COLOR_GREEN "\033[32m"
#define UNITTEST_COLOR_RED "\033[31m"
#define UNITTEST_COLOR_RESET "\033[0m"

#define UNITTEST_LOG_SUCCESS(...) \
    if (!unittest_summary_only) { \
        printf(UNITTEST_COLOR_GREEN __VA_ARGS__); \
        printf(UNITTEST_COLOR_RESET); \
    }

#define UNITTEST_LOG_FAILURE(...) \
    if (!unittest_summary_only) { \
        printf(UNITTEST_COLOR_RED __VA_ARGS__); \
        printf(UNITTEST_COLOR_RESET); \
    }


#define UNITTEST_LOG(...) if (!unittest_summary_only) { printf(__VA_ARGS__); }

/* Run Test Suite */
#define UNITTEST_RUN_SUITE(suite_name) UNITTEST_SAFE_BLOCK(\
    if (unittest_real_timer == 0) {\
        unittest_real_timer = unittest_timer_real();\
    }\
    if (unittest_proc_timer == 0) {\
        unittest_proc_timer = unittest_timer_cpu();\
    }\
    suite_name();\
    unittest_setup = NULL;\
    unittest_teardown = NULL;\
)

/* Configure Suite Setup and Teardown */
#define UNITTEST_SUITE_CONFIGURE(setup_func, teardown_func) UNITTEST_SAFE_BLOCK(\
    unittest_setup = setup_func;\
    unittest_teardown = teardown_func;\
)

/* Run a Test Case */
#define UNITTEST_RUN_TEST(test, total_tests) UNITTEST_SAFE_BLOCK(\
    int retries = unittest_retry_count; \
    while (retries >= 0) { \
        double start_time = unittest_timer_real(); \
        if (unittest_real_timer == 0) { \
            unittest_real_timer = start_time; \
        } \
        if (unittest_proc_timer == 0) { \
            unittest_proc_timer = unittest_timer_cpu(); \
        } \
        if (unittest_setup) (*unittest_setup)(); \
        unittest_status = 0; \
        test(); \
        double end_time = unittest_timer_real(); \
        unittest_run++; \
        double elapsed_time_ms = (end_time - start_time) * 1000.0; \
        printf("Elapsed time for %s: %.2f ms\n", #test, elapsed_time_ms); \
        if (unittest_timeout_ms > 0 && elapsed_time_ms > unittest_timeout_ms) { \
            snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\tTest timed out after %.2f ms", #test, elapsed_time_ms); \
            unittest_status = 1; \
        } \
        if (unittest_status == 0) { \
            break; /* Test passed, exit retry loop */ \
        } else { \
            printf("\033[31mTest failed: %s. Retrying...\033[0m\n", #test); \
            retries--; \
        } \
        if (unittest_teardown) (*unittest_teardown)(); \
    } \
    unittest_display_progress(unittest_run, total_tests); \
    if (retries < 0) { \
        unittest_fail++; \
        printf("\033[31mTest ultimately failed after retries: %s\033[0m\n", #test); \
    } else { \
        printf("\033[32mTest passed: %s\033[0m\n", #test); \
    } \
)


/* Report Results */
#define UNITTEST_REPORT() UNITTEST_SAFE_BLOCK(\
    double unittest_end_real_timer = unittest_timer_real(); \
    double unittest_end_proc_timer = unittest_timer_cpu(); \
    printf("\n\n\033[32m%d tests\033[0m, \033[32m%d assertions\033[0m, \033[31m%d failures\033[0m, \033[33m%d skipped\033[0m\n", \
        unittest_run, unittest_assert, unittest_fail, unittest_skipped); \
    if (unittest_skipped > 0) { \
        printf("\033[33m\nSkipped Tests:\n---------------\n%s\n\033[0m", unittest_last_message); \
    } \
    if (unittest_real_timer > 0) { \
        printf("\nFinished in \033[32m%.8f seconds (real)\033[0m \033[32m%.8f seconds (proc)\033[0m\n\n", \
            unittest_end_real_timer - unittest_real_timer, \
            unittest_end_proc_timer - unittest_proc_timer); \
    } else { \
        printf("\n\033[31mTiming information not available.\033[0m\n\n"); \
    } \
    if (unittest_fail > 0) { \
        exit(1); /* Non-zero exit for failures */ \
    } \
)


#define unittest_check(test) UNITTEST_SAFE_BLOCK(\
    unittest_assert++;\
    if (!(test)) {\
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: %s", __func__, __FILE__, __LINE__, #test);\
        unittest_status = 1;\
        UNITTEST_LOG("\033[31m%s\n\033[0m", unittest_last_message);\
        return;\
    } else {\
        UNITTEST_LOG(".");\
    }\
)


#define unittest_fail(message) UNITTEST_SAFE_BLOCK(\
    unittest_assert++;\
    snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: %s", __func__, __FILE__, __LINE__, message);\
    unittest_status = 1;\
    return;\
)

#define unittest_assert(test, message_format, ...) UNITTEST_SAFE_BLOCK(\
    unittest_assert++;\
    if (!(test)) {\
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: " message_format,\
                 __func__, __FILE__, __LINE__, ##__VA_ARGS__);\
        unittest_status = 1;\
        fprintf(stderr, "%s\n", unittest_last_message); /* Log failure message immediately */\
        return;\
    } else {\
        printf(".");\
    }\
)


#define unittest_assert_int_eq(expected, actual, message_format, ...) UNITTEST_SAFE_BLOCK(\
    int unittest_tmp_e = (expected); \
    int unittest_tmp_r = (actual); \
    unittest_assert++; \
    if (unittest_tmp_e != unittest_tmp_r) { \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: " message_format, \
            __func__, __FILE__, __LINE__, __VA_ARGS__); \
        fprintf(stderr, "%s\n", unittest_last_message); /* Log failure message immediately */ \
        unittest_status = 1; \
        return; \
    } else { \
        printf("."); \
    } \
)

#define unittest_assert_double_eq(expected, actual, message_format, ...) UNITTEST_SAFE_BLOCK(\
    double unittest_tmp_e = (expected); \
    double unittest_tmp_r = (actual); \
    unittest_assert++; \
    if (fabs(unittest_tmp_e - unittest_tmp_r) > UNITTEST_EPSILON) { \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: " message_format, \
            __func__, __FILE__, __LINE__, __VA_ARGS__); \
        fprintf(stderr, "%s\n", unittest_last_message); /* Log failure message immediately */ \
        unittest_status = 1; \
        return; \
    } else { \
        printf("."); \
    } \
)

#define unittest_assert_string_eq(expected, actual, message_format, ...) UNITTEST_SAFE_BLOCK(\
    const char* unittest_tmp_e = (expected); \
    const char* unittest_tmp_r = (actual); \
    unittest_assert++; \
    if (!unittest_tmp_e) unittest_tmp_e = "<null>"; \
    if (!unittest_tmp_r) unittest_tmp_r = "<null>"; \
    if (strcmp(unittest_tmp_e, unittest_tmp_r) != 0) { \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: " message_format, \
                 __func__, __FILE__, __LINE__, __VA_ARGS__); /* Properly use format and args */ \
        fprintf(stderr, "%s\n", unittest_last_message); /* Log failure message immediately */ \
        unittest_status = 1; \
        return; \
    } else { \
        printf("."); \
    } \
)

#define unittest_assert_not_null(pointer, message_format, ...) UNITTEST_SAFE_BLOCK( \
    unittest_assert++; \
    if ((pointer) == NULL) { \
        if (*#message_format != '\0') { /* Check if a custom message is provided */ \
            snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: " message_format, \
                     __func__, __FILE__, __LINE__, ##__VA_ARGS__); \
        } else { \
            snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: Pointer is NULL", \
                     __func__, __FILE__, __LINE__); \
        } \
        fprintf(stderr, "%s\n", unittest_last_message); /* Log failure message immediately */ \
        unittest_status = 1; \
        return; \
    } else { \
        printf("."); \
    } \
)

#define unittest_assert_in_range(value, min, max, message_format, ...) UNITTEST_SAFE_BLOCK( \
    double unittest_tmp_val = (value); \
    double unittest_tmp_min = (min); \
    double unittest_tmp_max = (max); \
    unittest_assert++; \
    if (unittest_tmp_val < unittest_tmp_min || unittest_tmp_val > unittest_tmp_max) { \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: " message_format, \
                 __func__, __FILE__, __LINE__, ##__VA_ARGS__); \
        fprintf(stderr, "%s\n", unittest_last_message); /* Log failure message immediately */ \
        unittest_status = 1; \
        return; \
    } else { \
        printf("."); \
    } \
)

#define unittest_assert_array_int_eq(expected, actual, size, message_format, ...) UNITTEST_SAFE_BLOCK( \
    int unittest_size = (size); \
    int unittest_arrays_equal = 1; \
    for (int i = 0; i < unittest_size; i++) { \
        if ((expected)[i] != (actual)[i]) { \
            unittest_arrays_equal = 0; \
            if (strlen(message_format) > 0) { /* Check if a custom message is provided */ \
                snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, \
                    "%s failed:\n\t%s:%d: " message_format " at index %d (expected: %d, actual: %d)", \
                    __func__, __FILE__, __LINE__, ##__VA_ARGS__, i, (expected)[i], (actual)[i]); \
            } else { \
                snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, \
                    "%s failed:\n\t%s:%d: Arrays differ at index %d (expected: %d, actual: %d)", \
                    __func__, __FILE__, __LINE__, i, (expected)[i], (actual)[i]); \
            } \
            break; \
        } \
    } \
    unittest_assert++; \
    if (!unittest_arrays_equal) { \
        fprintf(stderr, "%s\n", unittest_last_message); /* Log failure message immediately */ \
        unittest_status = 1; \
        return; \
    } else { \
        printf("."); \
    } \
)

#define unittest_assert_array_float_eq(expected, actual, size, epsilon, message_format, ...) UNITTEST_SAFE_BLOCK(\
    unittest_assert++; \
    int unittest_arrays_equal = 1; \
    for (size_t i = 0; i < size; i++) { \
        if (fabs((expected)[i] - (actual)[i]) > (epsilon)) { \
            unittest_arrays_equal = 0; \
            snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, \
                "%s failed:\n\t%s:%d: " message_format " (index %zu: expected %.6f, got %.6f)", \
                __func__, __FILE__, __LINE__, ##__VA_ARGS__, i, (expected)[i], (actual)[i]); \
            fprintf(stderr, "%s\n", unittest_last_message); \
            break; \
        } \
    } \
    if (!unittest_arrays_equal) { \
        unittest_status = 1; \
        return; \
    } else { \
        printf("."); \
    } \
)

#define unittest_assert_array_double_eq(expected, actual, size, epsilon, message_format, ...) UNITTEST_SAFE_BLOCK(\
    unittest_assert++; \
    int unittest_arrays_equal = 1; \
    for (size_t i = 0; i < size; i++) { \
        if (fabs((expected)[i] - (actual)[i]) > (epsilon)) { \
            unittest_arrays_equal = 0; \
            snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, \
                "%s failed:\n\t%s:%d: " message_format " (index %zu: expected %.12f, got %.12f)", \
                __func__, __FILE__, __LINE__, ##__VA_ARGS__, i, (expected)[i], (actual)[i]); \
            fprintf(stderr, "%s\n", unittest_last_message); \
            break; \
        } \
    } \
    if (!unittest_arrays_equal) { \
        unittest_status = 1; \
        return; \
    } else { \
        printf("."); \
    } \
)

#define unittest_assert_array_struct_eq(expected, actual, size, comparator, message_format, ...) UNITTEST_SAFE_BLOCK(\
    unittest_assert++; \
    int unittest_arrays_equal = 1; \
    for (size_t i = 0; i < size; i++) { \
        if (!(comparator(&(expected)[i], &(actual)[i]))) { \
            unittest_arrays_equal = 0; \
            snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, \
                "%s failed:\n\t%s:%d: " message_format " (index %zu)", \
                __func__, __FILE__, __LINE__, ##__VA_ARGS__, i); \
            fprintf(stderr, "%s\n", unittest_last_message); \
            break; \
        } \
    } \
    if (!unittest_arrays_equal) { \
        unittest_status = 1; \
        return; \
    } else { \
        printf("."); \
    } \
)


#define UNITTEST_PARAM_TEST(test, param) UNITTEST_SAFE_BLOCK(\
    if (unittest_setup) (*unittest_setup)();\
    unittest_status = 0;\
    unittest_param = (void*)param;\
    test();\
    unittest_run++;\
    if (unittest_status) {\
        unittest_fail++;\
        printf("Failed Tests :\n----------\n%s\n", unittest_last_message);\
    }\
    if (unittest_teardown) (*unittest_teardown)();\
)

#define UNITTEST_RUN_PARAM_TEST(test, params, count) UNITTEST_SAFE_BLOCK(\
    for (size_t i = 0; i < count; ++i) {\
        UNITTEST_PARAM_TEST(test, &params[i]);\
    }\
)

#define UNITTEST_SKIP(message) UNITTEST_SAFE_BLOCK(\
    snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s skipped:\n\tFile: %s\n\tLine: %d\n\tReason: %s", __func__, __FILE__, __LINE__, message);\
    unittest_skipped++;\
    return;\
)


#define UNITTEST_CHECK_NO_LEAKS(code_block) UNITTEST_SAFE_BLOCK(\
    size_t memory_before = total_memory_allocated; \
    code_block; \
    size_t memory_after = total_memory_allocated; \
    if (memory_before != memory_after) { \
        print_memory_leaks(); \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, \
            "Memory leak detected: %zu bytes not freed", memory_after - memory_before); \
        unittest_status = 1; \
        unittest_fail++; \
    } else { \
        printf("."); \
    } \
)

// Assert that a string matches a regex pattern
#define unittest_assert_regex_match(pattern, string, flags, message_format, ...) UNITTEST_SAFE_BLOCK(\
    Regex* unittest_regex = regex_compile(pattern, flags); \
    if (!unittest_regex) { \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: Failed to compile regex: %s", \
                 __func__, __FILE__, __LINE__, pattern); \
        fprintf(stderr, "%s\n", unittest_last_message); \
        unittest_status = 1; \
        return; \
    } \
    RegexMatch unittest_match; \
    RegexResult unittest_result = regex_search(unittest_regex, string, &unittest_match); \
    if (unittest_result != REGEX_SUCCESS) { \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: " message_format, \
                 __func__, __FILE__, __LINE__, ##__VA_ARGS__); \
        fprintf(stderr, "%s\n", unittest_last_message); \
        regex_deallocate(unittest_regex); \
        unittest_status = 1; \
        return; \
    } else { \
        printf("."); \
    } \
    regex_deallocate(unittest_regex); \
)

// Assert a captured regex group matches the expected string
#define unittest_assert_regex_group(pattern, input, group_index, expected, flags, message_format, ...) UNITTEST_SAFE_BLOCK( \
    Regex* regex = regex_compile((pattern), (flags)); \
    if (!regex) { \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: Regex compilation failed for pattern '%s'", \
                 __func__, __FILE__, __LINE__, (pattern)); \
        fprintf(stderr, "%s\n", unittest_last_message); \
        unittest_fail++; \
        return; \
    } \
    RegexMatch match; \
    RegexResult result = regex_search(regex, (input), &match); \
    if (result != REGEX_SUCCESS) { \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: Regex search failed for input '%s'", \
                 __func__, __FILE__, __LINE__, (input)); \
        fprintf(stderr, "%s\n", unittest_last_message); \
        unittest_fail++; \
        regex_deallocate(regex); \
        return; \
    } \
    if ((group_index) < 0 || (group_index) >= match.group_count) { \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: Group index %d out of bounds", \
                 __func__, __FILE__, __LINE__, (group_index)); \
        fprintf(stderr, "%s\n", unittest_last_message); \
        unittest_fail++; \
        regex_deallocate(regex); \
        return; \
    } \
    const char* group_start = match.group_starts[(group_index)]; \
    size_t group_length = match.group_lengths[(group_index)]; \
    if (strncmp(group_start, (expected), group_length) != 0) { \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: " message_format, \
                 __func__, __FILE__, __LINE__, ##__VA_ARGS__); \
        fprintf(stderr, "%s\n", unittest_last_message); \
        unittest_fail++; \
    } else { \
        printf("."); \
    } \
    regex_deallocate(regex); \
)

#define unittest_assert_file_eq(file1, file2, message_format, ...) UNITTEST_SAFE_BLOCK( \
    unittest_assert++; \
    FILE *unittest_f1 = fopen((file1), "rb"); \
    FILE *unittest_f2 = fopen((file2), "rb"); \
    int unittest_files_equal = 1; \
    if (!unittest_f1 || !unittest_f2) { \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, \
            "%s failed:\n\t%s:%d: Unable to open file(s): '%s', '%s'", \
            __func__, __FILE__, __LINE__, (file1), (file2)); \
        fprintf(stderr, "%s\n", unittest_last_message); \
        unittest_status = 1; \
        if (unittest_f1) fclose(unittest_f1); \
        if (unittest_f2) fclose(unittest_f2); \
        return; \
    } \
    char unittest_buf1[1024], unittest_buf2[1024]; \
    size_t unittest_read1, unittest_read2; \
    int unittest_line_number = 1; \
    while ((unittest_read1 = fread(unittest_buf1, 1, sizeof(unittest_buf1), unittest_f1)) > 0 && \
           (unittest_read2 = fread(unittest_buf2, 1, sizeof(unittest_buf2), unittest_f2)) > 0) { \
        if (unittest_read1 != unittest_read2 || memcmp(unittest_buf1, unittest_buf2, unittest_read1) != 0) { \
            unittest_files_equal = 0; \
            snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, \
                "%s failed:\n\t%s:%d: " message_format " (file content differs at line %d)", \
                __func__, __FILE__, __LINE__, ##__VA_ARGS__, unittest_line_number); \
            fprintf(stderr, "%s\n", unittest_last_message); \
            break; \
        } \
        unittest_line_number++; \
    } \
    if (!feof(unittest_f1) || !feof(unittest_f2)) { \
        unittest_files_equal = 0; \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, \
            "%s failed:\n\t%s:%d: " message_format " (file lengths differ)", \
            __func__, __FILE__, __LINE__, ##__VA_ARGS__); \
        fprintf(stderr, "%s\n", unittest_last_message); \
    } \
    fclose(unittest_f1); \
    fclose(unittest_f2); \
    if (!unittest_files_equal) { \
        unittest_status = 1; \
        return; \
    } else { \
        printf("."); \
    } \
)

#define unittest_assert_file_exists(filepath, message_format, ...) UNITTEST_SAFE_BLOCK({ \
    unittest_assert++; \
    if (access((filepath), F_OK) != 0) { \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: " message_format, \
                 __func__, __FILE__, __LINE__, ##__VA_ARGS__); \
        fprintf(stderr, "%s\n", unittest_last_message); \
        unittest_status = 1; \
        return; \
    } else { \
        printf("."); \
    } \
})

#define unittest_assert_file_not_exists(filepath, message_format, ...) UNITTEST_SAFE_BLOCK({ \
    unittest_assert++; \
    if (access((filepath), F_OK) == 0) { \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: " message_format, \
                 __func__, __FILE__, __LINE__, ##__VA_ARGS__); \
        fprintf(stderr, "%s\n", unittest_last_message); \
        unittest_status = 1; \
        return; \
    } else { \
        printf("."); \
    } \
})

// Macros for Mock Functions
#define UNITTEST_MOCK_FUNCTION(func_name, ...) \
    void func_name(__VA_ARGS__);

#define UNITTEST_MOCK_FUNCTION_WITH_STUB(func_name, return_type, ...) \
    return_type func_name(__VA_ARGS__);

#define UNITTEST_VERIFY_CALL_COUNT(func, count) \
    unittest_verify_mock_call_count(#func, count)

#define UNITTEST_VERIFY_ARGUMENTS(func, ...) \
    do { \
        void* args[] = {__VA_ARGS__}; \
        unittest_verify_mock_call(#func, sizeof(args) / sizeof(args[0]), args); \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif 