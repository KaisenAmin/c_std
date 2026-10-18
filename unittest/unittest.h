/**
 * @file    unittest.h
 * @author  Amin Tahmasebi
 * @date    2024
 * @class   Unittest
 *
 * @brief   A self-contained, cross-platform unit-testing framework in C.
 *
 *   - **Test runner**: the `UNITTEST_RUN_SUITE` / `UNITTEST_RUN_TEST` /
 *     `UNITTEST_REPORT` macros and the global counters they update.
 *   - **Assertions**: a family of `unittest_check` / `unittest_assert*`
 *     macros that record a pass on `.` and a fail with a printf-style
 *     message plus a `file:line` annotation. Every assertion macro
 *     increments `unittest_assert` so the final report knows the total
 *     number of checks performed.
 *   - **Mocking & stubbing**: a global mock-call registry that records
 *     every `unittest_register_mock_call`, a global stub registry that
 *     maps `(function_name, arg)` pairs to return values, plus
 *     verification helpers.
 *   - **Memory tracking**: `UNITTEST_MALLOC` / `UNITTEST_FREE`,
 *     `print_memory_leaks`, and the `UNITTEST_CHECK_NO_LEAKS` macro for
 *     verifying that a code block doesn't leak.
 *   - **Generators**: random / edge-case generators for ints, floats,
 *     doubles, strings, and raw bytes — for property tests and fuzzing.
 *   - **Cross-platform timing**: `unittest_timer_real` and
 *     `unittest_timer_cpu` work on Windows, macOS, and POSIX.
 *
 * Conventions:
 *   - Every assertion macro pretty-prints its failure on stderr and
 *     sets `unittest_status` to 1 so the surrounding `UNITTEST_RUN_TEST`
 *     can decide whether to retry or fail.
 *   - All "registry" globals are static-storage so the library is
 *     usable as a single-process test runner; it is **not** thread-safe.
 *
 * @note    To suppress per-check progress output (`.` per pass) set
 *          `unittest_summary_only = 1;` before running the suite.
 */


#ifndef UNITTEST_H_
#define UNITTEST_H_

/* Platform headers (<windows.h>, mach, <unistd.h>) are only needed by
   unittest.c: including <windows.h> here leaked its min/max/ERROR macros
   (and a `#define access _access`) into every test file. The regex
   declarations below are plain C; the regex module is only linked when
   a regex assertion is actually used. */
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

/**
 * @def MAX_STUBS
 * @brief Maximum number of stub entries the registry can hold.
 *
 * `unittest_register_stub` rejects (with a warning) any registration
 * beyond this limit so a runaway registration loop can't corrupt
 * adjacent memory. Sized at 100 to comfortably exceed typical
 * per-suite usage.
 */
#define MAX_STUBS 100

/**
 * @def MAX_ARGS
 * @brief Maximum number of arguments captured per mock call.
 *
 * Mock-call entries store a fixed-size array of `MAX_ARGS` pointers
 * to strdup'd argument strings. Calls with more arguments will lose
 * the extras silently.
 */
#define MAX_ARGS 10

/**
 * @struct MemoryRecord
 * @brief  Linked-list node tracking a live `UNITTEST_MALLOC` allocation.
 *
 * Every allocation made through `unittest_malloc` is recorded in a
 * doubly-purposed singly-linked list keyed by `address`. The matching
 * call to `unittest_free` looks up and removes the node, decrementing
 * `total_memory_allocated` accordingly.
 *
 * `file` and `line` are filled by the `UNITTEST_MALLOC` macro so
 * `print_memory_leaks` can report the original call site for each
 * unfreed allocation at the end of a run.
 */
typedef struct MemoryRecord {
    void* address;            /**< Allocation address (key). */
    size_t size;              /**< Byte size of the allocation. */
    const char* file;         /**< Source file where the allocation was made. */
    int line;                 /**< Source line where the allocation was made. */
    struct MemoryRecord* next;/**< Next node in the tracking list. */
} MemoryRecord;

/**
 * @struct MockCall
 * @brief  Captured record of a single mock-function invocation.
 *
 * Pushed onto the mock-call registry by `unittest_register_mock_call`.
 * Each entry owns the strdup'd `function_name` and every `actual_args[i]`
 * — they're released by `unittest_clear_mock_registry`.
 *
 * The `index` field is the zero-based occurrence number of this call
 * for the same function name, so `unittest_get_mock_call_by_index`
 * can address "the third call to `greet`" deterministically even when
 * many mock functions interleave.
 */
typedef struct MockCall {
    char* function_name;          /**< Mock function name (owned). */
    int call_count;               /**< Total calls to this function so far. */
    size_t arg_count;             /**< Number of args captured for this call. */
    char* actual_args[MAX_ARGS];  /**< Captured args (owned). */
    int index;                    /**< Zero-based per-function occurrence number. */
    struct MockCall* next;        /**< Next node in the registry list. */
} MockCall;

/**
 * @struct MockRegistry
 * @brief  Head of the global mock-call list.
 *
 * The framework keeps exactly one of these as a static global. New
 * calls are prepended at `head`, so iteration order is reverse
 * chronological — but consumers should rely on `index` for
 * deterministic ordering rather than list position.
 */
typedef struct MockRegistry {
    MockCall* head;
} MockRegistry;

