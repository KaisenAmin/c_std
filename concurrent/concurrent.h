#ifndef _CONCURRENT_H_
#define _CONCURRENT_H_

#include <time.h>


/* Which platform are we on? */
#if !defined(_TTHREAD_PLATFORM_DEFINED_)
  #if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
    #define _TTHREAD_WIN32_
  #else
    #define _TTHREAD_POSIX_
  #endif
  #define _TTHREAD_PLATFORM_DEFINED_
#endif

/* Activate some POSIX functionality (e.g. clock_gettime and recursive mutexes) */
#if defined(_TTHREAD_POSIX_)
  #undef _FEATURES_H
  #if !defined(_GNU_SOURCE)
    #define _GNU_SOURCE
  #endif
  #if !defined(_POSIX_C_SOURCE) || ((_POSIX_C_SOURCE - 0) < 199309L)
    #undef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 199309L
  #endif
  #if !defined(_XOPEN_SOURCE) || ((_XOPEN_SOURCE - 0) < 500)
    #undef _XOPEN_SOURCE
    #define _XOPEN_SOURCE 500
  #endif
  #define _XPG6
#endif


// Platform specific includes 
#if defined(_TTHREAD_POSIX_)
  #include <pthread.h>
#elif defined(_TTHREAD_WIN32_)
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #define __UNDEF_LEAN_AND_MEAN
  #endif
  #include <windows.h>
  #ifdef __UNDEF_LEAN_AND_MEAN
    #undef WIN32_LEAN_AND_MEAN
    #undef __UNDEF_LEAN_AND_MEAN
  #endif
#endif

// Compiler-specific information 
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
  #define TTHREAD_NORETURN _Noreturn
#elif defined(__GNUC__)
  #define TTHREAD_NORETURN __attribute__((__noreturn__))
#else
  #define TTHREAD_NORETURN
#endif

/* If TIME_UTC is missing, provide it and provide a wrapper for
   timespec_get. */
#ifndef TIME_UTC
  #define TIME_UTC 1
  #define _TTHREAD_EMULATE_TIMESPEC_GET_
  #if defined(_TTHREAD_WIN32_)
    struct _tthread_timespec {
      time_t tv_sec;
      long   tv_nsec;
    };
    #define timespec _tthread_timespec
  #endif
  int _tthread_timespec_get(struct timespec *ts, int base);
  #define timespec_get _tthread_timespec_get
#endif

// CThread version (major number).
#define CTHREAD_VERSION_MAJOR 1
// CThread version (minor number).
#define CTHREAD_VERSION_MINOR 2
// CThread version (full version). 
#define CTHREAD_VERSION (CTHREAD_VERSION_MAJOR * 100 + CTHREAD_VERSION_MINOR)

/**
 * Defines the _Thread_local storage class specifier for thread-local storage (TLS).
 *
 * The _Thread_local keyword allows variables to be declared in such a way that each thread
 * has its own separate instance of the variable. This is useful for situations where
 * variables need to be unique to each thread, such as for thread-specific error numbers
 * or other thread-local data.
 *
 * Usage example:
 * @code
 * _Thread_local int threadSpecificVariable;
 * @endcode
 *
 * This macro ensures compatibility across different compilers and standards:
 * - For C11 and newer (__STDC_VERSION__ >= 201102L), _Thread_local is directly supported.
 * - For compilers like GCC, Intel, Sun Pro, and IBM before C11 or when the C11 standard
 *   is not explicitly used, it falls back to __thread.
 * - For other cases, it uses __declspec(thread) for compatibility with compilers
 *   like MSVC that do not support __thread but use their own syntax for thread-local storage.
 *
 * @note Compatibility Notice:
 * The use of _Thread_local is not supported on Mac OS X when targeting older versions
 * that lack runtime support for thread-local storage using this keyword. Additionally,
 * some older compilers or versions (e.g., GCC before 4.9) might not fully support
 * this feature or may have limitations.
 *
 * @note The macro and its alternatives (__thread, __declspec(thread)) allow for
 * portable thread-local storage across various compilers and C standards, making
 * it easier to write cross-platform multithreaded applications.
 */
#if !(defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201102L)) && !defined(_Thread_local)
 #if defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__SUNPRO_CC) || defined(__IBMCPP__)
  #define _Thread_local __thread
 #else
  #define _Thread_local __declspec(thread)
 #endif
#elif defined(__GNUC__) && defined(__GNUC_MINOR__) && (((__GNUC__ << 8) | __GNUC_MINOR__) < ((4 << 8) | 9))
 #define _Thread_local __thread
#endif

/* Macros */
#if defined(_TTHREAD_WIN32_)
  #define TSS_DTOR_ITERATIONS (4)
#else
  #define TSS_DTOR_ITERATIONS PTHREAD_DESTRUCTOR_ITERATIONS
#endif

