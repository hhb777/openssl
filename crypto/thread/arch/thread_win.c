/*
 * Copyright 2019-2021 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <internal/thread_arch.h>

#if defined(OPENSSL_THREADS_WINNT)
# include <process.h>
# include <windows.h>

#include <stdio.h>

#define TERMIN(X) \
    LOG("Thread %p is being terminated (native)")
#define TERMED(X) \
    LOG("Thread %p terminated (native)", X)
#define JOINING(X) \
    LOG("Thread %p is being joined (native)");
#define JOINED(X) \
    LOG("Thread %p joined (native)", X);
#define CVING(X) \
    LOG("CV waiting %p", X);
#define CVED(X) \
    LOG("CV recvd bcast %p", X);
#define LOCKING(X) \
    LOG("Lock %p locking", X);
#define LOCKED(X) \
    LOG("Lock %p locked", X);
#define UNLOCKED(X) \
    LOG("Lock %p unlocked", X);
#define BCAST(X) \
    LOG("Bcast %p", X);
#define RET(X) \
    { \
        LOG("%s", "Returning" #X); \
        return X; \
    }
#define GOTO(X) \
    { \
        LOG("%s", "Goto" #X); \
        goto X; \
    }
#define LOG(FMT, ...) \
    { \
        printf("%s %s %d: "FMT"\n", __FILE__, __FUNCTION__, \
            __LINE__, __VA_ARGS__); \
        fflush(stdout); \
    }

static DWORD __stdcall thread_start_thunk(LPVOID vthread)
{
    CRYPTO_THREAD *thread;
    CRYPTO_THREAD_RETVAL ret;

    thread = (CRYPTO_THREAD *)vthread;

    thread->thread_id = GetCurrentThreadId();

    ret = thread->routine(thread->data);
    ossl_crypto_mutex_lock(thread->statelock);
    CRYPTO_THREAD_SET_STATE(thread, CRYPTO_THREAD_FINISHED);
    thread->retval = ret;
    ossl_crypto_condvar_broadcast(thread->condvar);
    ossl_crypto_mutex_unlock(thread->statelock);

    return 0;
}

int ossl_crypto_thread_native_spawn(CRYPTO_THREAD *thread)
{
    HANDLE *handle;

    handle = OPENSSL_zalloc(sizeof(*handle));
    if (handle == NULL)
        goto fail;

    *handle = (HANDLE)_beginthreadex(NULL, 0, &thread_start_thunk, thread, 0, NULL);
    if (*handle == NULL)
        goto fail;

    thread->handle = handle;
    return 1;

fail:
    thread->handle = NULL;
    OPENSSL_free(handle);
    return 0;
}

int ossl_crypto_thread_native_perform_join(CRYPTO_THREAD *thread, CRYPTO_THREAD_RETVAL *retval)
{
    DWORD thread_retval;
    HANDLE *handle;

    if (thread == NULL || thread->handle == NULL)
        return 0;

    handle = (HANDLE *) thread->handle;
    if (WaitForSingleObject(*handle, INFINITE) != WAIT_OBJECT_0)
        return 0;

    if (GetExitCodeThread(*handle, &thread_retval) == 0)
        return 0;

    /*
     * GetExitCodeThread call followed by this check is to make sure that
     * the thread exitted properly. In particular, thread_retval may be
     * non-zero when exitted via explicit ExitThread/TerminateThread or
     * if the thread is still active (returns STILL_ACTIVE (259)).
     */
    if (thread_retval != 0)
        return 0;

    if (CloseHandle(*handle) == 0)
        return 0;

    return 1;
}