/**
 * @struct Stub
 * @brief  One (function, arg) → return-value mapping in the stub registry.
 *
 * The framework stores up to `MAX_STUBS` of these in a static array.
 * Pointer fields are NOT owned by the registry — callers must keep
 * their lifetime alive (typically string literals).
 */
typedef struct {
    const char* function_name;
    const char* arg;
    void* return_value;
} Stub;

/* Link-time names. These functions keep their documented names in C code,
 * but the library exports them with a "unittest_" prefix: generic global symbols
 * such as add_memory_record clashed at link time with any other library or program
 * defining the same name ("multiple definition of ..."). */
#define total_memory_allocated unittest_total_memory_allocated
#define add_memory_record      unittest_add_memory_record
#define print_memory_leaks     unittest_print_memory_leaks

/**
 * @var   total_memory_allocated
 * @brief Running total of bytes tracked by `unittest_malloc` /
 *        `unittest_free`. Snapshot by `UNITTEST_CHECK_NO_LEAKS`.
 */
extern size_t total_memory_allocated;


/* ------------------------------------------------------------------ */
/* Memory tracking                                                    */
/* ------------------------------------------------------------------ */

void*       unittest_malloc                       (size_t size, const char* file, int line);
void        unittest_free                         (void* ptr);
void        add_memory_record                     (void* address, size_t size, const char* file, int line);
void        remove_memory_record                  (void* address);
void        print_memory_leaks                    (void);
size_t      unittest_get_memory_usage             (void);


/* ------------------------------------------------------------------ */
/* Mock-call registry                                                 */
/* ------------------------------------------------------------------ */

void        unittest_register_mock_call           (const char* function_name, size_t arg_count, void** args);
void        unittest_verify_mock_call             (const char* function_name, size_t arg_count, void** expected_args, int call_index);
void        unittest_verify_mock_call_count       (const char* function_name, int expected_count);
void        unittest_clear_mock_registry          (void);
MockCall*   unittest_get_mock_call                (const char* function_name);
MockCall*   unittest_get_mock_call_by_index       (const char* function_name, int call_index);
const char* unittest_mock_call_arg                (const char* function_name, int call_index, size_t arg_index);


/* ------------------------------------------------------------------ */
/* Stub registry                                                      */
/* ------------------------------------------------------------------ */

void        unittest_register_stub                (const char* function_name, const char* query, void* return_value);
void*       unittest_get_stub_return_value        (const char* function_name, const char* query);
void        unittest_clear_stub_registry          (void);


/* ------------------------------------------------------------------ */
/* Generators (random + edge cases)                                   */
/* ------------------------------------------------------------------ */

int         unittest_generate_random_int          (int min, int max);
float       unittest_generate_random_float        (float min, float max);
double      unittest_generate_random_double       (double min, double max);
char*       unittest_generate_random_string       (size_t length);
void        unittest_generate_random_bytes        (unsigned char* buffer, size_t size);

int*        unittest_generate_edge_case_integers  (size_t* size);
float*      unittest_generate_edge_case_floats    (size_t* size);
double*     unittest_generate_edge_case_doubles   (size_t* size);
char**      unittest_generate_edge_case_strings   (size_t* size);
void        unittest_free_generated_data          (void* data);


/* ------------------------------------------------------------------ */
/* Timing                                                             */
/* ------------------------------------------------------------------ */

double      unittest_timer_real                   (void);
double      unittest_timer_cpu                    (void);
void        unittest_set_timeout                  (double timeout_ms);


/* ------------------------------------------------------------------ */
/* Runner state / progress                                            */
/* ------------------------------------------------------------------ */

void        unittest_reset_state                  (void);
void        unittest_display_progress             (int completed, int total);
void        unittest_record_skip                  (const char* func, const char* file, int line, const char* reason);

/* Failure-message builders used by the assertion macros: set writes
 * "<func> failed:\n\t<file>:<line>: " and the formatted message (or
 * `fallback` when the message is empty) into unittest_last_message; append
 * adds more formatted text. The message is the first variadic argument of
 * every assertion macro, so the macros are plain C17 (no GNU ", ##__VA_ARGS__"
 * comma elision) and GCC / Clang still check the format against its arguments.
 * The macros glue one marker space in front of the message literal (" " fmt),
 * which unittest_set_failure skips: an empty message is then not a zero-length
 * format (-Wformat-zero-length). As before, the message must be a string literal. */
#if defined(__MINGW_PRINTF_FORMAT)   /* MinGW: the archetype its own stdio uses (knows %zu) */
    #define UNITTEST_PRINTF_FORMAT(fmt_index, first_arg) __attribute__((format(__MINGW_PRINTF_FORMAT, fmt_index, first_arg)))
#elif defined(__GNUC__) || defined(__clang__)
    #define UNITTEST_PRINTF_FORMAT(fmt_index, first_arg) __attribute__((format(printf, fmt_index, first_arg)))
#else
    #define UNITTEST_PRINTF_FORMAT(fmt_index, first_arg)
#endif
void        unittest_set_failure                  (const char* func, const char* file, int line, const char* fallback,
                                                   const char* fmt, ...) UNITTEST_PRINTF_FORMAT(5, 6);
void        unittest_append_failure               (const char* fmt, ...) UNITTEST_PRINTF_FORMAT(1, 2);
bool        unittest_file_exists                  (const char* path);

/**
 * @var   unittest_setup
 * @brief Optional per-test setup hook, set by `UNITTEST_SUITE_CONFIGURE`.
 *        Invoked before every test case if non-NULL.
 */
