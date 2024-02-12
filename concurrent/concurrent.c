#include "concurrent.h"
#include <stdlib.h>
#include <stdbool.h>

/* Platform specific includes */
#if defined(_TTHREAD_POSIX_)
  #include <signal.h>
  #include <sched.h>
  #include <unistd.h>
  #include <sys/time.h>
  #include <errno.h>
#elif defined(_TTHREAD_WIN32_)
  #include <process.h>
  #include <sys/timeb.h>
#endif

/* Standard, good-to-have defines */
#ifndef NULL
  #define NULL (void*)0
#endif

#if defined(_TTHREAD_WIN32_)
  #define _CONDITION_EVENT_ONE 0
  #define _CONDITION_EVENT_ALL 1
#endif


/**
 * Initializes a mutex.
 *
 * This function initializes a mutex with the specified type. The mutex can be
 * either a plain mutex, a timed mutex, or a recursive mutex. For timed mutexes
 * on Windows, a Windows mutex object is created. For non-timed mutexes on Windows,
 * a critical section is used. On POSIX systems, pthread_mutexattr_settype is
 * used to set the mutex type.
 *
 * @param mtx Pointer to the mutex to initialize. Must not be NULL.
 * @param type The type of mutex to initialize. This is a bit-mask that can be
 *        one of the following:
 *        - MUTEX_PLAIN for a non-recursive mutex.
 *        - MUTEX_TIMED for a mutex that supports timeout (Windows only).
 *        - MUTEX_RECURSIVE for a recursive mutex.
 *        - Combination of MUTEX_TIMED and MUTEX_RECURSIVE for a timed, recursive mutex.
 *
 * @return THREAD_SUCCESS if the mutex was successfully initialized,
 *         THREAD_ERROR if there was an error initializing the mutex.
 *         On Windows, if a timed mutex cannot be created, THREAD_ERROR is returned.
 *         On POSIX systems, if pthread_mutex_init fails, THREAD_ERROR is returned.
 *
 * @note On POSIX systems, the mutex type (e.g., PTHREAD_MUTEX_RECURSIVE) is set
 *       using pthread_mutexattr_settype if the type includes MUTEX_RECURSIVE.
 *       The function does not support timed mutexes on POSIX systems directly;
 *       the behavior is emulated in other functions if needed.
 * @note On Windows, for timed mutexes, CreateMutex is used. For non-timed mutexes,
 *       InitializeCriticalSection is used.
 */
int mutex_init(Mutex *mtx, int type) {
  #if defined(_TTHREAD_WIN32_)
    mtx->mAlreadyLocked = false;
    mtx->mRecursive = type & MUTEX_RECURSIVE;
    mtx->mTimed = type & MUTEX_TIMED;
    if (!mtx->mTimed) {
      InitializeCriticalSection(&(mtx->mHandle.cs));
    }
    else {
      mtx->mHandle.mut = CreateMutex(NULL, false, NULL);
      if (mtx->mHandle.mut == NULL) {
        return THREAD_ERROR;
      }
    }
    return THREAD_SUCCESS;
  #else
    int ret;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    if (type & mtx_recursive) {
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    }
    ret = pthread_mutex_init(mtx, &attr);
    pthread_mutexattr_destroy(&attr);

    return ret == 0 ? THREAD_SUCCESS : THREAD_ERROR;
  #endif
}

/**
 * Destroys a mutex.
 *
 * This function releases any resources associated with the mutex. After calling
 * this function, the mutex must not be used unless it is re-initialized using
 * mutex_init(). On Windows, if the mutex is a critical section (non-timed mutex),
 * DeleteCriticalSection is called. If it is a timed mutex, CloseHandle is used
 * to release the mutex handle. On POSIX systems, pthread_mutex_destroy is used
 * to destroy the mutex.
 *
 * @param mtx Pointer to the mutex to be destroyed. Must not be NULL.
 *
 * @note On Windows, the function distinguishes between timed and non-timed
 *       mutexes. Non-timed mutexes are implemented using critical sections,
 *       while timed mutexes are implemented using Windows mutex objects.
 * @note On POSIX systems, there is no distinction between timed and non-timed
 *       mutexes in this context; pthread_mutex_destroy is used for all mutexes.
 * @note It is undefined behavior to destroy a mutex that is locked or referenced
 *       by another thread. Ensure the mutex is unlocked and not in use before
 *       destroying it.
 */
void mutex_destroy(Mutex *mtx) {
  #if defined(_TTHREAD_WIN32_)
    if (!mtx->mTimed) {
      DeleteCriticalSection(&(mtx->mHandle.cs));
    }
    else {
      CloseHandle(mtx->mHandle.mut);
    }
  #else
    pthread_mutex_destroy(mtx);
  #endif
}

/**
 * Locks a mutex.
 *
 * This function attempts to lock a mutex. If the mutex is already locked by
 * another thread, the calling thread will block until the mutex becomes
 * available. On Windows, if the mutex is not a timed mutex, EnterCriticalSection
 * is used to lock the mutex. If it is a timed mutex, WaitForSingleObject is used
 * to wait indefinitely for the mutex. On POSIX systems, pthread_mutex_lock is
 * used to lock the mutex.
 *
 * @param mtx Pointer to the mutex to lock. Must not be NULL.
 *
 * @return THREAD_SUCCESS if the mutex was successfully locked,
 *         THREAD_ERROR if there was an error locking the mutex.
 *         On Windows, if WaitForSingleObject fails or returns WAIT_ABANDONED,
 *         THREAD_ERROR is returned. On POSIX systems, if pthread_mutex_lock
 *         fails, THREAD_ERROR is returned.
 *
 * @note On Windows, for non-recursive, non-timed mutexes, additional logic is
 *       implemented to simulate a deadlock if the mutex is already locked by
 *       the calling thread. This behavior mimics the deadlock behavior of
 *       attempting to lock a non-recursive mutex on POSIX systems.
 * @note This function will block indefinitely if the mutex cannot be immediately
 *       locked, except for Windows timed mutexes, where WaitForSingleObject
 *       handles the waiting.
 * @note It is undefined behavior to lock a mutex that has not been initialized
 *       using mutex_init().
 */
int mutex_lock(Mutex *mtx) {
  #if defined(_TTHREAD_WIN32_)
    if (!mtx->mTimed) {
      EnterCriticalSection(&(mtx->mHandle.cs));
    }
    else {
      switch (WaitForSingleObject(mtx->mHandle.mut, INFINITE)) {
        case WAIT_OBJECT_0:
          break;
        case WAIT_ABANDONED:
        default:
          return THREAD_ERROR;
      }
    }

    if (!mtx->mRecursive) {
      while(mtx->mAlreadyLocked) {
        Sleep(1); /* Simulate deadlock... */
      }
      mtx->mAlreadyLocked = true;
    }
    return THREAD_SUCCESS;
  #else
    return pthread_mutex_lock(mtx) == 0 ? THREAD_SUCCESS : THREAD_ERROR;
  #endif
}

