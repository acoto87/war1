/*
 * this is based on crossthread.h gists by Okamura Yasunobu
 * https://gist.github.com/informationsea/f63c2b3854b20bd540ab
 *
 */

#pragma once

#ifdef _WIN32

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>
#include <process.h>
#include <errno.h>

typedef struct
{
    HANDLE handle;
} pthread_mutex_t;

typedef struct
{
    int attr;
} pthread_mutexattr_t;

int pthread_mutexattr_init(pthread_mutexattr_t* attr)
{
    // do nothing
    return 0;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t* attr)
{
    // do nothing
    return 0;
}

int pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* attr)
{
    HANDLE handle = CreateMutex(NULL, false, NULL);

    if (handle != NULL)
    {
        mutex->handle = handle;
        return 0;
    }

    return 1;
}

int pthread_mutex_lock(pthread_mutex_t* mutex)
{
    DWORD retvalue = WaitForSingleObject(mutex->handle, INFINITE);

    if (retvalue == WAIT_OBJECT_0)
    {
        return 0;
    }

    return EINVAL;
}

int pthread_mutex_trylock(pthread_mutex_t* mutex)
{
    DWORD retvalue = WaitForSingleObject(mutex->handle, 0);
    if (retvalue == WAIT_OBJECT_0)
    {
        return 0;
    }

    if(retvalue == WAIT_TIMEOUT)
    {
        return EBUSY;
    }

    return EINVAL;
}

int pthread_mutex_unlock(pthread_mutex_t* mutex)
{
    return !ReleaseMutex(mutex->handle);
}

int pthread_mutex_destroy(pthread_mutex_t* mutex)
{
    return !CloseHandle(mutex->handle);
}

#ifdef __cplusplus
}
#endif

#else
#include <pthread.h>
#include <unistd.h>
#endif