extern void (*unittest_setup)(void);

/**
 * @var   unittest_teardown
 * @brief Optional per-test teardown hook, set by `UNITTEST_SUITE_CONFIGURE`.
 *        Invoked after every test case if non-NULL (even on failure).
 */
extern void (*unittest_teardown)(void);

/**
 * @def UNITTEST_MALLOC
 * @brief Tracked-malloc wrapper that records the call site for
 *        leak reporting. Use exactly like libc `malloc`.
 */
#define UNITTEST_MALLOC(size) unittest_malloc(size, __FILE__, __LINE__)

/**
 * @def UNITTEST_FREE
 * @brief Tracked-free wrapper. Use exactly like libc `free`.
 */
#define UNITTEST_FREE(ptr) unittest_free(ptr)

/**
 * @def   UNITTEST_MESSAGE_LEN
 * @brief Size of the static `unittest_last_message` buffer used to
 *        format assertion failure messages.
 */
#define UNITTEST_MESSAGE_LEN 1024

/**
 * @def   UNITTEST_EPSILON
 * @brief Default tolerance for `unittest_assert_double_eq` (1e-12).
 */
#define UNITTEST_EPSILON 1E-12

/**
 * @def   UNITTEST_EPSILON_FLOAT
 * @brief Default tolerance for float comparisons (1e-6).
 */
#define UNITTEST_EPSILON_FLOAT 1E-6

/**
 * @var unittest_run
 * @brief Number of test cases that have been executed by the runner.
 */
extern int unittest_run;

/**
 * @var unittest_assert
 * @brief Number of assertions (`unittest_check` / `unittest_assert*`)
 *        evaluated so far. Used in the final report.
 */
extern int unittest_assert;

/**
 * @var unittest_fail
 * @brief Number of test cases that ultimately failed.
 */
extern int unittest_fail;

/**
 * @var unittest_status
 * @brief Per-test status flag set by failing assertions. 0 = pass,
 *        1 = the current test has at least one failed assertion.
 */
extern int unittest_status;

/**
 * @var unittest_skipped
 * @brief Number of test cases that called `UNITTEST_SKIP`.
 */
extern int unittest_skipped;

/**
 * @var unittest_timeout_ms
 * @brief Per-test timeout in milliseconds (0 = disabled). Configured
 *        via `unittest_set_timeout`.
 */
extern double unittest_timeout_ms;

/**
 * @var unittest_real_timer
 * @brief Captured wall-clock baseline (seconds) at the start of the run.
 */
extern double unittest_real_timer;

/**
 * @var unittest_proc_timer
 * @brief Captured CPU-time baseline (seconds) at the start of the run.
 */
extern double unittest_proc_timer;

/**
 * @var unittest_summary_only
 * @brief When non-zero, suppress per-check `.` output so only the
 *        final summary line is printed. Useful for CI logs.
 */
extern int unittest_summary_only;

/**
 * @var unittest_last_message
 * @brief Most recently formatted failure (or skip) message. Cleared
 *        by `unittest_reset_state`.
 */
extern char unittest_last_message[UNITTEST_MESSAGE_LEN];

/**
 * @def   UNITTEST_SKIP_LOG_LEN
 * @brief Size of `unittest_skip_log`, the list of every skip message
 *        printed by `UNITTEST_REPORT` (longer lists are truncated).
 */
#define UNITTEST_SKIP_LOG_LEN 4096

/**
 * @var unittest_skip_log
 * @brief One line per `UNITTEST_SKIP` since the last reset
 *        ("test: reason (file:line)"). Cleared by `unittest_reset_state`.
 */
extern char unittest_skip_log[UNITTEST_SKIP_LOG_LEN];

/**
 * @var unittest_retry_count
 * @brief Number of times to retry a failing test before declaring it
 *        a permanent failure. Set per-test via `UNITTEST_RETRY_COUNT`.
 */
extern int unittest_retry_count;

/**
 * @var unittest_param
 * @brief Opaque per-test parameter slot used by `UNITTEST_PARAM_TEST`
 *        / `UNITTEST_RUN_PARAM_TEST`. The current parameter is
 *        accessible to the test body through this pointer.
 */
extern void* unittest_param;



/**
 * @def   UNITTEST_CASE
 * @brief Declare a test case as a static void function.
 *
 * @code
 *     UNITTEST_CASE(t_add_basic) {
 *         unittest_check(1 + 1 == 2);
 *     }
 * @endcode
 */
#define UNITTEST_CASE(method_name) static void method_name(void)

/**
 * @def   UNITTEST_SUITE
 * @brief Declare a test suite as a static void function. The body
 *        typically calls `UNITTEST_RUN_TEST` for every test case.
 */
#define UNITTEST_SUITE(suite_name) static void suite_name(void)

/**
 * @def   UNITTEST_RETRY_COUNT
 * @brief Set how many times a failing test should be re-run before it
 *        is declared a permanent failure.
 */
#define UNITTEST_RETRY_COUNT(count) do { unittest_retry_count = count; } while (0)

/**
 * @def   UNITTEST_SAFE_BLOCK
 * @brief Wrap a multi-statement macro body in `do { ... } while (0)`
 *        so it behaves as a single statement at the call site.
 */
#define UNITTEST_SAFE_BLOCK(...) do { __VA_ARGS__ } while (0)

/**
 * @def   UNITTEST_EXIT_CODE
 * @brief Suggested process exit code: 1 if any test failed, 0 otherwise.
 */