/**
 * Locks a mutex, with a timeout.
 *
 * Attempts to lock a mutex within a specified timeout period. If the mutex
 * is already locked by another thread, the calling thread will block until
 * the mutex becomes available or the timeout expires. This function is
 * intended for use with timed mutexes.
 *
 * @param mtx Pointer to the mutex to be locked. Must not be NULL.
 * @param ts Pointer to a timespec structure that specifies the absolute timeout
 *        time. The timespec structure should contain the time in seconds and
 *        nanoseconds since the Epoch (1970-01-01 00:00:00 +0000, UTC).
 *
 * @return THREAD_SUCCESS if the mutex was successfully locked,
 *         THREAD_TIMEOUT if the mutex could not be locked before the specified
 *         timeout expired,
 *         THREAD_ERROR if there was an error locking the mutex or if the
 *         function is used with a non-timed mutex.
 *
 * @note On Windows, this function converts the absolute timeout time to a
 *       relative time in milliseconds and uses WaitForSingleObject. It returns
 *       THREAD_ERROR if called on a non-timed mutex or if WaitForSingleObject
 *       fails. It does not account for time elapsed during system sleep.
 * @note On POSIX systems that support pthread_MUTEX_TIMEDlock (determined by
 *       _POSIX_TIMEOUTS and _POSIX_THREADS), this function directly maps to
 *       pthread_MUTEX_TIMEDlock. On systems that do not support it, the function
 *       emulates the behavior by repeatedly attempting to lock the mutex with
 *       pthread_mutex_trylock and sleeping for short intervals until the timeout
 *       expires.
 * @note The behavior of this function for recursive mutexes is platform-dependent.
 *       Specifically, on Windows, if the mutex is recursive and already locked by
 *       the calling thread, it may sleep indefinitely or return THREAD_ERROR,
 *       depending on the mutex's configuration.
 */
int MUTEX_TIMEDlock(Mutex *mtx, const struct timespec *ts) {
  #if defined(_TTHREAD_WIN32_)
    struct timespec current_ts;
    DWORD timeoutMs;

    if (!mtx->mTimed) {
      return THREAD_ERROR;
    }

    timespec_get(&current_ts, TIME_UTC);

    if ((current_ts.tv_sec > ts->tv_sec) || ((current_ts.tv_sec == ts->tv_sec) && (current_ts.tv_nsec >= ts->tv_nsec))) {
      timeoutMs = 0;
    }
    else {
      timeoutMs  = (DWORD)(ts->tv_sec  - current_ts.tv_sec)  * 1000;
      timeoutMs += (ts->tv_nsec - current_ts.tv_nsec) / 1000000;
      timeoutMs += 1;
    }

    /* TODO: the timeout for WaitForSingleObject doesn't include time
      while the computer is asleep. */
    switch (WaitForSingleObject(mtx->mHandle.mut, timeoutMs)) {
      case WAIT_OBJECT_0:
        break;
      case WAIT_TIMEOUT:
        return THREAD_TIMEOUT;
      case WAIT_ABANDONED:
      default:
        return THREAD_ERROR;
    }

    if (!mtx->mRecursive) {
      while(mtx->mAlreadyLocked) {
        Sleep(1); /* Simulate deadlock... */
      }
      mtx->mAlreadyLocked = true;
    }

    return THREAD_SUCCESS;
  #elif defined(_POSIX_TIMEOUTS) && (_POSIX_TIMEOUTS >= 200112L) && defined(_POSIX_THREADS) && (_POSIX_THREADS >= 200112L)
    switch (pthread_MUTEX_TIMEDlock(mtx, ts)) {
      case 0:
        return THREAD_SUCCESS;
      case ETIMEDOUT:
        return THREAD_TIMEOUT;
      default:
        return THREAD_ERROR;
    }
  #else
    int rc;
    struct timespec cur, dur;

    /* Try to acquire the lock and, if we fail, sleep for 5ms. */
    while ((rc = pthread_mutex_trylock (mtx)) == EBUSY) {
      timespec_get(&cur, TIME_UTC);

      if ((cur.tv_sec > ts->tv_sec) || ((cur.tv_sec == ts->tv_sec) && (cur.tv_nsec >= ts->tv_nsec))) {
        break;
      }

      dur.tv_sec = ts->tv_sec - cur.tv_sec;
      dur.tv_nsec = ts->tv_nsec - cur.tv_nsec;
      if (dur.tv_nsec < 0) {
        dur.tv_sec--;
        dur.tv_nsec += 1000000000;
      } 
      if ((dur.tv_sec != 0) || (dur.tv_nsec > 5000000)) {
        dur.tv_sec = 0;
        dur.tv_nsec = 5000000;
      }

      nanosleep(&dur, NULL);
    }

    switch (rc) {
      case 0:
        return THREAD_SUCCESS;
      case ETIMEDOUT:
      case EBUSY:
        return THREAD_TIMEOUT;
      default:
        return THREAD_ERROR;
    }
  #endif
}

/**
 * Attempts to lock a mutex without blocking.
 *
 * Tries to lock the specified mutex. If the mutex is already locked by another
 * thread, the function returns immediately with a THREAD_BUSY status, instead
 * of blocking the calling thread.
 *
 * @param mtx Pointer to the mutex to attempt to lock. Must not be NULL.
 *
 * @return THREAD_SUCCESS if the mutex was successfully locked without blocking,
 *         THREAD_BUSY if the mutex is already locked by another thread,
 *         indicating that the function did not lock the mutex.
 *
 * @note On Windows, for non-timed mutexes, TryEnterCriticalSection is used to
 *       attempt to enter the critical section without blocking. For timed
 *       mutexes, WaitForSingleObject with a zero timeout is used to check the
 *       mutex's state. If the mutex is recursive and already locked by the calling
 *       thread, it is considered busy to prevent recursive locking.
 * @note On POSIX systems, pthread_mutex_trylock is used to attempt to lock the
 *       mutex. If it returns 0, the mutex was successfully locked. Otherwise,
 *       if it is already locked by another thread, the function will return
 *       THREAD_BUSY.
 * @note This function can be used in situations where a thread needs to acquire
 *       a mutex but cannot block if the mutex is not available.
 */
int mutex_trylock(Mutex *mtx) {
  #if defined(_TTHREAD_WIN32_)
    int ret;

    if (!mtx->mTimed) {
      ret = TryEnterCriticalSection(&(mtx->mHandle.cs)) ? THREAD_SUCCESS : THREAD_BUSY;
    }
    else {
      ret = (WaitForSingleObject(mtx->mHandle.mut, 0) == WAIT_OBJECT_0) ? THREAD_SUCCESS : THREAD_BUSY;
    }

    if ((!mtx->mRecursive) && (ret == THREAD_SUCCESS)){
      if (mtx->mAlreadyLocked) {
        LeaveCriticalSection(&(mtx->mHandle.cs));
        ret = THREAD_BUSY;
      }
      else {
        mtx->mAlreadyLocked = true;
      }
    }
    return ret;
  #else
    return (pthread_mutex_trylock(mtx) == 0) ? THREAD_SUCCESS : THREAD_BUSY;
  #endif
}

/**
 * Unlocks a mutex.
 *
 * Releases a mutex that was previously locked by the calling thread. If the
 * mutex is a recursive mutex, it may need to be unlocked multiple times by
 * the thread that locked it to fully release it.
 *
 * @param mtx Pointer to the mutex to be unlocked. Must not be NULL.
 *
 * @return THREAD_SUCCESS if the mutex was successfully unlocked,
 *         THREAD_ERROR if there was an error unlocking the mutex.
 *
 * @note On Windows, if the mutex is not a timed mutex, LeaveCriticalSection
 *       is used to release the critical section. If it is a timed mutex,
 *       ReleaseMutex is called. If ReleaseMutex fails, THREAD_ERROR is returned.
 *       The mtx->mAlreadyLocked flag is used to track the lock status for
 *       non-recursive, non-timed mutexes, ensuring proper unlocking behavior.
 * @note On POSIX systems, pthread_mutex_unlock is used to unlock the mutex.
 *       If it succeeds, THREAD_SUCCESS is returned; otherwise, THREAD_ERROR
 *       is returned.
 * @note It is undefined behavior to unlock a mutex that was not locked by the
 *       calling thread or a mutex that is not locked at all. Such actions can
 *       lead to unpredictable behavior or runtime errors.
 */
