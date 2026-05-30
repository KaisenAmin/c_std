/**
 * @file   unittest.c
 * @author Amin Tahmasebi
 * @date   2024
 * @class  Unittest
 *
 * @brief  Implementation of the Unittest framework.
 *
 * This translation unit owns three static-storage registries:
 *
 *   - `mock_registry`         — a linked list of `MockCall` nodes that
 *                                grows every time a mock function calls
 *                                `unittest_register_mock_call`. Each
 *                                node owns its strdup'd strings.
 *   - `stub_registry`         — a `MAX_STUBS`-element static array of
 *                                `Stub` entries. Pointers are stored,
 *                                not copied (the caller keeps lifetime
 *                                ownership of the strings).
 *   - `memory_head` (with
 *      `total_memory_allocated`) — a singly-linked list of
 *                                `UNITTEST_MALLOC` allocations and a
 *                                running byte count used by
 *                                `UNITTEST_CHECK_NO_LEAKS`.
 *
 * The framework also exposes a set of public counters (`unittest_run`,
 * `unittest_assert`, `unittest_fail`, ...) and two timer baselines
 * (`unittest_real_timer`, `unittest_proc_timer`) so the macros defined
 * in `unittest.h` can do their bookkeeping without exposing any
 * additional accessors.
 *
 * No threading. The framework is intended for single-process,
 * single-threaded test runs.
 */

#include "unittest.h"
#include <time.h>


/** @brief Global mock-call registry head. */
static MockRegistry mock_registry = {NULL};

/* Stub registry sized to MAX_STUBS so the bounds check in
 * unittest_register_stub (which compares against MAX_STUBS) matches the
 * actual array size. Previously the array was 10 entries but the check
 * allowed up to 100 — entries 10..99 would silently corrupt adjacent
 * memory. */
/** @brief Static stub registry (fixed size = `MAX_STUBS`). */
static Stub stub_registry[MAX_STUBS];

/** @brief Number of stubs currently registered in `stub_registry`. */
static int stub_count = 0;

/** @brief Head of the live `UNITTEST_MALLOC` allocations list. */
static MemoryRecord* memory_head = NULL;

/** @brief Running total of bytes tracked by `unittest_malloc`. */
size_t total_memory_allocated = 0;

/** @brief When non-zero, per-check `.` output is suppressed. */
int unittest_summary_only = 0;

/* ---- Public counters and timer baselines ---- */
int unittest_retry_count = 0;
int unittest_run         = 0;
int unittest_assert      = 0;
int unittest_fail        = 0;
int unittest_status      = 0;
int unittest_skipped     = 0;

double unittest_timeout_ms = 0;
double unittest_real_timer = 0;
double unittest_proc_timer = 0;

char unittest_last_message[UNITTEST_MESSAGE_LEN];

/* ---- Optional per-test setup/teardown hooks ---- */
void (*unittest_setup)(void)    = NULL;
void (*unittest_teardown)(void) = NULL;

/** @brief Current per-test parameter pointer for `UNITTEST_PARAM_TEST`. */
void* unittest_param = NULL;


/**
 * @brief Return the current wall-clock time in seconds.
 *
 * Cross-platform implementation:
 *   - Windows: `QueryPerformanceCounter` / `QueryPerformanceFrequency`.
 *   - macOS:   `mach_absolute_time` scaled via `mach_timebase_info`.
 *   - POSIX:   `gettimeofday`.
 *
 * @return Current wall-clock time in seconds, or `-1.0` on failure /
 *         unsupported platform.
 */