#define UNITTEST_EXIT_CODE (unittest_fail > 0 ? 1 : 0)

/**
 * @def UNITTEST_COLOR_GREEN
 * @brief ANSI escape for green text.
 */
#define UNITTEST_COLOR_GREEN "\033[32m"
/**
 * @def UNITTEST_COLOR_RED
 * @brief ANSI escape for red text.
 */
#define UNITTEST_COLOR_RED   "\033[31m"
/**
 * @def UNITTEST_COLOR_RESET
 * @brief ANSI escape that resets text colour.
 */
#define UNITTEST_COLOR_RESET "\033[0m"

/**
 * @def   UNITTEST_LOG_SUCCESS
 * @brief Conditional `printf` that emits its argument list wrapped in
 *        green, but ONLY when `unittest_summary_only` is zero.
 */
#define UNITTEST_LOG_SUCCESS(...) \
    if (!unittest_summary_only) { \
        printf(UNITTEST_COLOR_GREEN __VA_ARGS__); \
        printf(UNITTEST_COLOR_RESET); \
    }

    
/**
 * @def   UNITTEST_LOG_FAILURE
 * @brief Conditional `printf` that emits its argument list wrapped in
 *        red, but ONLY when `unittest_summary_only` is zero.
 */
#define UNITTEST_LOG_FAILURE(...) \
    if (!unittest_summary_only) { \
        printf(UNITTEST_COLOR_RED __VA_ARGS__); \
        printf(UNITTEST_COLOR_RESET); \
    }


/**
 * @def   UNITTEST_LOG
 * @brief Conditional `printf` (no colour). Suppressed when
 *        `unittest_summary_only` is non-zero.
 */
#define UNITTEST_LOG(...) if (!unittest_summary_only) { printf(__VA_ARGS__); }


/**
 * @def   UNITTEST_RUN_SUITE
 * @brief Run a test suite, lazily capturing the real/CPU timer baselines
 *        on the first call. Clears the setup/teardown hooks afterwards
 *        so the next suite starts from a clean slate.
 *
 * @param suite_name  A `UNITTEST_SUITE`-declared function.
 */
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

/**
 * @def   UNITTEST_SUITE_CONFIGURE
 * @brief Install per-test setup and teardown hooks for the surrounding
 *        suite. Either argument may be NULL to skip that phase.
 */
#define UNITTEST_SUITE_CONFIGURE(setup_func, teardown_func) UNITTEST_SAFE_BLOCK(\
    unittest_setup = setup_func;\
    unittest_teardown = teardown_func;\
)

/**
 * @def   UNITTEST_RUN_TEST
 * @brief Run a single test case with retry support, per-test timing,
 *        optional timeout, and setup/teardown.
 *
 * Flow:
 *   1. Capture start time.
 *   2. If a setup hook is installed, call it.
 *   3. Execute the test.
 *   4. If a teardown hook is installed, call it (always — pass or fail).
 *   5. If the test failed AND retries remain, decrement retries and
 *      try again. Otherwise record the verdict.
 *   6. Update the progress bar.
 *
 * @param test         A `UNITTEST_CASE`-declared function.
 * @param total_tests  Total number of tests in the suite (used to size
 *                     the progress bar).
 */