int mutex_unlock(Mutex *mtx) {
  #if defined(_TTHREAD_WIN32_)
    mtx->mAlreadyLocked = false;
    if (!mtx->mTimed) {
      LeaveCriticalSection(&(mtx->mHandle.cs));
    }
    else {
      if (!ReleaseMutex(mtx->mHandle.mut)) {
        return THREAD_ERROR;
      }
    }
    return THREAD_SUCCESS;
  #else
    return pthread_mutex_unlock(mtx) == 0 ? THREAD_SUCCESS : THREAD_ERROR;;
  #endif
}

/**
 * Initializes a condition variable.
 *
 * Prepares a condition variable for use. On Windows, this involves initializing
 * a critical section for managing waiters count, and creating two events: one
 * for signaling individual waiters and another for broadcasting to all waiters.
 * On POSIX systems, it simply initializes a pthread condition variable.
 *
 * @param cond Pointer to the condition variable object to initialize. Must not be NULL.
 *
 * @return THREAD_SUCCESS if the condition variable was successfully initialized,
 *         THREAD_ERROR if there was an error initializing the condition variable.
 *
 * @note On Windows, two events are created for the condition variable:
 *       - One auto-reset event used for waking up a single waiter.
 *       - One manual-reset event used for waking up all waiters.
 *       If either event cannot be created, any resources allocated until that
 *       point are cleaned up, and THREAD_ERROR is returned.
 * @note On POSIX systems, pthread_cond_init is used with default attributes.
 *       If it fails, THREAD_ERROR is returned.
 * @note It is important to destroy initialized condition variables using
 *       condition_destroy to free any resources allocated during initialization.
 */
int condition_init(ThreadCondition *cond) {
  #if defined(_TTHREAD_WIN32_)
    cond->mWaitersCount = 0;

    /* Init critical section */
    InitializeCriticalSection(&cond->mWaitersCountLock);

    /* Init events */
    cond->mEvents[_CONDITION_EVENT_ONE] = CreateEvent(NULL, false, false, NULL);
    if (cond->mEvents[_CONDITION_EVENT_ONE] == NULL) {
      cond->mEvents[_CONDITION_EVENT_ALL] = NULL;
      return THREAD_ERROR;
    }

    cond->mEvents[_CONDITION_EVENT_ALL] = CreateEvent(NULL, true, false, NULL);
    if (cond->mEvents[_CONDITION_EVENT_ALL] == NULL) {
      CloseHandle(cond->mEvents[_CONDITION_EVENT_ONE]);
      cond->mEvents[_CONDITION_EVENT_ONE] = NULL;

      return THREAD_ERROR;
    }
    return THREAD_SUCCESS;
  #else
    return pthread_cond_init(cond, NULL) == 0 ? THREAD_SUCCESS : THREAD_ERROR;
  #endif
}

/**
 * Destroys a condition variable.
 *
 * Releases any resources associated with the condition variable. After calling
 * this function, the condition variable must not be used unless it is re-initialized
 * using condition_init(). On Windows, this involves closing the handles to the
 * events created for signaling and broadcasting to waiters and deleting the
 * critical section used for managing the waiters count. On POSIX systems, it
 * simply destroys a pthread condition variable.
 *
 * @param cond Pointer to the condition variable to be destroyed. Must not be NULL.
 *
 * @note On Windows, CloseHandle is called for both signaling and broadcasting
 *       event handles if they are not NULL, and the critical section for the
 *       waiters count lock is deleted. This ensures that all resources allocated
 *       during initialization are properly released.
 * @note On POSIX systems, pthread_cond_destroy is used to destroy the condition
 *       variable. It is the responsibility of the programmer to ensure that
 *       there are no waiters on the condition variable when it is destroyed.
 * @note Attempting to destroy a condition variable that is in use (i.e., has
 *       threads waiting on it) may result in undefined behavior. Ensure that
 *       the condition variable is no longer needed or that all waiting threads
 *       have been appropriately dealt with before destroying it.
 */
void condition_destroy(ThreadCondition *cond) {
  #if defined(_TTHREAD_WIN32_)
    if (cond->mEvents[_CONDITION_EVENT_ONE] != NULL) {
      CloseHandle(cond->mEvents[_CONDITION_EVENT_ONE]);
    }
    if (cond->mEvents[_CONDITION_EVENT_ALL] != NULL) {
      CloseHandle(cond->mEvents[_CONDITION_EVENT_ALL]);
    }
    DeleteCriticalSection(&cond->mWaitersCountLock);
  #else
    pthread_cond_destroy(cond);
  #endif
}

/**
 * Signals a condition variable.
 *
 * Unblocks at least one of the threads that are blocked on the specified
 * condition variable. If no threads are blocked on the condition variable at
 * the time of the call, the function does nothing and returns success. This
 * function should be called while the mutex associated with the condition
 * variable is locked, and it will not unlock the mutex.
 *
 * @param cond Pointer to the condition variable to signal. Must not be NULL.
 *
 * @return THREAD_SUCCESS if a thread was successfully signaled,
 *         THREAD_ERROR if there was an error signaling the condition variable.
 *
 * @note On Windows, the function first checks if there are any waiters by
 *       examining the waiters count within a critical section. If there are
 *       waiters, it sets the signaling event for one waiter. If setting the
 *       event fails, THREAD_ERROR is returned.
 * @note On POSIX systems, pthread_cond_signal is used to signal the condition
 *       variable. If the call to pthread_cond_signal fails, THREAD_ERROR is
 *       returned.
 * @note It is a logical error to call this function without holding the mutex
 *       associated with the condition variable as it can lead to lost wakeups.
 */
int condition_signal(ThreadCondition *cond) {
  #if defined(_TTHREAD_WIN32_)
    int haveWaiters;

    /* Are there any waiters? */
    EnterCriticalSection(&cond->mWaitersCountLock);
    haveWaiters = (cond->mWaitersCount > 0);
    LeaveCriticalSection(&cond->mWaitersCountLock);

    /* If we have any waiting threads, send them a signal */
    if(haveWaiters) {
      if (SetEvent(cond->mEvents[_CONDITION_EVENT_ONE]) == 0) {
        return THREAD_ERROR;
      }
    }
    return THREAD_SUCCESS;

  #else
    return pthread_cond_signal(cond) == 0 ? THREAD_SUCCESS : THREAD_ERROR;
  #endif
}

/**
 * Broadcasts to a condition variable.
 *
 * Unblocks all of the threads currently blocked on the specified condition variable.
 * If no threads are blocked on the condition variable at the time of the call,
 * the function does nothing and returns success. This function is typically used
 * when the state change being announced is relevant to all waiting threads rather
 * than just one. The function should be called while the mutex associated with the
 * condition variable is locked, and it will not unlock the mutex.
 *
 * @param cond Pointer to the condition variable to broadcast. Must not be NULL.
 *
 * @return THREAD_SUCCESS if the threads were successfully signaled to wake up,
 *         THREAD_ERROR if there was an error in signaling the condition variable.
 *
 * @note On Windows, this function checks if there are any waiters by accessing
 *       the waiters count within a critical section. If there are waiters, it
 *       sets the manual-reset event to wake up all waiting threads. If setting
 *       the event fails, THREAD_ERROR is returned.
 * @note On POSIX systems, pthread_cond_broadcast is used to wake up all threads
 *       waiting on the condition variable. If pthread_cond_broadcast fails, an
 *       error code is returned.
 * @note Care should be taken when using this function to ensure that it does not
 *       lead to performance issues by waking up multiple threads when only one
 *       can make progress.
 */
