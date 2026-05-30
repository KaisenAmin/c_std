/**
 * @class Concurrent
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in concurrent.c (file-level convention).
 *
 * Cross-platform threading primitives (Win32 + POSIX) — threads,
 * mutexes, condition variables, thread-local storage, one-time init.
 * Based on the public-domain tinycthread design.
 */

#ifndef _CONCURRENT_H_
#define _CONCURRENT_H_

#include <stdio.h>
#include <time.h>


/* #define CONCURRENT_LOGGING_ENABLE */

#ifdef CONCURRENT_LOGGING_ENABLE
    #define CONCURRENT_LOG(fmt, ...) \
        do { fprintf(stderr, "[CONCURRENT LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define CONCURRENT_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Platform detection / includes                                      */
/* ------------------------------------------------------------------ */

#if defined(_WIN32) || defined(_WIN64)
  #ifndef _TTHREAD_WIN32_
    #define _TTHREAD_WIN32_
  #endif
  #include <process.h>
  #include <sys/timeb.h>
#elif defined(__unix__) || defined(__APPLE__)
  #ifndef _TTHREAD_POSIX_
    #define _TTHREAD_POSIX_
  #endif
  #include <signal.h>
  #include <sched.h>
  #include <unistd.h>
  #include <sys/time.h>
  #include <errno.h>
#endif

#ifndef NULL
  #define NULL (void*)0
#endif

#if defined(_TTHREAD_WIN32_)
  #define _CONDITION_EVENT_ONE 0
  #define _CONDITION_EVENT_ALL 1
#endif

/* Activate POSIX functionality (clock_gettime, recursive mutexes). */
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

#if defined(_TTHREAD_POSIX_)
  #include <pthread.h>
  #include <semaphore.h>
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


/* ------------------------------------------------------------------ */
/* Compiler / language feature shims                                  */
/* ------------------------------------------------------------------ */

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
  #define TTHREAD_NORETURN _Noreturn
#elif defined(__GNUC__)
  #define TTHREAD_NORETURN __attribute__((__noreturn__))
#else
  #define TTHREAD_NORETURN
#endif

/* timespec_get fallback for platforms that don't ship it. */
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

/* _Thread_local fallback for compilers that don't expose it yet. */
#if !(defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201102L)) && !defined(_Thread_local)
 #if defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__SUNPRO_CC) || defined(__IBMCPP__)
  #define _Thread_local __thread
 #else
  #define _Thread_local __declspec(thread)
 #endif
#elif defined(__GNUC__) && defined(__GNUC_MINOR__) && (((__GNUC__ << 8) | __GNUC_MINOR__) < ((4 << 8) | 9))
 #define _Thread_local __thread
#endif


/* ------------------------------------------------------------------ */
/* Library version                                                    */
/* ------------------------------------------------------------------ */

#define CTHREAD_VERSION_MAJOR 1
#define CTHREAD_VERSION_MINOR 2
#define CTHREAD_VERSION       (CTHREAD_VERSION_MAJOR * 100 + CTHREAD_VERSION_MINOR)



/* Function return codes. */
typedef enum {
    THREAD_ERROR = 0,   /* operation failed                                 */
    THREAD_SUCCESS,     /* operation succeeded                              */
    THREAD_TIMEOUT,     /* time bound elapsed without acquiring resource    */
    THREAD_BUSY,        /* resource was already in use                      */
    THREAD_NOMEM        /* allocation failed                                */
} ThreadResult;


/* Mutex flavour for `mutex_init`. */
typedef enum {
    MUTEX_PLAIN = 0,    /* non-recursive                                    */
    MUTEX_TIMED,        /* supports timed lock                              */
    MUTEX_RECURSIVE     /* recursive                                        */
} MutexType;


/* Mutex handle. */
#if defined(_TTHREAD_WIN32_)
  typedef struct {
    union {
      CRITICAL_SECTION cs;    /* used for non-timed mutexes */
      HANDLE           mut;   /* used for timed mutex       */
    } mHandle;
    int mAlreadyLocked;
    int mRecursive;
    int mTimed;
  } Mutex;
#else
  typedef pthread_mutex_t Mutex;
#endif


/* Condition variable. */
#if defined(_TTHREAD_WIN32_)
  typedef struct {
    HANDLE           mEvents[2];           /* signal + broadcast events           */
    unsigned int     mWaitersCount;        /* number of waiters                   */
    CRITICAL_SECTION mWaitersCountLock;    /* protects mWaitersCount              */
  } ThreadCondition;
#else
  typedef pthread_cond_t ThreadCondition;
#endif


/* Thread handle. */
#if defined(_TTHREAD_WIN32_)
  typedef HANDLE Thread;
#else
  typedef pthread_t Thread;
#endif


/* Counting semaphore. */
#if defined(_TTHREAD_WIN32_)
  typedef HANDLE Semaphore;
#else
  typedef sem_t Semaphore;
#endif


/* Entry point passed to `thread_create`. */
typedef int (*ThreadStart)(void* arg);

/* Thread-specific storage. */
#if defined(_TTHREAD_WIN32_)
  typedef DWORD ThreadSpecific;
#else
  typedef pthread_key_t ThreadSpecific;
#endif

/* TLS slot destructor. */
typedef void (*ThreadSpecificDestructor)(void* val);

#if defined(_TTHREAD_WIN32_)
  #define TSS_DTOR_ITERATIONS (4)
#else
  #define TSS_DTOR_ITERATIONS PTHREAD_DESTRUCTOR_ITERATIONS
#endif

/* One-time initialisation. */
#if defined(_TTHREAD_WIN32_)
  typedef struct {
    LONG volatile    status;
    CRITICAL_SECTION lock;
  } OnceFlag;
  #define ONCE_FLAG_INIT {0,}
#else
  #define OnceFlag pthread_once_t
  #define ONCE_FLAG_INIT PTHREAD_ONCE_INIT
#endif


/* ------------------------------------------------------------------ */
/* Mutex                                                              */
/* ------------------------------------------------------------------ */

int                mutex_init                     (Mutex* mutex, int type);
int                mutex_lock                     (Mutex* mutex);
int                mutex_timed_lock                (Mutex* mutex, const struct timespec* ts);
int                mutex_trylock                  (Mutex* mutex);
int                mutex_unlock                   (Mutex* mutex);
void               mutex_destroy                  (Mutex* mutex);


/* ------------------------------------------------------------------ */
/* Semaphore (counting)                                               */
/* ------------------------------------------------------------------ */

int                semaphore_init                 (Semaphore* sem, unsigned int initial_count);
int                semaphore_wait                 (Semaphore* sem);
int                semaphore_trywait              (Semaphore* sem);
int                semaphore_post                 (Semaphore* sem);
void               semaphore_destroy              (Semaphore* sem);


/* ------------------------------------------------------------------ */
/* Condition variable                                                 */
/* ------------------------------------------------------------------ */

int                condition_init                 (ThreadCondition* cond);
int                condition_signal               (ThreadCondition* cond);
int                condition_broadcast            (ThreadCondition* cond);
int                condition_wait                 (ThreadCondition* cond, Mutex* mutex);
int                condition_timedwait            (ThreadCondition* cond, Mutex* mutex, const struct timespec* ts);
void               condition_destroy              (ThreadCondition* cond);


/* ------------------------------------------------------------------ */
/* Thread                                                             */
/* ------------------------------------------------------------------ */

int                thread_create                  (Thread* thr, ThreadStart func, void* arg);
int                thread_join                    (Thread thr, int* res);
int                thread_detach                  (Thread thr);
int                thread_equal                   (Thread thr0, Thread thr1);
int                thread_sleep                   (const struct timespec* duration, struct timespec* remaining);
TTHREAD_NORETURN
void               thread_exit                    (int res);
void               thread_yield                   (void);
unsigned long      thread_current                 (void);
unsigned long      thread_hardware_concurrency    (void);


/* ------------------------------------------------------------------ */
/* Thread-specific storage                                            */
/* ------------------------------------------------------------------ */

int                thread_specific_create         (ThreadSpecific* key, ThreadSpecificDestructor dtor);
int                thread_specific_set            (ThreadSpecific key, void* val);
void*              thread_specific_get            (ThreadSpecific key);
void               thread_specific_delete         (ThreadSpecific key);


/* ------------------------------------------------------------------ */
/* One-time initialization                                            */
/* ------------------------------------------------------------------ */

#if defined(_TTHREAD_WIN32_)
void               call_once                      (OnceFlag* flag, void (*func)(void));
#else
  #define          call_once(flag,func)           pthread_once(flag,func)
#endif


#endif 
