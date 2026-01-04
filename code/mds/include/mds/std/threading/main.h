#pragma once
#include <mds/genmod.h>
#include "conditions.h"
#include "mdthreads.h"
#include "mutexes.h"
#include "awaiter.h"

typedef struct {
    Module _minfo;

    option (*spawn)(option (*worker)(kvtable args), const Arguments args);
    option (*exit)(void *retval);
    option (*join)(Thread thrd);
    option (*detach)(Thread *thrd);
    option (*destroy)(Thread *thrd);
} std_threads;
std_threads __threading_md;

typedef struct {
    Module _minfo;

    option (*create)(MUTEX_TYPE type);
    option (*lock)(Mutex *mtx);
    option (*trylock)(Mutex *mtx);
    option (*unlock)(Mutex *mtx);
    option (*destroy)(Mutex *mtx);
} std_mutex;
std_mutex __std_mutex_struct;

typedef struct {
    Module _minfo;

    option (*create)();
    option (*wait)(Condt *cnd, Mutex *mtx);
    option (*timedwait)(Condt *cnd, Mutex *mtx, i64 nanos);
    option (*signal)(Condt *cnd);
    option (*broadcast)(Condt *cnd);
    option (*destroy)(Condt *cnd);

    void   (*subscribe)(Condt *cnd);
    void   (*unsubscribe)(Condt *cnd);
    void   (*subcheck)(Condt *cnd);
} std_condt;
std_condt __std_condt_struct;

typedef struct {
    Module _minfo;

    option (*create)(Condt *cnd_ptr, MUTEX_TYPE mtx_type);
    option (*wait)(Awaiter *aw);
    option (*destroy)(Awaiter *aw);
} std_awaiter;
std_awaiter __std_awaiter_struct;

void __mtx_setup(){
    __std_mutex_struct._minfo = mModuleNew("std.mutex");
    __std_mutex_struct.create = __mtx_create;
    __std_mutex_struct.lock = __mtx_lock;
    __std_mutex_struct.trylock = __mtx_trylock;
    __std_mutex_struct.unlock = __mtx_unlock;
    __std_mutex_struct.destroy = __mtx_free;
}

void __cnd_setup(){
    __std_condt_struct._minfo = mModuleNew("std.condt");
    __std_condt_struct.create = __cnd_create;
    __std_condt_struct.wait = __cnd_wait;
    __std_condt_struct.timedwait = __cnd_timedwait;
    __std_condt_struct.signal = __cnd_signal;
    __std_condt_struct.destroy = __cnd_destroy;
    __std_condt_struct.subcheck = __cnd_subcheck;
    __std_condt_struct.subscribe = __cnd_subscribe;
    __std_condt_struct.unsubscribe = __cnd_unsubscribe;
}

void __awaiter_setup(){
    __std_awaiter_struct._minfo = mModuleNew("std.awaiter");
    __std_awaiter_struct.create = __awt_create;
    __std_awaiter_struct.wait = __awt_wait;
    __std_awaiter_struct.destroy = __awt_free;
}

void __thr_setup(){
    __threading_md._minfo = mModuleNew("std.threading");
    __threading_md.spawn = __thr_create;
    __threading_md.exit = __thr_exit;
    __threading_md.join = __thr_join;
    __threading_md.detach = __thr_detach;
    __threading_md.destroy = __thr_destroy;
}

void __multiprocessing_setup(){
    __thr_setup();
    __cnd_setup();
    __mtx_setup();
    __awaiter_setup();
}

#define THRD_INJECT_MOD  {__threading_md._minfo, &__threading_md}
#define MTX_INJECT_MOD   {__std_mutex_struct._minfo, &__std_mutex_struct}
#define CNDT_INJECT_MOD  {__std_condt_struct._minfo, &__std_condt_struct}
#define AWAIT_INJECT_MOD {__std_awaiter_struct._minfo, &__std_awaiter_struct}