int condition_broadcast(ThreadCondition *cond) {
  #if defined(_TTHREAD_WIN32_)
    int haveWaiters;

    /* Are there any waiters? */
    EnterCriticalSection(&cond->mWaitersCountLock);
    haveWaiters = (cond->mWaitersCount > 0);
    LeaveCriticalSection(&cond->mWaitersCountLock);

    /* If we have any waiting threads, send them a signal */
    if(haveWaiters) {
      if (SetEvent(cond->mEvents[_CONDITION_EVENT_ALL]) == 0) {
        return THREAD_ERROR;
      }
    }

    return THREAD_SUCCESS;
  #else
    return pthread_cond_broadcast(cond) == 0 ? thrd_success : thrd_error;
  #endif
}

/**
 * A Windows-specific function for waiting on a condition variable with a timeout.
 *
 * This function implements the timed wait functionality for condition variables
 * on Windows, which is not natively supported as it is in POSIX. It uses the
 * WaitForMultipleObjects function to wait on both "signal" and "broadcast" events
 * associated with the condition variable for a specified duration.
 *
 * @param cond Pointer to the condition variable on which to wait. Must not be NULL.
 * @param mtx Pointer to the mutex associated with the condition variable. This mutex
 *        is released while waiting and re-acquired afterwards. Must not be NULL.
 * @param timeout The maximum time to wait for the condition variable to be signaled,
 *        in milliseconds. If the timeout is exceeded, the function returns with
 *        THREAD_TIMEOUT.
 *
 * @return THREAD_SUCCESS if the condition was signaled before the timeout expired,
 *         THREAD_TIMEOUT if the condition was not signaled before the timeout expired,
 *         THREAD_ERROR if an error occurred during waiting.
 *
 * @note This function increments the waiters count before waiting and decrements it
 *       after waiting. If the calling thread is the last waiter, it resets the
 *       "broadcast" event to non-signaled state to ensure correct behavior for future
 *       waits.
 * @note The mutex associated with the condition variable is unlocked before entering
 *       the wait state and is locked again after the wait is completed, regardless of
 *       the outcome. This ensures that the mutex is held by the calling thread when
 *       this function returns.
 * @note The function handles the WAIT_TIMEOUT and WAIT_FAILED cases, returning
 *       appropriate error codes. It also takes care of properly resetting the
 *       "broadcast" event if the calling thread is the last waiter.
 * @note It is important to handle the return values correctly in the calling context
 *       to ensure that the condition wait logic integrates seamlessly with the
 *       application's synchronization logic.
 */
#if defined(_TTHREAD_WIN32_)
static int _cnd_timedwait_win32(ThreadCondition *cond, Mutex *mtx, DWORD timeout) {
  DWORD result;
  int lastWaiter;

  /* Increment number of waiters */
  EnterCriticalSection(&cond->mWaitersCountLock);
  ++ cond->mWaitersCount;
  LeaveCriticalSection(&cond->mWaitersCountLock);

  /* Release the mutex while waiting for the condition (will decrease
     the number of waiters when done)... */
  mutex_unlock(mtx);

  /* Wait for either event to become signaled due to cnd_signal() or
     cnd_broadcast() being called */
  result = WaitForMultipleObjects(2, cond->mEvents, false, timeout);
  if (result == WAIT_TIMEOUT) {
    /* The mutex is locked again before the function returns, even if an error occurred */
    mutex_lock(mtx);
    return THREAD_TIMEOUT;
  }
  else if (result == WAIT_FAILED) {
    /* The mutex is locked again before the function returns, even if an error occurred */
    mutex_lock(mtx);
    return THREAD_ERROR;
  }

  /* Check if we are the last waiter */
  EnterCriticalSection(&cond->mWaitersCountLock);
  -- cond->mWaitersCount;
  lastWaiter = (result == (WAIT_OBJECT_0 + _CONDITION_EVENT_ALL)) && (cond->mWaitersCount == 0);
  LeaveCriticalSection(&cond->mWaitersCountLock);

  /* If we are the last waiter to be notified to stop waiting, reset the event */
  if (lastWaiter) {
    if (ResetEvent(cond->mEvents[_CONDITION_EVENT_ALL]) == 0) {
      /* The mutex is locked again before the function returns, even if an error occurred */
      mutex_lock(mtx);
      return THREAD_ERROR;
    }
  }

  /* Re-acquire the mutex */
  mutex_lock(mtx);

  return THREAD_SUCCESS;
}
#endif

/**
 * Waits for a condition variable to be signaled.
 *
 * Blocks the calling thread until the specified condition variable is signaled.
 * The function atomically releases the mutex associated with the condition variable
 * and adds the calling thread to the wait queue for the condition variable. When
 * the condition variable is signaled, the thread is awakened and re-acquires the mutex.
 *
 * @param cond Pointer to the condition variable to wait on. Must not be NULL.
 * @param mtx Pointer to the mutex associated with the condition variable. This mutex
 *        must be locked by the calling thread before calling this function. Must not be NULL.
 *
 * @return THREAD_SUCCESS if the thread was successfully awakened after the condition
 *         variable was signaled,
 *         THREAD_ERROR if an error occurred during the wait.
 *
 * @note On Windows, this function uses a custom implementation (_cnd_timedwait_win32)
 *       that waits indefinitely (INFINITE timeout) for the condition variable to be
 *       signaled. It handles the wait by using WaitForMultipleObjects on the condition
 *       variable's events and manages the waiters count to ensure proper signaling.
 * @note On POSIX systems, pthread_cond_wait is used to implement the wait functionality.
 *       It automatically releases the mutex and waits for the condition variable to be
 *       signaled. Upon being signaled, the mutex is re-acquired before the function returns.
 * @note This function must be called with the mutex locked by the calling thread. The mutex
 *       is automatically released while waiting and re-acquired before returning from this
 *       function. Failure to lock the mutex before calling this function results in undefined
 *       behavior.
 * @note The caller must prepare for "spurious wakeups" by using a loop to re-check the
 *       condition upon wakeup before proceeding.
 */
int condition_wait(ThreadCondition *cond, Mutex *mtx) {
  #if defined(_TTHREAD_WIN32_)
    return _cnd_timedwait_win32(cond, mtx, INFINITE);
  #else
    return pthread_cond_wait(cond, mtx) == 0 ? thrd_success : thrd_error;
  #endif
}