int ossl_crypto_thread_native_terminate(CRYPTO_THREAD *thread)
{
    uint64_t mask;
    HANDLE *handle;

    mask = CRYPTO_THREAD_FINISHED;
    mask |= CRYPTO_THREAD_TERMINATED;
    mask |= CRYPTO_THREAD_JOINED;

    if (thread == NULL)
        return 1;

    LOG("Terminating %p", thread);

    LOCKING(thread->statelock);
    ossl_crypto_mutex_lock(thread->statelock);
    LOCKED(thread->statelock);
    if (thread->handle == NULL || CRYPTO_THREAD_GET_STATE(thread, mask)) {
        GOTO(terminated);
    }
    ossl_crypto_mutex_unlock(thread->statelock);
    UNLOCKED(thread->statelock);

    handle = thread->handle;
    LOG("Calling waitforsingle...");
    if (WaitForSingleObject(*handle, 0) != WAIT_OBJECT_0) {
        LOG("Calling termthr...");
        if (TerminateThread(*handle, STILL_ACTIVE) == 0) {
            LOCKING(thread->statelock);
            ossl_crypto_mutex_lock(thread->statelock);
            LOCKED(thread->statelock);
            CRYPTO_THREAD_SET_ERROR(thread, CRYPTO_THREAD_TERMINATED);
            ossl_crypto_mutex_unlock(thread->statelock);
            LOCKED(thread->statelock);
            RET(0);
        }
        LOG("Called termthr...");
    }
    LOG("Called waitforsingle...");

    LOG("Calling closehandle...");
    if (CloseHandle(*handle) == 0) {
        CRYPTO_THREAD_SET_ERROR(thread, CRYPTO_THREAD_TERMINATED);
        RET(0);
    }
    LOG("Called closehandle...");

    thread->handle = NULL;
    OPENSSL_free(handle);

    LOCKING(thread->statelock);
    ossl_crypto_mutex_lock(thread->statelock);
    LOCKED(thread->statelock);
terminated:
    CRYPTO_THREAD_UNSET_ERROR(thread, CRYPTO_THREAD_TERMINATED);
    CRYPTO_THREAD_SET_STATE(thread, CRYPTO_THREAD_TERMINATED);
    ossl_crypto_mutex_unlock(thread->statelock);
    UNLOCKED(thread->statelock);
    return 1;
}

int ossl_crypto_thread_native_exit(void)
{
    _endthreadex(0);
    return 1;
}

int ossl_crypto_thread_native_is_self(CRYPTO_THREAD *thread)
{
    return thread->thread_id == GetCurrentThreadId();
}

CRYPTO_MUTEX *ossl_crypto_mutex_new(void)
{
    CRITICAL_SECTION *mutex;

    if ((mutex = OPENSSL_zalloc(sizeof(*mutex))) == NULL)
        return NULL;
    InitializeCriticalSection(mutex);
    return (CRYPTO_MUTEX *)mutex;
}

void ossl_crypto_mutex_lock(CRYPTO_MUTEX *mutex)
{
    CRITICAL_SECTION *mutex_p;

    mutex_p = (CRITICAL_SECTION *)mutex;
    EnterCriticalSection(mutex_p);
}

int ossl_crypto_mutex_try_lock(CRYPTO_MUTEX *mutex)
{
    CRITICAL_SECTION *mutex_p;

    mutex_p = (CRITICAL_SECTION *)mutex;
    if (TryEnterCriticalSection(mutex_p))
        return 1;

    return 0;
}

void ossl_crypto_mutex_unlock(CRYPTO_MUTEX *mutex)
{
    CRITICAL_SECTION *mutex_p;

    mutex_p = (CRITICAL_SECTION *)mutex;
    LeaveCriticalSection(mutex_p);
}

void ossl_crypto_mutex_free(CRYPTO_MUTEX **mutex)
{
    CRITICAL_SECTION **mutex_p;

    mutex_p = (CRITICAL_SECTION **)mutex;
    if (*mutex_p != NULL)
        DeleteCriticalSection(*mutex_p);
    OPENSSL_free(*mutex_p);
    *mutex = NULL;
}

CRYPTO_CONDVAR *ossl_crypto_condvar_new(void)
{
    CONDITION_VARIABLE *cv_p;

    if ((cv_p = OPENSSL_zalloc(sizeof(*cv_p))) == NULL)
        return NULL;
    InitializeConditionVariable(cv_p);
    return (CRYPTO_CONDVAR *)cv_p;
}

void ossl_crypto_condvar_wait(CRYPTO_CONDVAR *cv, CRYPTO_MUTEX *mutex)
{
    CONDITION_VARIABLE *cv_p;
    CRITICAL_SECTION *mutex_p;

    cv_p = (CONDITION_VARIABLE *)cv;
    mutex_p = (CRITICAL_SECTION *)mutex;
    SleepConditionVariableCS(cv_p, mutex_p, INFINITE);
}

void ossl_crypto_condvar_broadcast(CRYPTO_CONDVAR *cv)
{
    CONDITION_VARIABLE *cv_p;

    cv_p = (CONDITION_VARIABLE *)cv;
    WakeAllConditionVariable(cv_p);
}

void ossl_crypto_condvar_free(CRYPTO_CONDVAR **cv)
{
    CONDITION_VARIABLE **cv_p;

    cv_p = (CONDITION_VARIABLE **)cv;
    OPENSSL_free(*cv_p);
    *cv_p = NULL;
}

void ossl_crypto_mem_barrier(void)
{
    MemoryBarrier();
}

#endif