double unittest_timer_real(void) {
#if defined(_WIN32)
    LARGE_INTEGER Time, Frequency;
    if (!QueryPerformanceFrequency(&Frequency) || !QueryPerformanceCounter(&Time)) {
        fprintf(stderr, "Error: QueryPerformanceCounter failed.\n");
        return -1.0;
    }
    return (double)Time.QuadPart / (double)Frequency.QuadPart;

#elif defined(__MACH__) && defined(__APPLE__)
    static double timeConvert = 0.0;
    if (timeConvert == 0.0) {
        mach_timebase_info_data_t timeBase;
        if (mach_timebase_info(&timeBase) != KERN_SUCCESS) {
            fprintf(stderr, "Error: mach_timebase_info failed.\n");
            return -1.0;
        }
        timeConvert = (double)timeBase.numer / (double)timeBase.denom / 1e9;
    }
    return (double)mach_absolute_time() * timeConvert;

#elif defined(_POSIX_VERSION)
    struct timeval tm;
    if (gettimeofday(&tm, NULL) != 0) {
        perror("Error: gettimeofday failed");
        return -1.0;
    }
    return (double)tm.tv_sec + (double)tm.tv_usec / 1e6;

#else
    fprintf(stderr, "Error: Timer not supported on this platform.\n");
    return -1.0;
#endif
}


/**
 * @brief Return the current process CPU time in seconds.
 *
 * Implementation:
 *   - Windows: `GetProcessTimes` — sums user + kernel time.
 *   - POSIX:   `clock_gettime(CLOCK_PROCESS_CPUTIME_ID)`.
 *
 * @return CPU time in seconds, or `-1.0` if unavailable.
 */
double unittest_timer_cpu(void) {
#if defined(_WIN32)
    FILETIME createTime, exitTime, kernelTime, userTime;
    if (GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &kernelTime, &userTime)) {
        ULARGE_INTEGER userSysTime;
        memcpy(&userSysTime, &userTime, sizeof(ULARGE_INTEGER));

        return (double)userSysTime.QuadPart / 1e7;
    }
    return -1.0;

#elif defined(_POSIX_VERSION)
    struct timespec ts;
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) == 0) {
        return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
    }
    return -1.0;

#else
    return -1.0;
#endif
}


/**
 * @brief Configure the per-test wall-clock timeout (in milliseconds).
 *
 * After setting this, `UNITTEST_RUN_TEST` will mark any test that
 * runs longer than `timeout_ms` as failed. Pass 0 to disable.
 *
 * @param timeout_ms Timeout in milliseconds; 0 = no timeout.
 */
void unittest_set_timeout(double timeout_ms) {
    unittest_timeout_ms = timeout_ms;
}


/**
 * @brief Return the total bytes currently tracked by `unittest_malloc`.
 *
 * Equivalent to reading `total_memory_allocated`. Provided as a
 * function for callers who prefer that style.
 */
size_t unittest_get_memory_usage(void) {
    return total_memory_allocated;
}


/**
 * @brief Append a record to the live-allocation tracking list.
 *
 * Used by `unittest_malloc`. If the tracking node itself fails to
 * allocate, `total_memory_allocated` is still updated so the leak
 * total stays approximately correct.
 *
 * @param address  Pointer returned by the underlying `malloc`.
 * @param size     Byte size of the allocation.
 * @param file     Source file where the call originated.
 * @param line     Source line where the call originated.
 */
void add_memory_record(void* address, size_t size, const char* file, int line) {
    MemoryRecord* record = (MemoryRecord*)malloc(sizeof(MemoryRecord));
    if (!record) {
        // Can't track the allocation but don't blow up the program. Still
        // count the bytes so the total stays approximately correct.
        total_memory_allocated += size;
        return;
    }
    record->address = address;
    record->size = size;
    record->file = file;
    record->line = line;
    record->next = memory_head;
    memory_head = record;
    total_memory_allocated += size;
}


/**
 * @brief Remove the record for @p address from the tracking list and
 *        decrement `total_memory_allocated` by its recorded size.
 *
 * Called by `unittest_free` before forwarding to libc `free()`.
 * Unknown addresses are silently ignored.
 *
 * @param address Pointer whose tracking record should be removed.
 */
