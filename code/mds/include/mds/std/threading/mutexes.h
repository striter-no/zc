#pragma once
#include "mds/std/_preincl/base/preproc.h"
#include <pthread.h>
#include <mds/std/_preincl/base.h>
#include <mds/std/_preincl/defs.h>
#include <mds/std/_preincl/globals.h>
#include <mds/std/memory/allocators/abstract.h>

typedef enum {
    MUTEX_PLAIN,
    MUTEX_RECURSIVE
} MUTEX_TYPE;

typedef struct {
    pthread_mutex_t posix_mtx;
} Mutex;

option  __mtx_create(MUTEX_TYPE type);
option __mtx_trylock(Mutex *mtx);
option __mtx_lock(Mutex *mtx);
option __mtx_unlock(Mutex *mtx);
option __mtx_free(Mutex *mtx);
#ifdef MUTEXES_IMPLEMENTATION

option __mtx_unlock(Mutex *mtx){
    if (pthread_mutex_unlock(&mtx->posix_mtx) != 0) throw(
        "Mutex unlock failed, pthread_mutex_unlock() failed",
        "Mutex.Unlock.POSIX.Failure",
        -1
    );
    return noerropt;
}

option  __mtx_create(MUTEX_TYPE type){
    AbstractAllocator *absa = try(global.get(".absa")).data;
    Mutex *mtx = try(absa->alloc(absa->real, sizeof(Mutex))).data;
    if (!mtx) throw("Cannot create mutex, malloc failed", "Mutex.Creation.Malloc.Failed", -1);

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    switch (type){
        case (MUTEX_PLAIN):     pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); break;
        case (MUTEX_RECURSIVE): pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_DEFAULT); break;
        default: unreachable;
    }

    pthread_mutex_init(&mtx->posix_mtx, &attr);
    pthread_mutexattr_destroy(&attr);
    return opt(mtx, sizeof(Mutex), true);
}

option __mtx_lock(Mutex *mtx){
    if (pthread_mutex_lock(&mtx->posix_mtx) != 0) throw(
        "Mutex lock failed, pthread_mutex_lock() failed",
        "Mutex.Lock.POSIX.Failure",
        -1
    );
    return noerropt;
}

option __mtx_trylock(Mutex *mtx){
    if (pthread_mutex_trylock(&mtx->posix_mtx) != 0) throw(
        "Mutex trylock failed, pthread_mutex_trylock() failed",
        "Mutex.Trylock.POSIX.Failure",
        -1
    );
    return noerropt;
}

option __mtx_free(Mutex *mtx){
    if (!mtx) return noerropt;

    AbstractAllocator *absa = try(global.get(".absa")).data;
    pthread_mutex_destroy(&mtx->posix_mtx);
    try(absa->free(absa->real, mtx));

    return noerropt;
}


#endif