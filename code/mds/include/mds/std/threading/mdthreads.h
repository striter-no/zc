#ifndef MDTHREADS_HEADER

#include <pthread.h>
#include <mds/std/_preincl/base.h>
#include <mds/std/_preincl/defs.h>
#include <mds/std/containers/table.h>
#include <stdint.h>
#include "arguments.h"

typedef struct {
    bool is_joinable;
    pthread_t posix_thread;

    kvtable *args;
    option (*function)(kvtable args);
    option *output;

    AbstractAllocator *absa;
} Thread;

struct __thread_args {
    Thread *_main_pointer;
    option (*oworker)(kvtable args);
    kvtable *args;
    option *output;
};

static void *__thr_worker_interface(void *_args){
    if (!_args) panic("cannot wrap given function to void*(void*), args is NULL");

    var* args = (struct __thread_args*)_args;
    *args->output = args->oworker(*args->args);

    // args->_main_pointer->args = NULL;
    __kvtable_dfclean(args->args, ^(variable *key){
        delvar(key);
    }, ^(variable *val){
        delvar(val);
    });
    __kvtable_free(args->args);
    args->_main_pointer->absa->free(args->_main_pointer->absa->real, args->args);
    args->_main_pointer->absa->free(args->_main_pointer->absa->real, args);
    return NULL;
}

option __thr_create(option (*worker)(kvtable args), Arguments *args);
option __thr_join(Thread thrd);
option __thr_detach(Thread *thrd);
option __thr_exit(void *retval);
option __thr_destroy(Thread *thrd);
#ifdef MDTHREADS_IMPLEMENTATION

option __thr_create(option (*worker)(kvtable args), Arguments *args){
    AbstractAllocator *absa = try(global.get(".absa")).data;
    
    Thread *thrd = try(absa->alloc(absa->real, sizeof(Thread))).data;
    if (!thrd) throw(
        "Failed to create thread, malloc failed",
        "Threading.Create.Malloc.Failed",
        -1
    );
    thrd->absa = absa;

    struct __thread_args *newargs = try(absa->alloc(absa->real, sizeof(struct __thread_args))).data;
    newargs->args = try(absa->alloc(absa->real, sizeof(kvtable))).data;
    *newargs->args = __unwrap_args(*args);
    newargs->output = try(absa->alloc(absa->real, sizeof(option))).data;
    newargs->oworker = worker;
    newargs->_main_pointer = thrd;
    end_args(args);

    if (pthread_create(
        &thrd->posix_thread, 
        NULL, 
        __thr_worker_interface,
        newargs
    ) != 0) {
        try(absa->free(absa->real, newargs));
        throw(
            "Failed to create thread, pthread_create failed",
            "Threading.Create.Pthread.Failed",
            -2
        );
    }

    thrd->is_joinable = true;
    thrd->function = worker;
    thrd->args = newargs->args;
    thrd->output = newargs->output;

    return opt(thrd, sizeof(Thread), true);
}

option __thr_detach(Thread *thrd){
    if (!thrd) throw(
        "Cannot detach thread, thrd ptr is NULL",
        "Threading.Detach.Failed",
        1
    );

    pthread_detach(thrd->posix_thread);
    thrd->is_joinable = false;
    return noerropt;
}

option __thr_join(Thread thrd){
    if (!thrd.is_joinable) throw(
        "Cannot join detached thread, is_joinable is false",
        "Threading.Join.Failed",
        1
    );

    void *ret = NULL;
    pthread_join(thrd.posix_thread, &ret);

    return opt(ret, sizeof(void*), true);
}

option __thr_destroy(Thread *thrd){
    if (!thrd) return noerropt;
    try(thrd->absa->free(thrd->absa->real, thrd->output));
    try(thrd->absa->free(thrd->absa->real, thrd));
    return noerropt;
}

option __thr_exit(void *retval){
    pthread_exit(retval);
    return noerropt;
}

#endif
#endif
#define MDTHREADS_HEADER