#define UNITTEST_RUN_TEST(test, total_tests) UNITTEST_SAFE_BLOCK(\
    int retries = unittest_retry_count; \
    int unittest_skips_before = unittest_skipped; \
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
        double elapsed_time_ms = (end_time - start_time) * 1000.0; \
        UNITTEST_LOG("Elapsed time for %s: %.2f ms\n", #test, elapsed_time_ms); \
        /* checked once the test returns: a hung test is not interrupted */ \
        if (unittest_timeout_ms > 0 && elapsed_time_ms > unittest_timeout_ms) { \
            snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\tTest timed out after %.2f ms", #test, elapsed_time_ms); \
            unittest_status = 1; \
        } \
        /* Teardown should run after EVERY test execution — pass or fail. \
           Previously this only ran when the test had failed (because of \
           the early `break` on pass), leaving resources allocated by \
           setup() leaking on success. */ \
        if (unittest_teardown) (*unittest_teardown)(); \
        if (unittest_status == 0 || unittest_skipped != unittest_skips_before) { \
            break; /* passed or skipped: no retry */ \
        } \
        if (retries > 0) { \
            printf("\033[31mTest failed: %s. Retrying...\033[0m\n", #test); \
        } \
        retries--; \
    } \
    unittest_run++; /* once per test, however many attempts it took */ \
    if (!unittest_summary_only) { \
        unittest_display_progress(unittest_run, total_tests); \
    } \
    if (unittest_skipped != unittest_skips_before) { \
        UNITTEST_LOG("\033[33mTest skipped: %s\033[0m\n", #test); \
    } else if (retries < 0) { \
        unittest_fail++; \
        if (unittest_retry_count > 0) { \
            printf("\033[31mTest ultimately failed after retries: %s\033[0m\n", #test); \
        } else { \
            printf("\033[31mTest failed: %s\033[0m\n", #test); \
        } \
    } else { \
        UNITTEST_LOG("\033[32mTest passed: %s\033[0m\n", #test); \
    } \
)


/**
 * @def   UNITTEST_REPORT
 * @brief Print the final summary line (tests / assertions / failures /
 *        skipped) plus wall-clock and CPU elapsed time. Calls
 *        `exit(1)` if any tests failed.
 */
#define UNITTEST_REPORT() UNITTEST_SAFE_BLOCK(\
    double unittest_end_real_timer = unittest_timer_real(); \
    double unittest_end_proc_timer = unittest_timer_cpu(); \
    printf("\n\n\033[32m%d tests\033[0m, \033[32m%d assertions\033[0m, \033[31m%d failures\033[0m, \033[33m%d skipped\033[0m\n", \
        unittest_run, unittest_assert, unittest_fail, unittest_skipped); \
    if (unittest_skipped > 0) { \
        printf("\033[33m\nSkipped Tests:\n---------------\n%s\033[0m", unittest_skip_log); \
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


/**
 * @def   unittest_check
 * @brief Assert that @p test is truthy. Pass → prints `.`,
 *        fail → records `file:line: <expr>` and returns.
 */
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


/**
 * @def   unittest_fail
 * @brief Force a failure with a fixed message. Useful for
 *        unreachable-code or "this branch shouldn't execute" guards.
 */
#define unittest_fail(message) UNITTEST_SAFE_BLOCK(\
    unittest_assert++;\
    snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: %s", __func__, __FILE__, __LINE__, message);\
    unittest_status = 1;\
    return;\
)

/**
 * @def   unittest_assert
 * @brief Assert with a printf-style failure message.
 *
 * @code
 *     unittest_assert(n > 0, "expected positive, got %d", n);
 * @endcode
 */
#define unittest_assert(test, ...) UNITTEST_SAFE_BLOCK(\
    unittest_assert++;\
    if (!(test)) {\
        unittest_set_failure(__func__, __FILE__, __LINE__, NULL, " " __VA_ARGS__);\
        unittest_status = 1;\
        fprintf(stderr, "%s\n", unittest_last_message); /* Log failure message immediately */\
        return;\
    } else {\
        UNITTEST_LOG(".");\
    }\
)


/**
 * @def   unittest_assert_int_eq
 * @brief Assert that two `int` expressions are equal.
 */
#define unittest_assert_int_eq(expected, actual, ...) UNITTEST_SAFE_BLOCK(\
    int unittest_tmp_e = (expected); \
    int unittest_tmp_r = (actual); \
    unittest_assert++; \
    if (unittest_tmp_e != unittest_tmp_r) { \
        unittest_set_failure(__func__, __FILE__, __LINE__, NULL, " " __VA_ARGS__); \
        fprintf(stderr, "%s\n", unittest_last_message); /* Log failure message immediately */ \
        unittest_status = 1; \
        return; \
    } else { \
        UNITTEST_LOG("."); \
    } \
)

/**
 * @def   unittest_assert_double_eq
 * @brief Assert that two `double` expressions are equal within
 *        `UNITTEST_EPSILON` tolerance.
 */
#define unittest_assert_double_eq(expected, actual, ...) UNITTEST_SAFE_BLOCK(\
    double unittest_tmp_e = (expected); \
    double unittest_tmp_r = (actual); \
    unittest_assert++; \
    /* Fails for NaN (every comparison with NaN is false), passes for equal \
       infinities, and scales the tolerance with the magnitude of the values. */ \
    if (!(unittest_tmp_e == unittest_tmp_r || \
          fabs(unittest_tmp_e - unittest_tmp_r) <= UNITTEST_EPSILON * fmax(1.0, fmax(fabs(unittest_tmp_e), fabs(unittest_tmp_r))))) { \
        unittest_set_failure(__func__, __FILE__, __LINE__, NULL, " " __VA_ARGS__); \
        fprintf(stderr, "%s\n", unittest_last_message); /* Log failure message immediately */ \
        unittest_status = 1; \
        return; \
    } else { \
        UNITTEST_LOG("."); \
    } \
)


/**
 * @def   unittest_assert_string_eq
 * @brief Assert that two C strings are equal via `strcmp`. NULL is
 *        rendered as "<null>" in the failure message.
 */
#define unittest_assert_string_eq(expected, actual, ...) UNITTEST_SAFE_BLOCK(\
    const char* unittest_tmp_e = (expected); \
    const char* unittest_tmp_r = (actual); \
    unittest_assert++; \
    if (!unittest_tmp_e) unittest_tmp_e = "<null>"; \
    if (!unittest_tmp_r) unittest_tmp_r = "<null>"; \
    if (strcmp(unittest_tmp_e, unittest_tmp_r) != 0) { \
        unittest_set_failure(__func__, __FILE__, __LINE__, NULL, " " __VA_ARGS__); \
        fprintf(stderr, "%s\n", unittest_last_message); /* Log failure message immediately */ \
        unittest_status = 1; \
        return; \
    } else { \
        UNITTEST_LOG("."); \
    } \
)


/**
 * @def   unittest_assert_not_null
 * @brief Assert that @p pointer is non-NULL.
 */
#define unittest_assert_not_null(pointer, ...) UNITTEST_SAFE_BLOCK( \
    unittest_assert++; \
    if ((pointer) == NULL) { \
        unittest_set_failure(__func__, __FILE__, __LINE__, "Pointer is NULL", " " __VA_ARGS__); /* an empty message gives the default text */ \
        fprintf(stderr, "%s\n", unittest_last_message); /* Log failure message immediately */ \
        unittest_status = 1; \
        return; \
    } else { \
        UNITTEST_LOG("."); \
    } \
)


/**
 * @def   unittest_assert_in_range
 * @brief Assert that @p value is in the closed interval `[min, max]`.
 *        All three are coerced to `double` for the comparison.
 */
#define unittest_assert_in_range(value, min, max, ...) UNITTEST_SAFE_BLOCK( \
    double unittest_tmp_val = (value); \
    double unittest_tmp_min = (min); \
    double unittest_tmp_max = (max); \
    unittest_assert++; \
    if (!(unittest_tmp_val >= unittest_tmp_min && unittest_tmp_val <= unittest_tmp_max)) { /* NaN is out of range */ \
        unittest_set_failure(__func__, __FILE__, __LINE__, NULL, " " __VA_ARGS__); \
        fprintf(stderr, "%s\n", unittest_last_message); /* Log failure message immediately */ \
        unittest_status = 1; \
        return; \
    } else { \
        UNITTEST_LOG("."); \
    } \
)


/**
 * @def   unittest_assert_array_int_eq
 * @brief Element-wise equality assertion for `int` arrays. On the
 *        first differing index, prints `(expected: X, actual: Y)`.
 */
#define unittest_assert_array_int_eq(expected, actual, size, ...) UNITTEST_SAFE_BLOCK( \
    int unittest_size = (size); \
    int unittest_arrays_equal = 1; \
    /* unittest_i, not i: a caller's own `i` in the arguments (expected[i] in \
       a row loop) was captured by the macro's loop variable. */ \
    for (int unittest_i = 0; unittest_i < unittest_size; unittest_i++) { \
        if ((expected)[unittest_i] != (actual)[unittest_i]) { \
            unittest_arrays_equal = 0; \
            unittest_set_failure(__func__, __FILE__, __LINE__, "Arrays differ", " " __VA_ARGS__); \
            unittest_append_failure(" at index %d (expected: %d, actual: %d)", \
                                    unittest_i, (int)(expected)[unittest_i], (int)(actual)[unittest_i]); \
            break; \
        } \
    } \
    unittest_assert++; \
    if (!unittest_arrays_equal) { \
        fprintf(stderr, "%s\n", unittest_last_message); /* Log failure message immediately */ \
        unittest_status = 1; \
        return; \
    } else { \
        UNITTEST_LOG("."); \
    } \
)


/**
 * @def   unittest_assert_array_float_eq
 * @brief Element-wise equality for `float` arrays within a tolerance.
 */
#define unittest_assert_array_float_eq(expected, actual, size, epsilon, ...) UNITTEST_SAFE_BLOCK(\
    unittest_assert++; \
    int unittest_arrays_equal = 1; \
    for (size_t unittest_i = 0; unittest_i < (size_t)(size); unittest_i++) { \
        if (!((expected)[unittest_i] == (actual)[unittest_i] || fabs((expected)[unittest_i] - (actual)[unittest_i]) <= (epsilon))) { /* NaN fails */ \
            unittest_arrays_equal = 0; \
            unittest_set_failure(__func__, __FILE__, __LINE__, NULL, " " __VA_ARGS__); \
            unittest_append_failure(" (index %zu: expected %.6f, got %.6f)", \
                                    unittest_i, (double)(expected)[unittest_i], (double)(actual)[unittest_i]); \
            fprintf(stderr, "%s\n", unittest_last_message); \
            break; \
        } \
    } \
    if (!unittest_arrays_equal) { \
        unittest_status = 1; \
        return; \
    } else { \
        UNITTEST_LOG("."); \
    } \
)


/**
 * @def   unittest_assert_array_double_eq
 * @brief Element-wise equality for `double` arrays within a tolerance.
 */
#define unittest_assert_array_double_eq(expected, actual, size, epsilon, ...) UNITTEST_SAFE_BLOCK(\
    unittest_assert++; \
    int unittest_arrays_equal = 1; \
    for (size_t unittest_i = 0; unittest_i < (size_t)(size); unittest_i++) { \
        if (!((expected)[unittest_i] == (actual)[unittest_i] || fabs((expected)[unittest_i] - (actual)[unittest_i]) <= (epsilon))) { /* NaN fails */ \
            unittest_arrays_equal = 0; \
            unittest_set_failure(__func__, __FILE__, __LINE__, NULL, " " __VA_ARGS__); \
            unittest_append_failure(" (index %zu: expected %.12f, got %.12f)", \
                                    unittest_i, (double)(expected)[unittest_i], (double)(actual)[unittest_i]); \
            fprintf(stderr, "%s\n", unittest_last_message); \
            break; \
        } \
    } \
    if (!unittest_arrays_equal) { \
        unittest_status = 1; \
        return; \
    } else { \
        UNITTEST_LOG("."); \
    } \
)


/**
 * @def   unittest_assert_array_struct_eq
 * @brief Element-wise equality for arrays of POD structs using a
 *        user-supplied comparator (`bool comparator(const T*, const T*)`).
 */
#define unittest_assert_array_struct_eq(expected, actual, size, comparator, ...) UNITTEST_SAFE_BLOCK(\
    unittest_assert++; \
    int unittest_arrays_equal = 1; \
    for (size_t unittest_i = 0; unittest_i < (size_t)(size); unittest_i++) { \
        if (!(comparator(&(expected)[unittest_i], &(actual)[unittest_i]))) { \
            unittest_arrays_equal = 0; \
            unittest_set_failure(__func__, __FILE__, __LINE__, NULL, " " __VA_ARGS__); \
            unittest_append_failure(" (index %zu)", unittest_i); \
            fprintf(stderr, "%s\n", unittest_last_message); \
            break; \
        } \
    } \
    if (!unittest_arrays_equal) { \
        unittest_status = 1; \
        return; \
    } else { \
        UNITTEST_LOG("."); \
    } \
)


/**
 * @def   UNITTEST_PARAM_TEST
 * @brief Run a single test case with a parameter assigned to
 *        `unittest_param`. The test body reads the parameter from there.
 */
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


/**
 * @def   UNITTEST_RUN_PARAM_TEST
 * @brief Run @p test once for each entry in @p params (array of size
 *        @p count). Each invocation gets a pointer to the i-th param.
 */
#define UNITTEST_RUN_PARAM_TEST(test, params, count) UNITTEST_SAFE_BLOCK(\
    for (size_t i = 0; i < count; ++i) {\
        UNITTEST_PARAM_TEST(test, &params[i]);\
    }\
)


/**
 * @def   UNITTEST_SKIP
 * @brief Skip the current test with a recorded reason. Increments
 *        `unittest_skipped`. Returns from the test body.
 */
#define UNITTEST_SKIP(message) UNITTEST_SAFE_BLOCK(\
    unittest_record_skip(__func__, __FILE__, __LINE__, (message));\
    return;\
)


/**
 * @def   UNITTEST_CHECK_NO_LEAKS
 * @brief Run @p code_block and assert that `total_memory_allocated`
 *        is the same before and after. If not, prints every leaked
 *        record via `print_memory_leaks` and fails the test (counted
 *        once, by `UNITTEST_RUN_TEST`). Counts as one assertion; the
 *        block may contain top-level commas (`int a, b;`).
 *
 * @code
 *     UNITTEST_CHECK_NO_LEAKS({
 *         char* p = UNITTEST_MALLOC(64);
 *         UNITTEST_FREE(p);
 *     });
 * @endcode
 */
#define UNITTEST_CHECK_NO_LEAKS(...) UNITTEST_SAFE_BLOCK(\
    size_t memory_before = total_memory_allocated; \
    __VA_ARGS__; \
    size_t memory_after = total_memory_allocated; \
    unittest_assert++; \
    if (memory_before != memory_after) { \
        print_memory_leaks(); \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, \
            "%s failed:\n\t%s:%d: Memory leak detected: %zu bytes not freed", __func__, __FILE__, __LINE__, \
            memory_after - memory_before); \
        fprintf(stderr, "%s\n", unittest_last_message); \
        unittest_status = 1; \
    } else { \
        UNITTEST_LOG("."); \
    } \
)