/**
 * Waits for a condition variable to be signaled, with a timeout.
 *
 * Blocks the calling thread until the specified condition variable is signaled
 * or until the specified timeout expires. The function atomically releases the
 * mutex associated with the condition variable and adds the calling thread to
 * the wait queue for the condition variable. If the condition variable is signaled,
 * the thread is awakened and re-acquires the mutex; if the timeout expires, the
 * function returns with a timeout status.
 *
 * @param cond Pointer to the condition variable to wait on. Must not be NULL.
 * @param mtx Pointer to the mutex associated with the condition variable. This mutex
 *        must be locked by the calling thread before calling this function. Must not be NULL.
 * @param ts Pointer to a timespec structure specifying the absolute timeout time
 *        in seconds and nanoseconds since the Epoch (1970-01-01 00:00:00 +0000, UTC).
 *
 * @return THREAD_SUCCESS if the thread was successfully awakened after the condition
 *         variable was signaled,
 *         THREAD_TIMEOUT if the condition variable was not signaled before the
 *         timeout expired,
 *         THREAD_ERROR if an error occurred during the wait or if the current time
 *         could not be obtained.
 *
 * @note On Windows, this function converts the absolute timeout specified by `ts` into
 *       a relative timeout in milliseconds and uses a custom implementation
 *       (_cnd_timedwait_win32) to wait for the condition variable to be signaled or for
 *       the timeout to expire. The current system time is obtained using timespec_get
 *       to calculate the relative timeout.
 * @note On POSIX systems, pthread_cond_timedwait is used directly with the specified
 *       absolute timeout. If the wait times out, ETIMEDOUT is returned.
 * @note The function must be called with the mutex locked by the calling thread. The mutex
 *       is automatically released while waiting and re-acquired before returning from
 *       this function. Failure to lock the mutex before calling this function results in
 *       undefined behavior.
 * @note This function is suitable for implementing timeouts on condition waits, allowing
 *       threads to wake up either when signaled or when a specific point in time has
 *       passed.
 */
int condition_timedwait(ThreadCondition *cond, Mutex *mtx, const struct timespec *ts) {
  #if defined(_TTHREAD_WIN32_)
    struct timespec now;
    if (timespec_get(&now, TIME_UTC) == TIME_UTC) {
      unsigned long long nowInMilliseconds = now.tv_sec * 1000 + now.tv_nsec / 1000000;
      unsigned long long tsInMilliseconds  = ts->tv_sec * 1000 + ts->tv_nsec / 1000000;
      DWORD delta = (tsInMilliseconds > nowInMilliseconds) ? (DWORD)(tsInMilliseconds - nowInMilliseconds) : 0;

      return _cnd_timedwait_win32(cond, mtx, delta);
    }
    else {
      return THREAD_ERROR;
    }
  #else
    int ret;
    ret = pthread_cond_timedwait(cond, mtx, ts);
    if (ret == ETIMEDOUT) {
      return THREAD_TIMEOUT;
    }
    return ret == 0 ? THREAD_SUCCESS : THREAD_ERROR;
  #endif
}

#if defined(_TTHREAD_WIN32_)

struct CThreadTSSData {
  void* value;
  ThreadSpecific key;
  struct CThreadTSSData* next;
};

static ThreadSpecificDestructor _cthread_tss_dtors[1088] = { NULL, };
static _Thread_local struct CThreadTSSData* _cthread_tss_head = NULL;
static _Thread_local struct CThreadTSSData* _cthread_tss_tail = NULL;
static void _cthread_tss_cleanup (void);

/*
* This function performs cleanup for thread-specific storage (TSS) data at thread exit. 
* It iterates over all thread-specific data keys, invoking their associated destructors 
* for non-null values up to TSS_DTOR_ITERATIONS times to handle cases where destructors allocate new TSS during cleanup. 
* After processing all destructors, it frees the memory allocated for storing TSS keys and their values, 
* ensuring no resource leaks occur when a thread terminates.
*/
static void _cthread_tss_cleanup (void) {
  struct CThreadTSSData* data;
  int iteration;
  unsigned int again = 1;
  void* value;

  for (iteration = 0 ; iteration < TSS_DTOR_ITERATIONS && again > 0 ; iteration++) {
    again = 0;
    for (data = _cthread_tss_head ; data != NULL ; data = data->next) {
      if (data->value != NULL) {
        value = data->value;
        data->value = NULL;

        if (_cthread_tss_dtors[data->key] != NULL) {
          again = 1;
          _cthread_tss_dtors[data->key](value);
        }
      }
    }
  }

  while (_cthread_tss_head != NULL) {
    data = _cthread_tss_head->next;
    free (_cthread_tss_head);
    _cthread_tss_head = data;
  }
  _cthread_tss_head = NULL;
  _cthread_tss_tail = NULL;
}

/*
* This function acts as a callback for thread or process termination events in Windows environments. 
* It checks if there are any thread-specific storage (TSS) resources to clean up upon either DLL thread detach or process detach
* events, invoking _cthread_tss_cleanup to release those resources. 
* This ensures that allocated memory for TSS is properly freed, 
* preventing memory leaks when threads exit or when the process is terminated.
*/
static void NTAPI _cthread_tss_callback(PVOID h, DWORD dwReason, PVOID pv) {
  (void)h;
  (void)pv;

  if (_cthread_tss_head != NULL && (dwReason == DLL_THREAD_DETACH || dwReason == DLL_PROCESS_DETACH)) {
    _cthread_tss_cleanup();
  }
}

/*
* This code snippet registers the _cthread_tss_callback function as a thread local storage (TLS) callback in Windows applications.
* It uses compiler-specific pragmas and attributes to place the p_thread_callback variable in a specific section (.CRT$XLB)
* of the executable, ensuring that the callback is automatically called by the system during thread or process 
* initialization and termination. 
* This mechanism is crucial for properly managing thread-specific storage cleanup in C applications compiled with MSVC, 
* especially across different architectures like x64 and x86.
*/
#if defined(_MSC_VER)
  #ifdef _M_X64
    #pragma const_seg(".CRT$XLB")
  #else
    #pragma data_seg(".CRT$XLB")
  #endif
  PIMAGE_TLS_CALLBACK p_thread_callback = _cthread_tss_callback;
  #ifdef _M_X64
    #pragma data_seg()
  #else
    #pragma const_seg()
  #endif
#else
  PIMAGE_TLS_CALLBACK p_thread_callback __attribute__((section(".CRT$XLB"))) = _cthread_tss_callback;
#endif

#endif /* defined(_TTHREAD_WIN32_) */

/** Information to pass to the new thread (what to run). */
typedef struct {
  ThreadStart mFunction; /**< Pointer to the function to be executed. */
  void * mArg;            /**< Function argument for the thread function. */
} ThreadStartInfo;


/*
* This function serves as a universal thread wrapper that is compatible with both Windows and POSIX systems. 
* It extracts thread start-up information, including the function to be executed by the thread and its argument, 
* from the aArg parameter. After executing the specified function, it cleans up the allocated start-up information and, 
* on Windows, performs any necessary thread-specific storage (TSS) cleanup before returning the function's result. 
* This design ensures a consistent interface for thread creation across different platforms 
* while abstracting away platform-specific details.
*/
#if defined(_TTHREAD_WIN32_)
static DWORD WINAPI _thrd_wrapper_function(LPVOID aArg)
#elif defined(_TTHREAD_POSIX_)
static void * _thrd_wrapper_function(void * aArg)
#endif
{
  ThreadStart fun;
  void *arg;
  int  res;

  /* Get thread startup information */
  ThreadStartInfo *ti = (ThreadStartInfo *) aArg;
  fun = ti->mFunction;
  arg = ti->mArg;

  
  free((void *)ti); // The thread is responsible for freeing the startup information 
  res = fun(arg);  // Call the actual client thread function 

  #if defined(_TTHREAD_WIN32_)
    if (_cthread_tss_head != NULL){
      _cthread_tss_cleanup();
    }
    return (DWORD)res;
  #else
    return (void*)(intptr_t)res;
  #endif
}

