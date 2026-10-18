/**
 * @class Concurrent
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in concurrent.c (file-level convention).
 *
 * Cross-platform threading primitives (Win32 + POSIX) — threads,
 * mutexes, condition variables, thread-local storage, one-time init.
 *
 * This file is an ALTERED version of TinyCThread (modified and extended for
 * c_std; it is not the original software). TinyCThread is distributed under
 * the zlib/libpng license, whose notice follows:
 *
 *   Copyright (c) 2012 Marcus Geelnard
 *   Copyright (c) 2013-2016 Evan Nemerson
 *
 *   This software is provided 'as-is', without any express or implied
 *   warranty. In no event will the authors be held liable for any damages
 *   arising from the use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose,
 *   including commercial applications, and to alter it and redistribute it
 *   freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not
 *      claim that you wrote the original software. If you use this software
 *      in a product, an acknowledgment in the product documentation would be
 *      appreciated but is not required.
 *
 *   2. Altered source versions must be plainly marked as such, and must not be
 *      misrepresented as being the original software.
 *
 *   3. This notice may not be removed or altered from any source
 *      distribution.
 */

#ifndef CONCURRENT_H_
#define CONCURRENT_H_


#if !defined(_WIN32) && !defined(_WIN64) && (defined(__unix__) || defined(__APPLE__))
  #if !defined(_GNU_SOURCE)
    #define _GNU_SOURCE
  #endif
  #if !defined(_POSIX_C_SOURCE) || ((_POSIX_C_SOURCE - 0) < 200809L)
    #undef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200809L
  #endif
  #if !defined(_XOPEN_SOURCE) || ((_XOPEN_SOURCE - 0) < 700)
    #undef _XOPEN_SOURCE
    #define _XOPEN_SOURCE 700
  #endif
#endif

#include <stdio.h>
#include <time.h>


/* #define CONCURRENT_LOGGING_ENABLE */

#ifdef CONCURRENT_LOGGING_ENABLE
    #define CONCURRENT_LOG(...) \
        do { fprintf(stderr, "[CONCURRENT LOG] "); fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } while (0)
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

/* timespec_get fallback for platforms that don't ship it (MinGW's msvcrt).
   struct timespec itself is only replaced where the C library has none (old
   MSVC): renaming it unconditionally broke user code passing a struct
   timespec to nanosleep, clock_gettime or pthread_mutex_timedlock. */
#ifndef TIME_UTC
  #define TIME_UTC 1
  #define _TTHREAD_EMULATE_TIMESPEC_GET_
  #if defined(_TTHREAD_WIN32_) && !defined(_TIMESPEC_DEFINED) && defined(_MSC_VER) && _MSC_VER < 1900
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
  /* Built on a counting semaphore: every signal leaves one permit, so two
     signals can never collapse into one the way they could on an auto-reset
     event (a lost wakeup). Treat the fields as opaque. */
  typedef struct {
    HANDLE           mSemaphore;           /* one permit per pending wakeup       */
    unsigned int     mWaitersCount;        /* waiters not yet given a permit      */
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


/* Counting semaphore. macOS has no unnamed POSIX semaphores (sem_init fails
   with ENOSYS), so there - or when CSTD_SEMAPHORE_CONDVAR is defined - it is
   built from a mutex and a condition variable. */
#if defined(__APPLE__) && !defined(CSTD_SEMAPHORE_CONDVAR)
  #define CSTD_SEMAPHORE_CONDVAR
#endif
#if defined(_TTHREAD_WIN32_)
  typedef HANDLE Semaphore;
#elif defined(CSTD_SEMAPHORE_CONDVAR)
  typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    unsigned int    count;
  } Semaphore;
#else
  typedef sem_t Semaphore;
#endif


/* Read-write lock (writer-preference): many concurrent readers OR a single
 * exclusive writer. Built on the library's own Mutex + ThreadCondition so it
 * behaves identically on Win32 and POSIX. Treat the fields as opaque — use
 * the rwlock_* functions, never touch them directly. */
typedef struct {
  Mutex           lock;            /* protects the counters below              */
  ThreadCondition readers_ok;      /* signaled when readers may proceed        */
  ThreadCondition writers_ok;      /* signaled when a writer may proceed       */
  int             active_readers;  /* readers currently holding the lock       */
  int             active_writers;  /* 0 or 1 — a writer currently holding it   */
  int             waiting_writers; /* writers blocked in rwlock_wrlock         */
} RWLock;


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