/**
 * @def   unittest_assert_regex_match
 * @brief Assert that @p string contains a match for @p pattern using
 *        the project's PCRE-backed `regex_search`.
 */
#define unittest_assert_regex_match(pattern, string, flags, ...) UNITTEST_SAFE_BLOCK(\
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
        unittest_set_failure(__func__, __FILE__, __LINE__, NULL, " " __VA_ARGS__); \
        fprintf(stderr, "%s\n", unittest_last_message); \
        regex_deallocate(unittest_regex); \
        unittest_status = 1; \
        return; \
    } else { \
        UNITTEST_LOG("."); \
    } \
    regex_match_free(&unittest_match); \
    regex_deallocate(unittest_regex); \
)


/**
 * @def   unittest_assert_regex_group
 * @brief Assert that the @p group_index capture of regex @p pattern
 *        against @p input equals @p expected.
 */
#define unittest_assert_regex_group(pattern, input, group_index, expected, flags, ...) UNITTEST_SAFE_BLOCK( \
    /* group_index 0 is the FIRST capturing group (unlike regex_match_group, \
       where 0 is the whole match). Failures fail the test like every other \
       assertion; the group must equal `expected` exactly (a prefix such as \
       "2024" against "2024-EXTRA" used to pass). */ \
    unittest_assert++; \
    Regex* unittest_rx = regex_compile((pattern), (flags)); \
    if (!unittest_rx) { \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: Regex compilation failed for pattern '%s'", \
                 __func__, __FILE__, __LINE__, (pattern)); \
        fprintf(stderr, "%s\n", unittest_last_message); \
        unittest_status = 1; \
        return; \
    } \
    RegexMatch unittest_rm; \
    if (regex_search(unittest_rx, (input), &unittest_rm) != REGEX_SUCCESS) { \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: Regex search failed for input '%s'", \
                 __func__, __FILE__, __LINE__, (input)); \
        fprintf(stderr, "%s\n", unittest_last_message); \
        regex_deallocate(unittest_rx); \
        unittest_status = 1; \
        return; \
    } \
    int unittest_gi = (group_index); \
    if (unittest_gi < 0 || unittest_gi >= unittest_rm.group_count) { \
        snprintf(unittest_last_message, UNITTEST_MESSAGE_LEN, "%s failed:\n\t%s:%d: Group index %d out of bounds", \
                 __func__, __FILE__, __LINE__, unittest_gi); \
        fprintf(stderr, "%s\n", unittest_last_message); \
        regex_match_free(&unittest_rm); \
        regex_deallocate(unittest_rx); \
        unittest_status = 1; \
        return; \
    } \
    const char* unittest_gs = unittest_rm.group_starts[unittest_gi]; \
    size_t unittest_gl = unittest_gs ? unittest_rm.group_lengths[unittest_gi] : 0; \
    const char* unittest_ex = (expected) ? (expected) : ""; \
    int unittest_same = strlen(unittest_ex) == unittest_gl && (unittest_gl == 0 || memcmp(unittest_gs, unittest_ex, unittest_gl) == 0); \
    regex_match_free(&unittest_rm); \
    regex_deallocate(unittest_rx); \
    if (!unittest_same) { \
        unittest_set_failure(__func__, __FILE__, __LINE__, NULL, " " __VA_ARGS__); \
        fprintf(stderr, "%s\n", unittest_last_message); \
        unittest_status = 1; \
        return; \
    } else { \
        UNITTEST_LOG("."); \
    } \
)