/**
 * Creates a new thread.
 *
 * This function initializes a new thread, assigning it to execute the specified
 * `func` function with `arg` as its argument. The newly created thread's identifier
 * is stored in `thr`.
 *
 * @param thr Pointer to a variable of type Thread that will receive the thread identifier.
 * @param func A ThreadStart function pointer that specifies the function to be executed
 *        by the new thread.
 * @param arg A void pointer to the argument that will be passed to the `func` function.
 *
 * @return THREAD_SUCCESS if the thread was successfully created,
 *         THREAD_NOMEM if memory allocation for the thread's start-up information failed,
 *         THREAD_ERROR if the thread could not be created for any other reason.
 *
 * @note On Windows, CreateThread is used to create the thread. The `ti` structure,
 *       containing the start-up information (`func` and `arg`), is passed as the
 *       thread's parameter.
 * @note On POSIX systems, pthread_create is used for thread creation, with the `ti`
 *       structure passed similarly as the thread's parameter.
 * @note If thread creation fails, any allocated resources for the thread's start-up
 *       information are freed before returning.
 * @note The calling code is responsible for ensuring that the created thread is
 *       properly managed, including joining or detaching it as necessary to
 *       prevent resource leaks.
 */
int thread_create(Thread *thr, ThreadStart func, void *arg) {
  ThreadStartInfo* ti = (ThreadStartInfo*)malloc(sizeof(ThreadStartInfo));
  if (ti == NULL) {
    return THREAD_NOMEM;
  }
  
  ti->mFunction = func;
  ti->mArg = arg;

  /* Create the thread */
  #if defined(_TTHREAD_WIN32_)
    *thr = CreateThread(NULL, 0, _thrd_wrapper_function, (LPVOID) ti, 0, NULL);
  #elif defined(_TTHREAD_POSIX_)
    if(pthread_create(thr, NULL, _thrd_wrapper_function, (void *)ti) != 0) {
      *thr = 0;
    }
  #endif

  /* Did we fail to create the thread? */
  if(!*thr) {
    free(ti);
    return THREAD_ERROR;
  }

  return THREAD_SUCCESS;
}

/**
 * Retrieves the identifier of the calling thread.
 *
 * This function returns the identifier of the thread that is currently executing
 * the code. This can be used to compare thread identifiers or to perform operations
 * that are specific to the current thread.
 *
 * @return A Thread identifier for the calling thread. The type of the identifier
 *         is platform-specific: on Windows, it is of type HANDLE, and on POSIX
 *         systems, it is of type pthread_t.
 *
 * @note On Windows, GetCurrentThread() is called to obtain a pseudo handle to the
 *       current thread. This handle is not guaranteed to be unique across threads
 *       and cannot be directly compared with other thread handles without using
 *       specific comparison functions.
 * @note On POSIX systems, pthread_self() is used to obtain the   current thread's
 *       unique identifier.
 * @note The returned thread identifier can be used with other thread-related
 *       functions, such as thread_join or thread_equal, to perform operations
 *       on the current thread.
 */
unsigned long thread_current(void) {
  #if defined(_TTHREAD_WIN32_)
    return GetCurrentThreadId();
  #else
    return (unsigned long)pthread_self();
  #endif
}

unsigned long thread_hardware_concurrency(void) {
    #ifdef _TTHREAD_POSIX_
      return sysconf(_SC_NPROCESSORS_ONLN);
    #elif defined(_TTHREAD_WIN32_)
      SYSTEM_INFO sysinfo;
      GetSystemInfo(&sysinfo);
      return sysinfo.dwNumberOfProcessors;
    #else
      return 2; // Default fallback
    #endif
}

/**
 * Detaches a thread.
 *
 * This function detaches the specified thread from the calling process, allowing
 * its resources to be automatically released back to the system upon termination.
 * Once a thread is detached, it cannot be joined with thread_join().
 *
 * @param thr The thread to detach. This is the thread identifier returned by
 *        thread_create().
 *
 * @return THREAD_SUCCESS if the thread was successfully detached,
 *         THREAD_ERROR if there was an error detaching the thread.
 *
 * @note On Windows, CloseHandle is called to release the handle to the specified
 *       thread. A return value of non-zero indicates success, otherwise THREAD_ERROR
 *       is returned if the handle could not be closed.
 * @note On POSIX systems, pthread_detach is used to detach the thread. A return
 *       value of 0 indicates success, and THREAD_ERROR is returned if the thread
 *       could not be detached.
 * @note Detaching a thread that has already been detached or has terminated results
 *       in undefined behavior. Care should be taken to ensure that a thread is
 *       detached only once and that it is not joined after being detached.
 */
int thread_detach(Thread thr) {
  #if defined(_TTHREAD_WIN32_)
    return CloseHandle(thr) != 0 ? THREAD_SUCCESS : THREAD_ERROR;
  #else
    return pthread_detach(thr) == 0 ? THREAD_SUCCESS : THREAD_ERROR;
  #endif
}

/**
 * Compares two thread identifiers to determine if they refer to the same thread.
 *
 * This function checks whether the two provided thread identifiers, `thr0` and `thr1`,
 * correspond to the same thread. This is useful for ensuring thread-specific operations
 * or for managing thread resources accurately.
 *
 * @param thr0 The first thread identifier to compare.
 * @param thr1 The second thread identifier to compare.
 *
 * @return A non-zero value (true) if `thr0` and `thr1` refer to the same thread,
 *         zero (false) otherwise.
 *
 * @note On Windows, the function uses GetThreadId to retrieve the actual thread ID
 *       from the thread handle and compares these IDs for equality. The thread handle
 *       is a pseudo handle or a handle obtained through thread creation or opening.
 * @note On POSIX systems, pthread_equal is called directly with `thr0` and `thr1` as
 *       arguments, which checks for thread identifier equality.
 * @note Thread identifiers are unique system-wide on POSIX systems. On Windows,
 *       thread handles obtained from the system directly are unique, but pseudo handles
 *       obtained via GetCurrentThread are not unique and should not be compared directly.
 */
int thread_equal(Thread thr0, Thread thr1) {
  #if defined(_TTHREAD_WIN32_)
    return GetThreadId(thr0) == GetThreadId(thr1);
  #else
    return pthread_equal(thr0, thr1);
  #endif
}

/**
 * Terminates the calling thread.
 *
 * Exits the current thread, optionally returning a result code to any joining threads.
 * If thread-specific data destructors are set, they will be executed before the thread
 * terminates. This function does not return.
 *
 * @param res The exit code of the thread. This value can be retrieved by another
 *        thread that joins with this thread.
 *
 * @note On Windows, this function performs thread-specific storage cleanup before
 *       calling ExitThread to terminate the thread with the given result code.
 * @note On POSIX systems, pthread_exit is used to terminate the thread, passing
 *       `res` as the exit status converted to a void pointer.
 * @note Calling this function will immediately terminate the calling thread, and
 *       the stack of the thread will not be unwound (i.e., destructors for local
 *       objects will not be called).
 */
void thread_exit(int res) {
  #if defined(_TTHREAD_WIN32_)
    if (_cthread_tss_head != NULL) {
      _cthread_tss_cleanup();
    }

    ExitThread((DWORD)res);
  #else
    pthread_exit((void*)(intptr_t)res);
  #endif
}

