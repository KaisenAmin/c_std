#include "unittest.h"
#include <time.h>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__MACH__) && defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_time.h>
#elif defined(_POSIX_VERSION)
#include <sys/time.h>
#endif

 
static MemoryRecord* memory_head = NULL;
size_t total_memory_allocated = 0;

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