#ifndef TSS_DTOR_ITERATIONS   /* also a C11 <threads.h> name */
#if defined(_TTHREAD_WIN32_)
  #define TSS_DTOR_ITERATIONS (4)
#else
  #define TSS_DTOR_ITERATIONS PTHREAD_DESTRUCTOR_ITERATIONS
#endif
#endif

/* One-time initialisation. C11 <threads.h> uses the same names (call_once,
   ONCE_FLAG_INIT): the prefixed forms concurrent_call_once / CSTD_ONCE_FLAG_INIT
   never clash. The short names are also provided unless <threads.h> was
   included first (ONCE_FLAG_INIT already defined) or CSTD_NO_C11_NAMES is
   defined - define it when <threads.h> is included AFTER this header. */
#if defined(_TTHREAD_WIN32_)
  typedef struct {
    LONG volatile    status;
    CRITICAL_SECTION lock;
  } OnceFlag;
  #define CSTD_ONCE_FLAG_INIT {0,}
#else
  typedef pthread_once_t OnceFlag;
  #define CSTD_ONCE_FLAG_INIT PTHREAD_ONCE_INIT
#endif


/* Link-time names. These functions keep their documented names in C code,
 * but the library exports them with a "cstd_" prefix: generic global symbols
 * such as mutex_lock, thread_create or call_once (also a C11 <threads.h>
 * name) clashed at link time with any other library or program defining
 * the same name ("multiple definition of ..."). */
#define mutex_init                  cstd_mutex_init
#define mutex_lock                  cstd_mutex_lock
#define mutex_timed_lock            cstd_mutex_timed_lock
#define mutex_trylock               cstd_mutex_trylock
#define mutex_unlock                cstd_mutex_unlock
#define mutex_destroy               cstd_mutex_destroy
#define semaphore_init              cstd_semaphore_init
#define semaphore_wait              cstd_semaphore_wait
#define semaphore_trywait           cstd_semaphore_trywait
#define semaphore_post              cstd_semaphore_post
#define semaphore_destroy           cstd_semaphore_destroy
#define rwlock_init                 cstd_rwlock_init
#define rwlock_rdlock               cstd_rwlock_rdlock
#define rwlock_tryrdlock            cstd_rwlock_tryrdlock
#define rwlock_wrlock               cstd_rwlock_wrlock
#define rwlock_trywrlock            cstd_rwlock_trywrlock
#define rwlock_unlock               cstd_rwlock_unlock
#define rwlock_destroy              cstd_rwlock_destroy
#define condition_init              cstd_condition_init
#define condition_signal            cstd_condition_signal
#define condition_broadcast         cstd_condition_broadcast
#define condition_wait              cstd_condition_wait
#define condition_timedwait         cstd_condition_timedwait
#define condition_destroy           cstd_condition_destroy
#define thread_create               cstd_thread_create
#define thread_join                 cstd_thread_join
#define thread_detach               cstd_thread_detach
#define thread_equal                cstd_thread_equal
#define thread_sleep                cstd_thread_sleep
#define thread_exit                 cstd_thread_exit
#define thread_yield                cstd_thread_yield
#define thread_current              cstd_thread_current
#define thread_hardware_concurrency cstd_thread_hardware_concurrency
#define thread_specific_create      cstd_thread_specific_create
#define thread_specific_set         cstd_thread_specific_set
#define thread_specific_get         cstd_thread_specific_get
#define thread_specific_delete      cstd_thread_specific_delete

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
/* Read-write lock (shared readers / exclusive writer)                */
/* ------------------------------------------------------------------ */

int                rwlock_init                    (RWLock* rw);
int                rwlock_rdlock                  (RWLock* rw);
int                rwlock_tryrdlock               (RWLock* rw);
int                rwlock_wrlock                  (RWLock* rw);
int                rwlock_trywrlock               (RWLock* rw);
int                rwlock_unlock                  (RWLock* rw);
void               rwlock_destroy                 (RWLock* rw);


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
void               cstd_call_once                 (OnceFlag* flag, void (*func)(void));
  #define          concurrent_call_once(flag,func) cstd_call_once(flag,func)
#else
  #define          concurrent_call_once(flag,func) pthread_once(flag,func)
#endif

#if !defined(CSTD_NO_C11_NAMES) && !defined(ONCE_FLAG_INIT)
  #define          ONCE_FLAG_INIT                 CSTD_ONCE_FLAG_INIT
  #define          call_once(flag,func)           concurrent_call_once(flag,func)
#endif


#endif 