/**
 * Waits for a thread to terminate and optionally retrieves the thread's exit code.
 *
 * This function blocks the calling thread until the specified thread `thr` terminates.
 * After the thread has terminated, if `res` is not NULL, the exit code of the terminated
 * thread is stored in the location pointed to by `res`.
 *
 * @param thr The thread identifier of the thread to join. This identifier should have
 *        been obtained by a previous call to thread_create().
 * @param res Pointer to an integer that will receive the exit code of the joined thread.
 *        If the caller does not care about the exit code, this parameter can be NULL.
 *
 * @return THREAD_SUCCESS if the thread was successfully joined and, if applicable, the
 *         exit code was retrieved,
 *         THREAD_ERROR if the wait failed or if the exit code could not be retrieved.
 *
 * @note On Windows, WaitForSingleObject is used to wait for the thread to terminate.
 *       GetExitCodeThread is then called to retrieve the thread's exit code if `res`
 *       is not NULL. The handle to the thread is closed with CloseHandle before returning.
 * @note On POSIX systems, pthread_join is used to wait for the thread to terminate.
 *       The thread's exit status is obtained and, if `res` is not NULL, it is stored in
 *       the location pointed to by `res`.
 * @note It is undefined behavior to join a thread with itself, to join a thread more
 *       than once, or to join a detached thread. Doing so may lead to deadlocks or
 *       other undefined behavior.
 */
int thread_join(Thread thr, int *res) {
  #if defined(_TTHREAD_WIN32_)
    DWORD dwRes;

    if (WaitForSingleObject(thr, INFINITE) == WAIT_FAILED) {
      return THREAD_ERROR;
    }
    if (res != NULL) {
      if (GetExitCodeThread(thr, &dwRes) != 0) {
        *res = (int) dwRes;
      }
      else {
        return THREAD_ERROR;
      }
    }
    CloseHandle(thr);
  #elif defined(_TTHREAD_POSIX_)
    void *pres;
    if (pthread_join(thr, &pres) != 0) {
      return THREAD_ERROR;
    }
    if (res != NULL) {
      *res = (int)(intptr_t)pres;
    }
  #endif
    return THREAD_SUCCESS;
}

/**
 * Suspends the execution of the current thread for a specified duration.
 *
 * This function makes the calling thread sleep for the time specified in `duration`,
 * optionally returning the remaining time if the sleep was interrupted.
 *
 * @param duration Pointer to a timespec structure that specifies the desired sleep duration
 *        in seconds and nanoseconds.
 * @param remaining If not NULL, this parameter will be filled with the remaining time if
 *        the sleep was interrupted by a signal. On Windows, it represents the time remaining
 *        if the sleep was interrupted by an I/O completion callback.
 *
 * @return 0 if the sleep completed for the full duration,
 *         -1 if the sleep was interrupted (POSIX) or if an I/O completion callback woke
 *         the thread (Windows),
 *         -2 if an error occurred (other than being interrupted by a signal on POSIX).
 *
 * @note On POSIX systems, this function uses nanosleep to sleep for the specified duration.
 *       If interrupted by a signal (EINTR), it can optionally return the remaining time.
 * @note On Windows, SleepEx is used to sleep, allowing the thread to be woken up by I/O
 *       completion callbacks if `true` is passed as the second argument. The function
 *       calculates the elapsed time to potentially return the remaining duration.
 * @note The function's behavior regarding the remaining time is platform-dependent and
 *       should be used with caution for cross-platform applications.
 */
int thread_sleep(const struct timespec *duration, struct timespec *remaining) {
  #if !defined(_TTHREAD_WIN32_)
    int res = nanosleep(duration, remaining);
    if (res == 0) {
      return 0;
    } 
    else if (errno == EINTR) {
      return -1;
    } 
    else {
      return -2;
    }
  #else
    struct timespec start;
    DWORD t;

    timespec_get(&start, TIME_UTC);

    t = SleepEx((DWORD)(duration->tv_sec * 1000 +
                duration->tv_nsec / 1000000 +
                (((duration->tv_nsec % 1000000) == 0) ? 0 : 1)),
                true);

    if (t == 0) {
      return 0;
    } 
    else {
      if (remaining != NULL) {
        timespec_get(remaining, TIME_UTC);
        remaining->tv_sec -= start.tv_sec;
        remaining->tv_nsec -= start.tv_nsec;

        if (remaining->tv_nsec < 0) {
          remaining->tv_nsec += 1000000000;
          remaining->tv_sec -= 1;
        }
      }

      return (t == WAIT_IO_COMPLETION) ? -1 : -2;
    }
  #endif
}

/**
 * Yields the execution of the current thread.
 *
 * This function allows the current thread to offer the remainder of its time slice
 * to other threads of equal or greater priority on the processor. If there are no
 * other threads that need to run, the calling thread may continue execution without
 * delay.
 *
 * @note On Windows, Sleep(0) is called to yield the execution. This indicates to the
 *       system that the thread is yielding its current time slice but is ready to
 *       continue execution if no higher-priority threads need to run.
 * @note On POSIX systems, sched_yield() is used to yield the processor, allowing
 *       other threads to run. The POSIX version directly maps to the system's
 *       scheduling policy, potentially allowing other threads to run even if they
 *       have the same priority as the yielding thread.
 * @note This function can be used to improve the responsiveness of applications
 *       with multiple threads or to avoid busy waiting while waiting for a condition
 *       to become true.
 */
void thread_yield(void) {
  #if defined(_TTHREAD_WIN32_)
    Sleep(0);
  #else
    sched_yield();
  #endif
}

/**
 * Creates a thread-specific storage (TSS) key.
 *
 * Allocates a key that can be used by threads to store and retrieve data unique to each thread.
 * The key can be used across different threads to access data without interfering with each other.
 * Optionally, a destructor function can be provided to automatically clean up the data associated
 * with the key when a thread exits.
 *
 * @param key Pointer to a variable of type ThreadSpecific that will receive the newly created key.
 * @param dtor A destructor function that will be called to clean up thread-specific data when a
 *        thread exits. This parameter can be NULL if no destructor is needed.
 *
 * @return THREAD_SUCCESS if the key was successfully created,
 *         THREAD_ERROR if the key could not be created due to resource limitations or
 *         other errors.
 *
 * @note On Windows, TlsAlloc is used to allocate a TSS key. The destructor function
 *       provided is stored in an internal array to be called manually during thread
 *       cleanup, as Windows does not automatically invoke destructors for thread-specific
 *       data.
 * @note On POSIX systems, pthread_key_create is used to create the key and associate
 *       the destructor directly. The POSIX implementation takes care of calling the
 *       destructor automatically when a thread exits and has non-NULL data associated
 *       with the key.
 * @note It is important to ensure that thread_specific_destroy is called to free the
 *       key when it is no longer needed, especially on Windows, to prevent resource leaks.
 */
int thread_specific_create(ThreadSpecific *key, ThreadSpecificDestructor dtor) {
  #if defined(_TTHREAD_WIN32_)
    *key = TlsAlloc();
    if (*key == TLS_OUT_OF_INDEXES) {
      return THREAD_ERROR;
    }
    _cthread_tss_dtors[*key] = dtor;
  #else
    if (pthread_key_create(key, dtor) != 0) {
      return THREAD_ERROR;
    }
  #endif
    return THREAD_SUCCESS;
}

/**
 * Deletes a thread-specific storage (TSS) key.
 *
 * Frees a previously allocated TSS key, optionally invoking the destructor function
 * for any non-NULL values currently associated with the key in any thread. After this
 * function is called, the key and its associated data can no longer be used.
 *
 * @param key The TSS key to delete. This key should have been created with
 *        thread_specific_create().
 *
 * @note On Windows, this function first retrieves any data associated with the key
 *       for the calling thread, removes it from the internal tracking list, and frees
 *       the associated data structure. It then clears the destructor function pointer
 *       and calls TlsFree to release the key. If there are other threads using the key,
 *       their associated data will not be automatically freed; it is the responsibility
 *       of the application to ensure that all thread-specific data is cleaned up before
 *       a key is deleted.
 * @note On POSIX systems, pthread_key_delete is used to delete the key. The POSIX
 *       implementation automatically invokes the destructor function for any non-NULL
 *       values associated with the key in any thread before the key is deleted.
 * @note Care must be taken to ensure that thread_specific_delete is called at an
 *       appropriate time when no threads will be using the key, to avoid undefined behavior.
 */