/**
 * @def   unittest_assert_file_eq
 * @brief Byte-by-byte comparison of two files (1 KiB chunks). On the
 *        first differing chunk reports the line number (1-based, in
 *        units of 1 KiB blocks).
 */
#define unittest_assert_file_eq(file1, file2, ...) UNITTEST_SAFE_BLOCK( \
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
    /* Both files are read every round: the old && skipped reading file2 \
       once file1 ran dry, so a file equal to a prefix of the other passed. */ \
    for (;;) { \
        unittest_read1 = fread(unittest_buf1, 1, sizeof(unittest_buf1), unittest_f1); \
        unittest_read2 = fread(unittest_buf2, 1, sizeof(unittest_buf2), unittest_f2); \
        if (unittest_read1 != unittest_read2) { \
            unittest_files_equal = 0; \
            unittest_set_failure(__func__, __FILE__, __LINE__, NULL, " " __VA_ARGS__); \
            unittest_append_failure(" (file lengths differ)"); \
            fprintf(stderr, "%s\n", unittest_last_message); \
            break; \
        } \
        if (unittest_read1 == 0) { \
            break; \
        } \
        if (memcmp(unittest_buf1, unittest_buf2, unittest_read1) != 0) { \
            unittest_files_equal = 0; \
            unittest_set_failure(__func__, __FILE__, __LINE__, NULL, " " __VA_ARGS__); \
            unittest_append_failure(" (file content differs at line %d)", unittest_line_number); \
            fprintf(stderr, "%s\n", unittest_last_message); \
            break; \
        } \
        unittest_line_number++; \
    } \
    fclose(unittest_f1); \
    fclose(unittest_f2); \
    if (!unittest_files_equal) { \
        unittest_status = 1; \
        return; \
    } else { \
        UNITTEST_LOG("."); \
    } \
)