void remove_memory_record(void* address) {
    MemoryRecord** current = &memory_head;
    while (*current) {
        if ((*current)->address == address) {
            MemoryRecord* to_delete = *current;
            *current = (*current)->next;
            total_memory_allocated -= to_delete->size;
            free(to_delete);
            return;
        }
        current = &(*current)->next;
    }
}


/**
 * @brief Print every live `unittest_malloc` record to stdout.
 *
 * Walks the tracking list and emits one
 *   `Memory leak detected: <N> bytes not freed. Allocated at <file>:<line>`
 * line per remaining entry. Called by `UNITTEST_CHECK_NO_LEAKS` when a
 * block leaks, but safe to invoke ad-hoc at any time — it does not
 * mutate the list.
 */
void print_memory_leaks() {
    MemoryRecord* current = memory_head;
    while (current) {
        printf("Memory leak detected: %zu bytes not freed. Allocated at %s:%d\n",
               current->size, current->file, current->line);
        current = current->next;
    }
}


/**
 * @brief Allocate @p size bytes and record the allocation in the
 *        tracking list (so it shows up in leak reports).
 *
 * Use the `UNITTEST_MALLOC(size)` macro instead of calling this
 * directly so the source file and line are captured automatically.
 *
 * @param size  Bytes to allocate.
 * @param file  Caller's source file (filled by the macro).
 * @param line  Caller's source line (filled by the macro).
 *
 * @return Pointer to the new block, or NULL if `malloc` failed. On
 *         OOM no record is added.
 */
void* unittest_malloc(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    if (ptr) {
        add_memory_record(ptr, size, file, line);
    }
    return ptr;
}


/**
 * @brief Free a pointer previously returned by `unittest_malloc`.
 *
 * Removes the matching tracking record (if any) then forwards to
 * libc `free()`. Safe with NULL.
 */
void unittest_free(void* ptr) {
    if (ptr) {
        remove_memory_record(ptr);
    }
    free(ptr);
}


/**
 * @brief Record a mock-function invocation in the global registry.
 *
 * Prepends a new node to `mock_registry` carrying a `strdup` of
 * @p function_name and a `strdup` of each `args[i]`. The new entry's
 * `index` is set to the count of prior calls with the same function
 * name so `unittest_get_mock_call_by_index` can address it later.
 *
 * Captured strings are owned by the framework and freed by
 * `unittest_clear_mock_registry`.
 *
 * @param function_name  Mock function name.
 * @param arg_count      Number of args (capped at `MAX_ARGS`).
 * @param args           Array of pointers, each treated as a C string.
 */
void unittest_register_mock_call(const char* function_name, size_t arg_count, void** args) {
    // Determine current index for the function
    int index = 0;
    MockCall* current = mock_registry.head;
    while (current) {
        if (strcmp(current->function_name, function_name) == 0) {
            index++;
        }
        current = current->next;
    }

    // Register the new call
    MockCall* new_call = (MockCall*)malloc(sizeof(MockCall));
    new_call->function_name = strdup(function_name);
    new_call->call_count = index + 1;
    new_call->arg_count = arg_count;
    new_call->index = index; // Set the unique index

    for (size_t i = 0; i < arg_count; i++) {
        new_call->actual_args[i] = strdup((const char*)args[i]);
    }

    new_call->next = mock_registry.head;
    mock_registry.head = new_call;
}


/**
 * @brief Return the most-recent `MockCall` registered for @p function_name.
 *
 * Walks `mock_registry` from head and returns the first match. Because
 * new entries are prepended at the head, this is the LAST call made
 * for that name. To address a specific occurrence, use
 * `unittest_get_mock_call_by_index`.
 *
 * @return Pointer to the node (NOT owned by the caller), or NULL if
 *         no entry matches.
 */