typedef enum {
    THREAD_ERROR = 0,   // The requested operation failed 
    THREAD_SUCCESS,     // The requested operation succeeded 
    THREAD_TIMEOUT,     // The time specified in the call was reached without acquiring the requested resource 
    THREAD_BUSY,        // The requested operation failed because a resource requested by a test and return function is already in use 
    THREAD_NOMEM        // The requested operation failed because it was unable to allocate memory 
} ThreadResult;

typedef enum {
    MUTEX_PLAIN = 0,     // A non-recursive mutex
    MUTEX_TIMED,         // A mutex that supports timed lock 
    MUTEX_RECURSIVE      // A recursive mutex 
} MutexType;

/* Mutex */
#if defined(_TTHREAD_WIN32_)
  typedef struct {
    union {
      CRITICAL_SECTION cs;      // Critical section handle (used for non-timed mutexes)
      HANDLE mut;               // Mutex handle (used for timed mutex)
    } mHandle;                  // Mutex handle 
    int mAlreadyLocked;         // true if the mutex is already locked 
    int mRecursive;             // true if the mutex is recursive 
    int mTimed;                 // true if the mutex is timed 
  } Mutex;
#else
  typedef pthread_mutex_t Mutex;
#endif

int mutex_init(Mutex *mutex, int type);
int mutex_lock(Mutex *mutex);
int MUTEX_TIMEDlock(Mutex *mutex, const struct timespec *ts);
int mutex_unlock(Mutex *mutex);
int mutex_trylock(Mutex *mutex);
void mutex_destroy(Mutex *mutex);


/* Condition variable */
#if defined(_TTHREAD_WIN32_)
  typedef struct {
    HANDLE mEvents[2];                  // Signal and broadcast event HANDLEs. 
    unsigned int mWaitersCount;         // Count of the number of waiters. 
    CRITICAL_SECTION mWaitersCountLock; // Serialize access to mWaitersCount. 
  } ThreadCondition;
#else
  typedef pthread_cond_t ThreadCondition;
#endif

int condition_init(ThreadCondition *cond);
int condition_signal(ThreadCondition *cond);
int condition_broadcast(ThreadCondition *cond);
int condition_wait(ThreadCondition *cond, Mutex *mutex);
int condition_timedwait(ThreadCondition *cond, Mutex *mutex, const struct timespec *ts);
void condition_destroy(ThreadCondition *cond);

/* Thread */
#if defined(_TTHREAD_WIN32_)
  typedef HANDLE Thread;
#else
  typedef pthread_t Thread;
#endif

typedef int (*ThreadStart)(void *arg);

int thread_create(Thread *thr, ThreadStart func, void *arg);
int thread_detach(Thread thr);
int thread_equal(Thread thr0, Thread thr1);
int thread_join(Thread thr, int *res);
int thread_sleep(const struct timespec *duration, struct timespec *remaining);
unsigned long thread_current(void);
unsigned long thread_hardware_concurrency(void);

TTHREAD_NORETURN void thread_exit(int res);
void thread_yield(void);

/* Thread local storage */
#if defined(_TTHREAD_WIN32_)
  typedef DWORD ThreadSpecific;
#else
  typedef pthread_key_t ThreadSpecific;
#endif

// Destructor function for a thread-specific storage.
typedef void (*ThreadSpecificDestructor)(void *val);

int thread_specific_create(ThreadSpecific *key, ThreadSpecificDestructor dtor);
int thread_specific_set(ThreadSpecific key, void *val);
void thread_specific_delete(ThreadSpecific key);
void *thread_specific_get(ThreadSpecific key);

/*
* This code defines a cross-platform OnceFlag type and an initializer ONCE_FLAG_INIT used for one-time initialization. 
* On Windows, OnceFlag is a struct containing a status variable and a critical section object for thread synchronization. 
* On POSIX systems, it aliases pthread_once_t and uses PTHREAD_ONCE_INIT for initialization. 
* This abstraction allows for consistent one-time initialization logic across different operating systems by providing a 
* unified interface.
*/
#if defined(_TTHREAD_WIN32_)
  typedef struct {
    LONG volatile status;
    CRITICAL_SECTION lock;
  } OnceFlag;
  #define ONCE_FLAG_INIT {0,}
#else
  #define OnceFlag pthread_once_t
  #define ONCE_FLAG_INIT PTHREAD_ONCE_INIT
#endif

/*
* This code provides a cross-platform call_once function or macro that ensures a function is called exactly once across all 
* threads. On Windows, it declares a function prototype for call_once, requiring a custom implementation. 
* For POSIX systems, it defines call_once as a macro that wraps pthread_once, 
* leveraging the native one-time initialization provided by pthreads. This approach allows for consistent, 
* thread-safe initialization logic in both Windows and POSIX environments.
*/
#if defined(_TTHREAD_WIN32_)
  void call_once(OnceFlag *flag, void (*func)(void));
#else
  #define call_once(flag,func) pthread_once(flag,func)
#endif



#endif /* _CTHREAD_H_ */