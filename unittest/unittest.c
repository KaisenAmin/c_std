#include "unittest.h"
#include <time.h>


static MockRegistry mock_registry = {NULL};  // Global registry for mock calls
static Stub stub_registry[10];  // Adjust size as needed
static int stub_count = 0;
static MemoryRecord* memory_head = NULL;
size_t total_memory_allocated = 0;
int unittest_summary_only = 0;

int unittest_retry_count = 0;
int unittest_run = 0;
int unittest_assert = 0;
int unittest_fail = 0;
int unittest_status = 0;
int unittest_skipped = 0;

double unittest_timeout_ms = 0; 
double unittest_real_timer = 0;
double unittest_proc_timer = 0;

char unittest_last_message[UNITTEST_MESSAGE_LEN];

void (*unittest_setup)(void) = NULL;
void (*unittest_teardown)(void) = NULL;
void* unittest_param = NULL;

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


double unittest_timer_cpu(void) {
#if defined(_WIN32)
    FILETIME createTime, exitTime, kernelTime, userTime;
    if (GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &kernelTime, &userTime)) {
        ULARGE_INTEGER userSysTime;
        memcpy(&userSysTime, &userTime, sizeof(ULARGE_INTEGER));
        return (double)userSysTime.QuadPart / 1e7; /* Convert to seconds */
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

void unittest_set_timeout(double timeout_ms) {
    unittest_timeout_ms = timeout_ms;
}

size_t unittest_get_memory_usage(void) {
    return total_memory_allocated;
}

// Add a record to the tracking list
void add_memory_record(void* address, size_t size, const char* file, int line) {
    MemoryRecord* record = (MemoryRecord*)malloc(sizeof(MemoryRecord));
    record->address = address;
    record->size = size;
    record->file = file;
    record->line = line;
    record->next = memory_head;
    memory_head = record;
    total_memory_allocated += size;
}

// Remove a record from the tracking list
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

// Print all unfreed memory records
void print_memory_leaks() {
    MemoryRecord* current = memory_head;
    while (current) {
        printf("Memory leak detected: %zu bytes not freed. Allocated at %s:%d\n",
               current->size, current->file, current->line);
        current = current->next;
    }
}

void* unittest_malloc(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    if (ptr) {
        add_memory_record(ptr, size, file, line);
    }
    return ptr;
}

void unittest_free(void* ptr) {
    if (ptr) {
        remove_memory_record(ptr);
    }
    free(ptr);
}

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

void unittest_verify_mock_call(const char* function_name, size_t arg_count, void** expected_args, int call_index) {
    MockCall* call = unittest_get_mock_call_by_index(function_name, call_index);
    unittest_assert(call != NULL, "Mock function '%s' was not called for index %d", function_name, call_index);
    unittest_assert(call->arg_count == arg_count, "Expected %zu arguments but got %zu for '%s' at index %d",
                    arg_count, call->arg_count, function_name, call_index);

    for (size_t i = 0; i < arg_count; i++) {
        const char* actual_arg = (const char*)call->actual_args[i];
        const char* expected_arg = (const char*)expected_args[i];
        if (strcmp(actual_arg, expected_arg) != 0) {
            unittest_assert(false, "Argument %zu mismatch in '%s' at index %d: expected '%s', got '%s'",
                            i, function_name, call_index, expected_arg, actual_arg);
        }
    }
}

void unittest_verify_mock_call_count(const char* function_name, int expected_count) {
    int actual_count = 0;
    MockCall* current = mock_registry.head;
    while (current) {
        if (strcmp(current->function_name, function_name) == 0) {
            actual_count++;
        }
        current = current->next;
    }

    unittest_assert(actual_count == expected_count,
        "Expected '%s' to be called %d times but was called %d times",
        function_name, expected_count, actual_count);
}

void unittest_register_stub(const char* function_name, const char* arg, void* return_value) {
    if (stub_count >= MAX_STUBS) {
        fprintf(stderr, "Error: Stub registry is full.\n");
        exit(1);
    }
    
    stub_registry[stub_count].function_name = function_name;
    stub_registry[stub_count].arg = arg;
    stub_registry[stub_count].return_value = return_value;
    stub_count++;
}

void* unittest_get_stub_return_value(const char* function_name, const char* arg) {
    for (int i = 0; i < stub_count; i++) {
        if (strcmp(stub_registry[i].function_name, function_name) == 0 &&
            strcmp(stub_registry[i].arg, arg) == 0) {
            return stub_registry[i].return_value;
        }
    }
    return NULL;
}

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
    return NULL; // Not found
}

/* Random Integer Generator */
int unittest_generate_random_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

/* Random Float Generator */
float unittest_generate_random_float(float min, float max) {
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

/* Random Double Generator */
double unittest_generate_random_double(double min, double max) {
    return min + ((double)rand() / RAND_MAX) * (max - min);
}

/* Random String Generator */
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

/* Edge Case Integer Generator */
int* unittest_generate_edge_case_integers(size_t* size) {
    static int edge_cases[] = {INT_MIN, -1, 0, 1, INT_MAX};
    *size = sizeof(edge_cases) / sizeof(edge_cases[0]);

    return edge_cases;
}

float* unittest_generate_edge_case_floats(size_t* size) {
    static float edge_cases[] = {-FLT_MAX, -1.0f, 0.0f, 1.0f, FLT_MAX};
    *size = sizeof(edge_cases) / sizeof(edge_cases[0]);

    return edge_cases;
}

double* unittest_generate_edge_case_doubles(size_t* size) {
    static double edge_cases[] = {-DBL_MAX, -1.0f, 0.0f, 1.0f, DBL_MAX};
    *size = sizeof(edge_cases) / sizeof(edge_cases[0]);

    return edge_cases;
}

/* Edge Case String Generator */
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

/* Free Generated Data */
void unittest_free_generated_data(void* data) {
    if (data) {
        free(data);
    }
}

void unittest_display_progress(int completed, int total) {
    int bar_width = 50;  // Width of the progress bar
    float progress = (float)completed / total;
    int pos = (int)(bar_width * progress);
    printf("\rProgress: [");
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) 
            printf("=");
        else if (i == pos) 
            printf(">");
        else 
            printf(" ");
    }
    printf("] %d/%d (%.2f%%)", completed, total, progress * 100);
    fflush(stdout);
}
