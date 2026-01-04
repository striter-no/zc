#pragma once
#include <pthread.h>
#include <mds/std/_preincl/base.h>
#include <mds/std/_preincl/defs.h>
#include <mds/std/_preincl/globals.h>
#include <mds/std/memory/allocators/abstract.h>
#include "conditions.h"
#include "mutexes.h"

typedef struct {
    Condt *conditional_ptr;
    Mutex *self_mtx;
} Awaiter;

option __awt_create(Condt *cnd_ptr, MUTEX_TYPE mtx_type);
option __awt_wait(Awaiter *awt);
option __awt_free(Awaiter *awt);
#ifdef AWAITER_IMPLEMENTATION

option __awt_wait(Awaiter *awt){
    if (!awt) throw("Awaiter, failed to wait, awt is NULL", "Awaiter.Wait.AwtPtr.IsNULL", -1);
    
    __mtx_lock(awt->self_mtx);
    while (!atomic_load(&awt->conditional_ptr->data_ready)){
        __cnd_wait(awt->conditional_ptr, awt->self_mtx);
    }
    __mtx_unlock(awt->self_mtx);
    // atomic_fetch_add(&awt->conditional_ptr->done, 1);

    return noerropt;
}

option __awt_free(Awaiter *awt){
    if (!awt) return noerropt;
    AbstractAllocator *absa = try(global.get(".absa")).data;
    
    if (awt->self_mtx){
        __mtx_free(awt->self_mtx);
    }
    try(absa->free(absa->real, awt));
    return noerropt;
}

option __awt_create(Condt *cnd_ptr, MUTEX_TYPE mtx_type){
    if (!cnd_ptr) throw("Awaiter, failed to create, cnd_ptr is NULL", "Awaiter.Creation.CndPtr.IsNULL", -1);
    
    AbstractAllocator *absa = try(global.get(".absa")).data;
    Awaiter *awt = try(absa->alloc(absa->real, sizeof(Awaiter))).data;
    if (!awt) throw("Awaiter, failed to create, malloc() failed", "Awaiter.Creation.Malloc.Fail", -2);
    awt->conditional_ptr = cnd_ptr;
    awt->self_mtx = try(__mtx_create(mtx_type)).data;

    return opt(awt, sizeof(Awaiter), true);
}


#endif