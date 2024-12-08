#ifndef UNITTEST_H_
#define UNITTEST_H_


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// track memory allocations
typedef struct MemoryRecord {
    void* address;
    size_t size;
    const char* file;
    int line;
    struct MemoryRecord* next;
} MemoryRecord;

extern size_t total_memory_allocated;

void add_memory_record(void* address, size_t size, const char* file, int line);
void remove_memory_record(void* address);
void print_memory_leaks();
void* unittest_malloc(size_t size, const char* file, int line);
void unittest_free(void* ptr);
double unittest_timer_real(void);
double unittest_timer_cpu(void);
void unittest_set_timeout(double timeout_ms);
size_t unittest_get_memory_usage(void);

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

/* Timers */
extern double unittest_real_timer;
extern double unittest_proc_timer;

extern char unittest_last_message[UNITTEST_MESSAGE_LEN];
extern int unittest_retry_count; //Retry count for tests

extern void* unittest_param;

#define UNITTEST_CASE(method_name) static void method_name(void)
#define UNITTEST_SUITE(suite_name) static void suite_name(void)
#define UNITTEST_RETRY_COUNT(count) do { unittest_retry_count = count; } while (0)
#define UNITTEST_SAFE_BLOCK(block) do { block } while (0)
#define UNITTEST_EXIT_CODE (unittest_fail > 0 ? 1 : 0)

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
#define UNITTEST_RUN_TEST(test) UNITTEST_SAFE_BLOCK(\
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
            printf("Test failed: %s. Retrying...\n", #test); \
            retries--; \
        } \
        if (unittest_teardown) (*unittest_teardown)(); \
    } \
    if (retries < 0) { \
        unittest_fail++; \
        printf("Test ultimately failed after retries: %s\n", #test); \
    } \
)

/* Report Results */
#define UNITTEST_REPORT() UNITTEST_SAFE_BLOCK(\
    double unittest_end_real_timer = unittest_timer_real(); \
    double unittest_end_proc_timer = unittest_timer_cpu(); \
    printf("\n\n%d tests, %d assertions, %d failures, %d skipped\n", unittest_run, unittest_assert, unittest_fail, unittest_skipped); \
    if (unittest_skipped > 0) { \
        printf("\nSkipped Tests:\n---------------\n%s\n", unittest_last_message); \
    } \
    if (unittest_real_timer > 0) { \
        printf("\nFinished in %.8f seconds (real) %.8f seconds (proc)\n\n", \
            unittest_end_real_timer - unittest_real_timer, \
            unittest_end_proc_timer - unittest_proc_timer); \
    } else { \
        printf("\nTiming information not available.\n\n"); \
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
        return;\
    } else {\
        printf(".");\
    }\
)

#define unittest_fail(message) UNITTEST_SAFE_BLOCK(\
    unittest_assert++;\
    snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: %s", __func__, __FILE__, __LINE__, message);\
    unittest_status = 1;\
    return;\
)

#define unittest_assert(test, message) UNITTEST_SAFE_BLOCK(\
    unittest_assert++;\
    if (!(test)) {\
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: %s", __func__, __FILE__, __LINE__, message);\
        unittest_status = 1;\
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


#ifdef __cplusplus
}
#endif

#endif 