/**
 * @def   unittest_assert_file_exists
 * @brief Assert that @p filepath exists (`unittest_file_exists`).
 */
#define unittest_assert_file_exists(filepath, ...) UNITTEST_SAFE_BLOCK({ \
    unittest_assert++; \
    if (!unittest_file_exists(filepath)) { \
        unittest_set_failure(__func__, __FILE__, __LINE__, NULL, " " __VA_ARGS__); \
        fprintf(stderr, "%s\n", unittest_last_message); \
        unittest_status = 1; \
        return; \
    } else { \
        UNITTEST_LOG("."); \
    } \
})


/**
 * @def   unittest_assert_file_not_exists
 * @brief Assert that @p filepath does NOT exist.
 */
#define unittest_assert_file_not_exists(filepath, ...) UNITTEST_SAFE_BLOCK({ \
    unittest_assert++; \
    if (unittest_file_exists(filepath)) { \
        unittest_set_failure(__func__, __FILE__, __LINE__, NULL, " " __VA_ARGS__); \
        fprintf(stderr, "%s\n", unittest_last_message); \
        unittest_status = 1; \
        return; \
    } else { \
        UNITTEST_LOG("."); \
    } \
})


/**
 * @def   UNITTEST_MOCK_FUNCTION
 * @brief Forward-declare a mock function. Implementation is expected
 *        to call `unittest_register_mock_call` on entry.
 */
#define UNITTEST_MOCK_FUNCTION(func_name, ...) \
    void func_name(__VA_ARGS__);


/**
 * @def   UNITTEST_MOCK_FUNCTION_WITH_STUB
 * @brief Forward-declare a mock function with a non-void return.
 *        The implementation typically delegates to
 *        `unittest_get_stub_return_value` for the return value.
 */
#define UNITTEST_MOCK_FUNCTION_WITH_STUB(func_name, return_type, ...) \
    return_type func_name(__VA_ARGS__);


/**
 * @def   UNITTEST_VERIFY_CALL_COUNT
 * @brief Assert that @p func was invoked exactly @p count times.
 */
#define UNITTEST_VERIFY_CALL_COUNT(func, count) \
    unittest_verify_mock_call_count(#func, count)


/**
 * @def   UNITTEST_VERIFY_ARGUMENTS
 * @brief Assert that the most recent call to @p func received the
 *        given arguments (each as a C string). (It passed 3 arguments to
 *        the 4-parameter unittest_verify_mock_call and did not compile;
 *        call index -1 now means "the most recent call".)
 */
#define UNITTEST_VERIFY_ARGUMENTS(func, ...) \
    do { \
        void* unittest_args[] = {__VA_ARGS__}; \
        unittest_verify_mock_call(#func, sizeof(unittest_args) / sizeof(unittest_args[0]), unittest_args, -1); \
    } while (0)



#ifdef __cplusplus
}
#endif

#endif
