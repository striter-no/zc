#pragma once
#include <mds/genmod.h>
#include "async_api.h"
#include "async_polling.h"

typedef struct {
    Module _minfo;

    async_ring (*ring_init)(epoller *ptr);
    option (*ring_firstcallback)(async_ring *ring, option (*callback)(struct epoll_event *ev, epoller *eplr));
    option (*ring_end)(async_ring *ring);
    option (*ring_subscribe)(async_ring *ring, u32 events, option (*callback)(void *dataptr));
    option (*ring_clear)(async_ring *ring);
    option (*ring_iter)(void *ring);

    option (*async)(option (*func)(void*), void* arg, size_t stack_size);
    option (*await)(coro *ptr);
    option (*run)(coro *main_ptr);
} std_async;
std_async __async_struct;

void __async_setup(){
    __async_struct._minfo = mModuleNew("std.async");
    __async_struct.async = __std_async_async;
    __async_struct.await = __std_async_await;
    __async_struct.run = __std_async_run;

    __async_struct.ring_init = __async_ring_init;
    __async_struct.ring_firstcallback = __async_ring_firstcallback;
    __async_struct.ring_end = __async_ring_end;
    __async_struct.ring_subscribe = __async_ring_subscribe;
    __async_struct.ring_clear = __async_ring_clear;
    __async_struct.ring_iter = __async_ring_iter;
}

#define STD_ASYNC_INJECT_MOD {__async_struct._minfo, &__async_struct}