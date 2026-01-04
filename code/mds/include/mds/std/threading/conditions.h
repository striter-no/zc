#pragma once
#include <pthread.h>
#include <mds/std/_preincl/base.h>
#include <mds/std/_preincl/defs.h>
#include "mds/std/_preincl/base/preproc.h"
#include "mds/std/time/timing.h"
#include "mutexes.h"
#include <stdatomic.h>

typedef struct {
    pthread_cond_t posix_cnd;
    atomic_bool    data_ready;
    atomic_size_t  subscribers;
    atomic_size_t  done;
} Condt;

option __cnd_create();
option __cnd_wait(Condt *cnd, Mutex *mtx);
option __cnd_timedwait(Condt *cnd, Mutex *mtx, i64 nanos);
option __cnd_signal(Condt *cnd);
option __cnd_broadcast(Condt *cnd);
void __cnd_subscribe(Condt *cnd);
void __cnd_unsubscribe(Condt *cnd);
void __cnd_subcheck(Condt *cnd);
option __cnd_destroy(Condt *cnd);
#ifdef CONDITIONS_IMPLEMENTATION

void __cnd_subscribe(Condt *cnd){
    if (!cnd) return;
    atomic_fetch_add(&cnd->subscribers, 1);
}

void __cnd_subcheck(Condt *cnd){
    if (!cnd) return;

    size_t current_done = atomic_fetch_add(&cnd->done, 1) + 1;
    size_t total_subs = atomic_load(&cnd->subscribers);
    
    if (current_done >= total_subs) {
        atomic_store(&cnd->done, 0);
        atomic_store(&cnd->data_ready, false);
    }
}

option __cnd_create(){
    AbstractAllocator *absa = try(global.get(".absa")).data;
    Condt *cnd = try(absa->alloc(absa->real, sizeof(Condt))).data;
    if (!cnd) throw(
        "Conditional Variable: failed to create, malloc() failed",
        "Condt.Creation.Malloc.Failed",
        -1
    );
    if (pthread_cond_init(&cnd->posix_cnd, NULL) != 0) throw(
        "Conditional Variable: failed to create, pthread_cond_init() failed",
        "Condt.Creation.POSIX.Failure",
        -2
    );

    atomic_store(&cnd->data_ready, false);
    atomic_store(&cnd->done, 0);
    atomic_store(&cnd->subscribers, 0);
    return opt(cnd, sizeof(Condt), 1);
}

option __cnd_broadcast(Condt *cnd){
    if (!cnd) throw(
        "Conditional Variable: failed to broadcast, cnd ptr is NULL",
        "Condt.Broadcast.Cnd.Ptr.IsNULL",
        -1
    );

    if (pthread_cond_broadcast(&cnd->posix_cnd) != 0){
        throw(
            "Conditional Variable: failed to broadcast, pthread_cond_broadcast() failed",
            "Condt.Broadcast.POSIX.Failure",
            -2
        );
    }
    atomic_store(&cnd->data_ready, true);

    return noerropt;
}

option __cnd_wait(Condt *cnd, Mutex *mtx){
    if (!cnd) throw(
        "Conditional Variable: failed to wait, cnd ptr is NULL",
        "Condt.Wait.Cnd.Ptr.IsNULL",
        -1
    );

    if (!mtx) throw(
        "Conditional Variable: failed to wait, mtx ptr is NULL",
        "Condt.Wait.Mtx.Ptr.IsNULL",
        -2
    );

    if (pthread_cond_wait(&cnd->posix_cnd, &mtx->posix_mtx) != 0){
        throw(
            "Conditional Variable: failed to wait, pthread_cond_wait() failed",
            "Condt.Wait.POSIX.Failure",
            -3
        );
    }

    return noerropt;
}

option __cnd_timedwait(Condt *cnd, Mutex *mtx, i64 nanos){
    if (!cnd) throw(
        "Conditional Variable: failed to timedwait, cnd ptr is NULL",
        "Condt.Timedwait.Cnd.Ptr.IsNULL",
        -1
    );

    if (!mtx) throw(
        "Conditional Variable: failed to timedwait, mtx ptr is NULL",
        "Condt.Timedwait.Mtx.Ptr.IsNULL",
        -2
    );

    var posixt = __time_as_posix(nanos);
    if (pthread_cond_timedwait(&cnd->posix_cnd, &mtx->posix_mtx, (const struct timespec*)&posixt) != 0){
        throw(
            "Conditional Variable: failed to timedwait, pthread_cond_timedwait() failed",
            "Condt.Timedait.POSIX.Failure",
            -3
        );
    }

    return noerropt;
}

option __cnd_destroy(Condt *cnd){
    if (!cnd) return noerropt;

    AbstractAllocator *absa = try(global.get(".absa")).data;
    atomic_store(&cnd->data_ready, false);
    pthread_cond_destroy(&cnd->posix_cnd);
    try(absa->free(absa->real, cnd));

    return noerropt;
}

void __cnd_unsubscribe(Condt *cnd){
    if (!cnd) return;
    atomic_fetch_add(&cnd->subscribers, -1);
}

option __cnd_signal(Condt *cnd){
    if (!cnd) throw(
        "Conditional Variable: failed to signal, cnd ptr is NULL",
        "Condt.Signal.Cnd.Ptr.IsNULL",
        -1
    );

    if (pthread_cond_signal(&cnd->posix_cnd) != 0){
        throw(
            "Conditional Variable: failed to signal, pthread_cond_signal() failed",
            "Condt.Signal.POSIX.Failure",
            -2
        );
    }

    atomic_store(&cnd->data_ready, true);
    return noerropt;
}


#endif