MockCall* unittest_get_mock_call(const char* function_name) {
    MockCall* current = mock_registry.head;
    while (current) {
        if (strcmp(current->function_name, function_name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}


/**
 * @brief Assert that a specific recorded mock call matches the
 *        expected arguments.
 *
 * Looks up the (function, call_index) entry in the registry, asserts
 * via `unittest_assert` that the captured `arg_count` equals
 * @p arg_count, and that every `actual_args[i]` matches
 * `expected_args[i]` via `strcmp`. Any mismatch records a failure.
 *
 * @param function_name  Mock function name to look up.
 * @param arg_count      Expected number of arguments.
 * @param expected_args  Array of expected argument strings.
 * @param call_index     Zero-based occurrence index.
 */
void unittest_verify_mock_call(const char* function_name, size_t arg_count, void** expected_args, int call_index) {
    MockCall* call = unittest_get_mock_call_by_index(function_name, call_index);

    unittest_assert(call != NULL, "Mock function '%s' was not called for index %d", function_name, call_index);
    unittest_assert(call->arg_count == arg_count, "Expected %zu arguments but got %zu for '%s' at index %d", arg_count, call->arg_count, function_name, call_index);

    for (size_t i = 0; i < arg_count; i++) {
        const char* actual_arg = (const char*)call->actual_args[i];
        const char* expected_arg = (const char*)expected_args[i];

        if (strcmp(actual_arg, expected_arg) != 0) {
            unittest_assert(false, "Argument %zu mismatch in '%s' at index %d: expected '%s', got '%s'", i, function_name, call_index, expected_arg, actual_arg);
        }
    }
}


/**
 * @brief Assert that @p function_name was invoked exactly
 *        @p expected_count times.
 *
 * Counts every entry in `mock_registry` whose `function_name` matches
 * and triggers a `unittest_assert` failure if the totals differ.
 */
void unittest_verify_mock_call_count(const char* function_name, int expected_count) {
    int actual_count = 0;
    MockCall* current = mock_registry.head;

    while (current) {
        if (strcmp(current->function_name, function_name) == 0) {
            actual_count++;
        }
        current = current->next;
    }

    unittest_assert(actual_count == expected_count, "Expected '%s' to be called %d times but was called %d times", function_name, expected_count, actual_count);
}


/**
 * @brief Register a stub that maps (@p function_name, @p arg) to
 *        @p return_value.
 *
 * Stored in the static `stub_registry`. The framework does NOT take
 * ownership of any of the pointers — keep them alive until you
 * `unittest_clear_stub_registry`.
 *
 * If the registry is full, the call is dropped with a warning (the
 * framework never aborts the host process). NULL `function_name` is
 * rejected silently.
 */
void unittest_register_stub(const char* function_name, const char* arg, void* return_value) {
    if (stub_count >= MAX_STUBS) {
        fprintf(stderr, "Warning: Stub registry full (%d entries); '%s' not registered.\n", MAX_STUBS, function_name ? function_name : "(null)");
        return;
    }
    if (!function_name) {
        return;
    }

    stub_registry[stub_count].function_name = function_name;
    stub_registry[stub_count].arg = arg;
    stub_registry[stub_count].return_value = return_value;
    stub_count++;
}


/**
 * @brief Look up a previously-registered stub.
 *
 * @return The stored `return_value` for the matching (function_name,
 *         arg) pair, or NULL if no stub matches.
 */
void* unittest_get_stub_return_value(const char* function_name, const char* arg) {
    for (int i = 0; i < stub_count; i++) {
        if (strcmp(stub_registry[i].function_name, function_name) == 0 &&
            strcmp(stub_registry[i].arg, arg) == 0) {
            return stub_registry[i].return_value;
        }
    }
    return NULL;
}


/**
 * @brief Look up the @p call_index-th recorded call to @p function_name.
 *
 * Emits a single-line debug print indicating whether a match was found
 * (useful when test output otherwise gives no hint why a verification
 * failed).
 *
 * @return Pointer to the matching node (NOT owned by caller), or NULL
 *         if no entry matches.
 */
MockCall* unittest_get_mock_call_by_index(const char* function_name, int call_index) {
    MockCall* current = mock_registry.head;
    while (current) {
        if (strcmp(current->function_name, function_name) == 0 && current->index == call_index) {
            printf("Debug: Found call '%s' at index %d\n", function_name, call_index);
            return current;
        }
        current = current->next;
    }

    printf("Debug: No matching call for '%s' at index %d\n", function_name, call_index);
    return NULL; 
}


/**
 * @brief Uniformly-distributed pseudo-random `int` in `[min, max]`.
 *
 * If `max < min` the arguments are swapped so the call is always
 * valid. Uses `rand()` — seed with `srand()` for reproducibility.
 *
 * @return Random integer in the inclusive range.
 */
int unittest_generate_random_int(int min, int max) {
    // Guard against inverted range (would cause div-by-zero on max == min - 1
    // or a negative divisor on max < min). Swap so min <= max.
    if (max < min) {
        int t = min; min = max; max = t;
    }
    
    long long span = (long long)max - (long long)min + 1; // can't overflow int range
    if (span <= 0) {
        return min;
    }

    return min + (int)((unsigned long long)rand() % (unsigned long long)span);
}


/**
 * @brief Uniformly-distributed pseudo-random `float` in `[min, max]`.
 */
float unittest_generate_random_float(float min, float max) {
    return min + ((float)rand() / RAND_MAX) * (max - min);
}


/**
 * @brief Uniformly-distributed pseudo-random `double` in `[min, max]`.
 */
double unittest_generate_random_double(double min, double max) {
    return min + ((double)rand() / RAND_MAX) * (max - min);
}


/**
 * @brief Generate a `length`-character random alphanumeric string.
 *
 * Heap-allocated; NUL-terminated. Caller releases with `free()` or
 * `unittest_free_generated_data`.
 *
 * @return Pointer to the new string, or NULL on allocation failure.
 */
char* unittest_generate_random_string(size_t length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char* random_string = (char*)malloc((length + 1) * sizeof(char));

    if (!random_string) {
        fprintf(stderr, "Memory allocation failed for random string\n");
        return NULL;
    }

    for (size_t i = 0; i < length; i++) {
        random_string[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    random_string[length] = '\0';

    return random_string;
}


/**
 * @brief Return a static array of int edge cases.
 *
 * Values: `{INT_MIN, -1, 0, 1, INT_MAX}`. The pointer is internal —
 * do NOT free. Writes the count (5) into `*size`.
 */
int* unittest_generate_edge_case_integers(size_t* size) {
    static int edge_cases[] = {INT_MIN, -1, 0, 1, INT_MAX};
    *size = sizeof(edge_cases) / sizeof(edge_cases[0]);

    return edge_cases;
}


/**
 * @brief Return a static array of float edge cases.
 *
 * Values: `{-FLT_MAX, -1.0f, 0.0f, 1.0f, FLT_MAX}`. Do NOT free.
 */
float* unittest_generate_edge_case_floats(size_t* size) {
    static float edge_cases[] = {-FLT_MAX, -1.0f, 0.0f, 1.0f, FLT_MAX};
    *size = sizeof(edge_cases) / sizeof(edge_cases[0]);

    return edge_cases;
}


/**
 * @brief Return a static array of double edge cases.
 *
 * Values: `{-DBL_MAX, -1.0, 0.0, 1.0, DBL_MAX}`. Do NOT free.
 */
double* unittest_generate_edge_case_doubles(size_t* size) {
    static double edge_cases[] = {-DBL_MAX, -1.0f, 0.0f, 1.0f, DBL_MAX};
    *size = sizeof(edge_cases) / sizeof(edge_cases[0]);

    return edge_cases;
}


/**
 * @brief Return a static array of string edge cases.
 *
 * Entries: empty, single char, 80-char run, embedded NUL, newline,
 * tab. The pointers are to internal storage — do NOT free the strings
 * OR the outer array.
 */
char** unittest_generate_edge_case_strings(size_t* size) {
    static const char* edge_cases[] = {
        "",
        "a",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", // 80 chars
        "\0hidden_null",
        "newline\n",
        "tab\t",
    };
    *size = sizeof(edge_cases) / sizeof(edge_cases[0]);
    return (char**)edge_cases;
}


/**
 * @brief Free a heap block produced by a generator (currently only
 *        `unittest_generate_random_string` allocates). Safe with NULL.
 */
void unittest_free_generated_data(void* data) {
    if (data) {
        free(data);
    }
}


/**
 * @brief Render a single-line progress bar reflecting
 *        `completed / total` test cases.
 *
 * Writes to stdout with a leading `\r` and an `fflush(stdout)` so
 * successive renders overwrite the same line. Called by
 * `UNITTEST_RUN_TEST` after each case.
 *
 * @param completed Number of tests finished so far.
 * @param total     Total number of tests in this suite.
 */
void unittest_display_progress(int completed, int total) {
    int bar_width = 50;  // Width of the progress bar
    float progress = (float)completed / total;
    int pos = (int)(bar_width * progress);

    printf("\rProgress: [");
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) {
            printf("=");
        }
        else if (i == pos) {
            printf(">");
        }
        else {
            printf(" ");
        }
    }

    printf("] %d/%d (%.2f%%)", completed, total, progress * 100);
    fflush(stdout);
}


/**
 * @brief Reset every global test-runner counter back to its starting
 *        state, including the cached real / CPU timer baselines.
 *
 * Use this when you want to run multiple independent test passes in
 * the same process (benchmarks, parameterised suites, long-running
 * fixtures). It resets:
 *   - `unittest_run`, `unittest_assert`, `unittest_fail`, `unittest_status`,
 *     `unittest_skipped`
 *   - `unittest_real_timer`, `unittest_proc_timer` (so the next
 *     `UNITTEST_RUN_TEST` re-captures the baseline)
 *   - `unittest_last_message` (clears any pending failure message)
 *   - `unittest_retry_count`, `unittest_timeout_ms`, `unittest_param`
 *
 * Does NOT touch mock/stub registries or the memory-tracking list —
 * use `unittest_clear_mock_registry`, `unittest_clear_stub_registry`,
 * and the existing `unittest_free` for those.
 *
 * @code
 *   for (int trial = 0; trial < 5; ++trial) {
 *       unittest_reset_state();
 *       UNITTEST_RUN_SUITE(my_suite);
 *       printf("trial %d: %d/%d passed\n",
 *              trial, unittest_run - unittest_fail, unittest_run);
 *   }
 * @endcode
 */
void unittest_reset_state(void) {
    unittest_run       = 0;
    unittest_assert    = 0;
    unittest_fail      = 0;
    unittest_status    = 0;
    unittest_skipped   = 0;
    unittest_real_timer = 0.0;
    unittest_proc_timer = 0.0;
    unittest_timeout_ms = 0.0;
    unittest_retry_count = 0;
    unittest_param     = NULL;
    unittest_last_message[0] = '\0';
}


/**
 * @brief Free every heap allocation owned by the mock-call registry.
 *
 * Walks the registry linked list, freeing each `MockCall`'s
 * strdup'd `function_name`, each strdup'd entry in `actual_args`, and
 * then the node itself. After this call the registry is empty and
 * `unittest_get_mock_call` / `_by_index` will return NULL.
 *
 * Call between test groups, or at the end of `main()`, so that
 * accumulated mock-call data doesn't leak across runs and doesn't
 * leak when the process exits.
 *
 * @code
 *   // test_alpha runs ...
 *   UNITTEST_VERIFY_CALL_COUNT(my_mock, 1);
 *   unittest_clear_mock_registry();
 *
 *   // test_beta starts from a clean slate
 * @endcode
 */
void unittest_clear_mock_registry(void) {
    MockCall* current = mock_registry.head;
    while (current) {
        MockCall* next = current->next;
        free(current->function_name);
        for (size_t i = 0; i < current->arg_count && i < MAX_ARGS; ++i) {
            free(current->actual_args[i]);
        }
        free(current);
        current = next;
    }
    mock_registry.head = NULL;
}


/**
 * @brief Reset the stub registry.
 *
 * `unittest_register_stub` stores the supplied pointers in a static
 * array — it does NOT take ownership of the `function_name`, `arg`,
 * or `return_value` strings, so this function only needs to drop the
 * index. After clearing, `unittest_get_stub_return_value` will report
 * "no match" for every query until something is re-registered.
 *
 * @code
 *   unittest_register_stub("fetch", "key1", "value1");
 *   const char* v = unittest_get_stub_return_value("fetch", "key1"); // "value1"
 *   unittest_clear_stub_registry();
 *   v = unittest_get_stub_return_value("fetch", "key1");             // NULL
 * @endcode
 */
void unittest_clear_stub_registry(void) {
    stub_count = 0;

    for (int i = 0; i < MAX_STUBS; ++i) {
        stub_registry[i].function_name = NULL;
        stub_registry[i].arg = NULL;
        stub_registry[i].return_value = NULL;
    }
}


/**
 * @brief Fill @p buffer with @p size random bytes (0..255).
 *
 * Pulls each byte from `rand()` modulo 256. Useful for fuzzing
 * binary parsers, generating randomized test payloads for hashing,
 * compression, or network code, and seeding edge-case input. NULL
 * buffer or zero size is a safe no-op.
 *
 * For deterministic test runs seed `rand()` first:
 *
 * @code
 *   srand(12345);
 *   unsigned char payload[256];
 *   unittest_generate_random_bytes(payload, sizeof(payload));
 *   uint32_t crc = my_crc32(payload, sizeof(payload));
 *   unittest_check(crc != 0);
 * @endcode
 */
void unittest_generate_random_bytes(unsigned char* buffer, size_t size) {
    if (!buffer || size == 0) { 
        return;
    }
    for (size_t i = 0; i < size; ++i) {
        buffer[i] = (unsigned char)(rand() & 0xFF);
    }
}


/**
 * @brief Programmatic getter for an argument that a mock function
 *        was called with — returns the captured string for inspection.
 *
 * Pairs with `unittest_register_mock_call`. Unlike
 * `unittest_verify_mock_call`, this does NOT fail the test on a miss
 * — it simply returns NULL — so it's appropriate when test logic
 * needs to branch on the captured value.
 *
 * @param function_name Mock function to look up. NULL → NULL.
 * @param call_index    Zero-based call index (first call = 0).
 * @param arg_index     Zero-based argument index. Must be <
 *                      MAX_ARGS and < the call's recorded
 *                      `arg_count`.
 * @return Borrowed pointer to the captured argument string. Lifetime
 *         is owned by the mock call entry — it stays valid until
 *         `unittest_clear_mock_registry` is called. NULL on any
 *         lookup failure.
 *
 * @code
 *   void* args[] = { "alice", "42" };
 *   unittest_register_mock_call("greet", 2, args);
 *
 *   const char* name = unittest_mock_call_arg("greet", 0, 0);   // "alice"
 *   const char* age  = unittest_mock_call_arg("greet", 0, 1);   // "42"
 *   const char* miss = unittest_mock_call_arg("greet", 99, 0);  // NULL
 * @endcode
 */
const char* unittest_mock_call_arg(const char* function_name, int call_index, size_t arg_index) {
    if (!function_name) {
        return NULL;
    }
    if (call_index < 0)  {
        return NULL;
    }
    if (arg_index >= MAX_ARGS) {
        return NULL;
    }

    MockCall* current = mock_registry.head;
    while (current) {
        if (strcmp(current->function_name, function_name) == 0 &&
            current->index == call_index) {
            if (arg_index >= current->arg_count) { 
                return NULL;
            }
            return (const char*)current->actual_args[arg_index];
        }
        current = current->next;
    }
    return NULL;
}