void thread_specific_delete(ThreadSpecific key) {
  #if defined(_TTHREAD_WIN32_)
    struct CThreadTSSData* data = (struct CThreadTSSData*) TlsGetValue (key);
    struct CThreadTSSData* prev = NULL;

    if (data != NULL) {
      if (data == _cthread_tss_head) {
        _cthread_tss_head = data->next;
      }
      else {
        prev = _cthread_tss_head;
        if (prev != NULL) {
          while (prev->next != data) {
            prev = prev->next;
          }
        }
      }
      if (data == _cthread_tss_tail) {
        _cthread_tss_tail = prev;
      }
      free (data);
    }
    _cthread_tss_dtors[key] = NULL;
    TlsFree(key);
  #else
    pthread_key_delete(key);
  #endif
}

/**
 * Retrieves the value associated with a thread-specific storage (TSS) key for the current thread.
 *
 * This function returns the value associated with a given TSS key for the calling thread.
 * Each thread can have a distinct value associated with the same key, enabling threads
 * to maintain thread-local storage.
 *
 * @param key The TSS key for which to retrieve the value. The key should have been
 *        created previously using thread_specific_create().
 *
 * @return The value associated with the key for the current thread. If no value has been
 *         set for the key in the current thread, NULL is returned.
 *
 * @note On Windows, the function retrieves a pointer to a structure representing the
 *       thread-specific data associated with the key, and then returns the `value` field
 *       of that structure. If no data has been associated with the key for the current
 *       thread, NULL is returned.
 * @note On POSIX systems, pthread_getspecific is used directly to retrieve the value
 *       associated with the key for the current thread. The POSIX implementation handles
 *       the thread-local storage, ensuring that each thread gets its own distinct value.
 * @note This function can be used to access data that needs to be unique to each thread,
 *       such as thread-specific error information or a thread-specific data buffer.
 */
void *thread_specific_get(ThreadSpecific key) {
  #if defined(_TTHREAD_WIN32_)
    struct CThreadTSSData* data = (struct CThreadTSSData*)TlsGetValue(key);
    if (data == NULL) {
      return NULL;
    }
    return data->value;
  #else
    return pthread_getspecific(key);
  #endif
}

/**
 * Sets the value of a thread-specific storage (TSS) key for the current thread.
 *
 * Associates a new value with a given TSS key for the calling thread. Each thread
 * has its own distinct value for each key, allowing threads to maintain thread-local
 * data. If the key already has an associated value in the calling thread, the value
 * is replaced.
 *
 * @param key The TSS key for which to set the value. The key should have been
 *        created previously using thread_specific_create().
 * @param val Pointer to the new value to associate with the key. This can be NULL
 *        to clear the existing value for the current thread.
 *
 * @return THREAD_SUCCESS if the value was successfully associated with the key,
 *         THREAD_ERROR if the operation failed due to memory allocation errors
 *         (on Windows) or other errors.
 *
 * @note On Windows, if there is no existing data structure associated with the key
 *       for the current thread, a new one is allocated and linked into a global list
 *       managed by the library. TlsSetValue is then used to associate the data
 *       structure with the key for the current thread. This mechanism allows the
 *       library to track and clean up thread-local data.
 * @note On POSIX systems, pthread_setspecific is used directly to associate the value
 *       with the key for the current thread. POSIX takes care of managing the storage
 *       and automatically invokes the destructor function for the old value if one is
 *       set.
 * @note This function can fail on Windows if memory allocation for the new data
 *       structure fails or if TlsSetValue fails. On POSIX systems, failure can occur
 *       if pthread_setspecific encounters an error.
 */
int thread_specific_set(ThreadSpecific key, void *val) {
  #if defined(_TTHREAD_WIN32_)
    struct CThreadTSSData* data = (struct CThreadTSSData*)TlsGetValue(key);
    if (data == NULL) {
        data = (struct CThreadTSSData*)malloc(sizeof(struct CThreadTSSData));
        if (data == NULL) {
          return THREAD_ERROR;
	      }

      data->value = NULL;
      data->key = key;
      data->next = NULL;

      if (_cthread_tss_tail != NULL) {
        _cthread_tss_tail->next = data;
      }
      else {
        _cthread_tss_tail = data;
      }

      if (_cthread_tss_head == NULL){
          _cthread_tss_head = data;
      }

      if (!TlsSetValue(key, data)){
          free (data);
          return THREAD_ERROR;
      }
    }
    data->value = val;
  #else
  if (pthread_setspecific(key, val) != 0){
      return THREAD_ERROR;
  }
  #endif
    return THREAD_SUCCESS;
}

/*
* This function provides a cross-platform method to retrieve the current time in seconds and nanoseconds, 
* populating a struct timespec. It supports systems where clock_gettime is not available by falling back to other time functions. 
* On Windows, _ftime_s is used to get the system time, and on POSIX systems without CLOCK_REALTIME, 
* gettimeofday is used as a fallback. The function only succeeds if the base parameter is TIME_UTC, 
* ensuring compatibility with systems expecting UTC time.
*/
#if defined(_TTHREAD_EMULATE_TIMESPEC_GET_)
int _tthread_timespec_get(struct timespec *ts, int base) {
  #if defined(_TTHREAD_WIN32_)
    struct _timeb tb;
  #elif !defined(CLOCK_REALTIME)
    struct timeval tv;
  #endif

  if (base != TIME_UTC){
    return 0;
  }

  #if defined(_TTHREAD_WIN32_)
    _ftime_s(&tb);
    ts->tv_sec = (time_t)tb.time;
    ts->tv_nsec = 1000000L * (long)tb.millitm;
  #elif defined(CLOCK_REALTIME)
    base = (clock_gettime(CLOCK_REALTIME, ts) == 0) ? base : 0;
  #else
    gettimeofday(&tv, NULL);
    ts->tv_sec = (time_t)tv.tv_sec;
    ts->tv_nsec = 1000L * (long)tv.tv_usec;
  #endif

  return base;
}
#endif 

/*
* This function implements a thread-safe, one-time initialization routine on Windows. 
* It utilizes a spin lock with InterlockedCompareExchange to ensure that the initialization function func is 
* executed exactly once across all threads. The function's state is managed through flag->status, 
* with a critical section controlling access to the execution block of func, 
* ensuring that concurrent calls do not re-execute the already completed initialization code.
*/
#if defined(_TTHREAD_WIN32_)
void call_once(OnceFlag *flag, void (*func)(void)) {
  while (flag->status < 3) {
    switch (flag->status) {
      case 0: {
        if (InterlockedCompareExchange (&(flag->status), 1, 0) == 0) {
          InitializeCriticalSection(&(flag->lock));
          EnterCriticalSection(&(flag->lock));
          flag->status = 2;
          func();
          flag->status = 3;
          LeaveCriticalSection(&(flag->lock));
          return;
        }
        break;
      }
      case 1: 
        break;
      case 2:
        EnterCriticalSection(&(flag->lock));
        LeaveCriticalSection(&(flag->lock));
        break;
    }
  }
}
#endif /* defined(_TTHREAD_WIN32_) */


